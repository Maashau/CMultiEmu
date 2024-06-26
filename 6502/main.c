#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "6502.h"

#define PRG_STORE_TO_MEM	0
#define PRG_CLEAR_MEM		1
#define PRG_WEEKDAY			2

#define program PRG_WEEKDAY

typedef enum {
	mos6502_BIN,
	mos6502_HEX,
	mos6502_ASM
} mos6502_fileType;

U8 memRead8(U16 address);
U16 memRead16(U16 address);
void memWrite8(U16 address, U8 value);
void memWrite16(U16 address, U16 value);

void loadFile(U8 * memory, const char * path, mos6502_fileType fileType);

U8 * ram;

/*******************************************************************************
* Handles loading and running the program.
*
* Returns: -
*******************************************************************************/
int main(void) {

	U8 memory[MOS6502_MEMSIZE];

	//mos6502_assemble("./prg/asm/32bitdiv.6502asm", ram);

#if program == PRG_STORE_TO_MEM // write values to memory.

	loadFile(memory, "./prg/bin/memWrite.6502", mos6502_BIN);

#elif program == PRG_CLEAR_MEM // Clears X amount of memory from memory address (100), Y

	loadFile(memory, "./prg/bin/memClear.6502", mos6502_BIN);

#elif program == PRG_WEEKDAY // Day of the week. Y = year, X = month, AC = day

	loadFile(memory, "./prg/bin/weekday.6502", mos6502_BIN);

	printf("Calculating weekday (from 1 to 7) for %d.%d.%d\n\n", memory[5], memory[3], memory[1] + 1900);

#endif

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

	while (retVal != 0xFF) {
		retVal = mos6502_handleOp(&processor);

		if (retVal == 0xFF) {
			break;
		}

		totalOperations++;

		//getc(stdin);
	}
	clock_gettime(CLOCK_REALTIME, &eT);

#if program == PRG_WEEKDAY

	printf("Answer: %d\n\n", processor.reg.AC);

#endif

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
	return ram[address];
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
	return ram[address] | (ram[address + 1] << 8);
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
	ram[address] = value;
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
	ram[address] = (U8)value;
	ram[address + 1] = (U8)(value >> 8);
}