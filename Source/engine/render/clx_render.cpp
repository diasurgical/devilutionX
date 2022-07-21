/**
 * @file clx_render.cpp
 *
 * CL2 rendering.
 */
#include "clx_render.hpp"

#include <algorithm>

#include "engine/render/common_impl.h"
#include "engine/render/scrollrt.h"
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

BlitCommand Cl2GetBlitCommand(const uint8_t *src)
{
	const uint8_t control = *src++;
	if (!IsCl2Opaque(control))
		return BlitCommand { BlitType::Transparent, src, control, 0 };
	if (IsCl2OpaqueFill(control)) {
		const uint8_t width = GetCl2OpaqueFillWidth(control);
		const uint8_t color = *src++;
		return BlitCommand { BlitType::Fill, src, width, color };
	}
	const uint8_t width = GetCl2OpaquePixelsWidth(control);
	return BlitCommand { BlitType::Pixels, src + width, width, 0 };
}

template <bool Fill, bool North, bool West, bool South, bool East, bool SkipColorIndexZero>
uint8_t *RenderClxOutlinePixelsCheckFirstColumn(
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
		dst += width;
	}
	return dst;
}

template <bool Fill, bool North, bool West, bool South, bool East, bool SkipColorIndexZero>
uint8_t *RenderClxOutlinePixelsCheckLastColumn(
    uint8_t *dst, int dstPitch, int dstX, int dstW,
    const uint8_t *src, uint8_t width, uint8_t color)
{
	const bool lastPixel = dstX < dstW && width >= 1;
	const bool oobPixel = dstX + width > dstW;
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
			RenderOutlineForPixel</*North=*/false, West, /*South=*/false, /*East=*/false>(dst++, dstPitch, color);
		} else {
			RenderOutlineForPixel</*North=*/false, West, /*South=*/false, /*East=*/false, SkipColorIndexZero>(dst++, dstPitch, *src++, color);
		}
	}
	return dst;
}

template <bool Fill, bool North, bool West, bool South, bool East, bool SkipColorIndexZero, bool CheckFirstColumn, bool CheckLastColumn>
uint8_t *RenderClxOutlinePixels(
    uint8_t *dst, int dstPitch, int dstX, int dstW,
    const uint8_t *src, uint8_t width, uint8_t color)
{
	if (SkipColorIndexZero && Fill && *src == 0)
		return dst + width;

	if (CheckFirstColumn && dstX <= 0) {
		return RenderClxOutlinePixelsCheckFirstColumn<Fill, North, West, South, East, SkipColorIndexZero>(
		    dst, dstPitch, dstX, src, width, color);
	}
	if (CheckLastColumn && dstX + width >= dstW) {
		return RenderClxOutlinePixelsCheckLastColumn<Fill, North, West, South, East, SkipColorIndexZero>(
		    dst, dstPitch, dstX, dstW, src, width, color);
	}
	if (Fill) {
		RenderOutlineForPixels<North, West, South, East>(dst, dstPitch, width, color);
	} else {
		RenderOutlineForPixels<North, West, South, East, SkipColorIndexZero>(dst, dstPitch, width, src, color);
	}
	return dst + width;
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
			dst = RenderClxOutlinePixels</*Fill=*/true, North, West, South, East, SkipColorIndexZero, CheckFirstColumn, CheckLastColumn>(
			    dst, dstPitch, position.x, out.w(), src, w, color);
			++src;
		} else {
			dst = RenderClxOutlinePixels</*Fill=*/false, North, West, South, East, SkipColorIndexZero, CheckFirstColumn, CheckLastColumn>(
			    dst, dstPitch, position.x, out.w(), src, w, color);
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
			v = static_cast<uint8_t>(*src++);
			if (IsCl2Opaque(v)) {
				const bool fill = IsCl2OpaqueFill(v);
				v = fill ? GetCl2OpaqueFillWidth(v) : GetCl2OpaquePixelsWidth(v);
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
		if (IsCl2Opaque(v)) {
			const bool fill = IsCl2OpaqueFill(v);
			v = fill ? GetCl2OpaqueFillWidth(v) : GetCl2OpaquePixelsWidth(v);
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
			src = SkipRestOfLineWithOverrun<Cl2GetBlitCommand>(src, static_cast<int_fast16_t>(srcWidth), skipSize);
			if (skipSize.wholeLines > 1)
				dst -= dstPitch * (skipSize.wholeLines - 1);
			remainingWidth = -skipSize.xOffset;
		}
	}
	if (remainingWidth < 0) {
		skipSize = GetSkipSize(-remainingWidth, static_cast<int_fast16_t>(srcWidth));
		++skipSize.wholeLines;
	} else {
		skipSize.xOffset = 0;
		skipSize.wholeLines = 1;
	}

	return src;
}

template <bool SkipColorIndexZero>
void RenderClxOutlineClippedY(const Surface &out, Point position, RenderSrcBackwards src, // NOLINT(readability-function-cognitive-complexity)
    uint8_t color)
{
	// Skip the bottom clipped lines.
	const int dstHeight = out.h();
	SkipSize skipSize = { 0, SkipLinesForRenderBackwardsWithOverrun<Cl2GetBlitCommand>(position, src, dstHeight) };
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
void RenderClxOutlineClippedXY(const Surface &out, Point position, RenderSrcBackwards src, // NOLINT(readability-function-cognitive-complexity)
    uint8_t color)
{
	// Skip the bottom clipped lines.
	const int dstHeight = out.h();
	SkipSize skipSize = { 0, SkipLinesForRenderBackwardsWithOverrun<Cl2GetBlitCommand>(position, src, dstHeight) };
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
	RenderSrcBackwards srcForBackwards { src, src + srcSize, static_cast<uint_fast16_t>(srcWidth) };
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
		if (!IsCl2Opaque(val))
			continue;
		if (IsCl2OpaqueFill(val)) {
			--remaining;
			*dst = trn[*dst];
			dst++;
		} else {
			val = GetCl2OpaquePixelsWidth(val);
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
			if (!IsCl2Opaque(val)) {
				xCur += val;
				continue;
			}
			if (IsCl2OpaqueFill(val)) {
				val = GetCl2OpaqueFillWidth(val);
				++src;
			} else {
				val = GetCl2OpaquePixelsWidth(val);
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

void ClxDraw(const Surface &out, Point position, ClxSprite clx)
{
	DoRenderBackwards<Cl2GetBlitCommand>(out, position, clx.pixelData(), clx.pixelDataSize(), clx.width(), clx.height(), BlitDirect {});
}

void ClxDrawTRN(const Surface &out, Point position, ClxSprite clx, const uint8_t *trn)
{
	DoRenderBackwards<Cl2GetBlitCommand>(out, position, clx.pixelData(), clx.pixelDataSize(), clx.width(), clx.height(), BlitWithMap { trn });
}

void ClxDrawBlendedTRN(const Surface &out, Point position, ClxSprite clx, const uint8_t *trn)
{
	DoRenderBackwards<Cl2GetBlitCommand>(out, position, clx.pixelData(), clx.pixelDataSize(), clx.width(), clx.height(), BlitBlendedWithMap { trn });
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
