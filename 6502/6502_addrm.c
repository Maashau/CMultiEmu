/*******************************************************************************
* 6502_addrm.c
*
* 6502 Addressing modes.
*******************************************************************************/
#include <stdio.h>

#include "6502_addrm.h"


/*******************************************************************************
* Operand is an immediate byte in the memory.
*
* Return: Value -> (U16)PC + 1
*******************************************************************************/
mos6502_addr addrm_imm(
	mos6502_processor_st *	pProcessor
) {
	mos6502_addr retAddr = pProcessor->reg.PC;

	pProcessor->reg.PC++;

	return retAddr;
}

/*******************************************************************************
* Operand is an immediate unsigned byte in the memory.
*
* Return: Value -> (U16)PC + (U8)memory[PC + 1]
*******************************************************************************/
mos6502_addr addrm_rel(
	mos6502_processor_st *	pProcessor
) {
	I8 offset = (I8)pProcessor->memIf.read8(addrm_imm(pProcessor));
	mos6502_addr retAddr = pProcessor->reg.PC + offset;

	return retAddr;
}

/*******************************************************************************
* Operand is an absolute address.
*
* Return: Address -> (U16)memory[PC + 1]
*******************************************************************************/
mos6502_addr addrm_abs(
	mos6502_processor_st *	pProcessor
) {
	mos6502_addr retAddr = pProcessor->memIf.read16(pProcessor->reg.PC);

	pProcessor->reg.PC += 2;

	return retAddr;
}

/*******************************************************************************
* Operand is an absolute address indexed by X.
*
* Return: Address -> (U16)memory[PC + 1] + X
*******************************************************************************/
mos6502_addr addrm_absX(
	mos6502_processor_st *	pProcessor
) {
	mos6502_addr retAddr = pProcessor->memIf.read16(pProcessor->reg.PC) + pProcessor->reg.X;

	pProcessor->reg.PC += 2;

	return retAddr;
}

/*******************************************************************************
* Operand is an absolute address indexed by Y.
*
* Return: Address -> (U16)memory[PC + 1] + Y
*******************************************************************************/
mos6502_addr addrm_absY(
	mos6502_processor_st *	pProcessor
) {	
	mos6502_addr retAddr = pProcessor->memIf.read16(pProcessor->reg.PC) + pProcessor->reg.Y;

	pProcessor->reg.PC += 2;

	return retAddr;
}

/*******************************************************************************
* Operand is a zeropage address.
*
* Return: Address -> (U8)memory[PC + 1]
*******************************************************************************/
mos6502_addr addrm_zpg(
	mos6502_processor_st *	pProcessor
) {
	U8 address = pProcessor->memIf.read8(pProcessor->reg.PC);
	
	pProcessor->reg.PC++;
	
	return (mos6502_addr)address;
}

/*******************************************************************************
* Operand is a zeropage address indexed by X.
*
* Return: Address -> (U8)memory[PC + 1] + X
*******************************************************************************/
mos6502_addr addrm_zpgX(
	mos6502_processor_st *	pProcessor
) {
	U8 address = pProcessor->memIf.read8(pProcessor->reg.PC) + pProcessor->reg.X;

	pProcessor->reg.PC++;

	return (mos6502_addr)address;
}

/*******************************************************************************
* Operand is a zeropage address indexed by Y.
*
* Return: Address -> (U8)memory[PC + 1] + Y
*******************************************************************************/
mos6502_addr addrm_zpgY(
	mos6502_processor_st *	pProcessor
) {
	U8 address = pProcessor->memIf.read8(pProcessor->reg.PC) + pProcessor->reg.Y;
	
	pProcessor->reg.PC++;
	
	return (mos6502_addr)address;
}

/*******************************************************************************
* Operand is an address pointing to an address.
*
* Return: Address -> (U16)memory[(U16)memory[PC]]
*******************************************************************************/
mos6502_addr addrm_ind(
	mos6502_processor_st *	pProcessor
) {
	U16 indAddress = pProcessor->memIf.read16(pProcessor->reg.PC);
	
	pProcessor->reg.PC += 2;

	return pProcessor->memIf.read16(indAddress);
}

/*******************************************************************************
* Operand is a zeropage address indexed by X, pointing to an address.
*
* Return: Address -> (U16)memory[(U8)memory[PC + 1] + X]
*******************************************************************************/
mos6502_addr addrm_xInd(
	mos6502_processor_st *	pProcessor
) {
	U8 indAddress = pProcessor->memIf.read8(pProcessor->reg.PC) + pProcessor->reg.X;
	
	pProcessor->reg.PC++;

	return pProcessor->memIf.read16(indAddress);
}

/*******************************************************************************
* Operand is a zeropage address pointing to an address, indexed by Y.
*
* Return: Address -> (U16)memory[(U8)memory[PC + 1]] + Y
*******************************************************************************/
mos6502_addr addrm_indY(
	mos6502_processor_st *	pProcessor
) {
	U8 indAddress = pProcessor->memIf.read8(pProcessor->reg.PC);
	mos6502_addr address = pProcessor->memIf.read16(indAddress) + pProcessor->reg.Y;

	pProcessor->reg.PC++;
	
	return address;
}

/*******************************************************************************
* Pop a byte from stack.
*
* Return: (U8)memory[SP + 1]
*******************************************************************************/
U8 addrm_stackPop8(mos6502_processor_st * pProcessor) {
	pProcessor->reg.SP++;
	return pProcessor->memIf.read8(0x100 + pProcessor->reg.SP);
}

/*******************************************************************************
* Pop a word from stack.
*
* Return: (U16)memory[SP + 1]
*******************************************************************************/
U16 addrm_stackPop16(mos6502_processor_st * pProcessor) {
	U16 readValue;
	pProcessor->reg.SP++;
	readValue = pProcessor->memIf.read16(0x100 + pProcessor->reg.SP);
	pProcessor->reg.SP++;
	return readValue;
}

/*******************************************************************************
* Push a byte to stack.
*******************************************************************************/
void addrm_stackPush8(mos6502_processor_st * pProcessor, U8 value) {
	pProcessor->memIf.write8(0x100 + pProcessor->reg.SP, value);
	pProcessor->reg.SP--;
}

/*******************************************************************************
* Push a word to stack.
*******************************************************************************/
void addrm_stackPush16(mos6502_processor_st * pProcessor, U16 value) {
	pProcessor->reg.SP--;
	pProcessor->memIf.write16(0x100 + pProcessor->reg.SP, value);
	pProcessor->reg.SP--;
}