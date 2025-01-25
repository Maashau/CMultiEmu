#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
// Non-blocking fgetc()
#include <fcntl.h>


#include "65xx.h"


#include "apple_i/apple_i.h"
#include "c64/c64.h"


#define usToNs(_usecs)	(_usecs * 1000)
#define msToNs(_msecs)	(usToNs(_msecs) * 1000)
#define sToNs(_msecs)		(msToNs(_msecs) * 1000)

#define nsToUs(_nsecs)	(_nsecs / 1000)
#define nsToMs(_nsecs)	(nsToUs(_nsecs) / 1000)

#define CLOCK_FREQ 1000000
#define SCREEN_W 60
#define SCREEN_H 25
#define REFRESH_RATE_NS msToNs(20)
#define KB_SCAN_RATE_NS	msToNs(10)
#define TICK_NS (time_t)((double)1 / (double)CLOCK_FREQ * (double)sToNs(1))

#define TSPEC_NSEC_MAX 999999999

#define DRAW_TERM(_x) if (!(pCliArguments->debugLogging & 1)) _x

#define TERM_RESET		"\033[m"
#define COLOR_BLK_BLK	"\033[30;40m"
#define COLOR_GRY_BLK	"\033[30;47m"
#define CURSOR_OFF		"\033[?25l"
#define CURSOR_RESET	"\033[?25h"
#define BACKSPACE		"\033[D \033[D"

#define term_conf()	fcntl(0, F_SETFL, O_NONBLOCK); system("/bin/stty raw")
#define term_deConf() system("/bin/stty cooked"); printf("%s%s", CURSOR_RESET, TERM_RESET)

typedef enum {
	PRG_ASSEMBLE,
	PRG_STORE_TO_MEM,
	PRG_CLEAR_MEM,
	PRG_WEEKDAY,
	PRG_KEYBOARD,
	PRG_BLINK,
	PRG_NUMQUERY,
	PRG_PRINT,
	PRG_APPLE_I,
	PRG_C64,
	PRG_OPCODETEST,
	PRG_NONE
} SelectedProgram;

typedef struct {
	SelectedProgram	selectedProgram;
	U8				debugLogging;
	time_t			slowDown;
	U8				fasterSlowDown;
	U8				noSpeedLimit;
	U8				logIO;
} Parameters;

mos65xx_addr keyboardAddr = 0xFDFF;
mos65xx_addr screenMemAddr = 0xF800;
mos65xx_addr screenMemSize = SCREEN_W * SCREEN_H;

Parameters * handleCLIArguments(int argc, char * argv[]);
void printHelp(void);

U8 fnMemRead(Processor_65xx * pProcessor, mos65xx_addr address);
void fnMemWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value);

U8 readKeyboard(void);

void screenClear(Processor_65xx * pProcessor);
void screenRefresh(U8 * pMemory, unsigned int freq);

void programSelector(Processor_65xx * pProcessor, Parameters * parameters, U8 * pMemory);
void programEnd(int selected_program, U8 * pMemory, Processor_65xx * pProcessor);

U8 timePassed(struct timespec * oldTime, struct timespec * newTime, time_t timeToPass);

U8 memory[MOS65xx_MEMSIZE];

char lineBuf[512];
int lineBufIndex = 0;
char inBuff[20];
int inBuffIndex = 0;

extern opCode_st * mos65xx__opCodes;

extern FILE * tmpLog;

/*******************************************************************************
* Handles loading and running the program.
*
* Returns: -
*******************************************************************************/
int main(int argc, char * argv[]) {

	Processor_65xx processor;
	struct timespec startTime, endTime;
	time_t realizedSpeed;

	struct timespec runTime, syncTime, refreshTime = {0}, kbScanTime = {0};

	Parameters * pCliArguments = handleCLIArguments(argc, argv);

	programSelector(&processor, pCliArguments, memory);

	clock_gettime(CLOCK_REALTIME, &startTime);

	if (pCliArguments->selectedProgram == PRG_APPLE_I) {
		apple_i_run(&processor);
	} else if (pCliArguments->selectedProgram == PRG_C64) {
		c64_run(&processor);
	} else {
		term_conf();

		while (1) {

			time_t diff = 0;
			unsigned int frequency = 0;
			mos65xx_addr oldPC = processor.reg.PC;

			clock_gettime(CLOCK_REALTIME, &runTime);

			if (processor.cycles.currentOp != 0) {
				/* Calculate elapsed ns. */
				if (realizedSpeed > runTime.tv_nsec) {
					diff = TSPEC_NSEC_MAX - (realizedSpeed - runTime.tv_nsec);
				} else {
					diff = runTime.tv_nsec - realizedSpeed;
				}

				frequency = (unsigned int)((double)diff / (double)processor.cycles.currentOp * (double)CLOCK_FREQ) / 1000;
			}

			realizedSpeed = runTime.tv_nsec;

			mos65xx_handleOp(&processor);

			if (processor.reg.PC == oldPC) {

				DRAW_TERM(screenRefresh(memory, frequency));

				term_deConf();
				printf("\033[m\n\n0x%04X: Infinite loop detected!\n\n", oldPC);
				break;
			}


			if (timePassed(&refreshTime, &runTime, REFRESH_RATE_NS)) {
				/* Sync screen refreshing to 50 hz. */
				refreshTime = runTime;
				DRAW_TERM(screenRefresh(memory, frequency));
			}

			/* Read keyboard every 10 ms. */
			if (timePassed(&kbScanTime, &runTime, KB_SCAN_RATE_NS)) {
				U8 pressedKey = 0;
				kbScanTime = runTime;

				pressedKey = readKeyboard();
				if (pressedKey == 0x1B) { // ESC detected.
					printf(BACKSPACE);
					pressedKey = readKeyboard();

					if (pressedKey == 0xFF || pressedKey == 0x1B) {  // ESC key pressed, quit.

						term_deConf();

						printf("ESC key hit.\n");
						break;
					} else if (pressedKey == '[') { // CTRL character detected.
						printf(BACKSPACE); // TODO Doesn't erase ABCD from arrow keys
					} else { // Unknown ESC sequence.
						printf(BACKSPACE);
					}
				} else if (pressedKey == 0x09) { // TAB pressed (reset).
					Memory_areas mem;
					mem.RAM = memory;
					mem.ROM = NULL;
					mem.IO = NULL;
					mos65xx_init(&processor, &mem, fnMemRead, fnMemWrite, pCliArguments->debugLogging, pCliArguments);
					DRAW_TERM(screenClear(&processor));
				}

				fnMemWrite(&processor, keyboardAddr, pressedKey);

			}

			processor.totOperations++;

			if (!pCliArguments->noSpeedLimit) {
				/* Sync to 1 Mhz. */
				do {

					clock_gettime(CLOCK_REALTIME, &syncTime);

				} while (!timePassed(&runTime, &syncTime, (pCliArguments->slowDown ? pCliArguments->slowDown * (pCliArguments->fasterSlowDown ? 1 : 1000) : processor.cycles.currentOp) * TICK_NS));
			}
		}
	}
	clock_gettime(CLOCK_REALTIME, &endTime);

	programEnd(pCliArguments->selectedProgram, memory, &processor);

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
		"Operations performed: %lu\n\n Start time: %ds %ldms %ldns\n   End time: %lds %ldms %ldns\n\n Difference: %lds %ldms %ldns\n\nCycle count: %llu\n\n",
		processor.totOperations,
		0,
		nsToMs(startTime.tv_nsec),
		startTime.tv_nsec - msToNs(nsToMs(startTime.tv_nsec)),
		endTime.tv_sec - startTime.tv_sec,
		nsToMs(endTime.tv_nsec),
		endTime.tv_nsec - msToNs(nsToMs(endTime.tv_nsec)),
		secsPassed,
		nsToMs(nsecsPassed),
		nsecsPassed - msToNs(nsToMs(nsecsPassed)),
		processor.cycles.totalCycles
	);
}

/*******************************************************************************
* Reads one byte from the memory.
*
* Arguments:
*	address - Memory address to read from.
*
* Returns: Byte read from the memory.
*******************************************************************************/
U8 fnMemRead(Processor_65xx * pProcessor, mos65xx_addr address) {
	if (((Parameters *)pProcessor->pUtil)->selectedProgram == PRG_APPLE_I && address == 0xD010) {
		pProcessor->mem.RAM[0xD011] = 0;
	}

	if (!isROMAddress(address) && !isRAMAddress(address)) {
		return 0;
	}

	return pProcessor->mem.RAM[address];
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
void fnMemWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value) {
	if (!isROMAddress(address)) {
		pProcessor->mem.RAM[address] = value;
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
U8 readKeyboard(void) {
	U8 key;

	key = fgetc(stdin);

	if (key != 0xFF) {
		if (key == '\r' || key == 0x7F) {
			printf("\b \b");
		}
		printf("\b \b");
	}

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
void screenClear(Processor_65xx * pProcessor) {
	system("clear");

	printf("%s%s", CURSOR_OFF, COLOR_GRY_BLK);

	for (int memIndex = screenMemAddr + screenMemSize; memIndex >= screenMemAddr; memIndex--) {
		fnMemWrite(pProcessor, memIndex, ' ');
	}
}

/*******************************************************************************
* Refreshes screen from the screen memory.
*
* Arguments:
*	frequency - Calculated real frequency of the processor.
*
* Returns: Nothing.
*******************************************************************************/
void screenRefresh(U8 * pMemory, unsigned int frequency) {

	static int secondCounter = 50;

	U8 * screenMem = &pMemory[screenMemAddr];

	/* Buffer for the whole screen, including \r\n at the end. */
	U8 screenBuf[SCREEN_W * SCREEN_H + SCREEN_H * 2 - 1] = {' '};

	for (int lineIndex = 0; lineIndex < SCREEN_H; lineIndex++) {
		memcpy((screenBuf + lineIndex * (SCREEN_W + 2)), (screenMem + lineIndex * SCREEN_W), SCREEN_W);
		screenBuf[lineIndex * (SCREEN_W + 2) - 2] = '\r';
		screenBuf[lineIndex * (SCREEN_W + 2) - 1] = '\n';
	}
	screenBuf[sizeof(screenBuf) - 1] = '\0';

	printf("%s\033[1;1H%s%s", COLOR_GRY_BLK, screenBuf, COLOR_BLK_BLK);

	if (secondCounter++ >= 50) {
		printf("%s\033[%d;1H%10u hz%s", COLOR_GRY_BLK, SCREEN_H + 1, frequency, COLOR_BLK_BLK);
		secondCounter = 0;
	}
	printf("\033[%d;1H", SCREEN_H + 2);
}

/*******************************************************************************
* Loads selected program to the memory.
*
* Arguments:
*	selected_program	- Index number of the selected program.
*	pMemory				- Pointer to the memory buffer.
*
*******************************************************************************/
void programSelector(Processor_65xx * pProcessor, Parameters * parameters, U8 * pMemory) {

	if (parameters->selectedProgram == PRG_ASSEMBLE) {
		//mos65xx_assemble("./prg/asm/32bitdiv.asm", pMemory);
		exit(0);
	} else if (parameters->selectedProgram == PRG_STORE_TO_MEM) { // write values to memory.
		loadFile(pMemory, 0xFE00, "./prg/bin/memWrite.rom", mos65xx_BIN);
		addRAMArea(0x0000, 0xFFFF); // System & User space
	} else if (parameters->selectedProgram == PRG_CLEAR_MEM) { // Clears X amount of memory from memory address (100), Y
		loadFile(pMemory, 0xFE00, "./prg/bin/memClear.rom", mos65xx_BIN);
		addRAMArea(0x0000, 0xFFFF); // System & User space
	} else if (parameters->selectedProgram == PRG_WEEKDAY) { // Day of the week. Y = year, X = month, AC = day
		loadFile(pMemory, 0xFE00, "./prg/bin/weekday.rom", mos65xx_BIN);
		addRAMArea(0x0000, 0xFFFF); // System & User space
	} else if (parameters->selectedProgram == PRG_KEYBOARD) {
		loadFile(pMemory, 0xFE00, "./prg/bin/kbTest.rom", mos65xx_BIN);
		addRAMArea(0x0000, 0xFFFF); // System & User space
	} else if (parameters->selectedProgram == PRG_BLINK) {
		loadFile(pMemory, 0xFE00, "./prg/bin/blink.rom", mos65xx_BIN);
		addRAMArea(0x0000, 0xFFFF); // System & User space
	} else if (parameters->selectedProgram == PRG_NUMQUERY) {
		loadFile(pMemory, 0xFE00, "./prg/bin/numQuery.rom", mos65xx_BIN);
		addRAMArea(0x0000, 0xFFFF); // System & User space
	} else if (parameters->selectedProgram == PRG_PRINT) {
		loadFile(pMemory, 0xFE00, "./prg/bin/print.rom", mos65xx_BIN);
		addRAMArea(0x0000, 0xFFFF); // System & User space
	} else if (parameters->selectedProgram == PRG_APPLE_I) {
		apple_i_init(pProcessor);
	} else if (parameters->selectedProgram == PRG_C64) {
		c64_init(pProcessor);
	} else if (parameters->selectedProgram == PRG_OPCODETEST) {
		loadFile(pMemory, 0x0000, "./prg/test/6502_functional_test.bin", mos65xx_BIN);
		addRAMArea(0x0000, 0xFFFF); // System & User space
	}

	if (parameters->selectedProgram != PRG_APPLE_I
	&&	parameters->selectedProgram != PRG_C64
	) {
		Memory_areas mem;

		mem.RAM = pMemory;
		mem.ROM = NULL;
		mem.IO = NULL;

		mos65xx_init(pProcessor, &mem, fnMemRead, fnMemWrite, parameters->debugLogging, parameters);

		screenClear(pProcessor);

		if (parameters->selectedProgram == PRG_OPCODETEST) {
			pProcessor->reg.PC = 0x400;
		}
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
void programEnd(int selected_program, U8 * pMemory, Processor_65xx * pProcessor) {

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
U8 timePassed(struct timespec * pOldTime, struct timespec * pNewTime, time_t timeToPass) {

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
* Handle command line arguments.
*
* Arguments:
*	argc, argv	- Command line arguments
*
* Returns: Pointer to a struct with all resolved parameters.
*******************************************************************************/
Parameters * handleCLIArguments(int argc, char * argv[]) {

	Parameters * pCLIArgs = calloc(sizeof(Parameters), 1);
	char * currArg = argv[1];

	/* First argument must be program index number */
	if (argc < 2 ||	currArg[0] < '0' || currArg[0] > '9') {
		printf(
			"%s program index %s%s%sgiven as parameter\n\n",
			argc < 2 ? "No": "Invalid",
			argc < 2 ? "": "(",
			argc < 2 ? "" : currArg,
			argc < 2 ? "": ") "
		);
		printHelp();
	}

	/* Parse selected program index number */
	pCLIArgs->selectedProgram = currArg[0] - '0';
	if (currArg[1] >= '0' && currArg[1] <= '9') {
		pCLIArgs->selectedProgram *= 10;
		pCLIArgs->selectedProgram += currArg[1] - '0';
	} else if (currArg[1] != '\0') {
		printf("Invalid program index (%s) given as parameter\n\n", currArg);
		printHelp();
	}

	if (pCLIArgs->selectedProgram >= PRG_NONE) {
		printf("Program index (%d) larger than count of programs\n\n", pCLIArgs->selectedProgram);
		printHelp();
	}

	for (int argIdx = 2; argIdx < argc; argIdx++) {

		char * currArg = argv[argIdx];

		if (currArg[1] == 'd') {
			pCLIArgs->debugLogging = 1;

		} else if (currArg[1] == 'l') {
			pCLIArgs->logIO = 1;

		} else if (currArg[1] == 's') {

			pCLIArgs->debugLogging |= 2;

			pCLIArgs->fasterSlowDown = currArg[2] == 'f' ? 1 : 0;
			pCLIArgs->noSpeedLimit = currArg[2] == 'n' ? 1 : 0;

			if (argIdx + 1 == argc
			||	argv[argIdx + 1][0] == '-'
			) {
				pCLIArgs->slowDown = 500;
			} else {
				argIdx++;

				for (int strIdx = 0; currArg[strIdx] != 0; strIdx++) {
					if (!isdigit(currArg[strIdx])) {
						printf("\n\nInvalid argument: %s %s\n\n", argv[argIdx - 1], currArg);
						printHelp();
					}
					pCLIArgs->slowDown *= 10;
					pCLIArgs->slowDown += currArg[strIdx] - '0';
				}
			}
		} else {
			printf("\n\nInvalid argument: %s\n\n", currArg);
			printHelp();
		}
	}

	return pCLIArgs;
}

/*******************************************************************************
* Prints help text
*
* Arguments:
*	-
*
* Returns: None, exits the program
*******************************************************************************/
void printHelp(void) {
	printf("Valid program indexes:\n");
	printf("\t%d - Store to memory\n", PRG_STORE_TO_MEM);
	printf("\t%d - Clear memory region\n", PRG_CLEAR_MEM);
	printf("\t%d - Calculate weekday (1-7) from date\n", PRG_WEEKDAY);
	printf("\t%d - Run the assembler\n", PRG_ASSEMBLE);
	printf("\t%d - Keyboard test, prints out 5 characters entered\n", PRG_KEYBOARD);
	printf("\t%d - Cursor blink\n", PRG_BLINK);
	printf("\t%d - Number query\n", PRG_NUMQUERY);
	printf("\t%d - String print test\n", PRG_PRINT);
	printf("\t%d - Apple I emulator\n", PRG_APPLE_I);
	printf("\t%d - Commodore 64 emulator\n", PRG_C64);
	printf("\t%d - Opcode test\n", PRG_OPCODETEST);
	printf("\n\n");
	printf("Options:\n\t-d : Print operations.\n\t-s(f/n) : Slow down 1-1000 ms/op (def 500)\n\t          f: 1-1000 us/op\n\t          n: no limit\n\n");
	exit(1);
}
