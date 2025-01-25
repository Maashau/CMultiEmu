/*******************************************************************************
* c64_vic.c
*
* Commodore 64 VIC II chip source file.
*******************************************************************************/
#include <stdio.h>
#include <time.h>
#include <65xx.h>

#include <SDL2/SDL.h>

#include "c64.h"
#include "c64_vic.h"

#define SCREEN_W	504
#define SCREEN_H	312

#define BORDER_W	404
#define BORDER_H	284

#define VIEW_W		320
#define VIEW_H		200

#define H_BLANK		((SCREEN_W - BORDER_W) / 2)
#define V_BLANK		((SCREEN_H - BORDER_H) / 2)

#define COL_CNT		40
#define ROW_CNT		25


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

typedef U32 RGB_t;

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

static RGB_t scrBuf[BORDER_W * BORDER_H];

static U8 irqActive;

static U32 getColor(Processor_65xx * pProcessor);
static U8 getScreenCharacter(Processor_65xx * pProcessor, U8 row, U8 col);
static U8 getCharLine(Processor_65xx * pProcessor, U8 character);
static int ioThread(void * pData);

static SDL_sem * semaphore;

/*******************************************************************************
* Init VIC II.
*
* Arguments:
*		-
*
* Returns: -
*******************************************************************************/
void c64_vicInit(Processor_65xx * pProcessor) {

	SDL_CreateThread(ioThread, "DrawFn", pProcessor);
	semaphore = SDL_CreateSemaphore(0);

	pProcessor->mem.IO[D011_screenCtrl1] = 0x1B;
	pProcessor->mem.IO[D016_screenCtrl2] = 0xC8;
	pProcessor->mem.IO[D018_memSetupReg] = 0x12;

	srand(time(NULL));

    c64_vicVertRaster = 0;
	c64_vicHorizRaster = 0;
}

/*******************************************************************************
* Check if VIC II chip has active interrupts.
*
* Arguments:
*		-
*
* Returns: true if interrupt(s) active, otherwise 0
*******************************************************************************/
U8 c64_vicCheckIrq(void) {
	return irqActive;
}

/*******************************************************************************
* Tick VIC chip.
*
* Arguments:
*		advance - ticks to advance.
*
* Returns: -
*******************************************************************************/
void c64_vicTick(Processor_65xx * pProcessor, U8 tickAdvance) {

	for (U8 bitsToDraw = 8 * (tickAdvance + 1); bitsToDraw > 0; bitsToDraw++) {

		if (c64_vicVertRaster > V_BLANK
		&&	c64_vicVertRaster < (V_BLANK + BORDER_H)
		&&	c64_vicHorizRaster > H_BLANK
		&&	c64_vicHorizRaster < (H_BLANK + BORDER_W)
		) {
			RGB_t color = getColor(pProcessor);
			scrBuf[
				(c64_vicVertRaster - V_BLANK) * BORDER_W
			+	(c64_vicHorizRaster - H_BLANK)
			] = color;
		}

		if (++c64_vicHorizRaster >= SCREEN_W) {
			c64_vicHorizRaster = 0;

			c64_vicVertRaster++;
		}

		if (c64_vicVertRaster >= SCREEN_H) {
			c64_vicVertRaster = 0;

			/* TODO: Draw sprites */
			/* Trigger drawing */
			SDL_SemPost(semaphore);
		}
	}


	/* Write the most significant bit of raster line. */
	pProcessor->mem.IO[D011_screenCtrl1] = (
		(pProcessor->mem.IO[D011_screenCtrl1] & (1 << 7))
	|	(U8)((c64_vicVertRaster & 0x100) >> 1)
	);

	pProcessor->mem.IO[D012_currRasterLine] = ((U8)c64_vicVertRaster & 0xFF);
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
	return pProcessor->mem.IO[address];
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
	pProcessor->mem.IO[address] = value;
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

	if (c64_vicVertRaster > (V_BLANK + (BORDER_H - VIEW_H) / 2 - 1) /* From top */
	&&	c64_vicVertRaster < (V_BLANK + (BORDER_H - VIEW_H) / 2 + VIEW_H) /* Bottom */
	&&	c64_vicHorizRaster > (H_BLANK + (BORDER_W - VIEW_W) / 2 - 1) /* Left */
	&&	c64_vicHorizRaster < (H_BLANK + (BORDER_W - VIEW_W) / 2 + VIEW_W) /* Right */
	) {
		U8 currRow = (c64_vicVertRaster - V_BLANK - (BORDER_H - VIEW_H) / 2) / 8;
		U8 currCol = (c64_vicHorizRaster - H_BLANK - (BORDER_W - VIEW_W) / 2) / 8;
		U8 chr = getScreenCharacter(pProcessor, currRow, currCol);

		color = colorMap[pProcessor->mem.IO[D021_backgroundColor] & 0xF];

		if (chr != 0x20) {
			U8 charLine = getCharLine(pProcessor, chr);
			U8 subCol = (c64_vicHorizRaster - H_BLANK - (BORDER_W - VIEW_W) / 2) % 8;

			if (charLine & (1 << (7 - subCol))) {
				mos65xx_addr colorIndex = pProcessor->mem.IO[0xD800 + currRow * 40 + currCol];
				color = colorMap[colorIndex];
			}
		}

		/* View */
	} else {
		/* Border */
		color = colorMap[pProcessor->mem.IO[D020_borderColor] & 0xF];
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
	U8 vicBankIndex = (pProcessor->mem.IO[0xDD00] & 0x3);
	U8 screenMemBits = pProcessor->mem.IO[0xD018] >> 4;
	mos65xx_addr vicBankAddr = (4 - (vicBankIndex + 1)) * 16 * 1024;
	mos65xx_addr screenMem = vicBankAddr + ((screenMemBits & 0xF) << 10);

	return pProcessor->mem.RAM[screenMem + row * 40 + col];
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
	U8 subRow = (c64_vicVertRaster - V_BLANK - (BORDER_H - VIEW_H) / 2) % 8;
#if 0
	U8 vicBankIndex = (pProcessor->mem.IO[0xDD00] & 0x3);
	if (vicBankIndex == 0 || vicBankIndex == 2) {
		retVal = pProcessor->mem.ROM[C64CHAR_START + character + subRow];
	} else {
		U8 charMemBits = pProcessor->mem.IO[0xD018] >> 1;
		mos65xx_addr vicBankAddr = (4 - (vicBankIndex + 1)) * 16 * 1024;
		mos65xx_addr charMem = vicBankAddr + ((charMemBits & 0x7) << 11);
		retVal = pProcessor->mem.RAM[charMem + character + subRow];
	}
#else
	retVal = pProcessor->mem.ROM[C64CHAR_START + character * 8 + subRow];
#endif
	return retVal;
}

/*******************************************************************************
* Screen draw/event handler.
*
* Arguments:
*		pData	- (void) Pointer to 65xx processor struct.
*
* Returns: 0
*******************************************************************************/
int ioThread(void * pData) {

	int tempMultiplier = 0;
	int multiplier = 1;
	SDL_Rect pixel;
	Processor_65xx * pProcessor = (Processor_65xx *)pData;
	
	SDL_Window * pWindow = SDL_CreateWindow(
		"C64",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		BORDER_W, BORDER_H,
		0
	);

	SDL_Renderer * pRenderer = SDL_CreateRenderer(
		pWindow,
		-1,
		0
	);

	while (1) {

		SDL_SemWait(semaphore);

		if (tempMultiplier != multiplier) {
			SDL_SetWindowSize(pWindow, BORDER_W * multiplier, BORDER_H * multiplier);
			tempMultiplier = multiplier;
			pixel.w = multiplier;
			pixel.h = multiplier;
		}

			
		SDL_Event event;

		SDL_PollEvent(&event);

		if (event.type == SDL_QUIT) {
			((SDL_Event *)pProcessor->event)->type = event.type;
			break;
		} else if (event.type == SDL_KEYUP) {
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				((SDL_Event *)pProcessor->event)->type = SDL_QUIT;
			} else if (event.key.keysym.sym == SDLK_PAGEUP && multiplier < 3) {
				multiplier++;
			} else if (event.key.keysym.sym == SDLK_PAGEDOWN && multiplier > 1) {
				multiplier--;
			}
		}

		for (int ii = 0; ii < (BORDER_H * BORDER_W); ii++) {
			RGB_t color = scrBuf[ii];

			SDL_SetRenderDrawColor(pRenderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, SDL_ALPHA_OPAQUE);

			
			pixel.x = (ii % BORDER_W) * multiplier;
			pixel.y = (ii / BORDER_W) * multiplier;
			SDL_RenderFillRect(pRenderer, &pixel);
		}
		
		SDL_RenderPresent(pRenderer);
	}

	return 0;
}