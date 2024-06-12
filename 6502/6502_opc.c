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

#define DBG_FN_PRINT											\
	printf(														\
		"0x%04X 0x%02X: %s: (%s%s%s%s%s%s)\n",					\
		pProcessor->reg.PC,										\
		opCode,											\
		__FUNCTION__,											\
		pProcessor->reg.SR & SR_FLAG_NEGATIVE	? "N " : "",	\
		pProcessor->reg.SR & SR_FLAG_ZERO		? "Z " : "",	\
		pProcessor->reg.SR & SR_FLAG_CARRY		? "C " : "",	\
		pProcessor->reg.SR & SR_FLAG_IRQ			? "I " : "",	\
		pProcessor->reg.SR & SR_FLAG_DECIMAL		? "D " : "",	\
		pProcessor->reg.SR & SR_FLAG_OVERFLOW	? "V " : ""		\
	)

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

	OP_PRINT(DBG_FN_PRINT);

	pProcessor->reg.SR &= ~(SR_FLAG_NEGATIVE | SR_FLAG_ZERO | SR_FLAG_CARRY | SR_FLAG_OVERFLOW);

	// TODO implement decimal mode (BCD)

	switch (opCode) {
		case 0x65: // zeropage
			memValue = pProcessor->memIf.read8(addrm_zeropage(pProcessor));
			cyclesPassed = 3;
			break;

		case 0x69: // immediate
			memValue = addrm_immediate(pProcessor);
			cyclesPassed = 2;
			break;

		case 0x6D: {// absolute
			memValue = pProcessor->memIf.read8(addrm_absolute(pProcessor));
			cyclesPassed = 4;
			break;
		}
		case 0x7D: {// absolute,X
			mos6502_addr address = addrm_absoluteXind(pProcessor);
			memValue = pProcessor->memIf.read8(address);
			cyclesPassed = 4 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, address);
			break;
		}
		case 0x75: // zeropage,X
		case 0x79: // absolute,Y
		case 0x61: // (indirect,X)
		case 0x71: // (indirect),Y
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
	
	OP_PRINT(
		printf(
			"\tAdded value %d/%d/0x%02X to Accumulator (%d/%d/0x%02X), result: %d/%d/0x%02X. %s%s%s%s\n\n",
			memValue,
			(I8)memValue,
			memValue,
			tempAc,
			(I8)tempAc,
			tempAc,
			pProcessor->reg.AC,
			(I8)pProcessor->reg.AC,
			pProcessor->reg.AC,
			pProcessor->reg.SR & SR_FLAG_NEGATIVE ? "N " : "",
			pProcessor->reg.SR & SR_FLAG_ZERO ? "Z " : "",
			pProcessor->reg.SR & SR_FLAG_CARRY ? "C " : "",
			pProcessor->reg.SR & SR_FLAG_OVERFLOW ? "O " : ""
		)
	);
	
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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

	cyclesPassed = 2;

	if (pProcessor->reg.SR & SR_FLAG_CARRY) {
		OP_PRINT(printf("\tCarry bit set, continued to next instruction\n\n"));

	} else {
		I8 offset = addrm_relative(pProcessor);
		mos6502_addr branchAddress = pProcessor->reg.PC + offset;

		OP_PRINT(
			printf(
				"\tCarry bit not set, branching to offset %d (address 0x%04X)\n\n",
				offset,
				branchAddress + pOpCodeData->bytes
			)
		);

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

	OP_PRINT(DBG_FN_PRINT);

	cyclesPassed = 2;

	if (!(pProcessor->reg.SR & SR_FLAG_CARRY)) {
		OP_PRINT(printf("\tCarry bit not set, continued to next instruction\n\n"));

	} else {
		I8 offset = addrm_relative(pProcessor);
		mos6502_addr branchAddress = pProcessor->reg.PC + offset;

		OP_PRINT(
			printf(
				"\tCarry bit set, branching to offset %d (address 0x%04X)\n\n",
				offset,
				branchAddress + pOpCodeData->bytes
			)
		);

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

	OP_PRINT(DBG_FN_PRINT);

	cyclesPassed = 2;

	if (!(pProcessor->reg.SR & SR_FLAG_ZERO)) {
		OP_PRINT(printf("\tZero bit not set, continued to next instruction\n\n"));

	} else {
		I8 offset = addrm_relative(pProcessor);
		mos6502_addr branchAddress = pProcessor->reg.PC + offset;

		OP_PRINT(
			printf(
				"\tZero bit set, branching to offset %d (address 0x%04X)\n\n",
				offset,
				branchAddress + pOpCodeData->bytes
			)
		);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(
		printf(
			"\tAccumulator (%d/0x%02X) - Memory (%d/0x%02X) (address: 0x%04X) bit test resulted to flags: %s%s%s\n\n",
			pProcessor->reg.AC,
			pProcessor->reg.AC,
			memValue,
			memValue,
			address,
			pProcessor->reg.SR & SR_FLAG_NEGATIVE ? "N ": "",
			pProcessor->reg.SR & SR_FLAG_ZERO ? "Z ": "",
			pProcessor->reg.SR & SR_FLAG_OVERFLOW ? "V ": ""
		)
	);

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

	OP_PRINT(DBG_FN_PRINT);

	cyclesPassed = 2;

	if (!(pProcessor->reg.SR & SR_FLAG_NEGATIVE)) {
		OP_PRINT(printf("\tNegative bit not set, continued to next instruction."));

	} else {
		I8 offset = addrm_relative(pProcessor);
		mos6502_addr branchAddress = pProcessor->reg.PC + offset;

		OP_PRINT(
			printf(
				"\tNegative bit set, branching to offset %d (address 0x%04X)\n\n",
				offset,
				branchAddress + pOpCodeData->bytes
			)
		);

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

	OP_PRINT(DBG_FN_PRINT);

	cyclesPassed = 2;

	if (pProcessor->reg.SR & SR_FLAG_ZERO) {
		OP_PRINT(printf("\tZero bit set, continued to next instruction."));

	} else {
		I8 offset = addrm_relative(pProcessor);
		mos6502_addr branchAddress = pProcessor->reg.PC + offset;

		OP_PRINT(
			printf(
				"\tZero bit not set, branching to offset %d (address 0x%04X)\n\n",
				offset,
				branchAddress + pOpCodeData->bytes
			)
		);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

// TODO use addrm functions
	U16 address = pProcessor->memIf.read16(0xFFFE);

	// TODO

	OP_PRINT(
		printf(
			"\tBreak on address 0x%04X (BRK handler address: 0x%04X)\n\n",
			pProcessor->reg.PC,
			address
		)
	);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

	pProcessor->reg.SR &= ~SR_FLAG_CARRY;

	cyclesPassed = 2;
	
	OP_PRINT(printf("\tCleared carry flag.\n\n"));

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

	pProcessor->reg.SR &= ~(SR_FLAG_ZERO | SR_FLAG_NEGATIVE | SR_FLAG_CARRY);

	switch (opCode) {
		case 0xC9: // immediate
			address = pProcessor->reg.PC + 1;
			memValue = pProcessor->memIf.read8(address);
			cyclesPassed = 2;
			break;
		case 0xD1: // (indirect),Y
			address = addrm_indirectIndexed(pProcessor);
			memValue = pProcessor->memIf.read8(address);
			cyclesPassed = 5 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, address);
			break;
		case 0xC5: // zeropage
		case 0xD5: // zeropage,X
		case 0xCD: // absolute
		case 0xDD: // absolute,X
		case 0xD9: // absolute,Y
		case 0xC1: // (indirect,X
		default:
			return 0xFF;
	}
	
	pProcessor->reg.SR |= (pProcessor->reg.AC - memValue) & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.AC == memValue ? SR_FLAG_ZERO : 0;
	pProcessor->reg.SR |= pProcessor->reg.AC >= memValue ? SR_FLAG_CARRY : 0;

	OP_PRINT(
		printf(
			"\tAccumulator (%d/0x%02X) - Memory (%d/0x%02X) (address: 0x%04X) comparison resulted to flags: %s%s%s\n\n",
			pProcessor->reg.AC,
			pProcessor->reg.AC,
			memValue,
			memValue,
			address,
			pProcessor->reg.SR & SR_FLAG_NEGATIVE ? "N ": "",
			pProcessor->reg.SR & SR_FLAG_ZERO ? "Z ": "",
			pProcessor->reg.SR & SR_FLAG_CARRY ? "C ": ""
		)
	);

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

	OP_PRINT(DBG_FN_PRINT);

	pProcessor->reg.SR &= ~(SR_FLAG_ZERO | SR_FLAG_NEGATIVE | SR_FLAG_CARRY);

	switch (opCode) {
		case 0xE0: // immediate
			memValue = addrm_immediate(pProcessor);
			cyclesPassed = 2;
			break;
		case 0xE4: // zeropage
		case 0xEC: // absolute
			return 0xFF;
	}
	
	pProcessor->reg.SR |= (pProcessor->reg.X - memValue) & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.X == memValue ? SR_FLAG_ZERO : 0;
	pProcessor->reg.SR |= pProcessor->reg.X >= memValue ? SR_FLAG_CARRY : 0;

	OP_PRINT(
		printf(
			"\tIndex X (%d/0x%02X) - Memory (%d/0x%02X) comparison resulted to flags: %s%s%s\n\n",
			pProcessor->reg.X,
			pProcessor->reg.X,
			memValue,
			memValue,
			pProcessor->reg.SR & SR_FLAG_NEGATIVE ? "N ": "",
			pProcessor->reg.SR & SR_FLAG_ZERO ? "Z ": "",
			pProcessor->reg.SR & SR_FLAG_CARRY ? "C ": ""
		)
	);
	
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

	OP_PRINT(DBG_FN_PRINT);
	
	pProcessor->reg.SR &= ~(SR_FLAG_ZERO | SR_FLAG_NEGATIVE | SR_FLAG_CARRY);

	switch (opCode) {
		case 0xC0: // immediate
			memValue = addrm_immediate(pProcessor);
			cyclesPassed = 2;
			break;
		case 0xC4: // zeropage
		case 0xCC: // absolute
			return 0xFF;
	}
	
	pProcessor->reg.SR |= (pProcessor->reg.Y - memValue) & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.Y == memValue ? SR_FLAG_ZERO : 0;
	pProcessor->reg.SR |= pProcessor->reg.Y >= memValue ? SR_FLAG_CARRY : 0;

	OP_PRINT(
		printf(
			"\tIndex Y (%d/0x%02X) - Memory (%d/0x%02X) comparison resulted to flags: %s%s%s\n\n",
			pProcessor->reg.Y,
			pProcessor->reg.Y,
			memValue,
			memValue,
			pProcessor->reg.SR & SR_FLAG_NEGATIVE ? "N ": "",
			pProcessor->reg.SR & SR_FLAG_ZERO ? "Z ": "",
			pProcessor->reg.SR & SR_FLAG_CARRY ? "C ": ""
		)
	);
	
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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

	pProcessor->reg.SR &= ~(SR_FLAG_ZERO | SR_FLAG_NEGATIVE);

	pProcessor->reg.X--;

	pProcessor->reg.SR |= pProcessor->reg.X & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.X == 0 ? SR_FLAG_ZERO : 0;

	cyclesPassed = 2;

	OP_PRINT(
		printf(
			"\tDecremented Index X (%d/%d -> %d/%d). %s%s\n\n",
			pProcessor->reg.X + 1,
			(I8)pProcessor->reg.X + 1,
			pProcessor->reg.X,
			(I8)pProcessor->reg.X,
			pProcessor->reg.SR & SR_FLAG_NEGATIVE ? "N " : "",
			pProcessor->reg.SR & SR_FLAG_ZERO ? "Z " : ""
		)
	);

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

	OP_PRINT(DBG_FN_PRINT);

	pProcessor->reg.SR &= ~(SR_FLAG_ZERO | SR_FLAG_NEGATIVE);

	pProcessor->reg.Y--;

	pProcessor->reg.SR |= pProcessor->reg.Y & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.Y == 0 ? SR_FLAG_ZERO : 0;

	cyclesPassed = 2;

	OP_PRINT(
		printf(
			"\tDecremented Index Y (%d/%d -> %d/%d). %s%s\n\n",
			pProcessor->reg.Y + 1,
			(I8)pProcessor->reg.Y + 1,
			pProcessor->reg.Y,
			(I8)pProcessor->reg.Y,
			pProcessor->reg.SR & SR_FLAG_NEGATIVE ? "N " : "",
			pProcessor->reg.SR & SR_FLAG_ZERO ? "Z " : ""
		)
	);

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

	OP_PRINT(DBG_FN_PRINT);
	
	pProcessor->reg.SR &= ~(SR_FLAG_ZERO | SR_FLAG_NEGATIVE);

	switch (opCode) {
		case 0x49: // immediate
			memValue = addrm_immediate(pProcessor);
			pProcessor->reg.AC ^= memValue;
			cyclesPassed = 2;
			break;
		case 0x45: // zeropage
		case 0x55: // zeropage, X
		case 0x4D: // absolute
		case 0x5D: // absolute, X
		case 0x59: // absolute, Y
		case 0x41: // (indirect, X)
		case 0x51: // (indirect), Y
			return 0xFF;
	}
	
	pProcessor->reg.SR |= pProcessor->reg.AC & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.AC == 0 ? SR_FLAG_ZERO : 0;

	OP_PRINT(
		printf(
			"\tExclusive-OR of Memory (%d / 0x%02X) and Accumulator (%d / 0x%02X) was %d / 0x%02X. %s%s\n\n",
			memValue,
			memValue,
			tempAc,
			tempAc,
			pProcessor->reg.AC,
			pProcessor->reg.AC,
			(pProcessor->reg.SR & SR_FLAG_NEGATIVE) ? "N set." : "",
			pProcessor->reg.SR & SR_FLAG_ZERO ? "Z set." : ""
		)
	);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

	pProcessor->reg.X++;

	cyclesPassed = 2;

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

	OP_PRINT(DBG_FN_PRINT);

	pProcessor->reg.SR &= ~(SR_FLAG_ZERO | SR_FLAG_NEGATIVE);

	pProcessor->reg.Y++;

	cyclesPassed = 2;

	OP_PRINT(
		printf(
			"\tIncremented Index Y (%d/%d -> %d/%d)\n\n",
			pProcessor->reg.Y - 1,
			(I8)pProcessor->reg.Y - 1,
			pProcessor->reg.Y,
			(I8)pProcessor->reg.Y
		)
	);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

	return cyclesPassed;
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
U8 mos6502_JMP(
	mos6502_processor_st *	pProcessor,
	U8						opCode, 
	opCode__st *			pOpCodeData
) {

	U8 cyclesPassed = 0;

	OP_PRINT(DBG_FN_PRINT);

	switch (opCode) {
		case 0x4C: // absolute
			pProcessor->reg.PC = addrm_absolute(pProcessor);
			cyclesPassed = 3;
			break;
		case 0x6C: // indirect
			pProcessor->reg.PC = addrm_indirect(pProcessor);
			cyclesPassed = 5;
			break;
		default:
			return 0xFF;
	}

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

	OP_PRINT(DBG_FN_PRINT);

	addrm_stackPush16(pProcessor, pProcessor->reg.PC + 2);

	storedAddress = pProcessor->reg.PC;

	pProcessor->reg.PC = addrm_absolute(pProcessor);

	cyclesPassed = 6;

	OP_PRINT(
		printf(
			"\tStored Return Address to stack ((PC)0x%04X + 2), Jump to location (0x%04X). SP=0x%04X\n\n",
			storedAddress,
			pProcessor->reg.PC,
			pProcessor->reg.SP + 0x100
		)
	);


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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

	pProcessor->reg.SR &= ~(SR_FLAG_ZERO | SR_FLAG_NEGATIVE);

	switch (opCode) {
		case 0xA9:	// Immediate
			pProcessor->reg.AC = addrm_immediate(pProcessor);
			cyclesPassed = 2;
			break;
		case 0xB1: // (indirect), Y
			address = addrm_indirectIndexed(pProcessor);
			pProcessor->reg.AC = pProcessor->memIf.read8(address);
			cyclesPassed = 5 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, address);
			break;
		case 0xBD: // absolute, X
			address = addrm_absoluteXind(pProcessor);
			pProcessor->reg.AC = pProcessor->memIf.read8(address);
			cyclesPassed = 4 + MOS6502_OUTOFPAGE(pProcessor->reg.PC, address);
			break;
		case 0xA1: // (indirect, x)
		case 0xA5: // zeropage
		case 0xAD: // absolute
		case 0xB5: // zeropage, X
		case 0xB9: // absolute, Y
			return 0xFF;
	}

	if (opCode == 0xA9) {
		OP_PRINT(
			printf(
				"\tLoaded Accumulator with immediate value %d (0x%02X)\n\n",
				pProcessor->reg.AC,
				pProcessor->reg.AC
			)
		);
	} else {
		OP_PRINT(
			printf(
				"\tLoaded Accumulator with value %d (0x%02X) from memory address 0x%04X\n\n",
				pProcessor->reg.AC,
				pProcessor->reg.AC,
				address
			)
		);
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

	OP_PRINT(DBG_FN_PRINT);

	pProcessor->reg.SR &= ~(SR_FLAG_NEGATIVE | SR_FLAG_ZERO);

	switch (opCode) {
		case 0xA2: // immediate
			pProcessor->reg.X = addrm_immediate(pProcessor);
			cyclesPassed = 2;
			break;
		case 0xA6: // zeropage
		case 0xB6: // zeropage,Y
		case 0xAE: // absolute
		case 0xBE: // absolute,Y
			return 0xFF;
	}

	pProcessor->reg.SR |= pProcessor->reg.X & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.X == 0 ? SR_FLAG_ZERO : 0;

	OP_PRINT(
		printf(
			"\tLoaded Index X with value %d (0x%04X). %s%s\n\n",
			pProcessor->reg.X,
			pProcessor->reg.X,
			pProcessor->reg.SR & SR_FLAG_NEGATIVE ? "N " : "",
			pProcessor->reg.SR & SR_FLAG_ZERO ? "Z " : ""
		)
	);

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

	OP_PRINT(DBG_FN_PRINT);

	pProcessor->reg.SR &= ~(SR_FLAG_NEGATIVE | SR_FLAG_ZERO);

	switch (opCode) {
		case 0xA0: //immediate
			pProcessor->reg.Y = addrm_immediate(pProcessor);
			cyclesPassed = 2;
			break;
		case 0xA4: //zeropage
		case 0xB4: //zeropage,X
		case 0xAC: //absolute
		case 0xBC: //absolute,X
			return 0xFF;
	}

	pProcessor->reg.SR |= pProcessor->reg.Y & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.Y == 0 ? SR_FLAG_ZERO : 0;

	OP_PRINT(
		printf(
			"\tLoaded Index Y with value %d (0x%04X). %s%s\n\n",
			pProcessor->reg.Y,
			pProcessor->reg.Y,
			pProcessor->reg.SR & SR_FLAG_NEGATIVE ? "N " : "",
			pProcessor->reg.SR & SR_FLAG_ZERO ? "Z " : ""
		)
	);

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

	OP_PRINT(DBG_FN_PRINT);
	
	pProcessor->reg.SR &= ~(SR_FLAG_NEGATIVE | SR_FLAG_ZERO | SR_FLAG_CARRY);

	switch (opCode) {
		case 0x4A: {// accumulator
			U8 tempAc = pProcessor->reg.AC;
			pProcessor->reg.SR |= pProcessor->reg.AC & SR_FLAG_CARRY;
			pProcessor->reg.AC = pProcessor->reg.AC >> 1;
			pProcessor->reg.SR |= pProcessor->reg.AC == 0 ? SR_FLAG_ZERO : 0;
			OP_PRINT(
				printf(
					"\tShifted accumulator (%d/%d/0x%02X) 1 bit to the right, result: %d/%d/0x%02X. %s%s\n\n",
					tempAc,
					tempAc,
					tempAc,
					pProcessor->reg.AC,
					pProcessor->reg.AC,
					pProcessor->reg.AC,
					pProcessor->reg.SR & SR_FLAG_ZERO ? "Z " : "",
					pProcessor->reg.SR & SR_FLAG_CARRY ? "C " : ""
				)
			);
			cyclesPassed = 2;
			break;
		}
		case 0x46: // zeropage
		case 0x56: // zeropage,X
		case 0x4E: // absolute
		case 0x5E: // absolute,X
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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

	addrm_stackPush8(pProcessor, pProcessor->reg.AC);

	OP_PRINT(
		printf(
			"\tStored Accumulator %d (0x%02X) to stack. SP=0x%04X\n\n",
			pProcessor->reg.AC,
			pProcessor->reg.AC,
			pProcessor->reg.SP + 0x100
		)
	);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

	pProcessor->reg.SR &= ~(SR_FLAG_NEGATIVE | SR_FLAG_ZERO);

	pProcessor->reg.AC = addrm_stackPop8(pProcessor);

	pProcessor->reg.SR |= pProcessor->reg.AC & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.AC == 0 ? SR_FLAG_ZERO : 0;

	OP_PRINT(
		printf(
			"\tPulled Accumulator %d (0x%02X) from stack. SP=0x%04X. %s%s\n\n",
			pProcessor->reg.AC,
			pProcessor->reg.AC,
			pProcessor->reg.SP + 0x100,
			pProcessor->reg.SR & SR_FLAG_NEGATIVE ? "N " : "",
			pProcessor->reg.SR & SR_FLAG_ZERO ? "Z " : ""
		)
	);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

	pProcessor->reg.PC = addrm_stackPop16(pProcessor);
	
	cyclesPassed = 6;

	OP_PRINT(
		printf(
			"\tPopped return address 0x%04X (+ 1) from the stack. SP=0x%04X\n\n",
			pProcessor->reg.PC,
			pProcessor->reg.SP
		)
	);
	
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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

	// TODO implement decimal mode (BCD)
	switch (opCode) {
		case 0xE5: // zeropage
			memValue = pProcessor->memIf.read8(addrm_zeropage(pProcessor));
			cyclesPassed = 3;
			break;
		case 0xED: {// absolute
			memValue = pProcessor->memIf.read8(addrm_absolute(pProcessor));
			cyclesPassed = 4;
			break;
		}
		case 0xE9: // immediate
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
	
	OP_PRINT(
		printf(
			"\tSubtracted value %d/%d/0x%02X from Accumulator (%d/%d/0x%02X), result: %d/%d/0x%02X. %s%s%s%s\n\n",
			memValue,
			(I8)memValue,
			memValue,
			tempAc,
			(I8)tempAc,
			tempAc,
			pProcessor->reg.AC,
			(I8)pProcessor->reg.AC,
			pProcessor->reg.AC,
			pProcessor->reg.SR & SR_FLAG_NEGATIVE ? "N " : "",
			pProcessor->reg.SR & SR_FLAG_ZERO ? "Z " : "",
			pProcessor->reg.SR & SR_FLAG_CARRY ? "C " : "",
			pProcessor->reg.SR & SR_FLAG_OVERFLOW ? "O " : ""
		)
	);
	
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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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
		case 0x9D: // absolute,X
			destAddr = addrm_absoluteXind(pProcessor);
			cyclesPassed = 5;
			break;
		case 0x81: // (indirect,X)
		case 0x95: // zeropage,X
		case 0x99: // absolute,Y
			return 0xFF;
	}

	pProcessor->memIf.write8(destAddr, pProcessor->reg.AC);

	OP_PRINT(
		printf(
			"\tStored Accumulator (%d / 0x%02X) in memory address 0x%04X\n\n",
			pProcessor->reg.AC,
			pProcessor->reg.AC,
			destAddr
		)
	);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(
		printf(
			"\tStored Index Y value %d (0x%02X) to memory location 0x%04X\n\n",
			pProcessor->reg.Y,
			pProcessor->reg.Y,
			address
		)
	);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

	pProcessor->reg.SR &= ~(SR_FLAG_NEGATIVE | SR_FLAG_ZERO);

	pProcessor->reg.X = pProcessor->reg.AC;

	cyclesPassed = 2;

	pProcessor->reg.SR |= pProcessor->reg.X & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.X == 0 ? SR_FLAG_ZERO : 0;

	OP_PRINT(
		printf(
			"\tTransferred Accumulator (%d/0x%02X) to Index X. %s%s\n\n",
			pProcessor->reg.AC,
			pProcessor->reg.AC,
			pProcessor->reg.SR & SR_FLAG_NEGATIVE ? "N ": "",
			pProcessor->reg.SR & SR_FLAG_ZERO ? "Z ": ""
		)
	);

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

	OP_PRINT(DBG_FN_PRINT);

	pProcessor->reg.SR &= ~(SR_FLAG_ZERO | SR_FLAG_NEGATIVE);

	pProcessor->reg.Y = pProcessor->reg.AC;

	/* Update status register zero and negative flags. */
	pProcessor->reg.SR |= pProcessor->reg.AC & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.AC == 0 ? SR_FLAG_ZERO : 0;
	
	cyclesPassed = 2;

	OP_PRINT(
		printf(
			"\tTransferred value %d (0x%02X) from Accumulator to Index Y. %s%s\n\n",
			pProcessor->reg.Y,
			pProcessor->reg.Y,
			pProcessor->reg.SR & SR_FLAG_NEGATIVE ? "N " : "",
			pProcessor->reg.SR & SR_FLAG_ZERO ? "Z " : ""
		)
	);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

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

	OP_PRINT(DBG_FN_PRINT);

	pProcessor->reg.SR &= ~(SR_FLAG_NEGATIVE | SR_FLAG_ZERO);

	pProcessor->reg.AC = pProcessor->reg.Y;

	pProcessor->reg.SR |= pProcessor->reg.AC & SR_FLAG_NEGATIVE;
	pProcessor->reg.SR |= pProcessor->reg.AC == 0 ? SR_FLAG_ZERO : 0;

	cyclesPassed = 2;

	OP_PRINT(
		printf(
			"\tTransferred value %d (0x%02X) from Index Y to Accumulator. %s%s\n\n",
			pProcessor->reg.AC,
			pProcessor->reg.AC,
			pProcessor->reg.SR & SR_FLAG_NEGATIVE ? "N " : "",
			pProcessor->reg.SR & SR_FLAG_ZERO ? "Z " : ""
		)
	);

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

	OP_PRINT(DBG_FN_PRINT);

	switch (opCode) {
		default:
			return 0xFF;
	}

	pProcessor->reg.PC += pOpCodeData->bytes;

	return cyclesPassed;
}
