/*******************************************************************************
* 6502_opc.c
*
* 6502 instruction implementations.
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "6502.h"
//#include "6502_opc.h"
#include "6502_addrm.h"

/*******************************************************************************
* Macros and definitions.
*******************************************************************************/
#define SR_GET_N() (pProcessor->reg.SR & SR_FLAG_N) ? 1 : 0
#define SR_GET_Z() (pProcessor->reg.SR & SR_FLAG_Z) ? 1 : 0
#define SR_GET_C() (pProcessor->reg.SR & SR_FLAG_C) ? 1 : 0
#define SR_GET_I() (pProcessor->reg.SR & SR_FLAG_I) ? 1 : 0
#define SR_GET_V() (pProcessor->reg.SR & SR_FLAG_V) ? 1 : 0
#define SR_GET_D() (pProcessor->reg.SR & SR_FLAG_D) ? 1 : 0
#define SR_GET_B() (pProcessor->reg.SR & SR_FLAG_B) ? 1 : 0

#define SR_SET_N() (pProcessor->reg.SR |= SR_FLAG_N)
#define SR_SET_Z() (pProcessor->reg.SR |= SR_FLAG_Z)
#define SR_SET_C() (pProcessor->reg.SR |= SR_FLAG_C)
#define SR_SET_I() (pProcessor->reg.SR |= SR_FLAG_I)
#define SR_SET_V() (pProcessor->reg.SR |= SR_FLAG_V)
#define SR_SET_D() (pProcessor->reg.SR |= SR_FLAG_D)
#define SR_SET_B() (pProcessor->reg.SR |= SR_FLAG_B)

#define SR_RESET_N() (pProcessor->reg.SR &= ~SR_FLAG_N)
#define SR_RESET_Z() (pProcessor->reg.SR &= ~SR_FLAG_Z)
#define SR_RESET_C() (pProcessor->reg.SR &= ~SR_FLAG_C)
#define SR_RESET_I() (pProcessor->reg.SR &= ~SR_FLAG_I)
#define SR_RESET_V() (pProcessor->reg.SR &= ~SR_FLAG_V)
#define SR_RESET_D() (pProcessor->reg.SR &= ~SR_FLAG_D)
#define SR_RESET_B() (pProcessor->reg.SR &= ~SR_FLAG_B)

#define SR_COND_SET_N(_testValue) (_testValue) ? SR_SET_N() : SR_RESET_N()
#define SR_COND_SET_Z(_testValue) (_testValue) ? SR_SET_Z() : SR_RESET_Z()
#define SR_COND_SET_C(_testValue) (_testValue) ? SR_SET_C() : SR_RESET_C()
#define SR_COND_SET_I(_testValue) (_testValue) ? SR_SET_I() : SR_RESET_I()
#define SR_COND_SET_V(_testValue) (_testValue) ? SR_SET_V() : SR_RESET_V()
#define SR_COND_SET_D(_testValue) (_testValue) ? SR_SET_D() : SR_RESET_D()
#define SR_COND_SET_B(_testValue) (_testValue) ? SR_SET_B() : SR_RESET_B()

/*******************************************************************************
* Add Memory to Accumulator with Carry.
*
* A + M + C -> A, C
*
* N Z C I D V
* + + + - - +
*******************************************************************************/
U8 mos6502_ADC(mos6502_processor_st * pProcessor, U8 opCode) {
	mos6502_addr address;
	U16 oldPC = pProcessor->reg.PC;
	U8 cyclesPassed = 0;
	U8 memValue = 0;
	U8 tempAc = pProcessor->reg.AC;

	// TODO implement decimal mode (BCD)

	switch (opCode) {
		case 0x69: // immediate
			address = addrm_imm(pProcessor);
			cyclesPassed = 2;
			break;
		case 0x65: // zeropage
			address = addrm_zpg(pProcessor);
			cyclesPassed = 3;
			break;
		case 0x75: // zeropage,X
			address = addrm_zpgX(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x6D: // absolute
			address = addrm_abs(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x7D: // absolute,X
			address = addrm_absX(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x79: // absolute,Y
			address = addrm_absY(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x61: // (indirect,X)
			address = addrm_xInd(pProcessor);
			cyclesPassed = 6;
			break;
		case 0x71: // (indirect),Y
			address = addrm_indY(pProcessor);
			cyclesPassed = 5;
			break;
		default:
			return 0xFF;
	}

	memValue = addrm_read8(pProcessor, address);
	pProcessor->reg.AC += memValue;
	pProcessor->reg.AC += SR_GET_C();
	
	switch (opCode)	{
		case 0x7D:
		case 0x79:
		case 0x71:
			cyclesPassed += MOS6502_OUTOFPAGE(oldPC, address);
			break;
		default:
			break;
	}

	SR_COND_SET_V(
		!((tempAc & 0x80) ^ (memValue & 0x80)) // Both values have same sign
	&&	(pProcessor->reg.AC & 0x80) ^ (memValue & 0x80) // Output signedness differs from values.
	);

	SR_COND_SET_C(((U16)tempAc + (U16)memValue) > 0xFF);
	SR_COND_SET_N(pProcessor->reg.AC & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.AC == 0);

return cyclesPassed;
}

/*******************************************************************************
* AND oper + LSR
*
* A AND oper, 0 -> [76543210] -> C
*
* N Z C I D V
* + + + - - -
*******************************************************************************/
U8 mos6502_ALR(mos6502_processor_st * pProcessor, U8 opCode) {

	pProcessor->reg.AC &= addrm_read8(pProcessor, addrm_imm(pProcessor));

	SR_COND_SET_C(pProcessor->reg.AC & SR_FLAG_C);

	pProcessor->reg.AC >>= 1;

	SR_COND_SET_Z(pProcessor->reg.AC == 0);
	SR_RESET_N();

	return 2;
}

/*******************************************************************************
* AND oper + set C as ASL
*
* A AND oper, bit(7) -> C
*
* N Z C I D V
* + + + - - -
*******************************************************************************/
U8 mos6502_ANC(mos6502_processor_st * pProcessor, U8 opCode) {

	pProcessor->reg.AC &= addrm_read8(pProcessor, addrm_imm(pProcessor));

	SR_COND_SET_Z(pProcessor->reg.AC == 0);
	SR_COND_SET_N(pProcessor->reg.AC & SR_FLAG_N);
	SR_COND_SET_C(pProcessor->reg.AC & SR_FLAG_N);

	return 2;
}

/*******************************************************************************
* AND Memory with Accumulator
*
* A AND M -> A
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_AND(mos6502_processor_st * pProcessor, U8 opCode) {
	mos6502_addr address;
	mos6502_addr oldPC = pProcessor->reg.PC;
	U8 cyclesPassed = 0;

	switch (opCode) {
		case 0x29: //immediate
			address = addrm_imm(pProcessor);
			cyclesPassed = 2;
			break;
		case 0x25: //zeropage
			address = addrm_zpg(pProcessor);
			cyclesPassed = 3;
			break;
		case 0x35: //zeropage,X
			address = addrm_zpgX(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x2D: //absolute
			address = addrm_abs(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x3D: //absolute,X
			address = addrm_absX(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x39: //absolute,Y
			address = addrm_absY(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x21: //(indirect,X)
			address = addrm_xInd(pProcessor);
			cyclesPassed = 6;
			break;
		case 0x31: //(indirect),Y
			address = addrm_indY(pProcessor);
			cyclesPassed = 5;
			break;
		default:
			return 0xFF;
	}

	switch (opCode)
	{
		case 0x3D:
		case 0x39:
		case 0x31:
			cyclesPassed += MOS6502_OUTOFPAGE(oldPC, address);
			break;
		default:
			break;
	}

	pProcessor->reg.AC = pProcessor->reg.AC & addrm_read8(pProcessor, address);

	SR_COND_SET_N(pProcessor->reg.AC & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.AC == 0);

	return cyclesPassed;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
U8 mos6502_ANE(mos6502_processor_st * pProcessor, U8 opCode) {
	return 0xFF;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
U8 mos6502_ARR(mos6502_processor_st * pProcessor, U8 opCode) {
	return 0xFF;
}

/*******************************************************************************
* Shift Left One Bit (Memory or Accumulator)
*
* C <- [76543210] <- 0
*
* N Z C I D V
* + + + - - -
*******************************************************************************/
U8 mos6502_ASL(mos6502_processor_st * pProcessor, U8 opCode) {
	mos6502_addr address;
	U8 cyclesPassed = 0;
	U8 origVal = 0;
	U8 shiftedVal = 0;

	switch (opCode) {
		case 0x0A: // accumulator
			origVal = pProcessor->reg.AC;
			cyclesPassed = 2;
			break;
		case 0x06: // zeropage
			address = addrm_zpg(pProcessor);
			origVal = addrm_read8(pProcessor, address);
			cyclesPassed = 5;
			break;
		case 0x16: // zeropage,X
			address = addrm_zpgX(pProcessor);
			origVal = addrm_read8(pProcessor, address);
			cyclesPassed = 6;
			break;
		case 0x0E: // absolute
			address = addrm_abs(pProcessor);
			origVal = addrm_read8(pProcessor, address);
			cyclesPassed = 6;
			break;
		case 0x1E: // absolute,X
			address = addrm_absX(pProcessor);
			origVal = addrm_read8(pProcessor, address);
			cyclesPassed = 7;
			break;
		default:
			return 0xFF;
	}

	shiftedVal = origVal << 1;

	if (opCode == 0x0A) {
		pProcessor->reg.AC = shiftedVal;
	} else {
		addrm_write8(pProcessor, address, shiftedVal);
	}

	SR_COND_SET_N(shiftedVal & SR_FLAG_N);
	SR_COND_SET_C(((U16)origVal << 1) > 0xFF);
	SR_COND_SET_Z(shiftedVal == 0);

	return cyclesPassed;
}

/*******************************************************************************
* Branch on Carry Clear
*
* branch on C = 0
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_BCC(mos6502_processor_st * pProcessor, U8 opCode) {

	U16 oldPC = pProcessor->reg.PC;
	U8 cyclesPassed = 2;
	mos6502_addr branchAddress = addrm_rel(pProcessor);

	if (!SR_GET_C())  {

		cyclesPassed += 1 + MOS6502_OUTOFPAGE(oldPC, branchAddress);
		
		pProcessor->reg.PC = branchAddress;
	}

	return cyclesPassed;
}

/*******************************************************************************
* Branch on Carry Set
*
* branch on C = 1
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_BCS(mos6502_processor_st * pProcessor, U8 opCode) {

	U16 oldPC = pProcessor->reg.PC;
	U8 cyclesPassed = 2;
	mos6502_addr branchAddress = addrm_rel(pProcessor);

	if (SR_GET_C()) {

		cyclesPassed += 1 + MOS6502_OUTOFPAGE(oldPC, branchAddress);
		
		pProcessor->reg.PC = branchAddress;
	}

	return cyclesPassed;
}

/*******************************************************************************
* Branch on Result Zero
*
* branch on Z = 1
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_BEQ(mos6502_processor_st * pProcessor, U8 opCode) {

	U16 oldPC = pProcessor->reg.PC;
	U8 cyclesPassed = 2;
	mos6502_addr branchAddress = addrm_rel(pProcessor);

	if (SR_GET_Z()) {

		cyclesPassed += 1 + MOS6502_OUTOFPAGE(oldPC, branchAddress);
		
		pProcessor->reg.PC = branchAddress;
	}

	return cyclesPassed;
}

/*******************************************************************************
* Test Bits in Memory with Accumulator
*
* A AND M -> Z, M7 -> N, M6 -> V
*
* N  Z C I D V
* M7 + - - - M6
*******************************************************************************/
U8 mos6502_BIT(mos6502_processor_st * pProcessor, U8 opCode) {

	U8 cyclesPassed = 0;
	U8 memValue = 0;
	mos6502_addr address = 0;

	switch (opCode) {
		case 0x24: // zeropage
			address = addrm_zpg(pProcessor);
			cyclesPassed = 3;
			break;
		case 0x2C: // absolute
			address = addrm_abs(pProcessor);
			cyclesPassed = 4;
			break;
		default:
			return 0xFF;
	}

	memValue = addrm_read8(pProcessor, address);

	SR_COND_SET_N(memValue & SR_FLAG_N);
	SR_COND_SET_V(memValue & SR_FLAG_V);
	SR_COND_SET_Z((memValue & pProcessor->reg.AC) == 0);

	return cyclesPassed;
}

/*******************************************************************************
* Branch on Result Minus
*
* branch on N = 1
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_BMI(mos6502_processor_st * pProcessor, U8 opCode) {

	U16 oldPC = pProcessor->reg.PC;
	U8 cyclesPassed = 2;
	mos6502_addr branchAddress = addrm_rel(pProcessor);

	if (SR_GET_N()) {

		cyclesPassed += 1 + MOS6502_OUTOFPAGE(oldPC, branchAddress);
		
		pProcessor->reg.PC = branchAddress;
	}

	return cyclesPassed;
}

/*******************************************************************************
* Branch on Result not Zero
*
* branch on Z = 0
*******************************************************************************/
U8 mos6502_BNE(mos6502_processor_st * pProcessor, U8 opCode) {

	U16 oldPC = pProcessor->reg.PC;
	U8 cyclesPassed = 2;
	mos6502_addr branchAddress = addrm_rel(pProcessor);

	if (!SR_GET_Z()) {

		cyclesPassed += 1 + MOS6502_OUTOFPAGE(oldPC, branchAddress);
		
		pProcessor->reg.PC = branchAddress;
	}

	return cyclesPassed;
}

/*******************************************************************************
* Branch on Result Plus
*
* branch on N = 0
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_BPL(mos6502_processor_st * pProcessor, U8 opCode) {

	U16 oldPC = pProcessor->reg.PC;
	U8 cyclesPassed = 2;
	mos6502_addr branchAddress = addrm_rel(pProcessor);

	if (!SR_GET_N()) {

		cyclesPassed += 1 + MOS6502_OUTOFPAGE(oldPC, branchAddress);
		
		pProcessor->reg.PC = branchAddress;
	}

	return cyclesPassed;
}

/*******************************************************************************
* Breakpoint
*
*
*
*
*
*******************************************************************************/
U8 mos6502_BRK(mos6502_processor_st * pProcessor, U8 opCode) {

	U8 cyclesPassed = 7;

// TODO use addrm functions
	U16 address = addrm_read16(pProcessor, 0xFFFE);

	// TODO

	return 0xff;

	return cyclesPassed;
}

/*******************************************************************************
* Branch on Overflow Clear
*
* branch on V = 0
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_BVC(mos6502_processor_st * pProcessor, U8 opCode) {

	U16 oldPC = pProcessor->reg.PC;
	U8 cyclesPassed = 2;
	mos6502_addr branchAddress = addrm_rel(pProcessor);

	if (!SR_GET_V()) {

		cyclesPassed += 1 + MOS6502_OUTOFPAGE(oldPC, branchAddress);
		
		pProcessor->reg.PC = branchAddress;
	}

	return cyclesPassed;
}

/*******************************************************************************
* Branch on Overflow Set
*
* branch on V = 1
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_BVS(mos6502_processor_st * pProcessor, U8 opCode) {

	U16 oldPC = pProcessor->reg.PC;
	U8 cyclesPassed = 2;
	mos6502_addr branchAddress = addrm_rel(pProcessor);

	if (SR_GET_V()) {

		cyclesPassed += 1 + MOS6502_OUTOFPAGE(oldPC, branchAddress);
		
		pProcessor->reg.PC = branchAddress;
	}

	return cyclesPassed;
}

/*******************************************************************************
* Clear Carry Flag
*
* 0 -> C
*
* N Z C I D V
* - - 0 - - -
*******************************************************************************/
U8 mos6502_CLC(mos6502_processor_st * pProcessor, U8 opCode) {

	SR_RESET_C();

	return 2;
}

/*******************************************************************************
* Clear Decimal Mode
*
* 0 -> D
*
* N Z C I D V
* - - - - 0 -
*******************************************************************************/
U8 mos6502_CLD(mos6502_processor_st * pProcessor, U8 opCode) {

	SR_RESET_D();

	return 2;
}

/*******************************************************************************
* Clear Interrupt Disable Bit
*
* 0 -> I
*
* N Z C I D V
* - - - 0 - -
*******************************************************************************/
U8 mos6502_CLI(mos6502_processor_st * pProcessor, U8 opCode) {

	SR_RESET_I();

	return 2;
}

/*******************************************************************************
* Clear Overflow Flag
*
* 0 -> V
*
* N Z C I D V
* - - - - - 0
*******************************************************************************/
U8 mos6502_CLV(mos6502_processor_st * pProcessor, U8 opCode) {

	SR_RESET_V();

	return 2;
}

/*******************************************************************************
* Compare Memory with Accumulator
*
* A - M
*
* N Z C I D V
* + + + - - -
*******************************************************************************/
U8 mos6502_CMP(mos6502_processor_st * pProcessor, U8 opCode) {

	mos6502_addr address = 0;
	U16 oldPC = pProcessor->reg.PC;
	U8 cyclesPassed = 0;
	U8 memValue = 0;

	switch (opCode) {
		case 0xC9: // immediate
			address = addrm_imm(pProcessor);
			cyclesPassed = 2;
			break;
		case 0xC5: // zeropage
			address = addrm_zpg(pProcessor);
			cyclesPassed = 3;
			break;
		case 0xD5: // zeropage,X
			address = addrm_zpgX(pProcessor);
			cyclesPassed = 4;
			break;
		case 0xCD: // absolute
			address = addrm_abs(pProcessor);
			cyclesPassed = 4;
			break;
		case 0xDD: // absolute,X
			address = addrm_absX(pProcessor);
			cyclesPassed = 4;
			break;
		case 0xD9: // absolute,Y
			address = addrm_absY(pProcessor);
			cyclesPassed = 4;
			break;
		case 0xC1: // (indirect,X)
			address = addrm_xInd(pProcessor);
			cyclesPassed = 6;
			break;
		case 0xD1: // (indirect),Y
			address = addrm_indY(pProcessor);
			cyclesPassed = 5;
			break;
		default:
			return 0xFF;
	}

	memValue = addrm_read8(pProcessor, address);

	switch (opCode)
	{
		case 0xDD:
		case 0xD9:
		case 0xD1:
			cyclesPassed += MOS6502_OUTOFPAGE(oldPC, address);
			break;
		default:
			break;
	}
	
	SR_COND_SET_N((pProcessor->reg.AC - memValue) & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.AC == memValue);
	SR_COND_SET_C(pProcessor->reg.AC >= memValue);

	return cyclesPassed;
}

/*******************************************************************************
* Compare Memory and Index X
*
* X - M
*
* N Z C I D V
* + + + - - -
*******************************************************************************/
U8 mos6502_CPX(mos6502_processor_st * pProcessor, U8 opCode) {

	U8 cyclesPassed = 0;
	U8 memValue = 0;

	switch (opCode) {
		case 0xE0: // immediate
			memValue = addrm_read8(pProcessor, addrm_imm(pProcessor));
			cyclesPassed = 2;
			break;
		case 0xE4: // zeropage
			memValue = addrm_read8(pProcessor, addrm_zpg(pProcessor));
			cyclesPassed = 3;
			break;
		case 0xEC: // absolute
			memValue = addrm_read8(pProcessor, addrm_abs(pProcessor));
			cyclesPassed = 4;
			break;
		default:
			return 0xFF;
	}
	
	SR_COND_SET_N((pProcessor->reg.X - memValue) & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.X == memValue);
	SR_COND_SET_C(pProcessor->reg.X >= memValue);
	

	return cyclesPassed;
}

/*******************************************************************************
* Compare Memory and Index Y
*
* Y - M
*
* N Z C I D V
* + + + - - -
*******************************************************************************/
U8 mos6502_CPY(mos6502_processor_st * pProcessor, U8 opCode) {

	U8 cyclesPassed = 0;
	U8 memValue = 0;

	switch (opCode) {
		case 0xC0: // immediate
			memValue = addrm_read8(pProcessor, addrm_imm(pProcessor));
			cyclesPassed = 2;
			break;
		case 0xC4: // zeropage
			memValue = addrm_read8(pProcessor, addrm_zpg(pProcessor));
			cyclesPassed = 3;
			break;
		case 0xCC: // absolute
			memValue = addrm_read8(pProcessor, addrm_abs(pProcessor));
			cyclesPassed = 4;
			break;
		default:
			return 0xFF;
	}
	
	SR_COND_SET_N((pProcessor->reg.Y - memValue) & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.Y == memValue);
	SR_COND_SET_C(pProcessor->reg.Y >= memValue);
	

	return cyclesPassed;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
U8 mos6502_DCP(mos6502_processor_st * pProcessor, U8 opCode) {
	return 0xFF;
}

/*******************************************************************************
* Decrement Memory by One
*
* M - 1 -> M
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_DEC(mos6502_processor_st * pProcessor, U8 opCode) {

	mos6502_addr address;
	U8 cyclesPassed = 0;
	U8 memValue;

	switch (opCode) {
		case 0xC6:  //	zeropage
			address = addrm_zpg(pProcessor);
			cyclesPassed = 	5;
			break;
		case 0xD6:  //	zeropage,X
			address = addrm_zpgX(pProcessor);
			cyclesPassed = 	6;
			break;
		case 0xCE:  //	absolute
			address = addrm_abs(pProcessor);
			cyclesPassed = 	6;
			break;
		case 0xDE:  //	absolute,X
			address = addrm_absX(pProcessor);
			cyclesPassed = 	7;
			break;
		default:
			return 0xFF;
	}

	memValue = addrm_read8(pProcessor, address) - 1;
	addrm_write8(pProcessor, address, memValue);

	SR_COND_SET_N(memValue & SR_FLAG_N);
	SR_COND_SET_Z(memValue == 0);

	return cyclesPassed;
}

/*******************************************************************************
* Decrement Index X by One
*
* x - 1 -> x
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_DEX(mos6502_processor_st * pProcessor, U8 opCode) {

	pProcessor->reg.X--;

	SR_COND_SET_N(pProcessor->reg.X & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.X == 0);

	return 2;
}

/*******************************************************************************
* Decrement Index Y by One
*
* Y - 1 -> Y
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_DEY(mos6502_processor_st * pProcessor, U8 opCode) {

	pProcessor->reg.Y--;

	SR_COND_SET_N(pProcessor->reg.Y & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.Y == 0);

	return 2;
}

/*******************************************************************************
* Exclusive-OR Memory with Accumulator
*
* A EOR M -> A
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_EOR(mos6502_processor_st * pProcessor, U8 opCode) {
	mos6502_addr address;	
	U16 oldPC = pProcessor->reg.PC;
	U8 cyclesPassed = 0;
	U8 tempAc = pProcessor->reg.AC;

	switch (opCode) {
		case 0x49: // immediate
			address = addrm_imm(pProcessor);
			cyclesPassed = 2;
			break;
		case 0x45: // zeropage
			address = addrm_zpg(pProcessor);
			cyclesPassed = 3;
			break;
		case 0x55: // zeropage, X
			address = addrm_zpgX(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x4D: // absolute
			address = addrm_abs(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x5D: // absolute, X
			address = addrm_absX(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x59: // absolute, Y
			address = addrm_absY(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x41: // (indirect, X)
			address = addrm_xInd(pProcessor);
			cyclesPassed = 6;
			break;
		case 0x51: // (indirect), Y
			address = addrm_indY(pProcessor);
			cyclesPassed = 5;
			break;
		default:
			return 0xFF;
	}

	pProcessor->reg.AC ^= addrm_read8(pProcessor, address);
	
	switch (opCode) {
		case 0x5D:
		case 0x59:
		case 0x51:
			cyclesPassed += MOS6502_OUTOFPAGE(oldPC, address);
			break;
		default:
			break;
	}

	SR_COND_SET_N(pProcessor->reg.AC & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.AC == 0);

	return cyclesPassed;
}

/*******************************************************************************
* Increment Memory by One
*
* M + 1 -> M
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_INC(mos6502_processor_st * pProcessor, U8 opCode) {

	mos6502_addr address;
	U8 cyclesPassed = 0;

	switch (opCode) {
		case 0xE6:	// zeropage
			address = addrm_zpg(pProcessor);
			cyclesPassed = 5;
			break;
		case 0xF6:	// zeropage,X
			address = addrm_zpgX(pProcessor);
			cyclesPassed = 6;
			break;
		case 0xEE:	// absolute
			address = addrm_abs(pProcessor);
			cyclesPassed = 6;
			break;
		case 0xFE:	// absolute,X
			address = addrm_absX(pProcessor);
			cyclesPassed = 7;
			break;
		default:
			return 0xFF;
	}
	
	addrm_write8(pProcessor, address, addrm_read8(pProcessor, address) + 1);

	SR_COND_SET_N(pProcessor->reg.AC & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.AC == 0);

	return cyclesPassed;
}

/*******************************************************************************
* Increment Index X by One
*
* X + 1 -> X
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_INX(mos6502_processor_st * pProcessor, U8 opCode) {

	pProcessor->reg.X++;

	return 2;
}

/*******************************************************************************
* Increment Index Y by One
*
* Y + 1 -> Y
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_INY(mos6502_processor_st * pProcessor, U8 opCode) {

	pProcessor->reg.Y++;

	SR_COND_SET_N(pProcessor->reg.Y & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.Y == 0);

	return 2;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
U8 mos6502_ISC(mos6502_processor_st * pProcessor, U8 opCode) {
	return 0xFF;
}

/*******************************************************************************
* JAMs the processor
*******************************************************************************/
U8 mos6502_JAM(mos6502_processor_st * pProcessor, U8 opCode) {
	return 0xFE;
}

/*******************************************************************************
* Jump to New Location
*
* operand 1st byte -> PCL
* operand 2nd byte -> PCH
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_JMP(mos6502_processor_st * pProcessor, U8 opCode) {
	mos6502_addr address;
	U8 cyclesPassed = 0;

	switch (opCode) {
		case 0x4C: // absolute
			address = addrm_abs(pProcessor);
			cyclesPassed = 3;
			break;
		case 0x6C: // indirect
			address = addrm_ind(pProcessor);
			cyclesPassed = 5;
			break;
		default:
			return 0xFF;
	}

	pProcessor->reg.PC = address;

	return cyclesPassed;
}

/*******************************************************************************
* Jump to New Location Saving Return Address
*
* push (PC+2),
* operand 1st byte -> PCL ABC
* operand 2nd byte -> PCH
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_JSR(mos6502_processor_st * pProcessor, U8 opCode) {

	addrm_stackPush16(pProcessor, pProcessor->reg.PC + 1);

	pProcessor->reg.PC = addrm_abs(pProcessor);

	return 6;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
U8 mos6502_LAS(mos6502_processor_st * pProcessor, U8 opCode) {
	return 0xFF;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
U8 mos6502_LAX(mos6502_processor_st * pProcessor, U8 opCode) {
	return 0xFF;
}

/*******************************************************************************
* Load Accumulator with Memory.
*
* M -> A
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_LDA(mos6502_processor_st * pProcessor, U8 opCode) {

	U16 oldPC = pProcessor->reg.PC;
	U8 cyclesPassed = 0;
	U16 address = 0;

	switch (opCode) {
		case 0xA9:	// Immediate
			address = addrm_imm(pProcessor);
			cyclesPassed = 2;
			break;
		case 0xA5: // zeropage
			address = addrm_zpg(pProcessor);
			cyclesPassed = 3;
			break;
		case 0xB5: // zeropage, X
			address = addrm_zpgX(pProcessor);
			cyclesPassed = 4;
			break;
		case 0xAD: // absolute
			address = addrm_abs(pProcessor);
			cyclesPassed = 4;
			break;
		case 0xBD: // absolute, X
			address = addrm_absX(pProcessor);
			cyclesPassed = 4;
			break;
		case 0xB9: // absolute, Y
			address = addrm_absY(pProcessor);
			cyclesPassed = 4;
			break;
		case 0xA1: // (indirect, X)
			address = addrm_xInd(pProcessor);
			cyclesPassed = 6;
			break;
		case 0xB1: // (indirect), Y
			address = addrm_indY(pProcessor);
			cyclesPassed = 5;
			break;
		default:
			return 0xFF;
	}

	pProcessor->reg.AC = addrm_read8(pProcessor, address);

	switch (opCode) {
		case 0xBD:
		case 0xB9:
		case 0xB1:
			cyclesPassed += MOS6502_OUTOFPAGE(oldPC, address);
			break;
		default:
			break;
	}

	/* Update status register zero and negative flags. */
	SR_COND_SET_N(pProcessor->reg.AC & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.AC == 0);
	

	return cyclesPassed;
}

/*******************************************************************************
* Load Index X with Memory
*
* M -> X
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_LDX(mos6502_processor_st * pProcessor, U8 opCode) {

	U16 oldPC = pProcessor->reg.PC;
	U8 cyclesPassed = 0;
	mos6502_addr address = 0;

	switch (opCode) {
		case 0xA2: // immediate
			address = addrm_imm(pProcessor);
			cyclesPassed = 2;
			break;
		case 0xA6: // zeropage
			address = addrm_zpg(pProcessor);
			cyclesPassed = 3;
			break;
		case 0xB6: // zeropage,Y
			address = addrm_zpgY(pProcessor);
			cyclesPassed = 4;
			break;
		case 0xAE: // absolute
			address = addrm_abs(pProcessor);
			cyclesPassed = 4;
			break;
		case 0xBE: // absolute,Y
			address = addrm_absY(pProcessor);
			cyclesPassed = 4;
			break;
		default:
			return 0xFF;
	}

	pProcessor->reg.X = addrm_read8(pProcessor, address);

	if (opCode = 0xBE) {
		cyclesPassed += MOS6502_OUTOFPAGE(oldPC, address);
	}

	SR_COND_SET_N(pProcessor->reg.X & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.X == 0);

	return cyclesPassed;
}

/*******************************************************************************
* Load Index Y with Memory
*
* M -> Y
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_LDY(mos6502_processor_st * pProcessor, U8 opCode) {

	U16 oldPC = pProcessor->reg.PC;
	U8 cyclesPassed = 0;
	mos6502_addr address = 0;

	switch (opCode) {
		case 0xA0: //immediate
			address = addrm_imm(pProcessor);
			cyclesPassed = 2;
			break;
		case 0xA4: //zeropage
			address = addrm_zpg(pProcessor);
			cyclesPassed = 3;
			break;
		case 0xB4: //zeropage,X
			address = addrm_zpgX(pProcessor);
			cyclesPassed = 4;
			break;
		case 0xAC: //absolute
			address = addrm_abs(pProcessor);
			cyclesPassed = 4;
			break;
		case 0xBC: //absolute,X
			address = addrm_absX(pProcessor);
			cyclesPassed = 4;
			break;
		default:
			return 0xFF;
	}

	pProcessor->reg.Y = addrm_read8(pProcessor, address);

	if (opCode = 0xBC) {
		cyclesPassed += MOS6502_OUTOFPAGE(oldPC, address);
	}

	SR_COND_SET_N(pProcessor->reg.Y & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.Y == 0);

	return cyclesPassed;
}

/*******************************************************************************
* Shift One Bit Right (Memory or Accumulator)
*
* 0 -> [76543210] -> C
*
* N Z C I D V
* 0 + + - - -
*******************************************************************************/
U8 mos6502_LSR(mos6502_processor_st * pProcessor, U8 opCode) {
	mos6502_addr address;
	U8 cyclesPassed = 0;
	U8 origVal = 0;
	U8 shiftedVal = 0;

	switch (opCode) {
		case 0x4A: // accumulator
			origVal = pProcessor->reg.AC;
			cyclesPassed = 2;
			break;
		case 0x46: // zeropage
			address = addrm_zpg(pProcessor);
			origVal = addrm_read8(pProcessor, address);
			cyclesPassed = 5;
			break;
		case 0x56: // zeropage,X
			address = addrm_zpgX(pProcessor);
			origVal = addrm_read8(pProcessor, address);
			cyclesPassed = 6;
			break;
		case 0x4E: // absolute
			address = addrm_abs(pProcessor);
			origVal = addrm_read8(pProcessor, address);
			cyclesPassed = 6;
			break;
		case 0x5E: // absolute,X
			address = addrm_absX(pProcessor);
			origVal = addrm_read8(pProcessor, address);
			cyclesPassed = 7;
			break;
		default:
			return 0xFF;
	}

	shiftedVal = origVal >> 1;

	if (opCode == 0x4A) {
		pProcessor->reg.AC = shiftedVal;
	} else {
		addrm_write8(pProcessor, address, shiftedVal);
	}

	SR_RESET_N();
	SR_COND_SET_C(origVal & SR_FLAG_C);
	SR_COND_SET_Z(shiftedVal == 0);

	return cyclesPassed;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
U8 mos6502_LXA(mos6502_processor_st * pProcessor, U8 opCode) {
	return 0xFF;
}

/*******************************************************************************
* No Operation
*
*	---
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_NOP(mos6502_processor_st * pProcessor, U8 opCode) {

	U8 cyclesPassed = 0xFF;

	switch (opCode)
	{
		case 0xEA: // Only LEGAL NOP
			cyclesPassed = 2;
		case 0x1A: // implied 1
		case 0x3A:
		case 0x5A:
		case 0x7A:
		case 0xDA:
		case 0xFA: // implied 1
			cyclesPassed = 2;
			break;
		case 0x80:  //	immediate	2
		case 0x82: 
		case 0x89: 
		case 0xC2:
		case 0xE2: //	immediate	2
			cyclesPassed = 2;
			addrm_read8(pProcessor, addrm_imm(pProcessor));
			break;
		case 0x04: //	zeropage	2
		case 0x44:
		case 0x64: //	zeropage	2
			cyclesPassed = 3;
			addrm_read8(pProcessor, addrm_zpg(pProcessor));
			break;
		case 0x14: //	zeropage,X	2
		case 0x34:
		case 0x54:
		case 0x74:
		case 0xD4:
		case 0xF4: //	zeropage,X	2
			cyclesPassed = 4;
			addrm_read8(pProcessor, addrm_zpgX(pProcessor));
			break;
		case 0x0C: //	absolute	3
			cyclesPassed = 4;
			addrm_read16(pProcessor, addrm_abs(pProcessor));
			break;
		case 0x1C: //	absolut,X	3
		case 0x3C:
		case 0x5C:
		case 0x7C:
		case 0xDC:
		case 0xFC: //	absolut,X	3
			cyclesPassed = 4 + MOS6502_OUTOFPAGE(
				pProcessor->reg.PC,
				addrm_read16(pProcessor, addrm_absX(pProcessor))
			);
			break;
		default:
			return 0xFF;
	}

	return cyclesPassed;
}

/*******************************************************************************
* OR Memory with Accumulator
*
* A OR M -> A
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_ORA(mos6502_processor_st * pProcessor, U8 opCode) {

	mos6502_addr address;
	mos6502_addr oldPC = pProcessor->reg.PC;
	U8 cyclesPassed;
	U8 memValue;

	switch (opCode) {
		case 0x09:	// immediate
			address = addrm_imm(pProcessor);
			cyclesPassed = 2;
			break;
		case 0x05:	// zeropage
			address = addrm_zpg(pProcessor);
			cyclesPassed = 3;
			break;
		case 0x15:	// zeropage,X
			address = addrm_zpgX(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x0D:	// absolute
			address = addrm_abs(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x1D:	// absolute,X
			address = addrm_absX(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x19:	// absolute,Y
			address = addrm_absY(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x01:	// (indirect,X)
			address = addrm_xInd(pProcessor);
			cyclesPassed = 6;
			break;
		case 0x11:	// (indirect),Y
			address = addrm_indY(pProcessor);
			cyclesPassed = 5;
			break;
		default:
			return 0xFF;
	}

	memValue = addrm_read8(pProcessor, address);

	pProcessor->reg.AC |= memValue;

	switch (opCode) {
		case 0x1D:
		case 0x19:
		case 0x11:
			cyclesPassed += MOS6502_OUTOFPAGE(oldPC, address);
			break;
		default:
			break;
	}

	SR_COND_SET_N(pProcessor->reg.AC & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.AC == 0);

	return cyclesPassed;
}

/*******************************************************************************
* Push Accumulator on Stack
*
* push A
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_PHA(mos6502_processor_st * pProcessor, U8 opCode) {

	addrm_stackPush8(pProcessor, pProcessor->reg.AC);

	return 3;
}

/*******************************************************************************
* Push Processor Status on Stack
*
* The status register will be pushed with the break
* flag and bit 5 set to 1.
*
* push SR
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_PHP(mos6502_processor_st * pProcessor, U8 opCode) {

	addrm_stackPush8(pProcessor, pProcessor->reg.SR | SR_FLAG_B | SR_FLAG_UNUSED);

	return 3;
}

/*******************************************************************************
* Pull Accumulator from Stack
*
* pull A
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_PLA(mos6502_processor_st * pProcessor, U8 opCode) {

	pProcessor->reg.AC = addrm_stackPop8(pProcessor);

	SR_COND_SET_N(pProcessor->reg.AC & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.AC == 0);

	return 4;
}

/*******************************************************************************
* Pull Processor Status from Stack
*
* The status register will be pulled with the break
* flag and bit 5 ignored.
*
* pull SR
*
* N Z C I D V
* from stack
*******************************************************************************/
U8 mos6502_PLP(mos6502_processor_st * pProcessor, U8 opCode) {

	pProcessor->reg.SR = addrm_stackPop8(pProcessor) & ~(SR_FLAG_B | SR_FLAG_UNUSED);

	return 4;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
U8 mos6502_RLA(mos6502_processor_st * pProcessor, U8 opCode) {
	return 0xFF;
}

/*******************************************************************************
* Rotate One Bit Left (Memory or Accumulator)
*
* C <- [76543210] <- C
*
* N Z C I D V
* + + + - - -
*******************************************************************************/
U8 mos6502_ROL(mos6502_processor_st * pProcessor, U8 opCode) {

	mos6502_addr address;
	U8 cyclesPassed = 0;
	U8 memValue;
	U8 tempCarry = SR_GET_C();

	switch (opCode) {
		case 0x2A:	// accumulator
			memValue = pProcessor->reg.AC;
			cyclesPassed = 2;
			break;
		case 0x26:	// zeropage
			address = addrm_zpg(pProcessor);
			memValue = addrm_read8(pProcessor, address);
			cyclesPassed = 5;
			break;
		case 0x36:	// zeropage,X
			address = addrm_zpgX(pProcessor);
			memValue = addrm_read8(pProcessor, address);
			cyclesPassed = 6;
			break;
		case 0x2E:	// absolute
			address = addrm_abs(pProcessor);
			memValue = addrm_read8(pProcessor, address);
			cyclesPassed = 6;
			break;
		case 0x3E:	// absolute,X
			address = addrm_absX(pProcessor);
			memValue = addrm_read8(pProcessor, address);
			cyclesPassed = 7;
			break;
		default:
			return 0xFF;
	}

	SR_COND_SET_C(memValue & SR_FLAG_N);

	memValue <<= 1;
	memValue |= tempCarry;

	switch (opCode) {
		case 0x6A:
			pProcessor->reg.AC = memValue;
			break;
		default:
			addrm_write8(pProcessor, address, memValue);
			break;
	}

	SR_COND_SET_N(memValue & SR_FLAG_N);
	SR_COND_SET_Z(memValue == 0);

	return cyclesPassed;
}

/*******************************************************************************
* Rotate One Bit Right (Memory or Accumulator)
*
* C -> [76543210] -> C
*
* N Z C I D V
* + + + - - -
*******************************************************************************/
U8 mos6502_ROR(mos6502_processor_st * pProcessor, U8 opCode) {

	mos6502_addr address;
	U8 cyclesPassed = 0;
	U8 memValue;
	U8 tempCarry = SR_GET_C();

	switch (opCode) {
		case 0x6A:	// accumulator
			memValue = pProcessor->reg.AC;
			cyclesPassed = 2;
			break;
		case 0x66:	// zeropage
			address = addrm_zpg(pProcessor);
			memValue = addrm_read8(pProcessor, address);
			cyclesPassed = 5;
			break;
		case 0x76:	// zeropage,X
			address = addrm_zpgX(pProcessor);
			memValue = addrm_read8(pProcessor, address);
			cyclesPassed = 6;
			break;
		case 0x6E:	// absolute
			address = addrm_abs(pProcessor);
			memValue = addrm_read8(pProcessor, address);
			cyclesPassed = 6;
			break;
		case 0x7E:	// absolute,X
			address = addrm_absX(pProcessor);
			memValue = addrm_read8(pProcessor, address);
			cyclesPassed = 7;
			break;
		default:
			return 0xFF;
	}

	SR_COND_SET_C(memValue & SR_FLAG_C);

	memValue >>= 1;
	memValue |= tempCarry << 7;

	switch (opCode) {
		case 0x6A:
			pProcessor->reg.AC = memValue;
			break;
		default:
			addrm_write8(pProcessor, address, memValue);
			break;
	}

	SR_COND_SET_N(memValue & SR_FLAG_N);
	SR_COND_SET_Z(memValue == 0);

	return cyclesPassed;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
U8 mos6502_RRA(mos6502_processor_st * pProcessor, U8 opCode) {
	return 0xFF;
}

/*******************************************************************************
* Return from Interrupt
*
* The status register is pulled with the break flag
* and bit 5 ignored. Then PC is pulled from the stack.
*
* pull SR, pull PC
*
* N Z C I D V
* from stack
*******************************************************************************/
U8 mos6502_RTI(mos6502_processor_st * pProcessor, U8 opCode) {

	pProcessor->reg.SR = addrm_stackPop8(pProcessor) & ~(SR_FLAG_B & SR_FLAG_UNUSED);
	pProcessor->reg.PC = addrm_stackPop16(pProcessor);

	return 6;
}

/*******************************************************************************
* Return from Subroutine
*
* pull PC, PC+1 -> PC
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_RTS(mos6502_processor_st * pProcessor, U8 opCode) {

	pProcessor->reg.PC = addrm_stackPop16(pProcessor) + 1;

	return 6;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
U8 mos6502_SAX(mos6502_processor_st * pProcessor, U8 opCode) {
	return 0xFF;
}

/*******************************************************************************
* Subtract Memory from Accumulator with Borrow
*
* A - M - C� -> A
*
* N Z C I D V
* + + + - - +
*******************************************************************************/
U8 mos6502_SBC(mos6502_processor_st * pProcessor, U8 opCode) {
	mos6502_addr address;
	mos6502_addr oldPC = pProcessor->reg.PC;
	U8 cyclesPassed = 0;
	U8 memValue = 0;
	U8 tempAc = pProcessor->reg.AC;

	// TODO implement decimal mode (BCD)
	switch (opCode) {
		case 0xE9: // immediate
			address = addrm_imm(pProcessor);
			cyclesPassed = 2;
			break;
		case 0xE5: // zeropage
			address = addrm_zpg(pProcessor);
			cyclesPassed = 3;
			break;
		case 0xF5: // zeropage,X
			address = addrm_zpgX(pProcessor);
			cyclesPassed = 4;
			break;
		case 0xED: // absolute
			address = addrm_abs(pProcessor);
			cyclesPassed = 4;
			break;
		case 0xFD: // absolute,X
			address = addrm_absX(pProcessor);
			cyclesPassed = 4;
			break;
		case 0xF9: // absolute,Y
			address = addrm_absY(pProcessor);
			cyclesPassed = 4;
			break;
		case 0xE1: // (indirect,X)
			address = addrm_xInd(pProcessor);
			cyclesPassed = 6;
			break;
		case 0xF1: // (indirect),Y
			address = addrm_indY(pProcessor);
			cyclesPassed = 5;
			break;
		default:
			return 0xFF;
	}

	memValue = addrm_read8(pProcessor, address);
	pProcessor->reg.AC += ~memValue;
	pProcessor->reg.AC += SR_GET_C();

	switch (opCode)
	{
		case 0xFD:
		case 0xF9:
		case 0xF1:
			cyclesPassed += MOS6502_OUTOFPAGE(oldPC, address);
			break;
		default:
			break;
	}

	SR_COND_SET_V(
		!((tempAc & 0x80) ^ (~memValue & 0x80)) // Both values have same sign
	&&	(pProcessor->reg.AC & 0x80) ^ (~memValue & 0x80) // Output signedness differs from values.
	);
	SR_COND_SET_C(((U16)tempAc + (U16)memValue) > 0xFF);
	SR_COND_SET_N(pProcessor->reg.AC & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.AC == 0);
	

	return cyclesPassed;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
U8 mos6502_SBX(mos6502_processor_st * pProcessor, U8 opCode) {
	return 0xFF;
}

/*******************************************************************************
* Set Carry Flag
*
* 1 -> C
*
* N Z C I D V
* - - 1 - - -
*******************************************************************************/
U8 mos6502_SEC(mos6502_processor_st * pProcessor, U8 opCode) {

	SR_SET_C();

	return 2;
}

/*******************************************************************************
* Set Decimal Flag
*
* 1 -> D
*
* N Z C I D V
* - - - - 1 -
*******************************************************************************/
U8 mos6502_SED(mos6502_processor_st * pProcessor, U8 opCode) {

	SR_SET_D();

	return 2;
}

/*******************************************************************************
* Set Interrupt Flag
*
* 1 -> I
*
* N Z C I D V
* - - - 1 - -
*******************************************************************************/
U8 mos6502_SEI(mos6502_processor_st * pProcessor, U8 opCode) {

	SR_SET_I();

	return 2;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
U8 mos6502_SHA(mos6502_processor_st * pProcessor, U8 opCode) {
	return 0xFF;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
U8 mos6502_SHX(mos6502_processor_st * pProcessor, U8 opCode) {
	return 0xFF;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
U8 mos6502_SHY(mos6502_processor_st * pProcessor, U8 opCode) {
	return 0xFF;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
U8 mos6502_SLO(mos6502_processor_st * pProcessor, U8 opCode) {
	return 0xFF;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
U8 mos6502_SRE(mos6502_processor_st * pProcessor, U8 opCode) {
	return 0xFF;
}

/*******************************************************************************
* Store Accumulator in Memory
*
* A -> M
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_STA(mos6502_processor_st * pProcessor, U8 opCode) {
	mos6502_addr address;
	U8 cyclesPassed = 0;

	switch (opCode) {	
		case 0x85: // zeropage
			address = addrm_zpg(pProcessor);
			cyclesPassed = 3;
			break;
		case 0x95: // zeropage,X
			address = addrm_zpgX(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x8D: // absolute
			address = addrm_abs(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x9D: // absolute,X
			address = addrm_absX(pProcessor);
			cyclesPassed = 5;
			break;
		case 0x99: // absolute,Y
			address = addrm_absY(pProcessor);
			cyclesPassed = 5;
			break;
		case 0x81: // (indirect,X)
			address = addrm_xInd(pProcessor);
			cyclesPassed = 6;
			break;
		case 0x91: // (indirect),Y
			address = addrm_indY(pProcessor);
			cyclesPassed = 6;
			break;
		default:
			return 0xFF;
	}

	addrm_write8(pProcessor, address, pProcessor->reg.AC);

	return cyclesPassed;
}

/*******************************************************************************
* Store Index X in Memory
*
* X -> M
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_STX(mos6502_processor_st * pProcessor, U8 opCode) {
	mos6502_addr address;
	U8 cyclesPassed = 0;

	switch (opCode) {
		case 0x86: // zeropage
			address = addrm_zpg(pProcessor);
			cyclesPassed = 3;
			break;
		case 0x8E: // absolute
			address = addrm_abs(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x96: // zeropage,Y
			address = addrm_zpgY(pProcessor);
			cyclesPassed = 4;
			break;
		default:
			return 0xFF;
	}

	addrm_write8(pProcessor, address, pProcessor->reg.X);

	return cyclesPassed;
}

/*******************************************************************************
* Store Index Y in Memory
*
* Y -> M
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_STY(mos6502_processor_st * pProcessor, U8 opCode) {
	mos6502_addr address = 0;
	U8 cyclesPassed = 0;

	switch (opCode) {
		case 0x84: // zeropage
			address = addrm_zpg(pProcessor);
			cyclesPassed = 3;
			break;
		case 0x8C: // absolute
			address = addrm_abs(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x94: // zeropage,X
			address = addrm_zpgX(pProcessor);
			cyclesPassed = 4;
			break;
		default:
			return 0xFF;
	}

	addrm_write8(pProcessor, address, pProcessor->reg.Y);

	return cyclesPassed;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
U8 mos6502_TAS(mos6502_processor_st * pProcessor, U8 opCode) {
	return 0xFF;
}

/*******************************************************************************
* Transfer Accumulator to Index X
*
* A -> X
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_TAX(mos6502_processor_st * pProcessor, U8 opCode) {

	pProcessor->reg.X = pProcessor->reg.AC;

	SR_COND_SET_N(pProcessor->reg.X & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.X == 0);

	return 2;
}

/*******************************************************************************
* Transfer Accumulator to Index Y
*
* A -> Y
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_TAY(mos6502_processor_st * pProcessor, U8 opCode) {

	pProcessor->reg.Y = pProcessor->reg.AC;

	/* Update status register zero and negative flags. */
	SR_COND_SET_N(pProcessor->reg.AC & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.AC == 0);
	
	return 2;
}

/*******************************************************************************
* Transfer Stack Pointer to Index X
*
* SP -> X
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_TSX(mos6502_processor_st * pProcessor, U8 opCode) {

	pProcessor->reg.X = pProcessor->reg.SP;

	return 2;
}

/*******************************************************************************
* Transfer Index X to Accumulator
*
* X -> A
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_TXA(mos6502_processor_st * pProcessor, U8 opCode) {

	pProcessor->reg.AC = pProcessor->reg.X;

	return 2;
}

/*******************************************************************************
* Transfer Index X to Stack Register
*
* X -> SP
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_TXS(mos6502_processor_st * pProcessor, U8 opCode) {

	pProcessor->reg.SP = pProcessor->reg.X;

	return 2;
}

/*******************************************************************************
* Transfer Index Y to Accumulator
*
* Y -> A
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_TYA(mos6502_processor_st * pProcessor, U8 opCode) {

	pProcessor->reg.AC = pProcessor->reg.Y;

	SR_COND_SET_N(pProcessor->reg.AC & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.AC == 0);

	return 2;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
U8 mos6502_USBC(
	mos6502_processor_st *	pProcessor, 
	U8						opCode
) {
	return 0xFF;
}
