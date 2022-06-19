#include "utils/surface_to_cel.hpp"

namespace devilution {

namespace {

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

void AppendCelTransparentRun(unsigned width, std::vector<uint8_t> &out)
{
	while (width >= 128) {
		out.push_back(0x80);
		width -= 128;
	}
	if (width == 0)
		return;
	out.push_back(0xFF - (width - 1));
}

void AppendCelSolidRun(const uint8_t *src, unsigned width, std::vector<uint8_t> &out)
{
	while (width >= 127) {
		out.push_back(127);
		for (size_t i = 0; i < 127; ++i)
			out.push_back(src[i]);
		width -= 127;
		src += 127;
	}
	if (width == 0)
		return;
	out.push_back(width);
	for (size_t i = 0; i < width; ++i)
		out.push_back(src[i]);
}

void AppendCelLine(const uint8_t *src, unsigned width, uint8_t transparentColorIndex, std::vector<uint8_t> &out)
{
	unsigned runBegin = 0;
	bool transparentRun = false;
	for (unsigned i = 0; i < width; ++i) {
		const uint8_t pixel = src[i];
		if (pixel == transparentColorIndex) {
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

OwnedCelSpriteWithFrameHeight SurfaceToCel(const Surface &surface, unsigned numFrames, bool generateFrameHeaders,
    uint8_t transparentColorIndex)
{
	// CEL header: frame count, frame offset for each frame, file size
	std::vector<uint8_t> celData(4 * (2 + static_cast<size_t>(numFrames)));
	WriteLE32(celData.data(), numFrames);

	const auto height = static_cast<unsigned>(surface.h());
	const auto width = static_cast<unsigned>(surface.w());
	const auto pitch = static_cast<unsigned>(surface.pitch());
	const unsigned frameHeight = height / numFrames;

	// We process the surface a whole frame at a time because the lines are reversed in CEL.
	const uint8_t *dataPtr = surface.begin();
	for (unsigned frame = 1; frame <= numFrames; ++frame) {
		WriteLE32(&celData[4 * static_cast<size_t>(frame)], static_cast<uint32_t>(celData.size()));

		// Frame header: 5 16-bit offsets to 32-pixel height blocks.
		const size_t frameHeaderPos = celData.size();
		if (generateFrameHeaders) {
			constexpr size_t FrameHeaderSize = 10;
			celData.resize(celData.size() + FrameHeaderSize);
			WriteLE16(&celData[frameHeaderPos], FrameHeaderSize);
		}
		size_t line = frameHeight;
		dataPtr += static_cast<unsigned>(pitch * frameHeight);
		while (line-- != 0) {
			dataPtr -= pitch;
			AppendCelLine(dataPtr, width, transparentColorIndex, celData);
			if (generateFrameHeaders) {
				switch (line) {
				case 32:
					WriteLE16(&celData[frameHeaderPos + 2], static_cast<uint16_t>(celData.size() - frameHeaderPos));
					break;
				case 64:
					WriteLE16(&celData[frameHeaderPos + 4], static_cast<uint16_t>(celData.size() - frameHeaderPos));
					break;
				case 96:
					WriteLE16(&celData[frameHeaderPos + 6], static_cast<uint16_t>(celData.size() - frameHeaderPos));
					break;
				case 128:
					WriteLE16(&celData[frameHeaderPos + 8], static_cast<uint16_t>(celData.size() - frameHeaderPos));
					break;
				}
			}
		}
		dataPtr += static_cast<unsigned>(pitch * frameHeight);
	}

	WriteLE32(&celData[4 * (1 + static_cast<size_t>(numFrames))], static_cast<uint32_t>(celData.size()));

	auto out = std::unique_ptr<byte[]>(new byte[celData.size()]);
	memcpy(&out[0], celData.data(), celData.size());
	return OwnedCelSpriteWithFrameHeight {
		OwnedCelSprite { std::move(out), static_cast<uint16_t>(width) },
		frameHeight
	};
}

} // namespace devilution
