/**
 * @file dun_render.cpp
 *
 * Implementation of functionality for rendering the level tiles.
 */
#include "engine/render/dun_render.hpp"

#include <algorithm>
#include <climits>
#include <cstdint>

#include "lighting.h"
#include "options.h"
#include "utils/attributes.h"

namespace devilution {

namespace {

/**
 * Tile type.
 *
 * The tile type determines data encoding and the shape.
 *
 * Each tile type has its own encoding but they all encode data in the order
 * of bottom-to-top (bottom row first).
 */
enum class TileType {
	/**
	 * 🮆 A 32x32 square. Stored as an array of pixels.
	 */
	Square,

	/**
	 * 🮆 A 32x32 square with transparency. RLE encoded.
	 *
	 * Each run starts with an int8_t value.
	 * If positive, it is followed by this many pixels.
	 * If negative, it indicates `-value` fully transparent pixels, which are omitted.
	 *
	 * Runs do not cross row boundaries.
	 */
	TransparentSquare,

	/**
	 *🭮 Left-pointing 32x31 triangle. Encoded as 31 varying-width rows with 2 padding bytes before every even row.
	 *
	 * The smallest rows (bottom and top) are 2px wide, the largest row is 16px wide (middle row).
	 *
	 * Encoding:
	 * for i in [0, 30]:
	 * - 2 unused bytes if i is even
	 * - row (only the pixels within the triangle)
	 */
	LeftTriangle,

	/**
	 * 🭬Right-pointing 32x31 triangle.  Encoded as 31 varying-width rows with 2 padding bytes after every even row.
	 *
	 * The smallest rows (bottom and top) are 2px wide, the largest row is 16px wide (middle row).
	 *
	 * Encoding:
	 * for i in [0, 30]:
	 * - row (only the pixels within the triangle)
	 * - 2 unused bytes if i is even
	 */
	RightTriangle,

	/**
	 * 🭓 Left-pointing 32x32 trapezoid: a 32x16 rectangle and the 16x16 bottom part of `LeftTriangle`.
	 *
	 * Begins with triangle part, which uses the `LeftTriangle` encoding,
	 * and is followed by a flat array of pixels for the top rectangle part.
	 */
	LeftTrapezoid,

	/**
	 * 🭞 Right-pointing 32x32 trapezoid: 32x16 rectangle and the 16x16 bottom part of `RightTriangle`.
	 *
	 * Begins with the triangle part, which uses the `RightTriangle` encoding,
	 * and is followed by a flat array of pixels for the top rectangle part.
	 */
	RightTrapezoid,
};

/** Width of a tile rendering primitive. */
constexpr std::int_fast16_t Width = TILE_WIDTH / 2;

/** Height of a tile rendering primitive (except triangles). */
constexpr std::int_fast16_t Height = TILE_HEIGHT;

/** Height of the lower triangle of a triangular or a trapezoid tile. */
constexpr std::int_fast16_t LowerHeight = TILE_HEIGHT / 2;

/** Height of the upper triangle of a triangular tile. */
constexpr std::int_fast16_t TriangleUpperHeight = TILE_HEIGHT / 2 - 1;

/** Height of the upper rectangle of a trapezoid tile. */
constexpr std::int_fast16_t TrapezoidUpperHeight = TILE_HEIGHT / 2;

constexpr std::int_fast16_t TriangleHeight = LowerHeight + TriangleUpperHeight;

/** For triangles, for each pixel drawn vertically, this many pixels are drawn horizontally. */
constexpr std::int_fast16_t XStep = 2;

std::int_fast16_t GetTileHeight(TileType tile)
{
	if (tile == TileType::LeftTriangle || tile == TileType::RightTriangle)
		return TriangleHeight;
	return Height;
}

// Debugging variables
// #define DEBUG_RENDER_COLOR
// #define DEBUG_RENDER_OFFSET_X 5
// #define DEBUG_RENDER_OFFSET_Y 5

#ifdef DEBUG_RENDER_COLOR
int DBGCOLOR = 0;

int GetTileDebugColor(TileType tile)
{
	// clang-format off
	switch (tile) {
		case TileType::Square: return PAL16_YELLOW + 5;
		case TileType::TransparentSquare: return PAL16_ORANGE + 5;
		case TileType::LeftTriangle: return PAL16_GRAY + 5;
		case TileType::RightTriangle: return PAL16_BEIGE;
		case TileType::LeftTrapezoid: return PAL16_RED + 5;
		case TileType::RightTrapezoid: return PAL16_BLUE + 5;
		default: return 0;
	}
	// clang-format on
}
#endif // DEBUG_RENDER_COLOR

/** Fully transparent variant of WallMask. */
const std::uint32_t WallMaskFullyTrasparent[TILE_HEIGHT] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000
};
/** Transparent variant of RightMask. */
const std::uint32_t RightMaskTransparent[TILE_HEIGHT] = {
	0xC0000000,
	0xF0000000,
	0xFC000000,
	0xFF000000,
	0xFFC00000,
	0xFFF00000,
	0xFFFC0000,
	0xFFFF0000,
	0xFFFFC000,
	0xFFFFF000,
	0xFFFFFC00,
	0xFFFFFF00,
	0xFFFFFFC0,
	0xFFFFFFF0,
	0xFFFFFFFC,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF
};
/** Transparent variant of LeftMask. */
const std::uint32_t LeftMaskTransparent[TILE_HEIGHT] = {
	0x00000003,
	0x0000000F,
	0x0000003F,
	0x000000FF,
	0x000003FF,
	0x00000FFF,
	0x00003FFF,
	0x0000FFFF,
	0x0003FFFF,
	0x000FFFFF,
	0x003FFFFF,
	0x00FFFFFF,
	0x03FFFFFF,
	0x0FFFFFFF,
	0x3FFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF
};
/** Specifies the draw masks used to render transparency of the right side of tiles. */
const std::uint32_t RightMask[TILE_HEIGHT] = {
	0xEAAAAAAA,
	0xF5555555,
	0xFEAAAAAA,
	0xFF555555,
	0xFFEAAAAA,
	0xFFF55555,
	0xFFFEAAAA,
	0xFFFF5555,
	0xFFFFEAAA,
	0xFFFFF555,
	0xFFFFFEAA,
	0xFFFFFF55,
	0xFFFFFFEA,
	0xFFFFFFF5,
	0xFFFFFFFE,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF
};
/** Specifies the draw masks used to render transparency of the left side of tiles. */
const std::uint32_t LeftMask[TILE_HEIGHT] = {
	0xAAAAAAAB,
	0x5555555F,
	0xAAAAAABF,
	0x555555FF,
	0xAAAAABFF,
	0x55555FFF,
	0xAAAABFFF,
	0x5555FFFF,
	0xAAABFFFF,
	0x555FFFFF,
	0xAABFFFFF,
	0x55FFFFFF,
	0xABFFFFFF,
	0x5FFFFFFF,
	0xBFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF
};
/** Specifies the draw masks used to render transparency of wall tiles. */
const std::uint32_t WallMask[TILE_HEIGHT] = {
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555
};
/** Fully opaque mask */
const std::uint32_t SolidMask[TILE_HEIGHT] = {
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF
};
/** Used to mask out the left half of the tile diamond and only render additional content */
const std::uint32_t RightFoliageMask[TILE_HEIGHT] = {
	0xFFFFFFFF,
	0x3FFFFFFF,
	0x0FFFFFFF,
	0x03FFFFFF,
	0x00FFFFFF,
	0x003FFFFF,
	0x000FFFFF,
	0x0003FFFF,
	0x0000FFFF,
	0x00003FFF,
	0x00000FFF,
	0x000003FF,
	0x000000FF,
	0x0000003F,
	0x0000000F,
	0x00000003,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
/** Used to mask out the left half of the tile diamond and only render additional content */
const std::uint32_t LeftFoliageMask[TILE_HEIGHT] = {
	0xFFFFFFFF,
	0xFFFFFFFC,
	0xFFFFFFF0,
	0xFFFFFFC0,
	0xFFFFFF00,
	0xFFFFFC00,
	0xFFFFF000,
	0xFFFFC000,
	0xFFFF0000,
	0xFFFC0000,
	0xFFF00000,
	0xFFC00000,
	0xFF000000,
	0xFC000000,
	0xF0000000,
	0xC0000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};

inline int CountLeadingZeros(std::uint32_t mask)
{
	// Note: This function assumes that the argument is not zero,
	// which means there is at least one bit set.
	static_assert(
	    sizeof(std::uint32_t) == sizeof(uint32_t),
	    "CountLeadingZeros: std::uint32_t must be 32bits");
#if defined(__GNUC__) || defined(__clang__)
	return __builtin_clz(mask);
#else
	// Count the number of leading zeros using binary search.
	int n = 0;
	if ((mask & 0xFFFF0000) == 0)
		n += 16, mask <<= 16;
	if ((mask & 0xFF000000) == 0)
		n += 8, mask <<= 8;
	if ((mask & 0xF0000000) == 0)
		n += 4, mask <<= 4;
	if ((mask & 0xC0000000) == 0)
		n += 2, mask <<= 2;
	if ((mask & 0x80000000) == 0)
		n += 1;
	return n;
#endif
}

template <typename F>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void ForEachSetBit(std::uint32_t mask, const F &f)
{
	int i = 0;
	while (mask != 0) {
		int z = CountLeadingZeros(mask);
		i += z, mask <<= z;
		for (; mask & 0x80000000; i++, mask <<= 1)
			f(i);
	}
}

enum class TransparencyType {
	Solid,
	Blended,
	Stippled,
};

enum class LightType {
	FullyDark,
	PartiallyLit,
	FullyLit,
};

template <LightType Light>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderLineOpaque(std::uint8_t *dst, const std::uint8_t *src, std::uint_fast8_t n, const std::uint8_t *tbl)
{
	if (Light == LightType::FullyDark) {
		memset(dst, 0, n);
	} else if (Light == LightType::FullyLit) {
#ifndef DEBUG_RENDER_COLOR
		memcpy(dst, src, n);
#else
		memset(dst, DBGCOLOR, n);
#endif
	} else { // Partially lit
#ifndef DEBUG_RENDER_COLOR
		for (size_t i = 0; i < n; i++) {
			dst[i] = tbl[src[i]];
		}
#else
		memset(dst, tbl[DBGCOLOR], n);
#endif
	}
}

template <LightType Light>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderLineBlended(std::uint8_t *dst, const std::uint8_t *src, std::uint_fast8_t n, const std::uint8_t *tbl, std::uint32_t mask)
{
#ifndef DEBUG_RENDER_COLOR
	if (Light == LightType::FullyDark) {
		for (size_t i = 0; i < n; i++, mask <<= 1) {
			if ((mask & 0x80000000) != 0)
				dst[i] = 0;
			else
				dst[i] = paletteTransparencyLookup[0][dst[i]];
		}
	} else if (Light == LightType::FullyLit) {
		for (size_t i = 0; i < n; i++, mask <<= 1) {
			if ((mask & 0x80000000) != 0)
				dst[i] = src[i];
			else
				dst[i] = paletteTransparencyLookup[dst[i]][src[i]];
		}
	} else { // Partially lit
		for (size_t i = 0; i < n; i++, mask <<= 1) {
			if ((mask & 0x80000000) != 0)
				dst[i] = tbl[src[i]];
			else
				dst[i] = paletteTransparencyLookup[dst[i]][tbl[src[i]]];
		}
	}
#else
	for (size_t i = 0; i < n; i++, mask <<= 1) {
		if ((mask & 0x80000000) != 0)
			dst[i] = tbl[DBGCOLOR];
		else
			dst[i] = paletteTransparencyLookup[dst[i]][tbl[DBGCOLOR]];
	}
#endif
}

template <LightType Light>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderLineStippled(std::uint8_t *dst, const std::uint8_t *src, const std::uint8_t *tbl, std::uint32_t mask)
{
	if (Light == LightType::FullyDark) {
		ForEachSetBit(mask, [=](int i) { dst[i] = 0; });
	} else if (Light == LightType::FullyLit) {
#ifndef DEBUG_RENDER_COLOR
		ForEachSetBit(mask, [=](int i) { dst[i] = src[i]; });
#else
		ForEachSetBit(mask, [=](int i) { dst[i] = DBGCOLOR; });
#endif
	} else { // Partially lit
		ForEachSetBit(mask, [=](int i) { dst[i] = tbl[src[i]]; });
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderLine(std::uint8_t *dst, const std::uint8_t *src, std::uint_fast8_t n, const std::uint8_t *tbl, std::uint32_t mask)
{
	if (Transparency == TransparencyType::Solid) {
		RenderLineOpaque<Light>(dst, src, n, tbl);
	} else {
		// The number of iterations is limited by the size of the mask.
		// So we can limit it by ANDing the mask with another mask that only keeps
		// iterations that are lower than n. We can now avoid testing if i < n
		// at every loop iteration.
		assert(n != 0 && n <= sizeof(std::uint32_t) * CHAR_BIT);
		const std::uint32_t firstNOnes = std::uint32_t(-1) << ((sizeof(std::uint32_t) * CHAR_BIT) - n);
		mask &= firstNOnes;
		if (mask == firstNOnes) {
			RenderLineOpaque<Light>(dst, src, n, tbl);
		} else if (Transparency == TransparencyType::Blended) {
			RenderLineBlended<Light>(dst, src, n, tbl, mask);
		} else {
			RenderLineStippled<Light>(dst, src, tbl, mask);
		}
	}
}

struct Clip {
	std::int_fast16_t top;
	std::int_fast16_t bottom;
	std::int_fast16_t left;
	std::int_fast16_t right;
	std::int_fast16_t width;
	std::int_fast16_t height;
};

Clip CalculateClip(std::int_fast16_t x, std::int_fast16_t y, std::int_fast16_t w, std::int_fast16_t h, const Surface &out)
{
	Clip clip;
	clip.top = y + 1 < h ? h - (y + 1) : 0;
	clip.bottom = y + 1 > out.h() ? (y + 1) - out.h() : 0;
	clip.left = x < 0 ? -x : 0;
	clip.right = x + w > out.w() ? x + w - out.w() : 0;
	clip.width = w - clip.left - clip.right;
	clip.height = h - clip.top - clip.bottom;
	return clip;
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderSquareFull(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl)
{
	for (auto i = 0; i < Height; ++i, dst -= dstPitch, --mask) {
		RenderLine<Transparency, Light>(dst, src, Width, tbl, *mask);
		src += Width;
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderSquareClipped(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	src += clip.bottom * Height + clip.left;
	for (auto i = 0; i < clip.height; ++i, dst -= dstPitch, --mask) {
		RenderLine<Transparency, Light>(dst, src, clip.width, tbl, (*mask) << clip.left);
		src += Width;
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderSquare(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	if (clip.width == Width && clip.height == Height) {
		RenderSquareFull<Transparency, Light>(dst, dstPitch, src, mask, tbl);
	} else {
		RenderSquareClipped<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderTransparentSquareFull(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl)
{
	for (auto i = 0; i < Height; ++i, dst -= dstPitch + Width, --mask) {
		constexpr unsigned MaxMaskShift = 32;
		std::uint_fast8_t drawWidth = Width;
		std::uint32_t m = *mask;
		while (drawWidth > 0) {
			auto v = static_cast<std::int8_t>(*src++);
			if (v > 0) {
				RenderLine<Transparency, Light>(dst, src, v, tbl, m);
				src += v;
			} else {
				v = -v;
			}
			dst += v;
			drawWidth -= v;
			m = (v == MaxMaskShift) ? 0 : (m << v);
		}
	}
}

template <TransparencyType Transparency, LightType Light>
// NOLINTNEXTLINE(readability-function-cognitive-complexity): Actually complex and has to be fast.
DVL_ATTRIBUTE_HOT void RenderTransparentSquareClipped(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	const auto skipRestOfTheLine = [&src](std::int_fast16_t remainingWidth) {
		while (remainingWidth > 0) {
			const auto v = static_cast<std::int8_t>(*src++);
			if (v > 0) {
				src += v;
				remainingWidth -= v;
			} else {
				remainingWidth -= -v;
			}
		}
		assert(remainingWidth == 0);
	};

	// Skip the bottom clipped lines.
	for (auto i = 0; i < clip.bottom; ++i) {
		skipRestOfTheLine(Width);
	}

	for (auto i = 0; i < clip.height; ++i, dst -= dstPitch + clip.width, --mask) {
		constexpr unsigned MaxMaskShift = 32;
		auto drawWidth = clip.width;
		std::uint32_t m = *mask;

		// Skip initial src if clipping on the left.
		// Handles overshoot, i.e. when the RLE segment goes into the unclipped area.
		auto remainingLeftClip = clip.left;
		while (remainingLeftClip > 0) {
			auto v = static_cast<std::int8_t>(*src++);
			if (v > 0) {
				if (v > remainingLeftClip) {
					const auto overshoot = v - remainingLeftClip;
					RenderLine<Transparency, Light>(dst, src + remainingLeftClip, overshoot, tbl, m);
					dst += overshoot;
					drawWidth -= overshoot;
				}
				src += v;
			} else {
				v = -v;
				if (v > remainingLeftClip) {
					const auto overshoot = v - remainingLeftClip;
					dst += overshoot;
					drawWidth -= overshoot;
				}
			}
			remainingLeftClip -= v;
			m = (v == MaxMaskShift) ? 0 : (m << v);
		}

		// Draw the non-clipped segment
		while (drawWidth > 0) {
			auto v = static_cast<std::int8_t>(*src++);
			if (v > 0) {
				if (v > drawWidth) {
					RenderLine<Transparency, Light>(dst, src, drawWidth, tbl, m);
					src += v;
					dst += drawWidth;
					drawWidth -= v;
					break;
				}
				RenderLine<Transparency, Light>(dst, src, v, tbl, m);
				src += v;
			} else {
				v = -v;
				if (v > drawWidth) {
					dst += drawWidth;
					drawWidth -= v;
					break;
				}
			}
			dst += v;
			drawWidth -= v;
			m = (v == MaxMaskShift) ? 0 : (m << v);
		}

		// Skip the rest of src line if clipping on the right
		assert(drawWidth <= 0);
		skipRestOfTheLine(clip.right + drawWidth);
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderTransparentSquare(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	if (clip.width == Width && clip.height == Height) {
		RenderTransparentSquareFull<Transparency, Light>(dst, dstPitch, src, mask, tbl);
	} else {
		RenderTransparentSquareClipped<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
	}
}

/** Vertical clip for the lower and upper triangles of a diamond tile (L/RTRIANGLE).*/
struct DiamondClipY {
	std::int_fast16_t lowerBottom;
	std::int_fast16_t lowerTop;
	std::int_fast16_t upperBottom;
	std::int_fast16_t upperTop;
};

template <std::int_fast16_t UpperHeight = TriangleUpperHeight>
DiamondClipY CalculateDiamondClipY(const Clip &clip)
{
	DiamondClipY result;
	if (clip.bottom > LowerHeight) {
		result.lowerBottom = LowerHeight;
		result.upperBottom = clip.bottom - LowerHeight;
		result.lowerTop = result.upperTop = 0;
	} else if (clip.top > UpperHeight) {
		result.upperTop = UpperHeight;
		result.lowerTop = clip.top - UpperHeight;
		result.upperBottom = result.lowerBottom = 0;
	} else {
		result.upperTop = clip.top;
		result.lowerBottom = clip.bottom;
		result.lowerTop = result.upperBottom = 0;
	}
	return result;
}

std::size_t CalculateTriangleSourceSkipLowerBottom(std::int_fast16_t numLines)
{
	return XStep * numLines * (numLines + 1) / 2 + 2 * ((numLines + 1) / 2);
}

std::size_t CalculateTriangleSourceSkipUpperBottom(std::int_fast16_t numLines)
{
	return 2 * TriangleUpperHeight * numLines - numLines * (numLines - 1) + 2 * ((numLines + 1) / 2);
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderLeftTriangleFull(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl)
{
	dst += XStep * (LowerHeight - 1);
	for (auto i = 1; i <= LowerHeight; ++i, dst -= dstPitch + XStep, --mask) {
		src += 2 * (i % 2);
		const auto width = XStep * i;
		RenderLine<Transparency, Light>(dst, src, width, tbl, *mask);
		src += width;
	}
	dst += 2 * XStep;
	for (auto i = 1; i <= TriangleUpperHeight; ++i, dst -= dstPitch - XStep, --mask) {
		src += 2 * (i % 2);
		const auto width = Width - XStep * i;
		RenderLine<Transparency, Light>(dst, src, width, tbl, *mask);
		src += width;
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderLeftTriangleClipVertical(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	const auto clipY = CalculateDiamondClipY(clip);
	src += CalculateTriangleSourceSkipLowerBottom(clipY.lowerBottom);
	dst += XStep * (LowerHeight - clipY.lowerBottom - 1);
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	for (auto i = 1 + clipY.lowerBottom; i <= lowerMax; ++i, dst -= dstPitch + XStep, --mask) {
		src += 2 * (i % 2);
		const auto width = XStep * i;
		RenderLine<Transparency, Light>(dst, src, width, tbl, *mask);
		src += width;
	}
	src += CalculateTriangleSourceSkipUpperBottom(clipY.upperBottom);
	dst += 2 * XStep + XStep * clipY.upperBottom;
	const auto upperMax = TriangleUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch - XStep, --mask) {
		src += 2 * (i % 2);
		const auto width = Width - XStep * i;
		RenderLine<Transparency, Light>(dst, src, width, tbl, *mask);
		src += width;
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderLeftTriangleClipLeftAndVertical(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	const auto clipY = CalculateDiamondClipY(clip);
	const auto clipLeft = clip.left;
	src += CalculateTriangleSourceSkipLowerBottom(clipY.lowerBottom);
	dst += XStep * (LowerHeight - clipY.lowerBottom - 1) - clipLeft;
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	for (auto i = 1 + clipY.lowerBottom; i <= lowerMax; ++i, dst -= dstPitch + XStep, --mask) {
		src += 2 * (i % 2);
		const auto width = XStep * i;
		const auto startX = Width - XStep * i;
		const auto skip = startX < clipLeft ? clipLeft - startX : 0;
		if (width > skip)
			RenderLine<Transparency, Light>(dst + skip, src + skip, width - skip, tbl, (*mask) << skip);
		src += width;
	}
	src += CalculateTriangleSourceSkipUpperBottom(clipY.upperBottom);
	dst += 2 * XStep + XStep * clipY.upperBottom;
	const auto upperMax = TriangleUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch - XStep, --mask) {
		src += 2 * (i % 2);
		const auto width = Width - XStep * i;
		const auto startX = XStep * i;
		const auto skip = startX < clipLeft ? clipLeft - startX : 0;
		if (width > skip)
			RenderLine<Transparency, Light>(dst + skip, src + skip, width - skip, tbl, (*mask) << skip);
		src += width;
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderLeftTriangleClipRightAndVertical(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	const auto clipY = CalculateDiamondClipY(clip);
	const auto clipRight = clip.right;
	src += CalculateTriangleSourceSkipLowerBottom(clipY.lowerBottom);
	dst += XStep * (LowerHeight - clipY.lowerBottom - 1);
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	for (auto i = 1 + clipY.lowerBottom; i <= lowerMax; ++i, dst -= dstPitch + XStep, --mask) {
		src += 2 * (i % 2);
		const auto width = XStep * i;
		if (width > clipRight)
			RenderLine<Transparency, Light>(dst, src, width - clipRight, tbl, *mask);
		src += width;
	}
	src += CalculateTriangleSourceSkipUpperBottom(clipY.upperBottom);
	dst += 2 * XStep + XStep * clipY.upperBottom;
	const auto upperMax = TriangleUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch - XStep, --mask) {
		src += 2 * (i % 2);
		const auto width = Width - XStep * i;
		if (width <= clipRight)
			break;
		RenderLine<Transparency, Light>(dst, src, width - clipRight, tbl, *mask);
		src += width;
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderLeftTriangle(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	if (clip.width == Width) {
		if (clip.height == TriangleHeight) {
			RenderLeftTriangleFull<Transparency, Light>(dst, dstPitch, src, mask, tbl);
		} else {
			RenderLeftTriangleClipVertical<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
		}
	} else if (clip.right == 0) {
		RenderLeftTriangleClipLeftAndVertical<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
	} else {
		RenderLeftTriangleClipRightAndVertical<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderRightTriangleFull(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl)
{
	for (auto i = 1; i <= LowerHeight; ++i, dst -= dstPitch, --mask) {
		const auto width = XStep * i;
		RenderLine<Transparency, Light>(dst, src, width, tbl, *mask);
		src += width + 2 * (i % 2);
	}
	for (auto i = 1; i <= TriangleUpperHeight; ++i, dst -= dstPitch, --mask) {
		const auto width = Width - XStep * i;
		RenderLine<Transparency, Light>(dst, src, width, tbl, *mask);
		src += width + 2 * (i % 2);
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderRightTriangleClipVertical(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	const auto clipY = CalculateDiamondClipY(clip);
	src += CalculateTriangleSourceSkipLowerBottom(clipY.lowerBottom);
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	for (auto i = 1 + clipY.lowerBottom; i <= lowerMax; ++i, dst -= dstPitch, --mask) {
		const auto width = XStep * i;
		RenderLine<Transparency, Light>(dst, src, width, tbl, *mask);
		src += width + 2 * (i % 2);
	}
	src += CalculateTriangleSourceSkipUpperBottom(clipY.upperBottom);
	const auto upperMax = TriangleUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch, --mask) {
		const auto width = Width - XStep * i;
		RenderLine<Transparency, Light>(dst, src, width, tbl, *mask);
		src += width + 2 * (i % 2);
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderRightTriangleClipLeftAndVertical(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	const auto clipY = CalculateDiamondClipY(clip);
	const auto clipLeft = clip.left;
	src += CalculateTriangleSourceSkipLowerBottom(clipY.lowerBottom);
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	for (auto i = 1 + clipY.lowerBottom; i <= lowerMax; ++i, dst -= dstPitch, --mask) {
		const auto width = XStep * i;
		if (width > clipLeft)
			RenderLine<Transparency, Light>(dst, src + clipLeft, width - clipLeft, tbl, (*mask) << clipLeft);
		src += width + 2 * (i % 2);
	}
	src += CalculateTriangleSourceSkipUpperBottom(clipY.upperBottom);
	const auto upperMax = TriangleUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch, --mask) {
		const auto width = Width - XStep * i;
		if (width <= clipLeft)
			break;
		RenderLine<Transparency, Light>(dst, src + clipLeft, width - clipLeft, tbl, (*mask) << clipLeft);
		src += width + 2 * (i % 2);
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderRightTriangleClipRightAndVertical(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	const auto clipY = CalculateDiamondClipY(clip);
	const auto clipRight = clip.right;
	src += CalculateTriangleSourceSkipLowerBottom(clipY.lowerBottom);
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	for (auto i = 1 + clipY.lowerBottom; i <= lowerMax; ++i, dst -= dstPitch, --mask) {
		const auto width = XStep * i;
		const auto skip = Width - width < clipRight ? clipRight - (Width - width) : 0;
		if (width > skip)
			RenderLine<Transparency, Light>(dst, src, width - skip, tbl, *mask);
		src += width + 2 * (i % 2);
	}
	src += CalculateTriangleSourceSkipUpperBottom(clipY.upperBottom);
	const auto upperMax = TriangleUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch, --mask) {
		const auto width = Width - XStep * i;
		const auto skip = Width - width < clipRight ? clipRight - (Width - width) : 0;
		if (width > skip)
			RenderLine<Transparency, Light>(dst, src, width - skip, tbl, *mask);
		src += width + 2 * (i % 2);
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderRightTriangle(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	if (clip.width == Width) {
		if (clip.height == TriangleHeight) {
			RenderRightTriangleFull<Transparency, Light>(dst, dstPitch, src, mask, tbl);
		} else {
			RenderRightTriangleClipVertical<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
		}
	} else if (clip.right == 0) {
		RenderRightTriangleClipLeftAndVertical<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
	} else {
		RenderRightTriangleClipRightAndVertical<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderLeftTrapezoidFull(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl)
{
	dst += XStep * (LowerHeight - 1);
	for (auto i = 1; i <= LowerHeight; ++i, dst -= dstPitch + XStep, --mask) {
		src += 2 * (i % 2);
		const auto width = XStep * i;
		RenderLine<Transparency, Light>(dst, src, width, tbl, *mask);
		src += width;
	}
	dst += XStep;
	for (auto i = 1; i <= TrapezoidUpperHeight; ++i, dst -= dstPitch, --mask) {
		RenderLine<Transparency, Light>(dst, src, Width, tbl, *mask);
		src += Width;
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderLeftTrapezoidClipVertical(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	const auto clipY = CalculateDiamondClipY<TrapezoidUpperHeight>(clip);
	src += CalculateTriangleSourceSkipLowerBottom(clipY.lowerBottom);
	dst += XStep * (LowerHeight - clipY.lowerBottom - 1);
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	for (auto i = 1 + clipY.lowerBottom; i <= lowerMax; ++i, dst -= dstPitch + XStep, --mask) {
		src += 2 * (i % 2);
		const auto width = XStep * i;
		RenderLine<Transparency, Light>(dst, src, width, tbl, *mask);
		src += width;
	}
	src += clipY.upperBottom * Width;
	dst += XStep;
	const auto upperMax = TrapezoidUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch, --mask) {
		RenderLine<Transparency, Light>(dst, src, Width, tbl, *mask);
		src += Width;
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderLeftTrapezoidClipLeftAndVertical(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	const auto clipY = CalculateDiamondClipY<TrapezoidUpperHeight>(clip);
	const auto clipLeft = clip.left;
	src += CalculateTriangleSourceSkipLowerBottom(clipY.lowerBottom);
	dst += XStep * (LowerHeight - clipY.lowerBottom - 1) - clipLeft;
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	for (auto i = 1 + clipY.lowerBottom; i <= lowerMax; ++i, dst -= dstPitch + XStep, --mask) {
		src += 2 * (i % 2);
		const auto width = XStep * i;
		const auto startX = Width - XStep * i;
		const auto skip = startX < clipLeft ? clipLeft - startX : 0;
		if (width > skip)
			RenderLine<Transparency, Light>(dst + skip, src + skip, width - skip, tbl, (*mask) << skip);
		src += width;
	}
	src += clipY.upperBottom * Width + clipLeft;
	dst += XStep + clipLeft;
	const auto upperMax = TrapezoidUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch, --mask) {
		RenderLine<Transparency, Light>(dst, src, clip.width, tbl, (*mask) << clipLeft);
		src += Width;
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderLeftTrapezoidClipRightAndVertical(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	const auto clipY = CalculateDiamondClipY<TrapezoidUpperHeight>(clip);
	const auto clipRight = clip.right;
	src += CalculateTriangleSourceSkipLowerBottom(clipY.lowerBottom);
	dst += XStep * (LowerHeight - clipY.lowerBottom - 1);
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	for (auto i = 1 + clipY.lowerBottom; i <= lowerMax; ++i, dst -= dstPitch + XStep, --mask) {
		src += 2 * (i % 2);
		const auto width = XStep * i;
		if (width > clipRight)
			RenderLine<Transparency, Light>(dst, src, width - clipRight, tbl, *mask);
		src += width;
	}
	src += clipY.upperBottom * Width;
	dst += XStep;
	const auto upperMax = TrapezoidUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch, --mask) {
		RenderLine<Transparency, Light>(dst, src, clip.width, tbl, *mask);
		src += Width;
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderLeftTrapezoid(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	if (clip.width == Width) {
		if (clip.height == Height) {
			RenderLeftTrapezoidFull<Transparency, Light>(dst, dstPitch, src, mask, tbl);
		} else {
			RenderLeftTrapezoidClipVertical<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
		}
	} else if (clip.right == 0) {
		RenderLeftTrapezoidClipLeftAndVertical<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
	} else {
		RenderLeftTrapezoidClipRightAndVertical<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderRightTrapezoidFull(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl)
{
	for (auto i = 1; i <= LowerHeight; ++i, dst -= dstPitch, --mask) {
		const auto width = XStep * i;
		RenderLine<Transparency, Light>(dst, src, width, tbl, *mask);
		src += width + 2 * (i % 2);
	}
	for (auto i = 1; i <= TrapezoidUpperHeight; ++i, dst -= dstPitch, --mask) {
		RenderLine<Transparency, Light>(dst, src, Width, tbl, *mask);
		src += Width;
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderRightTrapezoidClipVertical(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	const auto clipY = CalculateDiamondClipY<TrapezoidUpperHeight>(clip);
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	src += CalculateTriangleSourceSkipLowerBottom(clipY.lowerBottom);
	for (auto i = 1 + clipY.lowerBottom; i <= lowerMax; ++i, dst -= dstPitch, --mask) {
		const auto width = XStep * i;
		RenderLine<Transparency, Light>(dst, src, width, tbl, *mask);
		src += width + 2 * (i % 2);
	}
	src += clipY.upperBottom * Width;
	const auto upperMax = TrapezoidUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch, --mask) {
		RenderLine<Transparency, Light>(dst, src, Width, tbl, *mask);
		src += Width;
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderRightTrapezoidClipLeftAndVertical(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	const auto clipY = CalculateDiamondClipY<TrapezoidUpperHeight>(clip);
	const auto clipLeft = clip.left;
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	src += CalculateTriangleSourceSkipLowerBottom(clipY.lowerBottom);
	for (auto i = 1 + clipY.lowerBottom; i <= lowerMax; ++i, dst -= dstPitch, --mask) {
		const auto width = XStep * i;
		if (width > clipLeft)
			RenderLine<Transparency, Light>(dst, src + clipLeft, width - clipLeft, tbl, (*mask) << clipLeft);
		src += width + 2 * (i % 2);
	}
	src += clipY.upperBottom * Width + clipLeft;
	const auto upperMax = TrapezoidUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch, --mask) {
		RenderLine<Transparency, Light>(dst, src, clip.width, tbl, (*mask) << clipLeft);
		src += Width;
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderRightTrapezoidClipRightAndVertical(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	const auto clipY = CalculateDiamondClipY<TrapezoidUpperHeight>(clip);
	const auto clipRight = clip.right;
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	src += CalculateTriangleSourceSkipLowerBottom(clipY.lowerBottom);
	for (auto i = 1 + clipY.lowerBottom; i <= lowerMax; ++i, dst -= dstPitch, --mask) {
		const auto width = XStep * i;
		const auto skip = Width - width < clipRight ? clipRight - (Width - width) : 0;
		if (width > skip)
			RenderLine<Transparency, Light>(dst, src, width - skip, tbl, *mask);
		src += width + 2 * (i % 2);
	}
	src += clipY.upperBottom * Width;
	const auto upperMax = TrapezoidUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch, --mask) {
		RenderLine<Transparency, Light>(dst, src, clip.width, tbl, *mask);
		src += Width;
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderRightTrapezoid(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	if (clip.width == Width) {
		if (clip.height == Height) {
			RenderRightTrapezoidFull<Transparency, Light>(dst, dstPitch, src, mask, tbl);
		} else {
			RenderRightTrapezoidClipVertical<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
		}
	} else if (clip.right == 0) {
		RenderRightTrapezoidClipLeftAndVertical<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
	} else {
		RenderRightTrapezoidClipRightAndVertical<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
	}
}

template <TransparencyType Transparency, LightType Light>
DVL_ATTRIBUTE_HOT void RenderTileType(TileType tile, std::uint8_t *dst, int dstPitch, const std::uint8_t *src, const std::uint32_t *mask, const std::uint8_t *tbl, Clip clip)
{
	switch (tile) {
	case TileType::Square:
		RenderSquare<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
		break;
	case TileType::TransparentSquare:
		RenderTransparentSquare<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
		break;
	case TileType::LeftTriangle:
		RenderLeftTriangle<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
		break;
	case TileType::RightTriangle:
		RenderRightTriangle<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
		break;
	case TileType::LeftTrapezoid:
		RenderLeftTrapezoid<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
		break;
	case TileType::RightTrapezoid:
		RenderRightTrapezoid<Transparency, Light>(dst, dstPitch, src, mask, tbl, clip);
		break;
	}
}

/** Returns the mask that defines what parts of the tile are opaque. */
const std::uint32_t *GetMask(TileType tile)
{
#ifdef _DEBUG
	if (GetAsyncKeyState(DVL_VK_MENU)) {
		return &SolidMask[TILE_HEIGHT - 1];
	}
#endif

	if (cel_transparency_active) {
		if (arch_draw_type == 0) {
			if (*sgOptions.Graphics.blendedTransparancy) // Use a fully transparent mask
				return &WallMaskFullyTrasparent[TILE_HEIGHT - 1];
			return &WallMask[TILE_HEIGHT - 1];
		}
		if (arch_draw_type == 1 && tile != TileType::LeftTriangle) {
			const auto c = block_lvid[level_piece_id];
			if (c == 1 || c == 3) {
				if (*sgOptions.Graphics.blendedTransparancy) // Use a fully transparent mask
					return &LeftMaskTransparent[TILE_HEIGHT - 1];
				return &LeftMask[TILE_HEIGHT - 1];
			}
		}
		if (arch_draw_type == 2 && tile != TileType::RightTriangle) {
			const auto c = block_lvid[level_piece_id];
			if (c == 2 || c == 3) {
				if (*sgOptions.Graphics.blendedTransparancy) // Use a fully transparent mask
					return &RightMaskTransparent[TILE_HEIGHT - 1];
				return &RightMask[TILE_HEIGHT - 1];
			}
		}
	} else if (arch_draw_type != 0 && cel_foliage_active) {
		if (tile != TileType::TransparentSquare)
			return nullptr;
		if (arch_draw_type == 1)
			return &LeftFoliageMask[TILE_HEIGHT - 1];
		if (arch_draw_type == 2)
			return &RightFoliageMask[TILE_HEIGHT - 1];
	}
	return &SolidMask[TILE_HEIGHT - 1];
}

// Blit with left and vertical clipping.
void RenderBlackTileClipLeftAndVertical(std::uint8_t *dst, int dstPitch, int sx, DiamondClipY clipY)
{
	dst += XStep * (LowerHeight - clipY.lowerBottom - 1);
	// Lower triangle (drawn bottom to top):
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	for (auto i = clipY.lowerBottom + 1; i <= lowerMax; ++i, dst -= dstPitch + XStep) {
		const auto w = 2 * XStep * i;
		const auto curX = sx + TILE_WIDTH / 2 - XStep * i;
		if (curX >= 0) {
			memset(dst, 0, w);
		} else if (-curX <= w) {
			memset(dst - curX, 0, w + curX);
		}
	}
	dst += 2 * XStep + XStep * clipY.upperBottom;
	// Upper triangle (drawn bottom to top):
	const auto upperMax = TriangleUpperHeight - clipY.upperTop;
	for (auto i = clipY.upperBottom; i < upperMax; ++i, dst -= dstPitch - XStep) {
		const auto w = 2 * XStep * (TriangleUpperHeight - i);
		const auto curX = sx + TILE_WIDTH / 2 - XStep * (TriangleUpperHeight - i);
		if (curX >= 0) {
			memset(dst, 0, w);
		} else if (-curX <= w) {
			memset(dst - curX, 0, w + curX);
		} else {
			break;
		}
	}
}

// Blit with right and vertical clipping.
void RenderBlackTileClipRightAndVertical(std::uint8_t *dst, int dstPitch, std::int_fast16_t maxWidth, DiamondClipY clipY)
{
	dst += XStep * (LowerHeight - clipY.lowerBottom - 1);
	// Lower triangle (drawn bottom to top):
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	for (auto i = clipY.lowerBottom + 1; i <= lowerMax; ++i, dst -= dstPitch + XStep) {
		const auto width = 2 * XStep * i;
		const auto endX = TILE_WIDTH / 2 + XStep * i;
		const auto skip = endX > maxWidth ? endX - maxWidth : 0;
		if (width > skip)
			memset(dst, 0, width - skip);
	}
	dst += 2 * XStep + XStep * clipY.upperBottom;
	// Upper triangle (drawn bottom to top):
	const auto upperMax = TriangleUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch - XStep) {
		const auto width = TILE_WIDTH - 2 * XStep * i;
		const auto endX = TILE_WIDTH / 2 + XStep * (TriangleUpperHeight - i + 1);
		const auto skip = endX > maxWidth ? endX - maxWidth : 0;
		if (width <= skip)
			break;
		memset(dst, 0, width - skip);
	}
}

// Blit with vertical clipping only.
void RenderBlackTileClipY(std::uint8_t *dst, int dstPitch, DiamondClipY clipY)
{
	dst += XStep * (LowerHeight - clipY.lowerBottom - 1);
	// Lower triangle (drawn bottom to top):
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	for (auto i = 1 + clipY.lowerBottom; i <= lowerMax; ++i, dst -= dstPitch + XStep) {
		memset(dst, 0, 2 * XStep * i);
	}
	dst += 2 * XStep + XStep * clipY.upperBottom;
	// Upper triangle (drawn bottom to top):
	const auto upperMax = TriangleUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch - XStep) {
		memset(dst, 0, TILE_WIDTH - 2 * XStep * i);
	}
}

// Blit a black tile without clipping (must be fully in bounds).
void RenderBlackTileFull(std::uint8_t *dst, int dstPitch)
{
	dst += XStep * (LowerHeight - 1);
	// Tile is fully in bounds, can use constant loop boundaries.
	// Lower triangle (drawn bottom to top):
	for (unsigned i = 1; i <= LowerHeight; ++i, dst -= dstPitch + XStep) {
		memset(dst, 0, 2 * XStep * i);
	}
	dst += 2 * XStep;
	// Upper triangle (drawn bottom to to top):
	for (unsigned i = 1; i <= TriangleUpperHeight; ++i, dst -= dstPitch - XStep) {
		memset(dst, 0, TILE_WIDTH - 2 * XStep * i);
	}
}

} // namespace

void RenderTile(const Surface &out, Point position)
{
	const auto tile = static_cast<TileType>((level_cel_block & 0x7000) >> 12);
	const auto *mask = GetMask(tile);
	if (mask == nullptr)
		return;

#ifdef DEBUG_RENDER_OFFSET_X
	position.x += DEBUG_RENDER_OFFSET_X;
#endif
#ifdef DEBUG_RENDER_OFFSET_Y
	position.y += DEBUG_RENDER_OFFSET_Y;
#endif
#ifdef DEBUG_RENDER_COLOR
	DBGCOLOR = GetTileDebugColor(tile);
#endif

	Clip clip = CalculateClip(position.x, position.y, Width, GetTileHeight(tile), out);
	if (clip.width <= 0 || clip.height <= 0)
		return;

	const std::uint8_t *tbl = &LightTables[256 * LightTableIndex];
	const auto *pFrameTable = reinterpret_cast<const std::uint32_t *>(pDungeonCels.get());
	const auto *src = reinterpret_cast<const std::uint8_t *>(&pDungeonCels[SDL_SwapLE32(pFrameTable[level_cel_block & 0xFFF])]);
	std::uint8_t *dst = out.at(static_cast<int>(position.x + clip.left), static_cast<int>(position.y - clip.bottom));
	const auto dstPitch = out.pitch();

	if (mask == &SolidMask[TILE_HEIGHT - 1]) {
		if (LightTableIndex == LightsMax) {
			RenderTileType<TransparencyType::Solid, LightType::FullyDark>(tile, dst, dstPitch, src, mask, tbl, clip);
		} else if (LightTableIndex == 0) {
			RenderTileType<TransparencyType::Solid, LightType::FullyLit>(tile, dst, dstPitch, src, mask, tbl, clip);
		} else {
			RenderTileType<TransparencyType::Solid, LightType::PartiallyLit>(tile, dst, dstPitch, src, mask, tbl, clip);
		}
	} else {
		mask -= clip.bottom;
		if (*sgOptions.Graphics.blendedTransparancy) {
			if (LightTableIndex == LightsMax) {
				RenderTileType<TransparencyType::Blended, LightType::FullyDark>(tile, dst, dstPitch, src, mask, tbl, clip);
			} else if (LightTableIndex == 0) {
				RenderTileType<TransparencyType::Blended, LightType::FullyLit>(tile, dst, dstPitch, src, mask, tbl, clip);
			} else {
				RenderTileType<TransparencyType::Blended, LightType::PartiallyLit>(tile, dst, dstPitch, src, mask, tbl, clip);
			}
		} else {
			if (LightTableIndex == LightsMax) {
				RenderTileType<TransparencyType::Stippled, LightType::FullyDark>(tile, dst, dstPitch, src, mask, tbl, clip);
			} else if (LightTableIndex == 0) {
				RenderTileType<TransparencyType::Stippled, LightType::FullyLit>(tile, dst, dstPitch, src, mask, tbl, clip);
			} else {
				RenderTileType<TransparencyType::Stippled, LightType::PartiallyLit>(tile, dst, dstPitch, src, mask, tbl, clip);
			}
		}
	}
}

void world_draw_black_tile(const Surface &out, int sx, int sy)
{
#ifdef DEBUG_RENDER_OFFSET_X
	sx += DEBUG_RENDER_OFFSET_X;
#endif
#ifdef DEBUG_RENDER_OFFSET_Y
	sy += DEBUG_RENDER_OFFSET_Y;
#endif
	auto clip = CalculateClip(sx, sy, TILE_WIDTH, TriangleHeight, out);
	if (clip.width <= 0 || clip.height <= 0)
		return;

	auto clipY = CalculateDiamondClipY(clip);
	std::uint8_t *dst = out.at(sx, static_cast<int>(sy - clip.bottom));
	if (clip.width == TILE_WIDTH) {
		if (clip.height == TriangleHeight) {
			RenderBlackTileFull(dst, out.pitch());
		} else {
			RenderBlackTileClipY(dst, out.pitch(), clipY);
		}
	} else {
		if (clip.right == 0) {
			RenderBlackTileClipLeftAndVertical(dst, out.pitch(), sx, clipY);
		} else {
			RenderBlackTileClipRightAndVertical(dst, out.pitch(), clip.width, clipY);
		}
	}
}

} // namespace devilution
