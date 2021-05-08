/**
 * @file cel_render.hpp
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

/** Renders a CEL with only vertical clipping to the output buffer. */
template <typename RenderLine>
void RenderCelClipY(const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize, std::size_t srcWidth, const RenderLine &renderLine)
{
	const auto *srcEnd = src + srcSize;

	// Skip the bottom clipped lines.
	const auto dstHeight = out.h();
	while (position.y >= dstHeight && src != srcEnd) {
		src = SkipRestOfCelLine(src, static_cast<std::int_fast16_t>(srcWidth));
		--position.y;
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
	}
}

/** Renders a CEL with both horizontal and vertical clipping to the output buffer. */
template <typename RenderLine>
void RenderCelClipXY(const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize, std::size_t srcWidth, ClipX clipX, const RenderLine &renderLine)
{
	const auto *srcEnd = src + srcSize;

	// Skip the bottom clipped lines.
	const auto dstHeight = out.h();
	while (position.y >= dstHeight && src != srcEnd) {
		src = SkipRestOfCelLine(src, static_cast<std::int_fast16_t>(srcWidth));
		--position.y;
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
	}
}

template <typename RenderLine>
void RenderCel(const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize, std::size_t srcWidth, const RenderLine &renderLine)
{
	const ClipX clipX = CalculateClipX(position.x, srcWidth, out);
	if (clipX.width <= 0)
		return;
	if (static_cast<std::size_t>(clipX.width) == srcWidth) {
		RenderCelClipY(out, position, src, srcSize, srcWidth, renderLine);
	} else {
		RenderCelClipXY(out, position, src, srcSize, srcWidth, clipX, renderLine);
	}
}

void RenderCelWithLightTable(const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize, std::size_t srcWidth, const std::uint8_t *tbl)
{
	RenderCel(out, position, src, srcSize, srcWidth, [tbl](std::uint8_t *dst, const std::uint8_t *src, std::size_t w) {
		while (w-- > 0) {
			*dst++ = tbl[static_cast<std::uint8_t>(*src)];
			++src;
		}
	});
}

constexpr auto RenderLineMemcpy = [](std::uint8_t *dst, const std::uint8_t *src, std::size_t w) {
	std::memcpy(dst, src, w);
};

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

void CelBlitSafeTo(const CelOutputBuffer &out, int sx, int sy, const byte *pRLEBytes, int nDataSize, int nWidth)
{
	assert(pRLEBytes != nullptr);
	RenderCel(out, { sx, sy }, pRLEBytes, nDataSize, nWidth, RenderLineMemcpy);
}

void CelClippedDrawSafeTo(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame)
{
	int nDataSize;
	const auto *pRLEBytes = CelGetFrameClipped(cel.Data(), frame, &nDataSize);
	CelBlitSafeTo(out, sx, sy, pRLEBytes, nDataSize, cel.Width(frame));
}

void CelBlitLightSafeTo(const CelOutputBuffer &out, int sx, int sy, const byte *pRLEBytes, int nDataSize, int nWidth, uint8_t *tbl)
{
	assert(pRLEBytes != nullptr);
	if (tbl == nullptr)
		tbl = &pLightTbl[light_table_index * 256];
	RenderCelWithLightTable(out, { sx, sy }, pRLEBytes, nDataSize, nWidth, tbl);
}

void CelBlitLightTransSafeTo(const CelOutputBuffer &out, int sx, int sy, const byte *pRLEBytes, int nDataSize, int nWidth)
{
	assert(pRLEBytes != nullptr);

	const auto *src = pRLEBytes;
	BYTE *dst = out.at(sx, sy);
	const uint8_t *tbl = &pLightTbl[light_table_index * 256];
	bool shift = (reinterpret_cast<uintptr_t>(dst) % 2) != 0;

	for (; src != &pRLEBytes[nDataSize]; dst -= out.pitch() + nWidth, shift = !shift) {
		for (int w = nWidth; w > 0;) {
			auto width = static_cast<std::uint8_t>(*src++);
			if (!IsCelTransparent(width)) {
				w -= width;
				if (dst < out.end() && dst > out.begin()) {
					if (((size_t)dst % 2) == shift) {
						if ((width & 1) == 0) {
							goto L_ODD;
						} else {
							src++;
							dst++;
						L_EVEN:
							width /= 2;
							if ((width & 1) != 0) {
								dst[0] = tbl[static_cast<std::uint8_t>(src[0])];
								src += 2;
								dst += 2;
							}
							width /= 2;
							for (; width > 0; width--) {
								dst[0] = tbl[static_cast<std::uint8_t>(src[0])];
								dst[2] = tbl[static_cast<std::uint8_t>(src[2])];
								src += 4;
								dst += 4;
							}
						}
					} else {
						if ((width & 1) == 0) {
							goto L_EVEN;
						} else {
							dst[0] = tbl[static_cast<std::uint8_t>(src[0])];
							src++;
							dst++;
						L_ODD:
							width /= 2;
							if ((width & 1) != 0) {
								dst[1] = tbl[static_cast<std::uint8_t>(src[1])];
								src += 2;
								dst += 2;
							}
							width /= 2;
							for (; width > 0; width--) {
								dst[1] = tbl[static_cast<std::uint8_t>(src[1])];
								dst[3] = tbl[static_cast<std::uint8_t>(src[3])];
								src += 4;
								dst += 4;
							}
						}
					}
				} else {
					src += width;
					dst += width;
				}
			} else {
				width = -static_cast<std::int8_t>(width);
				dst += width;
				w -= width;
			}
		}
	}
}

/**
 * @brief Same as CelBlitLightSafe, with blended transparancy applied
 * @param out The output buffer
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 * @param nWidth Width of sprite
 * @param tbl Palette translation table
 */
static void CelBlitLightBlendedSafeTo(const CelOutputBuffer &out, int sx, int sy, const byte *pRLEBytes, int nDataSize, int nWidth, uint8_t *tbl)
{
	assert(pRLEBytes != nullptr);
	if (tbl == nullptr)
		tbl = &pLightTbl[light_table_index * 256];

	RenderCel(out, { sx, sy }, pRLEBytes, nDataSize, nWidth, [tbl](std::uint8_t *dst, const uint8_t *src, std::size_t w) {
		while (w-- > 0) {
			*dst = paletteTransparencyLookup[*dst][tbl[*src++]];
			++dst;
		}
	});
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
	RenderCelClipY(out, { x, y }, pRLEBytes, nDataSize, cel.Width(frame), RenderLineMemcpy);
}

void CelBlitOutlineTo(const CelOutputBuffer &out, uint8_t col, int sx, int sy, const CelSprite &cel, int frame, bool skipColorIndexZero)
{
	int nDataSize;

	const byte *src = CelGetFrameClipped(cel.Data(), frame, &nDataSize);
	const auto *end = &src[nDataSize];
	uint8_t *dst = out.at(sx, sy);
	const int celWidth = static_cast<int>(cel.Width(frame));

	for (; src != end; dst -= out.pitch() + celWidth) {
		for (int w = celWidth; w > 0;) {
			auto width = static_cast<std::uint8_t>(*src++);
			if (!IsCelTransparent(width)) {
				w -= width;
				if (dst < out.end() && dst > out.begin()) {
					if (dst >= out.end() - out.pitch()) {
						while (width > 0) {
							if (!skipColorIndexZero || static_cast<std::uint8_t>(*src) > 0) {
								dst[-out.pitch()] = col;
								dst[-1] = col;
								dst[1] = col;
							}
							src++;
							dst++;
							width--;
						}
					} else {
						while (width > 0) {
							if (!skipColorIndexZero || static_cast<std::uint8_t>(*src) > 0) {
								dst[-out.pitch()] = col;
								dst[-1] = col;
								dst[1] = col;
								dst[out.pitch()] = col;
							}
							src++;
							dst++;
							width--;
						}
					}
				} else {
					src += width;
					dst += width;
				}
			} else {
				width = GetCelTransparentWidth(width);
				dst += width;
				w -= width;
			}
		}
	}
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
