/**
 * @file clx_render.cpp
 *
 * CL2 rendering.
 */
#include "clx_render.hpp"

#include <algorithm>
#include <cstdint>

#include "engine/render/blit_impl.hpp"
#include "engine/render/scrollrt.h"
#include "utils/attributes.h"
#include "utils/clx_decode.hpp"

#ifdef DEBUG_CLX
#include <fmt/format.h>

#include "utils/str_cat.hpp"
#endif

namespace devilution {
namespace {

/**
 * CL2 is similar to CEL, with the following differences:
 *
 * 1. Transparent runs can cross line boundaries.
 * 2. Control bytes are different, and the [0x80, 0xBE] control byte range
 *    indicates a fill-N command.
 */

struct ClipX {
	int_fast16_t left;
	int_fast16_t right;
	int_fast16_t width;
};

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT ClipX CalculateClipX(int_fast16_t x, std::size_t w, const Surface &out)
{
	ClipX clip;
	clip.left = static_cast<int_fast16_t>(x < 0 ? -x : 0);
	clip.right = static_cast<int_fast16_t>(static_cast<int_fast16_t>(x + w) > out.w() ? x + w - out.w() : 0);
	clip.width = static_cast<int_fast16_t>(w - clip.left - clip.right);
	return clip;
}

// Source data for rendering backwards: first line of input -> last line of output.
struct RenderSrc {
	const uint8_t *begin;
	const uint8_t *end;
	uint_fast16_t width;
};

struct SkipSize {
	int_fast16_t wholeLines;
	int_fast16_t xOffset;
};

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT SkipSize GetSkipSize(int_fast16_t remainingWidth, int_fast16_t srcWidth)
{
	if (remainingWidth < 0) {
		// If `remainingWidth` is negative, `-remainingWidth` is the overrun.
		const int_fast16_t overrunLines = -remainingWidth / srcWidth;
		return {
			static_cast<int_fast16_t>(1 + overrunLines),
			static_cast<int_fast16_t>(-remainingWidth - srcWidth * overrunLines)
		};
	}
	// If `remainingWidth` is non-negative, then it is 0, meaning we drew a whole line.
	return { 1, 0 };
}

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT const uint8_t *SkipRestOfLineWithOverrun(
    const uint8_t *src, int_fast16_t srcWidth, SkipSize &skipSize)
{
	int_fast16_t remainingWidth = srcWidth - skipSize.xOffset;
	while (remainingWidth > 0) {
		const BlitCommand cmd = ClxGetBlitCommand(src);
		src = cmd.srcEnd;
		remainingWidth -= cmd.length;
	}
	skipSize = GetSkipSize(remainingWidth, srcWidth);
	return src;
}

// Returns the horizontal overrun.
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT int_fast16_t SkipLinesForRenderBackwardsWithOverrun(
    Point &position, RenderSrc &src, int_fast16_t dstHeight)
{
	SkipSize skipSize { 0, 0 };
	while (position.y >= dstHeight && src.begin != src.end) {
		src.begin = SkipRestOfLineWithOverrun(
		    src.begin, static_cast<int_fast16_t>(src.width), skipSize);
		position.y -= static_cast<int>(skipSize.wholeLines);
	}
	return skipSize.xOffset;
}

template <typename BlitFn>
void DoRenderBackwardsClipY(
    const Surface &out, Point position, RenderSrc src, BlitFn &&blitFn)
{
	// Skip the bottom clipped lines.
	int_fast16_t xOffset = SkipLinesForRenderBackwardsWithOverrun(position, src, out.h());
	if (src.begin >= src.end)
		return;

	auto *dst = &out[position];
	const auto *dstBegin = out.begin();
	const int dstPitch = out.pitch();
	while (src.begin != src.end && dst >= dstBegin) {
		auto remainingWidth = static_cast<int_fast16_t>(src.width) - xOffset;
		dst += xOffset;
		while (remainingWidth > 0) {
			BlitCommand cmd = ClxGetBlitCommand(src.begin);
			blitFn(cmd, dst, src.begin + 1);
			src.begin = cmd.srcEnd;
			dst += cmd.length;
			remainingWidth -= cmd.length;
		}

		const SkipSize skipSize = GetSkipSize(remainingWidth, static_cast<int_fast16_t>(src.width));
		xOffset = skipSize.xOffset;
		dst -= skipSize.wholeLines * dstPitch + src.width - remainingWidth;
	}
}

template <typename BlitFn>
void DoRenderBackwardsClipXY(
    const Surface &out, Point position, RenderSrc src, ClipX clipX, BlitFn &&blitFn)
{
	// Skip the bottom clipped lines.
	int_fast16_t xOffset = SkipLinesForRenderBackwardsWithOverrun(position, src, out.h());
	if (src.begin >= src.end)
		return;

	position.x += static_cast<int>(clipX.left);
	auto *dst = &out[position];
	const auto *dstBegin = out.begin();
	const int dstPitch = out.pitch();

	while (src.begin != src.end && dst >= dstBegin) {
		// Skip initial src if clipping on the left.
		// Handles overshoot, i.e. when the RLE segment goes into the unclipped area.
		int_fast16_t remainingWidth = clipX.width;
		int_fast16_t remainingLeftClip = clipX.left - xOffset;
		if (remainingLeftClip < 0) {
			dst += std::min<unsigned>(remainingWidth, -remainingLeftClip);
			remainingWidth += remainingLeftClip;
		}
		while (remainingLeftClip > 0) {
			BlitCommand cmd = ClxGetBlitCommand(src.begin);
			if (static_cast<int_fast16_t>(cmd.length) > remainingLeftClip) {
				const auto overshoot = static_cast<int>(cmd.length - remainingLeftClip);
				cmd.length = std::min<unsigned>(remainingWidth, overshoot);
				blitFn(cmd, dst, src.begin + 1 + remainingLeftClip);
				dst += cmd.length;
				remainingWidth -= overshoot;
				src.begin = cmd.srcEnd;
				break;
			}
			src.begin = cmd.srcEnd;
			remainingLeftClip -= cmd.length;
		}
		while (remainingWidth > 0) {
			BlitCommand cmd = ClxGetBlitCommand(src.begin);
			const unsigned unclippedLength = cmd.length;
			cmd.length = std::min<unsigned>(remainingWidth, cmd.length);
			blitFn(cmd, dst, src.begin + 1);
			src.begin = cmd.srcEnd;
			dst += cmd.length;
			remainingWidth -= unclippedLength; // result can be negative
		}

		// `remainingWidth` can be negative, in which case it is the amount of pixels
		// that the source has overran the line.
		remainingWidth += clipX.right;
		SkipSize skipSize;
		if (remainingWidth > 0) {
			skipSize.xOffset = static_cast<int_fast16_t>(src.width) - remainingWidth;
			src.begin = SkipRestOfLineWithOverrun(
			    src.begin, static_cast<int_fast16_t>(src.width), skipSize);
		} else {
			skipSize = GetSkipSize(remainingWidth, static_cast<int_fast16_t>(src.width));
		}
		xOffset = skipSize.xOffset;
		dst -= dstPitch * skipSize.wholeLines + clipX.width;
	}
}

template <typename BlitFn>
void DoRenderBackwards(
    const Surface &out, Point position, const uint8_t *src, size_t srcSize,
    unsigned srcWidth, unsigned srcHeight, BlitFn &&blitFn)
{
	if (position.y < 0 || position.y + 1 >= static_cast<int>(out.h() + srcHeight))
		return;
	const ClipX clipX = CalculateClipX(position.x, srcWidth, out);
	if (clipX.width <= 0)
		return;
	RenderSrc srcForBackwards { src, src + srcSize, static_cast<uint_fast16_t>(srcWidth) };
	if (static_cast<std::size_t>(clipX.width) == srcWidth) {
		DoRenderBackwardsClipY(
		    out, position, srcForBackwards, std::forward<BlitFn>(blitFn));
	} else {
		DoRenderBackwardsClipXY(
		    out, position, srcForBackwards, clipX, std::forward<BlitFn>(blitFn));
	}
}

template <bool North, bool West, bool South, bool East>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderOutlineForPixel(uint8_t *dst, int dstPitch, uint8_t color)
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

template <bool North, bool West, bool South, bool East, bool SkipColorIndexZero = true>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderOutlineForPixel(uint8_t *dst, int dstPitch, uint8_t srcColor, uint8_t color)
{
	if (SkipColorIndexZero && srcColor == 0)
		return;
	RenderOutlineForPixel<North, West, South, East>(dst, dstPitch, color);
}

template <bool North, bool West, bool South, bool East>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderOutlineForPixels(uint8_t *dst, int dstPitch, int width, uint8_t color)
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

template <bool North, bool West, bool South, bool East, bool SkipColorIndexZero>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderOutlineForPixels(uint8_t *dst, int dstPitch, int width, const uint8_t *src, uint8_t color)
{
	if (SkipColorIndexZero) {
		while (width-- > 0)
			RenderOutlineForPixel<North, West, South, East>(dst++, dstPitch, *src++, color);
	} else {
		RenderOutlineForPixels<North, West, South, East>(dst, dstPitch, width, color);
	}
}

template <bool Fill, bool North, bool West, bool South, bool East, bool SkipColorIndexZero>
void RenderClxOutlinePixelsCheckFirstColumn(
    uint8_t *dst, int dstPitch, int dstX,
    const uint8_t *src, uint8_t width, uint8_t color)
{
	if (dstX == -1) {
		if (Fill) {
			RenderOutlineForPixel</*North=*/false, /*West=*/false, /*South=*/false, East>(
			    dst++, dstPitch, color);
		} else {
			RenderOutlineForPixel</*North=*/false, /*West=*/false, /*South=*/false, East, SkipColorIndexZero>(
			    dst++, dstPitch, *src++, color);
		}
		--width;
	}
	if (width > 0) {
		if (Fill) {
			RenderOutlineForPixel<North, /*West=*/false, South, East>(dst++, dstPitch, color);
		} else {
			RenderOutlineForPixel<North, /*West=*/false, South, East, SkipColorIndexZero>(dst++, dstPitch, *src++, color);
		}
		--width;
	}
	if (width > 0) {
		if (Fill) {
			RenderOutlineForPixels<North, West, South, East>(dst, dstPitch, width, color);
		} else {
			RenderOutlineForPixels<North, West, South, East, SkipColorIndexZero>(dst, dstPitch, width, src, color);
		}
	}
}

template <bool Fill, bool North, bool West, bool South, bool East, bool SkipColorIndexZero>
void RenderClxOutlinePixelsCheckLastColumn(
    uint8_t *dst, int dstPitch, int dstX, int dstW,
    const uint8_t *src, uint8_t width, uint8_t color)
{
	const bool lastPixel = dstX != dstW;
	const bool oobPixel = dstX + width == dstW + 1;
	const int numSpecialPixels = (lastPixel ? 1 : 0) + (oobPixel ? 1 : 0);
	if (width > numSpecialPixels) {
		width -= numSpecialPixels;
		if (Fill) {
			RenderOutlineForPixels<North, West, South, East>(dst, dstPitch, width, color);
		} else {
			RenderOutlineForPixels<North, West, South, East, SkipColorIndexZero>(dst, dstPitch, width, src, color);
			src += width;
		}
		dst += width;
	}
	if (lastPixel) {
		if (Fill) {
			RenderOutlineForPixel<North, West, South, /*East=*/false>(dst++, dstPitch, color);
		} else {
			RenderOutlineForPixel<North, West, South, /*East=*/false, SkipColorIndexZero>(dst++, dstPitch, *src++, color);
		}
	}
	if (oobPixel) {
		if (Fill) {
			RenderOutlineForPixel</*North=*/false, West, /*South=*/false, /*East=*/false>(dst, dstPitch, color);
		} else {
			RenderOutlineForPixel</*North=*/false, West, /*South=*/false, /*East=*/false, SkipColorIndexZero>(dst, dstPitch, *src, color);
		}
	}
}

template <bool Fill, bool North, bool West, bool South, bool East, bool SkipColorIndexZero, bool CheckFirstColumn, bool CheckLastColumn>
void RenderClxOutlinePixels(
    uint8_t *dst, int dstPitch, int dstX, int dstW,
    const uint8_t *src, uint8_t width, uint8_t color)
{
	if (SkipColorIndexZero && Fill && *src == 0)
		return;

	if (CheckFirstColumn && dstX <= 0) {
		RenderClxOutlinePixelsCheckFirstColumn<Fill, North, West, South, East, SkipColorIndexZero>(
		    dst, dstPitch, dstX, src, width, color);
	} else if (CheckLastColumn && dstX + width >= dstW) {
		RenderClxOutlinePixelsCheckLastColumn<Fill, North, West, South, East, SkipColorIndexZero>(
		    dst, dstPitch, dstX, dstW, src, width, color);
	} else if (Fill) {
		RenderOutlineForPixels<North, West, South, East>(dst, dstPitch, width, color);
	} else {
		RenderOutlineForPixels<North, West, South, East, SkipColorIndexZero>(dst, dstPitch, width, src, color);
	}
}

template <bool North, bool West, bool South, bool East, bool SkipColorIndexZero,
    bool ClipWidth = false, bool CheckFirstColumn = false, bool CheckLastColumn = false>
const uint8_t *RenderClxOutlineRowClipped( // NOLINT(readability-function-cognitive-complexity)
    const Surface &out, Point position, const uint8_t *src, std::size_t srcWidth,
    ClipX clipX, uint8_t color, SkipSize &skipSize)
{
	int_fast16_t remainingWidth = clipX.width;
	uint8_t v;

	auto *dst = &out[position];
	const auto dstPitch = out.pitch();

	const auto renderPixels = [&](bool fill, uint8_t w) {
		if (fill) {
			RenderClxOutlinePixels</*Fill=*/true, North, West, South, East, SkipColorIndexZero, CheckFirstColumn, CheckLastColumn>(
			    dst, dstPitch, position.x, out.w(), src, w, color);
			++src;
		} else {
			RenderClxOutlinePixels</*Fill=*/false, North, West, South, East, SkipColorIndexZero, CheckFirstColumn, CheckLastColumn>(
			    dst, dstPitch, position.x, out.w(), src, w, color);
			src += v;
		}
		dst += w;
	};

	if (ClipWidth) {
		auto remainingLeftClip = clipX.left - skipSize.xOffset;
		if (skipSize.xOffset > clipX.left) {
			position.x += static_cast<int>(skipSize.xOffset - clipX.left);
			dst += skipSize.xOffset - clipX.left;
		}
		while (remainingLeftClip > 0) {
			v = static_cast<uint8_t>(*src++);
			if (IsClxOpaque(v)) {
				const bool fill = IsClxOpaqueFill(v);
				v = fill ? GetClxOpaqueFillWidth(v) : GetClxOpaquePixelsWidth(v);
				if (v > remainingLeftClip) {
					const uint8_t overshoot = v - remainingLeftClip;
					renderPixels(fill, overshoot);
					position.x += overshoot;
				} else {
					src += fill ? 1 : v;
				}
			} else {
				if (v > remainingLeftClip) {
					const uint8_t overshoot = v - remainingLeftClip;
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
		v = static_cast<uint8_t>(*src++);
		if (IsClxOpaque(v)) {
			const bool fill = IsClxOpaqueFill(v);
			v = fill ? GetClxOpaqueFillWidth(v) : GetClxOpaquePixelsWidth(v);
			renderPixels(fill, ClipWidth ? std::min(remainingWidth, static_cast<int_fast16_t>(v)) : v);
		} else {
			dst += v;
		}
		remainingWidth -= v;
		position.x += v;
	}

	if (ClipWidth) {
		remainingWidth += clipX.right;
		if (remainingWidth > 0) {
			skipSize.xOffset = static_cast<int_fast16_t>(srcWidth) - remainingWidth;
			return SkipRestOfLineWithOverrun(src, static_cast<int_fast16_t>(srcWidth), skipSize);
		}
	}
	skipSize = GetSkipSize(remainingWidth, srcWidth);

	return src;
}

template <bool SkipColorIndexZero>
void RenderClxOutlineClippedY(const Surface &out, Point position, RenderSrc src, // NOLINT(readability-function-cognitive-complexity)
    uint8_t color)
{
	// Skip the bottom clipped lines.
	const int dstHeight = out.h();
	SkipSize skipSize = { 0, SkipLinesForRenderBackwardsWithOverrun(position, src, dstHeight) };
	if (src.begin == src.end)
		return;

	const ClipX clipX = { 0, 0, static_cast<decltype(ClipX {}.width)>(src.width) };

	if (position.y == dstHeight) {
		// After-bottom line - can only draw north.
		src.begin = RenderClxOutlineRowClipped</*North=*/true, /*West=*/false, /*South=*/false, /*East=*/false, SkipColorIndexZero>(
		    out, position, src.begin, src.width, clipX, color, skipSize);
		position.y -= static_cast<int>(skipSize.wholeLines);
	}
	if (src.begin == src.end)
		return;

	if (position.y + 1 == dstHeight) {
		// Bottom line - cannot draw south.
		src.begin = RenderClxOutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true, SkipColorIndexZero>(
		    out, position, src.begin, src.width, clipX, color, skipSize);
		position.y -= static_cast<int>(skipSize.wholeLines);
	}

	while (position.y > 0 && src.begin != src.end) {
		src.begin = RenderClxOutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true, SkipColorIndexZero>(
		    out, position, src.begin, src.width, clipX, color, skipSize);
		position.y -= static_cast<int>(skipSize.wholeLines);
	}
	if (src.begin == src.end)
		return;

	if (position.y == 0) {
		src.begin = RenderClxOutlineRowClipped</*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true, SkipColorIndexZero>(
		    out, position, src.begin, src.width, clipX, color, skipSize);
		position.y -= static_cast<int>(skipSize.wholeLines);
	}
	if (src.begin == src.end)
		return;

	if (position.y == -1) {
		// Special case: the top of the sprite is 1px below the last line, render just the outline above.
		RenderClxOutlineRowClipped</*North=*/false, /*West=*/false, /*South=*/true, /*East=*/false, SkipColorIndexZero>(
		    out, position, src.begin, src.width, clipX, color, skipSize);
	}
}

template <bool SkipColorIndexZero>
void RenderClxOutlineClippedXY(const Surface &out, Point position, RenderSrc src, // NOLINT(readability-function-cognitive-complexity)
    uint8_t color)
{
	// Skip the bottom clipped lines.
	const int dstHeight = out.h();
	SkipSize skipSize = { 0, SkipLinesForRenderBackwardsWithOverrun(position, src, dstHeight) };
	if (src.begin == src.end)
		return;

	ClipX clipX = CalculateClipX(position.x, src.width, out);
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
		if (position.x <= 0) {
			src.begin = RenderClxOutlineRowClipped</*North=*/true, /*West=*/false, /*South=*/false, /*East=*/false, SkipColorIndexZero,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/true, /*CheckLastColumn=*/false>(out, position, src.begin, src.width, clipX, color, skipSize);
		} else if (position.x + clipX.width >= out.w()) {
			src.begin = RenderClxOutlineRowClipped</*North=*/true, /*West=*/false, /*South=*/false, /*East=*/false, SkipColorIndexZero,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/false, /*CheckLastColumn=*/true>(out, position, src.begin, src.width, clipX, color, skipSize);
		} else {
			src.begin = RenderClxOutlineRowClipped</*North=*/true, /*West=*/false, /*South=*/false, /*East=*/false, SkipColorIndexZero,
			    /*ClipWidth=*/true>(out, position, src.begin, src.width, clipX, color, skipSize);
		}
		position.y -= static_cast<int>(skipSize.wholeLines);
	}
	if (src.begin == src.end)
		return;

	if (position.y + 1 == dstHeight) {
		// Bottom line - cannot draw south.
		if (position.x <= 0) {
			src.begin = RenderClxOutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true, SkipColorIndexZero,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/true, /*CheckLastColumn=*/false>(
			    out, position, src.begin, src.width, clipX, color, skipSize);
		} else if (position.x + clipX.width >= out.w()) {
			src.begin = RenderClxOutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true, SkipColorIndexZero,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/false, /*CheckLastColumn=*/true>(
			    out, position, src.begin, src.width, clipX, color, skipSize);
		} else {
			src.begin = RenderClxOutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true, SkipColorIndexZero,
			    /*ClipWidth=*/true>(
			    out, position, src.begin, src.width, clipX, color, skipSize);
		}
		position.y -= static_cast<int>(skipSize.wholeLines);
	}

	if (position.x <= 0) {
		while (position.y > 0 && src.begin != src.end) {
			src.begin = RenderClxOutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true, SkipColorIndexZero,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/true, /*CheckLastColumn=*/false>(
			    out, position, src.begin, src.width, clipX, color, skipSize);
			position.y -= static_cast<int>(skipSize.wholeLines);
		}
	} else if (position.x + clipX.width >= out.w()) {
		while (position.y > 0 && src.begin != src.end) {
			src.begin = RenderClxOutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true, SkipColorIndexZero,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/false, /*CheckLastColumn=*/true>(
			    out, position, src.begin, src.width, clipX, color, skipSize);
			position.y -= static_cast<int>(skipSize.wholeLines);
		}
	} else {
		while (position.y > 0 && src.begin != src.end) {
			src.begin = RenderClxOutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true, SkipColorIndexZero,
			    /*ClipWidth=*/true>(
			    out, position, src.begin, src.width, clipX, color, skipSize);
			position.y -= static_cast<int>(skipSize.wholeLines);
		}
	}
	if (src.begin == src.end)
		return;

	if (position.y == 0) {
		if (position.x <= 0) {
			src.begin = RenderClxOutlineRowClipped</*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true, SkipColorIndexZero,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/true, /*CheckLastColumn=*/false>(
			    out, position, src.begin, src.width, clipX, color, skipSize);
		} else if (position.x + clipX.width >= out.w()) {
			src.begin = RenderClxOutlineRowClipped</*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true, SkipColorIndexZero,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/false, /*CheckLastColumn=*/true>(
			    out, position, src.begin, src.width, clipX, color, skipSize);
		} else {
			src.begin = RenderClxOutlineRowClipped</*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true, SkipColorIndexZero,
			    /*ClipWidth=*/true>(
			    out, position, src.begin, src.width, clipX, color, skipSize);
		}
		position.y -= static_cast<int>(skipSize.wholeLines);
	}
	if (src.begin == src.end)
		return;

	if (position.y == -1) {
		// Before-top line - can only draw south.
		if (position.x <= 0) {
			src.begin = RenderClxOutlineRowClipped</*North=*/false, /*West=*/false, /*South=*/true, /*East=*/false, SkipColorIndexZero,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/true, /*CheckLastColumn=*/false>(out, position, src.begin, src.width, clipX, color, skipSize);
		} else if (position.x + clipX.width >= out.w()) {
			src.begin = RenderClxOutlineRowClipped</*North=*/false, /*West=*/false, /*South=*/true, /*East=*/false, SkipColorIndexZero,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/false, /*CheckLastColumn=*/true>(out, position, src.begin, src.width, clipX, color, skipSize);
		} else {
			src.begin = RenderClxOutlineRowClipped</*North=*/false, /*West=*/false, /*South=*/true, /*East=*/false, SkipColorIndexZero,
			    /*ClipWidth=*/true>(out, position, src.begin, src.width, clipX, color, skipSize);
		}
	}
}

template <bool SkipColorIndexZero>
void RenderClxOutline(const Surface &out, Point position, const uint8_t *src, std::size_t srcSize,
    std::size_t srcWidth, uint8_t color)
{
	RenderSrc srcForBackwards { src, src + srcSize, static_cast<uint_fast16_t>(srcWidth) };
	if (position.x > 0 && position.x + static_cast<int>(srcWidth) < static_cast<int>(out.w())) {
		RenderClxOutlineClippedY<SkipColorIndexZero>(out, position, srcForBackwards, color);
	} else {
		RenderClxOutlineClippedXY<SkipColorIndexZero>(out, position, srcForBackwards, color);
	}
}

void ClxApplyTrans(ClxSprite sprite, const uint8_t *trn)
{
	// A bit of a hack but this is the only place in the code where we need mutable sprites.
	auto *dst = const_cast<uint8_t *>(sprite.pixelData());
	uint16_t remaining = sprite.pixelDataSize();
	while (remaining != 0) {
		uint8_t val = *dst++;
		--remaining;
		if (!IsClxOpaque(val))
			continue;
		if (IsClxOpaqueFill(val)) {
			--remaining;
			*dst = trn[*dst];
			dst++;
		} else {
			val = GetClxOpaquePixelsWidth(val);
			remaining -= val;
			while (val-- > 0) {
				*dst = trn[*dst];
				dst++;
			}
		}
	}
}

} // namespace

void ClxApplyTrans(ClxSpriteList list, const uint8_t *trn)
{
	for (ClxSprite sprite : list) {
		ClxApplyTrans(sprite, trn);
	}
}

void ClxApplyTrans(ClxSpriteSheet sheet, const uint8_t *trn)
{
	for (ClxSpriteList list : sheet) {
		ClxApplyTrans(list, trn);
	}
}

std::pair<int, int> ClxMeasureSolidHorizontalBounds(ClxSprite clx)
{
	const uint8_t *src = clx.pixelData();
	const uint8_t *end = src + clx.pixelDataSize();
	const uint16_t width = clx.width();

	int xBegin = width;
	int xEnd = 0;
	int xCur = 0;
	while (src < end) {
		while (xCur < width) {
			auto val = *src++;
			if (!IsClxOpaque(val)) {
				xCur += val;
				continue;
			}
			if (IsClxOpaqueFill(val)) {
				val = GetClxOpaqueFillWidth(val);
				++src;
			} else {
				val = GetClxOpaquePixelsWidth(val);
				src += val;
			}
			xBegin = std::min(xBegin, xCur);
			xCur += val;
			xEnd = std::max(xEnd, xCur);
		}
		while (xCur >= width)
			xCur -= width;
		if (xBegin == 0 && xEnd == width)
			break;
	}
	return { xBegin, xEnd };
}

#ifdef DEBUG_CLX
std::string ClxDescribe(ClxSprite clx)
{
	std::string out = StrCat(
	    "CLX sprite: ", clx.width(), "x", clx.height(), " pixelDataSize=", clx.pixelDataSize(),
	    "b\n\n"
	    "command | width | bytes | color(s)\n"
	    "--------|------:|------:|---------\n");
	const uint8_t *src = clx.pixelData();
	const uint8_t *end = src + clx.pixelDataSize();
	while (src < end) {
		BlitCommand cmd = ClxGetBlitCommand(src);
		switch (cmd.type) {
		case BlitType::Transparent:
			out.append(fmt::format("Transp. | {:>5} | {:>5} |\n", cmd.length, cmd.srcEnd - src));
			break;
		case BlitType::Fill:
			out.append(fmt::format("Fill    | {:>5} | {:>5} | {}\n", cmd.length, cmd.srcEnd - src, cmd.color));
			break;
		case BlitType::Pixels:
			out.append(fmt::format("Pixels  | {:>5} | {:>5} | {}\n", cmd.length, cmd.srcEnd - src, fmt::join(src + 1, src + 1 + cmd.length, " ")));
			break;
		}
		src = cmd.srcEnd;
	}
	return out;
}
#endif // DEBUG_CLX

void ClxDraw(const Surface &out, Point position, ClxSprite clx)
{
	DoRenderBackwards(out, position, clx.pixelData(), clx.pixelDataSize(), clx.width(), clx.height(), BlitDirect {});
}

void ClxDrawTRN(const Surface &out, Point position, ClxSprite clx, const uint8_t *trn)
{
	DoRenderBackwards(out, position, clx.pixelData(), clx.pixelDataSize(), clx.width(), clx.height(), BlitWithMap { trn });
}

void ClxDrawBlendedTRN(const Surface &out, Point position, ClxSprite clx, const uint8_t *trn)
{
	DoRenderBackwards(out, position, clx.pixelData(), clx.pixelDataSize(), clx.width(), clx.height(), BlitBlendedWithMap { trn });
}

void ClxDrawOutline(const Surface &out, uint8_t col, Point position, ClxSprite clx)
{
	RenderClxOutline</*SkipColorIndexZero=*/false>(out, position, clx.pixelData(), clx.pixelDataSize(), clx.width(), col);
}

void ClxDrawOutlineSkipColorZero(const Surface &out, uint8_t col, Point position, ClxSprite clx)
{
	RenderClxOutline</*SkipColorIndexZero=*/true>(out, position, clx.pixelData(), clx.pixelDataSize(), clx.width(), col);
}

} // namespace devilution
