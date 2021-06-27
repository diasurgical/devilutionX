/**
 * @file cl2_render.cpp
 *
 * CL2 rendering.
 */
#include "cl2_render.hpp"

#include <algorithm>

#include "engine/cel_header.hpp"
#include "engine/render/common_impl.h"
#include "scrollrt.h"
#include "utils/attributes.h"

namespace devilution {
namespace {

/**
 * CL2 is similar to CEL, with the following differences:
 *
 * 1. Transparent runs can cross line boundaries.
 * 2. Control bytes are different, and the [0x80, 0xBE] control byte range
 *    indicates a fill-N command.
 */

constexpr bool IsCl2Opaque(std::uint8_t control)
{
	constexpr std::uint8_t Cl2OpaqueMin = 0x80;
	return control >= Cl2OpaqueMin;
}

constexpr std::uint8_t GetCl2OpaquePixelsWidth(std::uint8_t control)
{
	return -static_cast<std::int8_t>(control);
}

constexpr bool IsCl2OpaqueFill(std::uint8_t control)
{
	constexpr std::uint8_t Cl2FillMax = 0xBE;
	return control <= Cl2FillMax;
}

constexpr std::uint8_t GetCl2OpaqueFillWidth(std::uint8_t control)
{
	constexpr std::uint8_t Cl2FillEnd = 0xBF;
	return static_cast<std::int_fast16_t>(Cl2FillEnd - control);
}

struct SkipSize {
	std::int_fast16_t wholeLines;
	std::int_fast16_t xOffset;
};
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT SkipSize GetSkipSize(std::int_fast16_t overrun, std::int_fast16_t srcWidth)
{
	SkipSize result;
	result.wholeLines = overrun / srcWidth;
	result.xOffset = overrun - srcWidth * result.wholeLines;
	return result;
}

// Debugging variables
// #define DEBUG_RENDER_COLOR 213 // orange-ish hue

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT const byte *SkipRestOfCl2Line(
    const byte *src, std::int_fast16_t srcWidth,
    std::int_fast16_t remainingWidth, SkipSize &skipSize)
{
	while (remainingWidth > 0) {
		auto v = static_cast<std::uint8_t>(*src++);
		if (IsCl2Opaque(v)) {
			if (IsCl2OpaqueFill(v)) {
				remainingWidth -= GetCl2OpaqueFillWidth(v);
				++src;
			} else {
				v = GetCl2OpaquePixelsWidth(v);
				src += v;
				remainingWidth -= v;
			}
		} else {
			remainingWidth -= v;
		}
	}
	if (remainingWidth < 0) {
		skipSize = GetSkipSize(-remainingWidth, srcWidth);
		++skipSize.wholeLines;
	} else {
		skipSize.wholeLines = 1;
		skipSize.xOffset = 0;
	}
	return src;
}

/** Renders a CL2 sprite with only vertical clipping to the output buffer. */
template <typename RenderPixels, typename RenderFill>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderCl2ClipY(
    const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize, std::size_t srcWidth,
    const RenderPixels &renderPixels, const RenderFill &renderFill)
{
	const auto *srcEnd = src + srcSize;

	// Skip the bottom clipped lines.
	std::int_fast16_t xOffset = 0;
	{
		const auto dstHeight = out.h();
		SkipSize skipSize = { 0, 0 };
		while (position.y >= dstHeight && src != srcEnd) {
			src = SkipRestOfCl2Line(src, static_cast<std::int_fast16_t>(srcWidth),
			    static_cast<std::int_fast16_t>(srcWidth - skipSize.xOffset), skipSize);
			position.y -= static_cast<int>(skipSize.wholeLines);
		}
		xOffset = skipSize.xOffset;
	}

	auto *dst = &out[position];
	const auto *dstBegin = out.begin();
	const auto dstPitch = out.pitch();
	while (src != srcEnd && dst >= dstBegin) {
		dst += xOffset;
		auto remainingWidth = static_cast<std::int_fast16_t>(srcWidth - xOffset);
		while (remainingWidth > 0) {
			auto v = static_cast<std::uint8_t>(*src++);
			if (IsCl2Opaque(v)) {
				if (IsCl2OpaqueFill(v)) {
					v = GetCl2OpaqueFillWidth(v);
					renderFill(dst, static_cast<uint8_t>(*src++), v);
				} else {
					v = GetCl2OpaquePixelsWidth(v);
					renderPixels(dst, reinterpret_cast<const std::uint8_t *>(src), v);
					src += v;
				}
			}
			dst += v;
			remainingWidth -= v;
		}
		dst -= dstPitch + srcWidth - remainingWidth;
		if (remainingWidth < 0) {
			const auto skipSize = GetSkipSize(-remainingWidth, static_cast<std::int_fast16_t>(srcWidth));
			xOffset = skipSize.xOffset;
			dst -= skipSize.wholeLines * dstPitch;
		} else {
			xOffset = 0;
		}
	}
}

/** Renders a CEL with both horizontal and vertical clipping to the output buffer. */
template <typename RenderPixels, typename RenderFill>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderCl2ClipXY( // NOLINT(readability-function-cognitive-complexity)
    const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize, std::size_t srcWidth, ClipX clipX,
    const RenderPixels &renderPixels, const RenderFill &renderFill)
{
	const auto *srcEnd = src + srcSize;

	// Skip the bottom clipped lines.
	std::int_fast16_t xOffset = 0;
	{
		const auto dstHeight = out.h();
		SkipSize skipSize = { 0, 0 };
		while (position.y >= dstHeight && src != srcEnd) {
			src = SkipRestOfCl2Line(src, static_cast<std::int_fast16_t>(srcWidth),
			    static_cast<std::int_fast16_t>(srcWidth - skipSize.xOffset), skipSize);
			position.y -= static_cast<int>(skipSize.wholeLines);
		}
		xOffset = skipSize.xOffset;
	}

	position.x += static_cast<int>(clipX.left);

	auto *dst = &out[position];
	const auto *dstBegin = out.begin();
	const auto dstPitch = out.pitch();
	while (src < srcEnd && dst >= dstBegin) {
		// Skip initial src if clipping on the left.
		// Handles overshoot, i.e. when the RLE segment goes into the unclipped area.
		std::int_fast16_t remainingWidth = clipX.width;
		std::int_fast16_t remainingLeftClip = clipX.left - xOffset;
		if (xOffset > clipX.left)
			dst += xOffset - clipX.left;
		while (remainingLeftClip > 0) {
			auto v = static_cast<std::uint8_t>(*src++);
			if (IsCl2Opaque(v)) {
				if (IsCl2OpaqueFill(v)) {
					v = GetCl2OpaqueFillWidth(v);
					if (v > remainingLeftClip) {
						const auto overshoot = v - remainingLeftClip;
						renderFill(dst, static_cast<uint8_t>(*src), overshoot);
						dst += overshoot;
					}
					++src;
				} else {
					v = GetCl2OpaquePixelsWidth(v);
					if (v > remainingLeftClip) {
						const auto overshoot = v - remainingLeftClip;
						renderPixels(dst, reinterpret_cast<const std::uint8_t *>(src + remainingLeftClip), overshoot);
						dst += overshoot;
					}
					src += v;
				}
			} else if (v > remainingLeftClip) {
				const auto overshoot = v - remainingLeftClip;
				dst += overshoot;
			}
			remainingLeftClip -= v;
		}
		assert(remainingLeftClip <= 0);
		remainingWidth += remainingLeftClip;

		// Draw the non-clipped segment
		while (remainingWidth > 0) {
			auto v = static_cast<std::uint8_t>(*src++);

			if (IsCl2Opaque(v)) {
				if (IsCl2OpaqueFill(v)) {
					v = GetCl2OpaqueFillWidth(v);
					renderFill(dst, static_cast<uint8_t>(*src++), std::min(remainingWidth, static_cast<std::int_fast16_t>(v)));
				} else {
					v = GetCl2OpaquePixelsWidth(v);
					renderPixels(dst, reinterpret_cast<const std::uint8_t *>(src), std::min(remainingWidth, static_cast<std::int_fast16_t>(v)));
					src += v;
				}
			}
			dst += v;
			remainingWidth -= v;
		}

		// Set dst x to its initial value (clipLeft.x)
		dst -= dstPitch + clipX.width - remainingWidth;

		// Skip the rest of src line if clipping on the right
		assert(remainingWidth <= 0);
		remainingWidth += clipX.right;
		if (remainingWidth > 0) {
			SkipSize skipSize;
			src = SkipRestOfCl2Line(src, static_cast<std::int_fast16_t>(srcWidth),
			    remainingWidth, skipSize);
			if (skipSize.wholeLines > 1)
				dst -= dstPitch * (skipSize.wholeLines - 1);
			remainingWidth = -skipSize.xOffset;
		}
		if (remainingWidth < 0) {
			const auto skipSize = GetSkipSize(-remainingWidth, static_cast<std::int_fast16_t>(srcWidth));
			xOffset = skipSize.xOffset;
			dst -= skipSize.wholeLines * dstPitch;
		} else {
			xOffset = 0;
		}
	}
}

template <typename RenderPixels, typename RenderFill>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderCl2(
    const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize, std::size_t srcWidth,
    const RenderPixels &renderPixels, const RenderFill &renderFill)
{
	const ClipX clipX = CalculateClipX(position.x, srcWidth, out);
	if (clipX.width <= 0)
		return;
	if (static_cast<std::size_t>(clipX.width) == srcWidth) {
		RenderCl2ClipY(out, position, src, srcSize, srcWidth, renderPixels, renderFill);
	} else {
		RenderCl2ClipXY(out, position, src, srcSize, srcWidth, clipX, renderPixels, renderFill);
	}
}

/**
 * @brief Blit CL2 sprite to the given buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CL2 pixel stream (run-length encoded)
 * @param nDataSize Size of CL2 in bytes
 * @param nWidth Width of sprite
 */
void Cl2BlitSafe(const CelOutputBuffer &out, int sx, int sy, const byte *pRLEBytes, int nDataSize, int nWidth)
{
	RenderCl2(
	    out, { sx, sy }, pRLEBytes, nDataSize, nWidth,
#ifndef DEBUG_RENDER_COLOR
	    [](std::uint8_t *dst, const std::uint8_t *src, std::size_t w) {
		    std::memcpy(dst, src, w);
	    },
	    [](std::uint8_t *dst, std::uint8_t color, std::size_t w) {
		    std::memset(dst, color, w);
	    }
#else
	    [](std::uint8_t *dst, [[maybe_unused]] const std::uint8_t *src, std::size_t w) {
		    std::memset(dst, DEBUG_RENDER_COLOR, w);
	    },
	    [](std::uint8_t *dst, [[maybe_unused]] std::uint8_t color, std::size_t w) {
		    std::memset(dst, DEBUG_RENDER_COLOR, w);
	    }
#endif
	);
}

/**
 * @brief Blit CL2 sprite, and apply lighting, to the given buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CL2 pixel stream (run-length encoded)
 * @param nDataSize Size of CL2 in bytes
 * @param nWidth With of CL2 sprite
 * @param pTable Light color table
 */
void Cl2BlitLightSafe(const CelOutputBuffer &out, int sx, int sy, const byte *pRLEBytes, int nDataSize, int nWidth, uint8_t *pTable)
{
	RenderCl2(
	    out, { sx, sy }, pRLEBytes, nDataSize, nWidth,
#ifndef DEBUG_RENDER_COLOR
	    [pTable](std::uint8_t *dst, const std::uint8_t *src, std::size_t w) {
		    while (w-- > 0)
			    *dst++ = pTable[static_cast<std::uint8_t>(*src++)];
	    },
	    [pTable](std::uint8_t *dst, std::uint8_t color, std::size_t w) {
		    std::memset(dst, pTable[color], w);
	    }
#else
	    [pTable](std::uint8_t *dst, [[maybe_unused]] const std::uint8_t *src, std::size_t w) {
		    std::memset(dst, pTable[DEBUG_RENDER_COLOR], w);
	    },
	    [pTable](std::uint8_t *dst, [[maybe_unused]] std::uint8_t color, std::size_t w) {
		    std::memset(dst, pTable[DEBUG_RENDER_COLOR], w);
	    }
#endif
	);
}

template <bool North, bool West, bool South, bool East>
void RenderOutlineForPixel(std::uint8_t *dst, int dstPitch, std::uint8_t color)
{
	if (North)
		dst[-dstPitch] = color;
	if (West)
		dst[-1] = color;
	if (East)
		dst[1] = color;
	if (South)
		dst[dstPitch] = color;
}

template <bool North, bool West, bool South, bool East>
void RenderOutlineForPixels(std::uint8_t *dst, int dstPitch, int width, std::uint8_t color)
{
	if (North)
		std::memset(dst - dstPitch, color, width);

	if (West && East)
		std::memset(dst - 1, color, width + 2);
	else if (West)
		std::memset(dst - 1, color, width);
	else if (East)
		std::memset(dst + 1, color, width);

	if (South)
		std::memset(dst + dstPitch, color, width);
}

template <bool North, bool West, bool South, bool East>
void RenderOutlineForPixel(std::uint8_t *dst, int dstPitch, std::uint8_t srcColor, std::uint8_t color)
{
	if (srcColor == 0)
		return;
	RenderOutlineForPixel<North, West, South, East>(dst, dstPitch, color);
}

template <bool North, bool West, bool South, bool East>
void RenderOutlineForPixels(std::uint8_t *dst, int dstPitch, int width, const std::uint8_t *src, std::uint8_t color)
{
	while (width-- > 0)
		RenderOutlineForPixel<North, West, South, East>(dst++, dstPitch, *src++, color);
}

template <bool Fill, bool North, bool West, bool South, bool East>
std::uint8_t *RenderCl2OutlinePixelsCheckFirstColumn(
    std::uint8_t *dst, int dstPitch, int dstX,
    const std::uint8_t *src, std::uint8_t width, std::uint8_t color)
{
	if (dstX == -1) {
		if (Fill) {
			RenderOutlineForPixel</*North=*/false, /*West=*/false, /*South=*/false, East>(
			    dst++, dstPitch, color);
		} else {
			RenderOutlineForPixel</*North=*/false, /*West=*/false, /*South=*/false, East>(
			    dst++, dstPitch, *src++, color);
		}
		--width;
	}
	if (width > 0) {
		if (Fill) {
			RenderOutlineForPixel<North, /*West=*/false, South, East>(dst++, dstPitch, color);
		} else {
			RenderOutlineForPixel<North, /*West=*/false, South, East>(dst++, dstPitch, *src++, color);
		}
		--width;
	}
	if (width > 0) {
		if (Fill) {
			RenderOutlineForPixels<North, West, South, East>(dst, dstPitch, width, color);
		} else {
			RenderOutlineForPixels<North, West, South, East>(dst, dstPitch, width, src, color);
		}
		dst += width;
	}
	return dst;
}

template <bool Fill, bool North, bool West, bool South, bool East>
std::uint8_t *RenderCl2OutlinePixelsCheckLastColumn(
    std::uint8_t *dst, int dstPitch, int dstX, int dstW,
    const std::uint8_t *src, std::uint8_t width, std::uint8_t color)
{
	const bool lastPixel = dstX < dstW && width >= 1;
	const bool oobPixel = dstX + width > dstW;
	const int numSpecialPixels = (lastPixel ? 1 : 0) + (oobPixel ? 1 : 0);
	if (width > numSpecialPixels) {
		width -= numSpecialPixels;
		if (Fill) {
			RenderOutlineForPixels<North, West, South, East>(dst, dstPitch, width, color);
		} else {
			RenderOutlineForPixels<North, West, South, East>(dst, dstPitch, width, src, color);
			src += width;
		}
		dst += width;
	}
	if (lastPixel) {
		if (Fill) {
			RenderOutlineForPixel<North, West, South, /*East=*/false>(dst++, dstPitch, color);
		} else {
			RenderOutlineForPixel<North, West, South, /*East=*/false>(dst++, dstPitch, *src++, color);
		}
	}
	if (oobPixel) {
		if (Fill) {
			RenderOutlineForPixel</*North=*/false, West, /*South=*/false, /*East=*/false>(dst++, dstPitch, color);
		} else {
			RenderOutlineForPixel</*North=*/false, West, /*South=*/false, /*East=*/false>(dst++, dstPitch, *src++, color);
		}
	}
	return dst;
}

template <bool Fill, bool North, bool West, bool South, bool East, bool CheckFirstColumn, bool CheckLastColumn>
std::uint8_t *RenderCl2OutlinePixels(
    std::uint8_t *dst, int dstPitch, int dstX, int dstW,
    const std::uint8_t *src, std::uint8_t width, std::uint8_t color)
{
	if (Fill && *src == 0)
		return dst + width;

	if (CheckFirstColumn && dstX <= 0) {
		return RenderCl2OutlinePixelsCheckFirstColumn<Fill, North, West, South, East>(
		    dst, dstPitch, dstX, src, width, color);
	}
	if (CheckLastColumn && dstX + width >= dstW) {
		return RenderCl2OutlinePixelsCheckLastColumn<Fill, North, West, South, East>(
		    dst, dstPitch, dstX, dstW, src, width, color);
	}
	if (Fill) {
		RenderOutlineForPixels<North, West, South, East>(dst, dstPitch, width, color);
	} else {
		RenderOutlineForPixels<North, West, South, East>(dst, dstPitch, width, src, color);
	}
	return dst + width;
}

template <bool North, bool West, bool South, bool East,
    bool ClipWidth = false, bool CheckFirstColumn = false, bool CheckLastColumn = false>
const byte *RenderCl2OutlineRowClipped( // NOLINT(readability-function-cognitive-complexity)
    const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcWidth,
    ClipX clipX, std::uint8_t color, SkipSize &skipSize)
{
	std::int_fast16_t remainingWidth = clipX.width;
	std::uint8_t v;

	auto *dst = &out[position];
	const auto dstPitch = out.pitch();

	const auto renderPixels = [&](bool fill, std::uint8_t w) {
		if (fill) {
			dst = RenderCl2OutlinePixels</*Fill=*/true, North, West, South, East, CheckFirstColumn, CheckLastColumn>(
			    dst, dstPitch, position.x, out.w(), reinterpret_cast<const std::uint8_t *>(src), w, color);
			++src;
		} else {
			dst = RenderCl2OutlinePixels</*Fill=*/false, North, West, South, East, CheckFirstColumn, CheckLastColumn>(
			    dst, dstPitch, position.x, out.w(), reinterpret_cast<const std::uint8_t *>(src), w, color);
			src += v;
		}
	};

	if (ClipWidth) {
		auto remainingLeftClip = clipX.left - skipSize.xOffset;
		if (skipSize.xOffset > clipX.left) {
			position.x += static_cast<int>(skipSize.xOffset - clipX.left);
			dst += skipSize.xOffset - clipX.left;
		}
		while (remainingLeftClip > 0) {
			v = static_cast<std::uint8_t>(*src++);
			if (IsCl2Opaque(v)) {
				const bool fill = IsCl2OpaqueFill(v);
				v = fill ? GetCl2OpaqueFillWidth(v) : GetCl2OpaquePixelsWidth(v);
				if (v > remainingLeftClip) {
					const std::uint8_t overshoot = v - remainingLeftClip;
					renderPixels(fill, overshoot);
					position.x += overshoot;
				} else {
					src += fill ? 1 : v;
				}
			} else {
				if (v > remainingLeftClip) {
					const std::uint8_t overshoot = v - remainingLeftClip;
					dst += overshoot;
					position.x += overshoot;
				}
			}
			remainingLeftClip -= v;
		}
		remainingWidth += remainingLeftClip;
	} else {
		position.x += static_cast<int>(skipSize.xOffset);
		dst += skipSize.xOffset;
		remainingWidth -= skipSize.xOffset;
	}

	while (remainingWidth > 0) {
		v = static_cast<std::uint8_t>(*src++);
		if (IsCl2Opaque(v)) {
			const bool fill = IsCl2OpaqueFill(v);
			v = fill ? GetCl2OpaqueFillWidth(v) : GetCl2OpaquePixelsWidth(v);
			renderPixels(fill, ClipWidth ? std::min(remainingWidth, static_cast<std::int_fast16_t>(v)) : v);
		} else {
			dst += v;
		}
		remainingWidth -= v;
		position.x += v;
	}

	if (ClipWidth) {
		remainingWidth += clipX.right;
		if (remainingWidth > 0) {
			src = SkipRestOfCl2Line(src, static_cast<std::int_fast16_t>(srcWidth),
			    remainingWidth, skipSize);
			if (skipSize.wholeLines > 1)
				dst -= dstPitch * (skipSize.wholeLines - 1);
			remainingWidth = -skipSize.xOffset;
		}
	}
	if (remainingWidth < 0) {
		skipSize = GetSkipSize(-remainingWidth, static_cast<std::int_fast16_t>(srcWidth));
		++skipSize.wholeLines;
	} else {
		skipSize.xOffset = 0;
		skipSize.wholeLines = 1;
	}

	return src;
}

void RenderCl2OutlineClippedY(const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize, // NOLINT(readability-function-cognitive-complexity)
    std::size_t srcWidth, std::uint8_t color)
{
	const auto *srcEnd = src + srcSize;

	// Skip the bottom clipped lines.
	const auto dstHeight = out.h();
	SkipSize skipSize = { 0, 0 };
	while (position.y >= dstHeight && src != srcEnd) {
		src = SkipRestOfCl2Line(src, static_cast<std::int_fast16_t>(srcWidth),
		    static_cast<std::int_fast16_t>(srcWidth - skipSize.xOffset), skipSize);
		position.y -= static_cast<int>(skipSize.wholeLines);
	}
	if (src == srcEnd)
		return;

	const ClipX clipX = { 0, 0, static_cast<decltype(ClipX {}.width)>(srcWidth) };

	if (position.y == dstHeight) {
		// After-bottom line - can only draw north.
		src = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/false, /*South=*/false, /*East=*/false>(
		    out, position, src, srcWidth, clipX, color, skipSize);
		position.y -= static_cast<int>(skipSize.wholeLines);
	}
	if (src == srcEnd)
		return;

	if (position.y + 1 == dstHeight) {
		// Bottom line - cannot draw south.
		src = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true>(
		    out, position, src, srcWidth, clipX, color, skipSize);
		position.y -= static_cast<int>(skipSize.wholeLines);
	}

	while (position.y > 0 && src != srcEnd) {
		src = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true>(
		    out, position, src, srcWidth, clipX, color, skipSize);
		position.y -= static_cast<int>(skipSize.wholeLines);
	}
	if (src == srcEnd)
		return;

	if (position.y == 0) {
		src = RenderCl2OutlineRowClipped</*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true>(
		    out, position, src, srcWidth, clipX, color, skipSize);
		position.y -= static_cast<int>(skipSize.wholeLines);
	}
	if (src == srcEnd)
		return;

	if (position.y == -1) {
		// Special case: the top of the sprite is 1px below the last line, render just the outline above.
		RenderCl2OutlineRowClipped</*North=*/false, /*West=*/false, /*South=*/true, /*East=*/false>(
		    out, position, src, srcWidth, clipX, color, skipSize);
	}
}

void RenderCl2OutlineClippedXY(const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize, // NOLINT(readability-function-cognitive-complexity)
    std::size_t srcWidth, std::uint8_t color)
{
	const auto *srcEnd = src + srcSize;

	// Skip the bottom clipped lines.
	const auto dstHeight = out.h();
	SkipSize skipSize = { 0, 0 };
	while (position.y >= dstHeight && src != srcEnd) {
		src = SkipRestOfCl2Line(src, static_cast<std::int_fast16_t>(srcWidth),
		    static_cast<std::int_fast16_t>(srcWidth - skipSize.xOffset), skipSize);
		position.y -= static_cast<int>(skipSize.wholeLines);
	}
	if (src == srcEnd)
		return;

	ClipX clipX = CalculateClipX(position.x, srcWidth, out);
	if (clipX.width < 0)
		return;
	if (clipX.left > 0) {
		--clipX.left, ++clipX.width;
	} else if (clipX.right > 0) {
		--clipX.right, ++clipX.width;
	}
	position.x += static_cast<int>(clipX.left);

	if (position.y == dstHeight) {
		// After-bottom line - can only draw north.
		src = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/false, /*South=*/false, /*East=*/false,
		    /*ClipWidth=*/true>(out, position, src, srcWidth, clipX, color, skipSize);
		position.y -= static_cast<int>(skipSize.wholeLines);
	}
	if (src == srcEnd)
		return;

	if (position.y + 1 == dstHeight) {
		// Bottom line - cannot draw south.
		if (position.x <= 0) {
			src = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/true, /*CheckLastColumn=*/false>(
			    out, position, src, srcWidth, clipX, color, skipSize);
		} else if (position.x + clipX.width >= out.w()) {
			src = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/false, /*CheckLastColumn=*/true>(
			    out, position, src, srcWidth, clipX, color, skipSize);
		} else {
			src = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true,
			    /*ClipWidth=*/true>(
			    out, position, src, srcWidth, clipX, color, skipSize);
		}
		position.y -= static_cast<int>(skipSize.wholeLines);
	}

	if (position.x <= 0) {
		while (position.y > 0 && src != srcEnd) {
			src = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/true, /*CheckLastColumn=*/false>(
			    out, position, src, srcWidth, clipX, color, skipSize);
			position.y -= static_cast<int>(skipSize.wholeLines);
		}
	} else if (position.x + clipX.width >= out.w()) {
		while (position.y > 0 && src != srcEnd) {
			src = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/false, /*CheckLastColumn=*/true>(
			    out, position, src, srcWidth, clipX, color, skipSize);
			position.y -= static_cast<int>(skipSize.wholeLines);
		}
	} else {
		while (position.y > 0 && src != srcEnd) {
			src = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true>(
			    out, position, src, srcWidth, clipX, color, skipSize);
			position.y -= static_cast<int>(skipSize.wholeLines);
		}
	}
	if (src == srcEnd)
		return;

	if (position.y == 0) {
		if (position.x <= 0) {
			src = RenderCl2OutlineRowClipped</*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/true, /*CheckLastColumn=*/false>(
			    out, position, src, srcWidth, clipX, color, skipSize);
		} else if (position.x + clipX.width >= out.w()) {
			src = RenderCl2OutlineRowClipped</*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/false, /*CheckLastColumn=*/true>(
			    out, position, src, srcWidth, clipX, color, skipSize);
		} else {
			src = RenderCl2OutlineRowClipped</*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true>(
			    out, position, src, srcWidth, clipX, color, skipSize);
		}
		position.y -= static_cast<int>(skipSize.wholeLines);
	}
	if (src == srcEnd)
		return;

	if (position.y == -1) {
		// Special case: the top of the sprite is 1px below the last line, render just the outline above.
		RenderCl2OutlineRowClipped</*North=*/false, /*West=*/false, /*South=*/true, /*East=*/false,
		    /*ClipWidth=*/true>(
		    out, position, src, srcWidth, clipX, color, skipSize);
	}
}

void RenderCl2Outline(const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize,
    std::size_t srcWidth, std::uint8_t color)
{
	if (position.x > 0 && position.x + static_cast<int>(srcWidth) < static_cast<int>(out.w())) {
		RenderCl2OutlineClippedY(out, position, src, srcSize, srcWidth, color);
	} else {
		RenderCl2OutlineClippedXY(out, position, src, srcSize, srcWidth, color);
	}
}

} // namespace

void Cl2ApplyTrans(byte *p, const std::array<uint8_t, 256> &ttbl, int nCel)
{
	assert(p != nullptr);

	for (int i = 1; i <= nCel; i++) {
		constexpr int FrameHeaderSize = 10;
		int nDataSize;
		byte *dst = CelGetFrame(p, i, &nDataSize) + FrameHeaderSize;
		nDataSize -= FrameHeaderSize;
		while (nDataSize > 0) {
			auto v = static_cast<std::uint8_t>(*dst++);
			nDataSize--;
			assert(nDataSize >= 0);
			if (!IsCl2Opaque(v))
				continue;
			if (IsCl2OpaqueFill(v)) {
				nDataSize--;
				assert(nDataSize >= 0);
				*dst = static_cast<byte>(ttbl[static_cast<std::uint8_t>(*dst)]);
				dst++;
			} else {
				v = GetCl2OpaquePixelsWidth(v);
				nDataSize -= v;
				assert(nDataSize >= 0);
				while (v-- > 0) {
					*dst = static_cast<byte>(ttbl[static_cast<std::uint8_t>(*dst)]);
					dst++;
				}
			}
		}
	}
}

void Cl2Draw(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame)
{
	assert(frame > 0);

	int nDataSize;
	const byte *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);

	Cl2BlitSafe(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame));
}

void Cl2DrawOutline(const CelOutputBuffer &out, uint8_t col, int sx, int sy, const CelSprite &cel, int frame)
{
	assert(frame > 0);

	int nDataSize;
	const byte *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);

	RenderCl2Outline(out, { sx, sy }, pRLEBytes, nDataSize, cel.Width(frame), col);
}

void Cl2DrawLightTbl(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame, char light)
{
	assert(frame > 0);

	int nDataSize;
	const byte *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);
	Cl2BlitLightSafe(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame), GetLightTable(light));
}

void Cl2DrawLight(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame)
{
	assert(frame > 0);

	int nDataSize;
	const byte *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);

	if (light_table_index != 0)
		Cl2BlitLightSafe(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame), &pLightTbl[light_table_index * 256]);
	else
		Cl2BlitSafe(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame));
}

} // namespace devilution
