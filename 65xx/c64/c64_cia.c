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
	U16 *			DEF;
	mos65xx_addr	LSB;
	mos65xx_addr	MSB;
	mos65xx_addr	CTRL;
} CIA_Timer;


static void CIA__timAdvance(C64_t * pC64, U8 timerID);

extern U8 clearKeys;

static U16 CIA1_timAStartVal;
static U16 CIA1_timBStartVal;
static U16 CIA2_timAStartVal;
static U16 CIA2_timBStartVal;

static U8 CIA1_irqEn;
static U8 CIA2_irqEn;

static CIA_Timer timers[] = {
	{&CIA1_timAStartVal, DC04_timerAValueLSB, DC05_timerAValueMSB, DC0E_timerACtrl},
	{&CIA1_timBStartVal, DC06_timerBValueLSB, DC07_timerBValueMSB, DC0F_timerBCtrl},
	{&CIA2_timAStartVal, DD04_timerAValueLSB, DD05_timerAValueMSB, DD0E_timerACtrl},
	{&CIA2_timBStartVal, DD06_timerBValueLSB, DD07_timerBValueMSB, DD0F_timerBCtrl}
};

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

	if ((pC64->pProcessor->pMem->IO[DC0D_intCtrlStatusReg] & CIA1_irqEn & CIA_IRQ_EN_MASK)
	||	(pC64->pProcessor->pMem->IO[DD0D_intCtrlStatusReg] & CIA2_irqEn & CIA_IRQ_EN_MASK)
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

	Processor_65xx * pProcessor = pC64->pProcessor;
    U8 retVal;

	if (address == 0xDC01) {
		U8 tempCol = ~pProcessor->pMem->IO[DC00_portAKbdColJStick2];

		pProcessor->pMem->IO[DC01_portBKbdRowJStick1] = 0xFF;

		for (U16 currCharacter = 0; currCharacter <= 0xFF; currCharacter++) {
			if (pC64->keyBuffer[currCharacter]
			&&	(c64_keyMap[currCharacter] >> 8) & tempCol
			) {
				U8 rowFilter = ~(U8)(c64_keyMap[currCharacter] & 0xFF);
				pProcessor->pMem->IO[DC01_portBKbdRowJStick1] &= rowFilter;
			}
		}
	}

	retVal = pProcessor->pMem->IO[address];
	
	if (
		address == 0xDC0D
	||	address == 0xDD0D
	) {
		pProcessor->pMem->IO[address] = 0;
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
	if (address == 0xDC04) {
		CIA1_timAStartVal = (CIA1_timAStartVal & 0xFF00) | value;

	} else if (address == 0xDC05) {
		CIA1_timAStartVal = (CIA1_timAStartVal & 0xFF) | (value << 8);

	} else if (address == 0xDC06) {
		CIA1_timBStartVal = (CIA1_timBStartVal & 0xFF) | (value << 8);

	} else if (address == 0xDC07) {
		CIA1_timBStartVal = (CIA1_timBStartVal & 0xFF) | (value << 8);

	} else if (address == 0xDC0D) {
		CIA1_irqEn = value & 0x80
		?	CIA1_irqEn | (value & 0x7F)
		:	(CIA1_irqEn & ~value) & 0x7F;


	} else if (address == 0xDD04) {
		CIA2_timAStartVal = (CIA2_timAStartVal & 0xFF00) | value;

	} else if (address == 0xDD05) {
		CIA2_timAStartVal = (CIA2_timAStartVal & 0xFF) | (value << 8);

	} else if (address == 0xDD06) {
		CIA2_timBStartVal = (CIA2_timBStartVal & 0xFF00) | value;

	} else if (address == 0xDD07) {
		CIA2_timBStartVal = (CIA2_timBStartVal & 0xFF) | (value << 8);

	} else if (address == 0xDD0D) {
		CIA2_irqEn = value & 0x80
		?	CIA2_irqEn | (value & 0x7F)
		:	(CIA2_irqEn & ~value) & 0x7F;
	} else {
		
		//printf("CIA: $%04X = $%02X\r\n", address, value);
		pProcessor->pMem->IO[address] = value;
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
	if (pC64->pProcessor->pMem->IO[timers[timerID].CTRL] & TIM_RELOAD) {

		pC64->pProcessor->pMem->IO[timers[timerID].MSB] = *timers[timerID].DEF >> 8;
		pC64->pProcessor->pMem->IO[timers[timerID].LSB] = *timers[timerID].DEF & 0xFF;

		pC64->pProcessor->pMem->IO[timers[timerID].CTRL] &= ~TIM_RELOAD;
	}

    /* Timer is running */
	if (pC64->pProcessor->pMem->IO[timers[timerID].CTRL] & TIM_START) {
		U8 uflow = 0;
		U16 timerValue, oldValue;

		timerValue = pC64->pProcessor->pMem->IO[timers[timerID].MSB] << 8;
		timerValue |= pC64->pProcessor->pMem->IO[timers[timerID].LSB] & 0xFF;
		
		oldValue = timerValue;
		timerValue -= (U16)pC64->pProcessor->cycles.currentOp;

		if (*timers[timerID].DEF == 0) {
			timerValue = 0;
		}

		if (timerValue > oldValue) {
			uflow = 1;

            /* Reload on underflow or stop the timer. */
			if ((pC64->pProcessor->pMem->IO[timers[timerID].CTRL] & TIM_STOP_UFLOW) == 0) {
				timerValue = *timers[timerID].DEF - (U16_MAX - timerValue);

			} else {
				pC64->pProcessor->pMem->IO[timers[timerID].CTRL] &= 0xFE;
				timerValue = 0;
			}
		}

        /* Set new timer value. */
		pC64->pProcessor->pMem->IO[timers[timerID].MSB] = timerValue >> 8;
		pC64->pProcessor->pMem->IO[timers[timerID].LSB] = timerValue & 0xFF;

        /* On underflow, trigger interrupt. */
		if (uflow) {
            pC64->pProcessor->pMem->IO[DC0D_intCtrlStatusReg] |= CIA_IRQ_CS_TIM_A;

			if (timerID == CIA1_TIM_A
            &&  pC64->pProcessor->pMem->IO[timers[timerID].CTRL] & TIM_PORTB_UFLOW && uflow
            ) {
					// TODO: portA bit 6 ON
			}
			if (timerID == CIA1_TIM_B
            &&  pC64->pProcessor->pMem->IO[timers[timerID].CTRL] & TIM_PORTB_UFLOW && uflow
            ) {
					// TODO: portA bit 7 ON
			}
			if (timerID == CIA2_TIM_A
            &&  pC64->pProcessor->pMem->IO[timers[timerID].CTRL] & TIM_PORTB_UFLOW && uflow
            ) {
					// TODO: portB bit 6 ON
			}
			if (timerID == CIA2_TIM_B
            &&  pC64->pProcessor->pMem->IO[timers[timerID].CTRL] & TIM_PORTB_UFLOW && uflow
            ) {
					// TODO: portB bit 7 ON
			}
		}
	}
}
