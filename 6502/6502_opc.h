#include "6502.h"

#ifndef FILE_6502_OPC_H
#define FILE_6502_OPC_H


typedef struct {
	const char * mnemonic;
	U8 bytes;
	U8 cycles;
} opCode__st;

typedef U8 (*opFn)(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);

typedef struct {
	opFn handler;
    opCode__st opCode;
} opCode_st;

U8 mos6502_ADC(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_ALR(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_ANC(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_AND(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_ANE(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_ARR(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_ASL(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_BCC(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_BCS(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_BEQ(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_BIT(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_BMI(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_BNE(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_BPL(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_BRK(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_BVC(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_BVS(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_CLC(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_CLD(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_CLI(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_CLV(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_CMP(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_CPX(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_CPY(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_DCP(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_DEC(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_DEX(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_DEY(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_EOR(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_INC(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_INX(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_INY(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_ISC(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_JAM(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_JMP(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_JSR(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_LAS(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_LAX(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_LDA(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_LDX(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_LDY(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_LSR(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_LXA(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_NOP(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_ORA(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_PHA(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_PHP(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_PLA(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_PLP(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_RLA(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_ROL(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_ROR(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_RRA(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_RTI(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_RTS(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_SAX(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_SBC(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_SBX(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_SEC(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_SED(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_SEI(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_SHA(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_SHX(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_SHY(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_SLO(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_SRE(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_STA(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_STX(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_STY(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_TAS(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_TAX(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_TAY(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_TSX(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_TXA(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_TXS(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_TYA(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);
U8 mos6502_USBC(mos6502_processor_st * processor, U8 opCodeIndex, opCode__st * opCode);

#endif