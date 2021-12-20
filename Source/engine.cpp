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
#include <cassert>
#include <cstdint>

#include "engine/render/common_impl.h"
#include "lighting.h"
#include "movie.h"
#include "options.h"

namespace devilution {
namespace {

// Expects everything to be 4-byte aligned.
void DrawHalfTransparentAligned32BlendedRectTo(const Surface &out, unsigned sx, unsigned sy, unsigned width, unsigned height)
{
	assert(out.pitch() % 4 == 0);

	auto *pix = reinterpret_cast<uint32_t *>(out.at(static_cast<int>(sx), static_cast<int>(sy)));
	assert(reinterpret_cast<intptr_t>(pix) % 4 == 0);

	const uint16_t *lookupTable = paletteTransparencyLookupBlack16;

	const unsigned skipX = (out.pitch() - width) / 4;
	width /= 4;
	while (height-- > 0) {
		for (unsigned i = 0; i < width; ++i, ++pix) {
			const uint32_t v = *pix;
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
			*pix = lookupTable[v & 0xFFFF] | (lookupTable[(v >> 16) & 0xFFFF] << 16);
#else
			*pix = lookupTable[(v >> 16) & 0xFFFF] | (lookupTable[v & 0xFFFF] << 16);
#endif
		}
		pix += skipX;
	}
}

void DrawHalfTransparentUnalignedBlendedRectTo(const Surface &out, unsigned sx, unsigned sy, unsigned width, unsigned height)
{
	uint8_t *pix = out.at(static_cast<int>(sx), static_cast<int>(sy));
	const uint8_t *lookupTable = paletteTransparencyLookup[0];
	const unsigned skipX = out.pitch() - width;
	for (unsigned y = 0; y < height; ++y) {
		for (unsigned x = 0; x < width; ++x, ++pix) {
			*pix = lookupTable[*pix];
		}
		pix += skipX;
	}
}

void DrawHalfTransparentBlendedRectTo(const Surface &out, unsigned sx, unsigned sy, unsigned width, unsigned height)
{
	// All SDL surfaces are 4-byte aligned and divisible by 4.
	// However, our coordinates and widths may not be.

	// First, draw the leading unaligned part.
	if (sx % 4 != 0) {
		const unsigned w = 4 - sx % 4;
		DrawHalfTransparentUnalignedBlendedRectTo(out, sx, sy, w, height);
		sx += w;
		width -= w;
	}

	if (static_cast<int>(sx + width) == out.w()) {
		// The pitch is 4-byte aligned, so we can simply extend the width to the pitch.
		width = out.pitch() - sx;
	} else if (width % 4 != 0) {
		// Draw the trailing unaligned part.
		const unsigned w = width % 4;
		DrawHalfTransparentUnalignedBlendedRectTo(out, sx + (width / 4) * 4, sy, w, height);
		width -= w;
	}

	// Now everything is divisible by 4. Draw the aligned part.
	DrawHalfTransparentAligned32BlendedRectTo(out, sx, sy, width, height);
}
} // namespace

void DrawHorizontalLine(const Surface &out, Point from, int width, std::uint8_t colorIndex)
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

void UnsafeDrawHorizontalLine(const Surface &out, Point from, int width, std::uint8_t colorIndex)
{
	std::memset(&out[from], colorIndex, width);
}

void DrawVerticalLine(const Surface &out, Point from, int height, std::uint8_t colorIndex)
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

void UnsafeDrawVerticalLine(const Surface &out, Point from, int height, std::uint8_t colorIndex)
{
	auto *dst = &out[from];
	const auto pitch = out.pitch();
	while (height-- > 0) {
		*dst = colorIndex;
		dst += pitch;
	}
}

void DrawHalfTransparentRectTo(const Surface &out, int sx, int sy, int width, int height)
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

	DrawHalfTransparentBlendedRectTo(out, sx, sy, width, height);
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
	Direction md;

	int mx = destination.x - start.x;
	int my = destination.y - start.y;
	if (mx >= 0) {
		if (my >= 0) {
			if (5 * mx <= (my * 2)) // mx/my <= 0.4, approximation of tan(22.5)
				return Direction::SouthWest;
			md = Direction::South;
		} else {
			my = -my;
			if (5 * mx <= (my * 2))
				return Direction::NorthEast;
			md = Direction::East;
		}
		if (5 * my <= (mx * 2)) // my/mx <= 0.4
			md = Direction::SouthEast;
	} else {
		mx = -mx;
		if (my >= 0) {
			if (5 * mx <= (my * 2))
				return Direction::SouthWest;
			md = Direction::West;
		} else {
			my = -my;
			if (5 * mx <= (my * 2))
				return Direction::NorthEast;
			md = Direction::North;
		}
		if (5 * my <= (mx * 2))
			md = Direction::NorthWest;
	}
	return md;
}

int CalculateWidth2(int width)
{
	return (width - 64) / 2;
}

} // namespace devilution
