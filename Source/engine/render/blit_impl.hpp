#pragma once

#include <cstdint>
#include <cstring>

#include "engine/palette.h"
#include "utils/attributes.h"

namespace devilution {

enum class BlitType : uint8_t {
	Transparent,
	Pixels,
	Pixel,
	Fill
};

struct BlitCommand {
	BlitType type;
	const uint8_t *srcEnd; // Pointer past the end of the command.
	unsigned length;       // Number of pixels this command will write.
	uint8_t color;         // For `BlitType::Pixel` and `BlitType::Fill` only.
};

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT BlitCommand TransformBlitCommandNoop(BlitCommand cmd)
{
	return cmd;
}

struct TransformBlitCommandTransparentColor {
	uint8_t transparentColor;

	BlitCommand operator()(BlitCommand cmd) const
	{
		if (cmd.color == transparentColor)
			cmd.type = BlitType::Transparent;
		return cmd;
	}
};

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void BlitFillDirect(uint8_t *dst, unsigned length, uint8_t color)
{
	std::memset(dst, color, length);
}

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void BlitPixelsDirect(uint8_t *dst, const uint8_t *src, unsigned length)
{
	std::memcpy(dst, src, length);
}

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void BlitPixelDirect(uint8_t *dst, uint8_t color)
{
	*dst = color;
}

struct BlitDirect {
	DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void operator()(BlitCommand cmd, uint8_t *dst, const uint8_t *src)
	{
		switch (cmd.type) {
		case BlitType::Fill:
			BlitFillDirect(dst, cmd.length, cmd.color);
			return;
		case BlitType::Pixels:
			BlitPixelsDirect(dst, src, cmd.length);
			return;
		case BlitType::Pixel:
			BlitPixelDirect(dst, cmd.color);
			return;
		case BlitType::Transparent:
			return;
		}
	}
};

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void BlitFillWithMap(uint8_t *dst, unsigned length, uint8_t color, const uint8_t *colorMap)
{
	std::memset(dst, colorMap[color], length);
}

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void BlitPixelsWithMap(uint8_t *dst, const uint8_t *src, unsigned length, const uint8_t *colorMap)
{
	while (length-- > 0)
		*dst++ = colorMap[*src++];
}

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void BlitPixelWithMap(uint8_t *dst, uint8_t color, const uint8_t *colorMap)
{
	*dst = colorMap[color];
}

struct BlitWithMap {
	const uint8_t *colorMap;

	DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void operator()(BlitCommand cmd, uint8_t *dst, const uint8_t *src) const
	{
		switch (cmd.type) {
		case BlitType::Fill:
			BlitFillWithMap(dst, cmd.length, cmd.color, colorMap);
			return;
		case BlitType::Pixels:
			BlitPixelsWithMap(dst, src, cmd.length, colorMap);
			return;
		case BlitType::Pixel:
			BlitPixelWithMap(dst, cmd.color, colorMap);
			return;
		case BlitType::Transparent:
			return;
		}
	}
};

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void BlitFillBlendedWithMap(uint8_t *dst, unsigned length, uint8_t color, const uint8_t *colorMap)
{
	color = colorMap[color];
	while (length-- > 0) {
		*dst = paletteTransparencyLookup[*dst][color];
		++dst;
	}
}

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void BlitPixelsBlendedWithMap(uint8_t *dst, const uint8_t *src, unsigned length, const uint8_t *colorMap)
{
	while (length-- > 0) {
		*dst = paletteTransparencyLookup[*dst][colorMap[*src++]];
		++dst;
	}
}

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void BlitPixelBlendedWithMap(uint8_t *dst, uint8_t color, const uint8_t *colorMap)
{
	*dst = paletteTransparencyLookup[*dst][colorMap[color]];
}

struct BlitBlendedWithMap {
	const uint8_t *colorMap;

	DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void operator()(BlitCommand cmd, uint8_t *dst, const uint8_t *src) const
	{
		switch (cmd.type) {
		case BlitType::Fill:
			BlitFillBlendedWithMap(dst, cmd.length, cmd.color, colorMap);
			return;
		case BlitType::Pixels:
			BlitPixelsBlendedWithMap(dst, src, cmd.length, colorMap);
			return;
		case BlitType::Pixel:
			BlitPixelBlendedWithMap(dst, cmd.color, colorMap);
			return;
		case BlitType::Transparent:
			return;
		}
	}
};

} // namespace devilution
