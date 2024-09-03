#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
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
#define TICK_NS (time_t)((double)1 / (double)CLOCK_FREQ * (double)S_TO_NS(1)) //1000

#define TSPEC_NSEC_MAX 999999999

#define DRAW_TERM(_x) if (!debugLogging) _x

#define TERM_RESET		"\033[m"
#define COLOR_BLK_BLK	"\033[30;40m"
#define COLOR_GRY_BLK	"\033[30;47m"
#define CURSOR_OFF		"\033[?25l"
#define CURSOR_RESET	"\033[?25h"


enum {
	PRG_ASSEMBLE,
	PRG_STORE_TO_MEM,
	PRG_CLEAR_MEM,
	PRG_WEEKDAY,
	PRG_KEYBOARD,
	PRG_BLINK,
	PRG_NUMQUERY,
	PRG_PRINT,
	PRG_WOZMON
} programList;

typedef enum {
	mos6502_BIN,
	mos6502_HEX,
	mos6502_ASM
} mos6502_fileType;

mos6502_addr keyboardAddr = 0xFDFF;
mos6502_addr screenMemAddr = 0xF800;
mos6502_addr screenMemSize = SCREEN_W * SCREEN_H;

void loadFile(U8 * pMemory, U16 offset, const char * pPath, mos6502_fileType fileType);

U8 fnMemRead(U16 address);
void fnMemWrite(U16 address, U8 value);

U8 readKeyboard();

void screenClear();
void screenRefresh();

void programSelector(int selected_program, U8 * pMemory);
void programEnd(int selected_program, U8 * pMemory, mos6502_processor_st * pProcessor);

U8 * ram;

/*******************************************************************************
* Handles loading and running the program.
*
* Returns: -
*******************************************************************************/
int main(int argc, char * argv[]) {

	int selected_program;
	U16 opCount = 0;
	U8 memory[MOS6502_MEMSIZE];
	U8 debugLogging = 0;
	time_t slowDown = 0;

	if (
		argc >= 2
	&&	*argv[1] >= '0' && *argv[1] <= '9'
	) {
		selected_program = *argv[1] - '0';
		if (argv[1][1] != '\0') {
			selected_program *= 10;
			selected_program +=	argv[1][1] - '0';
		}

		if (argc > 2) {
			for (int currArg = 2; currArg < argc; currArg++) {

				if (argv[currArg][1] == 'd') {
					debugLogging = 1;
				} else if (argv[currArg][1] == 's') {
					if (currArg + 1 == argc
					||	argv[currArg + 1][0] == '-'
					) {
						slowDown = 500;
					} else {
						currArg++;

						for (int strIdx = 0; argv[currArg][strIdx] != 0; strIdx++) {
							if (!isdigit(argv[currArg][strIdx])) {
								printf("\n\nInvalid argument: %s %s\n\n", argv[currArg - 1], argv[currArg]);
								exit(0);
							}
							slowDown *= 10;
							slowDown += argv[currArg][strIdx] - '0';
						}
					}
				} else {
					printf("\n\nInvalid argument: %s\n\n", argv[currArg]);
					exit(0);
				}

			}
		}

	} else {

		printf("Invalid program index given as parameter\n\n");
		printf("Valid program indexes:\n");
		printf("\t%d - Store to memory\n", PRG_STORE_TO_MEM);
		printf("\t%d - Clear memory region\n", PRG_CLEAR_MEM);
		printf("\t%d - Calculate weekday (1-7) from date\n", PRG_WEEKDAY);
		printf("\t%d - Run the assembler\n", PRG_ASSEMBLE);
		printf("\t%d - Keyboard test, prints out 5 characters entered\n", PRG_KEYBOARD);
		printf("\t%d - Cursor blink\n", PRG_BLINK);
		printf("\t%d - Number query\n", PRG_NUMQUERY);
		printf("\t%d - String print test\n", PRG_PRINT);
		printf("\t%d - Wozmon\n", PRG_WOZMON);
		printf("\n\n");
		printf("Options:\n\t-d : Print operations.\n\t-s : Slow down\n");
		exit(1);

	}

	ram = memory;

	DRAW_TERM(screenClear());

	struct timespec startTime, endTime;
	unsigned long totalCycles = 0;
	unsigned long totalOperations = 0;
	U8 retVal = 0;

	mos6502_processor_st processor;

	struct timespec runTime, syncTime, refreshTime = {0}, kbScanTime = {0};

	programSelector(selected_program, memory);

	mos6502_init(&processor, fnMemRead, fnMemWrite, debugLogging);

	DRAW_TERM(fcntl(0, F_SETFL, O_NONBLOCK));
	DRAW_TERM(system("/bin/stty raw"));

	clock_gettime(CLOCK_REALTIME, &startTime);
	
	while (retVal != 0xFF) {

		time_t diff = 0;

		clock_gettime(CLOCK_REALTIME, &runTime);

		retVal = mos6502_handleOp(&processor);

		if (retVal == 0xFF) {
			DRAW_TERM(screenRefresh());
			printf(TERM_RESET);
			break;
		} else {

			/* Sync to 1 Mhz. */
			do {

				clock_gettime(CLOCK_REALTIME, &syncTime);

				if (syncTime.tv_nsec > runTime.tv_nsec) {
					diff = syncTime.tv_nsec - runTime.tv_nsec;
				} else {
					diff = TSPEC_NSEC_MAX - (runTime.tv_nsec - syncTime.tv_nsec);
				}

				diff += (syncTime.tv_sec - runTime.tv_sec) * (TSPEC_NSEC_MAX + 1);

			} while (diff < ((slowDown ? slowDown * 1000 : retVal) * TICK_NS));
		}

		/* Sync screen refreshing to 50 hz. */
		if (runTime.tv_nsec > refreshTime.tv_nsec) {
			diff = runTime.tv_nsec - refreshTime.tv_nsec;
		} else {
			diff = TSPEC_NSEC_MAX - (refreshTime.tv_nsec - runTime.tv_nsec);
		}

		if (diff > REFRESH_RATE_NS) {

			refreshTime = runTime;
			DRAW_TERM(screenRefresh());
		}

		/* Read keyboard every 10 ms. */
		if (runTime.tv_nsec > kbScanTime.tv_nsec) {
			diff = runTime.tv_nsec - kbScanTime.tv_nsec;
		} else {
			diff = TSPEC_NSEC_MAX - (kbScanTime.tv_nsec - runTime.tv_nsec);
		}

		if (diff > KB_SCAN_RATE_NS) {

			kbScanTime = runTime;
			DRAW_TERM(ram[keyboardAddr] = readKeyboard());
		}

		totalOperations++;

		//getc(stdin);
	}
	clock_gettime(CLOCK_REALTIME, &endTime);

	DRAW_TERM(system("/bin/stty cooked"));

	printf("%s\033[32;1H%s", CURSOR_RESET, TERM_RESET);

	programEnd(selected_program, memory, &processor);

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
void loadFile(U8 * pMemory, U16 offset, const char * pPath, mos6502_fileType fileType) {
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

	for (int memIdx = offset; memIdx <= 0xFFFF; memIdx++) {
		int tempByte = fgetc(fpPrg);

		if (tempByte != EOF) {
			pMemory[memIdx] = (U8)tempByte;
		} else {
			break;
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

/*******************************************************************************
* Loads selected program to the memory.
*
* Arguments:
*	selected_program	- Index number of the selected program.
*	pMemory				- Pointer to the memory buffer.
*
*******************************************************************************/
void programSelector(int selected_program, U8 * pMemory) {

	if (selected_program == PRG_ASSEMBLE) {
		mos6502_assemble("./prg/asm/32bitdiv.6502asm", ram);
		exit(0);
	} else if (selected_program == PRG_STORE_TO_MEM) { // write values to memory.
		loadFile(pMemory, 0xFE00, "./prg/bin/memWrite.6502", mos6502_BIN);
	} else if (selected_program == PRG_CLEAR_MEM) { // Clears X amount of memory from memory address (100), Y
		loadFile(pMemory, 0xFE00, "./prg/bin/memClear.6502", mos6502_BIN);
	} else if (selected_program == PRG_WEEKDAY) { // Day of the week. Y = year, X = month, AC = day
		loadFile(pMemory, 0xFE00, "./prg/bin/weekday.6502", mos6502_BIN);
	} else if (selected_program == PRG_KEYBOARD) {
		loadFile(pMemory, 0xFE00, "./prg/bin/kbTest.6502", mos6502_BIN);		
	} else if (selected_program == PRG_BLINK) {
		loadFile(pMemory, 0xFE00, "./prg/bin/blink.6502", mos6502_BIN);
	} else if (selected_program == PRG_NUMQUERY) {
		loadFile(pMemory, 0xFE00, "./prg/bin/numQuery.6502", mos6502_BIN);
	} else if (selected_program == PRG_PRINT) {
		loadFile(pMemory, 0xFE00, "./prg/bin/print.6502", mos6502_BIN);
	} else if (selected_program == PRG_WOZMON) {
		loadFile(pMemory, 0xFF00, "./prg/bin/wozmon.6502", mos6502_BIN);
	}
}

/*******************************************************************************
* Handles the after program operations if needed.
*
* Arguments:
*	selected_program	- Index number of the selected program.
*	pMemory				- Pointer to the memory buffer.
*	pProcessor			- Pointer to the processor struct.
*
*******************************************************************************/
void programEnd(int selected_program, U8 * pMemory, mos6502_processor_st * pProcessor) {

	if (selected_program == PRG_WEEKDAY) {
		printf(
			"weekday for %d.%d.%d is %d (%s)\n\n",
			pMemory[5],
			pMemory[3],
			pMemory[1] + 1900,
			pProcessor->reg.AC,
			pProcessor->reg.AC == 1 ? "MON" :
				pProcessor->reg.AC == 2 ? "TUE" :
					pProcessor->reg.AC == 3 ? "WED" :
						pProcessor->reg.AC == 4 ? "THU" :
							pProcessor->reg.AC == 5 ? "FRI" :
								pProcessor->reg.AC == 6 ? "SAT" :
									pProcessor->reg.AC == 7 ? "SUN" : "ERR!"
		);
	} else if (selected_program == PRG_KEYBOARD) {
		printf("String entered: %s\n\n", &pMemory[screenMemAddr]);
	}

}