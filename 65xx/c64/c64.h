/*******************************************************************************
* Commodore 64 emulator main header file.
*******************************************************************************/
#ifndef C64_H_INCLUDED
#define C64_H_INCLUDED

#include <SDL2/SDL.h>

#include "../65xx.h"

#include "c64_io.h"


#define C64CARTLO_START	0x8000U
#define C64CARTLO_END	0x9FFFU

#define C64BASIC_START	0xA000U
#define C64BASIC_END	0xBFFFU

#define C64CHAR_START	0xD000U
#define C64CHAR_END		0xDFFFU

#define C64KERNAL_START	0xE000U
#define C64KERNAL_END	0xFFFFU

typedef struct {
	Processor_65xx *	pProcessor;
	SDL_sem *			pSdlSema;
	RGB_t *				pScreenBuf;
	U8					irqActive;
	U8					keyBuffer[0xFF];
} C64_t;

void * c64_init(Processor_65xx * pProcessor);
void c64_run(void * pDevStruct);

#endif