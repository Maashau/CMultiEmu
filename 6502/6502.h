#include "6502_types.h"


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

#endif

void mos6502_init(mos6502_processor_st * pProcessor);
U8 mos6502_handleOp(mos6502_processor_st * pProcessor);
void mos6502_assemble(char * asmFilePath, U8 * memory);
