// Compatibility wrappers for SDL 1 & 2.
#pragma once
#include <SDL.h>

#define SDLC_KEYSTATE_LEFTCTRL SDL_SCANCODE_LCTRL
#define SDLC_KEYSTATE_RIGHTCTRL SDL_SCANCODE_RCTRL
#define SDLC_KEYSTATE_LEFTSHIFT SDL_SCANCODE_LSHIFT
#define SDLC_KEYSTATE_RIGHTSHIFT SDL_SCANCODE_RSHIFT
#define SDLC_KEYSTATE_LALT SDL_SCANCODE_LALT
#define SDLC_KEYSTATE_RALT SDL_SCANCODE_RALT
#define SDLC_KEYSTATE_UP SDL_SCANCODE_UP
#define SDLC_KEYSTATE_DOWN SDL_SCANCODE_DOWN
#define SDLC_KEYSTATE_LEFT SDL_SCANCODE_LEFT
#define SDLC_KEYSTATE_RIGHT SDL_SCANCODE_RIGHT

inline const Uint8 *SDLC_GetKeyState()
{
	return SDL_GetKeyboardState(NULL);
}

inline int SDLC_SetColorKey(SDL_Surface *surface, Uint32 key)
{
	SDL_SetSurfaceRLE(surface, 1);
	return SDL_SetColorKey(surface, SDL_TRUE, key);
}

// Copies the colors into the surface's palette.
inline int SDLC_SetSurfaceColors(SDL_Surface *surface, SDL_Color *colors, int firstcolor, int ncolors)
{
	return SDL_SetPaletteColors(surface->format->palette, colors, firstcolor, ncolors);
}

// Copies the colors into the surface's palette.
inline int SDLC_SetSurfaceColors(SDL_Surface *surface, SDL_Palette *palette)
{
	return SDLC_SetSurfaceColors(surface, palette->colors, 0, palette->ncolors);
}

// Sets the palette's colors and:
// SDL2: Points the surface's palette to the given palette if necessary.
// SDL1: Sets the surface's colors.
inline int SDLC_SetSurfaceAndPaletteColors(SDL_Surface *surface, SDL_Palette *palette, SDL_Color *colors, int firstcolor, int ncolors)
{
	if (SDL_SetPaletteColors(palette, colors, firstcolor, ncolors) < 0)
		return -1;
	if (surface->format->palette != palette)
		return SDL_SetSurfacePalette(surface, palette);
	return 0;
}
