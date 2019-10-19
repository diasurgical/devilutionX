#ifndef __RENDER_H__
#define __RENDER_H__

void RenderTile(BYTE *pBuff);
#define drawUpperScreen(p) RenderTile(p)
#define drawLowerScreen(p) RenderTile(p)
void world_draw_black_tile(int sx, int sy);
void trans_rect(int sx, int sy, int width, int height);

#endif /* __RENDER_H__ */
