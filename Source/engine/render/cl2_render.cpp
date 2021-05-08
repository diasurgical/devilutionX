/**
 * @file cl2_render.cpp
 *
 * CL2 rendering.
 */
#include "cl2_render.hpp"

#include <algorithm>

#include "engine/render/common_impl.h"
#include "scrollrt.h"

namespace devilution {
namespace {

/**
 * CL2 is similar to CEL, with the following differences:
 *
 * 1. Transparent runs can cross line boundaries.
 * 2. Control bytes are different, and the [0x80, 0xBE] control byte range
 *    indicates a fill-N command.
 */

constexpr std::uint8_t MaxCl2Width = 65;

constexpr bool IsCl2Opaque(std::uint8_t control)
{
	constexpr std::uint8_t Cl2OpaqueMin = 0x80;
	return control >= Cl2OpaqueMin;
}

constexpr std::uint8_t GetCl2Cl2OpaquePixelsWidth(std::uint8_t control)
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
SkipSize GetSkipSize(std::int_fast16_t overrun, std::int_fast16_t srcWidth)
{
	SkipSize result;
	result.wholeLines = overrun / srcWidth;
	result.xOffset = overrun - srcWidth * result.wholeLines;
	return result;
}

// Debugging variables
// #define DEBUG_RENDER_COLOR 213 // orange-ish hue

const byte *SkipRestOfCl2Line(
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
				v = GetCl2Cl2OpaquePixelsWidth(v);
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
void RenderCl2ClipY(const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize, std::size_t srcWidth,
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
					v = GetCl2Cl2OpaquePixelsWidth(v);
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
void RenderCl2ClipXY( // NOLINT(readability-function-cognitive-complexity)
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
					v = GetCl2Cl2OpaquePixelsWidth(v);
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
					v = GetCl2Cl2OpaquePixelsWidth(v);
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
void RenderCl2(const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize, std::size_t srcWidth,
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
 * @brief Blit a solid colder shape one pixel larger then the given sprite shape, to the given buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CL2 pixel stream (run-length encoded)
 * @param nDataSize Size of CL2 in bytes
 * @param nWidth Width of sprite
 * @param col Color index from current palette
 */
void Cl2BlitOutlineSafe(const CelOutputBuffer &out, int sx, int sy, const byte *pRLEBytes, int nDataSize, int nWidth, uint8_t col)
{
	const byte *src = pRLEBytes;
	BYTE *dst = out.at(sx, sy);
	int w = nWidth;

	while (nDataSize > 0) {
		auto width = static_cast<std::int8_t>(*src++);
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > MaxCl2Width) {
				width -= MaxCl2Width;
				nDataSize--;
				if (static_cast<std::uint8_t>(*src++) != 0 && dst < out.end() && dst > out.begin()) {
					w -= width;
					dst[-1] = col;
					dst[width] = col;
					while (width > 0) {
						dst[-out.pitch()] = col;
						dst[out.pitch()] = col;
						dst++;
						width--;
					}
					if (w == 0) {
						w = nWidth;
						dst -= out.pitch() + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				if (dst < out.end() && dst > out.begin()) {
					w -= width;
					while (width > 0) {
						if (static_cast<std::uint8_t>(*src) != 0) {
							dst[-1] = col;
							dst[1] = col;
							dst[-out.pitch()] = col;
							// BUGFIX: only set `if (dst+out.pitch() < out.end())`
							dst[out.pitch()] = col;
						}
						src++;
						dst++;
						width--;
					}
					if (w == 0) {
						w = nWidth;
						dst -= out.pitch() + w;
					}
					continue;
				}
				src += width;
			}
		}
		while (width > 0) {
			if (width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if (w == 0) {
				w = nWidth;
				dst -= out.pitch() + w;
			}
		}
	}
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

} // namespace

void Cl2ApplyTrans(byte *p, const std::array<uint8_t, 256> &ttbl, int nCel)
{
	assert(p != nullptr);

	for (int i = 1; i <= nCel; i++) {
		int nDataSize;
		byte *dst = CelGetFrame(p, i, &nDataSize) + 10;
		nDataSize -= 10;
		while (nDataSize > 0) {
			auto width = static_cast<std::int8_t>(*dst++);
			nDataSize--;
			assert(nDataSize >= 0);
			if (width < 0) {
				width = -width;
				if (width > MaxCl2Width) {
					nDataSize--;
					assert(nDataSize >= 0);
					*dst = static_cast<byte>(ttbl[static_cast<std::uint8_t>(*dst)]);
					dst++;
				} else {
					nDataSize -= width;
					assert(nDataSize >= 0);
					for (; width > 0; width--) {
						*dst = static_cast<byte>(ttbl[static_cast<std::uint8_t>(*dst)]);
						dst++;
					}
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

	const CelOutputBuffer &sub = out.subregionY(0, out.h() - 1);
	Cl2BlitOutlineSafe(sub, sx, sy, pRLEBytes, nDataSize, cel.Width(frame), col);
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
