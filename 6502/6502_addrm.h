/*******************************************************************************
* MOS 6502 emulator addressing mode handler header.
*******************************************************************************/
#include "6502_types.h"

/*******************************************************************************
* Function prototypes.
*******************************************************************************/
mos6502_addr addrm_IMM(mos6502_processor_st * processor);
mos6502_addr addrm_REL(mos6502_processor_st * processor);
mos6502_addr addrm_ABS(mos6502_processor_st * processor);
mos6502_addr addrm_ABX(mos6502_processor_st * processor);
mos6502_addr addrm_ABY(mos6502_processor_st * processor);
mos6502_addr addrm_ZPG(mos6502_processor_st * processor);
mos6502_addr addrm_ZPX(mos6502_processor_st * processor);
mos6502_addr addrm_ZPY(mos6502_processor_st * processor);
mos6502_addr addrm_IND(mos6502_processor_st * processor);
mos6502_addr addrm_INX(mos6502_processor_st * processor);
mos6502_addr addrm_INY(mos6502_processor_st * processor);

U8 addrm_stackPop8(mos6502_processor_st * processor);
U16 addrm_stackPop16(mos6502_processor_st * processor);
void addrm_stackPush8(mos6502_processor_st * processor, U8 value);
void addrm_stackPush16(mos6502_processor_st * processor, U16 value);

U8 addrm_read8(mos6502_processor_st * pProcessor, mos6502_addr address);
U16 addrm_read16(mos6502_processor_st * pProcessor, mos6502_addr address);
void addrm_write8(mos6502_processor_st * pProcessor, mos6502_addr address, U8 value);
void addrm_write16(mos6502_processor_st * pProcessor, mos6502_addr address, U16 value);