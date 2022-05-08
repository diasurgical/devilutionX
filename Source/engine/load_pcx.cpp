#include "engine/load_pcx.hpp"

#include <cstddef>
#include <memory>
#include <utility>

#include <SDL.h>

#include "appfat.h"
#include "engine/assets.hpp"
#include "utils/log.hpp"
#include "utils/pcx.hpp"

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

namespace devilution {

namespace {

std::unique_ptr<uint32_t[]> LoadFrameOffsets(PcxSprite sprite, uint16_t numFrames)
{
	uint16_t frameHeight = sprite.height() / numFrames;
	std::unique_ptr<uint32_t[]> frameOffsets { new uint32_t[numFrames] };
	frameOffsets[0] = 0;
	const unsigned width = sprite.width();
	const unsigned srcSkip = width % 2;
	const uint8_t *data = sprite.data();
	for (unsigned frame = 1; frame < numFrames; ++frame) {
		for (unsigned y = 0; y < frameHeight; ++y) {
			for (unsigned x = 0; x < width;) {
				constexpr uint8_t PcxMaxSinglePixel = 0xBF;
				const uint8_t byte = *data++;
				if (byte <= PcxMaxSinglePixel) {
					++x;
					continue;
				}
				constexpr uint8_t PcxRunLengthMask = 0x3F;
				const uint8_t runLength = (byte & PcxRunLengthMask);
				++data;
				x += runLength;
			}
		}
		data += srcSkip;
		frameOffsets[frame] = static_cast<uint32_t>(data - sprite.data());
	}
	return frameOffsets;
}

} // namespace

std::optional<OwnedPcxSprite> LoadPcxAsset(const char *path, std::optional<uint8_t> transparentColor, SDL_Color *outPalette)
{
	SDL_RWops *handle = OpenAsset(path);
	if (handle == nullptr) {
		LogError("Missing file: {}", path);
		return std::nullopt;
	}

	int width;
	int height;
	uint8_t bpp;
	if (!LoadPcxMeta(handle, width, height, bpp)) {
		SDL_RWclose(handle);
		return std::nullopt;
	}
	assert(bpp == 8);

	ptrdiff_t pixelDataSize = SDL_RWsize(handle);
	if (pixelDataSize < 0) {
		SDL_RWclose(handle);
		return std::nullopt;
	}
	constexpr unsigned NumPaletteColors = 256;
	constexpr unsigned PcxPaletteSize = 1 + NumPaletteColors * 3;
	pixelDataSize -= PcxHeaderSize;
	if (outPalette != nullptr)
		pixelDataSize -= PcxPaletteSize;

	std::unique_ptr<uint8_t[]> fileBuffer { new uint8_t[pixelDataSize] };
	if (SDL_RWread(handle, fileBuffer.get(), pixelDataSize, 1) == 0) {
		SDL_RWclose(handle);
		return std::nullopt;
	}

	if (outPalette != nullptr) {
		// The file has a 256 color palette that needs to be loaded.
		uint8_t paletteData[PcxPaletteSize];
		if (SDL_RWread(handle, paletteData, PcxPaletteSize, 1) == 0) {
			SDL_RWclose(handle);
			return std::nullopt;
		}
		const uint8_t *dataPtr = paletteData;
		[[maybe_unused]] constexpr unsigned PcxPaletteSeparator = 0x0C;
		assert(*dataPtr == PcxPaletteSeparator); // sanity check the delimiter
		++dataPtr;

		SDL_Color *out = outPalette;
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

	SDL_RWclose(handle);

	return OwnedPcxSprite { std::move(fileBuffer), static_cast<uint16_t>(width), static_cast<uint16_t>(height), transparentColor };
}

std::optional<OwnedPcxSpriteSheet> LoadPcxSpriteSheetAsset(const char *path, uint16_t numFrames, std::optional<uint8_t> transparentColor, SDL_Color *palette)
{
	std::optional<OwnedPcxSprite> ownedSprite = LoadPcxAsset(path, transparentColor, palette);
	if (ownedSprite == std::nullopt)
		return std::nullopt;
	std::unique_ptr<uint32_t[]> frameOffsets = LoadFrameOffsets(PcxSprite { *ownedSprite }, numFrames);
	return OwnedPcxSpriteSheet { *std::move(ownedSprite), std::move(frameOffsets), numFrames };
}

} // namespace devilution
