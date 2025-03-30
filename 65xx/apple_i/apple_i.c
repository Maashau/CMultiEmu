/*******************************************************************************
* apple_i.c
*
* Apple I emulator.
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// Non-blocking fgetc()
#include <fcntl.h>

#include "../65xx.h"

#define term_conf()	fcntl(0, F_SETFL, O_NONBLOCK); system("/bin/stty raw")
#define term_deConf() system("/bin/stty cooked");

#define TSPEC_NSEC_MAX 999999999

#define usToNs(_usecs)	(_usecs * 1000)
#define msToNs(_msecs)	(usToNs(_msecs) * 1000)
#define sToNs(_msecs)		(msToNs(_msecs) * 1000)

#define nsToUs(_nsecs)	(_nsecs / 1000)
#define nsToMs(_nsecs)	(nsToUs(_nsecs) / 1000)

#define CLOCK_FREQ 1000000
#define TICK_NS (time_t)((double)1 / (double)CLOCK_FREQ * (double)sToNs(1))
#define KB_SCAN_RATE_NS	msToNs(10)

#define BACKSPACE		"\033[D \033[D"

#define KBD_ADDR     0xD010
#define KBDCR_ADDR   0xD011
#define DSP_ADDR     0xD012
#define DSPCR_ADDR   0xD013

U8 apple_i_memRead(Processor_65xx * pProcessor, mos65xx_addr address);
void apple_i_memWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value);

void apple_i_out(Processor_65xx * pProcessor);
U8 apple_i_scanKbd(Processor_65xx * pProcessor);

U8 apple_i_keyConv(U8 input);
U8 apple_i_screenConv(U8 input);

U8 apple_i_timePassed(struct timespec * pOldTime, struct timespec * pNewTime, time_t timeToPass);
U8 apple_i_readKeyboard(void);

U8 apple_i_memory[U16_MAX];

/*******************************************************************************
* Initialize Apple I emulator.
*
* Arguments:
*	pProcessor - Pointer to 65xx processor struct.
*
* Returns: -
*******************************************************************************/
void apple_i_init(Processor_65xx * pProcessor) {

	Memory_areas * pMem = calloc(1, sizeof(Memory_areas));
	pMem->RAM = malloc(U16_MAX);

	loadFile(apple_i_memory, 0xE000, "./apple_i/roms/bin/intBasic.rom", mos65xx_BIN);
	loadFile(apple_i_memory, 0xFF00, "./apple_i/roms/bin/wozmon.rom", mos65xx_BIN);
	addRAMArea(0x0000, 0x7FFF); // System & User space
	addRAMArea(KBD_ADDR, 	KBD_ADDR);		// Keyboard In
	addRAMArea(KBDCR_ADDR,	KBDCR_ADDR);	// Keyboard Control
	addRAMArea(DSP_ADDR,	DSP_ADDR);		// Display Out
	addRAMArea(DSPCR_ADDR,	DSPCR_ADDR);	// Display Control
	addROMArea(0xE000, 0xEFFF); // Integer Basic
	addROMArea(0xFF00, 0xFFFF); // Monitor

	pMem->RAM = apple_i_memory;
	pMem->ROM = NULL;

	mos65xx_init(
		pProcessor,
		pMem,
		apple_i_memRead,
		apple_i_memWrite,
		2,
		NULL
	);
}

/*******************************************************************************
* Run Apple I emulator.
*
* Arguments:
*	pProcessor - Pointer to 65xx processor struct.
*
* Returns: -
*******************************************************************************/
void apple_i_run(Processor_65xx * pProcessor) {

	struct timespec runTime, kbScanTime = {0};
	
	//term_conf();

	while (1) {
		mos65xx_addr oldPC = pProcessor->reg.PC;

		mos65xx_handleOp(pProcessor);

		if (pProcessor->reg.PC == oldPC) {

			term_deConf();
			printf("\033[m\n\n0x%04X: Infinite loop detected!\n\n", oldPC);
			break;
		} 

		apple_i_out(pProcessor);

		/* Read keyboard every 10 ms. */
		if (apple_i_timePassed(&kbScanTime, &runTime, KB_SCAN_RATE_NS)) {
			kbScanTime = runTime;
			if (apple_i_scanKbd(pProcessor)) {
				break;
			}
		}

		pProcessor->totOperations++;

		/* TODO: Sync to 1 Mhz. */
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
U8 apple_i_memRead(Processor_65xx * pProcessor, mos65xx_addr address) {
	if (address == 0xD010) {
		pProcessor->fnMemWrite(pProcessor, KBDCR_ADDR, 0);
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
void apple_i_memWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value) {
	if (!isROMAddress(address)) {
		pProcessor->pMem->RAM[address] = value;
	}
}

/*******************************************************************************
* Handles terminal style output.
*
* Arguments:
*	none
*
* Returns: Nothing.
*******************************************************************************/
void apple_i_out(Processor_65xx * pProcessor) {

	U8 chr = apple_i_screenConv(pProcessor->fnMemRead(pProcessor, DSP_ADDR) & ~0x80);

	if (chr != 0xFF) printf("%c%s", chr, chr == '\r' ? "\n" : "");

	pProcessor->fnMemWrite(pProcessor, DSP_ADDR, 0);
}

/*******************************************************************************
* Filter user input to characters that Apple I supports.
*
* Arguments:
*	input		- User input character
*
* Returns: Converted input.
*******************************************************************************/
U8 apple_i_keyConv(U8 input) {

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

/*******************************************************************************
* Filter unwanted characters from the output of Apple I.
*
* Arguments:
*	input		- User input character
*
* Returns: Converted input.
*******************************************************************************/
U8 apple_i_screenConv(U8 input) {

	if (input == '\r' || (input >= ' ' && input <= '_')) {
		// Capital letters, numbers, return and some special characters are passed through
	} else {
		input = 0xFF;
	}

	return input;
}

/*******************************************************************************
* Read and filter keyboard input.
*
* Arguments:
*	pProcessor	- 65xx processor struct.
*
* Returns: 1 if esc was pressed, otherwise 0
*******************************************************************************/
U8 apple_i_scanKbd(Processor_65xx * pProcessor) {

	U8 pressedKey = 0;

	pressedKey = apple_i_readKeyboard();
	if (pressedKey == 0x1B) { // ESC detected.
		printf(BACKSPACE);
		pressedKey = apple_i_readKeyboard();

		if (pressedKey == 0xFF || pressedKey == 0x1B) {  // ESC key pressed, quit.
			
			term_deConf();

			printf("\n\nESC key hit.\n\n");
			return 1;

		} else if (pressedKey == '[') { // CTRL character detected.
			printf(BACKSPACE); // TODO Doesn't erase ABCD from arrow keys
		} else { // Unknown ESC sequence.
			printf(BACKSPACE);
		}
	} else if (pressedKey == 0x09) { // TAB pressed (reset).
		apple_i_init(pProcessor);
	}
	
	pressedKey = apple_i_keyConv(pressedKey);

	if (pressedKey != 0xFF
	&&	!(apple_i_memRead(pProcessor, 0xD011) & 0x80)
	) {
		apple_i_memWrite(pProcessor, 0xD011, 0x80);
		apple_i_memWrite(pProcessor, 0xD010, pressedKey + 0x80);

	} else if (
		pressedKey == 0xFF
	&&	apple_i_memRead(pProcessor, 0xD011) & 0x80
	) {
		apple_i_memWrite(pProcessor, 0xD011, 0);

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
U8 apple_i_readKeyboard(void) {
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
* Checks if given amount of time has been passed between two timespecs.
*
* Arguments:
*	pOldTime		- Starting time of the comparison.
*	pNewTime		- Test time.
*	timeToPass		- Amount of nano seconds to pass.
*
* Returns: 1 if passed, otherwise 0.
*******************************************************************************/
U8 apple_i_timePassed(struct timespec * pOldTime, struct timespec * pNewTime, time_t timeToPass) {

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
