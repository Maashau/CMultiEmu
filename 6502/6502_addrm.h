#include "6502.h"

typedef U8 mos6502_imm;
typedef I8 mos6502_rel;


mos6502_imm		addrm_immediate(mos6502_processor_st * processor);
mos6502_rel		addrm_relative(mos6502_processor_st * processor);
mos6502_addr	addrm_absolute(mos6502_processor_st * processor);
mos6502_addr	addrm_absoluteXind(mos6502_processor_st * processor);
mos6502_addr	addrm_absoluteYind(mos6502_processor_st * processor);
mos6502_addr	addrm_zeropage(mos6502_processor_st * processor);
mos6502_addr	addrm_zeropageXind(mos6502_processor_st * processor);
mos6502_addr	addrm_zeropageYind(mos6502_processor_st * processor);
mos6502_addr	addrm_indirect(mos6502_processor_st * processor);
mos6502_addr	addrm_indexedIndirect(mos6502_processor_st * processor);
mos6502_addr	addrm_indirectIndexed(mos6502_processor_st * processor);

U8 addrm_stackPop8(mos6502_processor_st * processor);
U16 addrm_stackPop16(mos6502_processor_st * processor);
void addrm_stackPush8(mos6502_processor_st * processor, U8 value);
void addrm_stackPush16(mos6502_processor_st * processor, U16 value);
