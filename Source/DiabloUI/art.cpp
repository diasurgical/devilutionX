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
constexpr size_t PcxHeaderSize = 128;
constexpr unsigned NumPaletteColors = 256;
constexpr unsigned PcxPaletteSize = 1 + NumPaletteColors * 3;

bool LoadPcxMeta(SDL_RWops *handle, int &width, int &height, std::uint8_t &bpp)
{
	PCXHeader pcxhdr;
	if (SDL_RWread(handle, &pcxhdr, PcxHeaderSize, 1) == 0) {
		return false;
	}
	width = SDL_SwapLE16(pcxhdr.Xmax) - SDL_SwapLE16(pcxhdr.Xmin) + 1;
	height = SDL_SwapLE16(pcxhdr.Ymax) - SDL_SwapLE16(pcxhdr.Ymin) + 1;
	bpp = pcxhdr.BitsPerPixel;
	return true;
}

bool LoadPcxPixelsAndPalette(SDL_RWops *handle, int width, int height, std::uint8_t bpp,
    uint8_t *buffer, std::size_t bufferPitch, SDL_Color *palette)
{
	const bool has256ColorPalette = palette != nullptr && bpp == 8;
	std::uint32_t pixelDataSize = SDL_RWsize(handle);
	if (pixelDataSize == static_cast<std::uint32_t>(-1)) {
		return false;
	}
	pixelDataSize -= PcxHeaderSize + (has256ColorPalette ? PcxPaletteSize : 0);

	// We read 1 extra byte because it delimits the palette.
	const size_t readSize = pixelDataSize + (has256ColorPalette ? PcxPaletteSize : 0);
	std::unique_ptr<uint8_t[]> fileBuffer { new uint8_t[readSize] };
	if (SDL_RWread(handle, fileBuffer.get(), readSize, 1) == 0) {
		return false;
	}
	const unsigned xSkip = bufferPitch - width;
	const unsigned srcSkip = width % 2;
	uint8_t *dataPtr = fileBuffer.get();
	for (int j = 0; j < height; j++) {
		for (int x = 0; x < width;) {
			constexpr std::uint8_t PcxMaxSinglePixel = 0xBF;
			const std::uint8_t byte = *dataPtr++;
			if (byte <= PcxMaxSinglePixel) {
				*buffer++ = byte;
				++x;
				continue;
			}
			constexpr std::uint8_t PcxRunLengthMask = 0x3F;
			const std::uint8_t runLength = (byte & PcxRunLengthMask);
			std::memset(buffer, *dataPtr++, runLength);
			buffer += runLength;
			x += runLength;
		}
		dataPtr += srcSkip;
		buffer += xSkip;
	}

	if (has256ColorPalette) {
		[[maybe_unused]] constexpr unsigned PcxPaletteSeparator = 0x0C;
		assert(*dataPtr == PcxPaletteSeparator);
		++dataPtr;

		auto *out = palette;
		for (unsigned i = 0; i < NumPaletteColors; ++i) {
			out->r = *dataPtr++;
			out->g = *dataPtr++;
			out->b = *dataPtr++;
#ifndef USE_SDL1
			out->a = SDL_ALPHA_OPAQUE;
#endif
			++out;
		}
	}
	return true;
}

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
