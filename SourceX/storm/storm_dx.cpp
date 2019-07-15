#include <SDL.h>

#include "devilution.h"
#include "miniwin/ddraw.h"
#include "stubs.h"

namespace dvl {

BOOL SDrawManualInitialize(
    HWND hWnd,
    LPDIRECTDRAW ddInterface,
    LPDIRECTDRAWSURFACE primarySurface,
    LPDIRECTDRAWSURFACE surface2,
    LPDIRECTDRAWSURFACE surface3,
    LPDIRECTDRAWSURFACE backSurface,
    LPDIRECTDRAWPALETTE ddPalette,
    HPALETTE hPalette)
{
	DUMMY();
	return true;
}

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
		c->unused = SDL_ALPHA_OPAQUE;
	}

	assert(palette);
	SDL_SetPalette(pal_surface, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, 256);
	if (SDL_SetPaletteColors(palette, colors, firstentry, numentries) <= -1) { // Todo(Amiga): Fix this!

		//klaus_OK
		SDL_Log(SDL_GetError());
		return false;
	}
	return true;
}

} // namespace dvl
