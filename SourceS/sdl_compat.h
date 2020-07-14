// Compatibility wrappers for SDL 1 & 2.
#pragma once
#include <SDL.h>

#ifndef USE_SDL1
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
#else
#define SDLC_KEYSTATE_LEFTCTRL SDLK_LCTRL
#define SDLC_KEYSTATE_RIGHTCTRL SDLK_RCTRL
#define SDLC_KEYSTATE_LEFTSHIFT SDLK_LSHIFT
#define SDLC_KEYSTATE_RIGHTSHIFT SDLK_LSHIFT
#define SDLC_KEYSTATE_LALT SDLK_LALT
#define SDLC_KEYSTATE_RALT SDLK_RALT
#define SDLC_KEYSTATE_UP SDLK_UP
#define SDLC_KEYSTATE_DOWN SDLK_DOWN
#define SDLC_KEYSTATE_LEFT SDLK_LEFT
#define SDLC_KEYSTATE_RIGHT SDLK_RIGHT
#endif

inline const Uint8 *SDLC_GetKeyState()
{
#ifndef USE_SDL1
	return SDL_GetKeyboardState(NULL);
#else
	return SDL_GetKeyState(NULL);
#endif
}

inline int SDLC_SetColorKey(SDL_Surface *surface, Uint32 key)
{
#ifdef USE_SDL1
	return SDL_SetColorKey(surface, SDL_SRCCOLORKEY, key);
#else
	return SDL_SetColorKey(surface, SDL_TRUE, key);
#endif
}

// Copies the colors into the surface's palette.
inline int SDLC_SetSurfaceColors(SDL_Surface *surface, SDL_Color *colors, int firstcolor, int ncolors)
{
#ifdef USE_SDL1
	return SDL_SetPalette(surface, SDL_LOGPAL, colors, firstcolor, ncolors) - 1;
#else
	return SDL_SetPaletteColors(surface->format->palette, colors, firstcolor, ncolors);
#endif
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
#ifdef USE_SDL1
	if (ncolors > (palette->ncolors - firstcolor)) {
		SDL_SetError("ncolors > (palette->ncolors - firstcolor)");
		return -1;
	}
	if (colors != (palette->colors + firstcolor))
		SDL_memcpy(palette->colors + firstcolor, colors, ncolors * sizeof(*colors));

	#if SDL1_VIDEO_MODE_BPP == 8
		// When the video surface is 8bit, we need to set the output pallet as well.
		SDL_SetColors(SDL_GetVideoSurface(), colors, firstcolor, ncolors);
	#endif
	// In SDL1, the surface always has its own distinct palette, so we need to
	// update it as well.
	return SDL_SetPalette(surface, SDL_LOGPAL, colors, firstcolor, ncolors) - 1;
#else
	if (SDL_SetPaletteColors(palette, colors, firstcolor, ncolors) < 0)
		return -1;
	if (surface->format->palette != palette)
		return SDL_SetSurfacePalette(surface, palette);
	return 0;
#endif
}
