#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
// Non-blocking fgetc()
#include <fcntl.h>

#include <SDL2/SDL.h>

#include "65xx.h"


#include "apple_i/apple_i.h"
#include "c64/c64.h"

#define SCREEN_W 60
#define SCREEN_H 25
#define REFRESH_RATE_MS 20

#define DRAW_TERM(_x) if (!(pCliArguments->debugLogging & 1)) _x

#define TERM_RESET		"\033[m"
#define COLOR_BLK_BLK	"\033[30;40m"
#define COLOR_GRY_BLK	"\033[30;47m"
#define CURSOR_OFF		"\033[?25l"
#define CURSOR_RESET	"\033[?25h"
#define BACKSPACE		"\033[D \033[D"

typedef enum {
	PRG_ASSEMBLE,
	PRG_BLINK,
	PRG_NUMQUERY,
	PRG_PRINT,
	PRG_APPLE_I,
	PRG_SCHEDULER,
	PRG_C64,
	PRG_OPCODETEST,
	PRG_NONE
} SelectedProgram;

typedef struct {
	SelectedProgram	selectedProgram;
	U8				debugLogging;
	U32				slowDown;
	U8				fasterSlowDown;
	U8				noSpeedLimit;
	U8				logIO;
} Parameters;

mos65xx_addr keyboardAddr = 0xEFFF;
mos65xx_addr screenMemAddr = 0xE000;
mos65xx_addr screenMemSize = SCREEN_W * SCREEN_H;

Parameters * handleCLIArguments(int argc, char * argv[]);
void printHelp(void);

U8 fnMemRead(Processor_65xx * pProcessor, mos65xx_addr address);
void fnMemWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value);

void screenClear(Processor_65xx * pProcessor);
void screenRefresh(U8 * pMemory, unsigned int freq);

void programSelector(Processor_65xx * pProcessor, Parameters * parameters);
void programEnd(int selected_program, U8 * pMemory, Processor_65xx * pProcessor);

char lineBuf[512];
int lineBufIndex = 0;
char inBuff[20];
int inBuffIndex = 0;

void * pDevStruct;

/*******************************************************************************
* Handles loading and running the program.
*
* Returns: -
*******************************************************************************/
int main(int argc, char * argv[]) {

	Processor_65xx processor = {0};

	Parameters * pCliArguments = handleCLIArguments(argc, argv);

	programSelector(&processor, pCliArguments);

	if (pCliArguments->selectedProgram == PRG_APPLE_I) {
		apple_i_run(&processor);
	} else if (pCliArguments->selectedProgram == PRG_C64) {
		c64_run(pDevStruct);
	} else {
		
		U32 screenTicks = SDL_GetTicks();
		U32 syncTicks = SDL_GetTicks();

		while (1) {
			U32 currTick = SDL_GetTicks();
			U32 screenDiff = currTick - screenTicks;

			unsigned int frequency = 0;
			mos65xx_addr oldPC = processor.reg.PC;

			// TODO: create SDL event handler for keyboard inputs.

			mos65xx_handleOp(&processor);

			if (processor.reg.PC == oldPC) {

				DRAW_TERM(screenRefresh(processor.pMem->RAM, frequency));

				printf("\033[m\n\n0x%04X: Infinite loop detected!\n\n", oldPC);
				break;
			}


			if (!(pCliArguments->debugLogging) && screenDiff >= REFRESH_RATE_MS) {
				/* Sync screen refreshing to 50 hz. */
				DRAW_TERM(screenRefresh(processor.pMem->RAM, frequency));

				screenTicks = currTick;
			}

			processor.totOperations++;

			if (!pCliArguments->noSpeedLimit) {
				/* TODO: Sync to 1 Mhz. */
				U32 syncDiff = currTick - syncTicks;

				while (syncDiff < pCliArguments->slowDown) {
			
					currTick = SDL_GetTicks();
					syncDiff = currTick - syncTicks;
				}

				syncTicks = currTick;
			}
		}
	}

	programEnd(pCliArguments->selectedProgram, processor.pMem->RAM, &processor);

	return 0;
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
		pProcessor->pMem->RAM[0xD011] = 0;
	}

	if (!isROMAddress(address) && !isRAMAddress(address)) {
		return 0;
	}

	return pProcessor->pMem->RAM[address];
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
		pProcessor->pMem->RAM[address] = value;
	}
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

	printf("%s", CURSOR_OFF);

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

	printf("\033[1;1H%s", screenBuf);

	if (secondCounter++ >= 50) {
		//printf("%s\033[%d;1H%10u hz%s", COLOR_GRY_BLK, SCREEN_H + 1, frequency, COLOR_BLK_BLK);
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
void programSelector(Processor_65xx * pProcessor, Parameters * parameters) {


	if (parameters->selectedProgram != PRG_APPLE_I
	&&	parameters->selectedProgram != PRG_C64
	) {
		U8 *  pMemory = malloc(U16_MAX);

		if (parameters->selectedProgram == PRG_ASSEMBLE) {
			//mos65xx_assemble("./prg/asm/32bitdiv.asm", pMemory);
			exit(0);
		} else if (parameters->selectedProgram == PRG_BLINK) {
			loadFile(pMemory, 0xF000, "./prg/bin/blink.rom", mos65xx_BIN);
			addRAMArea(0x0000, 0xFFFF); // System & User space
		} else if (parameters->selectedProgram == PRG_NUMQUERY) {
			loadFile(pMemory, 0xF000, "./prg/bin/numQuery.rom", mos65xx_BIN);
			addRAMArea(0x0000, 0xFFFF); // System & User space
		} else if (parameters->selectedProgram == PRG_PRINT) {
			loadFile(pMemory, 0xF000, "./prg/bin/print.rom", mos65xx_BIN);
			addRAMArea(0x0000, 0xFFFF); // System & User space
		} else if (parameters->selectedProgram == PRG_SCHEDULER) {
			loadFile(pMemory, 0xF000, "./prg/bin/scheduler.rom", mos65xx_BIN);
			addRAMArea(0x0000, 0xFFFF); // System & User space
		} else if (parameters->selectedProgram == PRG_OPCODETEST) {
			loadFile(pMemory, 0x0000, "./prg/test/6502_functional_test.bin", mos65xx_BIN);
			addRAMArea(0x0000, 0xFFFF); // System & User space
		}

		Memory_areas * pMem = calloc(1, sizeof(Memory_areas));

		pMem->RAM = pMemory;
		pMem->ROM = NULL;

		mos65xx_init(pProcessor, pMem, fnMemRead, fnMemWrite, parameters->debugLogging, parameters);

		screenClear(pProcessor);

		if (parameters->selectedProgram == PRG_OPCODETEST) {
			pProcessor->reg.PC = 0x400;
		}
	} else if (parameters->selectedProgram == PRG_APPLE_I) {
		apple_i_init(pProcessor);
	} else if (parameters->selectedProgram == PRG_C64) {
		pDevStruct = c64_init(pProcessor);
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
	selected_program = selected_program;
	pMemory = pMemory;
	pProcessor = pProcessor;
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

	Parameters * pCLIArgs = calloc(1, sizeof(Parameters));
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

			//pCLIArgs->debugLogging |= 2;

			pCLIArgs->fasterSlowDown = currArg[2] == 'f' ? 1 : 0;
			pCLIArgs->noSpeedLimit = currArg[2] == 'n' ? 1 : 0;

			if (argIdx + 1 == argc
			||	argv[argIdx + 1][0] == '-'
			) {
				pCLIArgs->slowDown = 500;
			} else {
				argIdx++;
				
				currArg = argv[argIdx];

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
	printf("\t%d - Run the assembler\n", PRG_ASSEMBLE);
	printf("\t%d - Cursor blink\n", PRG_BLINK);
	printf("\t%d - Number query\n", PRG_NUMQUERY);
	printf("\t%d - String print test\n", PRG_PRINT);
	printf("\t%d - Apple I emulator\n", PRG_APPLE_I);
	printf("\t%d - Task scheduler\n", PRG_SCHEDULER);
	printf("\t%d - Commodore 64 emulator\n", PRG_C64);
	printf("\t%d - Opcode test\n", PRG_OPCODETEST);
	printf("\n\n");
	printf("Options:\n\t-d : Print operations.\n\t-s(f/n) : Slow down 1-1000 ms/op (def 500)\n\t          f: 1-1000 us/op\n\t          n: no limit\n\n");
	exit(1);
}
