/*******************************************************************************
* c64_vic.c
*
* Commodore 64 VIC II chip source file.
*******************************************************************************/
#include <stdio.h>
#include <65xx.h>

#include "c64_vic.h"

extern U8 c64_IO[U16_MAX];

U16 c64_vicCurrRasterLine = 0;
U16 c64_vicRasterIrqLine;

static U8 irqActive;

/*******************************************************************************
* Init VIC II.
*
* Arguments:
*		-
*
* Returns: -
*******************************************************************************/
void c64_vicInit(void) {
	VIC->D011_screenCtrl1 = 0x1B;
	VIC->D016_screenCtrl2 = 0xC8;
    c64_vicCurrRasterLine = 0;
}

/*******************************************************************************
* Check if VIC II chip has active interrupts.
*
* Arguments:
*		-
*
* Returns: true if interrupt(s) active, otherwise 0
*******************************************************************************/
U8 c64_vicCheckIrq(void) {
	return irqActive;
}

/*******************************************************************************
* Tick VIC chip.
*
* Arguments:
*		advance - ticks to advance.
*
* Returns: -
*******************************************************************************/
void c64_vicTick(U64 tickAdvance) {

	c64_vicCurrRasterLine += 8 * (tickAdvance + 1);

	if (c64_vicCurrRasterLine >= 312) {
		c64_vicCurrRasterLine = c64_vicCurrRasterLine - 312;
	}

	/* Write the most significant bit of raster line. */
	VIC->D011_screenCtrl1 = (
		(VIC->D011_screenCtrl1 & (1 << 7))
	|	(U8)((c64_vicCurrRasterLine & 0x100) >> 1)
	);

	VIC->D012_currRasterLine = ((U8)c64_vicCurrRasterLine & 0xFF);
}

/*******************************************************************************
* Read from peripheral address.
*
* Arguments:
*		pProcessor	- Pointer to 65xx processor struct.
*		address 	- Address to read from.
*
* Returns: Value read from the VIC II.
*******************************************************************************/
U8 c64_vicRead(Processor_65xx * pProcessor, mos65xx_addr address) {
	return pProcessor->memAreas.IO[address];
}

/*******************************************************************************
* Write VIC II register.
*
* Arguments:
*		pProcessor	- Pointer to 65xx processor struct.
*		address 	- Address to write to.
*		value		- Value to be written.
*
* Returns: -
*******************************************************************************/
void c64_vicWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value) {
	pProcessor->memAreas.IO[address] = value;
}
