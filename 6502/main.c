#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "6502.h"
// Non-blocking fgetc()
#include <fcntl.h>

#define US_TO_NS(_usecs)	(_usecs * 1000)
#define MS_TO_NS(_msecs)	(US_TO_NS(_msecs) * 1000)
#define S_TO_NS(_msecs)		(MS_TO_NS(_msecs) * 1000)

#define NS_TO_US(_nsecs)	(_nsecs / 1000)
#define NS_TO_MS(_nsecs)	(NS_TO_US(_nsecs) / 1000)

#define CLOCK_FREQ 1000000
#define SCREEN_W 60
#define SCREEN_H 25
#define REFRESH_RATE_NS MS_TO_NS(20)
#define KB_SCAN_RATE_NS	MS_TO_NS(10)
#define TICK_NS (unsigned int)((double)1 / (double)CLOCK_FREQ * (double)S_TO_NS(1)) //1000

#define TSPEC_NSEC_MAX 999999999

#define PRG_AS_ARGUMENT			0
#define PRG_STORE_TO_MEM		1
#define PRG_CLEAR_MEM			2
#define PRG_WEEKDAY				3
#define PRG_count_impl_opc		4
#define PRG_count_impl_leg_opc	5
#define PRG_ASSEMBLE			6
#define PRG_KEYBOARD			7
#define PRG_BLINK				8
#define PRG_NUMQUERY			9
#define PRG_PRINT				10

#define program PRG_AS_ARGUMENT

#if PRINT_DBG_INFO
#define SUPPRESS_SCREEN(_x)
#else
#define SUPPRESS_SCREEN(_x) _x
#endif

#define TERM_RESET		"\033[m"
#define COLOR_BLK_BLK	"\033[30;40m"
#define COLOR_GRY_BLK	"\033[30;47m"
#define CURSOR_OFF		"\033[?25l"
#define CURSOR_RESET	"\033[?25h"


typedef enum {
	mos6502_BIN,
	mos6502_HEX,
	mos6502_ASM
} mos6502_fileType;

mos6502_addr keyboardAddr = 0xFFF0;
mos6502_addr screenMemAddr = 0xF690;
mos6502_addr screenMemSize = SCREEN_W * SCREEN_H;

void loadFile(U8 * memory, const char * path, mos6502_fileType fileType);

U8 fnMemRead(U16 address);
void fnMemWrite(U16 address, U8 value);

U8 readKeyboard();

void screenClear();
void screenRefresh();

void memDummyWrite8(mos6502_addr address, U8 value) { return; }

U8 * ram;

const U8 legalOpcodes[] = {
	0x00, 0x01, 0x05, 0x06, 0x08, 0x09, 0x0A, 0x0D, 0x0E, 0x10, 0x11, 0x15,
	0x16, 0x18, 0x19, 0x1D, 0x1E, 0x20, 0x21, 0x24, 0x25, 0x26, 0x28, 0x29,
	0x2A, 0x2C, 0x2D, 0x2E, 0x30, 0x31, 0x35, 0x36, 0x38, 0x39, 0x3D, 0x3E,
	0x40, 0x41, 0x45, 0x46, 0x48, 0x49, 0x4A, 0x4C, 0x4D, 0x4E, 0x50, 0x51,
	0x55, 0x56, 0x58, 0x59, 0x5D, 0x5E, 0x60, 0x61, 0x65, 0x66, 0x68, 0x69,
	0x6A, 0x6C, 0x6D, 0x6E, 0x70, 0x71, 0x75, 0x76, 0x78, 0x79, 0x7D, 0x7E,
	0x81, 0x84, 0x85, 0x86, 0x88, 0x8A, 0x8C, 0x8D, 0x8E, 0x90, 0x91, 0x94,
	0x95, 0x96, 0x98, 0x99, 0x9A, 0x9D, 0xA0, 0xA1, 0xA2, 0xA4, 0xA5, 0xA6,
	0xA8, 0xA9, 0xAA, 0xAC, 0xAD, 0xAE, 0xB0, 0xB1, 0xB4, 0xB5, 0xB6, 0xB8,
	0xB9, 0xBA, 0xBC, 0xBD, 0xBE, 0xC0, 0xC1, 0xC4, 0xC5, 0xC6, 0xC8, 0xC9,
	0xCA, 0xCC, 0xCD, 0xCE, 0xD0, 0xD1, 0xD5, 0xD6, 0xD8, 0xD9, 0xDD, 0xDE,
	0xE0, 0xE1, 0xE4, 0xE5, 0xE6, 0xE8, 0xE9, 0xEA, 0xEC, 0xED, 0xEE, 0xF0,
	0xF1, 0xF5, 0xF6, 0xF8, 0xF9, 0xFD, 0xFE
};

/*******************************************************************************
* Handles loading and running the program.
*
* Returns: -
*******************************************************************************/
int main(int argc, char * argv[]) {

	int selected_program;
	U16 opCount = 0;
	U8 memory[MOS6502_MEMSIZE];

#if program == PRG_AS_ARGUMENT
	if (argc == 2 && *argv[1] >= '0' && *argv[1] <= '9') {
		selected_program = *argv[1] - '0';
		if (argv[1][1] != '\0') {
			selected_program *= 10;
			selected_program +=	argv[1][1] - '0';
		}
	} else {
		printf("Invalid program index given as parameter\n\n");
		printf("Valid program indexes:\n");
		printf("\t1 - Store to memory\n");
		printf("\t2 - Clear memory region\n");
		printf("\t3 - Calculate weekday (1-7) from date\n");
		printf("\t4 - Count implemented op codes (all)\n");
		printf("\t5 - Count implemented legal op codes\n");
		printf("\t6 - Run the assembler\n");
		printf("\t7 - Keyboard test, prints out 5 characters entered\n");
		printf("\t8 - Cursor blink\n");
		printf("\t9 - Number query\n");
		printf("\t10 - String print test\n");
		printf("\n");
		exit(1);
	}
#else
	selected_program = program;
#endif



	if (selected_program == PRG_ASSEMBLE) {
		mos6502_assemble("./prg/asm/32bitdiv.6502asm", ram);
		return 0;
	} else if (selected_program == PRG_STORE_TO_MEM) { // write values to memory.
		loadFile(memory, "./prg/bin/memWrite.6502", mos6502_BIN);
	} else if (selected_program == PRG_CLEAR_MEM) { // Clears X amount of memory from memory address (100), Y
		loadFile(memory, "./prg/bin/memClear.6502", mos6502_BIN);
	} else if (selected_program == PRG_WEEKDAY) { // Day of the week. Y = year, X = month, AC = day
		loadFile(memory, "./prg/bin/weekday.6502", mos6502_BIN);
	} else if (selected_program == PRG_count_impl_opc) {
		opCount	= 256;
		for (int ii = 0; ii < opCount; ii++) {
			memory[ii] = ii;
		}
	} else if (selected_program == PRG_count_impl_leg_opc) {
		opCount = sizeof(legalOpcodes);
		for (int ii = 0; ii < opCount; ii++) {
			memory[ii] = legalOpcodes[ii];
		}
	} else if (selected_program == PRG_KEYBOARD) {
		loadFile(memory, "./prg/bin/kbTest.6502", mos6502_BIN);		
	} else if (selected_program == PRG_BLINK) {
		loadFile(memory, "./prg/bin/blink.6502", mos6502_BIN);
	} else if (selected_program == PRG_NUMQUERY) {
		loadFile(memory, "./prg/bin/numQuery.6502", mos6502_BIN);
	} else if (selected_program == PRG_PRINT) {
		loadFile(memory, "./prg/bin/print.6502", mos6502_BIN);
	}

	ram = memory;

	SUPPRESS_SCREEN(screenClear());

	struct timespec startTime, endTime;
	unsigned long totalCycles = 0;
	unsigned long totalOperations = 0;
	U8 retVal = 0;

	mos6502_processor_st processor;

	mos6502_init(&processor, fnMemRead, fnMemWrite);

	if (selected_program == PRG_count_impl_opc || selected_program == PRG_count_impl_leg_opc) {
		U8 count = 0;

		processor.fnMemWrite = memDummyWrite8;

		for (int ii = 0; ii < opCount; ii++) {
			processor.reg.PC = ii;
			retVal = mos6502_handleOp(&processor);

			if (retVal != 0xFF) {
				count++;
			}
		}

		printf("\nCount of implemented ops: %d/%d (%d\%)\n\n", count, opCount, (count * 100) / opCount);

	} else {

		struct timespec runTime, syncTime, refreshTime = {0}, kbScanTime = {0};

		SUPPRESS_SCREEN(fcntl(0, F_SETFL, O_NONBLOCK));
		SUPPRESS_SCREEN(system("/bin/stty raw"));

		clock_gettime(CLOCK_REALTIME, &startTime);
		
		while (retVal != 0xFF) {

			time_t diff = 0;

			clock_gettime(CLOCK_REALTIME, &runTime);

			retVal = mos6502_handleOp(&processor);

			if (retVal == 0xFF) {
				SUPPRESS_SCREEN(screenRefresh());
				printf(TERM_RESET);
				break;
			} else {

				do {

					clock_gettime(CLOCK_REALTIME, &syncTime);

					if (syncTime.tv_nsec > runTime.tv_nsec) {
						diff = syncTime.tv_nsec - runTime.tv_nsec;
					} else {
						diff = TSPEC_NSEC_MAX - (runTime.tv_nsec - syncTime.tv_nsec);
					}

				} while (diff < (retVal * TICK_NS));
			}

		
			if (runTime.tv_nsec > refreshTime.tv_nsec) {
				diff = runTime.tv_nsec - refreshTime.tv_nsec;
			} else {
				diff = TSPEC_NSEC_MAX - (refreshTime.tv_nsec - runTime.tv_nsec);
			}

			if (diff > REFRESH_RATE_NS) {

				refreshTime = runTime;
				SUPPRESS_SCREEN(screenRefresh());
			}

			if (runTime.tv_nsec > kbScanTime.tv_nsec) {
				diff = runTime.tv_nsec - kbScanTime.tv_nsec;
			} else {
				diff = TSPEC_NSEC_MAX - (kbScanTime.tv_nsec - runTime.tv_nsec);
			}

			if (diff > KB_SCAN_RATE_NS) {

				kbScanTime = runTime;
				SUPPRESS_SCREEN(ram[keyboardAddr] = readKeyboard());
			}

			totalOperations++;

			//getc(stdin);
		}
		clock_gettime(CLOCK_REALTIME, &endTime);

		SUPPRESS_SCREEN(system("/bin/stty cooked"));
	}

	printf("%s\033[32;1H%s", CURSOR_RESET, TERM_RESET);


	if (selected_program == PRG_WEEKDAY) {
		printf(
			"weekday for %d.%d.%d is %d (%s)\n\n",
			memory[5],
			memory[3],
			memory[1] + 1900,
			processor.reg.AC,
			processor.reg.AC == 1 ? "MON" :
				processor.reg.AC == 2 ? "TUE" :
					processor.reg.AC == 3 ? "WED" :
						processor.reg.AC == 4 ? "THU" :
							processor.reg.AC == 5 ? "FRI" :
								processor.reg.AC == 6 ? "SAT" :
									processor.reg.AC == 7 ? "SUN" : "ERR!"
		);
	} else if (selected_program == PRG_KEYBOARD) {
		printf("String entered: %s\n\n", &memory[screenMemAddr]);
	}

	time_t secsPassed, nsecsPassed = 0;

	secsPassed = endTime.tv_sec - startTime.tv_sec;

	if (secsPassed != 0 && (endTime.tv_nsec < startTime.tv_nsec)) {
		secsPassed--;
	}

	if (startTime.tv_nsec > endTime.tv_nsec) {
		nsecsPassed = TSPEC_NSEC_MAX - (startTime.tv_nsec - endTime.tv_nsec);
	} else {
		nsecsPassed = endTime.tv_nsec - startTime.tv_nsec;
	}

	printf(
		"Operations performed: %d\n\n Start time: %ds %dms %dns\n   End time: %ds %dms %dns\n\n Difference: %ds %dms %dns\n\nCycle count: %llu\n\n",
		totalOperations,
		0,
		NS_TO_MS(startTime.tv_nsec),
		startTime.tv_nsec - MS_TO_NS(NS_TO_MS(startTime.tv_nsec)),
		endTime.tv_sec - startTime.tv_sec,
		NS_TO_MS(endTime.tv_nsec),
		endTime.tv_nsec - MS_TO_NS(NS_TO_MS(endTime.tv_nsec)),
		secsPassed,
		NS_TO_MS(nsecsPassed),
		nsecsPassed - MS_TO_NS(NS_TO_MS(nsecsPassed)),
		processor.cycleCount
	);
}

/*******************************************************************************
* Loads binary from given path.
*
* Arguments:
*	pMemory - Pointer to a memory allocation.
*	pPath	- Pointer to a file path.
*
*
* Returns: Nothing.
*******************************************************************************/
void loadFile(U8 * pMemory, const char * pPath, mos6502_fileType fileType) {
	FILE * fpPrg;

	fpPrg = fopen(pPath, "rb");

	switch (fileType)
	{
	case mos6502_BIN:
		break;
	case mos6502_HEX:
	case mos6502_ASM:
		printf("Filetype not yet supported...\n");
		exit(1);
	}

	for (int memIdx = 0; memIdx <= 0xFFFF; memIdx++) {
		int tempByte = fgetc(fpPrg);

		if (tempByte != EOF) {
			pMemory[memIdx] = (U8)tempByte;
		} else {
			pMemory[memIdx] = 0;
		}
	}

	fclose(fpPrg);
}

/*******************************************************************************
* Reads one byte from the memory.
*
* Arguments:
*	address - Memory address to read from.
*
* Returns: Byte read from the memory.
*******************************************************************************/
U8 fnMemRead(mos6502_addr address) {
	return ram[address];
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
void fnMemWrite(mos6502_addr address, U8 value) {
	if (address != keyboardAddr) {
		ram[address] = value;
	}
}

/*******************************************************************************
* Read one byte from the stdin without blocking.
*
* Arguments:
*	none
*
* Returns: Byte representing the key pressed on the keyboard.
*******************************************************************************/
U8 readKeyboard() {
	U8 key;
	printf("\033[1;%dH%s", SCREEN_W + 2, COLOR_BLK_BLK);
	
	key = fgetc(stdin);

	//printf("\033[1D \033[1D%s", COLOR_GRY_BLK);
	return key;
}

/*******************************************************************************
* Clears and setups screen.
*
* Arguments:
*	none
*
* Returns: Nothing.
*******************************************************************************/
void screenClear() {
	system("clear");
	printf("%s%s", CURSOR_OFF, COLOR_GRY_BLK);

	for (int memIndex = screenMemAddr + screenMemSize; memIndex >= screenMemAddr; memIndex--) {
		ram[memIndex] = ' ';
	}
}

/*******************************************************************************
* Refreshes screen from the screen memory.
*
* Arguments:
*	none
*
* Returns: Nothing.
*******************************************************************************/
void screenRefresh() {

	U8 * screenMem = &ram[screenMemAddr];
	U8 screenLine[SCREEN_W + 1];

	screenLine[SCREEN_W] = '\0';

	printf("\033[1;1H%s", COLOR_GRY_BLK);

	for (int rowIndex = 0; rowIndex < SCREEN_H; rowIndex++) {
		for (int colIndex = 0; colIndex < SCREEN_W; colIndex++) {
			screenLine[colIndex] = *(screenMem++);
		}
		printf("%s\033[%d;1H%s%s", COLOR_GRY_BLK, rowIndex + 1, screenLine, COLOR_BLK_BLK);
	}
}
