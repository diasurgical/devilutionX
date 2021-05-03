#include "engine.h"

namespace devilution {

/**
 * @brief Draw a line in the target buffer from the given point towards north east at an `atan(1/2)` angle.
 *
 * Draws 2 horizontal pixels for each vertical step, then an additional one where it draws 1 pixel.
 *
 * The end point is at `{ from.x + 2 * height + 1, from.y - height }`.
 */
void DrawMapLineNE(const CelOutputBuffer &out, Point from, int height, std::uint8_t colorIndex);

/**
 * @brief Draw a line in the target buffer from the given point towards south east at an `atan(1/2)` angle.
 *
 * Draws 2 horizontal pixels for each vertical step, then an additional one where it draws 1 pixel.
 *
 * The end point is at `{ from.x + 2 * height + 1, from.y + height }`.
 */
void DrawMapLineSE(const CelOutputBuffer &out, Point from, int height, std::uint8_t colorIndex);

/**
 * @brief Draw a line in the target buffer from the given point towards north west at an `atan(1/2)` angle.
 *
 * Draws 2 horizontal pixels for each vertical step, then an additional one where it draws 1 pixel.
 *
 * The end point is at `{ from.x - 2 * height + 1, from.y - height }`.
 */
void DrawMapLineNW(const CelOutputBuffer &out, Point from, int height, std::uint8_t colorIndex);

/**
 * @brief Draw a line in the target buffer from the given point towards south west at an `atan(1/2)` angle.
 *
 * Draws 2 horizontal pixels for each vertical step, then an additional one where it draws 1 pixel.
 *
 * The end point is at `{ from.x - 2 * height + 1, from.y + height }`.
 */
void DrawMapLineSW(const CelOutputBuffer &out, Point from, int height, std::uint8_t colorIndex);

/**
 * @brief Draw a line in the target buffer from the given point towards north east at an `atan(1/2)` angle.
 *
 * Draws 2 vertical pixels for each horizontal step, then an additional one where it draws 1 pixel.
 *
 * The end point is at `{ from.x + width + 1, from.y - 2 * width }`.
 */
void DrawMapLineNE2(const CelOutputBuffer &out, Point from, int width, std::uint8_t colorIndex);

/**
 * @brief Draw a line in the target buffer from the given point towards south east at an `atan(2)` angle.
 *
 * Draws 2 vertical pixels for each horizontal step, then an additional one where it draws 1 pixel.
 *
 * The end point is at `{ from.x + width + 1, from.y + 2 * width }`.
 */
void DrawMapLineSE2(const CelOutputBuffer &out, Point from, int width, std::uint8_t colorIndex);

/**
 * @brief Draw a line in the target buffer from the given point towards north west at an `atan(1/2)` angle.
 *
 * Draws 2 vertical pixels for each horizontal step, then an additional one where it draws 1 pixel.
 *
 * The end point is at `{ from.x - (width + 1), from.y - 2 * width }`.
 */
void DrawMapLineNW2(const CelOutputBuffer &out, Point from, int width, std::uint8_t colorIndex);

/**
 * @brief Draw a line in the target buffer from the given point towards south west at an `atan(1/2)` angle.
 *
 * Draws 2 vertical pixels for each horizontal step, then an additional one where it draws 1 pixel.
 *
 * The end point is at `{ from.x - (width + 1), from.y + 2 * width }`.
 */
void DrawMapLineSW2(const CelOutputBuffer &out, Point from, int width, std::uint8_t colorIndex);

} // namespace devilution
