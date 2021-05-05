/**
 * @file engine.cpp
 *
 * Implementation of basic engine helper functions:
 * - Sprite blitting
 * - Drawing
 * - Angle calculation
 * - RNG
 * - Memory allocation
 * - File loading
 * - Video playback
 */

#include <array>

#include "lighting.h"
#include "movie.h"
#include "options.h"
#include "storm/storm.h"

// TODO: temporary, remove.
#include "utils/log.hpp"

namespace devilution {

/** Seed value before the most recent call to SetRndSeed() */
int32_t orgseed;
/** Current game seed */
int32_t sglGameSeed;

/**
 * Specifies the increment used in the Borland C/C++ pseudo-random.
 */
const uint32_t RndInc = 1;

/**
 * Specifies the multiplier used in the Borland C/C++ pseudo-random number generator algorithm.
 */
const uint32_t RndMult = 0x015A4E35;

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

constexpr std::uint8_t MaxCl2Width = 65;

uint8_t *GetLightTable(char light)
{
	int idx = 4096;
	if (light == 2)
		idx += 256; // gray colors
	if (light >= 4)
		idx += (light - 1) << 8;
	return &pLightTbl[idx];
}

struct ClipX {
	std::int_fast16_t left;
	std::int_fast16_t right;
	std::int_fast16_t width;
};

ClipX CalculateClipX(std::int_fast16_t x, std::size_t w, const CelOutputBuffer &out)
{
	ClipX clip;
	clip.left = static_cast<std::int_fast16_t>(x < 0 ? -x : 0);
	clip.right = static_cast<std::int_fast16_t>(static_cast<std::int_fast16_t>(x + w) > out.w() ? x + w - out.w() : 0);
	clip.width = static_cast<std::int_fast16_t>(w - clip.left - clip.right);
	return clip;
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
void RenderCelClipXY(const CelOutputBuffer &out, Point position, const byte *src, std::size_t srcSize, std::size_t srcWidth, ClipX clipX, const RenderLine &renderLine) {
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

CelSprite LoadCel(const char *pszName, int width)
{
	return CelSprite(LoadFileInMem(pszName), width);
}

CelSprite LoadCel(const char *pszName, const int *widths)
{
	return CelSprite(LoadFileInMem(pszName), widths);
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
					if (xBegin == 0 && xEnd == celWidth)
						break;
				}
				transparentRun = 0;
				xCur += val;
				src += val;
				remainingWidth -= val;
				if (remainingWidth == 0) {
					xEnd = celWidth;
					if (xBegin == 0)
						break;
					xCur = 0;
					firstTransparentRun = true;
				}
			}
		}
	}
	return { xBegin, xEnd };
}

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

void DrawHorizontalLine(const CelOutputBuffer &out, Point from, int width, std::uint8_t colorIndex)
{
	if (from.y < 0 || from.y >= out.h() || from.x >= out.w() || width <= 0 || from.x + width <= 0)
		return;
	if (from.x < 0) {
		width += from.x;
		from.x = 0;
	}
	if (from.x + width > out.w())
		width = (from.x + width) - out.w();
	return UnsafeDrawHorizontalLine(out, from, width, colorIndex);
}

void UnsafeDrawHorizontalLine(const CelOutputBuffer &out, Point from, int width, std::uint8_t colorIndex)
{
	std::memset(&out[from], colorIndex, width);
}

void DrawVerticalLine(const CelOutputBuffer &out, Point from, int height, std::uint8_t colorIndex)
{
	if (from.x < 0 || from.x >= out.w() || from.y >= out.h() || height <= 0 || from.y + height <= 0)
		return;
	if (from.y < 0) {
		height += from.y;
		from.y = 0;
	}
	if (from.y + height > out.h())
		height = (from.y + height) - out.h();
	return UnsafeDrawVerticalLine(out, from, height, colorIndex);
}

void UnsafeDrawVerticalLine(const CelOutputBuffer &out, Point from, int height, std::uint8_t colorIndex)
{
	auto *dst = &out[from];
	const auto pitch = out.pitch();
	while (height-- > 0) {
		*dst = colorIndex;
		dst += pitch;
	}
}

static void DrawHalfTransparentBlendedRectTo(const CelOutputBuffer &out, int sx, int sy, int width, int height)
{
	BYTE *pix = out.at(sx, sy);

	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			*pix = paletteTransparencyLookup[0][*pix];
			pix++;
		}
		pix += out.pitch() - width;
	}
}

static void DrawHalfTransparentStippledRectTo(const CelOutputBuffer &out, int sx, int sy, int width, int height)
{
	BYTE *pix = out.at(sx, sy);

	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			if (((row & 1) != 0 && (col & 1) != 0) || ((row & 1) == 0 && (col & 1) == 0))
				*pix = 0;
			pix++;
		}
		pix += out.pitch() - width;
	}
}

void DrawHalfTransparentRectTo(const CelOutputBuffer &out, int sx, int sy, int width, int height)
{
	if (sgOptions.Graphics.bBlendedTransparancy) {
		DrawHalfTransparentBlendedRectTo(out, sx, sy, width, height);
	} else {
		DrawHalfTransparentStippledRectTo(out, sx, sy, width, height);
	}
}

/**
 * @brief Returns the direction a vector from p1(x1, y1) to p2(x2, y2) is pointing to.
 *
 *      W    SW     S
 *            ^
 *            |
 *     NW ----+---> SE
 *            |
 *            |
 *      N    NE     E
 *
 * @param x1 the x coordinate of p1
 * @param y1 the y coordinate of p1
 * @param x2 the x coordinate of p2
 * @param y2 the y coordinate of p2
 * @return the direction of the p1->p2 vector
*/
direction GetDirection(Point start, Point destination)
{
	direction md = DIR_S;

	int mx = destination.x - start.x;
	int my = destination.y - start.y;
	if (mx >= 0) {
		if (my >= 0) {
			if (5 * mx <= (my * 2)) // mx/my <= 0.4, approximation of tan(22.5)
				return DIR_SW;
			md = DIR_S;
		} else {
			my = -my;
			if (5 * mx <= (my * 2))
				return DIR_NE;
			md = DIR_E;
		}
		if (5 * my <= (mx * 2)) // my/mx <= 0.4
			md = DIR_SE;
	} else {
		mx = -mx;
		if (my >= 0) {
			if (5 * mx <= (my * 2))
				return DIR_SW;
			md = DIR_W;
		} else {
			my = -my;
			if (5 * mx <= (my * 2))
				return DIR_NE;
			md = DIR_N;
		}
		if (5 * my <= (mx * 2))
			md = DIR_NW;
	}
	return md;
}

int CalculateWidth2(int width)
{
	return (width - 64) / 2;
}

/**
 * @brief Set the RNG seed
 * @param s RNG seed
 */
void SetRndSeed(int32_t s)
{
	sglGameSeed = s;
	orgseed = s;
}

/**
 * @brief Advance the internal RNG seed and return the new value
 * @return RNG seed
 */
int32_t AdvanceRndSeed()
{
	sglGameSeed = (RndMult * static_cast<uint32_t>(sglGameSeed)) + RndInc;
	return abs(sglGameSeed);
}

/**
 * @brief Get the current RNG seed
 * @return RNG seed
 */
int32_t GetRndSeed()
{
	return abs(sglGameSeed);
}

/**
 * @brief Main RNG function
 * @param v The upper limit for the return value
 * @return A random number from 0 to (v-1)
 */
int32_t GenerateRnd(int32_t v)
{
	if (v <= 0)
		return 0;
	if (v < 0xFFFF)
		return (AdvanceRndSeed() >> 16) % v;
	return AdvanceRndSeed() % v;
}

size_t GetFileSize(const char *pszName)
{
	HANDLE file;
	SFileOpenFile(pszName, &file);
	const size_t fileLen = SFileGetFileSize(file, nullptr);
	SFileCloseFile(file);

	return fileLen;
}

void LoadFileData(const char *pszName, byte *buffer, size_t fileLen)
{
	HANDLE file;
	SFileOpenFile(pszName, &file);

	if (fileLen == 0)
		app_fatal("Zero length SFILE:\n%s", pszName);

	SFileReadFileThreadSafe(file, buffer, fileLen);
	SFileCloseFile(file);
}

/**
 * @brief Apply the color swaps to a CL2 sprite
 * @param p CL2 buffer
 * @param ttbl Palette translation table
 * @param nCel Frame number in CL2 file
 */
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

/**
 * @brief Blit CL2 sprite to the given buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CL2 pixel stream (run-length encoded)
 * @param nDataSize Size of CL2 in bytes
 * @param nWidth Width of sprite
 */
static void Cl2BlitSafe(const CelOutputBuffer &out, int sx, int sy, const byte *pRLEBytes, int nDataSize, int nWidth)
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
				const auto fill = static_cast<std::uint8_t>(*src++);
				if (dst < out.end() && dst > out.begin()) {
					w -= width;
					while (width > 0) {
						*dst = fill;
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
						*dst = static_cast<std::uint8_t>(*src);
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
 * @brief Blit a solid colder shape one pixel larger then the given sprite shape, to the given buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CL2 pixel stream (run-length encoded)
 * @param nDataSize Size of CL2 in bytes
 * @param nWidth Width of sprite
 * @param col Color index from current palette
 */
static void Cl2BlitOutlineSafe(const CelOutputBuffer &out, int sx, int sy, const byte *pRLEBytes, int nDataSize, int nWidth, uint8_t col)
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
static void Cl2BlitLightSafe(const CelOutputBuffer &out, int sx, int sy, const byte *pRLEBytes, int nDataSize, int nWidth, uint8_t *pTable)
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
				const uint8_t fill = pTable[static_cast<std::uint8_t>(*src++)];
				if (dst < out.end() && dst > out.begin()) {
					w -= width;
					while (width > 0) {
						*dst = fill;
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
						*dst = pTable[static_cast<std::uint8_t>(*src)];
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

/**
 * @brief Fade to black and play a video
 * @param pszMovie file path of movie
 */
void PlayInGameMovie(const char *pszMovie)
{
	PaletteFadeOut(8);
	play_movie(pszMovie, false);
	ClearScreenBuffer();
	force_redraw = 255;
	scrollrt_draw_game_screen(true);
	PaletteFadeIn(8);
	force_redraw = 255;
}

} // namespace devilution
