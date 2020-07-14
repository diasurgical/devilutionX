#include <SDL.h>

#include "devilution.h"
#include "miniwin/ddraw.h"
#include "stubs.h"

namespace dvl {

void SDrawUpdatePalette(unsigned int firstentry, unsigned int numentries, PALETTEENTRY *pPalEntries, int a4)
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
	if (SDLC_SetSurfaceAndPaletteColors(pal_surface, palette, colors, firstentry, numentries) <= -1) {
		ErrSdl();
	}
	pal_surface_palette_version++;
}

} // namespace dvl
