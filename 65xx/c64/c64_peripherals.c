/*******************************************************************************
* c64_peripherals.c
*
* Commodore 64 peripheral source file.
*******************************************************************************/
#include <stdio.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#include "../65xx.h"

#include "c64.h"
#include "c64_peripherals.h"
#include "c64_vic.h"
#include "c64_cia.h"


/*******************************************************************************
* Functions
*******************************************************************************/
/*******************************************************************************
* Init C64 peripherals.
*
* Arguments:
*		-
*
* Returns: -
*******************************************************************************/
void c64_periphInit(C64_t * pC64) {
	c64_vicInit(pC64);
	c64_ciaInit(pC64);
}

/*******************************************************************************
* Tick peripherals.
*
* Arguments:
*		advance - ticks to advance.
*
* Returns: -
*******************************************************************************/
void c64_periphTick(C64_t * pC64) {

	c64_vicTick(pC64);
	c64_ciaTick(pC64);

}

/*******************************************************************************
* Read from peripheral address.
*
* Arguments:
*		pProcessor	- Pointer to 65xx processor struct.
*		address 	- Address to read from.
*
* Returns: Value read from the peripheral.
*******************************************************************************/
U8 c64_periphRead(C64_t * pC64, mos65xx_addr address) {

	Processor_65xx * pProcessor = pC64->pProcessor;
	U8 retVal = 0;

	if (address >= 0xD000 && address <= 0xD03F) {
		retVal = c64_vicRead(pProcessor, address);
	} else if (address >= 0xDC00 && address <= 0xDD0F) {
		retVal = c64_ciaRead(pC64, address);
	}
	else if (address >= 0xD800 && address <= 0xDBE7) {
		retVal = pProcessor->pMem->IO[address];
	}


	return retVal;
}

/*******************************************************************************
* Write peripheral register.
*
* Arguments:
*		pProcessor	- Pointer to 65xx processor struct.
*		address 	- Address to write to.
*		value		- Value to be written.
*
* Returns: -
*******************************************************************************/
void c64_periphWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value) {

	if (address >= 0xD000 && address <= 0xD03F) {
		c64_vicWrite(pProcessor, address, value);
	} else if (address >= 0xDC00 && address <= 0xDD0F) {
		c64_ciaWrite(pProcessor, address, value);
	} else if (address >= 0xD800 && address <= 0xDBE7) {
		pProcessor->pMem->IO[address] = value;
	}
}
