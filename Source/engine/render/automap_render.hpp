/**
 * @file automap_render.hpp
 * Defines 2 sets of rendering primitives for drawing automap lines.
 *
 * 1. DrawMapLine* - used for rendering most map lines - 2 pixels horizontally for each pixel vertically.
 * 2. DrawMapLineSteep* - currently only used for rendering the player arrow - 2 pixels vertically for each pixel horizontally.
 *
 * These functions draw a single extra pixel at the end of the line -- they always draw an odd number of pixels.
 * These functions clip to the output buffer -- they are safe to call with out-of-bounds coordinates.
 */
#pragma once

#include <cstdint>

#include "engine.h"
#include "engine/point.hpp"

namespace devilution {

void SetMapPixel(const Surface &out, Point point, uint8_t color);
/**
 * @brief Draw a line in the target buffer from the given point towards north east at an `atan(1/2)` angle.
 *
 * Draws 2 horizontal pixels for each vertical step, then an additional one where it draws 1 pixel.
 *
 * The end point is at `{ from.x + 2 * height + 1, from.y - height }`.
 */
void DrawMapLineNE(const Surface &out, Point from, int height, std::uint8_t colorIndex);

/**
 * @brief Draw a line in the target buffer from the given point towards south east at an `atan(1/2)` angle.
 *
 * Draws 2 horizontal pixels for each vertical step, then an additional one where it draws 1 pixel.
 *
 * The end point is at `{ from.x + 2 * height + 1, from.y + height }`.
 */
void DrawMapLineSE(const Surface &out, Point from, int height, std::uint8_t colorIndex);

/**
 * @brief Draw a line in the target buffer from the given point towards north west at an `atan(1/2)` angle.
 *
 * Draws 2 horizontal pixels for each vertical step, then an additional one where it draws 1 pixel.
 *
 * The end point is at `{ from.x - 2 * height + 1, from.y - height }`.
 */
void DrawMapLineNW(const Surface &out, Point from, int height, std::uint8_t colorIndex);

/**
 * @brief Draw a line in the target buffer from the given point towards south west at an `atan(1/2)` angle.
 *
 * Draws 2 horizontal pixels for each vertical step, then an additional one where it draws 1 pixel.
 *
 * The end point is at `{ from.x - 2 * height + 1, from.y + height }`.
 */
void DrawMapLineSW(const Surface &out, Point from, int height, std::uint8_t colorIndex);

/**
 * @brief Draw a line in the target buffer from the given point towards north east at an `atan(1/2)` angle.
 *
 * Draws 2 vertical pixels for each horizontal step, then an additional one where it draws 1 pixel.
 *
 * The end point is at `{ from.x + width + 1, from.y - 2 * width }`.
 */
void DrawMapLineSteepNE(const Surface &out, Point from, int width, std::uint8_t colorIndex);

/**
 * @brief Draw a line in the target buffer from the given point towards south east at an `atan(2)` angle.
 *
 * Draws 2 vertical pixels for each horizontal step, then an additional one where it draws 1 pixel.
 *
 * The end point is at `{ from.x + width + 1, from.y + 2 * width }`.
 */
void DrawMapLineSteepSE(const Surface &out, Point from, int width, std::uint8_t colorIndex);

/**
 * @brief Draw a line in the target buffer from the given point towards north west at an `atan(1/2)` angle.
 *
 * Draws 2 vertical pixels for each horizontal step, then an additional one where it draws 1 pixel.
 *
 * The end point is at `{ from.x - (width + 1), from.y - 2 * width }`.
 */
void DrawMapLineSteepNW(const Surface &out, Point from, int width, std::uint8_t colorIndex);

/**
 * @brief Draw a line in the target buffer from the given point towards south west at an `atan(1/2)` angle.
 *
 * Draws 2 vertical pixels for each horizontal step, then an additional one where it draws 1 pixel.
 *
 * The end point is at `{ from.x - (width + 1), from.y + 2 * width }`.
 */
void DrawMapLineSteepSW(const Surface &out, Point from, int width, std::uint8_t colorIndex);
void DrawMapEllipse(const Surface &out, Point from, int radius, uint8_t colorIndex);
void DrawMapStar(const Surface &out, Point from, int radius, uint8_t colorIndex);

} // namespace devilution
