/*******************************************************************************
* MOS 65xx emulator type definitions.
*******************************************************************************/
#ifndef FILE_65xx_TYPES_H
#define FILE_65xx_TYPES_H
typedef unsigned char U8;
typedef signed char I8;
typedef unsigned short U16;
typedef unsigned long U32;
typedef unsigned long long U64;

typedef U16 mos65xx_addr;

typedef struct {
	mos65xx_addr PC;
	U8 AC;
	U8 X;
	U8 Y;
	U8 SR;
	U8 SP;
} mos65xx_reg_st;

typedef struct {
	U64	totalCycles;
	U8	currentOp;
} processor_cycles_st;

typedef struct Processor_65xx_st Processor_65xx;

typedef U8				(*mos65xx_memRead)(Processor_65xx * pProcessor, mos65xx_addr address);
typedef void			(*mos65xx_memWrite)(Processor_65xx * pProcessor, mos65xx_addr address, U8 value);

typedef struct {
	U8 * ROM;
	U8 * RAM;
	U8 * IO;
} Memory_areas;

typedef struct Processor_65xx_st {
	mos65xx_reg_st		reg;
	mos65xx_memRead		fnMemRead;
	mos65xx_memWrite	fnMemWrite;
	processor_cycles_st	cycles;
	size_t				totOperations;
	Memory_areas *		pMem;
	void *				pUtil;
	U8					lastOp;
	U8					interrupt;
	U8					debugLevel;
	void *				event;
} Processor_65xx;

typedef enum {
	mos65xx_BIN,
	mos65xx_HEX,
	mos65xx_ASM
} mos65xx_fileType;

typedef struct {
	mos65xx_addr startAddr;
	mos65xx_addr endAddr;
} address_range;

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
} mos65xx_addrm;

typedef enum {
	vector_NMI		= 0xFFFA,
	vector_RESET	= 0xFFFC,
	vector_IRQBRK	= 0xFFFE
} mos65xx_vector;

typedef void (*opFn)(Processor_65xx * processor, U8 opCodeIndex, mos65xx_addrm addrm);
typedef mos65xx_addr (*addrmFn)(Processor_65xx * processor);

typedef struct {
	opFn handler;
	char * mnemonic;
	mos65xx_addrm addrm;
	U8 bytes;
} opCode_st;

#endif