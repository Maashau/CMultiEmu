/*******************************************************************************
* c64_vic.c
*
* Commodore 64 VIC II chip source file.
*******************************************************************************/
#include <stdio.h>
#include <time.h>
#include <assert.h>


#include "../65xx.h"

#include "c64.h"
#include "c64_vic.h"
#include "c64_io.h"

#define U8ArrToU32(_r, _g, _b) (U32)(_r << 16 | _g << 8 | _b)

#define C64_COLOR_BLACK		U8ArrToU32(0x00, 0x00, 0x00)
#define C64_COLOR_WHITE		U8ArrToU32(0xFF, 0xFF, 0xFF)
#define C64_COLOR_RED		U8ArrToU32(0x92, 0x4A, 0x40)
#define C64_COLOR_CYAN		U8ArrToU32(0x84, 0xC5, 0xCC)
#define C64_COLOR_PURPLE	U8ArrToU32(0x93, 0x51, 0xB6)
#define C64_COLOR_GREEN		U8ArrToU32(0x72, 0xB1, 0x4B)
#define C64_COLOR_BLUE		U8ArrToU32(0x48, 0x3A, 0xAA)
#define C64_COLOR_YELLOW	U8ArrToU32(0xD5, 0xDF, 0x7C)
#define C64_COLOR_BROWN		U8ArrToU32(0x67, 0x52, 0x00)
#define C64_COLOR_ORANGE	U8ArrToU32(0xC3, 0x3D, 0x00)
#define C64_COLOR_LRED		U8ArrToU32(0xC1, 0x81, 0x78)
#define C64_COLOR_GREY1		U8ArrToU32(0x60, 0x60, 0x60)
#define C64_COLOR_GREY2		U8ArrToU32(0x8A, 0x8A, 0x8A)
#define C64_COLOR_LGREEN	U8ArrToU32(0xB3, 0xEC, 0x91)
#define C64_COLOR_LBLUE		U8ArrToU32(0x86, 0x7A, 0xDE)
#define C64_COLOR_GREY3		U8ArrToU32(0xB3, 0xB3, 0xB3)

static RGB_t colorMap[16] = {
	C64_COLOR_BLACK,
	C64_COLOR_WHITE,
	C64_COLOR_RED,
	C64_COLOR_CYAN,
	C64_COLOR_PURPLE,
	C64_COLOR_GREEN,
	C64_COLOR_BLUE,
	C64_COLOR_YELLOW,
	C64_COLOR_BROWN,
	C64_COLOR_ORANGE,
	C64_COLOR_LRED,
	C64_COLOR_GREY1,
	C64_COLOR_GREY2,
	C64_COLOR_LGREEN,
	C64_COLOR_LBLUE,
	C64_COLOR_GREY3
};

static U16 c64_vicVertRaster = 0;
static U16 c64_vicHorizRaster = 0;
static U16 c64_vicRasterIrqLine;

static U32 getColor(Processor_65xx * pProcessor);
static U8 getScreenCharacter(Processor_65xx * pProcessor, U8 row, U8 col);
static U8 getCharLine(Processor_65xx * pProcessor, U8 character);

/*******************************************************************************
* Init VIC II.
*
* Arguments:
*		-
*
* Returns: -
*******************************************************************************/
void c64_vicInit(C64_t * pC64) {

	pC64->pProcessor->pMem->IO[D011_screenCtrl1] = 0x1B;
	pC64->pProcessor->pMem->IO[D016_screenCtrl2] = 0xC8;
	pC64->pProcessor->pMem->IO[D018_memSetupReg] = 0x12;

	srand(time(NULL));

    c64_vicVertRaster = 0;
	c64_vicHorizRaster = 0;
}

/*******************************************************************************
* Tick VIC chip.
*
* Arguments:
*		advance - ticks to advance.
*
* Returns: -
*******************************************************************************/
void c64_vicTick(C64_t * pC64) {

	for (U8 bitsToDraw = 8 * (pC64->pProcessor->cycles.currentOp + 1); bitsToDraw > 0; bitsToDraw++) {

		if (c64_vicVertRaster > C64_V_BLANK
		&&	c64_vicVertRaster < (C64_V_BLANK + C64_BORDER_H)
		&&	c64_vicHorizRaster > C64_H_BLANK
		&&	c64_vicHorizRaster < (C64_H_BLANK + C64_BORDER_W)
		) {
			RGB_t color = getColor(pC64->pProcessor);

			int index = (c64_vicVertRaster - C64_V_BLANK) * C64_BORDER_W + (c64_vicHorizRaster - C64_H_BLANK);

			assert(index < (C64_BORDER_H * C64_BORDER_W));

			pC64->pScreenBuf[index] = color;
		}

		if (++c64_vicHorizRaster >= C64_SCREEN_W) {
			c64_vicHorizRaster = 0;

			c64_vicVertRaster++;
		}

		if (c64_vicVertRaster >= C64_SCREEN_H) {
			c64_vicVertRaster = 0;

			/* TODO: Draw sprites */
			/* Trigger drawing */
			SDL_SemPost(pC64->pSdlSema);
		}
	}

	/* Write the most significant bit of raster line. */
	pC64->pProcessor->pMem->IO[D011_screenCtrl1] = (
		(pC64->pProcessor->pMem->IO[D011_screenCtrl1] & (1 << 7))
	|	(U8)((c64_vicVertRaster & 0x100) >> 1)
	);

	pC64->pProcessor->pMem->IO[D012_currRasterLine] = ((U8)c64_vicVertRaster & 0xFF);
}

/*******************************************************************************
* Read from peripheral address.
*
* Arguments:
*		pProcessor	- Pointer to 65xx processor struct.
*		address 	- Address to read from.
*
* Returns: Value read from the VIC II.
*******************************************************************************/
U8 c64_vicRead(Processor_65xx * pProcessor, mos65xx_addr address) {
	return pProcessor->pMem->IO[address];
}

/*******************************************************************************
* Write VIC II register.
*
* Arguments:
*		pProcessor	- Pointer to 65xx processor struct.
*		address 	- Address to write to.
*		value		- Value to be written.
*
* Returns: -
*******************************************************************************/
void c64_vicWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value) {
	
	//printf("VIC: $%04X = $%02X\r\n", address, value);
	pProcessor->pMem->IO[address] = value;
}

/*******************************************************************************
* Get color for a pixel currently being drawn.
*
* Arguments:
*		pProcessor	- Processor struct pointer.
*
* Returns: RGB888 color code.
*******************************************************************************/
static U32 getColor(Processor_65xx * pProcessor) {
	
	RGB_t color = 0;

	if (c64_vicVertRaster > (C64_V_BLANK + (C64_BORDER_H - C64_VIEW_H) / 2 - 1) /* From top */
	&&	c64_vicVertRaster < (C64_V_BLANK + (C64_BORDER_H - C64_VIEW_H) / 2 + C64_VIEW_H) /* Bottom */
	&&	c64_vicHorizRaster > (C64_H_BLANK + (C64_BORDER_W - C64_VIEW_W) / 2 - 1) /* Left */
	&&	c64_vicHorizRaster < (C64_H_BLANK + (C64_BORDER_W - C64_VIEW_W) / 2 + C64_VIEW_W) /* Right */
	) {
		U8 currRow = (c64_vicVertRaster - C64_V_BLANK - (C64_BORDER_H - C64_VIEW_H) / 2) / 8;
		U8 currCol = (c64_vicHorizRaster - C64_H_BLANK - (C64_BORDER_W - C64_VIEW_W) / 2) / 8;
		U8 chr = getScreenCharacter(pProcessor, currRow, currCol);

		color = colorMap[pProcessor->pMem->IO[D021_backgroundColor] & 0xF];

		if (chr != 0x20) {
			U8 charLine = getCharLine(pProcessor, chr);
			U8 subCol = (c64_vicHorizRaster - C64_H_BLANK - (C64_BORDER_W - C64_VIEW_W) / 2) % 8;

			if (charLine & (1 << (7 - subCol))) {
				mos65xx_addr colorIndex = pProcessor->pMem->IO[0xD800 + currRow * 40 + currCol];
				color = colorMap[colorIndex];
			}
		}

		/* View */
	} else {
		/* Border */
		color = colorMap[pProcessor->pMem->IO[D020_borderColor] & 0xF];
	}

	return color;
}

/*******************************************************************************
* Get character from screen memory.
*
* Arguments:
*		pProcessor	- Processor struct pointer.
*		row			- Row of the character.
*		col			- Column of the character.
*
* Returns: Screencode of the character.
*******************************************************************************/
static U8 getScreenCharacter(Processor_65xx * pProcessor, U8 row, U8 col) {
	U8 vicBankIndex = (pProcessor->pMem->IO[0xDD00] & 0x3);
	U8 screenMemBits = pProcessor->pMem->IO[0xD018] >> 4;
	mos65xx_addr vicBankAddr = (4 - (vicBankIndex + 1)) * 16 * 1024;
	mos65xx_addr screenMem = vicBankAddr + ((screenMemBits & 0xF) << 10);

	return pProcessor->pMem->RAM[screenMem + row * 40 + col];
}

/*******************************************************************************
* Get line of pixels for a character.
*
* Arguments:
*		pProcessor	- Processor struct pointer.
*		pData		- (void) Pointer to 65xx processor struct.
*
* Returns: Byte containing row of pixels
*******************************************************************************/
static U8 getCharLine(Processor_65xx * pProcessor, U8 character) {
	U8 retVal;
	U8 subRow = (c64_vicVertRaster - C64_V_BLANK - (C64_BORDER_H - C64_VIEW_H) / 2) % 8;
#if 0
	U8 vicBankIndex = (pProcessor->pMem->IO[0xDD00] & 0x3);
	if (vicBankIndex == 0 || vicBankIndex == 2) {
		retVal = pProcessor->pMem->ROM[C64CHAR_START + character + subRow];
	} else {
		U8 charMemBits = pProcessor->pMem->IO[0xD018] >> 1;
		mos65xx_addr vicBankAddr = (4 - (vicBankIndex + 1)) * 16 * 1024;
		mos65xx_addr charMem = vicBankAddr + ((charMemBits & 0x7) << 11);
		retVal = pProcessor->pMem->RAM[charMem + character + subRow];
	}
#else
	retVal = pProcessor->pMem->ROM[C64CHAR_START + character * 8 + subRow];
#endif
	return retVal;
}