/*******************************************************************************
* c64.c
*
* Commodore 64 emulator main source file.
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
// Non-blocking fgetc()
#include <fcntl.h>
#include <SDL2/SDL.h>

#include <65xx.h>

#include "c64.h"
#include "c64_peripherals.h"
#include "c64_vic.h"

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

#define loRamEnabled(_pProcessor) (!((_pProcessor->mem.RAM[PORT_REG] & 3) == 3)) // Only ROM when 0b...11
#define midRamEnabled(_pProcessor) ((_pProcessor->mem.RAM[PORT_REG] ^ 3) == 3)
#define hiRamEnabled(_pProcessor) (!(_pProcessor->mem.RAM[PORT_REG] & 2)) // Only ROM when 0b...1x

#define charRomEnabled(_pProcessor) (!(_pProcessor->mem.RAM[PORT_REG] & CHAREN_BIT) && !midRamEnabled(_pProcessor))
#define ioAreaVisible(_pProcessor) ((_pProcessor->mem.RAM[PORT_REG] & CHAREN_BIT) && !midRamEnabled(_pProcessor))

#define isRAM(_pProcessor, _address) (						\
	isInRange(_address, C64BASIC_START, C64BASIC_END) 		\
	? 	(loRamEnabled(_pProcessor) ? TRUE : FALSE) 			\
	:	isInRange(_address, C64CHAR_START, C64CHAR_END) 	\
		?	(midRamEnabled(_pProcessor) ? TRUE : FALSE)		\
		:	notSmaller(_address, C64KERNAL_START)			\
			?	(hiRamEnabled(_pProcessor) ? TRUE : FALSE)		\
			:	FALSE										\
)

#define isIO(_pProcessor, _address) \
	isInRange(_address, C64CHAR_START, C64CHAR_END) ? ioAreaVisible(_pProcessor) : FALSE

#define CLOCK_FREQ 1000000
#define TICK_NS (time_t)((double)1 / (double)CLOCK_FREQ * (double)sToNs(1))
#define KB_SCAN_RATE_NS	msToNs(10)
#define REFRESH_RATE_NS	msToNs(20)

#define BACKSPACE		"\033[D \033[D"

static U8 c64_memRead(Processor_65xx * pProcessor, mos65xx_addr address);
static void c64_memWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value);

static U8 c64_scanKbd(void);

static U8 c64_timePassed(struct timespec * pOldTime, struct timespec * pNewTime, time_t timeToPass);
static U8 c64_readKeyboard(void);

U8 clearKeys = 0;

enum {
	OP_RAM,
	OP_ROM,
	OP_IO,
};
U8 lastOpMemType = 0;
U8 lastOpType = 0;
mos65xx_addr lastAddr = 0;

extern FILE * tmpLog;

U8 c64_keyBuffer[8];

#include "c64_keyMap.c"

/*******************************************************************************
* Initialize Commodore 64 emulator.
*
* Arguments:
*	pProcessor - Pointer to 65xx processor struct.
*
* Returns: -
*******************************************************************************/
void c64_init(Processor_65xx * pProcessor) {

	Memory_areas mem;

	mem.RAM = malloc(U16_MAX);
	mem.ROM = malloc(U16_MAX);
	mem.IO = malloc(U16_MAX);

	addRAMArea(0x0000, 0xFFFF);

	addROMArea(C64BASIC_START, C64BASIC_END); // Basic
	loadFile(mem.ROM, C64BASIC_START, "./c64/roms/bin/c64Basic.rom", mos65xx_BIN);

	addROMArea(C64CHAR_START, C64CHAR_END); // CHAR rom
	loadFile(mem.ROM, C64CHAR_START, "./c64/roms/bin/c64Char.rom", mos65xx_BIN);

	addROMArea(C64KERNAL_START, C64KERNAL_END); // Kernal
	loadFile(mem.ROM, C64KERNAL_START, "./c64/roms/bin/c64Kernal.rom", mos65xx_BIN);

	addIOArea(C64CHAR_START, C64CHAR_END);
	

	mem.RAM[DATA_DIR_REG] = 0x2F; // Default = 0b0010 1111
	mem.RAM[PORT_REG] = 0x07;// Default = 0b0000 0111

	mos65xx_init(
		pProcessor,
		&mem,
		c64_memRead,
		c64_memWrite,
		2,
		NULL
	);

	/* Set peripheral default values. */
	c64_periphInit(pProcessor);
}

/*******************************************************************************
* Run Commodore 64 emulator.
*
* Arguments:
*	pProcessor - Pointer to 65xx processor struct.
*
* Returns: -
*******************************************************************************/
void c64_run(Processor_65xx * pProcessor) {

	struct timespec runTime, syncTime;
	U32 kbScan = 0;
	mos65xx_addr oldPC = 0;

	SDL_Event nextEvent;

	pProcessor->event = &nextEvent;

#if 1
	term_conf();
#endif

	while (1) {

		oldPC = pProcessor->reg.PC;
#if 1
		clock_gettime(CLOCK_REALTIME, &runTime);
#endif	
		if (c64_periphCheckIrq()) {
			/* PIN interrupt occured. */
			mos65xx_irqEnter(pProcessor);
		}
		mos65xx_handleOp(pProcessor);

		if (pProcessor->cycles.currentOp == 0) {
			term_deConf();
			printf("\033[m\n\n0x%04X: Invalid opcode!\n\n", oldPC);
			break;
		}
#if 1
		if (lastOpMemType == OP_RAM) {
			DBG_LOG(pProcessor, fprintf(tmpLog, " | RAM %c($%04X)", lastOpType, lastAddr));
		} else if (lastOpMemType == OP_ROM) {
			DBG_LOG(pProcessor, fprintf(tmpLog, " | ROM %c($%04X)", lastOpType, lastAddr));
		} else if (lastOpMemType == OP_IO) {
			DBG_LOG(pProcessor, fprintf(tmpLog, " | IO  %c($%04X)", lastOpType, lastAddr));
		}
#endif
		if (pProcessor->lastOp == 0x40){ /* RTI */
			mos65xx_irqLeave(pProcessor);
		}
#if 1
		if (pProcessor->reg.PC == oldPC) {

			term_deConf();
			printf("\033[m\n\n0x%04X: Infinite loop detected!\n\n", oldPC);
			break;
		}
#if 1
		kbScan += pProcessor->cycles.currentOp;
		if (kbScan >= 10000) {
			if (c64_scanKbd()) {
				term_deConf();
				break;
			}
			kbScan = 0;
		}
#endif

		/* Sync to 1 Mhz. */
		do {

			clock_gettime(CLOCK_REALTIME, &syncTime);

		} while (!c64_timePassed(&runTime, &syncTime, pProcessor->cycles.currentOp * TICK_NS));
#endif
		
		c64_periphTick(pProcessor, pProcessor->cycles.currentOp);

		pProcessor->totOperations++;

		if (nextEvent.type == SDL_QUIT) {
			break;
		}
		nextEvent.type = 0;
	}
	term_deConf();
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

	if (!opLogging) {
		lastOpType = 'R';
	}

	if (!isROMAddress(address) || isRAM(pProcessor, address)) {
		memVal = pProcessor->mem.RAM[address];
		lastOpMemType = OP_RAM;
	} else if (isIO(pProcessor, address)) {
		memVal = c64_periphRead(pProcessor, address);
		lastOpMemType = OP_IO;
	} else {
		memVal = pProcessor->mem.ROM[address];
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
		pProcessor->mem.RAM[address] = value;
		lastOpMemType = OP_RAM;
	}

	lastAddr = address;
}

/*******************************************************************************
* Read and filter keyboard input.
*
* Arguments:
*	pProcessor	- 65xx processor struct.
*
* Returns: 1 if esc was pressed, otherwise 0
*******************************************************************************/
static U8 c64_scanKbd(void) {

	static U8 pressedKey = 0;

	pressedKey = c64_readKeyboard();
	
	if (clearKeys) {
		memset(c64_keyBuffer, 0, sizeof(c64_keyBuffer));
		
		clearKeys = 0;
	}

	if (pressedKey == 0x1B) { // ESC detected.
		printf(BACKSPACE);
		pressedKey = c64_readKeyboard();

		if (pressedKey == 0xFF || pressedKey == 0x1B) {  // ESC key pressed, quit.

			term_deConf();

			printf("\n\nESC key hit.\n\n");

			return 1;
		}
	} else if (pressedKey != 0xFF) {

		U8 keyColumn;
		U8 keyRow;
		U16 convKey = c64_keyMap[pressedKey];

		if ((convKey & C64_KB_LSH) == C64_KB_LSH) {
			c64_keyBuffer[1] = C64_KB_LSH & 0xFF;

			if (convKey & ~C64_KB_LSH & 0xFF00) {
				convKey &= ~(C64_KB_LSH & 0xFF00); 
			}
			if (convKey & ~C64_KB_LSH & 0xFF) {
				convKey &= ~(C64_KB_LSH & 0xFF); 
			}
		}

		keyRow = convKey & 0xFF;

		convKey = convKey >> 8;

		for (
			keyColumn = 0;
			(convKey & 1) == 0 && convKey != 0;
			keyColumn++
		) {
			convKey = convKey >> 1;
		}

		c64_keyBuffer[keyColumn] = keyRow;
	}

	return 0;
}

/*******************************************************************************
* Read one byte from the stdin without blocking.
*
* Arguments:
*	none
*
* Returns: Byte representing the key pressed on the keyboard.
*******************************************************************************/
static U8 c64_readKeyboard(void) {
	U8 key;

	key = fgetc(stdin);
/*
	if (key != 0xFF) {
		if (key == '\r' || key == 0x7F) {
			printf("\b \b");
		}
		printf("\b \b");
	}
*/
	return key;
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
