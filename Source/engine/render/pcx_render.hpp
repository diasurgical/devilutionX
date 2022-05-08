#pragma once

#include <array>
#include <cstdint>

#include "engine/pcx_sprite.hpp"
#include "engine/point.hpp"
#include "engine/surface.hpp"

namespace devilution {

/**
 * @brief Renders a PCX sprite to surface.
 *
 * @param out Output surface.
 * @param sprite Source sprite.
 * @param position Top-left position of the sprite on the surface.
 */
void RenderPcxSprite(const Surface &out, PcxSprite sprite, Point position);

/**
 * @brief Renders a PCX sprite to surface, translating the colors per the given map.
 *
 * @param out Output surface.
 * @param sprite Source sprite.
 * @param position Top-left position of the sprite on the surface.
 * @param colorMap Palette translation map.
 */
void RenderPcxSpriteWithColorMap(const Surface &out, PcxSprite sprite, Point position, const std::array<uint8_t, 256> &colorMap);

} // namespace devilution
