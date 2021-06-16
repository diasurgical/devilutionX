/**
 * @file cel_render.hpp
 *
 * CEL rendering.
 */
#pragma once

#include <utility>

#include "engine.h"
#include "engine/point.hpp"

namespace devilution {

/**
 * Returns a pair of X coordinates containing the start (inclusive) and end (exclusive)
 * of fully transparent columns in the sprite.
 */
std::pair<int, int> MeasureSolidHorizontalBounds(const CelSprite &cel, int frame = 1);

/**
 * @brief Blit CEL sprite to the back buffer at the given coordinates
 * @param out Target buffer
 * @param position Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelDrawTo(const CelOutputBuffer &out, Point position, const CelSprite &cel, int frame);

/**
 * @briefBlit CEL sprite to the given buffer, does not perform bounds-checking.
 * @param out Target buffer
 * @param position Coordinate in the target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelDrawUnsafeTo(const CelOutputBuffer &out, Point position, const CelSprite &cel, int frame);

/**
 * @brief Same as CelDrawTo but with the option to skip parts of the top and bottom of the sprite
 * @param out Target buffer
 * @param position Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelClippedDrawTo(const CelOutputBuffer &out, Point position, const CelSprite &cel, int frame);

/**
 * @brief Blit CEL sprite, and apply lighting, to the back buffer at the given coordinates
 * @param out Target buffer
 * @param position Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelDrawLightTo(const CelOutputBuffer &out, Point position, const CelSprite &cel, int frame, uint8_t *tbl);

/**
 * @brief Same as CelDrawLightTo but with the option to skip parts of the top and bottom of the sprite
 * @param out Target buffer
 * @param position Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelClippedDrawLightTo(const CelOutputBuffer &out, Point position, const CelSprite &cel, int frame);

/**
 * @brief Same as CelBlitLightTransSafeTo
 * @param out Target buffer
 * @param position Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelClippedBlitLightTransTo(const CelOutputBuffer &out, Point position, const CelSprite &cel, int frame);

/**
 * @brief Blit CEL sprite, and apply lighting, to the back buffer at the given coordinates, translated to a red hue
 * @param out Target buffer
 * @param position Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelDrawLightRedTo(const CelOutputBuffer &out, Point position, const CelSprite &cel, int frame);

/**
 * @brief Blit item's CEL sprite recolored red if not usable, normal if usable
 * @param usable indicates if the item should be recolored red or not
 * @param out Target buffer
 * @param position Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelDrawItem(bool usable, const CelOutputBuffer &out, Point position, const CelSprite &cel, int frame);

/**
 * @brief Same as CelClippedDrawTo but checks for drawing outside the buffer
 * @param out Target buffer
 * @param position Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelClippedDrawSafeTo(const CelOutputBuffer &out, Point position, const CelSprite &cel, int frame);

/**
 * @brief Same as CelDrawLightRedTo but checks for drawing outside the buffer
 * @param out Target buffer
 * @param position Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelDrawLightRedSafeTo(const CelOutputBuffer &out, Point position, const CelSprite &cel, int frame);

/**
 * @brief Blit a solid colder shape one pixel larger than the given sprite shape, to the target buffer at the given coordianates
 * @param out Target buffer
 * @param col Color index from current palette
 * @param position Target buffer coordinate
 * @param pCelBuff CEL buffer
 * @param frame CEL frame number
 * @param skipColorIndexZero If true, color in index 0 will be treated as transparent (these are typically used for shadows in sprites)
 */
void CelBlitOutlineTo(const CelOutputBuffer &out, uint8_t col, Point position, const CelSprite &cel, int frame, bool skipColorIndexZero = true);

} // namespace devilution
