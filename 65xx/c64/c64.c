/*******************************************************************************
* c64.c
*
* Commodore 64 emulator main source file.
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include <string.h>
#include <time.h>
// Non-blocking fgetc()
#include <fcntl.h>
#include <SDL2/SDL.h>

#include "../65xx.h"

#include "c64.h"
#include "c64_peripherals.h"
#include "c64_vic.h"
#include "c64_io.h"

#define TRUE	1U
#define FALSE	0U

#define term_conf()	fcntl(0, F_SETFL, O_NONBLOCK); system("/bin/stty raw")
#define term_deConf() system("/bin/stty cooked");

#define TSPEC_NSEC_MAX 999999999U

#define CHAREN_BIT (1 << 2)

#define usToNs(_usecs)	(_usecs * 1000U)
#define msToNs(_msecs)	(usToNs(_msecs) * 1000U)
#define sToNs(_msecs)		(msToNs(_msecs) * 1000U)

#define nsToUs(_nsecs)	(_nsecs / 1000U)
#define nsToMs(_nsecs)	(nsToUs(_nsecs) / 1000U)

#define notSmaller(_value, _min) (_value >= _min)
#define notLarger(_value, _max) (_value <= _max)
#define isInRange(_value, _min, _max) (notSmaller(_value, _min) && notLarger(_value, _max))

#define DATA_DIR_REG 	((mos65xx_addr)0)
#define PORT_REG 		((mos65xx_addr)1)

#define loRamEnabled(_pProcessor) (!((_pProcessor->pMem->RAM[PORT_REG] & 3) == 3)) // Only ROM when 0b...11
#define midRamEnabled(_pProcessor) ((_pProcessor->pMem->RAM[PORT_REG] ^ 3) == 3)
#define hiRamEnabled(_pProcessor) (!(_pProcessor->pMem->RAM[PORT_REG] & 2)) // Only ROM when 0b...1x

#define charRomEnabled(_pProcessor) (!(_pProcessor->pMem->RAM[PORT_REG] & CHAREN_BIT) && !midRamEnabled(_pProcessor))
#define ioAreaVisible(_pProcessor) ((_pProcessor->pMem->RAM[PORT_REG] & CHAREN_BIT) && !midRamEnabled(_pProcessor))


#define isIO(_pProcessor, _address) \
	isInRange(_address, C64CHAR_START, C64CHAR_END) ? ioAreaVisible(_pProcessor) : FALSE

#define CLOCK_FREQ 1000000
#define TICK_NS (time_t)((double)1 / (double)CLOCK_FREQ * (double)sToNs(1))
#define KB_SCAN_RATE_NS	msToNs(10)
#define REFRESH_RATE_NS	msToNs(20)

#define BACKSPACE		"\033[D \033[D"

static U8 c64_memRead(Processor_65xx * pProcessor, mos65xx_addr address);
static void c64_memWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value);

static U8 c64_timePassed(struct timespec * pOldTime, struct timespec * pNewTime, time_t timeToPass);

static U8 c64_isRam(Processor_65xx * pProcessor, mos65xx_addr address);

void addIOArea(mos65xx_addr startAddr, mos65xx_addr endAddr);
U8 isIOAddress(mos65xx_addr address);

U8 IOAddrRangeCount;
address_range IOAddrList[0xFF];

enum {
	OP_RAM,
	OP_ROM,
	OP_IO,
};
U8 lastOpMemType = 0;
U8 lastOpType = 0;
mos65xx_addr lastAddr = 0;

extern FILE * tmpLog;

/*******************************************************************************
* Initialize Commodore 64 emulator.
*
* Arguments:
*	pProcessor - Pointer to 65xx processor struct.
*
* Returns: Pointer to C64 struct.
*******************************************************************************/
void * c64_init(Processor_65xx * pProcessor) {

	Memory_areas *	pMem		= calloc(1, sizeof(Memory_areas));
	C64_t *			pC64		= calloc(1, sizeof(C64_t));
	RGB_t *			pScreenBuf	= malloc(C64_BORDER_W * C64_BORDER_H * sizeof(RGB_t));

	pMem->RAM = malloc(U16_MAX);
	pMem->ROM = malloc(U16_MAX);
	pMem->IO = malloc(U16_MAX);

	addRAMArea(0x0000, 0xFFFF);

	addROMArea(C64BASIC_START, C64BASIC_END); // Basic
	loadFile(pMem->ROM, C64BASIC_START, "./c64/roms/c64Basic.rom", mos65xx_BIN);

	addROMArea(C64CHAR_START, C64CHAR_END); // CHAR rom
	loadFile(pMem->ROM, C64CHAR_START, "./c64/roms/c64Char.rom", mos65xx_BIN);

	addROMArea(C64KERNAL_START, C64KERNAL_END); // Kernal
	loadFile(pMem->ROM, C64KERNAL_START, "./c64/roms/c64Kernal.rom", mos65xx_BIN);

	addIOArea(C64CHAR_START, C64CHAR_END);
	

	//addROMArea(C64CARTLO_START, C64CARTLO_END); // Cartridge rom
	//loadFile(pMem->ROM, C64CARTLO_START, "./c64/roms/CommodoreDiagRev410.bin", mos65xx_BIN);


	pMem->RAM[DATA_DIR_REG] = 0x2F; // Default = 0b0010 1111
	pMem->RAM[PORT_REG] = 0x07;// Default = 0b0000 0111

	mos65xx_init(
		pProcessor,
		pMem,
		c64_memRead,
		c64_memWrite,
		0,
		pC64
	);

	pC64->pProcessor = pProcessor;
	pC64->pScreenBuf = pScreenBuf;

	/* Set peripheral default values. */
	c64_periphInit(pC64);

	return pC64;
}

/*******************************************************************************
* Run Commodore 64 emulator.
*
* Arguments:
*	pProcessor - Pointer to 65xx processor struct.
*
* Returns: -
*******************************************************************************/
void c64_run(void * pDevice) {

	C64_t * pC64 = (C64_t *)pDevice;
	struct timespec runTime, syncTime;
	mos65xx_addr oldPC = 0;

	SDL_Event nextEvent;

	pC64->pProcessor->event = &nextEvent;

	pC64->pSdlSema = SDL_CreateSemaphore(0);
	SDL_CreateThread(sdlThread, "DrawFn", pC64);

	while (1) {

		oldPC = pC64->pProcessor->reg.PC;

#ifndef _WIN32
		clock_gettime(CLOCK_REALTIME, &runTime);
#endif	

		if (pC64->irqActive) {
			/* PIN interrupt occured. */
			mos65xx_irqEnter(pC64->pProcessor);
		}
		mos65xx_handleOp(pC64->pProcessor);

		if (pC64->pProcessor->cycles.currentOp == 0) {
			term_deConf();
			printf("\033[m\n\n0x%04X: Invalid opcode!\n\n", oldPC);
			break;
		}

		if (pC64->pProcessor->lastOp == 0x40){ /* RTI */
			mos65xx_irqLeave(pC64->pProcessor);
		}
		
		if (pC64->pProcessor->reg.PC == oldPC) {

			term_deConf();
			printf("\033[m\n\n0x%04X: Infinite loop detected!\n\n", oldPC);
			break;
		}

#ifndef _WIN32
		/* Sync to 1 Mhz. */
		do {

			clock_gettime(CLOCK_REALTIME, &syncTime);

		} while (!c64_timePassed(&runTime, &syncTime, pC64->pProcessor->cycles.currentOp * TICK_NS));
#endif
		
		c64_periphTick(pC64);

		pC64->pProcessor->totOperations++;

		if (nextEvent.type == SDL_QUIT) {
			break;
		}
		nextEvent.type = 0;
	}
}

/*******************************************************************************
* Reads one byte from the memory.
*
* Arguments:
*	address - Memory address to read from.
*
* Returns: Byte read from the memory.
*******************************************************************************/
static U8 c64_memRead(Processor_65xx * pProcessor, mos65xx_addr address) {

	extern U8 opLogging;
	U8 memVal = 0;
	U8 isRomAddr = isROMAddress(address);

	if (!opLogging) {
		lastOpType = 'R';
	}

	if (!isRomAddr || c64_isRam(pProcessor, address)) {
		memVal = pProcessor->pMem->RAM[address];
		lastOpMemType = OP_RAM;
	} else if (isIO(pProcessor, address)) {
		memVal = c64_periphRead((C64_t *)pProcessor->pUtil, address);
		lastOpMemType = OP_IO;
	} else {
		memVal = pProcessor->pMem->ROM[address];
		lastOpMemType = OP_ROM;
	}

	lastAddr = address;

	return memVal;
}

/*******************************************************************************
* Writes one byte to the memory.
*
* Arguments:
*	address - Memory address to write to.
*	value	- Byte to be written to the memory.
*
* Returns: Nothing.
*******************************************************************************/
static void c64_memWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value) {
	
	extern U8 opLogging;
	
	if (!opLogging) {
		lastOpType = 'W';
	}

	if (isIO(pProcessor, address)){
		c64_periphWrite(pProcessor, address, value);
		lastOpMemType = OP_IO;
	} else {
		pProcessor->pMem->RAM[address] = value;
		lastOpMemType = OP_RAM;
	}

	lastAddr = address;
}

/*******************************************************************************
* Checks if given amount of time has been passed between two timespecs.
*
* Arguments:
*	pOldTime		- Starting time of the comparison.
*	pNewTime		- Test time.
*	timeToPass		- Amount of nano seconds to pass.
*
* Returns: 1 if passed, otherwise 0.
*******************************************************************************/
static U8 c64_timePassed(struct timespec * pOldTime, struct timespec * pNewTime, time_t timeToPass) {

	U8 passed = 0;
	time_t diff;

	/* Calculate elapsed ns. */
	if (pOldTime->tv_nsec > pNewTime->tv_nsec) {
		diff = TSPEC_NSEC_MAX - (pOldTime->tv_nsec - pNewTime->tv_nsec);
	} else {
		diff = pNewTime->tv_nsec - pOldTime->tv_nsec;
	}

	/* Add elapsed seconds to the diff. */
	diff += (pNewTime->tv_sec - pOldTime->tv_sec) * (TSPEC_NSEC_MAX + 1);

	if (diff > timeToPass) {
		passed = 1;
	}

	return passed;
}

static U8 c64_isRam(Processor_65xx * pProcessor, mos65xx_addr address){

	if (isInRange(address, C64BASIC_START, C64BASIC_END)
	&&	loRamEnabled(pProcessor)
	) {
		return TRUE;

	} else if (
		isInRange(address, C64CHAR_START, C64CHAR_END)
	&&	midRamEnabled(pProcessor)
	) {
		return TRUE;

	} else if (
		notSmaller(address, C64KERNAL_START)
	&&	hiRamEnabled(pProcessor)
	) {
		return TRUE;
	}

	return FALSE;
}

/*******************************************************************************
* Mark address range as IO.
*
* Arguments:
*	startAddr	- Starting address of the RAM region
*	endAddr		- Ending address of the RAM region
*
* Returns: -
*******************************************************************************/
void addIOArea(mos65xx_addr startAddr, mos65xx_addr endAddr) {
	if (startAddr > endAddr) {
		printf("\n\nInvalid IO area range (0x%04x - 0x%04x)...\n\n", startAddr, endAddr);
		exit(1);
	}

	IOAddrList[IOAddrRangeCount].startAddr = startAddr;
	IOAddrList[IOAddrRangeCount].endAddr = endAddr;

	IOAddrRangeCount++;
}

/*******************************************************************************
* Check if given address is on IO region.
*
* Arguments:
*	address		- Address to be checked
*
* Returns: 1 if IO, otherwise 0
*******************************************************************************/
U8 isIOAddress(mos65xx_addr address) {
	for (int areaIdx = 0; areaIdx < IOAddrRangeCount; areaIdx++) {
		if (IOAddrList[areaIdx].startAddr <= address
		&&	IOAddrList[areaIdx].endAddr >= address
		) {
			return 1;
		}
	}

	return 0;
}
