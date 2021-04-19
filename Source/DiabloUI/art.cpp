#include "DiabloUI/art.h"
#include "storm/storm.h"
#include "utils/display.h"
#include "utils/sdl_compat.h"

namespace devilution {

void LoadArt(const char *pszFile, Art *art, int frames, SDL_Color *pPalette)
{
	if (art == nullptr || art->surface != nullptr)
		return;

	art->frames = frames;

	DWORD width, height, bpp;
	if (!SBmpLoadImage(pszFile, nullptr, nullptr, 0, &width, &height, &bpp)) {
		SDL_Log("Failed to load image meta");
		return;
	}

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
	SDLSurfaceUniquePtr art_surface { SDL_CreateRGBSurfaceWithFormat(SDL_SWSURFACE, width, height, bpp, format) };

	if (!SBmpLoadImage(pszFile, pPalette, static_cast<BYTE *>(art_surface->pixels),
	        art_surface->pitch * art_surface->format->BytesPerPixel * height, nullptr, nullptr, nullptr)) {
		SDL_Log("Failed to load image");
		return;
	}

	art->logical_width = art_surface->w;
	art->frame_height = height / frames;

	art->surface = ScaleSurfaceToOutput(std::move(art_surface));
}

void LoadMaskedArt(const char *pszFile, Art *art, int frames, int mask)
{
	LoadArt(pszFile, art, frames);
	if (art->surface != nullptr)
		SDLC_SetColorKey(art->surface.get(), mask);
}

void LoadArt(Art *art, const BYTE *artData, int w, int h, int frames)
{
	art->frames = frames;
	art->surface = ScaleSurfaceToOutput(SDLSurfaceUniquePtr { SDL_CreateRGBSurfaceWithFormatFrom(
	    const_cast<BYTE *>(artData), w, h, 8, w, SDL_PIXELFORMAT_INDEX8) });
	art->logical_width = w;
	art->frame_height = h / frames;
}

} // namespace devilution
