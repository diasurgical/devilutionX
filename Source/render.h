/**
 * @file render.h
 *
 * Interface of functionality for rendering the level tiles.
 */
#pragma once

namespace devilution {

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
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 */
void world_draw_black_tile(CelOutputBuffer out, int sx, int sy);

}
