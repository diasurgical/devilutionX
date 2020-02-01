#include <SDL.h>

#include "devilution.h"
#include "display.h"
#include "stubs.h"

namespace dvl {

void SDrawUpdatePalette(unsigned int firstentry, unsigned int numentries, SDL_Color *pPalEntries, int a4)
{
	assert(firstentry == 0);
	assert(numentries == 256);

	SDL_Color colors[256];
	for (unsigned int i = firstentry; i < numentries; i++) {
		colors[i] = pPalEntries[i];
	}

	assert(palette);
	if (SDLC_SetSurfaceAndPaletteColors(pal_surface, palette, colors, firstentry, numentries) <= -1) {
		ErrSdl();
	}
	pal_surface_palette_version++;
}

} // namespace dvl
