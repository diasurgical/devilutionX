/**
 * @file dun_render.cpp
 *
 * Implementation of functionality for rendering the level tiles.
 */

// Debugging variables
// #define DEBUG_STR
// #define DEBUG_RENDER_COLOR
// #define DEBUG_RENDER_OFFSET_X 5
// #define DEBUG_RENDER_OFFSET_Y 5

#include "engine/render/dun_render.hpp"

#include <SDL_endian.h>

#include <algorithm>
#include <climits>
#include <cstdint>

#include "engine/render/blit_impl.hpp"
#include "lighting.h"
#include "options.h"
#include "utils/attributes.h"
#ifdef DEBUG_STR
#include "engine/render/text_render.hpp"
#endif
#if defined(DEBUG_STR) || defined(DUN_RENDER_STATS)
#include "utils/str_cat.hpp"
#endif

#ifdef DEVILUTIONX_DUN_RENDER_SMALL_CODE_SIZE
#define DEFINE_LIGHT_TRANSPARENT_FN(NAME, ...) DVL_ATTRIBUTE_HOT void NAME(LightType Light, bool Transparent, __VA_ARGS__)
#define CALL_LIGHT_TRANSPARENT_FN(NAME, ...) NAME(Light, Transparent, __VA_ARGS__)
#define DEFINE_LIGHT_PREFIX_FN(NAME, ...) DVL_ATTRIBUTE_HOT void NAME(LightType Light, bool OpaquePrefix, int8_t PrefixIncrement, __VA_ARGS__)
#define CALL_LIGHT_PREFIX_FN(NAME, ...) NAME(Light, OpaquePrefix, PrefixIncrement, __VA_ARGS__)
#define RENDER_LINE_TRANSPARENT(...) RenderLineTransparentDispatch(Light, __VA_ARGS__)
#define RENDER_LINE_OPAQUE(...) RenderLineOpaqueDispatch(Light, __VA_ARGS__)
#else
#define DEFINE_LIGHT_TRANSPARENT_FN(NAME, ...) template <LightType Light, bool Transparent> \
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void NAME(__VA_ARGS__)
#define CALL_LIGHT_TRANSPARENT_FN(NAME, ...) NAME<Light, Transparent>(__VA_ARGS__)
#define DEFINE_LIGHT_PREFIX_FN(NAME, ...) template <LightType Light, bool OpaquePrefix, int8_t PrefixIncrement> \
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void NAME(__VA_ARGS__)
#define CALL_LIGHT_PREFIX_FN(NAME, ...) NAME<Light, OpaquePrefix, PrefixIncrement>(__VA_ARGS__)
#define RENDER_LINE_TRANSPARENT(...) RenderLineTransparent<Light>(__VA_ARGS__)
#define RENDER_LINE_OPAQUE(...) RenderLineOpaque<Light>(__VA_ARGS__)
#endif

namespace devilution {

namespace {

/** Width of a tile rendering primitive. */
constexpr int_fast16_t Width = TILE_WIDTH / 2;

/** Height of a tile rendering primitive (except triangles). */
constexpr int_fast16_t Height = TILE_HEIGHT;

/** Height of the lower triangle of a triangular or a trapezoid tile. */
constexpr int_fast16_t LowerHeight = TILE_HEIGHT / 2;

/** Height of the upper triangle of a triangular tile. */
constexpr int_fast16_t TriangleUpperHeight = TILE_HEIGHT / 2 - 1;

/** Height of the upper rectangle of a trapezoid tile. */
constexpr int_fast16_t TrapezoidUpperHeight = TILE_HEIGHT / 2;

constexpr int_fast16_t TriangleHeight = LowerHeight + TriangleUpperHeight;

/** For triangles, for each pixel drawn vertically, this many pixels are drawn horizontally. */
constexpr int_fast16_t XStep = 2;

int_fast16_t GetTileHeight(TileType tile)
{
	if (tile == TileType::LeftTriangle || tile == TileType::RightTriangle)
		return TriangleHeight;
	return Height;
}

#ifdef DEBUG_STR
std::pair<std::string_view, UiFlags> GetTileDebugStr(TileType tile)
{
	// clang-format off
	switch (tile) {
		case TileType::Square: return {"S", UiFlags::AlignCenter | UiFlags::VerticalCenter};
		case TileType::TransparentSquare: return {"T", UiFlags::AlignCenter | UiFlags::VerticalCenter};
		case TileType::LeftTriangle: return {"<", UiFlags::AlignRight | UiFlags::VerticalCenter};
		case TileType::RightTriangle: return {">", UiFlags::VerticalCenter};
		case TileType::LeftTrapezoid: return {"\\", UiFlags::AlignCenter};
		case TileType::RightTrapezoid: return {"/", UiFlags::AlignCenter};
		default: return {"", {}};
	}
	// clang-format on
}
#endif

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
}
#endif // DEBUG_RENDER_COLOR

// Masks are defined by 2 template variables:
//
// 1. `OpaquePrefix`: Whether the line starts with opaque pixels
//    followed by blended pixels or the other way around.
// 2. `PrefixIncrement`: The change to the prefix when going
//    up 1 line.
//
// The Left mask can only be applied to LeftTrapezoid and TransparentSquare.
// The Right mask can only be applied to RightTrapezoid and TransparentSquare.
// The Left/RightFoliage masks can only be applied to TransparentSquare.

// True if the given OpaquePrefix and PrefixIncrement represent foliage.
// For foliage, we skip transparent pixels instead of blending them.
constexpr bool IsFoliage(bool OpaquePrefix, int8_t PrefixIncrement) {
	return PrefixIncrement != 0 && (OpaquePrefix == (PrefixIncrement > 0));
}

// True for foliage:
constexpr bool SkipTransparentPixels(bool OpaquePrefix, int8_t PrefixIncrement) {
	return IsFoliage(OpaquePrefix, PrefixIncrement);
}

// True if the entire lower half of the mask is transparent.
// True for Transparent, LeftFoliage, and RightFoliage.
constexpr bool LowerHalfTransparent(bool OpaquePrefix, int8_t PrefixIncrement){
	return (OpaquePrefix == (PrefixIncrement >= 0));
}

// The initial value for the prefix:
constexpr int8_t InitPrefix(int8_t PrefixIncrement)
{
	return PrefixIncrement >= 0 ? -32 : 64;
}

// The initial value for the prefix at y-th line (counting from the bottom).
constexpr int8_t InitPrefix(int8_t PrefixIncrement, int8_t y)
{
	return InitPrefix(PrefixIncrement) + PrefixIncrement * y;
}

#ifdef DEBUG_STR
template <bool OpaquePrefix, int8_t PrefixIncrement>
std::string prefixDebugString(int8_t prefix) {
	std::string out(32, OpaquePrefix ? '0' : '1');
	const uint8_t clamped = std::clamp<int8_t>(prefix, 0, 32);
	out.replace(0, clamped, clamped, OpaquePrefix ? '1' : '0');
	StrAppend(out, " prefix=", prefix, " OpaquePrefix=", OpaquePrefix, " PrefixIncrement=", PrefixIncrement);
	return out;
}
#endif

enum class LightType : uint8_t {
	FullyDark,
	PartiallyLit,
	FullyLit,
};

template <LightType Light>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderLineOpaque(uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src, uint_fast8_t n, const uint8_t *DVL_RESTRICT tbl);

template <>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderLineOpaque<LightType::FullyDark>(uint8_t *DVL_RESTRICT dst, [[maybe_unused]] const uint8_t *DVL_RESTRICT src, uint_fast8_t n, [[maybe_unused]] const uint8_t *DVL_RESTRICT tbl)
{
	BlitFillDirect(dst, n, 0);
}

template <>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderLineOpaque<LightType::FullyLit>(uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src, uint_fast8_t n, [[maybe_unused]] const uint8_t *DVL_RESTRICT tbl)
{
#ifndef DEBUG_RENDER_COLOR
	BlitPixelsDirect(dst, src, n);
#else
	BlitFillDirect(dst, n, DBGCOLOR);
#endif
}

template <>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderLineOpaque<LightType::PartiallyLit>(uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src, uint_fast8_t n, const uint8_t *DVL_RESTRICT tbl)
{
#ifndef DEBUG_RENDER_COLOR
	BlitPixelsWithMap(dst, src, n, tbl);
#else
	BlitFillDirect(dst, n, tbl[DBGCOLOR]);
#endif
}

#ifndef DEBUG_RENDER_COLOR
template <LightType Light>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderLineTransparent(uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src, uint_fast8_t n, const uint8_t *DVL_RESTRICT tbl);

template <>
void RenderLineTransparent<LightType::FullyDark>(uint8_t *DVL_RESTRICT dst, [[maybe_unused]] const uint8_t *DVL_RESTRICT src, uint_fast8_t n, [[maybe_unused]] const uint8_t *DVL_RESTRICT tbl)
{
	BlitFillBlended(dst, n, 0);
}

template <>
void RenderLineTransparent<LightType::FullyLit>(uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src, uint_fast8_t n, [[maybe_unused]] const uint8_t *DVL_RESTRICT tbl)
{
	BlitPixelsBlended(dst, src, n);
}

template <>
void RenderLineTransparent<LightType::PartiallyLit>(uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src, uint_fast8_t n, const uint8_t *DVL_RESTRICT tbl)
{
	BlitPixelsBlendedWithMap(dst, src, n, tbl);
}

#ifdef DEVILUTIONX_DUN_RENDER_SMALL_CODE_SIZE
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderLineOpaqueDispatch(LightType Light, uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src, uint_fast8_t n, const uint8_t *DVL_RESTRICT tbl)
{
	switch (Light) {
		case LightType::FullyDark:
			RenderLineOpaque<LightType::FullyDark>(dst, src, n, tbl);
			break;
		case LightType::FullyLit:
			RenderLineOpaque<LightType::FullyLit>(dst, src, n, tbl);
			break;
		case LightType::PartiallyLit:
			RenderLineOpaque<LightType::PartiallyLit>(dst, src, n, tbl);
			break;
	}
}

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderLineTransparentDispatch(LightType Light, uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src, uint_fast8_t n, const uint8_t *DVL_RESTRICT tbl)
{
	switch (Light) {
		case LightType::FullyDark:
			RenderLineTransparent<LightType::FullyDark>(dst, src, n, tbl);
			break;
		case LightType::FullyLit:
			RenderLineTransparent<LightType::FullyLit>(dst, src, n, tbl);
			break;
		case LightType::PartiallyLit:
			RenderLineTransparent<LightType::PartiallyLit>(dst, src, n, tbl);
			break;
	}
}
#endif

#else // DEBUG_RENDER_COLOR
template <LightType Light>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderLineTransparent(uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src, uint_fast8_t n, const uint8_t *DVL_RESTRICT tbl)
{
	for (size_t i = 0; i < n; i++) {
		dst[i] = paletteTransparencyLookup[dst[i]][tbl[DBGCOLOR + 4]];
	}
}
#endif

DEFINE_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src, uint_fast8_t width, const uint8_t *DVL_RESTRICT tbl)
{
	if (Transparent) {
		RENDER_LINE_TRANSPARENT(dst, src, width, tbl);
	} else {
		RENDER_LINE_OPAQUE(dst, src, width, tbl);
	}
}

DEFINE_LIGHT_PREFIX_FN(RenderLineTransparentAndOpaque, uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src, uint_fast8_t prefixWidth, uint_fast8_t width, const uint8_t *DVL_RESTRICT tbl)
{
	if (OpaquePrefix) {
		RENDER_LINE_OPAQUE(dst, src, prefixWidth, tbl);
		if (!SkipTransparentPixels(OpaquePrefix, PrefixIncrement))
			RENDER_LINE_TRANSPARENT(dst + prefixWidth, src + prefixWidth, width - prefixWidth, tbl);
	} else {
		if (!SkipTransparentPixels(OpaquePrefix, PrefixIncrement))
			RENDER_LINE_TRANSPARENT(dst, src, prefixWidth, tbl);
		RENDER_LINE_OPAQUE(dst + prefixWidth, src + prefixWidth, width - prefixWidth, tbl);
	}
}

DEFINE_LIGHT_PREFIX_FN(RenderLine, uint8_t *DVL_RESTRICT dst, const uint8_t *DVL_RESTRICT src, uint_fast8_t n, const uint8_t *DVL_RESTRICT tbl, int8_t prefix)
{
	if (PrefixIncrement == 0) {
		const bool Transparent = OpaquePrefix;
		CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst, src, n, tbl);
	} else if (prefix >= static_cast<int8_t>(n)) {
		// We std::clamp the prefix to (0, n] and avoid calling `RenderLineTransparent/Opaque` with width=0.
		if (OpaquePrefix) {
			RENDER_LINE_OPAQUE(dst, src, n, tbl);
		} else {
			if (!SkipTransparentPixels(OpaquePrefix, PrefixIncrement))
				RENDER_LINE_TRANSPARENT(dst, src, n, tbl);
		}
	} else if (prefix <= 0) {
		if (!OpaquePrefix) {
			RENDER_LINE_OPAQUE(dst, src, n, tbl);
		} else {
			if (!SkipTransparentPixels(OpaquePrefix, PrefixIncrement))
				RENDER_LINE_TRANSPARENT(dst, src, n, tbl);
		}
	} else {
		CALL_LIGHT_PREFIX_FN(RenderLineTransparentAndOpaque, dst, src, prefix, n, tbl);
	}
}

struct Clip {
	int_fast16_t top;
	int_fast16_t bottom;
	int_fast16_t left;
	int_fast16_t right;
	int_fast16_t width;
	int_fast16_t height;
};

DVL_ALWAYS_INLINE Clip CalculateClip(int_fast16_t x, int_fast16_t y, int_fast16_t w, int_fast16_t h, const Surface &out)
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

DEFINE_LIGHT_TRANSPARENT_FN(RenderSquareFull, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl)
{
	for (auto i = 0; i < Height; ++i, dst -= dstPitch) {
		CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst, src, Width, tbl);
		src += Width;
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderSquareClipped, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	src += clip.bottom * Height + clip.left;
	for (auto i = 0; i < clip.height; ++i, dst -= dstPitch) {
		CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst, src, clip.width, tbl);
		src += Width;
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderSquare, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	if (clip.width == Width && clip.height == Height) {
		CALL_LIGHT_TRANSPARENT_FN(RenderSquareFull, dst, dstPitch, src, tbl);
	} else {
		CALL_LIGHT_TRANSPARENT_FN(RenderSquareClipped, dst, dstPitch, src, tbl, clip);
	}
}

DEFINE_LIGHT_PREFIX_FN(RenderTransparentSquareFull, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl)
{
	int8_t prefix = InitPrefix(PrefixIncrement);
	for (auto i = 0; i < Height; ++i, dst -= dstPitch + Width) {
		uint_fast8_t drawWidth = Width;
		while (drawWidth > 0) {
			auto v = static_cast<int8_t>(*src++);
			if (v > 0) {
				CALL_LIGHT_PREFIX_FN(RenderLine, dst, src, v, tbl, prefix - (Width - drawWidth));
				src += v;
			} else {
				v = -v;
			}
			dst += v;
			drawWidth -= v;
		}
		prefix += PrefixIncrement;
	}
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity): Actually complex and has to be fast.
DEFINE_LIGHT_PREFIX_FN(RenderTransparentSquareClipped, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	const auto skipRestOfTheLine = [&src](int_fast16_t remainingWidth) {
		while (remainingWidth > 0) {
			const auto v = static_cast<int8_t>(*src++);
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

	int8_t prefix = InitPrefix(PrefixIncrement, clip.bottom);
	for (auto i = 0; i < clip.height; ++i, dst -= dstPitch + clip.width) {
		auto drawWidth = clip.width;

		// Skip initial src if clipping on the left.
		// Handles overshoot, i.e. when the RLE segment goes into the unclipped area.
		auto remainingLeftClip = clip.left;
		while (remainingLeftClip > 0) {
			auto v = static_cast<int8_t>(*src++);
			if (v > 0) {
				if (v > remainingLeftClip) {
					const auto overshoot = v - remainingLeftClip;
					CALL_LIGHT_PREFIX_FN(RenderLine, dst, src + remainingLeftClip, overshoot, tbl, prefix - (Width - remainingLeftClip));
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
		}

		// Draw the non-clipped segment
		while (drawWidth > 0) {
			auto v = static_cast<int8_t>(*src++);
			if (v > 0) {
				if (v > drawWidth) {
					CALL_LIGHT_PREFIX_FN(RenderLine, dst, src, drawWidth, tbl, prefix - (Width - drawWidth));
					src += v;
					dst += drawWidth;
					drawWidth -= v;
					break;
				}
				CALL_LIGHT_PREFIX_FN(RenderLine, dst, src, v, tbl, prefix - (Width - drawWidth));
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
		}

		// Skip the rest of src line if clipping on the right
		assert(drawWidth <= 0);
		skipRestOfTheLine(clip.right + drawWidth);
		prefix += PrefixIncrement;
	}
}

DEFINE_LIGHT_PREFIX_FN(RenderTransparentSquare, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	if (clip.width == Width && clip.height == Height) {
		CALL_LIGHT_PREFIX_FN(RenderTransparentSquareFull, dst, dstPitch, src, tbl);
	} else {
		CALL_LIGHT_PREFIX_FN(RenderTransparentSquareClipped, dst, dstPitch, src, tbl, clip);
	}
}

/** Vertical clip for the lower and upper triangles of a diamond tile (L/RTRIANGLE).*/
struct DiamondClipY {
	int_fast16_t lowerBottom;
	int_fast16_t lowerTop;
	int_fast16_t upperBottom;
	int_fast16_t upperTop;
};

template <int_fast16_t UpperHeight = TriangleUpperHeight>
DVL_ALWAYS_INLINE DiamondClipY CalculateDiamondClipY(const Clip &clip)
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

DVL_ALWAYS_INLINE std::size_t CalculateTriangleSourceSkipLowerBottom(int_fast16_t numLines)
{
	return XStep * numLines * (numLines + 1) / 2 + 2 * ((numLines + 1) / 2);
}

DVL_ALWAYS_INLINE std::size_t CalculateTriangleSourceSkipUpperBottom(int_fast16_t numLines)
{
	return 2 * TriangleUpperHeight * numLines - numLines * (numLines - 1) + 2 * ((numLines + 1) / 2);
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderLeftTriangleLower, uint8_t *DVL_RESTRICT &dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT &src, const uint8_t *DVL_RESTRICT tbl)
{
	dst += XStep * (LowerHeight - 1);
	for (auto i = 1; i <= LowerHeight; ++i, dst -= dstPitch + XStep) {
		src += 2 * (i % 2);
		const auto width = XStep * i;
		CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst, src, width, tbl);
		src += width;
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderLeftTriangleLowerClipVertical, const DiamondClipY &clipY, uint8_t *DVL_RESTRICT &dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT &src, const uint8_t *DVL_RESTRICT tbl)
{
	src += CalculateTriangleSourceSkipLowerBottom(clipY.lowerBottom);
	dst += XStep * (LowerHeight - clipY.lowerBottom - 1);
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	for (auto i = 1 + clipY.lowerBottom; i <= lowerMax; ++i, dst -= dstPitch + XStep) {
		src += 2 * (i % 2);
		const auto width = XStep * i;
		CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst, src, width, tbl);
		src += width;
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderLeftTriangleLowerClipLeftAndVertical, int_fast16_t clipLeft, const DiamondClipY &clipY, uint8_t *DVL_RESTRICT &dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT &src, const uint8_t *DVL_RESTRICT tbl)
{
	src += CalculateTriangleSourceSkipLowerBottom(clipY.lowerBottom);
	dst += XStep * (LowerHeight - clipY.lowerBottom - 1) - clipLeft;
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	for (auto i = 1 + clipY.lowerBottom; i <= lowerMax; ++i, dst -= dstPitch + XStep) {
		src += 2 * (i % 2);
		const auto width = XStep * i;
		const auto startX = Width - XStep * i;
		const auto skip = startX < clipLeft ? clipLeft - startX : 0;
		if (width > skip)
			CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst + skip, src + skip, width - skip, tbl);
		src += width;
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderLeftTriangleLowerClipRightAndVertical, int_fast16_t clipRight, const DiamondClipY &clipY, uint8_t *DVL_RESTRICT &dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT &src, const uint8_t *DVL_RESTRICT tbl)
{
	src += CalculateTriangleSourceSkipLowerBottom(clipY.lowerBottom);
	dst += XStep * (LowerHeight - clipY.lowerBottom - 1);
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	for (auto i = 1 + clipY.lowerBottom; i <= lowerMax; ++i, dst -= dstPitch + XStep) {
		src += 2 * (i % 2);
		const auto width = XStep * i;
		if (width > clipRight)
			CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst, src, width - clipRight, tbl);
		src += width;
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderLeftTriangleFull, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl)
{
	CALL_LIGHT_TRANSPARENT_FN(RenderLeftTriangleLower, dst, dstPitch, src, tbl);
	dst += 2 * XStep;
	for (auto i = 1; i <= TriangleUpperHeight; ++i, dst -= dstPitch - XStep) {
		src += 2 * (i % 2);
		const auto width = Width - XStep * i;
		CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst, src, width, tbl);
		src += width;
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderLeftTriangleClipVertical, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	const DiamondClipY clipY = CalculateDiamondClipY(clip);
	CALL_LIGHT_TRANSPARENT_FN(RenderLeftTriangleLowerClipVertical, clipY, dst, dstPitch, src, tbl);
	src += CalculateTriangleSourceSkipUpperBottom(clipY.upperBottom);
	dst += 2 * XStep + XStep * clipY.upperBottom;
	const auto upperMax = TriangleUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch - XStep) {
		src += 2 * (i % 2);
		const auto width = Width - XStep * i;
		CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst, src, width, tbl);
		src += width;
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderLeftTriangleClipLeftAndVertical, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	const DiamondClipY clipY = CalculateDiamondClipY(clip);
	const int_fast16_t clipLeft = clip.left;
	CALL_LIGHT_TRANSPARENT_FN(RenderLeftTriangleLowerClipLeftAndVertical, clipLeft, clipY, dst, dstPitch, src, tbl);
	src += CalculateTriangleSourceSkipUpperBottom(clipY.upperBottom);
	dst += 2 * XStep + XStep * clipY.upperBottom;
	const auto upperMax = TriangleUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch - XStep) {
		src += 2 * (i % 2);
		const auto width = Width - XStep * i;
		const auto startX = XStep * i;
		const auto skip = startX < clipLeft ? clipLeft - startX : 0;
		CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst + skip, src + skip, width > skip ? width - skip : 0, tbl);
		src += width;
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderLeftTriangleClipRightAndVertical, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	const DiamondClipY clipY = CalculateDiamondClipY(clip);
	const int_fast16_t clipRight = clip.right;
	CALL_LIGHT_TRANSPARENT_FN(RenderLeftTriangleLowerClipRightAndVertical, clipRight, clipY, dst, dstPitch, src, tbl);
	src += CalculateTriangleSourceSkipUpperBottom(clipY.upperBottom);
	dst += 2 * XStep + XStep * clipY.upperBottom;
	const auto upperMax = TriangleUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch - XStep) {
		src += 2 * (i % 2);
		const auto width = Width - XStep * i;
		if (width <= clipRight)
			break;
		CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst, src, width - clipRight, tbl);
		src += width;
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderLeftTriangle, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	if (clip.width == Width) {
		if (clip.height == TriangleHeight) {
			CALL_LIGHT_TRANSPARENT_FN(RenderLeftTriangleFull, dst, dstPitch, src, tbl);
		} else {
			CALL_LIGHT_TRANSPARENT_FN(RenderLeftTriangleClipVertical, dst, dstPitch, src, tbl, clip);
		}
	} else if (clip.right == 0) {
		CALL_LIGHT_TRANSPARENT_FN(RenderLeftTriangleClipLeftAndVertical, dst, dstPitch, src, tbl, clip);
	} else {
		CALL_LIGHT_TRANSPARENT_FN(RenderLeftTriangleClipRightAndVertical, dst, dstPitch, src, tbl, clip);
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderRightTriangleLower, uint8_t *DVL_RESTRICT &dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT &src, const uint8_t *DVL_RESTRICT tbl)
{
	for (auto i = 1; i <= LowerHeight; ++i, dst -= dstPitch) {
		const auto width = XStep * i;
		CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst, src, width, tbl);
		src += width + 2 * (i % 2);
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderRightTriangleLowerClipVertical, const DiamondClipY &clipY, uint8_t *DVL_RESTRICT &dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT &src, const uint8_t *DVL_RESTRICT tbl)
{
	src += CalculateTriangleSourceSkipLowerBottom(clipY.lowerBottom);
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	for (auto i = 1 + clipY.lowerBottom; i <= lowerMax; ++i, dst -= dstPitch) {
		const auto width = XStep * i;
		CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst, src, width, tbl);
		src += width + 2 * (i % 2);
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderRightTriangleLowerClipLeftAndVertical, int_fast16_t clipLeft, const DiamondClipY &clipY, uint8_t *DVL_RESTRICT &dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT &src, const uint8_t *DVL_RESTRICT tbl)
{
	src += CalculateTriangleSourceSkipLowerBottom(clipY.lowerBottom);
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	for (auto i = 1 + clipY.lowerBottom; i <= lowerMax; ++i, dst -= dstPitch) {
		const auto width = XStep * i;
		if (width > clipLeft)
			CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst, src + clipLeft, width - clipLeft, tbl);
		src += width + 2 * (i % 2);
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderRightTriangleLowerClipRightAndVertical, int_fast16_t clipRight, const DiamondClipY &clipY, uint8_t *DVL_RESTRICT &dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT &src, const uint8_t *DVL_RESTRICT tbl)
{
	src += CalculateTriangleSourceSkipLowerBottom(clipY.lowerBottom);
	const auto lowerMax = LowerHeight - clipY.lowerTop;
	for (auto i = 1 + clipY.lowerBottom; i <= lowerMax; ++i, dst -= dstPitch) {
		const auto width = XStep * i;
		const auto skip = Width - width < clipRight ? clipRight - (Width - width) : 0;
		if (width > skip)
			CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst, src, width - skip, tbl);
		src += width + 2 * (i % 2);
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderRightTriangleFull, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl)
{
	CALL_LIGHT_TRANSPARENT_FN(RenderRightTriangleLower, dst, dstPitch, src, tbl);
	for (auto i = 1; i <= TriangleUpperHeight; ++i, dst -= dstPitch) {
		const auto width = Width - XStep * i;
		CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst, src, width, tbl);
		src += width + 2 * (i % 2);
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderRightTriangleClipVertical, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	const DiamondClipY clipY = CalculateDiamondClipY(clip);
	CALL_LIGHT_TRANSPARENT_FN(RenderRightTriangleLowerClipVertical, clipY, dst, dstPitch, src, tbl);
	src += CalculateTriangleSourceSkipUpperBottom(clipY.upperBottom);
	const auto upperMax = TriangleUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch) {
		const auto width = Width - XStep * i;
		CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst, src, width, tbl);
		src += width + 2 * (i % 2);
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderRightTriangleClipLeftAndVertical, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	const DiamondClipY clipY = CalculateDiamondClipY(clip);
	const int_fast16_t clipLeft = clip.left;
	CALL_LIGHT_TRANSPARENT_FN(RenderRightTriangleLowerClipLeftAndVertical, clipLeft, clipY, dst, dstPitch, src, tbl);
	src += CalculateTriangleSourceSkipUpperBottom(clipY.upperBottom);
	const auto upperMax = TriangleUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch) {
		const auto width = Width - XStep * i;
		if (width <= clipLeft)
			break;
		CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst, src + clipLeft, width - clipLeft, tbl);
		src += width + 2 * (i % 2);
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderRightTriangleClipRightAndVertical, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	const DiamondClipY clipY = CalculateDiamondClipY(clip);
	const int_fast16_t clipRight = clip.right;
	CALL_LIGHT_TRANSPARENT_FN(RenderRightTriangleLowerClipRightAndVertical, clipRight, clipY, dst, dstPitch, src, tbl);
	src += CalculateTriangleSourceSkipUpperBottom(clipY.upperBottom);
	const auto upperMax = TriangleUpperHeight - clipY.upperTop;
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch) {
		const auto width = Width - XStep * i;
		const auto skip = Width - width < clipRight ? clipRight - (Width - width) : 0;
		CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst, src, width > skip ? width - skip : 0, tbl);
		src += width + 2 * (i % 2);
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderRightTriangle, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	if (clip.width == Width) {
		if (clip.height == TriangleHeight) {
			CALL_LIGHT_TRANSPARENT_FN(RenderRightTriangleFull, dst, dstPitch, src, tbl);
		} else {
			CALL_LIGHT_TRANSPARENT_FN(RenderRightTriangleClipVertical, dst, dstPitch, src, tbl, clip);
		}
	} else if (clip.right == 0) {
		CALL_LIGHT_TRANSPARENT_FN(RenderRightTriangleClipLeftAndVertical, dst, dstPitch, src, tbl, clip);
	} else {
		CALL_LIGHT_TRANSPARENT_FN(RenderRightTriangleClipRightAndVertical, dst, dstPitch, src, tbl, clip);
	}
}

DEFINE_LIGHT_PREFIX_FN(RenderTrapezoidUpperHalf, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl)
{
	if (PrefixIncrement != 0) {
		// The first and the last line are always fully transparent/opaque (or vice-versa).
		// We handle them specially to avoid calling the blitter with width=0.
		const uint8_t *srcEnd = src + Width * TrapezoidUpperHeight;
		const bool FirstLineIsTransparent = OpaquePrefix ^ (PrefixIncrement < 0);
		if (FirstLineIsTransparent) {
			if (!SkipTransparentPixels(OpaquePrefix, PrefixIncrement))
				RENDER_LINE_TRANSPARENT(dst, src, Width, tbl);
		} else {
			RENDER_LINE_OPAQUE(dst, src, Width, tbl);
		}
		src += Width;
		dst -= dstPitch;
		uint8_t prefixWidth = (PrefixIncrement < 0 ? 32 : 0) + PrefixIncrement;
		do {
			CALL_LIGHT_PREFIX_FN(RenderLineTransparentAndOpaque, dst, src, prefixWidth, Width, tbl);
			prefixWidth += PrefixIncrement;
			src += Width;
			dst -= dstPitch;
		} while (src != srcEnd);
	} else { // PrefixIncrement == 0;
		const uint8_t *srcEnd = src + Width * TrapezoidUpperHeight;
		const bool Transparent = OpaquePrefix;
		do {
			CALL_LIGHT_TRANSPARENT_FN(RenderLineTransparentOrOpaque, dst, src, Width, tbl);
			src += Width;
			dst -= dstPitch;
		} while (src != srcEnd);
	}
}

DEFINE_LIGHT_PREFIX_FN(RenderTrapezoidUpperHalfClipVertical, const Clip &clip, const DiamondClipY &clipY, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl)
{
	const auto upperMax = TrapezoidUpperHeight - clipY.upperTop;
	int8_t prefix = InitPrefix(PrefixIncrement, clip.bottom);
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch) {
		CALL_LIGHT_PREFIX_FN(RenderLine, dst, src, Width, tbl, prefix);
		src += Width;
		prefix += PrefixIncrement;
	}
}

DEFINE_LIGHT_PREFIX_FN(RenderTrapezoidUpperHalfClipLeftAndVertical, const Clip &clip, const DiamondClipY &clipY, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl)
{
	const auto upperMax = TrapezoidUpperHeight - clipY.upperTop;
	int8_t prefix = InitPrefix(PrefixIncrement, clip.bottom);
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch) {
		CALL_LIGHT_PREFIX_FN(RenderLine, dst, src, clip.width, tbl, prefix - clip.left);
		src += Width;
		prefix += PrefixIncrement;
	}
}

DEFINE_LIGHT_PREFIX_FN(RenderTrapezoidUpperHalfClipRightAndVertical, const Clip &clip, const DiamondClipY &clipY, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl)
{
	const auto upperMax = TrapezoidUpperHeight - clipY.upperTop;
	int8_t prefix = InitPrefix(PrefixIncrement, clip.bottom);
	for (auto i = 1 + clipY.upperBottom; i <= upperMax; ++i, dst -= dstPitch) {
		CALL_LIGHT_PREFIX_FN(RenderLine, dst, src, clip.width, tbl, prefix);
		src += Width;
		prefix += PrefixIncrement;
	}
}

DEFINE_LIGHT_PREFIX_FN(RenderLeftTrapezoidFull, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl)
{
	const bool Transparent = LowerHalfTransparent(OpaquePrefix, PrefixIncrement);
	CALL_LIGHT_TRANSPARENT_FN(RenderLeftTriangleLower, dst, dstPitch, src, tbl);
	dst += XStep;
	CALL_LIGHT_PREFIX_FN(RenderTrapezoidUpperHalf, dst, dstPitch, src, tbl);
}

DEFINE_LIGHT_PREFIX_FN(RenderLeftTrapezoidClipVertical, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	const DiamondClipY clipY = CalculateDiamondClipY<TrapezoidUpperHeight>(clip);
	const bool Transparent = LowerHalfTransparent(OpaquePrefix, PrefixIncrement);
	CALL_LIGHT_TRANSPARENT_FN(RenderLeftTriangleLowerClipVertical, clipY, dst, dstPitch, src, tbl);
	src += clipY.upperBottom * Width;
	dst += XStep;
	CALL_LIGHT_PREFIX_FN(RenderTrapezoidUpperHalfClipVertical, clip, clipY, dst, dstPitch, src, tbl);
}

DEFINE_LIGHT_PREFIX_FN(RenderLeftTrapezoidClipLeftAndVertical, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	const DiamondClipY clipY = CalculateDiamondClipY<TrapezoidUpperHeight>(clip);
	const bool Transparent = LowerHalfTransparent(OpaquePrefix, PrefixIncrement);
	CALL_LIGHT_TRANSPARENT_FN(RenderLeftTriangleLowerClipLeftAndVertical, clip.left, clipY, dst, dstPitch, src, tbl);
	src += clipY.upperBottom * Width + clip.left;
	dst += XStep + clip.left;
	CALL_LIGHT_PREFIX_FN(RenderTrapezoidUpperHalfClipLeftAndVertical, clip, clipY, dst, dstPitch, src, tbl);
}

DEFINE_LIGHT_PREFIX_FN(RenderLeftTrapezoidClipRightAndVertical, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	const DiamondClipY clipY = CalculateDiamondClipY<TrapezoidUpperHeight>(clip);
	const bool Transparent = LowerHalfTransparent(OpaquePrefix, PrefixIncrement);
	CALL_LIGHT_TRANSPARENT_FN(RenderLeftTriangleLowerClipRightAndVertical, clip.right, clipY, dst, dstPitch, src, tbl);
	src += clipY.upperBottom * Width;
	dst += XStep;
	CALL_LIGHT_PREFIX_FN(RenderTrapezoidUpperHalfClipRightAndVertical, clip, clipY, dst, dstPitch, src, tbl);
}

DEFINE_LIGHT_PREFIX_FN(RenderLeftTrapezoid, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	if (clip.width == Width) {
		if (clip.height == Height) {
			CALL_LIGHT_PREFIX_FN(RenderLeftTrapezoidFull, dst, dstPitch, src, tbl);
		} else {
			CALL_LIGHT_PREFIX_FN(RenderLeftTrapezoidClipVertical, dst, dstPitch, src, tbl, clip);
		}
	} else if (clip.right == 0) {
		CALL_LIGHT_PREFIX_FN(RenderLeftTrapezoidClipLeftAndVertical, dst, dstPitch, src, tbl, clip);
	} else {
		CALL_LIGHT_PREFIX_FN(RenderLeftTrapezoidClipRightAndVertical, dst, dstPitch, src, tbl, clip);
	}
}

DEFINE_LIGHT_PREFIX_FN(RenderRightTrapezoidFull, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl)
{
	{
		const bool Transparent = LowerHalfTransparent(OpaquePrefix, PrefixIncrement);
		CALL_LIGHT_TRANSPARENT_FN(RenderRightTriangleLower, dst, dstPitch, src, tbl);
	}
	CALL_LIGHT_PREFIX_FN(RenderTrapezoidUpperHalf, dst, dstPitch, src, tbl);
}

DEFINE_LIGHT_PREFIX_FN(RenderRightTrapezoidClipVertical, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	const DiamondClipY clipY = CalculateDiamondClipY<TrapezoidUpperHeight>(clip);
	const bool Transparent = LowerHalfTransparent(OpaquePrefix, PrefixIncrement);
	CALL_LIGHT_TRANSPARENT_FN(RenderRightTriangleLowerClipVertical, clipY, dst, dstPitch, src, tbl);
	src += clipY.upperBottom * Width;
	CALL_LIGHT_PREFIX_FN(RenderTrapezoidUpperHalfClipVertical, clip, clipY, dst, dstPitch, src, tbl);
}

DEFINE_LIGHT_PREFIX_FN(RenderRightTrapezoidClipLeftAndVertical, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	const DiamondClipY clipY = CalculateDiamondClipY<TrapezoidUpperHeight>(clip);
	const bool Transparent = LowerHalfTransparent(OpaquePrefix, PrefixIncrement);
	CALL_LIGHT_TRANSPARENT_FN(RenderRightTriangleLowerClipLeftAndVertical, clip.left, clipY, dst, dstPitch, src, tbl);
	src += clipY.upperBottom * Width + clip.left;
	CALL_LIGHT_PREFIX_FN(RenderTrapezoidUpperHalfClipLeftAndVertical, clip, clipY, dst, dstPitch, src, tbl);
}

DEFINE_LIGHT_PREFIX_FN(RenderRightTrapezoidClipRightAndVertical, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	const DiamondClipY clipY = CalculateDiamondClipY<TrapezoidUpperHeight>(clip);
	const bool Transparent = LowerHalfTransparent(OpaquePrefix, PrefixIncrement);
	CALL_LIGHT_TRANSPARENT_FN(RenderRightTriangleLowerClipRightAndVertical, clip.right, clipY, dst, dstPitch, src, tbl);
	src += clipY.upperBottom * Width;
	CALL_LIGHT_PREFIX_FN(RenderTrapezoidUpperHalfClipRightAndVertical, clip, clipY, dst, dstPitch, src, tbl);
}

DEFINE_LIGHT_PREFIX_FN(RenderRightTrapezoid, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	if (clip.width == Width) {
		if (clip.height == Height) {
			CALL_LIGHT_PREFIX_FN(RenderRightTrapezoidFull, dst, dstPitch, src, tbl);
		} else {
			CALL_LIGHT_PREFIX_FN(RenderRightTrapezoidClipVertical, dst, dstPitch, src, tbl, clip);
		}
	} else if (clip.right == 0) {
		CALL_LIGHT_PREFIX_FN(RenderRightTrapezoidClipLeftAndVertical, dst, dstPitch, src, tbl, clip);
	} else {
		CALL_LIGHT_PREFIX_FN(RenderRightTrapezoidClipRightAndVertical, dst, dstPitch, src, tbl, clip);
	}
}

DEFINE_LIGHT_TRANSPARENT_FN(RenderTileType, TileType tile, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	const bool OpaquePrefix = Transparent;
	const int8_t PrefixIncrement = 0;
	switch (tile) {
	case TileType::Square:
		CALL_LIGHT_TRANSPARENT_FN(RenderSquare, dst, dstPitch, src, tbl, clip);
		break;
	case TileType::TransparentSquare:
		CALL_LIGHT_PREFIX_FN(RenderTransparentSquare, dst, dstPitch, src, tbl, clip);
		break;
	case TileType::LeftTriangle:
		CALL_LIGHT_TRANSPARENT_FN(RenderLeftTriangle, dst, dstPitch, src, tbl, clip);
		break;
	case TileType::RightTriangle:
		CALL_LIGHT_TRANSPARENT_FN(RenderRightTriangle, dst, dstPitch, src, tbl, clip);
		break;
	case TileType::LeftTrapezoid:
		CALL_LIGHT_PREFIX_FN(RenderLeftTrapezoid, dst, dstPitch, src, tbl, clip);
		break;
	case TileType::RightTrapezoid:
		CALL_LIGHT_PREFIX_FN(RenderRightTrapezoid, dst, dstPitch, src, tbl, clip);
		break;
	}
}

DEFINE_LIGHT_PREFIX_FN(RenderLeftTrapezoidOrTransparentSquare, TileType tile, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	switch (tile) {
	case TileType::TransparentSquare:
		CALL_LIGHT_PREFIX_FN(RenderTransparentSquare, dst, dstPitch, src, tbl, clip);
		break;
	case TileType::LeftTrapezoid:
		CALL_LIGHT_PREFIX_FN(RenderLeftTrapezoid, dst, dstPitch, src, tbl, clip);
		break;
	default:
		app_fatal("Given mask can only be applied to TransparentSquare or LeftTrapezoid tiles");
	}
}

DEFINE_LIGHT_PREFIX_FN(RenderRightTrapezoidOrTransparentSquare, TileType tile, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	switch (tile) {
	case TileType::TransparentSquare:
		CALL_LIGHT_PREFIX_FN(RenderTransparentSquare, dst, dstPitch, src, tbl, clip);
		break;
	case TileType::RightTrapezoid:
		CALL_LIGHT_PREFIX_FN(RenderRightTrapezoid, dst, dstPitch, src, tbl, clip);
		break;
	default:
		app_fatal("Given mask can only be applied to TransparentSquare or LeftTrapezoid tiles");
	}
}

#ifdef DEVILUTIONX_DUN_RENDER_SMALL_CODE_SIZE
DVL_ALWAYS_INLINE LightType GetLightType(const uint8_t *tbl)
{
	if (tbl == LightTables[LightsMax].data()) return LightType::FullyDark;
	if (tbl == LightTables[0].data()) return LightType::FullyLit;
	return LightType::PartiallyLit;
}
#else
template <bool OpaquePrefix, int8_t PrefixIncrement>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderTransparentSquareDispatch(uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	if (tbl == LightTables[LightsMax].data()) {
		RenderTransparentSquare<LightType::FullyDark, OpaquePrefix, PrefixIncrement>(dst, dstPitch, src, tbl, clip);
	} else if (tbl == LightTables[0].data()) {
		RenderTransparentSquare<LightType::FullyLit, OpaquePrefix, PrefixIncrement>(dst, dstPitch, src, tbl, clip);
	} else {
		RenderTransparentSquare<LightType::PartiallyLit, OpaquePrefix, PrefixIncrement>(dst, dstPitch, src, tbl, clip);
	}
}

template <bool OpaquePrefix, int8_t PrefixIncrement>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderLeftTrapezoidOrTransparentSquareDispatch(TileType tile, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	if (tbl == LightTables[LightsMax].data()) {
		RenderLeftTrapezoidOrTransparentSquare<LightType::FullyDark, OpaquePrefix, PrefixIncrement>(tile, dst, dstPitch, src, tbl, clip);
	} else if (tbl == LightTables[0].data()) {
		RenderLeftTrapezoidOrTransparentSquare<LightType::FullyLit, OpaquePrefix, PrefixIncrement>(tile, dst, dstPitch, src, tbl, clip);
	} else {
		RenderLeftTrapezoidOrTransparentSquare<LightType::PartiallyLit, OpaquePrefix, PrefixIncrement>(tile, dst, dstPitch, src, tbl, clip);
	}
}

template <bool OpaquePrefix, int8_t PrefixIncrement>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderRightTrapezoidOrTransparentSquareDispatch(TileType tile, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	if (tbl == LightTables[LightsMax].data()) {
		RenderRightTrapezoidOrTransparentSquare<LightType::FullyDark, OpaquePrefix, PrefixIncrement>(tile, dst, dstPitch, src, tbl, clip);
	} else if (tbl == LightTables[0].data()) {
		RenderRightTrapezoidOrTransparentSquare<LightType::FullyLit, OpaquePrefix, PrefixIncrement>(tile, dst, dstPitch, src, tbl, clip);
	} else {
		RenderRightTrapezoidOrTransparentSquare<LightType::PartiallyLit, OpaquePrefix, PrefixIncrement>(tile, dst, dstPitch, src, tbl, clip);
	}
}

template <bool Transparent>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderTileDispatch(TileType tile, uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, const uint8_t *DVL_RESTRICT src, const uint8_t *DVL_RESTRICT tbl, Clip clip)
{
	if (tbl == LightTables[LightsMax].data()) {
		RenderTileType<LightType::FullyDark, Transparent>(tile, dst, dstPitch, src, tbl, clip);
	} else if (tbl == LightTables[0].data()) {
		RenderTileType<LightType::FullyLit, Transparent>(tile, dst, dstPitch, src, tbl, clip);
	} else {
		RenderTileType<LightType::PartiallyLit, Transparent>(tile, dst, dstPitch, src, tbl, clip);
	}
}
#endif

// Blit with left and vertical clipping.
void RenderBlackTileClipLeftAndVertical(uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, int sx, DiamondClipY clipY)
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
void RenderBlackTileClipRightAndVertical(uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, int_fast16_t maxWidth, DiamondClipY clipY)
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
void RenderBlackTileClipY(uint8_t *DVL_RESTRICT dst, uint16_t dstPitch, DiamondClipY clipY)
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
void RenderBlackTileFull(uint8_t *DVL_RESTRICT dst, uint16_t dstPitch)
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

#ifdef DUN_RENDER_STATS
std::unordered_map<DunRenderType, size_t, DunRenderTypeHash> DunRenderStats;

std::string_view TileTypeToString(TileType tileType)
{
	// clang-format off
	switch (tileType) {
	case TileType::Square: return "Square";
	case TileType::TransparentSquare: return "TransparentSquare";
	case TileType::LeftTriangle: return "LeftTriangle";
	case TileType::RightTriangle: return "RightTriangle";
	case TileType::LeftTrapezoid: return "LeftTrapezoid";
	case TileType::RightTrapezoid: return "RightTrapezoid";
	default: return "???";
	}
	// clang-format on
}

std::string_view MaskTypeToString(MaskType maskType)
{
	// clang-format off
	switch (maskType) {
	case MaskType::Solid: return "Solid";
	case MaskType::Transparent: return "Transparent";
	case MaskType::Right: return "Right";
	case MaskType::Left: return "Left";
	case MaskType::RightFoliage: return "RightFoliage";
	case MaskType::LeftFoliage: return "LeftFoliage";
	default: return "???";
	}
	// clang-format on
}
#endif

void RenderTile(const Surface &out, Point position,
    LevelCelBlock levelCelBlock, MaskType maskType, const uint8_t *tbl)
{
	const TileType tile = levelCelBlock.type();

#ifdef DEBUG_RENDER_OFFSET_X
	position.x += DEBUG_RENDER_OFFSET_X;
#endif
#ifdef DEBUG_RENDER_OFFSET_Y
	position.y += DEBUG_RENDER_OFFSET_Y;
#endif
#ifdef DEBUG_RENDER_COLOR
	DBGCOLOR = GetTileDebugColor(tile);
#endif

	const Clip clip = CalculateClip(position.x, position.y, Width, GetTileHeight(tile), out);
	if (clip.width <= 0 || clip.height <= 0)
		return;

	const auto *pFrameTable = reinterpret_cast<const uint32_t *>(pDungeonCels.get());
	const auto *src = reinterpret_cast<const uint8_t *>(&pDungeonCels[SDL_SwapLE32(pFrameTable[levelCelBlock.frame()])]);
	uint8_t *dst = out.at(static_cast<int>(position.x + clip.left), static_cast<int>(position.y - clip.bottom));
	const uint16_t dstPitch = out.pitch();

#ifdef DUN_RENDER_STATS
	++DunRenderStats[DunRenderType { tile, maskType }];
#endif

#ifdef DEVILUTIONX_DUN_RENDER_SMALL_CODE_SIZE
	const LightType Light = GetLightType(tbl);
	switch (maskType) {
	case MaskType::Solid:
		RenderTileType(Light, /*Transparent=*/false, tile, dst, dstPitch, src, tbl, clip);
		break;
	case MaskType::Transparent:
		RenderTileType(Light, /*Transparent=*/true, tile, dst, dstPitch, src, tbl, clip);
		break;
	case MaskType::Left:
		RenderLeftTrapezoidOrTransparentSquare(Light, /*OpaquePrefix=*/false, /*PrefixIncrement=*/2, tile, dst, dstPitch, src, tbl, clip);
		break;
	case MaskType::Right:
		RenderRightTrapezoidOrTransparentSquare(Light, /*OpaquePrefix=*/true, /*PrefixIncrement=*/-2, tile, dst, dstPitch, src, tbl, clip);
		break;
	case MaskType::LeftFoliage:
		RenderTransparentSquare(Light, /*OpaquePrefix=*/true, /*PrefixIncrement=*/2, dst, dstPitch, src, tbl, clip);
		break;
	case MaskType::RightFoliage:
		RenderTransparentSquare(Light, /*OpaquePrefix=*/false, /*PrefixIncrement=*/-2, dst, dstPitch, src, tbl, clip);
		break;
	}
#else
	switch (maskType) {
	case MaskType::Solid:
		RenderTileDispatch</*Transparent=*/false>(tile, dst, dstPitch, src, tbl, clip);
		break;
	case MaskType::Transparent:
		RenderTileDispatch</*Transparent=*/true>(tile, dst, dstPitch, src, tbl, clip);
		break;
	case MaskType::Left:
		RenderLeftTrapezoidOrTransparentSquareDispatch</*OpaquePrefix=*/false, /*PrefixIncrement=*/2>(tile, dst, dstPitch, src, tbl, clip);
		break;
	case MaskType::Right:
		RenderRightTrapezoidOrTransparentSquareDispatch</*OpaquePrefix=*/true, /*PrefixIncrement=*/-2>(tile, dst, dstPitch, src, tbl, clip);
		break;
	case MaskType::LeftFoliage:
		RenderTransparentSquareDispatch</*OpaquePrefix=*/true, /*PrefixIncrement=*/2>(dst, dstPitch, src, tbl, clip);
		break;
	case MaskType::RightFoliage:
		RenderTransparentSquareDispatch</*OpaquePrefix=*/false, /*PrefixIncrement=*/-2>(dst, dstPitch, src, tbl, clip);
		break;
	}
#endif
#ifdef DEBUG_STR
	const auto [debugStr, flags] = GetTileDebugStr(tile);
	DrawString(out, debugStr, Rectangle { Point { position.x + 2, position.y - 29 }, Size { 28, 28 } }, flags);
#endif
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
	uint8_t *dst = out.at(sx, static_cast<int>(sy - clip.bottom));
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

void DunTileColorMap(std::span<const uint8_t> src, TileType tileType,
    const uint8_t *tbl, uint8_t *dst)
{
	if (tileType == TileType::TransparentSquare) {
		const uint8_t *srcPtr = src.data();
		for (auto i = 0; i < Height; ++i) {
			for (uint_fast8_t drawWidth = Width; drawWidth > 0;) {
				auto v = static_cast<int8_t>(*srcPtr++);
				if (v > 0) {
					drawWidth -= v;
					while (v-- > 0) {
						*dst++ = tbl[*srcPtr++];
					}
				} else {
					drawWidth += v;
				}
			}
		}
	} else {
		for (const uint8_t pix : src) {
			*dst++ = tbl[pix];
		}
	}
}

} // namespace devilution
