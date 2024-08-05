/*******************************************************************************
* MOS 6502 emulator addressing mode handler header.
*******************************************************************************/
#include "6502_types.h"

/*******************************************************************************
* Function prototypes.
*******************************************************************************/
mos6502_addr addrm_imm(mos6502_processor_st * processor);
mos6502_addr addrm_rel(mos6502_processor_st * processor);
mos6502_addr addrm_abs(mos6502_processor_st * processor);
mos6502_addr addrm_absX(mos6502_processor_st * processor);
mos6502_addr addrm_absY(mos6502_processor_st * processor);
mos6502_addr addrm_zpg(mos6502_processor_st * processor);
mos6502_addr addrm_zpgX(mos6502_processor_st * processor);
mos6502_addr addrm_zpgY(mos6502_processor_st * processor);
mos6502_addr addrm_ind(mos6502_processor_st * processor);
mos6502_addr addrm_xInd(mos6502_processor_st * processor);
mos6502_addr addrm_indY(mos6502_processor_st * processor);

U8 addrm_stackPop8(mos6502_processor_st * processor);
U16 addrm_stackPop16(mos6502_processor_st * processor);
void addrm_stackPush8(mos6502_processor_st * processor, U8 value);
void addrm_stackPush16(mos6502_processor_st * processor, U16 value);
