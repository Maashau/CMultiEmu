#include <SDL2/SDL.h>
#include <assert.h>

#include "../65xx.h"

#include "c64.h"
#include "c64_io.h"

enum {
	EXT_KEYS_LEFTARR = SDLK_DELETE + 1,
	EXT_KEYS_UPARR,
	EXT_KEYS_RIGHTARR,
	EXT_KEYS_DOWNARR,
	EXT_KEYS_LSHIFT,
	EXT_KEYS_RSHIFT,
	EXT_KEYS_CTRL,
    EXT_KEYS_COMMODORE,
};

void bufferKey(C64_t * pC64, SDL_Keycode key, U8 isPressed);

/*******************************************************************************
* Screen draw/event handler.
*
* Arguments:
*		pData	- (void) Pointer to 65xx processor struct.
*
* Returns: 0
*******************************************************************************/
int sdlThread(void * pData) {

	int tempMultiplier = 0;
	int multiplier = 1;
	SDL_Rect pixel;
	C64_t * pC64 = (C64_t *)pData;
	
	SDL_Window * pWindow = SDL_CreateWindow(
		"C64",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		C64_BORDER_W, C64_BORDER_H,
		0
	);

	SDL_Renderer * pRenderer = SDL_CreateRenderer(
		pWindow,
		-1,
		0
	);

	while (1) {

		SDL_SemWait(pC64->pSdlSema);

		if (tempMultiplier != multiplier) {
			SDL_SetWindowSize(pWindow, C64_BORDER_W * multiplier, C64_BORDER_H * multiplier);
			tempMultiplier = multiplier;
			pixel.w = multiplier;
			pixel.h = multiplier;
		}

			
		SDL_Event event;

		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) {
				((SDL_Event *)pC64->pProcessor->event)->type = event.type;
				break;

			} else if (event.type == SDL_KEYUP || event.type == SDL_KEYDOWN) {
				
				SDL_KeyCode key = event.key.keysym.sym;

				if (key == SDLK_ESCAPE) {
					((SDL_Event *)pC64->pProcessor->event)->type = SDL_QUIT;

				} else if (key == SDLK_PAGEUP && multiplier < 3 && event.type == SDL_KEYUP) {
					multiplier++;

				} else if (key == SDLK_PAGEDOWN && multiplier > 1 && event.type == SDL_KEYUP) {
					multiplier--;

				} else {
					bufferKey(pC64, key, event.type == SDL_KEYDOWN);
				}
			}
		}

		for (int ii = 0; ii < (C64_BORDER_H * C64_BORDER_W); ii++) {

			assert(ii < (C64_BORDER_H * C64_BORDER_W));

			RGB_t color = pC64->pScreenBuf[ii];

			SDL_SetRenderDrawColor(pRenderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, SDL_ALPHA_OPAQUE);

			
			pixel.x = (ii % C64_BORDER_W) * multiplier;
			pixel.y = (ii / C64_BORDER_W) * multiplier;
			SDL_RenderFillRect(pRenderer, &pixel);
		}
		
		SDL_RenderPresent(pRenderer);
	}

	return 0;
}

void bufferKey(C64_t * pC64, SDL_Keycode key, U8 isPressed) {

	if (key >= 0 && key <= SDLK_DELETE) {
		pC64->keyBuffer[key] = isPressed;

	} else if (key == SDLK_BACKSPACE) {
		pC64->keyBuffer[SDLK_DELETE] = isPressed;

	} else if (key == SDLK_LEFT) {
		pC64->keyBuffer[EXT_KEYS_LEFTARR] = isPressed;

	} else if (key == SDLK_UP) {
		pC64->keyBuffer[EXT_KEYS_UPARR] = isPressed;
		
	} else if (key == SDLK_RIGHT) {
		pC64->keyBuffer[EXT_KEYS_RIGHTARR] = isPressed;
		
	} else if (key == SDLK_DOWN) {
		pC64->keyBuffer[EXT_KEYS_DOWNARR] = isPressed;
		
	} else if (key == SDLK_LSHIFT) {
		pC64->keyBuffer[EXT_KEYS_LSHIFT] = isPressed;
		
	} else if (key == SDLK_RSHIFT) {
		pC64->keyBuffer[EXT_KEYS_RSHIFT] = isPressed;

	} else if (key == SDLK_LCTRL || key == SDLK_RCTRL) {
		pC64->keyBuffer[EXT_KEYS_CTRL] = isPressed;

	} else if (key == SDLK_LALT || key == SDLK_RALT) {
		pC64->keyBuffer[EXT_KEYS_COMMODORE] = isPressed;

	}
}
