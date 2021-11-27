/**
 * @file cel_render.cpp
 *
 * CEL rendering.
 */
#include "engine/render/cel_render.hpp"

#include <cstdint>
#include <cstring>

#include "engine/cel_header.hpp"
#include "engine/render/common_impl.h"
#include "options.h"
#include "palette.h"
#include "scrollrt.h"
#include "utils/attributes.h"

namespace devilution {

namespace {

constexpr bool IsCelTransparent(std::uint8_t control)
{
	constexpr std::uint8_t CelTransparentMin = 0x80;
	return control >= CelTransparentMin;
}

constexpr std::uint8_t GetCelTransparentWidth(std::uint8_t control)
{
	return -static_cast<std::int8_t>(control);
}

DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT const byte *SkipRestOfCelLine(const byte *src, std::int_fast16_t remainingWidth)
{
	while (remainingWidth > 0) {
		const auto v = static_cast<std::int8_t>(*src++);
		if (!IsCelTransparent(v)) {
			src += v;
			remainingWidth -= v;
		} else {
			remainingWidth += v;
		}
	}
	return src;
}

constexpr auto NullLineEndFn = []() {};

/** Renders a CEL with only vertical clipping to the output buffer. */
template <typename RenderLine, typename LineEndFn>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderCelClipY(const Surface &out, Point position, const byte *src, std::size_t srcSize, std::size_t srcWidth,
    const RenderLine &renderLine, const LineEndFn &lineEndFn)
{
	const auto *srcEnd = src + srcSize;

	// Skip the bottom clipped lines.
	const auto dstHeight = out.h();
	while (position.y >= dstHeight && src != srcEnd) {
		src = SkipRestOfCelLine(src, static_cast<std::int_fast16_t>(srcWidth));
		--position.y;
		lineEndFn();
	}

	auto *dst = &out[position];
	const auto *dstBegin = out.begin();
	const auto dstPitch = out.pitch();
	while (src != srcEnd && dst >= dstBegin) {
		for (std::size_t remainingWidth = srcWidth; remainingWidth > 0;) {
			auto v = static_cast<std::uint8_t>(*src++);
			if (!IsCelTransparent(v)) {
				renderLine(dst, reinterpret_cast<const std::uint8_t *>(src), v);
				src += v;
			} else {
				v = GetCelTransparentWidth(v);
			}
			dst += v;
			remainingWidth -= v;
		}
		dst -= dstPitch + srcWidth;
		lineEndFn();
	}
}

/** Renders a CEL with both horizontal and vertical clipping to the output buffer. */
template <typename RenderLine, typename LineEndFn>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderCelClipXY( // NOLINT(readability-function-cognitive-complexity)
    const Surface &out, Point position, const byte *src, std::size_t srcSize, std::size_t srcWidth, ClipX clipX,
    const RenderLine &renderLine, const LineEndFn &lineEndFn)
{
	const auto *srcEnd = src + srcSize;

	// Skip the bottom clipped lines.
	const auto dstHeight = out.h();
	while (position.y >= dstHeight && src != srcEnd) {
		src = SkipRestOfCelLine(src, static_cast<std::int_fast16_t>(srcWidth));
		--position.y;
		lineEndFn();
	}

	position.x += static_cast<int>(clipX.left);

	auto *dst = &out[position];
	const auto *dstBegin = out.begin();
	const auto dstPitch = out.pitch();
	while (src < srcEnd && dst >= dstBegin) {
		// Skip initial src if clipping on the left.
		// Handles overshoot, i.e. when the RLE segment goes into the unclipped area.
		auto remainingWidth = clipX.width;
		auto remainingLeftClip = clipX.left;
		while (remainingLeftClip > 0) {
			auto v = static_cast<std::uint8_t>(*src++);
			if (!IsCelTransparent(v)) {
				if (v > remainingLeftClip) {
					const auto overshoot = v - remainingLeftClip;
					renderLine(dst, reinterpret_cast<const std::uint8_t *>(src + remainingLeftClip), overshoot);
					dst += overshoot;
					remainingWidth -= overshoot;
				}
				src += v;
			} else {
				v = GetCelTransparentWidth(v);
				if (v > remainingLeftClip) {
					const auto overshoot = v - remainingLeftClip;
					dst += overshoot;
					remainingWidth -= overshoot;
				}
			}
			remainingLeftClip -= v;
		}

		// Draw the non-clipped segment
		while (remainingWidth > 0) {
			auto v = static_cast<std::uint8_t>(*src++);
			if (!IsCelTransparent(v)) {
				if (v > remainingWidth) {
					renderLine(dst, reinterpret_cast<const std::uint8_t *>(src), remainingWidth);
					src += v;
					dst += remainingWidth;
					remainingWidth -= v;
					break;
				}
				renderLine(dst, reinterpret_cast<const std::uint8_t *>(src), v);
				src += v;
			} else {
				v = GetCelTransparentWidth(v);
				if (v > remainingWidth) {
					dst += remainingWidth;
					remainingWidth -= v;
					break;
				}
			}
			dst += v;
			remainingWidth -= v;
		}

		// Skip the rest of src line if clipping on the right
		assert(remainingWidth <= 0);
		src = SkipRestOfCelLine(src, clipX.right + remainingWidth);

		dst -= dstPitch + clipX.width;
		lineEndFn();
	}
}

template <typename RenderLine, typename LineEndFn>
DVL_ALWAYS_INLINE DVL_ATTRIBUTE_HOT void RenderCel(
    const Surface &out, Point position, const byte *src, std::size_t srcSize, std::size_t srcWidth,
    const RenderLine &renderLine, const LineEndFn &lineEndFn)
{
	const ClipX clipX = CalculateClipX(position.x, srcWidth, out);
	if (clipX.width <= 0)
		return;
	if (static_cast<std::size_t>(clipX.width) == srcWidth) {
		RenderCelClipY(out, position, src, srcSize, srcWidth, renderLine, lineEndFn);
	} else {
		RenderCelClipXY(out, position, src, srcSize, srcWidth, clipX, renderLine, lineEndFn);
	}
}

void RenderCelWithLightTable(const Surface &out, Point position, const byte *src, std::size_t srcSize, std::size_t srcWidth, const std::uint8_t *tbl)
{
	RenderCel(
	    out, position, src, srcSize, srcWidth, [tbl](std::uint8_t *dst, const std::uint8_t *src, std::size_t w) {
		    while (w-- > 0) {
			    *dst++ = tbl[static_cast<std::uint8_t>(*src)];
			    ++src;
		    }
	    },
	    NullLineEndFn);
}

constexpr auto RenderLineMemcpy = [](std::uint8_t *dst, const std::uint8_t *src, std::size_t w) {
	std::memcpy(dst, src, w);
};

template <bool SkipColorIndexZero, bool North, bool West, bool South, bool East>
void RenderOutlineForPixels( // NOLINT(readability-function-cognitive-complexity)
    std::uint8_t *dst, int dstPitch, int width, const std::uint8_t *src, std::uint8_t color)
{
	if (SkipColorIndexZero) {
		for (; width-- > 0; ++src, ++dst) {
			if (*src == 0)
				continue;
			if (North)
				dst[-dstPitch] = color;
			if (West)
				dst[-1] = color;
			if (East)
				dst[1] = color;
			if (South)
				dst[dstPitch] = color;
		}
	} else {
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
}

template <bool SkipColorIndexZero, bool North, bool West, bool South, bool East>
void RenderOutlineForPixel(std::uint8_t *dst, int dstPitch, std::uint8_t srcColor, std::uint8_t color)
{
	if (SkipColorIndexZero && srcColor == 0)
		return;
	if (North)
		dst[-dstPitch] = color;
	if (West)
		dst[-1] = color;
	if (East)
		dst[1] = color;
	if (South)
		dst[dstPitch] = color;
}

template <bool SkipColorIndexZero, bool North, bool West, bool South, bool East>
std::uint8_t *RenderCelOutlinePixelsCheckFirstColumn(
    std::uint8_t *dst, int dstPitch, int dstX,
    const std::uint8_t *src, std::uint8_t width, std::uint8_t color)
{
	if (dstX == -1) {
		RenderOutlineForPixel<SkipColorIndexZero, /*North=*/false, /*West=*/false, /*South=*/false, East>(
		    dst++, dstPitch, *src++, color);
		--width;
	}
	if (width > 0) {
		RenderOutlineForPixel<SkipColorIndexZero, North, /*West=*/false, South, East>(dst++, dstPitch, *src++, color);
		--width;
	}
	if (width > 0) {
		RenderOutlineForPixels<SkipColorIndexZero, North, West, South, East>(dst, dstPitch, width, src, color);
		dst += width;
	}
	return dst;
}

template <bool SkipColorIndexZero, bool North, bool West, bool South, bool East>
std::uint8_t *RenderCelOutlinePixelsCheckLastColumn(
    std::uint8_t *dst, int dstPitch, int dstX, int dstW,
    const std::uint8_t *src, std::uint8_t width, std::uint8_t color)
{
	const bool lastPixel = dstX < dstW && width >= 1;
	const bool oobPixel = dstX + width > dstW;
	const int numSpecialPixels = (lastPixel ? 1 : 0) + (oobPixel ? 1 : 0);
	if (width > numSpecialPixels) {
		width -= numSpecialPixels;
		RenderOutlineForPixels<SkipColorIndexZero, North, West, South, East>(dst, dstPitch, width, src, color);
		src += width;
		dst += width;
	}
	if (lastPixel)
		RenderOutlineForPixel<SkipColorIndexZero, North, West, South, /*East=*/false>(dst++, dstPitch, *src++, color);
	if (oobPixel)
		RenderOutlineForPixel<SkipColorIndexZero, /*North=*/false, West, /*South=*/false, /*East=*/false>(dst++, dstPitch, *src++, color);
	return dst;
}

template <bool SkipColorIndexZero, bool North, bool West, bool South, bool East, bool CheckFirstColumn, bool CheckLastColumn>
std::uint8_t *RenderCelOutlinePixels(
    std::uint8_t *dst, int dstPitch, int dstX, int dstW,
    const std::uint8_t *src, std::uint8_t width, std::uint8_t color)
{
	if (CheckFirstColumn && dstX <= 0) {
		return RenderCelOutlinePixelsCheckFirstColumn<SkipColorIndexZero, North, West, South, East>(
		    dst, dstPitch, dstX, src, width, color);
	}
	if (CheckLastColumn && dstX + width >= dstW) {
		return RenderCelOutlinePixelsCheckLastColumn<SkipColorIndexZero, North, West, South, East>(
		    dst, dstPitch, dstX, dstW, src, width, color);
	}
	RenderOutlineForPixels<SkipColorIndexZero, North, West, South, East>(dst, dstPitch, width, src, color);
	return dst + width;
}

template <bool SkipColorIndexZero, bool North, bool West, bool South, bool East,
    bool ClipWidth = false, bool CheckFirstColumn = false, bool CheckLastColumn = false>
const byte *RenderCelOutlineRowClipped( // NOLINT(readability-function-cognitive-complexity,misc-no-recursion)
    const Surface &out, Point position, const byte *src, ClipX clipX, std::uint8_t color)
{
	std::int_fast16_t remainingWidth = clipX.width;
	std::uint8_t v;

	auto *dst = &out[position];
	const auto dstPitch = out.pitch();
	const auto dstW = out.w();

	if (ClipWidth) {
		auto remainingLeftClip = clipX.left;
		while (remainingLeftClip > 0) {
			v = static_cast<std::uint8_t>(*src++);
			if (!IsCelTransparent(v)) {
				if (v > remainingLeftClip) {
					RenderCelOutlinePixels<SkipColorIndexZero, North, West, South, East, CheckFirstColumn, CheckLastColumn>(
					    dst, dstPitch, position.x, dstW, reinterpret_cast<const std::uint8_t *>(src), v - remainingLeftClip, color);
				}
				src += v;
			} else {
				v = GetCelTransparentWidth(v);
			}
			remainingLeftClip -= v;
		}
		dst -= static_cast<int>(remainingLeftClip);
		position.x -= static_cast<int>(remainingLeftClip);
		remainingWidth += remainingLeftClip;
	}

	while (remainingWidth > 0) {
		v = static_cast<std::uint8_t>(*src++);
		if (!IsCelTransparent(v)) {
			dst = RenderCelOutlinePixels<SkipColorIndexZero, North, West, South, East, CheckFirstColumn, CheckLastColumn>(
			    dst, dstPitch, position.x, dstW, reinterpret_cast<const std::uint8_t *>(src),
			    std::min(remainingWidth, static_cast<std::int_fast16_t>(v)), color);
			src += v;
		} else {
			v = GetCelTransparentWidth(v);
			dst += v;
		}
		remainingWidth -= v;
		position.x += v;
	}

	src = SkipRestOfCelLine(src, clipX.right + remainingWidth);

	return src;
}

template <bool SkipColorIndexZero>
void RenderCelOutlineClippedY(const Surface &out, Point position, const byte *src, std::size_t srcSize, // NOLINT(readability-function-cognitive-complexity)
    std::size_t srcWidth, std::uint8_t color)
{
	const auto *srcEnd = src + srcSize;

	// Skip the bottom clipped lines.
	const auto dstHeight = out.h();
	while (position.y > dstHeight && src != srcEnd) {
		src = SkipRestOfCelLine(src, static_cast<std::int_fast16_t>(srcWidth));
		--position.y;
	}
	if (src == srcEnd)
		return;

	const ClipX clipX = { 0, 0, static_cast<decltype(ClipX {}.width)>(srcWidth) };

	if (position.y == dstHeight) {
		// After-bottom line - can only draw north.
		src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/true, /*West=*/false, /*South=*/false, /*East=*/false>(
		    out, position, src, clipX, color);
		--position.y;
	}
	if (src == srcEnd)
		return;

	if (position.y + 1 == dstHeight) {
		// Bottom line - cannot draw south.
		src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true>(
		    out, position, src, clipX, color);
		--position.y;
	}

	while (position.y > 0 && src != srcEnd) {
		src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true>(
		    out, position, src, clipX, color);
		--position.y;
	}
	if (src == srcEnd)
		return;

	if (position.y == 0) {
		src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true>(
		    out, position, src, clipX, color);
		--position.y;
	}
	if (src == srcEnd)
		return;

	if (position.y == -1) {
		// Special case: the top of the sprite is 1px below the last line, render just the outline above.
		RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/false, /*West=*/false, /*South=*/true, /*East=*/false>(
		    out, position, src, clipX, color);
	}
}

template <bool SkipColorIndexZero>
void RenderCelOutlineClippedXY(const Surface &out, Point position, const byte *src, std::size_t srcSize, // NOLINT(readability-function-cognitive-complexity)
    std::size_t srcWidth, std::uint8_t color)
{
	const auto *srcEnd = src + srcSize;

	// Skip the bottom clipped lines.
	const auto dstHeight = out.h();
	while (position.y > dstHeight && src != srcEnd) {
		src = SkipRestOfCelLine(src, static_cast<std::int_fast16_t>(srcWidth));
		--position.y;
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
		src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/true, /*West=*/false, /*South=*/false, /*East=*/false,
		    /*ClipWidth=*/true>(
		    out, position, src, clipX, color);
		--position.y;
	}
	if (src == srcEnd)
		return;

	if (position.y + 1 == dstHeight) {
		// Bottom line - cannot draw south.
		if (position.x <= 0) {
			src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/true, /*CheckLastColumn=*/false>(
			    out, position, src, clipX, color);
		} else if (position.x + clipX.width >= out.w()) {
			src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/false, /*CheckLastColumn=*/true>(
			    out, position, src, clipX, color);
		} else {
			src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true,
			    /*ClipWidth=*/true>(
			    out, position, src, clipX, color);
		}
		--position.y;
	}

	if (position.x <= 0) {
		while (position.y > 0 && src != srcEnd) {
			src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/true, /*CheckLastColumn=*/false>(
			    out, position, src, clipX, color);
			--position.y;
		}
	} else if (position.x + clipX.width >= out.w()) {
		while (position.y > 0 && src != srcEnd) {
			src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/false, /*CheckLastColumn=*/true>(
			    out, position, src, clipX, color);
			--position.y;
		}
	} else {
		while (position.y > 0 && src != srcEnd) {
			src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true>(
			    out, position, src, clipX, color);
			--position.y;
		}
	}
	if (src == srcEnd)
		return;

	if (position.y == 0) {
		if (position.x <= 0) {
			src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/true, /*CheckLastColumn=*/false>(
			    out, position, src, clipX, color);
		} else if (position.x + clipX.width >= out.w()) {
			src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/false, /*CheckLastColumn=*/true>(
			    out, position, src, clipX, color);
		} else {
			src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true>(
			    out, position, src, clipX, color);
		}
		--position.y;
	}
	if (src == srcEnd)
		return;

	if (position.y == -1) {
		// Special case: the top of the sprite is 1px below the last line, render just the outline above.
		RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/false, /*West=*/false, /*South=*/true, /*East=*/false,
		    /*ClipWidth=*/true>(
		    out, position, src, clipX, color);
	}
}

template <bool SkipColorIndexZero>
void RenderCelOutline(const Surface &out, Point position, const byte *src, std::size_t srcSize,
    std::size_t srcWidth, std::uint8_t color)
{
	if (position.x > 0 && position.x + static_cast<int>(srcWidth) < static_cast<int>(out.w())) {
		RenderCelOutlineClippedY<SkipColorIndexZero>(out, position, src, srcSize, srcWidth, color);
	} else {
		RenderCelOutlineClippedXY<SkipColorIndexZero>(out, position, src, srcSize, srcWidth, color);
	}
}

/**
 * @brief Blit CEL sprite to the given buffer, checks for drawing outside the buffer.
 * @param out Target buffer
 * @param position Target buffer coordinate
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 */
void CelBlitSafeTo(const Surface &out, Point position, const byte *pRLEBytes, int nDataSize, int nWidth)
{
	assert(pRLEBytes != nullptr);
	RenderCel(out, position, pRLEBytes, nDataSize, nWidth, RenderLineMemcpy, NullLineEndFn);
}

/**
 * @brief Same as CelBlitLightSafeTo but with stippled transparency applied
 * @param out Target buffer
 * @param position Target buffer coordinate
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 */
void CelBlitLightTransSafeTo(const Surface &out, Point position, const byte *pRLEBytes, int nDataSize, int nWidth)
{
	assert(pRLEBytes != nullptr);
	const std::uint8_t *tbl = &LightTables[LightTableIndex * 256];
	bool shift = (reinterpret_cast<uintptr_t>(&out[position]) % 2 == 1);
	const bool pitchIsEven = (out.pitch() % 2 == 0);
	RenderCel(
	    out, position, pRLEBytes, nDataSize, nWidth,
	    [tbl, &shift](std::uint8_t *dst, const std::uint8_t *src, std::size_t width) {
		    if (reinterpret_cast<uintptr_t>(dst) % 2 == (shift ? 1 : 0)) {
			    ++dst, ++src, --width;
		    }
		    for (const auto *dstEnd = dst + width; dst < dstEnd; dst += 2, src += 2) {
			    *dst = tbl[*src];
		    }
	    },
	    [pitchIsEven, &shift]() { if (pitchIsEven) shift = !shift; });
}

/**
 * @brief Same as CelBlitLightSafe, with blended transparency applied
 * @param out The output buffer
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 * @param nWidth Width of sprite
 * @param tbl Palette translation table
 */
void CelBlitLightBlendedSafeTo(const Surface &out, Point position, const byte *pRLEBytes, int nDataSize, int nWidth, const uint8_t *tbl)
{
	assert(pRLEBytes != nullptr);
	if (tbl == nullptr)
		tbl = &LightTables[LightTableIndex * 256];

	RenderCel(
	    out, position, pRLEBytes, nDataSize, nWidth, [tbl](std::uint8_t *dst, const uint8_t *src, std::size_t w) {
		    while (w-- > 0) {
			    *dst = paletteTransparencyLookup[*dst][tbl[*src++]];
			    ++dst;
		    }
	    },
	    NullLineEndFn);
}

/**
 * @brief Blit CEL sprite, and apply lighting, to the given buffer, checks for drawing outside the buffer
 * @param out Target buffer
 * @param position Target buffer coordinate
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 * @param tbl Palette translation table
 */
void CelBlitLightSafeTo(const Surface &out, Point position, const byte *pRLEBytes, int nDataSize, int nWidth, uint8_t *tbl)
{
	assert(pRLEBytes != nullptr);
	if (tbl == nullptr)
		tbl = &LightTables[LightTableIndex * 256];
	RenderCelWithLightTable(out, position, pRLEBytes, nDataSize, nWidth, tbl);
}

} // namespace

void CelDrawTo(const Surface &out, Point position, const CelSprite &cel, int frame)
{
	int nDataSize;
	const auto *pRLEBytes = CelGetFrame(cel.Data(), frame, &nDataSize);
	CelBlitSafeTo(out, position, pRLEBytes, nDataSize, cel.Width(frame));
}

void CelClippedDrawTo(const Surface &out, Point position, const CelSprite &cel, int frame)
{
	int nDataSize;
	const auto *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);

	CelBlitSafeTo(out, position, pRLEBytes, nDataSize, cel.Width(frame));
}

void CelDrawLightTo(const Surface &out, Point position, const CelSprite &cel, int frame, uint8_t *tbl)
{
	int nDataSize;
	const auto *pRLEBytes = CelGetFrame(cel.Data(), frame, &nDataSize);

	if (LightTableIndex != 0 || tbl != nullptr)
		CelBlitLightSafeTo(out, position, pRLEBytes, nDataSize, cel.Width(frame), tbl);
	else
		CelBlitSafeTo(out, position, pRLEBytes, nDataSize, cel.Width(frame));
}

void CelClippedDrawLightTo(const Surface &out, Point position, const CelSprite &cel, int frame)
{
	int nDataSize;
	const auto *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);

	if (LightTableIndex != 0)
		CelBlitLightSafeTo(out, position, pRLEBytes, nDataSize, cel.Width(frame), nullptr);
	else
		CelBlitSafeTo(out, position, pRLEBytes, nDataSize, cel.Width(frame));
}

void CelDrawLightRedTo(const Surface &out, Point position, const CelSprite &cel, int frame)
{
	int nDataSize;
	const auto *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);
	RenderCelWithLightTable(out, position, pRLEBytes, nDataSize, cel.Width(frame), GetLightTable(1));
}

void CelDrawItem(const Item &item, const Surface &out, Point position, const CelSprite &cel, int frame)
{
	bool usable = item._iStatFlag;
	if (!usable) {
		CelDrawLightRedTo(out, position, cel, frame);
	} else {
		CelClippedDrawTo(out, position, cel, frame);
	}
}

void CelClippedBlitLightTransTo(const Surface &out, Point position, const CelSprite &cel, int frame)
{
	int nDataSize;
	const byte *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);

	if (cel_transparency_active) {
		if (*sgOptions.Graphics.blendedTransparancy)
			CelBlitLightBlendedSafeTo(out, position, pRLEBytes, nDataSize, cel.Width(frame), nullptr);
		else
			CelBlitLightTransSafeTo(out, position, pRLEBytes, nDataSize, cel.Width(frame));
	} else if (LightTableIndex != 0)
		CelBlitLightSafeTo(out, position, pRLEBytes, nDataSize, cel.Width(frame), nullptr);
	else
		CelBlitSafeTo(out, position, pRLEBytes, nDataSize, cel.Width(frame));
}

void CelDrawUnsafeTo(const Surface &out, Point position, const CelSprite &cel, int frame)
{
	int nDataSize;
	const auto *pRLEBytes = CelGetFrame(cel.Data(), frame, &nDataSize);
	RenderCelClipY(out, position, pRLEBytes, nDataSize, cel.Width(frame), RenderLineMemcpy, NullLineEndFn);
}

void CelBlitOutlineTo(const Surface &out, uint8_t col, Point position, const CelSprite &cel, int frame, bool skipColorIndexZero)
{
	int nDataSize;
	const byte *src = CelGetFrameClipped(cel.Data(), frame, &nDataSize);
	if (skipColorIndexZero)
		RenderCelOutline<true>(out, position, src, nDataSize, cel.Width(frame), col);
	else
		RenderCelOutline<false>(out, position, src, nDataSize, cel.Width(frame), col);
}

std::pair<int, int> MeasureSolidHorizontalBounds(const CelSprite &cel, int frame)
{
	int nDataSize;
	const byte *src = CelGetFrameClipped(cel.Data(), frame, &nDataSize);
	const auto *end = &src[nDataSize];
	const int celWidth = cel.Width(frame);

	int xBegin = celWidth;
	int xEnd = 0;
	while (src < end) {
		int xCur = 0;
		while (xCur < celWidth) {
			const auto val = static_cast<std::uint8_t>(*src++);
			if (IsCelTransparent(val)) {
				const int width = GetCelTransparentWidth(val);
				xCur += width;
			} else {
				xBegin = std::min(xBegin, xCur);
				xCur += val;
				xEnd = std::max(xEnd, xCur);
				src += val;
			}
		}
		if (xBegin == 0 && xEnd == celWidth)
			break;
	}
	return { xBegin, xEnd };
}

} // namespace devilution
