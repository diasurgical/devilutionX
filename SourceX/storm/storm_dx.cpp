#include <SDL.h>

#include "devilution.h"
#include "dx.h"
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
		c->a = SDL_ALPHA_OPAQUE;
	}

	assert(palette);
	if (SDL_SetPaletteColors(palette, colors, firstentry, numentries) != 0) {
		SDL_Log("SDL_SetPaletteColors: %s\n", SDL_GetError());
		return false;
	}

	if (pal_surface) {
		sdl_update_entire_surface();
		sdl_present_surface();
	}

	return true;
}

}  // namespace dvl
