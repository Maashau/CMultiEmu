/*******************************************************************************
* 65xx.c
*
* 65xx processor emulator.
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "65xx.h"
#include "65xx_opc.h"
#include "65xx_addrm.h"


#include "65xx_opcList.c"

U8 ROMAddrRangeCount;
address_range ROMAddrList[0xFF];
U8 RAMAddrRangeCount;
address_range RAMAddrList[0xFF];

U8 opLogging;

void logOperation(
	Processor_65xx *	pProcessor,
	mos65xx_reg_st *		oldReg,
	U8						opCode,
	U8 *					operands
);

FILE * tmpLog;

/*******************************************************************************
* Initialize the processor.
*
* Arguments:
*	pProcessor - Pointer to a processor data structure.
*
* Returns: Nothing
*******************************************************************************/
void mos65xx_init(
	Processor_65xx *		pProcessor,
	Memory_areas *			pMemory,
	mos65xx_memRead			fnMemRead,
	mos65xx_memWrite		fnMemWrite,
	U8						debugLevel,
	void *					pUtil
) {

	memset(pProcessor, 0, sizeof(Processor_65xx));

	pProcessor->pMem = pMemory;
	pProcessor->fnMemRead = fnMemRead;
	pProcessor->fnMemWrite = fnMemWrite;
	pProcessor->debugLevel = debugLevel;
	pProcessor->pUtil = pUtil;

	pProcessor->reg.PC = addrm_read16(pProcessor, vector_RESET);
	pProcessor->reg.SR = SR_FLAG_UNUSED | SR_FLAG_B | SR_FLAG_I;
	pProcessor->reg.SP = 0xFF;
	
	DBG_PRINT(pProcessor, printf("                                         |  Registers  | Flags  |              \n"));
	DBG_PRINT(pProcessor, printf("\033[4mADDR | OP       | ASM           | Memory | AC XR YR SP | NVDIZC | Cycles (Tot)\033[m\n"));

	DBG_LOG(pProcessor, tmpLog = fopen("./tempLog.txt", "w+"));
}

/*******************************************************************************
* Handle next instruction
*
* Arguments:
*	pProcessor - Pointer to a processor data structure.
*
* Returns: -
*******************************************************************************/
void mos65xx_handleOp(Processor_65xx * pProcessor) {

	mos65xx_reg_st oldReg = pProcessor->reg;
	U8 pOperands[2];
	U8 opCode;

	/* Read operands for debug printing. */
	pOperands[0] = addrm_read8(pProcessor, pProcessor->reg.PC + 1);
	pOperands[1] = addrm_read8(pProcessor, pProcessor->reg.PC + 2);
	
	if (pProcessor->reg.PC == 0) {
		//__debugbreak();
	}

	/* Handle instruction. */
	pProcessor->cycles.currentOp = 0;
	opCode = addrm_read8(pProcessor, addrm_IMM(pProcessor));


	mos65xx__opCodes[opCode].handler(pProcessor, opCode, mos65xx__opCodes[opCode].addrm);

	pProcessor->lastOp = opCode;
	pProcessor->cycles.totalCycles += pProcessor->cycles.currentOp;

	logOperation(pProcessor, &oldReg, opCode, pOperands);
}

/*******************************************************************************
* Pin interrupt triggered
*
* Arguments:
*	pProcessor - Pointer to a processor data structure.
*
* Returns: -
*******************************************************************************/
U8 dbg_hwIntActive;
void mos65xx_irqOn(Processor_65xx * pProcessor) {

	dbg_hwIntActive = 1;

	mos65xx_IRQ(pProcessor, 0, IMP);
}

/*******************************************************************************
* Return from interrupt.
*
* Arguments:
*	pProcessor - Pointer to a processor data structure.
*
* Returns: -
*******************************************************************************/
void mos65xx_irqOff(Processor_65xx * pProcessor) {
	pProcessor = pProcessor;
	dbg_hwIntActive = 0;
}

/*******************************************************************************
* Mark address range as ROM.
*
* Arguments:
*	startAddr	- Starting address of the ROM region
*	endAddr		- Ending address of the ROM region
*
* Returns: -
*******************************************************************************/
void addROMArea(mos65xx_addr startAddr, mos65xx_addr endAddr) {
	if (startAddr > endAddr) {
		printf("\n\nInvalid ROM area range (0x%04x - 0x%04x)...\n\n", startAddr, endAddr);
		exit(1);
	}

	ROMAddrList[ROMAddrRangeCount].startAddr = startAddr;
	ROMAddrList[ROMAddrRangeCount].endAddr = endAddr;

	ROMAddrRangeCount++;
}

/*******************************************************************************
* Check if given address is on ROM region.
*
* Arguments:
*	address		- Address to be checked
*
* Returns: 1 if ROM, otherwise 0
*******************************************************************************/
U8 isROMAddress(mos65xx_addr address) {
	for (int areaIdx = 0; areaIdx < ROMAddrRangeCount; areaIdx++) {
		if (ROMAddrList[areaIdx].startAddr <= address
		&&	ROMAddrList[areaIdx].endAddr >= address
		) {
			return 1;
		}
	}

	return 0;
}

/*******************************************************************************
* Mark address range as RAM.
*
* Arguments:
*	startAddr	- Starting address of the RAM region
*	endAddr		- Ending address of the RAM region
*
* Returns: -
*******************************************************************************/
void addRAMArea(mos65xx_addr startAddr, mos65xx_addr endAddr) {
	if (startAddr > endAddr) {
		printf("\n\nInvalid RAM area range (0x%04x - 0x%04x)...\n\n", startAddr, endAddr);
		exit(1);
	}

	RAMAddrList[RAMAddrRangeCount].startAddr = startAddr;
	RAMAddrList[RAMAddrRangeCount].endAddr = endAddr;

	RAMAddrRangeCount++;
}

/*******************************************************************************
* Check if given address is on RAM region.
*
* Arguments:
*	address		- Address to be checked
*
* Returns: 1 if RAM, otherwise 0
*******************************************************************************/
U8 isRAMAddress(mos65xx_addr address) {
	for (int areaIdx = 0; areaIdx < RAMAddrRangeCount; areaIdx++) {
		if (RAMAddrList[areaIdx].startAddr <= address
		&&	RAMAddrList[areaIdx].endAddr >= address
		) {
			return 1;
		}
	}

	return 0;
}

/*******************************************************************************
* Loads binary from given path.
*
* Arguments:
*	pMemory - Pointer to a memory allocation.
*	pPath	- Pointer to a file path.
*
*
* Returns: Nothing.
*******************************************************************************/
void loadFile(U8 * pMemory, U16 offset, const char * pPath, mos65xx_fileType fileType) {
	FILE * fpPrg;

	fpPrg = fopen(pPath, "rb");

	if (fpPrg == NULL) {
		printf("\n\nError: file (%s) not found...\n\n", pPath);
		exit(1);
	}

	switch (fileType)
	{
	case mos65xx_BIN:
		break;
	case mos65xx_HEX:
	case mos65xx_ASM:
		printf("Filetype not yet supported...\n");
		exit(1);
	}

	for (int memIdx = offset; memIdx <= 0xFFFF; memIdx++) {
		int tempByte = fgetc(fpPrg);

		if (memIdx == 0) {
			printf("\n\nError: Too large binary (%s)...\n\n", pPath);
			exit(1);
		}

		if (tempByte != EOF) {
			pMemory[memIdx] = (U8)tempByte;
		} else {
			break;
		}
	}

	fclose(fpPrg);
}

/*******************************************************************************
* Prints information of current operation.
*
* Arguments:
*	pProcessor	- Pointer to a processor data structure.
*	PC			- Program counter value for printed operation.
*	opCode		- Numeric value representing the op code.
*	operBytes	- Operand bytes for current operation.
*
* Returns: Nothing
*******************************************************************************/
void logOperation(
	Processor_65xx *	pProcessor,
	mos65xx_reg_st *		oldReg,
	U8						opCode,
	U8 *					operBytes
) {	
	char * opLine = (char *)malloc(512);

	extern U8 CIA1_read[0xF];

	sprintf(opLine, "\n%04X | %s |  %02X %02X %02X | %02X %02X %02X %02X | %c%c%c%c%c%c | %1d (%llu) | %c | %u",
		oldReg->PC,
		mos65xx__opCodes[opCode].mnemonic,
		opCode,
		operBytes[0],
		operBytes[1],
		pProcessor->reg.AC,
		pProcessor->reg.X,
		pProcessor->reg.Y,
		pProcessor->reg.SP,
		pProcessor->reg.SR & SR_FLAG_N ? 'N' : ' ',
		pProcessor->reg.SR & SR_FLAG_V ? 'V' : ' ',
		pProcessor->reg.SR & SR_FLAG_D ? 'D' : ' ',
		pProcessor->reg.SR & SR_FLAG_I ? 'I' : ' ',
		pProcessor->reg.SR & SR_FLAG_Z ? 'Z' : ' ',
		pProcessor->reg.SR & SR_FLAG_C ? 'C' : ' ',
		pProcessor->cycles.currentOp,
		pProcessor->cycles.totalCycles,
		dbg_hwIntActive ? 'I' : ' ',
		CIA1_read[4] | (CIA1_read[5] << 8)
	);

	DBG_PRINT(pProcessor, printf("%s", opLine));
	DBG_LOG(pProcessor, fprintf(tmpLog, "%s", opLine));

	free(opLine);
}
