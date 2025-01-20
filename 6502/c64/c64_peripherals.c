/*******************************************************************************
* c64_peripherals.c
*
* Commodore 64 peripheral source file.
*******************************************************************************/
#include <stdio.h>
#include <unistd.h>

#include <65xx.h>

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
void c64_periphInit(void) {
	c64_vicInit();
	c64_ciaInit();
}

/*******************************************************************************
* Check if any peripherals have active interrupts.
*
* Arguments:
*		-
*
* Returns: true if interrupt(s) active, otherwise 0
*******************************************************************************/
U8 c64_periphCheckIrq(void) {
	U8 irqActive = 0;
	irqActive |= c64_ciaCheckIrq();
	irqActive |= c64_vicCheckIrq();

	return irqActive;
}

/*******************************************************************************
* Tick peripherals.
*
* Arguments:
*		advance - ticks to advance.
*
* Returns: -
*******************************************************************************/
void c64_periphTick(U64 advance) {

	c64_vicTick(advance);
	c64_ciaTick(advance);

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
U8 c64_periphRead(Processor_65xx * pProcessor, mos65xx_addr address) {
	U8 retVal;

	if (address >= 0xD000 && address <= 0xD03F) {
		retVal = c64_vicRead(pProcessor, address);
	} else if (address >= 0xDC00 && address <= 0xDD0F) {
		retVal = c64_ciaRead(pProcessor, address);
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
	}
}
