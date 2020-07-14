#include "DiabloUI/art.h"
#include "display.h"

namespace dvl {

void LoadArt(const char *pszFile, Art *art, int frames, SDL_Color *pPalette)
{
	if (art == NULL || art->surface != NULL)
		return;

	art->frames = frames;

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
		art->surface = NULL;
		return;
	}

	art->surface = art_surface;
	art->logical_width = art_surface->w;
	art->frame_height = height / frames;

	ScaleSurfaceToOutput(&art->surface);
}

void LoadMaskedArt(const char *pszFile, Art *art, int frames, int mask)
{
	LoadArt(pszFile, art, frames);
	if (art->surface != NULL)
		SDLC_SetColorKey(art->surface, mask);
}

void LoadArt(Art *art, const BYTE *artData, int w, int h, int frames)
{
	art->frames = frames;
	art->surface = SDL_CreateRGBSurfaceWithFormatFrom(
		const_cast<BYTE *>(artData), w, h, 8, w, SDL_PIXELFORMAT_INDEX8);
	art->logical_width = w;
	art->frame_height = h / frames;
	ScaleSurfaceToOutput(&art->surface);
}

} // namespace dvl
