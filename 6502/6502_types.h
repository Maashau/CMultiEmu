/*******************************************************************************
* MOS 6502 emulator type definitions.
*******************************************************************************/
#ifndef FILE_6502_TYPES_H
#define FILE_6502_TYPES_H
typedef unsigned char U8;
typedef signed char I8;
typedef unsigned short U16;
typedef unsigned long long U64;

typedef U16 mos6502_addr;

typedef struct {
	mos6502_addr PC;
	U8 AC;
	U8 X;
	U8 Y;
	U8 SR;
	U8 SP;
} mos6502_reg_st;

typedef U8				(*mos6502_memRead)(mos6502_addr address);
typedef void			(*mos6502_memWrite)(mos6502_addr address, U8 value);

typedef struct {
	U64	totalCycles;
	U8	currentOpCycles;
} processor_cycles_st;

typedef struct {
	mos6502_reg_st		reg;
	mos6502_memRead		fnMemRead;
	mos6502_memWrite	fnMemWrite;
	processor_cycles_st	cycles;
} mos6502_processor_st;

enum {
	SR_FLAG_C		= (1 << 0),
	SR_FLAG_Z		= (1 << 1),
	SR_FLAG_I		= (1 << 2),
	SR_FLAG_D		= (1 << 3),
	SR_FLAG_B		= (1 << 4),
	SR_FLAG_UNUSED	= (1 << 5),
	SR_FLAG_V		= (1 << 6),
	SR_FLAG_N		= (1 << 7),
};

typedef enum {
	NON = 0,	// none (JAMs)
	IMP = 1,	// implied
	ACC = 2,	// accumulator
	IMM = 3,	// immediate
	REL = 4,	// relative
	ABS = 5,	// absolute
	ABX = 6,	// absoluteXind
	ABY = 7,	// absoluteYind
	ZPG = 8,	// zeropage
	ZPX = 9,	// zeropageXind
	ZPY = 10,	// zeropageYind
	IND = 11,	// indirect
	INX = 12,	// indexedIndirect
	INY = 13 	// indirectIndexed
} mos6502_addrm;

typedef enum {
	vector_NMI		= 0xFFFA,
	vector_RESET	= 0xFFFC,
	vector_IRQBRK	= 0xFFFE
} mos6502_vector;

typedef void (*opFn)(mos6502_processor_st * processor, U8 opCodeIndex, mos6502_addrm addrm);
typedef mos6502_addr (*addrmFn)(mos6502_processor_st * processor);

typedef struct {
	opFn handler;
	char * mnemonic;
	mos6502_addrm addrm;
	U8 bytes;
} opCode_st;

#endif