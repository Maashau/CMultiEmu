#include <stdio.h>
#include <time.h>
#include "6502.h"

#define PRG_STORE_TO_MEM	0
#define PRG_CLEAR_MEM		1
#define PRG_WEEKDAY			2
#define PRG_BBL_SORT		3

#define program PRG_WEEKDAY

U8 memRead8(U16 address);
U16 memRead16(U16 address);
void memWrite8(U16 address, U8 value);
void memWrite16(U16 address, U16 value);

U8 * ram;

int main(void) {

#if program == PRG_STORE_TO_MEM // write values to memory.
	U8 memory[MOS6502_MEMSIZE] = {
		0xA9, 0x01,			// LDA
		0x8D, 0x00, 0x02,	// STA
		0xA9, 0x05,			// LDA
		0x8D, 0x01, 0x02,	// STA
		0xA9, 0x08,			// LDA
		0x8D, 0x02, 0x02,	// STA
	};

	memory[0xFFFF] = 0x20;
	memory[0xFFFE] = 0x80;

#elif program == PRG_CLEAR_MEM // Clears X amount of memory from memory address (100), Y
	U8 memory[MOS6502_MEMSIZE] = {
		0xA2,	5,			// LDX
		0xA9,	0x00,		// LDA
		0xA8,				// TAY
/*loop*/0x91,	100,		// STA
		0xC8,				// INY
		0xCA,				// DEX
		0xD0,	-6,			// BNE loop
		0
	};
	memory[255] = 0x4;
	memory[254] = 0x3;
	memory[253] = 0x2;
	memory[252] = 0x1;

	memory[100] = 252;

#elif program == PRG_WEEKDAY // Day of the week. Y = year, X = month, AC = day
	U8 memory[MOS6502_MEMSIZE] = {
		/* Y = 120 (2020), X = 3 (MAR), A = 7 -- Should result in 6 (SAT)*/
		0xA0, 0x78,
		0xA2, 0x03,
		0xA9, 0x07,
		0x20, 0x0D, 0x00,

		/* Inf loop */
		0x0,//0xEA,
		0x4C, 0x09, 0x00,

		/* Weekday */
		0xE0, 0x03,
		0xB0, 0x01,
		0x88,

		/* Handle march */
		0x49, 0x7F,
		0xC0, 0xC8,
		0x7D, 0x31, 0x00,
		0x8D, 0x40, 0x00,
		0x98,
		0x20, 0x2D, 0x00,
		0xED, 0x40, 0x00,
		0x8D, 0x40, 0x00,
		0x98,
		0x4A,
		0x4A,
		0x18,
		0x6D, 0x40, 0x00,

		/* Modulo 7 */
		0x69, 0x07,
		0x90, 0xFC,

		/* RTS */
		0x60,

		/* Storage */
		0x01, 0x05, 0x06, 0x03, 0x01, 0x05, 0x03, 0x00, 0x04, 0x02, 0x06, 0x04
	};
	printf("Calculating weekday (from 1 to 7) for %d.%d.%d\n\n", memory[5], memory[3], memory[1] + 1900);

#elif program == PRG_BBL_SORT
	U8 memory[MOS6502_MEMSIZE] = {
		/* Sort 8 */
		0xA0, 0x00,
		0x84, 0x32,
		0xB1, 0x30,
		0xAA,
		0xC8,
		0xCA,

		/* Next element */
		0xB1, 0x30,
		0xC8,
		0xD1, 0x30,
		0x90, 0x10,
		0xF0, 0x0E,
		0x48,
		0xB1, 0x30,
		0x88,
		0x91, 0x30,
		0x68,
		0xC8,
		0x91, 0x30,
		0xA9, 0xFF,
		0x85, 0x32,

		/* Check end of the list? */
		0xCA,
		0xD0, 0xE6,
		0x24, 0x32,
		0x30, 0xD9,
		0x60
	};

	memory[0x30] = 0xE0;
	memory[0x31] = 0x00;

	memory[0xE0] = 3;
	memory[0xE1] = 2;
	memory[0xE2] = 1;
	memory[0xE3] = 3;

	printf("Starting order:\n");
	for (int ii = 0; ii < memory[0xE0]; ii++) {
		printf("\t%d: %d\n", ii + 1, memory[0xE0 + 1 + ii]);
	}
	printf("\n");
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

#elif program == PRG_BBL_SORT

	printf("Final order:\n");
	for (int ii = 0; ii < memory[0xE0]; ii++) {
		printf("\t%d: %d\n", ii + 1, memory[0xE0 + 1 + ii]);
	}
	printf("\n");

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


U8 memRead8(U16 address) {
	return ram[address];
}
U16 memRead16(U16 address) {
	return ram[address] | (ram[address + 1] << 8);
}
void memWrite8(U16 address, U8 value) {
	ram[address] = value;
}
void memWrite16(U16 address, U16 value) {
	ram[address] = (U8)value;
	ram[address + 1] = (U8)(value >> 8);
}