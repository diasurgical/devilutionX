/**
 * @file dun_render.hpp
 *
 * Interface of functionality for rendering the level tiles.
 */
#pragma once

#include "engine.h"

namespace devilution {

/**
 * @brief Blit current world CEL to the given buffer
 * @param out Target buffer
 * @param position Target buffer coordinates
 */
void RenderTile(const Surface &out, Point position);

/**
 * @brief Render a black 64x31 tile ◆
 * @param out Target buffer
 * @param sx Target buffer coordinate (left corner of the tile)
 * @param sy Target buffer coordinate (bottom corner of the tile)
 */
void world_draw_black_tile(const Surface &out, int sx, int sy);

} // namespace devilution
