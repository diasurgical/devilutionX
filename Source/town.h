//HEADER_GOES_HERE
#ifndef __TOWN_H__
#define __TOWN_H__

void town_clear_buf(BYTE *pBuff);
void town_draw_clipped_e_flag_2(BYTE *pBuff, int x, int y, int skipChunks, int sx, int sy);
void town_draw_e_flag(BYTE *pBuff, int x, int y, int capChunks, int sx, int sy);
void town_draw_town_all(BYTE *pBuff, int x, int y, int sx, int sy, int eflag);
void town_draw(int x, int y, int sx, int sy, int chunks, int dPieceRow);
void T_DrawGame(int x, int y);
void T_DrawZoom(int x, int y);
void T_DrawView(int StartX, int StartY);
void SetTownMicros();
void T_FillSector(BYTE *P3Tiles, BYTE *pSector, int xi, int yi, int w, int h);
void T_FillTile(BYTE *P3Tiles, int xx, int yy, int t);
void T_Pass3();
void CreateTown(int entry);

#endif /* __TOWN_H__ */
