
#ifndef FILE_6502_H
#define FILE_6502_H
#define PRINT_OPS 1

#if PRINT_OPS == 1
# define OP_PRINT(_x) _x
#else
# define OP_PRINT(_x)
#endif

#define U16_MAX 65535U
#define MOS6502_MEMSIZE (U16_MAX + 1)

#define MOS6502_OUTOFPAGE(_pc, _address) ({int retval; retval = (_pc / 0xFF != _address / 0xFF) ? 1 : 0; retval;})

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

typedef U8 (*mos6502_memRead8)(U16 address);
typedef U16 (*mos6502_memRead16)(U16 address);
typedef void (*mos6502_memWrite8)(U16 address, U8 value);
typedef void (*mos6502_memWrite16)(U16 address, U16 value);

typedef struct {
	mos6502_memRead8	read8;
	mos6502_memRead16	read16;
	mos6502_memWrite8	write8;
	mos6502_memWrite16	write16;
} mos6502_memInterface_st;

typedef struct {
	mos6502_reg_st reg;
	mos6502_memInterface_st memIf;
	U64 cycleCount;
} mos6502_processor_st;

enum {
	SR_FLAG_CARRY		= (1 << 0),
	SR_FLAG_ZERO		= (1 << 1),
	SR_FLAG_IRQ			= (1 << 2),
	SR_FLAG_DECIMAL		= (1 << 3),
	SR_FLAG_BREAK		= (1 << 4),
	SR_FLAG_UNUSED		= (1 << 5),
	SR_FLAG_OVERFLOW	= (1 << 6),
	SR_FLAG_NEGATIVE	= (1 << 7),
};

void mos6502_init(mos6502_processor_st * processor);
U8 mos6502_handleOp(mos6502_processor_st * processor);
#endif