#include "utils/pcx_to_cel.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

#include "appfat.h"
#include "utils/pcx.hpp"
#include "utils/stdcompat/cstddef.hpp"

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

namespace devilution {

namespace {

constexpr uint8_t PcxTransparentColorIndex = 1;

void WriteLE32(uint8_t *out, uint32_t val)
{
	const uint32_t littleEndian = SDL_SwapLE32(val);
	memcpy(out, &littleEndian, 4);
}

void WriteLE16(uint8_t *out, uint16_t val)
{
	const uint16_t littleEndian = SDL_SwapLE16(val);
	memcpy(out, &littleEndian, 2);
}

void AppendCelTransparentRun(uint8_t width, std::vector<uint8_t> &out)
{
	out.push_back(0xFF - (width - 1));
}

void AppendCelSolidRun(const uint8_t *src, unsigned width, std::vector<uint8_t> &out)
{
	assert(width < 126);
	out.push_back(width);
	for (size_t i = 0; i < width; ++i)
		out.push_back(src[i]);
}

void AppendCelLine(const uint8_t *src, unsigned width, std::vector<uint8_t> &out)
{
	unsigned runBegin = 0;
	bool transparentRun = false;
	for (unsigned i = 0; i < width; ++i) {
		const uint8_t pixel = src[i];
		if (pixel == PcxTransparentColorIndex) {
			if (transparentRun)
				continue;
			if (runBegin != i)
				AppendCelSolidRun(src + runBegin, i - runBegin, out);
			transparentRun = true;
			runBegin = i;
		} else if (transparentRun) {
			AppendCelTransparentRun(i - runBegin, out);
			transparentRun = false;
			runBegin = i;
		}
	}
	if (transparentRun) {
		AppendCelTransparentRun(width - runBegin, out);
	} else {
		AppendCelSolidRun(src + runBegin, width - runBegin, out);
	}
}

} // namespace

std::optional<OwnedCelSpriteWithFrameHeight> LoadPcxAsCel(SDL_RWops *handle, unsigned numFrames, bool generateFrameHeaders)
{
	int width;
	int height;
	uint8_t bpp;
	if (!LoadPcxMeta(handle, width, height, bpp)) {
		SDL_RWclose(handle);
		return std::nullopt;
	}
	assert(bpp == 8);
	assert(width <= 128);

	uint32_t pixelDataSize = SDL_RWsize(handle);
	if (pixelDataSize == static_cast<uint32_t>(-1)) {
		SDL_RWclose(handle);
		return std::nullopt;
	}

	pixelDataSize -= PcxHeaderSize;

	std::unique_ptr<uint8_t[]> fileBuffer { new uint8_t[pixelDataSize] };
	if (SDL_RWread(handle, fileBuffer.get(), pixelDataSize, 1) == 0) {
		SDL_RWclose(handle);
		return std::nullopt;
	}

	// CEL header: frame count, frame offset for each frame, file size
	std::vector<uint8_t> celData(4 * (2 + static_cast<size_t>(numFrames)));
	WriteLE32(&celData[0], numFrames);

	// We process the PCX a whole frame at a time because the lines are reversed in CEL.
	const unsigned frameHeight = height / numFrames;
	auto frameBuffer = std::unique_ptr<uint8_t[]>(new uint8_t[static_cast<size_t>(frameHeight) * width]);

	const unsigned srcSkip = width % 2;
	uint8_t *dataPtr = fileBuffer.get();
	for (unsigned frame = 1; frame <= numFrames; ++frame) {
		WriteLE32(&celData[4 * static_cast<size_t>(frame)], static_cast<uint32_t>(celData.size()));

		// Frame header: 5 16-bit offsets to 32-pixel height blocks.
		const size_t frameHeaderPos = celData.size();
		if (generateFrameHeaders) {
			constexpr size_t FrameHeaderSize = 10;
			celData.resize(celData.size() + FrameHeaderSize);
			WriteLE16(&celData[frameHeaderPos], FrameHeaderSize);
		}

		for (unsigned j = 0; j < frameHeight; ++j) {
			uint8_t *buffer = &frameBuffer[static_cast<size_t>(j) * width];
			for (unsigned x = 0; x < static_cast<unsigned>(width);) {
				constexpr uint8_t PcxMaxSinglePixel = 0xBF;
				const uint8_t byte = *dataPtr++;
				if (byte <= PcxMaxSinglePixel) {
					*buffer++ = byte;
					++x;
					continue;
				}
				constexpr uint8_t PcxRunLengthMask = 0x3F;
				const uint8_t runLength = (byte & PcxRunLengthMask);
				std::memset(buffer, *dataPtr++, runLength);
				buffer += runLength;
				x += runLength;
			}
			dataPtr += srcSkip;
		}

		size_t line = frameHeight;
		while (line-- != 0) {
			AppendCelLine(&frameBuffer[line * width], width, celData);
			if (generateFrameHeaders) {
				switch (line) {
				case 32:
					WriteLE16(&celData[frameHeaderPos + 2], celData.size() - frameHeaderPos);
					break;
				case 64:
					WriteLE16(&celData[frameHeaderPos + 4], celData.size() - frameHeaderPos);
					break;
				case 96:
					WriteLE16(&celData[frameHeaderPos + 6], celData.size() - frameHeaderPos);
					break;
				case 128:
					WriteLE16(&celData[frameHeaderPos + 8], celData.size() - frameHeaderPos);
					break;
				}
			}
		}
	}
	WriteLE32(&celData[4 * (1 + static_cast<size_t>(numFrames))], static_cast<uint32_t>(celData.size()));

	SDL_RWclose(handle);

	auto out = std::unique_ptr<byte[]>(new byte[celData.size()]);
	memcpy(&out[0], celData.data(), celData.size());
	return OwnedCelSpriteWithFrameHeight {
		OwnedCelSprite { std::move(out), static_cast<uint16_t>(width) },
		static_cast<unsigned>(frameHeight)
	};
}

} // namespace devilution
