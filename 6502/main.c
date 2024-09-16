#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#define TICK_NS (time_t)((double)1 / (double)CLOCK_FREQ * (double)S_TO_NS(1))

#define TSPEC_NSEC_MAX 999999999

#define DRAW_TERM(_x) if (!(debugLogging & 1)) _x

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
	PRG_APPLE_I,
	PRG_OPCODETEST
} programList;

typedef enum {
	mos6502_BIN,
	mos6502_HEX,
	mos6502_ASM
} mos6502_fileType;

typedef struct {
	mos6502_addr startAddr;
	mos6502_addr endAddr;
} address_range;

mos6502_addr keyboardAddr = 0xFDFF;
mos6502_addr screenMemAddr = 0xF800;
mos6502_addr screenMemSize = SCREEN_W * SCREEN_H;

void loadFile(U8 * pMemory, mos6502_addr offset, const char * pPath, mos6502_fileType fileType);

U8 fnMemRead(mos6502_addr address);
void fnMemWrite(mos6502_addr address, U8 value);

U8 readKeyboard(void);

void screenClear(void);
void screenRefresh(unsigned int freq);
void terminalOutput(void);

void programSelector(int selected_program, U8 * pMemory);
void programEnd(int selected_program, U8 * pMemory, mos6502_processor_st * pProcessor);
void addROMArea(mos6502_addr startAddr, mos6502_addr endAddr);
U8 isROMAddress(mos6502_addr address);
void addRAMArea(mos6502_addr startAddr, mos6502_addr endAddr);
U8 isRAMAddress(mos6502_addr address);

U8 timePassed(struct timespec * oldTime, struct timespec * newTime, time_t timeToPass);

U8 appleKeyConv(U8 input);
U8 appleScreenConv(U8 input);

void logTrace(void);

U8 * ram;

char lineBuf[512];
int lineBufIndex = 0;
char inBuff[20];
int inBuffIndex = 0;

int * g_selectedProgram;

U8 ROMAddrRangeCount;
address_range ROMAddrList[0xFF];
U8 RAMAddrRangeCount;
address_range RAMAddrList[0xFF];

extern opCode_st * mos6502__opCodes;

extern char * traceBuf[80];
extern U8 traceBufIndex;

extern FILE * tmpLog;

/*******************************************************************************
* Handles loading and running the program.
*
* Returns: -
*******************************************************************************/
int main(int argc, char * argv[]) {

	int selected_program;
	U8 memory[MOS6502_MEMSIZE];
	U8 debugLogging = 0;
	time_t slowDown = 0;
	U8 fasterSlowDown = 0;
	U8 noSpeedLimit = 0;

	g_selectedProgram = &selected_program;

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

					debugLogging |= 2;

					fasterSlowDown = argv[currArg][2] == 'f' ? 1 : 0;
					noSpeedLimit = argv[currArg][2] == 'n' ? 1 : 0;

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
		printf("\t%d - Apple I emulator\n", PRG_APPLE_I);
		printf("\t%d - Opcode test\n", PRG_OPCODETEST);
		printf("\n\n");
		printf("Options:\n\t-d : Print operations.\n\t-s(f/n) : Slow down 1-1000 ms/op (def 500)\n\t          f: 1-1000 us/op\n\t          n: no limit\n\n");
		exit(1);

	}

	ram = memory;

	DRAW_TERM(screenClear());

	struct timespec startTime, endTime;
	unsigned long totalOperations = 0;
	time_t realizedSpeed;
	U8 retVal = 0;

	mos6502_processor_st processor;

	struct timespec runTime, syncTime, refreshTime = {0}, kbScanTime = {0};

	programSelector(selected_program, memory);

	mos6502_init(&processor, fnMemRead, fnMemWrite, debugLogging);

	if (selected_program == PRG_OPCODETEST) {
		processor.reg.PC = 0x400;
	}

	fcntl(0, F_SETFL, O_NONBLOCK);
	system("/bin/stty raw");

	clock_gettime(CLOCK_REALTIME, &startTime);
	
	while (retVal != 0xFF) {

		time_t diff = 0;
		unsigned int frequency = 0;
		mos6502_addr oldPC = processor.reg.PC;

		clock_gettime(CLOCK_REALTIME, &runTime);

		if (processor.cycles.currentOpCycles != 0) {
			/* Calculate elapsed ns. */
			if (realizedSpeed > runTime.tv_nsec) {
				diff = TSPEC_NSEC_MAX - (realizedSpeed - runTime.tv_nsec);
			} else {
				diff = runTime.tv_nsec - realizedSpeed;
			}

			frequency = (unsigned int)((double)diff / (double)retVal * (double)CLOCK_FREQ) / 1000;
		}

		realizedSpeed = runTime.tv_nsec;

		mos6502_handleOp(&processor);

		if (processor.reg.PC == oldPC) {
			
			DRAW_TERM(screenRefresh(frequency));

			system("/bin/stty cooked");
			printf("%s%s", CURSOR_RESET, TERM_RESET);
			printf("\033[m\n\n0x%04X: Infinite loop detected!\n\n", oldPC);
			break;
		} 

		/* Sync screen refreshing to 50 hz. */
		if (timePassed(&refreshTime, &runTime, REFRESH_RATE_NS)) {
			refreshTime = runTime;
			
			if (selected_program == PRG_APPLE_I) {
				DRAW_TERM(terminalOutput());
			} else {
				DRAW_TERM(screenRefresh(frequency));
			}
		}

		/* Read keyboard every 10 ms. */
		if (timePassed(&kbScanTime, &runTime, KB_SCAN_RATE_NS)) {
			U8 pressedKey = 0;
			kbScanTime = runTime;

			pressedKey = readKeyboard();
			if (pressedKey == 0x1B) { // ESC detected.
				printf("\b \b");
				pressedKey = readKeyboard();

				if (pressedKey == 0xFF || pressedKey == 0x1B) {  // ESC key pressed, quit.
					system("/bin/stty cooked");
					printf("%s%s", CURSOR_RESET, TERM_RESET);

					printf("ESC key hit.\n");

					logTrace();
					exit(0);
				} else if (pressedKey == '[') { // CTRL character detected.
					printf("\b \b");
					pressedKey = readKeyboard();
					printf("\b \b");
					pressedKey = readKeyboard();
					printf("\b \b");
				} else { // Unknown ESC sequence.
					printf("\b \b");
				}
			} else if (pressedKey == 0x09) { // TAB pressed (reset).
				mos6502_init(&processor, fnMemRead, fnMemWrite, debugLogging);
				printf("\b \b\b \b\b \b\b \b\b \b\b \b\b \b");
			}
			
			if (selected_program == PRG_APPLE_I) {

				pressedKey = appleKeyConv(pressedKey);

				if (pressedKey != 0xFF
				&&	!(fnMemRead(0xD011) & 0x80)
				) {

					inBuff[inBuffIndex++] = pressedKey;
					if (pressedKey == '\r') {
						inBuff[inBuffIndex - 1] = '\0';
						fprintf(tmpLog, "INPUT: %s\r\n", inBuff);
						inBuffIndex = 0;
					}
					fnMemWrite(0xD011, 0x80);
					fnMemWrite(0xD010, pressedKey + 0x80);

				} else if (
					pressedKey == 0xFF
				&&	fnMemRead(0xD011) & 0x80
				) {
					fnMemWrite(0xD011, 0);

				} else {
				}
			} else {
				fnMemWrite(keyboardAddr, pressedKey);
			}
		}

		totalOperations++;

		if (!noSpeedLimit) {
			/* Sync to 1 Mhz. */
			do {

				clock_gettime(CLOCK_REALTIME, &syncTime);

			} while (!timePassed(&runTime, &syncTime, (slowDown ? slowDown * (fasterSlowDown ? 1 : 1000) : retVal) * TICK_NS));
		}
	}
	clock_gettime(CLOCK_REALTIME, &endTime);

	logTrace();


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
		"Operations performed: %lu\n\n Start time: %ds %ldms %ldns\n   End time: %lds %ldms %ldns\n\n Difference: %lds %ldms %ldns\n\nCycle count: %llu\n\n",
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
		processor.cycles.totalCycles
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
	if (*g_selectedProgram == PRG_APPLE_I && address == 0xD010) {
		ram[0xD011] = 0;
	}
	
	if (!isROMAddress(address) && !isRAMAddress(address)) {
		return 0;
	}

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
	if (!isROMAddress(address)) {
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
void screenClear(void) {
	system("clear");

	if (*g_selectedProgram == PRG_APPLE_I) return;

	printf("%s%s", CURSOR_OFF, COLOR_GRY_BLK);

	for (int memIndex = screenMemAddr + screenMemSize; memIndex >= screenMemAddr; memIndex--) {
		fnMemWrite(memIndex, ' ');
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
void screenRefresh(unsigned int frequency) {

	static int secondCounter = 50;

	U8 * screenMem = &ram[screenMemAddr];

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
* Handles terminal style output.
*
* Arguments:
*	none
*
* Returns: Nothing.
*******************************************************************************/
void terminalOutput(void) {

	U8 memChar = fnMemRead(0xD012);

	if (memChar != 0) {
		if (memChar & 0x80) {
			U8 chr = appleScreenConv(memChar & ~0x80);

			if (chr != 0xFF) {
				lineBuf[lineBufIndex++] = chr;

				if (chr == '\r') {
					lineBuf[lineBufIndex - 1] = '\0';
					fprintf(tmpLog, "OUTPUT: %s\r\n", lineBuf);
					lineBufIndex = 0;
				}

				printf("%c%s", chr, chr == '\r' ? "\n" : "");
			}
		}
	}

	fnMemWrite(0xD012, 0);
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
		addRAMArea(0x0000, 0xFFFF); // System & User space
	} else if (selected_program == PRG_CLEAR_MEM) { // Clears X amount of memory from memory address (100), Y
		loadFile(pMemory, 0xFE00, "./prg/bin/memClear.6502", mos6502_BIN);
		addRAMArea(0x0000, 0xFFFF); // System & User space
	} else if (selected_program == PRG_WEEKDAY) { // Day of the week. Y = year, X = month, AC = day
		loadFile(pMemory, 0xFE00, "./prg/bin/weekday.6502", mos6502_BIN);
		addRAMArea(0x0000, 0xFFFF); // System & User space
	} else if (selected_program == PRG_KEYBOARD) {
		loadFile(pMemory, 0xFE00, "./prg/bin/kbTest.6502", mos6502_BIN);
		addRAMArea(0x0000, 0xFFFF); // System & User space
	} else if (selected_program == PRG_BLINK) {
		loadFile(pMemory, 0xFE00, "./prg/bin/blink.6502", mos6502_BIN);
		addRAMArea(0x0000, 0xFFFF); // System & User space
	} else if (selected_program == PRG_NUMQUERY) {
		loadFile(pMemory, 0xFE00, "./prg/bin/numQuery.6502", mos6502_BIN);
		addRAMArea(0x0000, 0xFFFF); // System & User space
	} else if (selected_program == PRG_PRINT) {
		loadFile(pMemory, 0xFE00, "./prg/bin/print.6502", mos6502_BIN);
		addRAMArea(0x0000, 0xFFFF); // System & User space
	} else if (selected_program == PRG_APPLE_I) {
		loadFile(pMemory, 0xE000, "./prg/apple_i/bin/intBasic.6502", mos6502_BIN);
		loadFile(pMemory, 0xFF00, "./prg/apple_i/bin/wozmon.6502", mos6502_BIN);
		screenMemAddr = 0xD012;
		keyboardAddr = 0xD010;
		addRAMArea(0x0000, 0x7FFF); // System & User space
		addRAMArea(0xD010, 0xD010); // Keyboard In
		addRAMArea(0xD011, 0xD011); // Keyboard Control
		addRAMArea(0xD012, 0xD012); // Display Out
		addRAMArea(0xD013, 0xD013); // Display Control
		addROMArea(0xE000, 0xEFFF); // Integer Basic
		addROMArea(0xFF00, 0xFFFF); // Monitor
	} else if (selected_program == PRG_OPCODETEST) {
		loadFile(pMemory, 0x0000, "./prg/test/6502_functional_test.bin", mos6502_BIN);
		addRAMArea(0x0000, 0xFFFF); // System & User space
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


U8 appleKeyConv(U8 input) {

	if (input >= 'a' && input <= 'z') {
		input -= 0x20;
	} else if (input == 0x7F) {
		input = '_';
	} else if ((input >= 'A' && input <= 'Z') || (input >= ' ' && input <= '@') || input == '^' || input == '\r') {
		// Capital letters, numbers, return and some special characters are passed through
	} else {
		input = 0xFF;
	}

	return input;
}

U8 appleScreenConv(U8 input) {

	if (input == '\r' || (input >= ' ' && input <= '_')) {
		// Capital letters, numbers, return and some special characters are passed through
	} else {
		input = 0xFF;
	}

	return input;
}


void logTrace(void) {
	U8 tempIndex;
	
	for (tempIndex = 0; tempIndex < 80; tempIndex++) {
		if (traceBuf[tempIndex] != NULL) break;
	}

	if (tempIndex < 80) {
		for (tempIndex = 0; tempIndex < 80; tempIndex++) {
			if (traceBuf[tempIndex] == NULL) break;
		}

		if (tempIndex != 80) {
			for (tempIndex = 0; tempIndex < traceBufIndex; tempIndex++) {
				fprintf(tmpLog, "%s", traceBuf[tempIndex]);
			}
		} else {
			for (tempIndex = traceBufIndex; tempIndex < 80; tempIndex++) {
				fprintf(tmpLog, "%s", traceBuf[tempIndex]);
			}
			for (tempIndex = 0; tempIndex < traceBufIndex; tempIndex++) {
				fprintf(tmpLog, "%s", traceBuf[tempIndex]);
			}
		}
	}
}

void addROMArea(mos6502_addr startAddr, mos6502_addr endAddr) {
	if (startAddr > endAddr) {
		printf("\n\nInvalid ROM area range (0x%04x - 0x%04x)...\n\n", startAddr, endAddr);
		exit(1);
	}

	ROMAddrList[ROMAddrRangeCount].startAddr = startAddr;
	ROMAddrList[ROMAddrRangeCount].endAddr = endAddr;

	ROMAddrRangeCount++;
}
U8 isROMAddress(mos6502_addr address) {
	for (int areaIdx = 0; areaIdx < ROMAddrRangeCount; areaIdx++) {
		if (ROMAddrList[areaIdx].startAddr <= address
		&&	ROMAddrList[areaIdx].endAddr >= address
		) {
			return 1;
		}
	}

	return 0;
}

void addRAMArea(mos6502_addr startAddr, mos6502_addr endAddr) {
	if (startAddr > endAddr) {
		printf("\n\nInvalid RAM area range (0x%04x - 0x%04x)...\n\n", startAddr, endAddr);
		exit(1);
	}

	RAMAddrList[RAMAddrRangeCount].startAddr = startAddr;
	RAMAddrList[RAMAddrRangeCount].endAddr = endAddr;

	RAMAddrRangeCount++;
}
U8 isRAMAddress(mos6502_addr address) {
	for (int areaIdx = 0; areaIdx < RAMAddrRangeCount; areaIdx++) {
		if (RAMAddrList[areaIdx].startAddr <= address
		&&	RAMAddrList[areaIdx].endAddr >= address
		) {
			return 1;
		}
	}

	return 0;
}