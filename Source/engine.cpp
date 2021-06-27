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

namespace devilution {

/** Current game seed */
uint32_t sglGameSeed;

/**
 * Specifies the increment used in the Borland C/C++ pseudo-random.
 */
const uint32_t RndInc = 1;

/**
 * Specifies the multiplier used in the Borland C/C++ pseudo-random number generator algorithm.
 */
const uint32_t RndMult = 0x015A4E35;

namespace {

template <bool SkipColorIndexZero>
void BufferBlit(const CelOutputBuffer &src, SDL_Rect srcRect, const CelOutputBuffer &dst, Point dstPosition)
{
	// We do not use `SDL_BlitSurface` here because the palettes may be different objects
	// and SDL would attempt to map them.

	dst.Clip(&srcRect, &dstPosition);
	if (srcRect.w <= 0 || srcRect.h <= 0)
		return;

	const std::uint8_t *srcBuf = src.at(srcRect.x, srcRect.y);
	const auto srcPitch = src.pitch();
	std::uint8_t *dstBuf = &dst[dstPosition];
	const auto dstPitch = dst.pitch();

	for (unsigned h = srcRect.h; h != 0; --h) {
		if (SkipColorIndexZero) {
			for (unsigned w = srcRect.w; w != 0; --w) {
				if (*srcBuf != 0)
					*dstBuf = *srcBuf;
				++srcBuf, ++dstBuf;
			}
			srcBuf += srcPitch - srcRect.w;
			dstBuf += dstPitch - srcRect.w;
		} else {
			std::memcpy(dstBuf, srcBuf, srcRect.w);
			srcBuf += srcPitch;
			dstBuf += dstPitch;
		}
	}
}

} // namespace

void CelOutputBuffer::BlitFrom(const CelOutputBuffer &src, SDL_Rect srcRect, Point targetPosition) const
{
	BufferBlit</*SkipColorIndexZero=*/false>(src, srcRect, *this, targetPosition);
}

void CelOutputBuffer::BlitFromSkipColorIndexZero(const CelOutputBuffer &src, SDL_Rect srcRect, Point targetPosition) const
{
	BufferBlit</*SkipColorIndexZero=*/true>(src, srcRect, *this, targetPosition);
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
		width = out.w() - from.x;
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
	if (sx + width < 0)
		return;
	if (sy + height < 0)
		return;
	if (sx >= out.w())
		return;
	if (sy >= out.h())
		return;

	if (sx < 0) {
		width += sx;
		sx = 0;
	} else if (sx + width >= out.w()) {
		width = out.w() - sx;
	}

	if (sy < 0) {
		height += sy;
		sy = 0;
	} else if (sy + height >= out.h()) {
		height = out.h() - sy;
	}

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
Direction GetDirection(Point start, Point destination)
{
	Direction md = DIR_S;

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
void SetRndSeed(uint32_t s)
{
	sglGameSeed = s;
}

/**
 * @brief Advance the internal RNG seed and return the new value
 * @return RNG seed
 */
int32_t AdvanceRndSeed()
{
	sglGameSeed = (RndMult * sglGameSeed) + RndInc;
	return GetRndSeed();
}

/**
 * @brief Get the current RNG seed
 * @return RNG seed
 */
int32_t GetRndSeed()
{
	return abs(static_cast<int32_t>(sglGameSeed));
}

uint32_t GetLCGEngineState()
{
	return sglGameSeed;
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
	scrollrt_draw_game_screen();
	PaletteFadeIn(8);
	force_redraw = 255;
}

} // namespace devilution
