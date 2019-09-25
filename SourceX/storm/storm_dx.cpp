#include <SDL.h>

#include "devilution.h"
#include "miniwin/ddraw.h"
#include "stubs.h"

namespace dvl {

BOOL SDrawUpdatePalette(unsigned int firstentry, unsigned int numentries, PALETTEENTRY *pPalEntries, int a4)
{
	assert(firstentry == 0);
	assert(numentries == 256);

	SDL_Color colors[256];
	for (unsigned int i = firstentry; i < numentries; i++) {
		SDL_Color *c = &colors[i];
		PALETTEENTRY *p = &pPalEntries[i];
		c->r = p->peRed;
		c->g = p->peGreen;
		c->b = p->peBlue;
#ifndef USE_SDL1
		c->a = SDL_ALPHA_OPAQUE;
#endif
	}

	assert(palette);
	if (SDL_SetPaletteColors(palette, colors, firstentry, numentries) <= -1) {
		SDL_Log(SDL_GetError());
		return false;
	}

#ifdef USE_SDL1
	// In SDL2, the `pal_surface` owns the `pallete`, so modifying the palette
	// directly updates the `pal_surface` palette.
	//
	// In SDL1, the `palette` is independent of `pal_surface`, so we need to
	// explictly update the surface's palette here (SetPalette *copies* the palette).
	if (SDL_SetPalette(pal_surface, SDL_LOGPAL, palette->colors, 0, palette->ncolors) != 1) {
		SDL_Log(SDL_GetError());
		return false;
	}
#endif
	++pal_surface_palette_version;

	return true;
}

} // namespace dvl
