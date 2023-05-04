#pragma once

#include <cstdint>
#include <cstring>

#include "engine/palette.h"
#include "utils/attributes.h"

namespace devilution {

enum class BlitType : uint8_t {
	Transparent,
	Pixels,
	Fill
};

struct BlitCommand {
	BlitType type;
	const uint8_t *srcEnd; // Pointer past the end of the command.
	unsigned length;       // Number of pixels this command will write.
	uint8_t color;         // For `BlitType::Pixel` and `BlitType::Fill` only.
};

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void BlitFillDirect(uint8_t *dst, unsigned length, uint8_t color)
{
	std::memset(dst, color, length);
}

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void BlitPixelsDirect(uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src, unsigned length)
{
	std::memcpy(dst, src, length);
}

struct BlitDirect {
	DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void operator()(BlitCommand cmd, uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src)
	{
		switch (cmd.type) {
		case BlitType::Fill:
			BlitFillDirect(dst, cmd.length, cmd.color);
			return;
		case BlitType::Pixels:
			BlitPixelsDirect(dst, src, cmd.length);
			return;
		case BlitType::Transparent:
			return;
		}
	}
};

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void BlitFillWithMap(uint8_t *dst, unsigned length, uint8_t color, const uint8_t *DVL_RESTRICT colorMap)
{
	assert(length != 0);
	std::memset(dst, colorMap[color], length);
}

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void BlitPixelsWithMap(uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src, unsigned length, const uint8_t *DVL_RESTRICT colorMap)
{
	assert(length != 0);
	const uint8_t *end = src + length;
	while (src + 3 < end) {
		*dst++ = colorMap[*src++];
		*dst++ = colorMap[*src++];
		*dst++ = colorMap[*src++];
		*dst++ = colorMap[*src++];
	}
	while (src < end) {
		*dst++ = colorMap[*src++];
	}
}

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void BlitFillBlended(uint8_t *dst, unsigned length, uint8_t color)
{
	assert(length != 0);
	const uint8_t *end = dst + length;
	const std::array<uint8_t, 256> &tbl = paletteTransparencyLookup[color];
	while (dst + 3 < end) {
		*dst = tbl[*dst];
		++dst;
		*dst = tbl[*dst];
		++dst;
		*dst = tbl[*dst];
		++dst;
		*dst = tbl[*dst];
		++dst;
	}
	while (dst < end) {
		*dst = tbl[*dst];
		++dst;
	}
}

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void BlitPixelsBlended(uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src, unsigned length)
{
	assert(length != 0);
	const uint8_t *end = src + length;
	while (src + 3 < end) {
		*dst = paletteTransparencyLookup[*dst][*src++];
		++dst;
		*dst = paletteTransparencyLookup[*dst][*src++];
		++dst;
		*dst = paletteTransparencyLookup[*dst][*src++];
		++dst;
		*dst = paletteTransparencyLookup[*dst][*src++];
		++dst;
	}
	while (src < end) {
		*dst = paletteTransparencyLookup[*dst][*src++];
		++dst;
	}
}

struct BlitWithMap {
	const uint8_t *DVL_RESTRICT colorMap;

	DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void operator()(BlitCommand cmd, uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src) const
	{
		switch (cmd.type) {
		case BlitType::Fill:
			BlitFillWithMap(dst, cmd.length, cmd.color, colorMap);
			return;
		case BlitType::Pixels:
			BlitPixelsWithMap(dst, src, cmd.length, colorMap);
			return;
		case BlitType::Transparent:
			return;
		}
	}
};

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void BlitPixelsBlendedWithMap(uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src, unsigned length, const uint8_t *DVL_RESTRICT colorMap)
{
	assert(length != 0);
	const uint8_t *end = src + length;
	while (src + 3 < end) {
		*dst = paletteTransparencyLookup[*dst][colorMap[*src++]];
		++dst;
		*dst = paletteTransparencyLookup[*dst][colorMap[*src++]];
		++dst;
		*dst = paletteTransparencyLookup[*dst][colorMap[*src++]];
		++dst;
		*dst = paletteTransparencyLookup[*dst][colorMap[*src++]];
		++dst;
	}
	while (src < end) {
		*dst = paletteTransparencyLookup[*dst][colorMap[*src++]];
		++dst;
	}
}

struct BlitBlendedWithMap {
	const uint8_t *colorMap;

	DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void operator()(BlitCommand cmd, uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src) const
	{
		switch (cmd.type) {
		case BlitType::Fill:
			BlitFillBlended(dst, cmd.length, colorMap[cmd.color]);
			return;
		case BlitType::Pixels:
			BlitPixelsBlendedWithMap(dst, src, cmd.length, colorMap);
			return;
		case BlitType::Transparent:
			return;
		}
	}
};

} // namespace devilution
