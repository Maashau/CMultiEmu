
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <65xx.h>
#include "c64_peripherals.h"

enum {
	CIA1_TIM_A,
	CIA1_TIM_B,
	CIA2_TIM_A,
	CIA2_TIM_B,
};

typedef struct {
	U16 *	DEF;
	U8 *	LSB;
	U8 *	MSB;
	U8 *	CTRL;
} CIA_Timer;


extern U8 c64_IO[U16_MAX];
extern U8 c64_keyBuffer[8];

U16 vicII_currRasterLine = 0;

U16 VICII_rasterIrqLine;

U16 CIA1_timAStartVal;
U16 CIA1_timBStartVal;
U16 CIA2_timAStartVal;
U16 CIA2_timBStartVal;

U8 CIA1_irqEn;
U8 CIA2_irqEn;

CIA_Timer timers[] = {
	{&CIA1_timAStartVal, &CIA1->DC04_timerAValueLSB, &CIA1->DC05_timerAValueMSB, &CIA1->DC0E_timerACtrl},
	{&CIA1_timBStartVal, &CIA1->DC06_timerBValueLSB, &CIA1->DC07_timerBValueMSB, &CIA1->DC0F_timerBCtrl},
	{&CIA2_timAStartVal, &CIA2->DD04_timerAValueLSB, &CIA2->DD05_timerAValueMSB, &CIA2->DD0E_timerACtrl},
	{&CIA2_timBStartVal, &CIA2->DD06_timerBValueLSB, &CIA2->DD07_timerBValueMSB, &CIA2->DD0F_timerBCtrl}
};

static U8 pinIrqActive = 0;

static void CIA__timAdvance(U8 timerID, U64 advance);

U8 c64_checkPinIRQ(void) {
	return pinIrqActive;
}

U8 c64_ioRead(Processor_65xx * pProcessor, mos65xx_addr address) {
	U8 retVal;

	if (address == 0xDC01) {
		U8 tempCol = ~CIA1->DC00_portAKbdColJStick2;

		CIA1->DC01_portBKbdRowJStick1 = 0xFF;

		if (tempCol != 0) {
			for (U8 currCol = 0; tempCol != 0; currCol++) {

				if (tempCol & 1
				&&	c64_keyBuffer[currCol] != 0
				) {
					U8 rowMask = ~CIA1->DC01_portBKbdRowJStick1;

					rowMask |= c64_keyBuffer[currCol];

					CIA1->DC01_portBKbdRowJStick1 = ~rowMask;
				}

				tempCol = tempCol >> 1;
			}
		}
	}

	retVal = pProcessor->memAreas.IO[address];
	
	if (
		address == 0xDC0D
	||	address == 0xDD0D
	) {
		pProcessor->memAreas.IO[address] = 0;
	}

	return retVal;
}

void c64_ioWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value) {
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
		pProcessor->memAreas.IO[address] = value;
	}
}

void vicII_init(void) {
	VIC_II->D011_screenCtrl1 = 0x1B;
	VIC_II->D016_screenCtrl2 = 0xC8;
    vicII_currRasterLine = 0;
}

void vicII_tick(U64 tickAdvance) {

	vicII_currRasterLine += 8 * (tickAdvance + 1);

	if (vicII_currRasterLine >= 312) {
		vicII_currRasterLine = vicII_currRasterLine - 312;
	}

	VIC_II->D011_screenCtrl1 = (VIC_II->D011_screenCtrl1 & (1 << 7)) | (U8)((vicII_currRasterLine & 0x100) >> 1);
	VIC_II->D012_currRasterLine = ((U8)vicII_currRasterLine & 0xFF);
}

void CIA_tick(U64 tickAdvance) {

	CIA__timAdvance(CIA1_TIM_A, tickAdvance);
	CIA__timAdvance(CIA1_TIM_B, tickAdvance);
	CIA__timAdvance(CIA2_TIM_A, tickAdvance);
	CIA__timAdvance(CIA2_TIM_B, tickAdvance);

	if ((CIA1->DC0D_intCtrlStatusReg & CIA1_irqEn & CIA_IRQ_EN_MASK)
	||	(CIA2->DD0D_intCtrlStatusReg & CIA2_irqEn & CIA_IRQ_EN_MASK)
	) {
		pinIrqActive = 1;
	} else {
		pinIrqActive = 0;
	}
}

static void CIA__timAdvance(U8 timerID, U64 advance) {

	if (*timers[timerID].CTRL & TIM_RELOAD) {
		
		*timers[timerID].MSB = *timers[timerID].DEF >> 8;
		*timers[timerID].LSB = *timers[timerID].DEF & 0xFF;

		*timers[timerID].CTRL &= ~TIM_RELOAD;
	}

	if (*timers[timerID].CTRL & TIM_START) {
		/* Timer is running */
		U8 uflow = 0;
		U16 timerValue, oldValue;

		timerValue = *timers[timerID].MSB << 8;
		timerValue |= *timers[timerID].LSB & 0xFF;
		
		oldValue = timerValue;
		timerValue -= (U16)advance;

		if (*timers[timerID].DEF == 0) {
			timerValue = 0;
		}

		if (timerValue > oldValue) {
			uflow = 1;

			if ((*timers[timerID].CTRL & TIM_STOP_UFLOW) == 0) {
				/* Reload. */
				timerValue = *timers[timerID].DEF - (U16_MAX - timerValue);
			} else {
				/* Stop the timer */
				*timers[timerID].CTRL &= 0xFE;
				timerValue = 0;
			}
		}

		*timers[timerID].MSB = timerValue >> 8;
		*timers[timerID].LSB = timerValue & 0xFF;

		if (uflow) {
			if (timerID == CIA1_TIM_A) {
				CIA1->DC0D_intCtrlStatusReg |= CIA_IRQ_CS_TIM_A;
				if (*timers[timerID].CTRL & TIM_PORTB_UFLOW && uflow) {
					// TODO: portA bit 6 ON
				}
			}
			if (timerID == CIA1_TIM_B) {
				CIA1->DC0D_intCtrlStatusReg |= CIA_IRQ_CS_TIM_B;
				if (*timers[timerID].CTRL & TIM_PORTB_UFLOW && uflow) {
					// TODO: portA bit 7 ON
				}
			}
			if (timerID == CIA2_TIM_A) {
				CIA2->DD0D_intCtrlStatusReg |= CIA_IRQ_CS_TIM_A;
				if (*timers[timerID].CTRL & TIM_PORTB_UFLOW && uflow) {
					// TODO: portB bit 6 ON
				}
			}
			if (timerID == CIA2_TIM_B) {
				CIA2->DD0D_intCtrlStatusReg |= CIA_IRQ_CS_TIM_B;
				if (*timers[timerID].CTRL & TIM_PORTB_UFLOW && uflow) {
					// TODO: portB bit 7 ON
				}
			}
		}
	}
}
