/*******************************************************************************
* MOS 65xx emulator main header file.
*******************************************************************************/
#include "65xx_types.h"

/*******************************************************************************
* Macros and definitions
*******************************************************************************/

#define DBG_PRINT(_pProcessor, _x) if (_pProcessor->debugLevel & 1) _x
#define DBG_LOG(_pProcessor, _x) if (_pProcessor->debugLevel & 2) _x

#define U16_MAX 65535U
#define MOS65xx_MEMSIZE (U16_MAX)

#define MOS65xx_OUTOFPAGE(_pc, _address) ((_pc / 0xFF != _address / 0xFF) ? 1 : 0)

/*******************************************************************************
* Public functions
*******************************************************************************/
void mos65xx_init(
	Processor_65xx *	pProcessor,
	Memory_areas *		pMemory,
	mos65xx_memRead		fnMemRead,
	mos65xx_memWrite	fnMemWrite,
	U8                  debugLevel,
	void *              pUtil
);

void loadFile(U8 * pMemory, mos65xx_addr offset, const char * pPath, mos65xx_fileType fileType);

void addROMArea(mos65xx_addr startAddr, mos65xx_addr endAddr);
void addRAMArea(mos65xx_addr startAddr, mos65xx_addr endAddr);
U8 isROMAddress(mos65xx_addr address);
U8 isRAMAddress(mos65xx_addr address);

void mos65xx_handleOp(Processor_65xx * pProcessor);
void mos65xx_irqEnter(Processor_65xx * pProcessor);
void mos65xx_irqLeave(Processor_65xx * pProcessor);

void mos65xx_assemble(char * asmFilePath, U8 * memory);
