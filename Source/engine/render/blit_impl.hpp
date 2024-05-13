#pragma once

#include <cstdint>
#include <cstring>
#include <execution>
#include <version>

#include "engine/palette.h"
#include "utils/attributes.h"

namespace devilution {

#if __cpp_lib_execution >= 201902L
#define DEVILUTIONX_BLIT_EXECUTION_POLICY std::execution::unseq,
#else
#define DEVILUTIONX_BLIT_EXECUTION_POLICY
#endif

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
	std::transform(DEVILUTIONX_BLIT_EXECUTION_POLICY src, src + length, dst, [colorMap](uint8_t srcColor) { return colorMap[srcColor]; });
}

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void BlitFillBlended(uint8_t *dst, unsigned length, uint8_t color)
{
	assert(length != 0);
	std::for_each(DEVILUTIONX_BLIT_EXECUTION_POLICY dst, dst + length, [tbl = paletteTransparencyLookup[color]](uint8_t &dstColor) {
		dstColor = tbl[dstColor];
	});
}

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void BlitPixelsBlended(uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src, unsigned length)
{
	assert(length != 0);
	std::transform(DEVILUTIONX_BLIT_EXECUTION_POLICY src, src + length, dst, dst, [pal = paletteTransparencyLookup](uint8_t srcColor, uint8_t dstColor) {
		return pal[srcColor][dstColor];
	});
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
	std::transform(DEVILUTIONX_BLIT_EXECUTION_POLICY src, src + length, dst, dst, [colorMap, pal = paletteTransparencyLookup](uint8_t srcColor, uint8_t dstColor) {
		return pal[dstColor][colorMap[srcColor]];
	});
}

struct BlitBlendedWithMap {
	const uint8_t *DVL_RESTRICT colorMap;

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
