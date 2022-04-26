#include "utils/pcx.hpp"

#include <cstring>
#include <memory>

#include "appfat.h"

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

namespace devilution {
namespace {
constexpr unsigned NumPaletteColors = 256;
constexpr unsigned PcxPaletteSize = 1 + NumPaletteColors * 3;
} // namespace

bool LoadPcxMeta(SDL_RWops *handle, int &width, int &height, uint8_t &bpp)
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
    uint8_t *buffer, std::ptrdiff_t bufferPitch, SDL_Color *palette)
{
	std::ptrdiff_t pixelDataSize = SDL_RWsize(handle);
	if (pixelDataSize < 0) {
		// Unable to determine size, or an error occurred.
		return false;
	}

	// SDL_RWsize gives the total size of the file however we've already read the header from an earlier call to
	//  LoadPcxMeta, so we only need to read the remainder of the file.
	const std::size_t readSize = pixelDataSize - PcxHeaderSize;
	std::unique_ptr<uint8_t[]> fileBuffer { new uint8_t[readSize] };
	if (SDL_RWread(handle, fileBuffer.get(), readSize, 1) == 0) {
		return false;
	}
	const std::ptrdiff_t xSkip = bufferPitch - width;
	const std::ptrdiff_t srcSkip = width % 2;
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

	if (palette != nullptr && bpp == 8) {
		// The file has a 256 color palette that needs to be loaded.
		[[maybe_unused]] constexpr unsigned PcxPaletteSeparator = 0x0C;
		assert(*dataPtr == PcxPaletteSeparator); // sanity check the delimiter
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

} // namespace devilution
