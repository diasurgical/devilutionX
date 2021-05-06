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

#include "engine/render/common_impl.h"
#include "lighting.h"
#include "movie.h"
#include "options.h"
#include "storm/storm.h"

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
constexpr std::uint8_t MaxCl2Width = 65;
} // namespace

CelSprite LoadCel(const char *pszName, int width)
{
	return CelSprite(LoadFileInMem(pszName), width);
}

CelSprite LoadCel(const char *pszName, const int *widths)
{
	return CelSprite(LoadFileInMem(pszName), widths);
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
