/**
 * @file engine.h
 *
 *  of basic engine helper functions:
 * - Sprite blitting
 * - Drawing
 * - Angle calculation
 * - Memory allocation
 * - File loading
 * - Video playback
 */
#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <utility>

// We include `cinttypes` here so that it is included before `inttypes.h`
// to work around a bug in older GCC versions on some platforms,
// where including `inttypes.h` before `cinttypes` leads to missing
// defines for `PRIuMAX` et al. SDL transitively includes `inttypes.h`.
// See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=97044
#include <cinttypes>
#include <cstddef>

#include <SDL.h>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#include "appfat.h"
#include "engine/point.hpp"
#include "engine/size.hpp"
#include "engine/surface.hpp"
#include "utils/attributes.h"

namespace devilution {

template <typename V, typename X, typename... Xs>
DVL_ALWAYS_INLINE constexpr bool IsAnyOf(const V &v, X x, Xs... xs)
{
	return v == x || ((v == xs) || ...);
}

template <typename V, typename X, typename... Xs>
DVL_ALWAYS_INLINE constexpr bool IsNoneOf(const V &v, X x, Xs... xs)
{
	return v != x && ((v != xs) && ...);
}

/**
 * @brief Fill a rectangle with the given color.
 */
void FillRect(const Surface &out, int x, int y, int width, int height, uint8_t colorIndex);

/**
 * @brief Draw a horizontal line segment in the target buffer (left to right)
 * @param out Target buffer
 * @param from Start of the line segment
 * @param width
 * @param colorIndex Color index from current palette
 */
void DrawHorizontalLine(const Surface &out, Point from, int width, std::uint8_t colorIndex);

/** Same as DrawHorizontalLine but without bounds clipping. */
void UnsafeDrawHorizontalLine(const Surface &out, Point from, int width, std::uint8_t colorIndex);

/**
 * @brief Draw a vertical line segment in the target buffer (top to bottom)
 * @param out Target buffer
 * @param from Start of the line segment
 * @param height
 * @param colorIndex Color index from current palette
 */
void DrawVerticalLine(const Surface &out, Point from, int height, std::uint8_t colorIndex);

/** Same as DrawVerticalLine but without bounds clipping. */
void UnsafeDrawVerticalLine(const Surface &out, Point from, int height, std::uint8_t colorIndex);

/**
 * Draws a half-transparent rectangle by palette blending with black.
 *
 * @brief Render a transparent black rectangle
 * @param out Target buffer
 * @param sx Screen coordinate
 * @param sy Screen coordinate
 * @param width Rectangle width
 * @param height Rectangle height
 */
void DrawHalfTransparentRectTo(const Surface &out, int sx, int sy, int width, int height);

/**
 * Draws a half-transparent pixel
 *
 * @brief Render a transparent pixel
 * @param out Target buffer
 * @param position Screen coordinates
 * @param col Pixel color
 */
void SetHalfTransparentPixel(const Surface &out, Point position, uint8_t color);

/**
 * Draws a 2px inset border.
 *
 * @param out Target buffer
 * @param rect The rectangle that border pixels are rendered inside of.
 * @param color Border color.
 */
void UnsafeDrawBorder2px(const Surface &out, Rectangle rect, uint8_t color);

/**
 * @brief Calculate the best fit direction between two points
 * @param start Tile coordinate
 * @param destination Tile coordinate
 * @return A value from the direction enum
 */
Direction GetDirection(Point start, Point destination);

/**
 * @brief Calculate Width2 from the orginal Width
 * Width2 is needed for savegame compatiblity and to render animations centered
 * @return Returns Width2
 */
int CalculateWidth2(int width);

inline int GetAnimationFrame(int frames, int fps = 60)
{
	int frame = (SDL_GetTicks() / fps) % frames;
	return frame > frames ? 0 : frame;
}

} // namespace devilution
