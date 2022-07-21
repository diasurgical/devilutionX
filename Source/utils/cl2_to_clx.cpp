#include "utils/cl2_to_clx.hpp"

#include <cstring>

#include "utils/endian.hpp"

namespace devilution {

namespace {

constexpr bool IsCl2Opaque(uint8_t control)
{
	constexpr uint8_t Cl2OpaqueMin = 0x80;
	return control >= Cl2OpaqueMin;
}

constexpr uint8_t GetCl2OpaquePixelsWidth(uint8_t control)
{
	return -static_cast<std::int8_t>(control);
}

constexpr bool IsCl2OpaqueFill(uint8_t control)
{
	constexpr uint8_t Cl2FillMax = 0xBE;
	return control <= Cl2FillMax;
}

constexpr uint8_t GetCl2OpaqueFillWidth(uint8_t control)
{
	constexpr uint8_t Cl2FillEnd = 0xBF;
	return static_cast<int_fast16_t>(Cl2FillEnd - control);
}

size_t CountCl2FramePixels(const uint8_t *src, const uint8_t *srcEnd)
{
	size_t numPixels = 0;
	while (src != srcEnd) {
		uint8_t val = *src++;
		if (IsCl2Opaque(val)) {
			if (IsCl2OpaqueFill(val)) {
				numPixels += GetCl2OpaqueFillWidth(val);
				++src;
			} else {
				val = GetCl2OpaquePixelsWidth(val);
				numPixels += val;
				src += val;
			}
		} else {
			numPixels += val;
		}
	}
	return numPixels;
}

} // namespace

uint16_t Cl2ToClx(uint8_t *data, size_t size, PointerOrValue<uint16_t> widthOrWidths)
{
	uint32_t numGroups = 1;
	const uint32_t maybeNumFrames = LoadLE32(data);
	uint8_t *groupBegin = data;

	// If it is a number of frames, then the last frame offset will be equal to the size of the file.
	if (LoadLE32(&data[maybeNumFrames * 4 + 4]) != size) {
		// maybeNumFrames is the address of the first group, right after
		// the list of group offsets.
		numGroups = maybeNumFrames / 4;
	}

	for (size_t group = 0; group < numGroups; ++group) {
		uint32_t numFrames;
		if (numGroups == 1) {
			numFrames = maybeNumFrames;
		} else {
			groupBegin = &data[LoadLE32(&data[group * 4])];
			numFrames = LoadLE32(groupBegin);
		}

		uint8_t *frameEnd = &groupBegin[LoadLE32(&groupBegin[4])];
		for (size_t frame = 1; frame <= numFrames; ++frame) {
			uint8_t *frameBegin = frameEnd;
			frameEnd = &groupBegin[LoadLE32(&groupBegin[4 * (frame + 1)])];

			constexpr size_t Cl2FrameHeaderSize = 10;
			const size_t numPixels = CountCl2FramePixels(frameBegin + Cl2FrameHeaderSize, frameEnd);

			const uint16_t frameWidth = widthOrWidths.HoldsPointer() ? widthOrWidths.AsPointer()[frame - 1] : widthOrWidths.AsValue();
			const uint16_t frameHeight = numPixels / frameWidth;
			WriteLE16(&frameBegin[2], frameWidth);
			WriteLE16(&frameBegin[4], frameHeight);
			memset(&frameBegin[6], 0, 4);
		}
	}
	return numGroups == 1 ? 0 : numGroups;
}

} // namespace devilution
