#include "DiabloUI/art.h"

#include <cstddef>
#include <cstdint>
#include <memory>

#include "engine/assets.hpp"
#include "utils/display.h"
#include "utils/log.hpp"
#include "utils/pcx.hpp"
#include "utils/sdl_compat.h"
#include "utils/sdl_wrap.h"

namespace devilution {
namespace {

Uint32 GetPcxSdlPixelFormat(unsigned bpp)
{
	switch (bpp) {
	case 8: // NOLINT(readability-magic-numbers)
		return SDL_PIXELFORMAT_INDEX8;
	case 24: // NOLINT(readability-magic-numbers)
		return SDL_PIXELFORMAT_RGB888;
	case 32: // NOLINT(readability-magic-numbers)
		return SDL_PIXELFORMAT_RGBA8888;
	default:
		return 0;
	}
}

} // namespace

void LoadArt(const char *pszFile, Art *art, int frames, SDL_Color *pPalette, const std::array<uint8_t, 256> *colorMapping)
{
	if (art == nullptr || art->surface != nullptr)
		return;

	art->frames = frames;

	int width;
	int height;
	std::uint8_t bpp;
	SDL_RWops *handle = OpenAsset(pszFile);
	if (handle == nullptr) {
		return;
	}

	if (!LoadPcxMeta(handle, width, height, bpp)) {
		Log("LoadArt(\"{}\"): LoadPcxMeta failed with code {}", pszFile, SDL_GetError());
		SDL_RWclose(handle);
		return;
	}

	SDLSurfaceUniquePtr artSurface = SDLWrap::CreateRGBSurfaceWithFormat(SDL_SWSURFACE, width, height, bpp, GetPcxSdlPixelFormat(bpp));
	if (!LoadPcxPixelsAndPalette(handle, width, height, bpp, static_cast<uint8_t *>(artSurface->pixels),
	        artSurface->pitch, pPalette)) {
		Log("LoadArt(\"{}\"): LoadPcxPixelsAndPalette failed with code {}", pszFile, SDL_GetError());
		SDL_RWclose(handle);
		return;
	}
	SDL_RWclose(handle);

	if (colorMapping != nullptr) {
		for (int i = 0; i < artSurface->h * artSurface->pitch; i++) {
			auto &pixel = static_cast<uint8_t *>(artSurface->pixels)[i];
			pixel = (*colorMapping)[pixel];
		}
	}

	art->logical_width = artSurface->w;
	art->frame_height = height / frames;

	art->surface = ScaleSurfaceToOutput(std::move(artSurface));
}

void LoadMaskedArt(const char *pszFile, Art *art, int frames, int mask, const std::array<uint8_t, 256> *colorMapping)
{
	LoadArt(pszFile, art, frames, nullptr, colorMapping);
	if (art->surface != nullptr)
		SDLC_SetColorKey(art->surface.get(), mask);
}

void LoadArt(Art *art, const std::uint8_t *artData, int w, int h, int frames)
{
	constexpr int DefaultArtBpp = 8;
	constexpr int DefaultArtFormat = SDL_PIXELFORMAT_INDEX8;
	art->frames = frames;
	art->surface = ScaleSurfaceToOutput(SDLWrap::CreateRGBSurfaceWithFormatFrom(
	    const_cast<std::uint8_t *>(artData), w, h, DefaultArtBpp, w, DefaultArtFormat));
	art->logical_width = w;
	art->frame_height = h / frames;
}

} // namespace devilution
