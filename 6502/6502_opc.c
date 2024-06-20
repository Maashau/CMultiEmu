/*******************************************************************************
* 6502_opc.c
*
* 6502 instruction implementations.
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "6502.h"
#include "6502_opc.h"
#include "6502_addrm.h"

/*******************************************************************************
* Add Memory to Accumulator with Carry.
*
* A + M + C -> A, C
*
* N Z C I D V
* + + + - - +
*******************************************************************************/
U8 mos6502_ADC(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;
	U8 memValue = 0;
	U8 tempAc = pProcessor->reg.AC;

	pProcessor->reg.SR &= ~(SR_FLAG_NEGATIVE | SR_FLAG_ZERO | SR_FLAG_CARRY | SR_FLAG_OVERFLOW);

	// TODO implement decimal mode (BCD)

	switch (opCode) {
		case 0x61: // (indirect,X)
			memValue = pProcessor->memIf.read8(addrm_indexedIndirect(pProcessor));
			cyclesPassed = 6;
			break;
		case 0x65: // zeropage
			memValue = pProcessor->memIf.read8(addrm_zeropage(pProcessor));
			cyclesPassed = 3;
			break;
		case 0x69: // immediate
			memValue = pProcessor->memIf.read8(addrm_immediate(pProcessor));
			cyclesPassed = 2;
			break;
		case 0x6D: // absolute
			memValue = pProcessor->memIf.read8(addrm_absolute(pProcessor));
			cyclesPassed = 4;
			break;
		case 0x71: {// (indirect),Y
			mos6502_addr address = addrm_indirectIndexed(pProcessor);
			memValue = pProcessor->memIf.read8(address);
			cyclesPassed = 5 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, address);
			break;
		}
		case 0x75: // zeropage,X
			memValue = pProcessor->memIf.read8(addrm_zeropageXind(pProcessor));
			cyclesPassed = 4;
			break;
		case 0x79: {// absolute,Y
			mos6502_addr address = addrm_absoluteYind(pProcessor);
			memValue = pProcessor->memIf.read8(address);
			cyclesPassed = 4 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, address);
			break;
		}
		case 0x7D: {// absolute,X
			mos6502_addr address = addrm_absoluteXind(pProcessor);
			memValue = pProcessor->memIf.read8(address);
			cyclesPassed = 4 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, address);
			break;
		}
		default:
			return 0xFF;
	}
			
	pProcessor->reg.AC += memValue;
	
	if (
		!((tempAc & 0x80) ^ (memValue & 0x80)) // Both values have same sign
	&&	(pProcessor->reg.AC & 0x80) ^ (memValue & 0x80) // Output signedness differs from values.
	) {
		pProcessor->reg.SR |= SR_FLAG_OVERFLOW;
	}

	
	pProcessor->reg.SR |= ((U16)tempAc + (U16)memValue > 255) ? SR_FLAG_CARRY : 0;
	pProcessor->reg.SR |= pProcessor->reg.AC & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.AC == 0 ? SR_FLAG_ZERO : 0;
		
	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_ALR(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_ANC(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_AND(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_ANE(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_ARR(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_ASL(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_BCC(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	cyclesPassed = 2;

	if (!(pProcessor->reg.SR & SR_FLAG_CARRY))  {

		mos6502_addr branchAddress = addrm_relative(pProcessor);

		cyclesPassed += 1 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, branchAddress);
		
		pProcessor->reg.PC = branchAddress;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_BCS(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	cyclesPassed = 2;

	if (pProcessor->reg.SR & SR_FLAG_CARRY) {

		mos6502_addr branchAddress = addrm_relative(pProcessor);

		cyclesPassed += 1 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, branchAddress);
		
		pProcessor->reg.PC = branchAddress;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_BEQ(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	cyclesPassed = 2;

	if (pProcessor->reg.SR & SR_FLAG_ZERO) {

		mos6502_addr branchAddress = addrm_relative(pProcessor);

		cyclesPassed += 1 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, branchAddress);
		
		pProcessor->reg.PC = branchAddress;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_BIT(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;
	U8 memValue = 0;
	mos6502_addr address = 0;


	pProcessor->reg.SR &= ~(SR_FLAG_NEGATIVE | SR_FLAG_ZERO | SR_FLAG_OVERFLOW);

	switch (opCode) {
		case 0x24: // zeropage
			address = addrm_zeropage(pProcessor);
			memValue = pProcessor->memIf.read8(address);
			cyclesPassed = 3;
			break;
		case 0x2C: // absolute
			address = addrm_absolute(pProcessor);
			memValue = pProcessor->memIf.read8(address);
			cyclesPassed = 4;
			break;
		default:
			return 0xFF;
	}

	pProcessor->reg.SR |= memValue & (SR_FLAG_ZERO | SR_FLAG_OVERFLOW);
	pProcessor->reg.SR |= memValue & pProcessor->reg.AC == 0 ? SR_FLAG_ZERO : 0;

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_BMI(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	cyclesPassed = 2;

	if (pProcessor->reg.SR & SR_FLAG_NEGATIVE) {
		mos6502_addr branchAddress = addrm_relative(pProcessor);

		cyclesPassed += 1 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, branchAddress);
		
		pProcessor->reg.PC = branchAddress;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

	return cyclesPassed;
}

/*******************************************************************************
* Branch on Result not Zero
*
* branch on Z = 0
*******************************************************************************/
U8 mos6502_BNE(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	cyclesPassed = 2;

	if (!(pProcessor->reg.SR & SR_FLAG_ZERO)) {
		mos6502_addr branchAddress = addrm_relative(pProcessor);

		cyclesPassed += 1 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, branchAddress);
		
		pProcessor->reg.PC = branchAddress;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_BPL(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_BRK(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


// TODO use addrm functions
	U16 address = pProcessor->memIf.read16(0xFFFE);

	// TODO

	return 0xFF;
	exit(0);

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_BVC(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_BVS(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_CLC(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	pProcessor->reg.SR &= ~SR_FLAG_CARRY;

	cyclesPassed = 2;

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_CLD(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_CLI(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_CLV(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

	return cyclesPassed;
}

/*******************************************************************************
* Compare Memory with Accumulator
*
* A - M
*
* N Z C I D V
* + + + - - -
*******************************************************************************/
U8 mos6502_CMP(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;
	U8 memValue = 0;
	U16 address = 0;


	pProcessor->reg.SR &= ~(SR_FLAG_ZERO | SR_FLAG_NEGATIVE | SR_FLAG_CARRY);

	switch (opCode) {
		case 0xC1: // (indirect,X)
			address = addrm_indexedIndirect(pProcessor);
			memValue = pProcessor->memIf.read8(address);
			cyclesPassed = 6;
			break;
		case 0xC5: // zeropage
			address = addrm_zeropage(pProcessor);
			memValue = pProcessor->memIf.read8(address);
			cyclesPassed = 3;
			break;
		case 0xC9: // immediate
			address = addrm_immediate(pProcessor);
			memValue = pProcessor->memIf.read8(address);
			cyclesPassed = 2;
			break;
		case 0xCD: // absolute
			address = addrm_absolute(pProcessor);
			memValue = pProcessor->memIf.read8(address);
			cyclesPassed = 4;
			break;
		case 0xD1: // (indirect),Y
			address = addrm_indirectIndexed(pProcessor);
			memValue = pProcessor->memIf.read8(address);
			cyclesPassed = 5 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, address);
			break;
		case 0xD5: // zeropage,X
			address = addrm_zeropageXind(pProcessor);
			memValue = pProcessor->memIf.read8(address);
			cyclesPassed = 4;
			break;
		case 0xDD: // absolute,X
			address = addrm_absoluteXind(pProcessor);
			memValue = pProcessor->memIf.read8(address);
			cyclesPassed = 4;
			break;
		case 0xD9: // absolute,Y
			address = addrm_absoluteYind(pProcessor);
			memValue = pProcessor->memIf.read8(address);
			cyclesPassed = 4;
			break;
		default:
			return 0xFF;
	}
	
	pProcessor->reg.SR |= (pProcessor->reg.AC - memValue) & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.AC == memValue ? SR_FLAG_ZERO : 0;
	pProcessor->reg.SR |= pProcessor->reg.AC >= memValue ? SR_FLAG_CARRY : 0;

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_CPX(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;
	U8 memValue = 0;


	pProcessor->reg.SR &= ~(SR_FLAG_ZERO | SR_FLAG_NEGATIVE | SR_FLAG_CARRY);

	switch (opCode) {
		case 0xE0: // immediate
			memValue = pProcessor->memIf.read8(addrm_immediate(pProcessor));
			cyclesPassed = 2;
			break;
		case 0xE4: // zeropage
			memValue = pProcessor->memIf.read8(addrm_zeropage(pProcessor));
			cyclesPassed = 3;
			break;
		case 0xEC: // absolute
			memValue = pProcessor->memIf.read8(addrm_absolute(pProcessor));
			cyclesPassed = 4;
			break;
		default:
			return 0xFF;
	}
	
	pProcessor->reg.SR |= (pProcessor->reg.X - memValue) & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.X == memValue ? SR_FLAG_ZERO : 0;
	pProcessor->reg.SR |= pProcessor->reg.X >= memValue ? SR_FLAG_CARRY : 0;
	
	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_CPY(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;
	U8 memValue = 0;

	
	pProcessor->reg.SR &= ~(SR_FLAG_ZERO | SR_FLAG_NEGATIVE | SR_FLAG_CARRY);

	switch (opCode) {
		case 0xC0: // immediate
			memValue = pProcessor->memIf.read8(addrm_immediate(pProcessor));
			cyclesPassed = 2;
			break;
		case 0xC4: // zeropage
			memValue = pProcessor->memIf.read8(addrm_zeropage(pProcessor));
			cyclesPassed = 3;
			break;
		case 0xCC: // absolute
			memValue = pProcessor->memIf.read8(addrm_absolute(pProcessor));
			cyclesPassed = 4;
			break;
		default:
			return 0xFF;
	}
	
	pProcessor->reg.SR |= (pProcessor->reg.Y - memValue) & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.Y == memValue ? SR_FLAG_ZERO : 0;
	pProcessor->reg.SR |= pProcessor->reg.Y >= memValue ? SR_FLAG_CARRY : 0;
	
	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_DCP(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_DEC(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_DEX(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	pProcessor->reg.SR &= ~(SR_FLAG_ZERO | SR_FLAG_NEGATIVE);

	pProcessor->reg.X--;

	pProcessor->reg.SR |= pProcessor->reg.X & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.X == 0 ? SR_FLAG_ZERO : 0;

	cyclesPassed = 2;

	pProcessor->reg.PC += pOpCodeData->bytes;

	return cyclesPassed;
}

/*******************************************************************************
* Decrement Index Y by One
*
* Y - 1 -> Y
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_DEY(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	pProcessor->reg.SR &= ~(SR_FLAG_ZERO | SR_FLAG_NEGATIVE);

	pProcessor->reg.Y--;

	pProcessor->reg.SR |= pProcessor->reg.Y & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.Y == 0 ? SR_FLAG_ZERO : 0;

	cyclesPassed = 2;

	pProcessor->reg.PC += pOpCodeData->bytes;

	return cyclesPassed;
}

/*******************************************************************************
* Exclusive-OR Memory with Accumulator
*
* A EOR M -> A
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_EOR(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;
	U8 tempAc = pProcessor->reg.AC;
	U8 memValue = 0;

	
	pProcessor->reg.SR &= ~(SR_FLAG_ZERO | SR_FLAG_NEGATIVE);

	switch (opCode) {
		case 0x41: // (indirect, X)
			memValue = pProcessor->memIf.read8(addrm_indexedIndirect(pProcessor));
			cyclesPassed = 6;
			break;
		case 0x45: // zeropage
			memValue = pProcessor->memIf.read8(addrm_zeropage(pProcessor));
			cyclesPassed = 3;
			break;
		case 0x49: // immediate
			memValue = pProcessor->memIf.read8(addrm_immediate(pProcessor));
			cyclesPassed = 2;
			break;
		case 0x4D: // absolute
			memValue = pProcessor->memIf.read8(addrm_absolute(pProcessor));
			cyclesPassed = 4;
			break;
		case 0x51: {// (indirect), Y
			mos6502_addr address = addrm_indirectIndexed(pProcessor);
			memValue = pProcessor->memIf.read8(address);
			cyclesPassed = 6 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, address);
			break;
		}
		case 0x55: // zeropage, X
			memValue = pProcessor->memIf.read8(addrm_zeropageXind(pProcessor));
			cyclesPassed = 4;
			break;
		case 0x59: {// absolute, Y
			mos6502_addr address = addrm_absoluteYind(pProcessor);
			memValue = pProcessor->memIf.read8(address);
			cyclesPassed = 4 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, address);
			break;
		}
		case 0x5D: {// absolute, X
			mos6502_addr address = addrm_absoluteXind(pProcessor);
			memValue = pProcessor->memIf.read8(address);
			cyclesPassed = 4 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, address);
			break;
		}
		default:
			return 0xFF;
	}

	pProcessor->reg.AC ^= memValue;
	
	pProcessor->reg.SR |= pProcessor->reg.AC & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.AC == 0 ? SR_FLAG_ZERO : 0;

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_INC(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_INX(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

	return cyclesPassed;
}

/*******************************************************************************
* Increment Index Y by One
*
* Y + 1 -> Y
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_INY(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	pProcessor->reg.SR &= ~(SR_FLAG_ZERO | SR_FLAG_NEGATIVE);

	pProcessor->reg.Y++;

	cyclesPassed = 2;

	pProcessor->reg.SR |= pProcessor->reg.Y & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.Y == 0 ? SR_FLAG_ZERO : 0;

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_ISC(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_JAM(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_JMP(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

	return cyclesPassed;
}

/*******************************************************************************
* Jump to New Location Saving Return Address
*
* push (PC+2),
* operand 1st byte -> PCL
* operand 2nd byte -> PCH
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_JSR(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;
	U16 storedAddress = 0;


	addrm_stackPush16(pProcessor, pProcessor->reg.PC + 2);

	storedAddress = pProcessor->reg.PC;

	pProcessor->reg.PC = addrm_absolute(pProcessor);

	cyclesPassed = 6;

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
U8 mos6502_LAS(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_LAX(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

	return cyclesPassed;
}

/*******************************************************************************
* Load Accumulator with Memory.
*
* M -> A
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_LDA(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;
	U16 address = 0;


	pProcessor->reg.SR &= ~(SR_FLAG_ZERO | SR_FLAG_NEGATIVE);

	switch (opCode) {
		case 0xA1: // (indirect, X)
			address = addrm_indexedIndirect(pProcessor);
			pProcessor->reg.AC = pProcessor->memIf.read8(address);
			cyclesPassed = 6;
			break;
		case 0xA5: // zeropage
			address = addrm_zeropage(pProcessor);
			pProcessor->reg.AC = pProcessor->memIf.read8(address);
			cyclesPassed = 3;
			break;
		case 0xA9:	// Immediate
			address = addrm_immediate(pProcessor);
			pProcessor->reg.AC = pProcessor->memIf.read8(address);
			cyclesPassed = 2;
			break;
		case 0xAD: // absolute
			address = addrm_absolute(pProcessor);
			pProcessor->reg.AC = pProcessor->memIf.read8(address);
			cyclesPassed = 4;
			break;
		case 0xB1: // (indirect), Y
			address = addrm_indirectIndexed(pProcessor);
			pProcessor->reg.AC = pProcessor->memIf.read8(address);
			cyclesPassed = 5 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, address);
			break;
		case 0xB5: // zeropage, X
			address = addrm_zeropageXind(pProcessor);
			pProcessor->reg.AC = pProcessor->memIf.read8(address);
			cyclesPassed = 4;
			break;
		case 0xB9: // absolute, Y
			address = addrm_absoluteYind(pProcessor);
			pProcessor->reg.AC = pProcessor->memIf.read8(address);
			cyclesPassed = 5 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, address);
			break;
		case 0xBD: // absolute, X
			address = addrm_absoluteXind(pProcessor);
			pProcessor->reg.AC = pProcessor->memIf.read8(address);
			cyclesPassed = 6;
			break;
		default:
			return 0xFF;
	}

	/* Update status register zero and negative flags. */
	pProcessor->reg.SR |= pProcessor->reg.AC & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.AC == 0 ? SR_FLAG_ZERO : 0;
	
	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_LDX(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;
	mos6502_addr address = 0;


	pProcessor->reg.SR &= ~(SR_FLAG_NEGATIVE | SR_FLAG_ZERO);

	switch (opCode) {
		case 0xA2: // immediate
			address = addrm_immediate(pProcessor);
			pProcessor->reg.X = pProcessor->memIf.read8(address);
			cyclesPassed = 2;
			break;
		case 0xA6: // zeropage
			address = addrm_zeropage(pProcessor);
			pProcessor->reg.X = pProcessor->memIf.read8(address);
			cyclesPassed = 3;
			break;
		case 0xB6: // zeropage,Y
			address = addrm_zeropageYind(pProcessor);
			pProcessor->reg.X = pProcessor->memIf.read8(address);
			cyclesPassed = 4;
			break;
		case 0xAE: // absolute
			address = addrm_absolute(pProcessor);
			pProcessor->reg.X = pProcessor->memIf.read8(address);
			cyclesPassed = 4;
			break;
		case 0xBE: // absolute,Y
			address = addrm_absoluteYind(pProcessor);
			pProcessor->reg.X = pProcessor->memIf.read8(address);
			cyclesPassed = 4 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, address);
			break;
		default:
			return 0xFF;
	}

	pProcessor->reg.SR |= pProcessor->reg.X & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.X == 0 ? SR_FLAG_ZERO : 0;

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_LDY(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;
	mos6502_addr address = 0;


	pProcessor->reg.SR &= ~(SR_FLAG_NEGATIVE | SR_FLAG_ZERO);

	switch (opCode) {
		case 0xA0: //immediate
			address = addrm_immediate(pProcessor);
			pProcessor->reg.Y = pProcessor->memIf.read8(address);
			cyclesPassed = 2;
			break;
		case 0xA4: //zeropage
			address = addrm_zeropage(pProcessor);
			pProcessor->reg.Y = pProcessor->memIf.read8(address);
			cyclesPassed = 3;
			break;
		case 0xB4: //zeropage,X
			address = addrm_zeropageXind(pProcessor);
			pProcessor->reg.Y = pProcessor->memIf.read8(address);
			cyclesPassed = 4;
			break;
		case 0xAC: //absolute
			address = addrm_absolute(pProcessor);
			pProcessor->reg.Y = pProcessor->memIf.read8(address);
			cyclesPassed = 4;
			break;
		case 0xBC: //absolute,X
			address = addrm_absoluteXind(pProcessor);
			pProcessor->reg.Y = pProcessor->memIf.read8(address);
			cyclesPassed = 4 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, address);
			break;
		default:
			return 0xFF;
	}

	pProcessor->reg.SR |= pProcessor->reg.Y & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.Y == 0 ? SR_FLAG_ZERO : 0;

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_LSR(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;
	U8 origVal = 0;
	U8 shiftedVal = 0;

	
	pProcessor->reg.SR &= ~(SR_FLAG_NEGATIVE | SR_FLAG_ZERO | SR_FLAG_CARRY);

	switch (opCode) {
		case 0x4A: // accumulator
			origVal = pProcessor->reg.AC;
			pProcessor->reg.SR |= origVal & SR_FLAG_CARRY;
			pProcessor->reg.AC = origVal >> 1;
			shiftedVal = pProcessor->reg.AC;
			pProcessor->reg.SR |= shiftedVal == 0 ? SR_FLAG_ZERO : 0;
			cyclesPassed = 2;
			break;
		case 0x46: // zeropage
			origVal = pProcessor->memIf.read8(addrm_zeropage(pProcessor));
			pProcessor->reg.SR |= origVal & SR_FLAG_CARRY;
			pProcessor->memIf.write8(addrm_zeropage(pProcessor), origVal >> 1);
			shiftedVal = pProcessor->memIf.read8(addrm_zeropage(pProcessor));
			pProcessor->reg.SR |= shiftedVal == 0 ? SR_FLAG_ZERO : 0;
			cyclesPassed = 5;
			break;
		case 0x56: // zeropage,X
			origVal = pProcessor->memIf.read8(addrm_zeropageXind(pProcessor));
			pProcessor->reg.SR |= origVal & SR_FLAG_CARRY;
			pProcessor->memIf.write8(addrm_zeropageXind(pProcessor), origVal >> 1);
			shiftedVal = pProcessor->memIf.read8(addrm_zeropageXind(pProcessor));
			pProcessor->reg.SR |= shiftedVal == 0 ? SR_FLAG_ZERO : 0;
			cyclesPassed = 6;
			break;
		case 0x4E: // absolute
			origVal = pProcessor->memIf.read8(addrm_absolute(pProcessor));
			pProcessor->reg.SR |= origVal & SR_FLAG_CARRY;
			pProcessor->memIf.write8(addrm_absolute(pProcessor), origVal >> 1);
			shiftedVal = pProcessor->memIf.read8(addrm_absolute(pProcessor));
			pProcessor->reg.SR |= shiftedVal == 0 ? SR_FLAG_ZERO : 0;
			cyclesPassed = 6;
			break;
		case 0x5E: // absolute,X
			origVal = pProcessor->memIf.read8(addrm_absoluteXind(pProcessor));
			pProcessor->reg.SR |= origVal & SR_FLAG_CARRY;
			pProcessor->memIf.write8(addrm_absoluteXind(pProcessor), origVal >> 1);
			shiftedVal = pProcessor->memIf.read8(addrm_absoluteXind(pProcessor));
			pProcessor->reg.SR |= shiftedVal == 0 ? SR_FLAG_ZERO : 0;
			cyclesPassed = 7;
			break;
		default:
			return 0xFF;
	}

	pProcessor->reg.SR |= SR_FLAG_NEGATIVE;

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_LXA(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_NOP(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_ORA(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_PHA(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	addrm_stackPush8(pProcessor, pProcessor->reg.AC);

	cyclesPassed = 3;

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_PHP(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

	return cyclesPassed;
}

/*******************************************************************************
* Pull Accumulator from Stack
*
* pull A
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_PLA(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	pProcessor->reg.SR &= ~(SR_FLAG_NEGATIVE | SR_FLAG_ZERO);

	pProcessor->reg.AC = addrm_stackPop8(pProcessor);

	pProcessor->reg.SR |= pProcessor->reg.AC & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.AC == 0 ? SR_FLAG_ZERO : 0;

	cyclesPassed = 4;

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_PLP(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_RLA(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_ROL(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_ROR(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_RRA(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_RTI(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

	return cyclesPassed;
}

/*******************************************************************************
* Return from Subroutine
*
* pull PC, PC+1 -> PC
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_RTS(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	pProcessor->reg.PC = addrm_stackPop16(pProcessor);
	
	cyclesPassed = 6;

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_SAX(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

	return cyclesPassed;
}

/*******************************************************************************
* Subtract Memory from Accumulator with Borrow
*
* A - M - C� -> A
*
* N Z C I D V
* + + + - - +
*******************************************************************************/
U8 mos6502_SBC(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;
	U8 memValue = 0;
	U8 tempAc = pProcessor->reg.AC;


	// TODO implement decimal mode (BCD)
	switch (opCode) {
		case 0xE5: // zeropage
			memValue = pProcessor->memIf.read8(addrm_zeropage(pProcessor));
			cyclesPassed = 3;
			break;
		case 0xED: // absolute
			memValue = pProcessor->memIf.read8(addrm_absolute(pProcessor));
			cyclesPassed = 4;
			break;
		case 0xE9: // immediate
			memValue = pProcessor->memIf.read8(addrm_immediate(pProcessor));
			cyclesPassed = 4;
			break;
		case 0xF5: // zeropage,X
		case 0xFD: // absolute,X
		case 0xF9: // absolute,Y
		case 0xE1: // (indirect,X)
		case 0xF1: // (indirect),Y
			return 0xFF;
	}
			
	pProcessor->reg.AC += ~memValue;
	if (pProcessor->reg.SR & SR_FLAG_CARRY && tempAc < pProcessor->reg.AC) {
		pProcessor->reg.AC += 1;
		pProcessor->reg.SR &= ~(SR_FLAG_CARRY);
	} else {
		pProcessor->reg.SR |= (tempAc < pProcessor->reg.AC) ? SR_FLAG_CARRY : 0;
	}
	
	if (
		!((tempAc & 0x80) ^ (~memValue & 0x80)) // Both values have same sign
	&&	(pProcessor->reg.AC & 0x80) ^ (~memValue & 0x80) // Output signedness differs from values.
	) {
		pProcessor->reg.SR |= SR_FLAG_OVERFLOW;
	}

	pProcessor->reg.SR |= pProcessor->reg.AC & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.AC == 0 ? SR_FLAG_ZERO : 0;
	
	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_SBX(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_SEC(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_SED(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_SEI(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_SHA(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_SHX(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_SHY(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_SLO(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_SRE(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

	return cyclesPassed;
}

/*******************************************************************************
* Store Accumulator in Memory
*
* A -> M
*
* N Z C I D V
* - - - - - -
*******************************************************************************/
U8 mos6502_STA(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	U16 destAddr;

	switch (opCode) {	
		case 0x85: // zeropage
			destAddr = addrm_zeropage(pProcessor);
			cyclesPassed = 3;
			break;
		case 0x8D: // absolute
			destAddr = addrm_absolute(pProcessor);
			cyclesPassed = 4;
			break;
		case 0x91: // (indirect),Y
			destAddr = addrm_indirectIndexed(pProcessor);
			cyclesPassed = 6;
			break;
		case 0x81: // (indirect,X)
		case 0x95: // zeropage,X
		case 0x99: // absolute,Y
		case 0x9D: // absolute,X
			return 0xFF;
	}

	pProcessor->memIf.write8(destAddr, pProcessor->reg.AC);

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_STX(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_STY(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;
	U16 address = 0;


	switch (opCode) {
		case 0x84: // zeropage
			address = addrm_zeropage(pProcessor);
			pProcessor->memIf.write8(address, pProcessor->reg.Y);
			cyclesPassed = 3;
			break;
		case 0x8C: // absolute
		case 0x94: // zeropage,X
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_TAS(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

	return cyclesPassed;
}

/*******************************************************************************
* Transfer Accumulator to Index X
*
* A -> X
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_TAX(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	pProcessor->reg.SR &= ~(SR_FLAG_NEGATIVE | SR_FLAG_ZERO);

	pProcessor->reg.X = pProcessor->reg.AC;

	cyclesPassed = 2;

	pProcessor->reg.SR |= pProcessor->reg.X & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.X == 0 ? SR_FLAG_ZERO : 0;

	pProcessor->reg.PC += pOpCodeData->bytes;

	return cyclesPassed;
}

/*******************************************************************************
* Transfer Accumulator to Index Y
*
* A -> Y
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_TAY(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	pProcessor->reg.SR &= ~(SR_FLAG_ZERO | SR_FLAG_NEGATIVE);

	pProcessor->reg.Y = pProcessor->reg.AC;

	/* Update status register zero and negative flags. */
	pProcessor->reg.SR |= pProcessor->reg.AC & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.AC == 0 ? SR_FLAG_ZERO : 0;
	
	cyclesPassed = 2;

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_TSX(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_TXA(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_TXS(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

	return cyclesPassed;
}

/*******************************************************************************
* Transfer Index Y to Accumulator
*
* Y -> A
*
* N Z C I D V
* + + - - - -
*******************************************************************************/
U8 mos6502_TYA(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	pProcessor->reg.SR &= ~(SR_FLAG_NEGATIVE | SR_FLAG_ZERO);

	pProcessor->reg.AC = pProcessor->reg.Y;

	pProcessor->reg.SR |= pProcessor->reg.AC & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.AC == 0 ? SR_FLAG_ZERO : 0;

	cyclesPassed = 2;

	pProcessor->reg.PC += pOpCodeData->bytes;

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
U8 mos6502_USBC(
	mos6502_processor_st *	pProcessor, 
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;


	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

	return cyclesPassed;
}
