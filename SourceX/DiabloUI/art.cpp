#include "DiabloUI/art.h"

namespace dvl {

void LoadArt(const char *pszFile, Art *art, int frames, PALETTEENTRY *pPalette)
{
	if (art == NULL || art->surface != NULL)
		return;

	DWORD width, height, bpp;
	if (!SBmpLoadImage(pszFile, 0, 0, 0, &width, &height, &bpp))
		return;

	Uint32 format;
	switch (bpp) {
	case 8:
		format = SDL_PIXELFORMAT_INDEX8;
		break;
	case 24:
		format = SDL_PIXELFORMAT_RGB888;
		break;
	case 32:
		format = SDL_PIXELFORMAT_RGBA8888;
		break;
	default:
		format = 0;
		break;
	}
	SDL_Surface *art_surface = SDL_CreateRGBSurfaceWithFormat(SDL_SWSURFACE, width, height, bpp, format);

	if (!SBmpLoadImage(pszFile, pPalette, static_cast<BYTE *>(art_surface->pixels),
	        art_surface->pitch * art_surface->format->BytesPerPixel * height, 0, 0, 0)) {
		SDL_Log("Failed to load image");
		SDL_FreeSurface(art_surface);
		art->surface = nullptr;
		return;
	}

	art->surface = art_surface;
	art->frames = frames;
	art->frame_height = height / frames;
}

void LoadMaskedArt(const char *pszFile, Art *art, int frames, int mask)
{
	LoadArt(pszFile, art, frames);
	if (art->surface != nullptr) {
#ifdef USE_SDL1
		SDL_SetColorKey(art->surface, SDL_SRCCOLORKEY, mask);
#else
		SDL_SetColorKey(art->surface, SDL_TRUE, mask);
#endif
	}
}

} // namespace dvl
