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
 * @param x Target buffer coordinate
 * @param y Target buffer coordinate
 */
void RenderTile(const CelOutputBuffer &out, int x, int y);

/**
 * @brief Render a black 64x31 tile ◆
 * @param out Target buffer
 * @param sx Target buffer coordinate (left corner of the tile)
 * @param sy Target buffer coordinate (bottom corner of the tile)
 */
void world_draw_black_tile(const CelOutputBuffer &out, int sx, int sy);

} // namespace devilution
