/**
 * @file cl2_render.cpp
 *
 * CL2 rendering.
 */
#include "cl2_render.hpp"

#include <algorithm>

#include "engine/cel_header.hpp"
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

/**
 * @brief Blit CL2 sprite to the given buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CL2 pixel stream (run-length encoded)
 * @param nDataSize Size of CL2 in bytes
 * @param nWidth Width of sprite
 */
void Cl2BlitSafe(const Surface &out, int sx, int sy, const byte *pRLEBytes, int nDataSize, int nWidth)
{
	DoRenderBackwards</*TransparentCommandCanCrossLines=*/true, Cl2GetBlitCommand>(
	    out, { sx, sy }, reinterpret_cast<const uint8_t *>(pRLEBytes), nDataSize, nWidth, BlitDirect {});
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
void Cl2BlitLightSafe(const Surface &out, int sx, int sy, const byte *pRLEBytes, int nDataSize, int nWidth, uint8_t *pTable)
{
	DoRenderBackwards</*TransparentCommandCanCrossLines=*/true, Cl2GetBlitCommand>(
	    out, { sx, sy }, reinterpret_cast<const uint8_t *>(pRLEBytes), nDataSize, nWidth, BlitWithMap { pTable });
}

template <bool Fill, bool North, bool West, bool South, bool East>
uint8_t *RenderCl2OutlinePixelsCheckFirstColumn(
    uint8_t *dst, int dstPitch, int dstX,
    const uint8_t *src, uint8_t width, uint8_t color)
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
uint8_t *RenderCl2OutlinePixelsCheckLastColumn(
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
uint8_t *RenderCl2OutlinePixels(
    uint8_t *dst, int dstPitch, int dstX, int dstW,
    const uint8_t *src, uint8_t width, uint8_t color)
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
const uint8_t *RenderCl2OutlineRowClipped( // NOLINT(readability-function-cognitive-complexity)
    const Surface &out, Point position, const uint8_t *src, std::size_t srcWidth,
    ClipX clipX, uint8_t color, SkipSize &skipSize)
{
	int_fast16_t remainingWidth = clipX.width;
	uint8_t v;

	auto *dst = &out[position];
	const auto dstPitch = out.pitch();

	const auto renderPixels = [&](bool fill, uint8_t w) {
		if (fill) {
			dst = RenderCl2OutlinePixels</*Fill=*/true, North, West, South, East, CheckFirstColumn, CheckLastColumn>(
			    dst, dstPitch, position.x, out.w(), src, w, color);
			++src;
		} else {
			dst = RenderCl2OutlinePixels</*Fill=*/false, North, West, South, East, CheckFirstColumn, CheckLastColumn>(
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

void RenderCl2OutlineClippedY(const Surface &out, Point position, RenderSrcBackwards src, // NOLINT(readability-function-cognitive-complexity)
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
		src.begin = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/false, /*South=*/false, /*East=*/false>(
		    out, position, src.begin, src.width, clipX, color, skipSize);
		position.y -= static_cast<int>(skipSize.wholeLines);
	}
	if (src.begin == src.end)
		return;

	if (position.y + 1 == dstHeight) {
		// Bottom line - cannot draw south.
		src.begin = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true>(
		    out, position, src.begin, src.width, clipX, color, skipSize);
		position.y -= static_cast<int>(skipSize.wholeLines);
	}

	while (position.y > 0 && src.begin != src.end) {
		src.begin = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true>(
		    out, position, src.begin, src.width, clipX, color, skipSize);
		position.y -= static_cast<int>(skipSize.wholeLines);
	}
	if (src.begin == src.end)
		return;

	if (position.y == 0) {
		src.begin = RenderCl2OutlineRowClipped</*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true>(
		    out, position, src.begin, src.width, clipX, color, skipSize);
		position.y -= static_cast<int>(skipSize.wholeLines);
	}
	if (src.begin == src.end)
		return;

	if (position.y == -1) {
		// Special case: the top of the sprite is 1px below the last line, render just the outline above.
		RenderCl2OutlineRowClipped</*North=*/false, /*West=*/false, /*South=*/true, /*East=*/false>(
		    out, position, src.begin, src.width, clipX, color, skipSize);
	}
}

void RenderCl2OutlineClippedXY(const Surface &out, Point position, RenderSrcBackwards src, // NOLINT(readability-function-cognitive-complexity)
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
			src.begin = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/false, /*South=*/false, /*East=*/false,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/true, /*CheckLastColumn=*/false>(out, position, src.begin, src.width, clipX, color, skipSize);
		} else if (position.x + clipX.width >= out.w()) {
			src.begin = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/false, /*South=*/false, /*East=*/false,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/false, /*CheckLastColumn=*/true>(out, position, src.begin, src.width, clipX, color, skipSize);
		} else {
			src.begin = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/false, /*South=*/false, /*East=*/false,
			    /*ClipWidth=*/true>(out, position, src.begin, src.width, clipX, color, skipSize);
		}
		position.y -= static_cast<int>(skipSize.wholeLines);
	}
	if (src.begin == src.end)
		return;

	if (position.y + 1 == dstHeight) {
		// Bottom line - cannot draw south.
		if (position.x <= 0) {
			src.begin = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/true, /*CheckLastColumn=*/false>(
			    out, position, src.begin, src.width, clipX, color, skipSize);
		} else if (position.x + clipX.width >= out.w()) {
			src.begin = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/false, /*CheckLastColumn=*/true>(
			    out, position, src.begin, src.width, clipX, color, skipSize);
		} else {
			src.begin = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true,
			    /*ClipWidth=*/true>(
			    out, position, src.begin, src.width, clipX, color, skipSize);
		}
		position.y -= static_cast<int>(skipSize.wholeLines);
	}

	if (position.x <= 0) {
		while (position.y > 0 && src.begin != src.end) {
			src.begin = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/true, /*CheckLastColumn=*/false>(
			    out, position, src.begin, src.width, clipX, color, skipSize);
			position.y -= static_cast<int>(skipSize.wholeLines);
		}
	} else if (position.x + clipX.width >= out.w()) {
		while (position.y > 0 && src.begin != src.end) {
			src.begin = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/false, /*CheckLastColumn=*/true>(
			    out, position, src.begin, src.width, clipX, color, skipSize);
			position.y -= static_cast<int>(skipSize.wholeLines);
		}
	} else {
		while (position.y > 0 && src.begin != src.end) {
			src.begin = RenderCl2OutlineRowClipped</*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true>(
			    out, position, src.begin, src.width, clipX, color, skipSize);
			position.y -= static_cast<int>(skipSize.wholeLines);
		}
	}
	if (src.begin == src.end)
		return;

	if (position.y == 0) {
		if (position.x <= 0) {
			src.begin = RenderCl2OutlineRowClipped</*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/true, /*CheckLastColumn=*/false>(
			    out, position, src.begin, src.width, clipX, color, skipSize);
		} else if (position.x + clipX.width >= out.w()) {
			src.begin = RenderCl2OutlineRowClipped</*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/false, /*CheckLastColumn=*/true>(
			    out, position, src.begin, src.width, clipX, color, skipSize);
		} else {
			src.begin = RenderCl2OutlineRowClipped</*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true,
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
			src.begin = RenderCl2OutlineRowClipped</*North=*/false, /*West=*/false, /*South=*/true, /*East=*/false,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/true, /*CheckLastColumn=*/false>(out, position, src.begin, src.width, clipX, color, skipSize);
		} else if (position.x + clipX.width >= out.w()) {
			src.begin = RenderCl2OutlineRowClipped</*North=*/false, /*West=*/false, /*South=*/true, /*East=*/false,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/false, /*CheckLastColumn=*/true>(out, position, src.begin, src.width, clipX, color, skipSize);
		} else {
			src.begin = RenderCl2OutlineRowClipped</*North=*/false, /*West=*/false, /*South=*/true, /*East=*/false,
			    /*ClipWidth=*/true>(out, position, src.begin, src.width, clipX, color, skipSize);
		}
	}
}

void RenderCl2Outline(const Surface &out, Point position, const uint8_t *src, std::size_t srcSize,
    std::size_t srcWidth, uint8_t color)
{
	RenderSrcBackwards srcForBackwards { src, src + srcSize, static_cast<uint_fast16_t>(srcWidth) };
	if (position.x > 0 && position.x + static_cast<int>(srcWidth) < static_cast<int>(out.w())) {
		RenderCl2OutlineClippedY(out, position, srcForBackwards, color);
	} else {
		RenderCl2OutlineClippedXY(out, position, srcForBackwards, color);
	}
}

} // namespace

void Cl2ApplyTrans(byte *p, const std::array<uint8_t, 256> &ttbl, int numFrames)
{
	assert(p != nullptr);

	for (int i = 0; i < numFrames; ++i) {
		constexpr int FrameHeaderSize = 10;
		int nDataSize;
		byte *dst = CelGetFrame(p, i, &nDataSize) + FrameHeaderSize;
		nDataSize -= FrameHeaderSize;
		while (nDataSize > 0) {
			auto v = static_cast<uint8_t>(*dst++);
			nDataSize--;
			assert(nDataSize >= 0);
			if (!IsCl2Opaque(v))
				continue;
			if (IsCl2OpaqueFill(v)) {
				nDataSize--;
				assert(nDataSize >= 0);
				*dst = static_cast<byte>(ttbl[static_cast<uint8_t>(*dst)]);
				dst++;
			} else {
				v = GetCl2OpaquePixelsWidth(v);
				nDataSize -= v;
				assert(nDataSize >= 0);
				while (v-- > 0) {
					*dst = static_cast<byte>(ttbl[static_cast<uint8_t>(*dst)]);
					dst++;
				}
			}
		}
	}
}

void Cl2Draw(const Surface &out, int sx, int sy, CelSprite cel, int frame)
{
	assert(frame >= 0);

	int nDataSize;
	const byte *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);

	Cl2BlitSafe(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame));
}

void Cl2DrawOutline(const Surface &out, uint8_t col, int sx, int sy, CelSprite cel, int frame)
{
	assert(frame >= 0);

	int nDataSize;
	const byte *src = CelGetFrameClipped(cel.Data(), frame, &nDataSize);

	RenderCl2Outline(out, { sx, sy }, reinterpret_cast<const uint8_t *>(src), nDataSize, cel.Width(frame), col);
}

void Cl2DrawTRN(const Surface &out, int sx, int sy, CelSprite cel, int frame, uint8_t *trn)
{
	assert(frame >= 0);

	int nDataSize;
	const byte *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);
	Cl2BlitLightSafe(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame), trn);
}

void Cl2DrawLight(const Surface &out, int sx, int sy, CelSprite cel, int frame)
{
	assert(frame >= 0);

	int nDataSize;
	const byte *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);

	if (LightTableIndex != 0)
		Cl2BlitLightSafe(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame), &LightTables[LightTableIndex * 256]);
	else
		Cl2BlitSafe(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame));
}

} // namespace devilution
