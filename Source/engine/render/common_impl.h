/**
 * @file common_impl.h
 *
 * Common code for implementing various renderers.
 */
#pragma once

#include <cstddef>
#include <cstdint>

#include "engine.h"
#include "engine/point.hpp"
#include "engine/render/blit_impl.hpp"
#include "lighting.h"
#include "utils/attributes.h"

namespace devilution {

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
struct RenderSrcBackwards {
	const uint8_t *begin;
	const uint8_t *end;
	uint_fast16_t width;
};

struct SkipSize {
	int_fast16_t wholeLines;
	int_fast16_t xOffset;
};
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT SkipSize GetSkipSize(int_fast16_t overrun, int_fast16_t srcWidth)
{
	SkipSize result;
	result.wholeLines = overrun / srcWidth;
	result.xOffset = overrun - srcWidth * result.wholeLines;
	return result;
}

using GetBlitCommandFn = BlitCommand (*)(const uint8_t *src);

template <GetBlitCommandFn GetBlitCommand>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT const uint8_t *SkipRestOfLineWithOverrun(
    const uint8_t *src, int_fast16_t srcWidth, SkipSize &skipSize)
{
	int_fast16_t remainingWidth = srcWidth - skipSize.xOffset;
	while (remainingWidth > 0) {
		const BlitCommand cmd = GetBlitCommand(src);
		src = cmd.srcEnd;
		remainingWidth -= cmd.length;
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

// Returns the horizontal overrun.
template <GetBlitCommandFn GetBlitCommand>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT int_fast16_t SkipLinesForRenderBackwardsWithOverrun(
    Point &position, RenderSrcBackwards &src, int_fast16_t dstHeight)
{
	SkipSize skipSize = { 0, 0 };
	while (position.y >= dstHeight && src.begin != src.end) {
		src.begin = SkipRestOfLineWithOverrun<GetBlitCommand>(
		    src.begin, static_cast<int_fast16_t>(src.width), skipSize);
		position.y -= static_cast<int>(skipSize.wholeLines);
	}
	return skipSize.xOffset;
}

template <GetBlitCommandFn GetBlitCommand, typename BlitFn>
void DoRenderBackwardsClipY(
    const Surface &out, Point position, RenderSrcBackwards src, BlitFn &&blitFn)
{
	// Skip the bottom clipped lines.
	int_fast16_t xOffset = SkipLinesForRenderBackwardsWithOverrun<GetBlitCommand>(position, src, out.h());
	if (src.begin >= src.end)
		return;

	auto *dst = &out[position];
	const auto *dstBegin = out.begin();
	const int dstPitch = out.pitch();
	while (src.begin != src.end && dst >= dstBegin) {
		auto remainingWidth = static_cast<int_fast16_t>(src.width) - xOffset;
		dst += xOffset;
		while (remainingWidth > 0) {
			BlitCommand cmd = GetBlitCommand(src.begin);
			blitFn(cmd, dst, src.begin + 1);
			src.begin = cmd.srcEnd;
			dst += cmd.length;
			remainingWidth -= cmd.length;
		}
		dst -= dstPitch + src.width - remainingWidth;

		if (remainingWidth < 0) {
			const auto skipSize = GetSkipSize(-remainingWidth, static_cast<int_fast16_t>(src.width));
			xOffset = skipSize.xOffset;
			dst -= skipSize.wholeLines * dstPitch;
		} else {
			xOffset = 0;
		}
	}
}

template <GetBlitCommandFn GetBlitCommand, typename BlitFn>
void DoRenderBackwardsClipXY(
    const Surface &out, Point position, RenderSrcBackwards src, ClipX clipX, BlitFn &&blitFn)
{
	// Skip the bottom clipped lines.
	int_fast16_t xOffset = SkipLinesForRenderBackwardsWithOverrun<GetBlitCommand>(position, src, out.h());
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
			BlitCommand cmd = GetBlitCommand(src.begin);
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
			BlitCommand cmd = GetBlitCommand(src.begin);
			const unsigned unclippedLength = cmd.length;
			cmd.length = std::min<unsigned>(remainingWidth, cmd.length);
			blitFn(cmd, dst, src.begin + 1);
			src.begin = cmd.srcEnd;
			dst += cmd.length;
			remainingWidth -= unclippedLength; // result can be negative
		}

		// We've advanced `dst` by `clipX.width`.
		//
		// Set dst to (position.y - 1, position.x)
		dst -= dstPitch + clipX.width;

		// `remainingWidth` can be negative, in which case it is the amount of pixels
		// that the source has overran the line.
		remainingWidth += clipX.right;
		SkipSize skipSize { 0, 0 };
		if (remainingWidth > 0) {
			skipSize.xOffset = static_cast<int_fast16_t>(src.width) - remainingWidth;
			src.begin = SkipRestOfLineWithOverrun<GetBlitCommand>(
			    src.begin, static_cast<int_fast16_t>(src.width), skipSize);
			--skipSize.wholeLines;
		} else if (remainingWidth < 0) {
			skipSize = GetSkipSize(-remainingWidth, static_cast<int_fast16_t>(src.width));
		}
		xOffset = skipSize.xOffset;
		dst -= dstPitch * skipSize.wholeLines;
	}
}

template <GetBlitCommandFn GetBlitCommand, typename BlitFn>
void DoRenderBackwards(
    const Surface &out, Point position, const uint8_t *src, size_t srcSize,
    unsigned srcWidth, unsigned srcHeight, BlitFn &&blitFn)
{
	if (position.y < 0 || position.y + 1 >= static_cast<int>(out.h() + srcHeight))
		return;
	const ClipX clipX = CalculateClipX(position.x, srcWidth, out);
	if (clipX.width <= 0)
		return;
	RenderSrcBackwards srcForBackwards { src, src + srcSize, static_cast<uint_fast16_t>(srcWidth) };
	if (static_cast<std::size_t>(clipX.width) == srcWidth) {
		DoRenderBackwardsClipY<GetBlitCommand>(
		    out, position, srcForBackwards, std::forward<BlitFn>(blitFn));
	} else {
		DoRenderBackwardsClipXY<GetBlitCommand>(
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

template <bool North, bool West, bool South, bool East, bool SkipColorIndexZero = true>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderOutlineForPixels(uint8_t *dst, int dstPitch, int width, const uint8_t *src, uint8_t color)
{
	if (SkipColorIndexZero) {
		while (width-- > 0)
			RenderOutlineForPixel<North, West, South, East>(dst++, dstPitch, *src++, color);
	} else {
		RenderOutlineForPixels<North, West, South, East>(dst, dstPitch, width, color);
	}
}

} // namespace devilution
