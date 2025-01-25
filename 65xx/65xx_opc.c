/*******************************************************************************
* 65xx_opc.c
*
* 65xx instruction implementations.
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "65xx.h"
//#include "65xx_opc.h"
#include "65xx_addrm.h"

/*******************************************************************************
* Macros and definitions.
*******************************************************************************/
#define CHK_IF_N(_var) ((_var) & SR_FLAG_N)
#define CHK_IF_V(_var) ((_var) & SR_FLAG_V)
#define CHK_IF_Z(_var) ((_var) == 0)

#define SR_GET_N() ((pProcessor->reg.SR & SR_FLAG_N) ? 1 : 0)
#define SR_GET_Z() ((pProcessor->reg.SR & SR_FLAG_Z) ? 1 : 0)
#define SR_GET_C() ((pProcessor->reg.SR & SR_FLAG_C) ? 1 : 0)
#define SR_GET_I() ((pProcessor->reg.SR & SR_FLAG_I) ? 1 : 0)
#define SR_GET_V() ((pProcessor->reg.SR & SR_FLAG_V) ? 1 : 0)
#define SR_GET_D() ((pProcessor->reg.SR & SR_FLAG_D) ? 1 : 0)
#define SR_GET_B() ((pProcessor->reg.SR & SR_FLAG_B) ? 1 : 0)

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

#define SR_COND_SET_N(_testValue) ((_testValue) ? SR_SET_N() : SR_RESET_N())
#define SR_COND_SET_Z(_testValue) ((_testValue) ? SR_SET_Z() : SR_RESET_Z())
#define SR_COND_SET_C(_testValue) ((_testValue) ? SR_SET_C() : SR_RESET_C())
#define SR_COND_SET_I(_testValue) ((_testValue) ? SR_SET_I() : SR_RESET_I())
#define SR_COND_SET_V(_testValue) ((_testValue) ? SR_SET_V() : SR_RESET_V())
#define SR_COND_SET_D(_testValue) ((_testValue) ? SR_SET_D() : SR_RESET_D())
#define SR_COND_SET_B(_testValue) ((_testValue) ? SR_SET_B() : SR_RESET_B())

#define BIN_TO_DEC(_bin) (((_bin / 10) << 4) + (_bin - ((_bin / 10) * 10)))
#define DEC_TO_BIN(_bin) (((_bin >> 4) * 10) + (_bin & 0xF))

#define UNUSED(_var) _var = _var

addrmFn addrm_functions[] = {
/*NON*/	NULL,
/*IMP*/	NULL,
/*ACC*/	NULL,
/*IMM*/	addrm_IMM,
/*REL*/	addrm_REL,
/*ABS*/	addrm_ABS,
/*ABX*/	addrm_ABX,
/*ABY*/	addrm_ABY,
/*ZPG*/	addrm_ZPG,
/*ZPX*/	addrm_ZPX,
/*ZPY*/	addrm_ZPY,
/*IND*/	addrm_IND,
/*INX*/	addrm_INX,
/*INY*/	addrm_INY
};

/*******************************************************************************
* Common private functions
*******************************************************************************/
void mos65xx_branch(Processor_65xx * pProcessor, U8 condition);
U8 mos65xx_binAdd(Processor_65xx * pProcessor, U8 lValue, U8 rValue);
U8 mos65xx_decAdd(Processor_65xx * pProcessor, U8 lValue, U8 rValue, I8 operator);
void mos65xx_compare(Processor_65xx * pProcessor, U8 lValue, U8 rValue);


void mos65xx_ILLNOP(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm);

/*******************************************************************************
* Add Memory to Accumulator with Carry.
*
* A + M + C -> A, C
*
* N Z C I D V
* + + + - - +
*******************************************************************************/
void mos65xx_ADC(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	mos65xx_addr address;
	U16 oldPC = pProcessor->reg.PC;
	U8 memValue = 0;

	// TODO implement decimal mode (BCD)

	switch (opCode) {
		case 0x69: // immediate
			break;
		case 0x65: // zeropage
			pProcessor->cycles.currentOp = 3;
			break;
		case 0x75: // zeropage,X
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x6D: // absolute
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x7D: // absolute,X
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x79: // absolute,Y
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x61: // (indirect,X)
			pProcessor->cycles.currentOp = 6;
			break;
		case 0x71: // (indirect),Y
			pProcessor->cycles.currentOp = 5;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	address = addrm_functions[addrm](pProcessor);

	memValue = addrm_read8(pProcessor, address);

	if (SR_GET_D()) {	// Decimal mode
		pProcessor->reg.AC = mos65xx_decAdd(pProcessor, pProcessor->reg.AC, memValue, 1); 
	} else {			// Binary mode
		pProcessor->reg.AC = mos65xx_binAdd(pProcessor, pProcessor->reg.AC, memValue); 
	}
	
	switch (opCode)	{
		case 0x7D:
		case 0x79:
		case 0x71:
			pProcessor->cycles.currentOp += MOS65xx_OUTOFPAGE(oldPC, address);
			break;
		default:
			break;
	}
}

/*******************************************************************************
* AND Memory with Accumulator
*
* A AND M -> A
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
void mos65xx_AND(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	mos65xx_addr address;
	mos65xx_addr oldPC = pProcessor->reg.PC;

	switch (opCode) {
		case 0x29: //immediate
			break;
		case 0x25: //zeropage
			pProcessor->cycles.currentOp = 3;
			break;
		case 0x35: //zeropage,X
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x2D: //absolute
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x3D: //absolute,X
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x39: //absolute,Y
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x21: //(indirect,X)
			pProcessor->cycles.currentOp = 6;
			break;
		case 0x31: //(indirect),Y
			pProcessor->cycles.currentOp = 5;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	address = addrm_functions[addrm](pProcessor);

	switch (opCode)
	{
		case 0x3D:
		case 0x39:
		case 0x31:
			pProcessor->cycles.currentOp += MOS65xx_OUTOFPAGE(oldPC, address);
			break;
		default:
			break;
	}

	pProcessor->reg.AC = pProcessor->reg.AC & addrm_read8(pProcessor, address);

	SR_COND_SET_N(CHK_IF_N(pProcessor->reg.AC));
	SR_COND_SET_Z(CHK_IF_Z(pProcessor->reg.AC));
}

/*******************************************************************************
* Shift Left One Bit (Memory or Accumulator)
*
* C <- [76543210] <- 0
*
* N Z C I D V
* + + + - - -
*******************************************************************************/
void mos65xx_ASL(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	mos65xx_addr address;
	U8 origVal = 0;
	U8 result = 0;

	switch (opCode) {
		case 0x0A: // accumulator
			pProcessor->cycles.currentOp = 2;
			break;
		case 0x06: // zeropage
			pProcessor->cycles.currentOp = 5;
			break;
		case 0x16: // zeropage,X
			pProcessor->cycles.currentOp = 6;
			break;
		case 0x0E: // absolute
			pProcessor->cycles.currentOp = 6;
			break;
		case 0x1E: // absolute,X
			pProcessor->cycles.currentOp = 7;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	if (opCode == 0x0A) {
		origVal = pProcessor->reg.AC;
	} else {
		address = addrm_functions[addrm](pProcessor);
		origVal = addrm_read8(pProcessor, address);
	}

	result = origVal << 1;

	if (opCode == 0x0A) {
		pProcessor->reg.AC = result;
	} else {
		addrm_write8(pProcessor, address, result);
	}

	SR_COND_SET_N(CHK_IF_N(result));
	SR_COND_SET_C(CHK_IF_N(origVal));
	SR_COND_SET_Z(CHK_IF_Z(result));
}

/*******************************************************************************
* Branch on Carry Clear
*
* branch on C = 0
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
void mos65xx_BCC(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(opCode);
	UNUSED(addrm);
	
	mos65xx_branch(pProcessor, !SR_GET_C());
}

/*******************************************************************************
* Branch on Carry Set
*
* branch on C = 1
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
void mos65xx_BCS(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(opCode);
	UNUSED(addrm);
	
	mos65xx_branch(pProcessor, SR_GET_C());	
}

/*******************************************************************************
* Branch on Result Zero
*
* branch on Z = 1
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
void mos65xx_BEQ(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(opCode);
	UNUSED(addrm);
	
	mos65xx_branch(pProcessor, SR_GET_Z());
}

/*******************************************************************************
* Test Bits in Memory with Accumulator
*
* A AND M -> Z, M7 -> N, M6 -> V
*
* N  Z C I D V
* M7 + - - - M6
*******************************************************************************/
void mos65xx_BIT(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	U8 memValue = 0;
	mos65xx_addr address = 0;

	switch (opCode) {
		case 0x24: // zeropage
			pProcessor->cycles.currentOp = 3;
			break;
		case 0x2C: // absolute
			pProcessor->cycles.currentOp = 4;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	address = addrm_functions[addrm](pProcessor);
	memValue = addrm_read8(pProcessor, address);

	SR_COND_SET_N(CHK_IF_N(memValue));
	SR_COND_SET_V(CHK_IF_V(memValue));
	SR_COND_SET_Z(CHK_IF_Z(memValue & pProcessor->reg.AC));
}

/*******************************************************************************
* Branch on Result Minus
*
* branch on N = 1
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
void mos65xx_BMI(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(opCode);
	UNUSED(addrm);
	
	mos65xx_branch(pProcessor, SR_GET_N());
}

/*******************************************************************************
* Branch on Result not Zero
*
* branch on Z = 0
*******************************************************************************/
void mos65xx_BNE(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(opCode);
	UNUSED(addrm);
	
	mos65xx_branch(pProcessor, !SR_GET_Z());
}

/*******************************************************************************
* Branch on Result Plus
*
* branch on N = 0
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
void mos65xx_BPL(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(opCode);
	UNUSED(addrm);
	
	mos65xx_branch(pProcessor, !SR_GET_N());
}

/*******************************************************************************
* Breakpoint
*
* push PC+2, push SR, ($FFFE) -> PC
*
* N Z C I D V
* - - - 1 - -
*******************************************************************************/
void mos65xx_BRK(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 7;

	addrm_stackPush16(pProcessor, pProcessor->reg.PC + 1);
	addrm_stackPush8(pProcessor, pProcessor->reg.SR | SR_FLAG_B | SR_FLAG_UNUSED);

	pProcessor->reg.PC = addrm_read16(pProcessor, 0xFFFE);

	SR_SET_I();
}

/*******************************************************************************
* Branch on Overflow Clear
*
* branch on V = 0
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
void mos65xx_BVC(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(opCode);
	UNUSED(addrm);
	
	mos65xx_branch(pProcessor, !SR_GET_V());
}

/*******************************************************************************
* Branch on Overflow Set
*
* branch on V = 1
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
void mos65xx_BVS(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(opCode);
	UNUSED(addrm);
	
	mos65xx_branch(pProcessor, SR_GET_V());
}

/*******************************************************************************
* Clear Carry Flag
*
* 0 -> C
*
* N Z C I D V
* - - 0 - - -
*******************************************************************************/
void mos65xx_CLC(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	SR_RESET_C();
	pProcessor->cycles.currentOp += 1;
}

/*******************************************************************************
* Clear Decimal Mode
*
* 0 -> D
*
* N Z C I D V
* - - - - 0 -
*******************************************************************************/
void mos65xx_CLD(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	SR_RESET_D();
	pProcessor->cycles.currentOp += 1;
}

/*******************************************************************************
* Clear Interrupt Disable Bit
*
* 0 -> I
*
* N Z C I D V
* - - - 0 - -
*******************************************************************************/
void mos65xx_CLI(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	SR_RESET_I();
	pProcessor->cycles.currentOp += 1;
}

/*******************************************************************************
* Clear Overflow Flag
*
* 0 -> V
*
* N Z C I D V
* - - - - - 0
*******************************************************************************/
void mos65xx_CLV(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	SR_RESET_V();
	pProcessor->cycles.currentOp += 1;
}

/*******************************************************************************
* Compare Memory with Accumulator
*
* A - M
*
* N Z C I D V
* + + + - - -
*******************************************************************************/
void mos65xx_CMP(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	mos65xx_addr address = 0;
	U16 oldPC = pProcessor->reg.PC;
	U8 memValue = 0;

	switch (opCode) {
		case 0xC9: // immediate
			break;
		case 0xC5: // zeropage
			pProcessor->cycles.currentOp = 3;
			break;
		case 0xD5: // zeropage,X
			pProcessor->cycles.currentOp = 4;
			break;
		case 0xCD: // absolute
			pProcessor->cycles.currentOp = 4;
			break;
		case 0xDD: // absolute,X
			pProcessor->cycles.currentOp = 4;
			break;
		case 0xD9: // absolute,Y
			pProcessor->cycles.currentOp = 4;
			break;
		case 0xC1: // (indirect,X)
			pProcessor->cycles.currentOp = 6;
			break;
		case 0xD1: // (indirect),Y
			pProcessor->cycles.currentOp = 5;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	address = addrm_functions[addrm](pProcessor);
	memValue = addrm_read8(pProcessor, address);

	switch (opCode)
	{
		case 0xDD:
		case 0xD9:
		case 0xD1:
			pProcessor->cycles.currentOp += MOS65xx_OUTOFPAGE(oldPC, address);
			break;
		default:
			break;
	}
	
	mos65xx_compare(pProcessor, pProcessor->reg.AC, memValue);
}

/*******************************************************************************
* Compare Memory and Index X
*
* X - M
*
* N Z C I D V
* + + + - - -
*******************************************************************************/
void mos65xx_CPX(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	U8 memValue = 0;
	mos65xx_addr address;

	switch (opCode) {
		case 0xE0: // immediate
			break;
		case 0xE4: // zeropage
			pProcessor->cycles.currentOp = 3;
			break;
		case 0xEC: // absolute
			pProcessor->cycles.currentOp = 4;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	address = addrm_functions[addrm](pProcessor);
	memValue = addrm_read8(pProcessor, address);
	
	mos65xx_compare(pProcessor, pProcessor->reg.X, memValue);
}

/*******************************************************************************
* Compare Memory and Index Y
*
* Y - M
*
* N Z C I D V
* + + + - - -
*******************************************************************************/
void mos65xx_CPY(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	U8 memValue = 0;
	mos65xx_addr address;

	switch (opCode) {
		case 0xC0: // immediate
			break;
		case 0xC4: // zeropage
			pProcessor->cycles.currentOp = 3;
			break;
		case 0xCC: // absolute
			pProcessor->cycles.currentOp = 4;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	address = addrm_functions[addrm](pProcessor);
	memValue = addrm_read8(pProcessor, address);
	
	mos65xx_compare(pProcessor, pProcessor->reg.Y, memValue);
}

/*******************************************************************************
* Decrement Memory by One
*
* M - 1 -> M
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
void mos65xx_DEC(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	mos65xx_addr address;
	U8 memValue;

	switch (opCode) {
		case 0xC6:  //	zeropage
			pProcessor->cycles.currentOp = 	5;
			break;
		case 0xD6:  //	zeropage,X
			pProcessor->cycles.currentOp = 	6;
			break;
		case 0xCE:  //	absolute
			pProcessor->cycles.currentOp = 	6;
			break;
		case 0xDE:  //	absolute,X
			pProcessor->cycles.currentOp = 	7;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	address = addrm_functions[addrm](pProcessor);
	memValue = addrm_read8(pProcessor, address) - 1;
	addrm_write8(pProcessor, address, memValue);

	SR_COND_SET_N(memValue & SR_FLAG_N);
	SR_COND_SET_Z(memValue == 0);
}

/*******************************************************************************
* Decrement Index X by One
*
* x - 1 -> x
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
void mos65xx_DEX(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->reg.X--;

	SR_COND_SET_N(CHK_IF_N(pProcessor->reg.X));
	SR_COND_SET_Z(CHK_IF_Z(pProcessor->reg.X));
	pProcessor->cycles.currentOp += 1;
}

/*******************************************************************************
* Decrement Index Y by One
*
* Y - 1 -> Y
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
void mos65xx_DEY(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->reg.Y--;

	SR_COND_SET_N(pProcessor->reg.Y & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.Y == 0);
	pProcessor->cycles.currentOp += 1;
}

/*******************************************************************************
* Exclusive-OR Memory with Accumulator
*
* A EOR M -> A
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
void mos65xx_EOR(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	mos65xx_addr address;	
	U16 oldPC = pProcessor->reg.PC;

	switch (opCode) {
		case 0x49: // immediate
			break;
		case 0x45: // zeropage
			pProcessor->cycles.currentOp = 3;
			break;
		case 0x55: // zeropage, X
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x4D: // absolute
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x5D: // absolute, X
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x59: // absolute, Y
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x41: // (indirect, X)
			pProcessor->cycles.currentOp = 6;
			break;
		case 0x51: // (indirect), Y
			pProcessor->cycles.currentOp = 5;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	address = addrm_functions[addrm](pProcessor);
	pProcessor->reg.AC ^= addrm_read8(pProcessor, address);
	
	switch (opCode) {
		case 0x5D:
		case 0x59:
		case 0x51:
			pProcessor->cycles.currentOp += MOS65xx_OUTOFPAGE(oldPC, address);
			break;
		default:
			break;
	}

	SR_COND_SET_N(CHK_IF_N(pProcessor->reg.AC));
	SR_COND_SET_Z(CHK_IF_Z(pProcessor->reg.AC));
}

/*******************************************************************************
* Increment Memory by One
*
* M + 1 -> M
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
void mos65xx_INC(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	mos65xx_addr address;
	U8 memValue = 0;

	switch (opCode) {
		case 0xE6:	// zeropage
			pProcessor->cycles.currentOp = 5;
			break;
		case 0xF6:	// zeropage,X
			pProcessor->cycles.currentOp = 6;
			break;
		case 0xEE:	// absolute
			pProcessor->cycles.currentOp = 6;
			break;
		case 0xFE:	// absolute,X
			pProcessor->cycles.currentOp = 7;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	address = addrm_functions[addrm](pProcessor);
	memValue = addrm_read8(pProcessor, address) + 1;
	
	addrm_write8(pProcessor, address, memValue);

	SR_COND_SET_N(CHK_IF_N(memValue));
	SR_COND_SET_Z(CHK_IF_Z(memValue));
}

/*******************************************************************************
* Increment Index X by One
*
* X + 1 -> X
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
void mos65xx_INX(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->reg.X++;

	SR_COND_SET_N(CHK_IF_N(pProcessor->reg.X));
	SR_COND_SET_Z(CHK_IF_Z(pProcessor->reg.X));
	pProcessor->cycles.currentOp += 1;
}

/*******************************************************************************
* Increment Index Y by One
*
* Y + 1 -> Y
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
void mos65xx_INY(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	UNUSED(addrm);

	pProcessor->reg.Y++;

	SR_COND_SET_N(pProcessor->reg.Y & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.Y == 0);
	pProcessor->cycles.currentOp += 1;
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
void mos65xx_JMP(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	mos65xx_addr address;

	switch (opCode) {
		case 0x4C: // absolute
			pProcessor->cycles.currentOp = 3;
			break;
		case 0x6C: // indirect
			pProcessor->cycles.currentOp = 5;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	address = addrm_functions[addrm](pProcessor);
	pProcessor->reg.PC = address;
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
void mos65xx_JSR(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	UNUSED(addrm);
	
	addrm_stackPush16(pProcessor, pProcessor->reg.PC + 1);

	pProcessor->reg.PC = addrm_read16(pProcessor, pProcessor->reg.PC);
	pProcessor->cycles.currentOp += 5;
}

/*******************************************************************************
* Load Accumulator with Memory.
*
* M -> A
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
void mos65xx_LDA(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	U16 oldPC = pProcessor->reg.PC;
	U16 address = 0;

	switch (opCode) {
		case 0xA9:	// Immediate
			break;
		case 0xA5: // zeropage
			pProcessor->cycles.currentOp = 3;
			break;
		case 0xB5: // zeropage, X
			pProcessor->cycles.currentOp = 4;
			break;
		case 0xAD: // absolute
			pProcessor->cycles.currentOp = 4;
			break;
		case 0xBD: // absolute, X
			pProcessor->cycles.currentOp = 4;
			break;
		case 0xB9: // absolute, Y
			pProcessor->cycles.currentOp = 4;
			break;
		case 0xA1: // (indirect, X)
			pProcessor->cycles.currentOp = 6;
			break;
		case 0xB1: // (indirect), Y
			pProcessor->cycles.currentOp = 5;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	address = addrm_functions[addrm](pProcessor);
	pProcessor->reg.AC = addrm_read8(pProcessor, address);

	switch (opCode) {
		case 0xBD:
		case 0xB9:
		case 0xB1:
			pProcessor->cycles.currentOp += MOS65xx_OUTOFPAGE(oldPC, address);
			break;
		default:
			break;
	}

	/* Update status register zero and negative flags. */
	SR_COND_SET_N(pProcessor->reg.AC & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.AC == 0);
}

/*******************************************************************************
* Load Index X with Memory
*
* M -> X
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
void mos65xx_LDX(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	U16 oldPC = pProcessor->reg.PC;
	mos65xx_addr address = 0;

	switch (opCode) {
		case 0xA2: // immediate
			break;
		case 0xA6: // zeropage
			pProcessor->cycles.currentOp = 3;
			break;
		case 0xB6: // zeropage,Y
			pProcessor->cycles.currentOp = 4;
			break;
		case 0xAE: // absolute
			pProcessor->cycles.currentOp = 4;
			break;
		case 0xBE: // absolute,Y
			pProcessor->cycles.currentOp = 4;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	address = addrm_functions[addrm](pProcessor);
	pProcessor->reg.X = addrm_read8(pProcessor, address);

	if (opCode == 0xBE) {
		pProcessor->cycles.currentOp += MOS65xx_OUTOFPAGE(oldPC, address);
	}

	SR_COND_SET_N(CHK_IF_N(pProcessor->reg.X));
	SR_COND_SET_Z(CHK_IF_Z(pProcessor->reg.X));
}

/*******************************************************************************
* Load Index Y with Memory
*
* M -> Y
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
void mos65xx_LDY(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	U16 oldPC = pProcessor->reg.PC;
	mos65xx_addr address = 0;

	switch (opCode) {
		case 0xA0: //immediate
			break;
		case 0xA4: //zeropage
			pProcessor->cycles.currentOp = 3;
			break;
		case 0xB4: //zeropage,X
			pProcessor->cycles.currentOp = 4;
			break;
		case 0xAC: //absolute
			pProcessor->cycles.currentOp = 4;
			break;
		case 0xBC: //absolute,X
			pProcessor->cycles.currentOp = 4;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	address = addrm_functions[addrm](pProcessor);
	pProcessor->reg.Y = addrm_read8(pProcessor, address);

	if (opCode == 0xBC) {
		pProcessor->cycles.currentOp += MOS65xx_OUTOFPAGE(oldPC, address);
	}

	SR_COND_SET_N(pProcessor->reg.Y & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.Y == 0);
}

/*******************************************************************************
* Shift One Bit Right (Memory or Accumulator)
*
* 0 -> [76543210] -> C
*
* N Z C I D V
* 0 + + - - -
*******************************************************************************/
void mos65xx_LSR(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	mos65xx_addr address;
	U8 origVal = 0;
	U8 shiftedVal = 0;

	switch (opCode) {
		case 0x4A: // accumulator
			pProcessor->cycles.currentOp = 2;
			break;
		case 0x46: // zeropage
			pProcessor->cycles.currentOp = 5;
			break;
		case 0x56: // zeropage,X
			pProcessor->cycles.currentOp = 6;
			break;
		case 0x4E: // absolute
			pProcessor->cycles.currentOp = 6;
			break;
		case 0x5E: // absolute,X
			pProcessor->cycles.currentOp = 7;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	if (opCode == 0x4A) {
		origVal = pProcessor->reg.AC;
	} else {
		address = addrm_functions[addrm](pProcessor);
		origVal = addrm_read8(pProcessor, address);
	}

	shiftedVal = origVal >> 1;

	if (opCode == 0x4A) {
		pProcessor->reg.AC = shiftedVal;
	} else {
		addrm_write8(pProcessor, address, shiftedVal);
	}

	SR_RESET_N();
	SR_COND_SET_C(origVal & SR_FLAG_C);
	SR_COND_SET_Z(CHK_IF_Z(shiftedVal));
}

/*******************************************************************************
* No Operation
*
*	---
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
void mos65xx_NOP(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(addrm);

	pProcessor->cycles.currentOp = 0xFF;

	if (opCode == 0xEA) { // Only LEGAL NOP
		pProcessor->cycles.currentOp = 2;
	} else {
		mos65xx_ILLNOP(pProcessor, opCode, addrm);
	}
}

/*******************************************************************************
* OR Memory with Accumulator
*
* A OR M -> A
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
void mos65xx_ORA(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	mos65xx_addr address;
	mos65xx_addr oldPC = pProcessor->reg.PC;
	U8 memValue;

	switch (opCode) {
		case 0x09:	// immediate
			break;
		case 0x05:	// zeropage
			pProcessor->cycles.currentOp = 3;
			break;
		case 0x15:	// zeropage,X
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x0D:	// absolute
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x1D:	// absolute,X
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x19:	// absolute,Y
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x01:	// (indirect,X)
			pProcessor->cycles.currentOp = 6;
			break;
		case 0x11:	// (indirect),Y
			pProcessor->cycles.currentOp = 5;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	address = addrm_functions[addrm](pProcessor);
	memValue = addrm_read8(pProcessor, address);

	pProcessor->reg.AC |= memValue;

	switch (opCode) {
		case 0x1D:
		case 0x19:
		case 0x11:
			pProcessor->cycles.currentOp += MOS65xx_OUTOFPAGE(oldPC, address);
			break;
		default:
			break;
	}

	SR_COND_SET_N(CHK_IF_N(pProcessor->reg.AC));
	SR_COND_SET_Z(CHK_IF_Z(pProcessor->reg.AC));
}

/*******************************************************************************
* Push Accumulator on Stack
*
* push A
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
void mos65xx_PHA(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	UNUSED(addrm);
	
	addrm_stackPush8(pProcessor, pProcessor->reg.AC);
	pProcessor->cycles.currentOp += 2;
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
void mos65xx_PHP(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	UNUSED(addrm);
	
	addrm_stackPush8(pProcessor, pProcessor->reg.SR | SR_FLAG_B | SR_FLAG_UNUSED);
	pProcessor->cycles.currentOp += 2;
}

/*******************************************************************************
* Pull Accumulator from Stack
*
* pull A
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
void mos65xx_PLA(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->reg.AC = addrm_stackPop8(pProcessor);

	SR_COND_SET_N(CHK_IF_N(pProcessor->reg.AC));
	SR_COND_SET_Z(CHK_IF_Z(pProcessor->reg.AC));
	pProcessor->cycles.currentOp += 3;
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
void mos65xx_PLP(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->reg.SR = addrm_stackPop8(pProcessor) & ~(SR_FLAG_B | SR_FLAG_UNUSED);
	pProcessor->cycles.currentOp += 3;
}

/*******************************************************************************
* Rotate One Bit Left (Memory or Accumulator)
*
* C <- [76543210] <- C
*
* N Z C I D V
* + + + - - -
*******************************************************************************/
void mos65xx_ROL(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	mos65xx_addr address;
	U8 memValue;
	U8 tempCarry = SR_GET_C();

	switch (opCode) {
		case 0x2A:	// accumulator
			pProcessor->cycles.currentOp = 2;
			break;
		case 0x26:	// zeropage
			pProcessor->cycles.currentOp = 5;
			break;
		case 0x36:	// zeropage,X
			pProcessor->cycles.currentOp = 6;
			break;
		case 0x2E:	// absolute
			pProcessor->cycles.currentOp = 6;
			break;
		case 0x3E:	// absolute,X
			pProcessor->cycles.currentOp = 7;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	if (opCode == 0x2A) {
		memValue = pProcessor->reg.AC;
	} else {
		address = addrm_functions[addrm](pProcessor);
		memValue = addrm_read8(pProcessor, address);
	}

	SR_COND_SET_C(CHK_IF_N(memValue));

	memValue <<= 1;
	memValue |= tempCarry;

	if (opCode == 0x2A) {
		pProcessor->reg.AC = memValue;
	} else {
		addrm_write8(pProcessor, address, memValue);
	}

	SR_COND_SET_Z(CHK_IF_Z(memValue));
	SR_COND_SET_N(CHK_IF_N(memValue));
}

/*******************************************************************************
* Rotate One Bit Right (Memory or Accumulator)
*
* C -> [76543210] -> C
*
* N Z C I D V
* + + + - - -
*******************************************************************************/
void mos65xx_ROR(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	mos65xx_addr address;
	U8 memValue;
	U8 tempCarry = SR_GET_C();

	switch (opCode) {
		case 0x6A:	// accumulator
			pProcessor->cycles.currentOp = 2;
			break;
		case 0x66:	// zeropage
			pProcessor->cycles.currentOp = 5;
			break;
		case 0x76:	// zeropage,X
			pProcessor->cycles.currentOp = 6;
			break;
		case 0x6E:	// absolute
			pProcessor->cycles.currentOp = 6;
			break;
		case 0x7E:	// absolute,X
			pProcessor->cycles.currentOp = 7;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	if (opCode == 0x6A) {
		memValue = pProcessor->reg.AC;
	} else {
		address = addrm_functions[addrm](pProcessor);
		memValue = addrm_read8(pProcessor, address);
	}

	SR_COND_SET_N(SR_GET_C());
	SR_COND_SET_C(memValue & 1);

	memValue >>= 1;
	memValue |= tempCarry << 7;

	if (opCode == 0x6A) {
		pProcessor->reg.AC = memValue;
	} else {
		addrm_write8(pProcessor, address, memValue);
	}

	SR_COND_SET_Z(CHK_IF_Z(memValue));
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
void mos65xx_RTI(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->reg.SR = addrm_stackPop8(pProcessor) & ~(SR_FLAG_B & SR_FLAG_UNUSED);
	pProcessor->reg.PC = addrm_stackPop16(pProcessor);
	pProcessor->cycles.currentOp += 5;
}

/*******************************************************************************
* Return from Subroutine
*
* pull PC, PC+1 -> PC
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
void mos65xx_RTS(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->reg.PC = addrm_stackPop16(pProcessor) + 1;
	pProcessor->cycles.currentOp += 5;
}

/*******************************************************************************
* Subtract Memory from Accumulator with Borrow
*
* A - M - C� -> A
*
* N Z C I D V
* + + + - - +
*******************************************************************************/
void mos65xx_SBC(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	mos65xx_addr address;
	mos65xx_addr oldPC = pProcessor->reg.PC;
	U8 memValue = 0;

	switch (opCode) {
		case 0xE9: // immediate
			break;
		case 0xE5: // zeropage
			pProcessor->cycles.currentOp = 3;
			break;
		case 0xF5: // zeropage,X
			pProcessor->cycles.currentOp = 4;
			break;
		case 0xED: // absolute
			pProcessor->cycles.currentOp = 4;
			break;
		case 0xFD: // absolute,X
			pProcessor->cycles.currentOp = 4;
			break;
		case 0xF9: // absolute,Y
			pProcessor->cycles.currentOp = 4;
			break;
		case 0xE1: // (indirect,X)
			pProcessor->cycles.currentOp = 6;
			break;
		case 0xF1: // (indirect),Y
			pProcessor->cycles.currentOp = 5;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	address = addrm_functions[addrm](pProcessor);
	memValue = addrm_read8(pProcessor, address);
	
	if (SR_GET_D()) {	// Decimal mode
		pProcessor->reg.AC = mos65xx_decAdd(pProcessor, pProcessor->reg.AC, memValue, -1); 
	} else {			// Binary mode
		pProcessor->reg.AC = mos65xx_binAdd(pProcessor, pProcessor->reg.AC, ~memValue); 
	}

	if (opCode == 0xFD || opCode == 0xF9 || opCode == 0xF1) {
		pProcessor->cycles.currentOp += MOS65xx_OUTOFPAGE(oldPC, address);
	}
}

/*******************************************************************************
* Set Carry Flag
*
* 1 -> C
*
* N Z C I D V
* - - 1 - - -
*******************************************************************************/
void mos65xx_SEC(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	SR_SET_C();
	pProcessor->cycles.currentOp += 1;
}

/*******************************************************************************
* Set Decimal Flag
*
* 1 -> D
*
* N Z C I D V
* - - - - 1 -
*******************************************************************************/
void mos65xx_SED(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	SR_SET_D();
	pProcessor->cycles.currentOp += 1;
}

/*******************************************************************************
* Set Interrupt Flag
*
* 1 -> I
*
* N Z C I D V
* - - - 1 - -
*******************************************************************************/
void mos65xx_SEI(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	SR_SET_I();
	pProcessor->cycles.currentOp += 1;
}

/*******************************************************************************
* Store Accumulator in Memory
*
* A -> M
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
void mos65xx_STA(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	mos65xx_addr address;

	switch (opCode) {	
		case 0x85: // zeropage
			pProcessor->cycles.currentOp = 3;
			break;
		case 0x95: // zeropage,X
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x8D: // absolute
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x9D: // absolute,X
			pProcessor->cycles.currentOp = 5;
			break;
		case 0x99: // absolute,Y
			pProcessor->cycles.currentOp = 5;
			break;
		case 0x81: // (indirect,X)
			pProcessor->cycles.currentOp = 6;
			break;
		case 0x91: // (indirect),Y
			pProcessor->cycles.currentOp = 6;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	address = addrm_functions[addrm](pProcessor);
	addrm_write8(pProcessor, address, pProcessor->reg.AC);
}

/*******************************************************************************
* Store Index X in Memory
*
* X -> M
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
void mos65xx_STX(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	mos65xx_addr address;

	switch (opCode) {
		case 0x86: // zeropage
			pProcessor->cycles.currentOp = 3;
			break;
		case 0x96: // zeropage,Y
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x8E: // absolute
			pProcessor->cycles.currentOp = 4;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	address = addrm_functions[addrm](pProcessor);
	addrm_write8(pProcessor, address, pProcessor->reg.X);
}

/*******************************************************************************
* Store Index Y in Memory
*
* Y -> M
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
void mos65xx_STY(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	mos65xx_addr address = 0;

	switch (opCode) {
		case 0x84: // zeropage
			pProcessor->cycles.currentOp = 3;
			break;
		case 0x8C: // absolute
			pProcessor->cycles.currentOp = 4;
			break;
		case 0x94: // zeropage,X
			pProcessor->cycles.currentOp = 4;
			break;
		default:
			pProcessor->cycles.currentOp = 0;
	}

	address = addrm_functions[addrm](pProcessor);
	addrm_write8(pProcessor, address, pProcessor->reg.Y);
}

/*******************************************************************************
* Transfer Accumulator to Index X
*
* A -> X
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
void mos65xx_TAX(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->reg.X = pProcessor->reg.AC;

	SR_COND_SET_N(pProcessor->reg.X & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.X == 0);
	pProcessor->cycles.currentOp += 1;
}

/*******************************************************************************
* Transfer Accumulator to Index Y
*
* A -> Y
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
void mos65xx_TAY(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->reg.Y = pProcessor->reg.AC;

	/* Update status register zero and negative flags. */
	SR_COND_SET_N(pProcessor->reg.AC & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.AC == 0);
	pProcessor->cycles.currentOp += 1;
}

/*******************************************************************************
* Transfer Stack Pointer to Index X
*
* SP -> X
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
void mos65xx_TSX(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->reg.X = pProcessor->reg.SP;

	SR_COND_SET_N(CHK_IF_N(pProcessor->reg.X));
	SR_COND_SET_Z(CHK_IF_Z(pProcessor->reg.X));
	pProcessor->cycles.currentOp += 1;
}

/*******************************************************************************
* Transfer Index X to Accumulator
*
* X -> A
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
void mos65xx_TXA(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->reg.AC = pProcessor->reg.X;

	SR_COND_SET_N(CHK_IF_N(pProcessor->reg.AC));
	SR_COND_SET_Z(CHK_IF_Z(pProcessor->reg.AC));
	pProcessor->cycles.currentOp += 1;
}

/*******************************************************************************
* Transfer Index X to Stack
*
* X -> SP
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
void mos65xx_TXS(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->reg.SP = pProcessor->reg.X;
	pProcessor->cycles.currentOp += 1;
}

/*******************************************************************************
* Transfer Index Y to Accumulator
*
* Y -> A
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
void mos65xx_TYA(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->reg.AC = pProcessor->reg.Y;

	SR_COND_SET_N(pProcessor->reg.AC & SR_FLAG_N);
	SR_COND_SET_Z(pProcessor->reg.AC == 0);
	pProcessor->cycles.currentOp += 1;
}

/*******************************************************************************
* Interrupt
*
* push PC+2, push SR, ($FFFE) -> PC
*
* N Z C I D V
* - - - 1 - -
*******************************************************************************/
void mos65xx_IRQ(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 7;

	addrm_stackPush16(pProcessor, pProcessor->reg.PC);
	addrm_stackPush8(pProcessor, pProcessor->reg.SR | SR_FLAG_UNUSED);

	pProcessor->reg.PC = addrm_read16(pProcessor, 0xFFFE);

	SR_SET_I();
}

/*******************************************************************************
* Common private functions
*******************************************************************************/
/*******************************************************************************
* Branch if condition is met.
*******************************************************************************/
void mos65xx_branch(Processor_65xx * pProcessor, U8 condition) {

	mos65xx_addr oldPC = pProcessor->reg.PC;
	mos65xx_addr branchAddr = addrm_REL(pProcessor);

	if (condition) {
		pProcessor->reg.PC = branchAddr;
		pProcessor->cycles.currentOp += 1 + MOS65xx_OUTOFPAGE(oldPC, branchAddr);
	}
}

/*******************************************************************************
* Binary add/subtract function.
*******************************************************************************/
U8 mos65xx_binAdd(Processor_65xx * pProcessor, U8 lValue, U8 rValue) {

	U8 origLValue = lValue;

	lValue += rValue + SR_GET_C();

	SR_COND_SET_V(
		!((origLValue & 0x80) ^ (rValue & 0x80)) // Both values have same sign
	&&	(lValue & 0x80) ^ (rValue & 0x80) // Output signedness differs from values.
	);

	SR_COND_SET_C(origLValue > lValue || (!CHK_IF_Z(rValue) && origLValue == lValue));
	SR_COND_SET_N(CHK_IF_N(lValue));
	SR_COND_SET_Z(CHK_IF_Z(lValue));

	return lValue;
}

/*******************************************************************************
* Decimal add/subtract function.
*******************************************************************************/
U8 mos65xx_decAdd(Processor_65xx * pProcessor, U8 lValue, U8 rValue, I8 operator) {

	U8 retVal = 0;

	lValue = DEC_TO_BIN(lValue);
	rValue = DEC_TO_BIN(rValue);

	if (SR_GET_C()) {
		retVal = lValue + (operator * rValue) + (operator == 1 ? 1 : 0);
	} else {
		retVal = lValue + (operator * rValue) - (operator == 1 ? 0 : 1);
	}

	if (operator == 1) {
		if (retVal < lValue) {
			retVal = (0xFF - lValue) + retVal;
		}
		SR_COND_SET_C(retVal > 99 || retVal < lValue);
	} else {
		if (retVal > lValue) {
			retVal = 99 - (0xFF - retVal);
		}
		SR_COND_SET_C(retVal < lValue || (rValue == 0 && SR_GET_C()));
	}

	while (retVal > 99) retVal -= 100;

	retVal = BIN_TO_DEC(retVal);

	SR_COND_SET_N(CHK_IF_N(retVal));
	SR_COND_SET_Z(CHK_IF_Z(retVal));

	return retVal;
}

/*******************************************************************************
* Comparison function.
*******************************************************************************/
void mos65xx_compare(Processor_65xx * pProcessor, U8 lValue, U8 rValue) {
	SR_COND_SET_N(CHK_IF_N(lValue - rValue));
	SR_COND_SET_Z(lValue == rValue);
	SR_COND_SET_C(lValue >= rValue);
}

/*******************************************************************************
* Undocumented opcodes
*******************************************************************************/
/*******************************************************************************
* AND oper + LSR
*
* A AND oper, 0 -> [76543210] -> C
*
* N Z C I D V
* + + + - - -
*******************************************************************************/
void mos65xx_ALR(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(opCode);
	
	mos65xx_addr address = addrm_functions[addrm](pProcessor);

	pProcessor->reg.AC &= addrm_read8(pProcessor, address);

	SR_COND_SET_C(pProcessor->reg.AC & SR_FLAG_C);

	pProcessor->reg.AC >>= 1;

	SR_COND_SET_Z(pProcessor->reg.AC == 0);
	SR_RESET_N();
	pProcessor->cycles.currentOp += 1;
}

/*******************************************************************************
* AND oper + set C as ASL
*
* A AND oper, bit(7) -> C
*
* N Z C I D V
* + + + - - -
*******************************************************************************/
void mos65xx_ANC(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(opCode);
	
	mos65xx_addr address = addrm_functions[addrm](pProcessor);

	pProcessor->reg.AC &= addrm_read8(pProcessor, address);

	SR_COND_SET_Z(pProcessor->reg.AC == 0);
	SR_COND_SET_N(pProcessor->reg.AC & SR_FLAG_N);
	SR_COND_SET_C(pProcessor->reg.AC & SR_FLAG_N);
	pProcessor->cycles.currentOp += 1;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
void mos65xx_ANE(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	
	pProcessor->cycles.currentOp = 0;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
void mos65xx_ARR(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 0;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
void mos65xx_DCP(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 0;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
void mos65xx_ISC(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 0;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
void mos65xx_LAS(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 0;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
void mos65xx_LAX(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 0;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
void mos65xx_LXA(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 0;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
void mos65xx_ILLNOP(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {

	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 0;
}

/*******************************************************************************
* JAMs the processor
*******************************************************************************/
void mos65xx_JAM(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);

	pProcessor->cycles.currentOp = 0;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
void mos65xx_RLA(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 0;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
void mos65xx_RRA(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 0;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
void mos65xx_SAX(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 0;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
void mos65xx_SBX(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 0;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
void mos65xx_SHA(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 0;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
void mos65xx_SHX(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 0;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
void mos65xx_SHY(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 0;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
void mos65xx_SLO(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 0;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
void mos65xx_SRE(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 0;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
void mos65xx_TAS(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 0;
}

/*******************************************************************************
* 
*
*
*
*
*
*******************************************************************************/
void mos65xx_USBC(Processor_65xx * pProcessor, U8 opCode, mos65xx_addrm addrm) {
	
	UNUSED(pProcessor);
	UNUSED(opCode);
	UNUSED(addrm);
	
	pProcessor->cycles.currentOp = 0;
}
