/**
 * @file render.h
 *
 * Interface of functionality for rendering the level tiles.
 */
#pragma once

#include "engine.h"

namespace devilution {

#define BUFFER_BORDER_LEFT 64
#define BUFFER_BORDER_TOP 160
#define BUFFER_BORDER_RIGHT devilution::borderRight
#define BUFFER_BORDER_BOTTOM 16

#define TILE_WIDTH 64
#define TILE_HEIGHT 32

/**
 * @brief Blit current world CEL to the given buffer
 * @param out Target buffer
 * @param x Target buffer coordinate
 * @param y Target buffer coordinate
 */
void RenderTile(CelOutputBuffer out, int x, int y);

/**
 * @brief Render a black tile
 * @param out Target buffer
 * @param sx Target buffer coordinate (left corner of the tile)
 * @param sy Target buffer coordinate (bottom corner of the tile)
 */
void world_draw_black_tile(const CelOutputBuffer &out, int sx, int sy);

} // namespace devilution
