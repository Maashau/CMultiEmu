/*******************************************************************************
* MOS 6502 emulator main header file.
*******************************************************************************/
#include "6502_types.h"

/*******************************************************************************
* Macros and definitions
*******************************************************************************/

#define PRINT_DBG_INFO 1

#if PRINT_DBG_INFO == 1
# define DBG_PRINT(_x) _x
#else
# define DBG_PRINT(_x)
#endif

#define U16_MAX 65535U
#define MOS6502_MEMSIZE (U16_MAX + 1)

#define MOS6502_OUTOFPAGE(_pc, _address) ({int retval; retval = (_pc / 0xFF != _address / 0xFF) ? 1 : 0; retval;})

/*******************************************************************************
* Public functions
*******************************************************************************/
void mos6502_init(mos6502_processor_st * pProcessor);
U8 mos6502_handleOp(mos6502_processor_st * pProcessor);
void mos6502_assemble(char * asmFilePath, U8 * memory);
