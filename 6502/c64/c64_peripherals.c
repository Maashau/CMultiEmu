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
void c64_periphInit(Processor_65xx * pProcessor) {
	c64_vicInit(pProcessor);
	c64_ciaInit(pProcessor);
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
void c64_periphTick(Processor_65xx * pProcessor, U8 advance) {

	c64_vicTick(pProcessor, advance);
	c64_ciaTick(pProcessor, advance);

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
#include <stdlib.h>
U8 c64_periphRead(Processor_65xx * pProcessor, mos65xx_addr address) {
	U8 retVal;

	if (address >= 0xD000 && address <= 0xD03F) {
		retVal = c64_vicRead(pProcessor, address);
	} else if (address >= 0xDC00 && address <= 0xDD0F) {
		retVal = c64_ciaRead(pProcessor, address);
	} else if (address >= 0xD800 && address <= 0xDBE7) {
		retVal = pProcessor->mem.IO[address];
	}
#if 0
	else {
		system("/bin/stty cooked");
		printf("Address ($%04X) read not mapped!\r\n", address);
		exit(0);
	}
#else

#endif

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
		pProcessor->mem.IO[address] = value;
	}

#if 0
	else {
		system("/bin/stty cooked");
		printf("Address ($%04X) write not mapped!\r\n", address);
		exit(0);
	}
#else

#endif
}
