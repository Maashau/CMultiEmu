/*******************************************************************************
* MOS 65xx emulator op-code handler header.
*******************************************************************************/
#include "65xx_types.h"

/*******************************************************************************
* Function prototypes.
*******************************************************************************/
void mos65xx_ADC(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_ALR(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_ANC(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_AND(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_ANE(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_ARR(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_ASL(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_BCC(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_BCS(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_BEQ(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_BIT(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_BMI(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_BNE(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_BPL(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_BRK(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_BVC(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_BVS(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_CLC(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_CLD(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_CLI(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_CLV(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_CMP(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_CPX(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_CPY(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_DCP(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_DEC(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_DEX(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_DEY(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_EOR(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_INC(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_INX(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_INY(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_ISC(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_JAM(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_JMP(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_JSR(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_LAS(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_LAX(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_LDA(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_LDX(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_LDY(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_LSR(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_LXA(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_NOP(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_ORA(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_PHA(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_PHP(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_PLA(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_PLP(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_RLA(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_ROL(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_ROR(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_RRA(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_RTI(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_RTS(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_SAX(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_SBC(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_SBX(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_SEC(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_SED(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_SEI(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_SHA(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_SHX(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_SHY(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_SLO(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_SRE(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_STA(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_STX(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_STY(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_TAS(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_TAX(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_TAY(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_TSX(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_TXA(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_TXS(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_TYA(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);
void mos65xx_USBC(Processor_65xx * processor, U8 opCode, mos65xx_addrm addrm);

void mos65xx_IRQ(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm);
