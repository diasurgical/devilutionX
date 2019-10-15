// Compatibility wrappers for SDL 1 & 2.
#ifndef VITA
#include <SDL.h>
#else
#ifdef USE_SDL1
#include <SDL/SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#endif

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
