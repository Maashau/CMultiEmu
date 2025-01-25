/*******************************************************************************
* 65xx_addrm.c
*
* 65xx Addressing modes.
*******************************************************************************/
#include <stdio.h>

#include "65xx_addrm.h"

/*******************************************************************************
* Operand is an immediate byte in the memory.
*
* Return: Value -> (U16)PC + 1
*******************************************************************************/
mos65xx_addr addrm_IMM(
	Processor_65xx *	pProcessor
) {
	mos65xx_addr retAddr = pProcessor->reg.PC;

	pProcessor->reg.PC++;

	pProcessor->cycles.currentOp += 1;

	return retAddr;
}

/*******************************************************************************
* Operand is an immediate unsigned byte in the memory.
*
* Return: Value -> (U16)PC + (U8)memory[PC + 1]
*******************************************************************************/
mos65xx_addr addrm_REL(
	Processor_65xx *	pProcessor
) {
	I8 offset = (I8)addrm_read8(pProcessor, addrm_IMM(pProcessor));
	mos65xx_addr retAddr = pProcessor->reg.PC + offset;

	return retAddr;
}

/*******************************************************************************
* Operand is an absolute address.
*
* Return: Address -> (U16)memory[PC + 1]
*******************************************************************************/
mos65xx_addr addrm_ABS(
	Processor_65xx *	pProcessor
) {
	mos65xx_addr retAddr = addrm_read16(pProcessor, pProcessor->reg.PC);

	pProcessor->reg.PC += 2;

	return retAddr;
}

/*******************************************************************************
* Operand is an absolute address indexed by X.
*
* Return: Address -> (U16)memory[PC + 1] + X
*******************************************************************************/
mos65xx_addr addrm_ABX(
	Processor_65xx *	pProcessor
) {
	mos65xx_addr retAddr = addrm_read16(pProcessor, pProcessor->reg.PC) + pProcessor->reg.X;

	pProcessor->reg.PC += 2;

	return retAddr;
}

/*******************************************************************************
* Operand is an absolute address indexed by Y.
*
* Return: Address -> (U16)memory[PC + 1] + Y
*******************************************************************************/
mos65xx_addr addrm_ABY(
	Processor_65xx *	pProcessor
) {	
	mos65xx_addr retAddr = addrm_read16(pProcessor, pProcessor->reg.PC) + pProcessor->reg.Y;

	pProcessor->reg.PC += 2;

	return retAddr;
}

/*******************************************************************************
* Operand is a zeropage address.
*
* Return: Address -> (U8)memory[PC + 1]
*******************************************************************************/
mos65xx_addr addrm_ZPG(
	Processor_65xx *	pProcessor
) {
	U8 address = addrm_read8(pProcessor, pProcessor->reg.PC);
	
	pProcessor->reg.PC++;
	
	return (mos65xx_addr)address;
}

/*******************************************************************************
* Operand is a zeropage address indexed by X.
*
* Return: Address -> (U8)memory[PC + 1] + X
*******************************************************************************/
mos65xx_addr addrm_ZPX(
	Processor_65xx *	pProcessor
) {
	U8 address = addrm_read8(pProcessor, pProcessor->reg.PC) + pProcessor->reg.X;

	pProcessor->reg.PC++;

	return (mos65xx_addr)address;
}

/*******************************************************************************
* Operand is a zeropage address indexed by Y.
*
* Return: Address -> (U8)memory[PC + 1] + Y
*******************************************************************************/
mos65xx_addr addrm_ZPY(
	Processor_65xx *	pProcessor
) {
	U8 address = addrm_read8(pProcessor, pProcessor->reg.PC) + pProcessor->reg.Y;
	
	pProcessor->reg.PC++;
	
	return (mos65xx_addr)address;
}

/*******************************************************************************
* Operand is an address pointing to an address.
*
* Return: Address -> (U16)memory[(U16)memory[PC]]
*******************************************************************************/
mos65xx_addr addrm_IND(
	Processor_65xx *	pProcessor
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
mos65xx_addr addrm_INX(
	Processor_65xx *	pProcessor
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
mos65xx_addr addrm_INY(
	Processor_65xx *	pProcessor
) {
	U8 indAddress = addrm_read8(pProcessor, pProcessor->reg.PC);
	mos65xx_addr address = addrm_read16(pProcessor, indAddress) + pProcessor->reg.Y;

	pProcessor->reg.PC++;

	return address;
}

/*******************************************************************************
* Pop a byte from stack.
*
* Return: (U8)memory[SP + 1]
*******************************************************************************/
U8 addrm_stackPop8(Processor_65xx * pProcessor) {
	pProcessor->reg.SP++;
	return addrm_read8(pProcessor, 0x100 + pProcessor->reg.SP);
}

/*******************************************************************************
* Pop a word from stack.
*
* Return: (U16)memory[SP + 1]
*******************************************************************************/
U16 addrm_stackPop16(Processor_65xx * pProcessor) {
	U16 readValue;
	pProcessor->reg.SP++;
	readValue = addrm_read16(pProcessor, 0x100 + pProcessor->reg.SP);
	pProcessor->reg.SP++;
	return readValue;
}

/*******************************************************************************
* Push a byte to stack.
*******************************************************************************/
void addrm_stackPush8(Processor_65xx * pProcessor, U8 value) {
	addrm_write8(pProcessor, 0x100 + pProcessor->reg.SP, value);
	pProcessor->reg.SP--;
}

/*******************************************************************************
* Push a word to stack.
*******************************************************************************/
void addrm_stackPush16(Processor_65xx * pProcessor, U16 value) {
	pProcessor->reg.SP--;
	addrm_write16(pProcessor, 0x100 + pProcessor->reg.SP, value);
	pProcessor->reg.SP--;
}

/*******************************************************************************
* Read byte from memory.
*******************************************************************************/
U8 addrm_read8(Processor_65xx * pProcessor, mos65xx_addr address) {
	return pProcessor->fnMemRead(
		pProcessor,
		address
	);
}

/*******************************************************************************
* Read word from memory.
*******************************************************************************/
U16 addrm_read16(Processor_65xx * pProcessor, mos65xx_addr address) {
	return pProcessor->fnMemRead(pProcessor, address) 
	| (pProcessor->fnMemRead(pProcessor, address + 1) << 8);
}

/*******************************************************************************
* Write byte to memory.
*******************************************************************************/
void addrm_write8(Processor_65xx * pProcessor, mos65xx_addr address, U8 value) {
	pProcessor->fnMemWrite(pProcessor, address, value);
}

/*******************************************************************************
* Write word to memory.
*******************************************************************************/
void addrm_write16(Processor_65xx * pProcessor, mos65xx_addr address, U16 value) {
	pProcessor->fnMemWrite(pProcessor, address, (U8)(value & 0xFF));
	pProcessor->fnMemWrite(pProcessor, address + 1, (U8)((value >> 8) & 0xFF));
}