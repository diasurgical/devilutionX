/**
 * @file cel_render.hpp
 *
 * CEL rendering.
 */
#pragma once

#include <utility>

#include "engine.h"
#include "items.h"
#include "engine/cel_sprite.hpp"
#include "engine/point.hpp"

namespace devilution {

/**
 * Returns if cursor is within the CEL sprite (ignores shadow)
 */
bool IsCursorWithinCel(Point position, const CelSprite &cel, int frame, bool ignoreTransparent = true);

/**
 * Returns if cursor is within the CEL sprite's bounding box
 */
bool IsCursorWithinCelBoundingBox(Point position, const CelSprite &cel, int frame, int xError = 0, int yError = 0);

/**
 * Returns a pair of X coordinates containing the start (inclusive) and end (exclusive)
 * of fully transparent columns in the sprite.
 */
std::pair<int, int> MeasureSolidHorizontalBounds(const CelSprite &cel, int frame = 1, bool ignoreShadow = false);

/**
 * Returns a pair of Y coordinates containing the start (inclusive) and end (exclusive)
 * of fully transparent rows in the sprite.
 */
std::pair<int, int> MeasureSolidVerticalBounds(const CelSprite &cel, int frame = 1);

/**
 * @brief Blit CEL sprite to the back buffer at the given coordinates
 * @param out Target buffer
 * @param position Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelDrawTo(const Surface &out, Point position, const CelSprite &cel, int frame);

/**
 * @briefBlit CEL sprite to the given buffer, does not perform bounds-checking.
 * @param out Target buffer
 * @param position Coordinate in the target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelDrawUnsafeTo(const Surface &out, Point position, const CelSprite &cel, int frame);

/**
 * @brief Same as CelDrawTo but with the option to skip parts of the top and bottom of the sprite
 * @param out Target buffer
 * @param position Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelClippedDrawTo(const Surface &out, Point position, const CelSprite &cel, int frame);

/**
 * @brief Blit CEL sprite, and apply lighting, to the back buffer at the given coordinates
 * @param out Target buffer
 * @param position Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelDrawLightTo(const Surface &out, Point position, const CelSprite &cel, int frame, uint8_t *tbl);

/**
 * @brief Same as CelDrawLightTo but with the option to skip parts of the top and bottom of the sprite
 * @param out Target buffer
 * @param position Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelClippedDrawLightTo(const Surface &out, Point position, const CelSprite &cel, int frame);

/**
 * @brief Same as CelBlitLightTransSafeTo
 * @param out Target buffer
 * @param position Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelClippedBlitLightTransTo(const Surface &out, Point position, const CelSprite &cel, int frame);

/**
 * @brief Blit CEL sprite, and apply lighting, to the back buffer at the given coordinates, translated to a red hue
 * @param out Target buffer
 * @param position Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelDrawLightRedTo(const Surface &out, Point position, const CelSprite &cel, int frame);

/**
 * @brief Blit item's CEL sprite recolored red if not usable, normal if usable
 * @param item Item being drawn
 * @param out Target buffer
 * @param position Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelDrawItem(const Item &item, const Surface &out, Point position, const CelSprite &cel, int frame);

/**
 * @brief Blit a solid colder shape one pixel larger than the given sprite shape, to the target buffer at the given coordianates
 * @param out Target buffer
 * @param col Color index from current palette
 * @param position Target buffer coordinate
 * @param pCelBuff CEL buffer
 * @param frame CEL frame number
 * @param skipColorIndexZero If true, color in index 0 will be treated as transparent (these are typically used for shadows in sprites)
 */
void CelBlitOutlineTo(const Surface &out, uint8_t col, Point position, const CelSprite &cel, int frame, bool skipColorIndexZero = true);

} // namespace devilution
