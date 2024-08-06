#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "6502.h"
// Non-blocking fgetc()
#include <fcntl.h>

#define PRG_AS_ARGUMENT			0
#define PRG_STORE_TO_MEM		1
#define PRG_CLEAR_MEM			2
#define PRG_WEEKDAY				3
#define PRG_count_impl_opc		4
#define PRG_count_impl_leg_opc	5
#define PRG_ASSEMBLE			6
#define PRG_KEYBOARD			7

#define program PRG_AS_ARGUMENT

typedef enum {
	mos6502_BIN,
	mos6502_HEX,
	mos6502_ASM
} mos6502_fileType;

mos6502_addr keyboardAddr = 0xFFF0;

U8 memRead8(U16 address);
U16 memRead16(U16 address);
void memWrite8(U16 address, U8 value);
void memWrite16(U16 address, U16 value);

U8 readKeyboard();

void memDummyWrite8(mos6502_addr address, U8 value) { return; }
void memDummyWrite16(mos6502_addr address, U16 value) { return; }

void loadFile(U8 * memory, const char * path, mos6502_fileType fileType);

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
		printf("Calculating weekday (from 1 to 7) for %d.%d.%d\n\n", memory[5], memory[3], memory[1] + 1900);
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
	}

	ram = memory;

	struct timespec sT, eT;
	unsigned long totalCycles = 0;
	unsigned long totalOperations = 0;
	U8 retVal = 0;

	mos6502_processor_st processor;

	processor.memIf.read8 = memRead8;
	processor.memIf.read16 = memRead16;
	processor.memIf.write8 = memWrite8;
	processor.memIf.write16 = memWrite16;

	mos6502_init(&processor);

	clock_gettime(CLOCK_REALTIME, &sT);

	if (selected_program == PRG_count_impl_opc || selected_program == PRG_count_impl_leg_opc) {
		U8 count = 0;

		processor.memIf.write8 = memDummyWrite8;
		processor.memIf.write16 = memDummyWrite16;

		for (int ii = 0; ii < opCount; ii++) {
			processor.reg.PC = ii;
			retVal = mos6502_handleOp(&processor);

			if (retVal != 0xFF) {
				count++;
			}
		}

		printf("\nCount of implemented ops: %d/%d (%d\%)\n\n", count, opCount, (count * 100) / opCount);

	} else {

		while (retVal != 0xFF) {
			retVal = mos6502_handleOp(&processor);

			if (retVal == 0xFF) {
				break;
			}

			totalOperations++;

			//getc(stdin);
		}
		clock_gettime(CLOCK_REALTIME, &eT);
	}


	if (selected_program == PRG_WEEKDAY) {
		printf("Answer: %d\n\n", processor.reg.AC);
	} else if (selected_program == PRG_KEYBOARD) {
		printf("String entered: %s\n\n", &memory[0x50]);
	}


	time_t secsPassed, nsecsPassed = 0;

	secsPassed = eT.tv_sec - sT.tv_sec;

	if (secsPassed != 0 && (eT.tv_nsec < sT.tv_nsec)) {
		secsPassed--;
	}

	if (eT.tv_nsec < sT.tv_nsec) {
		nsecsPassed = 100000000 - (sT.tv_nsec + eT.tv_nsec);
	} else {
		nsecsPassed = eT.tv_nsec - sT.tv_nsec;
	}

	printf(
		"Operations performed: %d\n\n Start time: %ds %dms %dns\n   End time: %ds %dms %dns\n\n Difference: %ds %dms %dns\n\nCycle count: %llu\n\n",
		totalOperations,
		0,
		sT.tv_nsec / 1000000,
		sT.tv_nsec - (sT.tv_nsec / 1000000 * 1000000),
		eT.tv_sec - sT.tv_sec,
		eT.tv_nsec / 1000000,
		eT.tv_nsec - (eT.tv_nsec / 1000000 * 1000000),
		secsPassed,
		nsecsPassed / 1000000,
		nsecsPassed - (nsecsPassed / 1000000 * 1000000),
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
U8 memRead8(mos6502_addr address) {
	if (address == keyboardAddr) {
		return readKeyboard();
	} else {
		return ram[address];
	}
}

/*******************************************************************************
* Reads an address(word) from the memory.
*
* Arguments:
*	address - Memory address to read from.
*
* Returns: Address read from the memory.
*******************************************************************************/
mos6502_addr memRead16(mos6502_addr address) {
	if (address == keyboardAddr) {
		return (memRead8(address + 1) << 8) | readKeyboard();
	} else if (address == keyboardAddr - 1) {
		return (readKeyboard() << 8) | memRead8(address + 1);
	} else {
		return memRead8(address) | (memRead8(address + 1) << 8);
	}
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
void memWrite8(mos6502_addr address, U8 value) {
	if (address != keyboardAddr) {
		ram[address] = value;
	}
}

/*******************************************************************************
* Writes an address(word) to the memory.
*
* Arguments:
*	address - Memory address to write to.
*	value	- Address to be written to the memory.
*
* Returns: Nothing.
*******************************************************************************/
void memWrite16(mos6502_addr address, U16 value) {
	if (address == keyboardAddr) {
		memWrite8(address + 1, (U8)(value >> 8));
	} else if (address == keyboardAddr - 1) {
		memWrite8(address, (U8)value);
	} else {
		memWrite8(address, (U8)value);
		memWrite8(address + 1, (U8)(value >> 8));
	}
}

U8 readKeyboard() {
	U8 key;
	system("/bin/stty raw");
	fcntl(0, F_SETFL, O_NONBLOCK);
	key = fgetc(stdin);

	while(1) {

		U8 tempKey = fgetc(stdin);
		if (tempKey != key || tempKey == 0xFF) {
			break;
		}
	}
	system("/bin/stty cooked");
	return key;
}