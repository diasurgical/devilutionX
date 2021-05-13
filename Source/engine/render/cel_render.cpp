/**
 * @file cel_render.cpp
 *
 * CEL rendering.
 */
#include "engine/render/cel_render.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "engine/render/common_impl.h"
#include "options.h"
#include "palette.h"
#include "scrollrt.h"

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

const byte *SkipRestOfCelLine(const byte *src, std::int_fast16_t remainingWidth)
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
void RenderCelClipY(const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize, std::size_t srcWidth,
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
void RenderCelClipXY(const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize, std::size_t srcWidth, ClipX clipX,
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
void RenderCel(const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize, std::size_t srcWidth,
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

void RenderCelWithLightTable(const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize, std::size_t srcWidth, const std::uint8_t *tbl)
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
void RenderOutlineForPixels(std::uint8_t *dst, int dstPitch, const std::uint8_t *src, int width, std::uint8_t color)
{
	for (; width-- > 0; ++src, ++dst) {
		if (SkipColorIndexZero && *src == 0)
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

template <bool SkipColorIndexZero, bool North, bool West, bool South, bool East,
    bool ClipWidth = false, bool CheckFirstColumn = false, bool CheckLastColumn = false, bool ContinueRleRun = false>
const byte *RenderCelOutlineRowClipped( // NOLINT(readability-function-cognitive-complexity,misc-no-recursion)
    const CelOutputBuffer &out, Point position, const byte *src, int srcWidth,
    ClipX clipX, std::uint8_t color, std::uint8_t continueRleRun = 0)
{
	std::int_fast16_t remainingWidth = clipX.width;
	std::uint8_t v;

	if (ClipWidth && !ContinueRleRun) {
		auto remainingLeftClip = clipX.left;
		while (remainingLeftClip > 0) {
			v = static_cast<std::uint8_t>(*src++);
			if (!IsCelTransparent(v)) {
				if (v > remainingLeftClip) {
					return RenderCelOutlineRowClipped<SkipColorIndexZero, North, West, South, East, ClipWidth,
					    CheckFirstColumn, CheckLastColumn, /*ContinueRleRun=*/true>(out, position,
					    src + remainingLeftClip, srcWidth - clipX.left, { 0, 0, clipX.width }, color, v - remainingLeftClip);
				}
				src += v;
			} else {
				v = GetCelTransparentWidth(v);
				if (v > remainingLeftClip) {
					const std::uint8_t overshoot = v - remainingLeftClip;
					position.x += overshoot;
					remainingWidth -= overshoot;
					if (remainingWidth <= 0)
						return src;
					break;
				}
			}
			remainingLeftClip -= v;
		}
	}

	auto *dst = &out[position];
	const auto dstPitch = out.pitch();

	const auto handleEdgePixels = [&](std::uint8_t width) -> bool {
		if (CheckFirstColumn && position.x <= 0) {
			if (position.x == -1) {
				RenderOutlineForPixel<SkipColorIndexZero, /*North=*/false, /*West=*/false, /*South=*/false, East>(
				    dst++, dstPitch, static_cast<std::uint8_t>(*src++), color);
				--width;
			}
			if (width > 0) {
				RenderOutlineForPixel<SkipColorIndexZero, North, /*West=*/false, South, East>(
				    dst++, dstPitch, static_cast<std::uint8_t>(*src++), color);
				--width;
			}
			if (width > 0) {
				RenderOutlineForPixels<SkipColorIndexZero, North, West, South, East>(
				    dst, dstPitch, reinterpret_cast<const std::uint8_t *>(src), width, color);
				src += width, dst += width;
			}
			return true;
		}
		if (CheckLastColumn && position.x + width >= out.w()) {
			const bool lastPixel = position.x < out.w() && width >= 1;
			const bool oobPixel = position.x + width > out.w();
			const int numSpecialPixels = (lastPixel ? 1 : 0) + (oobPixel ? 1 : 0);
			if (width > numSpecialPixels) {
				width -= numSpecialPixels;
				RenderOutlineForPixels<SkipColorIndexZero, North, West, South, East>(
				    dst, dstPitch, reinterpret_cast<const std::uint8_t *>(src), width, color);
				src += width, dst += width;
			}
			if (lastPixel) {
				RenderOutlineForPixel<SkipColorIndexZero, North, West, South, /*East=*/false>(
				    dst++, dstPitch, static_cast<std::uint8_t>(*src++), color);
			}
			if (oobPixel) {
				RenderOutlineForPixel<SkipColorIndexZero, /*North=*/false, West, /*South=*/false, /*East=*/false>(
				    dst++, dstPitch, static_cast<std::uint8_t>(*src++), color);
			}
			return true;
		}
		return false;
	};

	const auto handleOvershoot = [&]() -> bool {
		if (!((ClipWidth || CheckLastColumn || CheckFirstColumn) && v >= remainingWidth))
			return false;
		if (handleEdgePixels(remainingWidth)) {
			src += v - remainingWidth;
		} else {
			RenderOutlineForPixels<SkipColorIndexZero, North, West, South, East>(
			    dst, dstPitch, reinterpret_cast<const std::uint8_t *>(src), remainingWidth, color);
			src += v;
		}
		if (ClipWidth && clipX.right != 0)
			src = SkipRestOfCelLine(src, srcWidth - clipX.width - (v - remainingWidth));
		return true;
	};

	if (ClipWidth || CheckFirstColumn || ContinueRleRun) {
		v = ContinueRleRun ? continueRleRun : static_cast<std::uint8_t>(*src++);
		if (ContinueRleRun || !IsCelTransparent(v)) {
			if (handleOvershoot())
				return src;
			if (!handleEdgePixels(v)) {
				RenderOutlineForPixels<SkipColorIndexZero, North, West, South, East>(
				    dst, dstPitch, reinterpret_cast<const std::uint8_t *>(src), v, color);
				src += v, dst += v;
			}
		} else {
			v = GetCelTransparentWidth(v);
			if ((ClipWidth || CheckLastColumn) && v >= remainingWidth) {
				if (clipX.right != 0)
					src = SkipRestOfCelLine(src, srcWidth - clipX.width - (v - remainingWidth));
				return src;
			}
			dst += v;
		}
		position.x += v;
		remainingWidth -= v;
		if (!(ClipWidth || CheckLastColumn) && remainingWidth == 0) {
			if (ClipWidth && clipX.right != 0)
				src = SkipRestOfCelLine(src, srcWidth - clipX.width);
			return src;
		}
	}

	while (ClipWidth || CheckLastColumn || remainingWidth > 0) {
		v = static_cast<std::uint8_t>(*src++);
		if (!IsCelTransparent(v)) {
			if (handleOvershoot())
				return src;
			if (!handleEdgePixels(v)) {
				RenderOutlineForPixels<SkipColorIndexZero, North, West, South, East>(
				    dst, dstPitch, reinterpret_cast<const std::uint8_t *>(src), v, color);
				src += v, dst += v;
			}
		} else {
			v = GetCelTransparentWidth(v);
			dst += v;
			if ((ClipWidth || CheckLastColumn) && v >= remainingWidth) {
				if (clipX.right != 0)
					src = SkipRestOfCelLine(src, srcWidth - clipX.width - (v - remainingWidth));
				return src;
			}
		}
		remainingWidth -= v;
		position.x += v;
	}

	return src;
}

template <bool SkipColorIndexZero>
void RenderCelOutlineClippedY(const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize, // NOLINT(readability-function-cognitive-complexity)
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
		    out, position, src, srcWidth, clipX, color);
		--position.y;
	}
	if (src == srcEnd)
		return;

	if (position.y + 1 == dstHeight) {
		// Bottom line - cannot draw south.
		src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true>(
		    out, position, src, srcWidth, clipX, color);
		--position.y;
	}

	while (position.y > 0 && src != srcEnd) {
		src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true>(
		    out, position, src, srcWidth, clipX, color);
		--position.y;
	}
	if (src == srcEnd)
		return;

	if (position.y == 0) {
		src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true>(
		    out, position, src, srcWidth, clipX, color);
		--position.y;
	}
	if (src == srcEnd)
		return;

	if (position.y == -1) {
		// Special case: the top of the sprite is 1px below the last line, render just the outline above.
		RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/false, /*West=*/false, /*South=*/true, /*East=*/false>(
		    out, position, src, srcWidth, clipX, color);
	}
}

template <bool SkipColorIndexZero>
void RenderCelOutlineClippedXY(const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize, // NOLINT(readability-function-cognitive-complexity)
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
		    out, position, src, srcWidth, clipX, color);
		--position.y;
	}
	if (src == srcEnd)
		return;

	if (position.y + 1 == dstHeight) {
		// Bottom line - cannot draw south.
		if (position.x <= 0) {
			src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/true, /*CheckLastColumn=*/false>(
			    out, position, src, srcWidth, clipX, color);
		} else if (position.x + clipX.width >= out.w()) {
			src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/false, /*CheckLastColumn=*/true>(
			    out, position, src, srcWidth, clipX, color);
		} else {
			src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/true, /*West=*/true, /*South=*/false, /*East=*/true,
			    /*ClipWidth=*/true>(
			    out, position, src, srcWidth, clipX, color);
		}
		--position.y;
	}

	if (position.x <= 0) {
		while (position.y > 0 && src != srcEnd) {
			src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/true, /*CheckLastColumn=*/false>(
			    out, position, src, srcWidth, clipX, color);
			--position.y;
		}
	} else if (position.x + clipX.width >= out.w()) {
		while (position.y > 0 && src != srcEnd) {
			src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/false, /*CheckLastColumn=*/true>(
			    out, position, src, srcWidth, clipX, color);
			--position.y;
		}
	} else {
		while (position.y > 0 && src != srcEnd) {
			src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/true, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true>(
			    out, position, src, srcWidth, clipX, color);
			--position.y;
		}
	}
	if (src == srcEnd)
		return;

	if (position.y == 0) {
		if (position.x <= 0) {
			src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/true, /*CheckLastColumn=*/false>(
			    out, position, src, srcWidth, clipX, color);
		} else if (position.x + clipX.width >= out.w()) {
			src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true, /*CheckFirstColumn=*/false, /*CheckLastColumn=*/true>(
			    out, position, src, srcWidth, clipX, color);
		} else {
			src = RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/false, /*West=*/true, /*South=*/true, /*East=*/true,
			    /*ClipWidth=*/true>(
			    out, position, src, srcWidth, clipX, color);
		}
		--position.y;
	}
	if (src == srcEnd)
		return;

	if (position.y == -1) {
		// Special case: the top of the sprite is 1px below the last line, render just the outline above.
		RenderCelOutlineRowClipped<SkipColorIndexZero, /*North=*/false, /*West=*/false, /*South=*/true, /*East=*/false,
		    /*ClipWidth=*/true>(
		    out, position, src, srcWidth, clipX, color);
	}
}

template <bool SkipColorIndexZero>
void RenderCelOutline(const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize,
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
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 */
void CelBlitSafeTo(const CelOutputBuffer &out, int sx, int sy, const byte *pRLEBytes, int nDataSize, int nWidth)
{
	assert(pRLEBytes != nullptr);
	RenderCel(out, { sx, sy }, pRLEBytes, nDataSize, nWidth, RenderLineMemcpy, NullLineEndFn);
}

/**
 * @brief Same as CelBlitLightSafeTo but with stippled transparency applied
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 */
void CelBlitLightTransSafeTo(const CelOutputBuffer &out, int sx, int sy, const byte *pRLEBytes, int nDataSize, int nWidth)
{
	assert(pRLEBytes != nullptr);
	const std::uint8_t *tbl = &pLightTbl[light_table_index * 256];
	const Point from { sx, sy };
	bool shift = (reinterpret_cast<uintptr_t>(&out[from]) % 2 == 1);
	const bool pitchIsEven = (out.pitch() % 2 == 0);
	RenderCel(
	    out, from, pRLEBytes, nDataSize, nWidth,
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
void CelBlitLightBlendedSafeTo(const CelOutputBuffer &out, int sx, int sy, const byte *pRLEBytes, int nDataSize, int nWidth, uint8_t *tbl)
{
	assert(pRLEBytes != nullptr);
	if (tbl == nullptr)
		tbl = &pLightTbl[light_table_index * 256];

	RenderCel(
	    out, { sx, sy }, pRLEBytes, nDataSize, nWidth, [tbl](std::uint8_t *dst, const uint8_t *src, std::size_t w) {
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
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 * @param tbl Palette translation table
 */
void CelBlitLightSafeTo(const CelOutputBuffer &out, int sx, int sy, const byte *pRLEBytes, int nDataSize, int nWidth, uint8_t *tbl)
{
	assert(pRLEBytes != nullptr);
	if (tbl == nullptr)
		tbl = &pLightTbl[light_table_index * 256];
	RenderCelWithLightTable(out, { sx, sy }, pRLEBytes, nDataSize, nWidth, tbl);
}

} // namespace

void CelDrawTo(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame)
{
	int nDataSize;
	const auto *pRLEBytes = CelGetFrame(cel.Data(), frame, &nDataSize);
	CelBlitSafeTo(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame));
}

void CelClippedDrawTo(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame)
{
	int nDataSize;
	const auto *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);

	CelBlitSafeTo(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame));
}

void CelDrawLightTo(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame, uint8_t *tbl)
{
	int nDataSize;
	const auto *pRLEBytes = CelGetFrame(cel.Data(), frame, &nDataSize);

	if (light_table_index != 0 || tbl != nullptr)
		CelBlitLightSafeTo(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame), tbl);
	else
		CelBlitSafeTo(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame));
}

void CelClippedDrawLightTo(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame)
{
	int nDataSize;
	const auto *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);

	if (light_table_index != 0)
		CelBlitLightSafeTo(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame), nullptr);
	else
		CelBlitSafeTo(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame));
}

void CelDrawLightRedTo(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame, char light)
{
	int nDataSize;
	const auto *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);
	const std::uint8_t *tbl = GetLightTable(light);
	RenderCelWithLightTable(out, { sx, sy }, pRLEBytes, nDataSize, cel.Width(frame), tbl);
}

void CelClippedDrawSafeTo(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame)
{
	int nDataSize;
	const auto *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);
	CelBlitSafeTo(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame));
}

void CelClippedBlitLightTransTo(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame)
{
	int nDataSize;
	const byte *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);

	if (cel_transparency_active) {
		if (sgOptions.Graphics.bBlendedTransparancy)
			CelBlitLightBlendedSafeTo(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame), nullptr);
		else
			CelBlitLightTransSafeTo(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame));
	} else if (light_table_index != 0)
		CelBlitLightSafeTo(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame), nullptr);
	else
		CelBlitSafeTo(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame));
}

void CelDrawLightRedSafeTo(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame, char light)
{
	int nDataSize;
	const auto *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);
	RenderCelWithLightTable(out, { sx, sy }, pRLEBytes, nDataSize, cel.Width(frame), GetLightTable(light));
}

void CelDrawUnsafeTo(const CelOutputBuffer &out, int x, int y, const CelSprite &cel, int frame)
{
	int nDataSize;
	const auto *pRLEBytes = CelGetFrame(cel.Data(), frame, &nDataSize);
	RenderCelClipY(out, { x, y }, pRLEBytes, nDataSize, cel.Width(frame), RenderLineMemcpy, NullLineEndFn);
}

void CelBlitOutlineTo(const CelOutputBuffer &out, uint8_t col, int sx, int sy, const CelSprite &cel, int frame, bool skipColorIndexZero)
{
	int nDataSize;
	const byte *src = CelGetFrameClipped(cel.Data(), frame, &nDataSize);
	if (skipColorIndexZero)
		RenderCelOutline<true>(out, { sx, sy }, src, nDataSize, cel.Width(frame), col);
	else
		RenderCelOutline<false>(out, { sx, sy }, src, nDataSize, cel.Width(frame), col);
}

std::pair<int, int> MeasureSolidHorizontalBounds(const CelSprite &cel, int frame)
{
	int nDataSize;
	auto src = reinterpret_cast<const uint8_t *>(CelGetFrame(cel.Data(), frame, &nDataSize));
	auto end = &src[nDataSize];
	const int celWidth = cel.Width(frame);

	int xBegin = celWidth;
	int xEnd = 0;

	int transparentRun = 0;
	int xCur = 0;
	bool firstTransparentRun = true;
	while (src < end) {
		std::int_fast16_t remainingWidth = celWidth;
		while (remainingWidth > 0) {
			const auto val = static_cast<std::uint8_t>(*src++);
			if (IsCelTransparent(val)) {
				const int width = GetCelTransparentWidth(val);
				transparentRun += width;
				xCur += width;
				remainingWidth -= width;
				if (remainingWidth == 0) {
					xEnd = std::max(xEnd, celWidth - transparentRun);
					xCur = 0;
					firstTransparentRun = true;
					transparentRun = 0;
				}
			} else {
				if (firstTransparentRun) {
					xBegin = std::min(xBegin, transparentRun);
					firstTransparentRun = false;
					if (xBegin == 0 && xEnd == celWidth) {
						return { xBegin, xEnd };
					}
				}
				transparentRun = 0;
				xCur += val;
				src += val;
				remainingWidth -= val;
				if (remainingWidth == 0) {
					xEnd = celWidth;
					if (xBegin == 0) {
						return { xBegin, xEnd };
					}
					xCur = 0;
					firstTransparentRun = true;
				}
			}
		}
	}
	return { xBegin, xEnd };
}

} // namespace devilution
