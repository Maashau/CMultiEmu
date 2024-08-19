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
	I8 offset = (I8)addrm_read8(pProcessor, addrm_imm(pProcessor));
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
	mos6502_addr retAddr = addrm_read16(pProcessor, pProcessor->reg.PC);

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
	mos6502_addr retAddr = addrm_read16(pProcessor, pProcessor->reg.PC) + pProcessor->reg.X;

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
	mos6502_addr retAddr = addrm_read16(pProcessor, pProcessor->reg.PC) + pProcessor->reg.Y;

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
	U8 address = addrm_read8(pProcessor, pProcessor->reg.PC);
	
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
	U8 address = addrm_read8(pProcessor, pProcessor->reg.PC) + pProcessor->reg.X;

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
	U8 address = addrm_read8(pProcessor, pProcessor->reg.PC) + pProcessor->reg.Y;
	
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
	U16 indAddress = addrm_read16(pProcessor, pProcessor->reg.PC);
	
	pProcessor->reg.PC += 2;

	return addrm_read16(pProcessor, indAddress);
}

/*******************************************************************************
* Operand is a zeropage address indexed by X, pointing to an address.
*
* Return: Address -> (U16)memory[(U8)memory[PC + 1] + X]
*******************************************************************************/
mos6502_addr addrm_xInd(
	mos6502_processor_st *	pProcessor
) {
	U8 indAddress = addrm_read8(pProcessor, pProcessor->reg.PC) + pProcessor->reg.X;
	
	pProcessor->reg.PC++;

	return addrm_read16(pProcessor, indAddress);
}

/*******************************************************************************
* Operand is a zeropage address pointing to an address, indexed by Y.
*
* Return: Address -> (U16)memory[(U8)memory[PC + 1]] + Y
*******************************************************************************/
mos6502_addr addrm_indY(
	mos6502_processor_st *	pProcessor
) {
	U8 indAddress = addrm_read8(pProcessor, pProcessor->reg.PC);
	mos6502_addr address = addrm_read16(pProcessor, indAddress) + pProcessor->reg.Y;

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
	return addrm_read8(pProcessor, 0x100 + pProcessor->reg.SP);
}

/*******************************************************************************
* Pop a word from stack.
*
* Return: (U16)memory[SP + 1]
*******************************************************************************/
U16 addrm_stackPop16(mos6502_processor_st * pProcessor) {
	U16 readValue;
	pProcessor->reg.SP++;
	readValue = addrm_read16(pProcessor, 0x100 + pProcessor->reg.SP);
	pProcessor->reg.SP++;
	return readValue;
}

/*******************************************************************************
* Push a byte to stack.
*******************************************************************************/
void addrm_stackPush8(mos6502_processor_st * pProcessor, U8 value) {
	addrm_write8(pProcessor, 0x100 + pProcessor->reg.SP, value);
	pProcessor->reg.SP--;
}

/*******************************************************************************
* Push a word to stack.
*******************************************************************************/
void addrm_stackPush16(mos6502_processor_st * pProcessor, U16 value) {
	pProcessor->reg.SP--;
	addrm_write16(pProcessor, 0x100 + pProcessor->reg.SP, value);
	pProcessor->reg.SP--;
}

/*******************************************************************************
* Read byte from memory.
*******************************************************************************/
U8 addrm_read8(mos6502_processor_st * pProcessor, mos6502_addr address) {
	return pProcessor->fnMemRead(address);
}

/*******************************************************************************
* Read word from memory.
*******************************************************************************/
U16 addrm_read16(mos6502_processor_st * pProcessor, mos6502_addr address) {
	return pProcessor->fnMemRead(address) | (pProcessor->fnMemRead(address + 1) << 8);
}

/*******************************************************************************
* Write byte to memory.
*******************************************************************************/
void addrm_write8(mos6502_processor_st * pProcessor, mos6502_addr address, U8 value) {
	pProcessor->fnMemWrite(address, value);
}

/*******************************************************************************
* Write word to memory.
*******************************************************************************/
void addrm_write16(mos6502_processor_st * pProcessor, mos6502_addr address, U16 value) {
	pProcessor->fnMemWrite(address, (U8)(value & 0xFF));
	pProcessor->fnMemWrite(address + 1, (U8)((value >> 8) & 0xFF));
}