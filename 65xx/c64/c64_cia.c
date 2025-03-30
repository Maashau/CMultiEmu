/*******************************************************************************
* c64_cia.c
*
* Commodore 64 CIA 1 & 2 chips source file.
*******************************************************************************/
#include <stdio.h>
#include <assert.h>

#include "../65xx.h"
#include "c64.h"
#include "c64_cia.h"
#include "c64_peripherals.h"

#include "c64_keyMap.c"

enum {
	CIA1_TIM_A,
	CIA1_TIM_B,
	CIA2_TIM_A,
	CIA2_TIM_B,
};

typedef struct {
	U8 *			DEFLSB;
	U8 *			DEFMSB;
	U8 *			CURRLSB;
	U8 *			CURRMSB;
	U8 *			CTRL;
} CIA_Timer;

U8 CIA1_read[CIA1_COUNT];
U8 CIA1_write[CIA1_COUNT];

U8 CIA2_read[CIA2_COUNT];
U8 CIA2_write[CIA2_COUNT];

static void CIA__timAdvance(C64_t * pC64, U8 timerID);

static CIA_Timer timers[] = {
	{&CIA1_write[DC04_timerAValueLSB], &CIA1_write[DC05_timerAValueMSB], &CIA1_read[DC04_timerAValueLSB], &CIA1_read[DC05_timerAValueMSB], &CIA1_write[DC0E_timerACtrl]},
	{&CIA1_write[DC06_timerBValueLSB], &CIA1_write[DC07_timerBValueMSB], &CIA1_read[DC06_timerBValueLSB], &CIA1_read[DC07_timerBValueMSB], &CIA1_write[DC0F_timerBCtrl]},
	{&CIA2_write[DD04_timerAValueLSB], &CIA2_write[DD05_timerAValueMSB], &CIA2_read[DD04_timerAValueLSB], &CIA2_read[DD05_timerAValueMSB], &CIA2_write[DD0E_timerACtrl]},
	{&CIA2_write[DD06_timerBValueLSB], &CIA2_write[DD07_timerBValueMSB], &CIA2_read[DD06_timerBValueLSB], &CIA2_read[DD07_timerBValueMSB], &CIA2_write[DD0F_timerBCtrl]}
};

static inline U8 AND_IS(U16 base, U16 and, U16 mask) {
	U8 retVal = ((base & mask) == and);
	return retVal;
}

/*******************************************************************************
* Init CIA 1 and 2.
*
* Arguments:
*		-
*
* Returns: -
*******************************************************************************/
void c64_ciaInit(C64_t * pC64) {
	pC64 = pC64;
}

/*******************************************************************************
* Tick CIA 1 and 2.
*
* Arguments:
*		advance - ticks to advance.
*
* Returns: -
*******************************************************************************/
void c64_ciaTick(C64_t * pC64) {

	CIA__timAdvance(pC64, CIA1_TIM_A);
	CIA__timAdvance(pC64, CIA1_TIM_B);
	CIA__timAdvance(pC64, CIA2_TIM_A);
	CIA__timAdvance(pC64, CIA2_TIM_B);

	if ((CIA1_read[DC0D_intCtrlStatusReg] & CIA1_write[DC0D_intCtrlStatusReg] & CIA_IRQ_EN_MASK)
	||	(CIA2_read[DD0D_intCtrlStatusReg] & CIA2_write[DD0D_intCtrlStatusReg] & CIA_IRQ_EN_MASK)
	) {
		pC64->irqActive |= IRQ_CIA;
	} else {
		pC64->irqActive &= ~IRQ_CIA;
	}
}

/*******************************************************************************
* Read from CIA address.
*
* Arguments:
*		pProcessor	- Pointer to 65xx processor struct.
*		address 	- Address to read from.
*
* Returns: Value read from the CIA 1/2.
*******************************************************************************/
U8 c64_ciaRead(C64_t * pC64, mos65xx_addr address) {

    U8 retVal = 0xFF;
	U8 subAddr = address & 0xFF;

	if (AND_IS(address, 0xDC00, 0xFF00)) {
		/* CIA1 register reads. */

		if (AND_IS(address, 0x01, 0xFF)) {

			U8 tempCol = ~CIA1_read[DC00_portAKbdColJStick2];

			CIA1_read[DC01_portBKbdRowJStick1] = 0xFF;

			for (U16 currCharacter = 0; currCharacter <= 0xFF; currCharacter++) {
				if (pC64->keyBuffer[currCharacter]
					&& ((c64_keyMap[currCharacter] >> 8) & tempCol)
				) {
					U8 rowFilter = ~(U8)(c64_keyMap[currCharacter] & 0xFF);
					CIA1_read[DC01_portBKbdRowJStick1] &= rowFilter;
				}
			}

			retVal = CIA1_read[subAddr];

		} else if (AND_IS(address, 0x0D, 0xFF)) {
			retVal = CIA1_read[subAddr];
			CIA1_read[subAddr] = 0;

		} else {
			retVal = CIA1_read[subAddr];
		}

	} else {
		/* CIA2 register reads. */

		if (AND_IS(address, 0x0D, 0xFF)) {
			retVal = CIA1_read[subAddr];
			CIA1_read[subAddr] = 0;
		} else {
			retVal = CIA2_read[subAddr];
		}
	}

    return retVal;
}

/*******************************************************************************
* Write CIA register.
*
* Arguments:
*		pProcessor	- Pointer to 65xx processor struct.
*		address 	- Address to write to.
*		value		- Value to be written.
*
* Returns: -
*******************************************************************************/
void c64_ciaWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value) {
	pProcessor = pProcessor;

	U8 subAddr = address & 0xFF;
	U8 * readReg = CIA1_read;
	U8 * writeReg = CIA1_write;

 	if (AND_IS(address, 0xDC00, 0xFF00)) {
		switch (subAddr) {
			case DC00_portAKbdColJStick2:
			case DC01_portBKbdRowJStick1:
			case DC02_portADataDir:
			case DC03_portBDataDir:
				readReg[subAddr] = value;
				writeReg[subAddr] = value;
				break;

			case DC04_timerAValueLSB:
			case DC05_timerAValueMSB:
			case DC06_timerBValueLSB:
			case DC07_timerBValueMSB:
				writeReg[subAddr] = value;
				break;

			case DC08_timeOfDayTenths:
			case DC09_timeOfDaySecs:
			case DC0A_timeOfDayMins:
			case DC0B_timeOfDayHours:
			case DC0C_serialShiftReg:
				readReg[subAddr] = value;
				writeReg[subAddr] = value;
				break;

			case DC0D_intCtrlStatusReg:
				writeReg[subAddr] = value & 0x80
					? writeReg[subAddr] | (value & 0x7F)
					: (writeReg[subAddr] & ~value) & 0x7F;
				readReg[subAddr] &= ~value;
				break;

			case DC0E_timerACtrl:
			case DC0F_timerBCtrl:
				readReg[subAddr] = value;
				writeReg[subAddr] = value;
				break;

			default:
				assert(0);
		}

	} else if (AND_IS(address, 0xDD00, 0xFF00)) {
		readReg = CIA2_read;
		writeReg = CIA2_write;

		switch (subAddr) {
			case DD00_portASerial:
				readReg[subAddr] = value;
				writeReg[subAddr] = value;
				break;
			case DD01_portBRS232:
			case DD02_portADataDir:
			case DD03_portBDataDir:
				readReg[subAddr] = value;
				writeReg[subAddr] = value;
				break;

			case DD04_timerAValueLSB:
			case DD05_timerAValueMSB:
			case DD06_timerBValueLSB:
			case DD07_timerBValueMSB:
				writeReg[subAddr] = value;
				break;

			case DD08_timeOfDayTenths:
			case DD09_timeOfDaySecs:
			case DD0A_timeOfDayMins:
			case DD0B_timeOfDayHours:
			case DD0C_serialShiftReg:
				readReg[subAddr] = value;
				writeReg[subAddr] = value;
				break;
			case DD0D_intCtrlStatusReg:
				writeReg[subAddr] = value & 0x80
					? writeReg[subAddr] | (value & 0x7F)
					: (writeReg[subAddr] & ~value) & 0x7F;
				readReg[subAddr] &= ~value;
				break;

			case DD0E_timerACtrl:
			case DD0F_timerBCtrl:
				readReg[subAddr] = value;
				writeReg[subAddr] = value;
				break;

			default:
				assert(0);
		}
	} else {
		assert(0);
	}
}

/*******************************************************************************
* Advance CIA timer.
*
* Arguments:
*		timerID - Timer to advance.
*		advance - Count of ticks subtracted from timer.
*
* Returns: -
*******************************************************************************/
static void CIA__timAdvance(C64_t * pC64, U8 timerID) {

    /* Timer is set to reload. */
	if (*timers[timerID].CTRL & TIM_RELOAD) {

		*timers[timerID].CURRMSB = *timers[timerID].DEFMSB;
		*timers[timerID].CURRLSB = *timers[timerID].DEFLSB;

		*timers[timerID].CTRL &= ~TIM_RELOAD;
	}

    /* Timer is running */
	if (*timers[timerID].CTRL & TIM_START) {
		U8 uflow = 0;
		U16 timerValue, oldValue;

		timerValue = *timers[timerID].CURRMSB << 8;
		timerValue |= *timers[timerID].CURRLSB;
		
		oldValue = timerValue;
		timerValue -= (U16)pC64->pProcessor->cycles.currentOp;

		if (*timers[timerID].DEFMSB == 0 && *timers[timerID].DEFLSB == 0) {
			timerValue = 0;
		}

		if (timerValue > oldValue) {
			uflow = 1;

            /* Reload on underflow or stop the timer. */
			if ((*timers[timerID].CTRL & TIM_STOP_UFLOW) == 0) {
				/* Set new timer value. */
				U16 timerDefault = (*timers[timerID].DEFMSB << 8) | *timers[timerID].DEFLSB;
				timerValue = timerDefault - (U16_MAX - timerValue);

			} else {
				*timers[timerID].CTRL &= 0xFE;
				timerValue = 0;
			}
		}

		*timers[timerID].CURRMSB = timerValue >> 8;
		*timers[timerID].CURRLSB = timerValue & 0xFF;

        /* Underflow occured. */
		if (uflow) {

			if (timerID == CIA1_TIM_A) {

				if (CIA1_write[DC0D_intCtrlStatusReg] & CIA_IRQ_CS_TIM_A) {
					CIA1_read[DC0D_intCtrlStatusReg] |= CIA_IRQ_CS_TIM_A;
				}

				if ((*timers[timerID].CTRL & TIM_PORTB_UFLOW) && uflow) {
					// TODO: portA bit 6 ON
				}
			}
			if (timerID == CIA1_TIM_B) {

				if (CIA1_write[DC0D_intCtrlStatusReg] & CIA_IRQ_CS_TIM_B) {
					CIA1_read[DC0D_intCtrlStatusReg] |= CIA_IRQ_CS_TIM_B;
				}

				if ((*timers[timerID].CTRL & TIM_PORTB_UFLOW) && uflow) {
					// TODO: portA bit 7 ON
				}
			}
			if (timerID == CIA2_TIM_A) {

				if (CIA2_write[DC0D_intCtrlStatusReg] & CIA_IRQ_CS_TIM_A) {
					CIA2_read[DD0D_intCtrlStatusReg] |= CIA_IRQ_CS_TIM_A;
				}

				if ((*timers[timerID].CTRL & TIM_PORTB_UFLOW) && uflow) {
					// TODO: portB bit 6 ON
				}
			}
			if (timerID == CIA2_TIM_B) {

				if (CIA2_write[DC0D_intCtrlStatusReg] & CIA_IRQ_CS_TIM_B) {
					CIA2_read[DD0D_intCtrlStatusReg] |= CIA_IRQ_CS_TIM_B;
				}

				if ((*timers[timerID].CTRL & TIM_PORTB_UFLOW) && uflow) {
					// TODO: portB bit 7 ON
				}
			}
		}
	}
}
