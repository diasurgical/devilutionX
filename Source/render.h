/**
 * @file render.h
 *
 * Interface of functionality for rendering the level tiles.
 */
#ifndef __RENDER_H__
#define __RENDER_H__

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

}

#endif /* __RENDER_H__ */
