/*******************************************************************************
* c64.c
*
* Commodore 64 emulator.
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
// Non-blocking fgetc()
#include <fcntl.h>

#include <65xx.h>

#include "c64_peripherals.h"

#define TRUE	1U
#define FALSE	0U

#define term_conf()	fcntl(0, F_SETFL, O_NONBLOCK); system("/bin/stty raw")
#define term_deConf() system("/bin/stty cooked");

#define TSPEC_NSEC_MAX 999999999U

#define CHAREN_BIT (1 << 2)

#define C64BASIC_START	0xA000U
#define C64BASIC_END	0xBFFFU

#define C64CHAR_START	0xD000U
#define C64CHAR_END		0xDFFFU

#define C64KERNAL_START	0xE000U
#define C64KERNAL_END	0xFFFFU

#define usToNs(_usecs)	(_usecs * 1000U)
#define msToNs(_msecs)	(usToNs(_msecs) * 1000U)
#define sToNs(_msecs)		(msToNs(_msecs) * 1000U)

#define nsToUs(_nsecs)	(_nsecs / 1000U)
#define nsToMs(_nsecs)	(nsToUs(_nsecs) / 1000U)

#define notSmaller(_value, _min) (_value >= _min)
#define notLarger(_value, _max) (_value <= _max)
#define isInRange(_value, _min, _max) (notSmaller(_value, _min) && notLarger(_value, _max))

#define DATA_DIR_REG c64_RAM[0]
#define PORT_REG c64_RAM[1]

#define loRamEnabled() (!((PORT_REG & 3) == 3)) // Only ROM when 0b...11
#define midRamEnabled() ((PORT_REG ^ 3) == 3)
#define hiRamEnabled() (!(PORT_REG & 2)) // Only ROM when 0b...1x

#define charRomEnabled() (!(PORT_REG & CHAREN_BIT) && !midRamEnabled())
#define ioAreaVisible() ((PORT_REG & CHAREN_BIT) && !midRamEnabled())

#define isRAM(_address) (								\
	isInRange(_address, C64BASIC_START, C64BASIC_END) 	\
	? 	(loRamEnabled() ? TRUE : FALSE) 				\
	:	isInRange(_address, C64CHAR_START, C64CHAR_END) \
		?	(midRamEnabled() ? TRUE : FALSE)			\
		:	notSmaller(_address, C64KERNAL_START)		\
			?	(hiRamEnabled() ? TRUE : FALSE)			\
			:	FALSE									\
)

#define isIO(_address) \
	isInRange(_address, C64CHAR_START, C64CHAR_END) ? ioAreaVisible() : FALSE

#define CLOCK_FREQ 1000000
#define TICK_NS (time_t)((double)1 / (double)CLOCK_FREQ * (double)sToNs(1))
#define KB_SCAN_RATE_NS	msToNs(10)
#define REFRESH_RATE_NS	msToNs(20)

#define BACKSPACE		"\033[D \033[D"

#define KBD_ADDR     0xD010U
#define KBDCR_ADDR   0xD011U
#define DSP_ADDR     0xD012U
#define DSPCR_ADDR   0xD013U

static U8 c64_memRead(Processor_65xx * pProcessor, mos65xx_addr address);
static void c64_memWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value);

static void c64_out(void);
static U8 c64_scanKbd(void);
static void c64_screenConv(U8 * lineIn);

static U8 c64_timePassed(struct timespec * pOldTime, struct timespec * pNewTime, time_t timeToPass);
static U8 c64_readKeyboard(void);

static void c64_tick(U64 advance);

U8 c64_RAM[U16_MAX];
U8 c64_ROM[U16_MAX];
U8 c64_IO[U16_MAX];

enum {
	OP_RAM,
	OP_ROM,
	OP_IO,
};
U8 lastOpMemType = 0;
U8 lastOpType = 0;
mos65xx_addr lastAddr = 0;

extern FILE * tmpLog;

extern U8 CIA1_irqEn;
extern U8 CIA2_irqEn;

pthread_t tickThread;

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

	Memory_areas memAreas;

	addRAMArea(0x0000, 0xFFFF);

	addROMArea(C64BASIC_START, C64BASIC_END); // Basic
	loadFile(c64_ROM, C64BASIC_START, "./c64/roms/bin/c64Basic.rom", mos65xx_BIN);

	addROMArea(C64CHAR_START, C64CHAR_END); // CHAR rom
	loadFile(c64_ROM, C64CHAR_START, "./c64/roms/bin/c64Char.rom", mos65xx_BIN);

	addROMArea(C64KERNAL_START, C64KERNAL_END); // Kernal
	loadFile(c64_ROM, C64KERNAL_START, "./c64/roms/bin/c64Kernal.rom", mos65xx_BIN);

	addIOArea(C64CHAR_START, C64CHAR_END);

	memAreas.RAM = c64_RAM;
	memAreas.ROM = c64_ROM;
	memAreas.IO = c64_IO;
	

	DATA_DIR_REG = 0x2F; // Default = 0b0010 1111
	PORT_REG = 0x07;// Default = 0b0000 0111

	mos65xx_init(
		pProcessor,
		&memAreas,
		c64_memRead,
		c64_memWrite,
		0,
		NULL
	);

	/* Set peripheral default values. */
	vicII_init();
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
	struct timespec kbScanTime = {0};
	struct timespec scrRefresh = {0};
	mos65xx_addr oldPC = 0;

	term_conf();

	for (int ii = 0; ii < 35; ii++) {
		printf("\r\n");
	}

	while (1) {
		oldPC = pProcessor->reg.PC;

		clock_gettime(CLOCK_REALTIME, &runTime);
		
		if (c64_checkPinIRQ()) {
			/* PIN interrupt occured. */
			mos65xx_irqEnter(pProcessor);
		}
		mos65xx_handleOp(pProcessor);

		if (pProcessor->cycles.currentOp == 0) {
			term_deConf();
			printf("\033[m\n\n0x%04X: Invalid opcode!\n\n", oldPC);
			break;
		}

		if (lastOpMemType == OP_RAM) {
			DBG_LOG(pProcessor, fprintf(tmpLog, " | RAM %c($%04X)", lastOpType, lastAddr));
		} else if (lastOpMemType == OP_ROM) {
			DBG_LOG(pProcessor, fprintf(tmpLog, " | ROM %c($%04X)", lastOpType, lastAddr));
		} else if (lastOpMemType == OP_IO) {
			DBG_LOG(pProcessor, fprintf(tmpLog, " | IO  %c($%04X)", lastOpType, lastAddr));
		}

#if 0
		DBG_LOG(pProcessor, fprintf(
			tmpLog,
			" $%02X/$%02X $%02X/$%02X",
			CIA1->DC0D_intCtrlStatusReg,
			CIA1_irqEn,
			CIA2->DD0D_intCtrlStatusReg,
			CIA2_irqEn
		));

		DBG_LOG(pProcessor, fprintf(tmpLog, " TIM_A: %d", (CIA1->DC05_timerAValueMSB << 8) | CIA1->DC04_timerAValueLSB));
#endif

		if (pProcessor->lastOp == 0x40){ /* RTI */
			mos65xx_irqLeave(pProcessor);
		}

		if (pProcessor->reg.PC == oldPC) {

			term_deConf();
			printf("\033[m\n\n0x%04X: Infinite loop detected!\n\n", oldPC);
			break;
		}

		/* Read keyboard every 10 ms. */
		if (c64_timePassed(&kbScanTime, &runTime, KB_SCAN_RATE_NS)) {

			if (c64_scanKbd()) {
				term_deConf();
				break;
			}

			kbScanTime = runTime;
		}

		/* Refresh screen every 100 ms. */
		if (c64_timePassed(&scrRefresh, &runTime, REFRESH_RATE_NS)) {

			scrRefresh = runTime;
			
			c64_out();
		}

		pProcessor->totOperations++;

		/* Sync to 1 Mhz. */
		do {

			clock_gettime(CLOCK_REALTIME, &syncTime);

		} while (!c64_timePassed(&runTime, &syncTime, pProcessor->cycles.currentOp * TICK_NS));

		
		c64_tick(pProcessor->cycles.currentOp);
	}

	FILE * temp;

	temp = fopen("./endScreen.txt", "w+");

	for (int ii = 0x400; ii <= 0x7E7; ii ++) {
		fputc(c64_RAM[ii], temp);
	}

	fclose(temp);
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

	if (!isROMAddress(address) || isRAM(address)) {
		memVal = pProcessor->memAreas.RAM[address];
		lastOpMemType = OP_RAM;
	} else if (isIO(address)) {
		memVal = c64_ioRead(pProcessor, address);
		lastOpMemType = OP_IO;
	} else {
		memVal = pProcessor->memAreas.ROM[address];
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

	if (isIO(address)){
		c64_ioWrite(pProcessor, address, value);
		lastOpMemType = OP_IO;
	} else {
		pProcessor->memAreas.RAM[address] = value;
		lastOpMemType = OP_RAM;
	}

	lastAddr = address;
}

/*******************************************************************************
* Handles terminal style output.
*
* Arguments:
*	none
*
* Returns: Nothing.
*******************************************************************************/
static void c64_out(void) {
	U8 line[41];

	line[40] = 0;

	printf("\033[1;1H");

	printf("\033[104m                                                \033[m \r\n");
	printf("\033[104m                                                \033[m \r\n");

	for (int scrIndex = 0x400; scrIndex <= 0x7E7; scrIndex+=40) {

		memcpy(line, &c64_RAM[scrIndex], 40);

		c64_screenConv(line);

		printf("\033[104m    \033[44m\033[94m%s\033[104m    \033[m \r\n", line);
	}

	printf("\033[104m                                                \033[m \r\n");
	printf("\033[104m                                                \033[m \r\n");
}

/*******************************************************************************
* Convert C64 screen codes to ascii.
*
* Arguments:
*	lineIn		- User input character
*
* Returns: Converted input.
*******************************************************************************/
static void c64_screenConv(U8 * lineIn) {

	for (int bufIndex = 0; bufIndex < 40; bufIndex++) {
		if (lineIn[bufIndex] == 0) {
			lineIn[bufIndex] = '@';
		
		} else if (lineIn[bufIndex] < 27) {
			/* Alphabet */
			lineIn[bufIndex] = lineIn[bufIndex] + 0x40;

		} else if (lineIn[bufIndex] < 32) {
			/* [, �, ], Up arrow, Left arrow */
			lineIn[bufIndex] = lineIn[bufIndex] + 0x40;
		
		} else if (lineIn[bufIndex] < 64) {
			/* These symbols and numeric characters match */
		
		} else if (lineIn[bufIndex] == 0xA0) {
			/* Rest TODO */
			lineIn[bufIndex] = '#';

		} else {
			/* Rest TODO */
			lineIn[bufIndex] = '^';
		}
	}
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

		memset(c64_keyBuffer, 0, sizeof(c64_keyBuffer));

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

		c64_keyBuffer[keyColumn] |= keyRow;
	} else {
		memset(c64_keyBuffer, 0, sizeof(c64_keyBuffer));
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

/*******************************************************************************
* 1 Mhz tick.
*
* Arguments:
*		tickCounter
*
* Returns: -
*******************************************************************************/
static void c64_tick(U64 advance) {

	vicII_tick(advance);
	CIA_tick(advance);

}
