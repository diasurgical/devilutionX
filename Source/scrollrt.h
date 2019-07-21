//HEADER_GOES_HERE
#ifndef __SCROLLRT_H__
#define __SCROLLRT_H__

extern int light_table_index;
extern int PitchTbl[1024];
extern BYTE *gpBufEnd;
extern DWORD level_cel_block;
extern char arch_draw_type;
extern DDSURFACEDESC DDS_desc;
extern int cel_transparency_active;
extern int level_piece_id;
extern void (*DrawPlrProc)(int, int, int, int, int, BYTE *, int, int, int, int);
extern int draw_monster_num;

void ClearCursor();
void DrawMissile(int x, int y, int sx, int sy, int CelSkip, int capChunks, BOOL pre);
void DrawClippedMissile(int x, int y, int sx, int sy, int CelSkip, int capChunks, BOOL pre);
void DrawDeadPlayer(int x, int y, int sx, int sy, int CelSkip, int capChunks, BOOL clipped);
void DrawPlayer(int pnum, int x, int y, int px, int py, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int capChunks);
void DrawClippedPlayer(int pnum, int x, int y, int px, int py, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int capChunks);
void DrawView(int StartX, int StartY);
void DrawGame(int x, int y);
void scrollrt_draw_lower(int x, int y, int sx, int sy, int chunks, int eflag);
void scrollrt_draw_clipped_dungeon(BYTE *pBuff, int sx, int sy, int dx, int dy, int eflag);
void DrawClippedMonster(int x, int y, int mx, int my, int m, int CelSkip, int capChunks);
void DrawClippedObject(int x, int y, int ox, int oy, BOOL pre, int CelSkip, int capChunks);
void scrollrt_draw_clipped_e_flag(BYTE *pBuff, int x, int y, int sx, int sy);
void scrollrt_draw_lower_2(int x, int y, int sx, int sy, int chunks, int skipChunks, int eflag);
void scrollrt_draw_clipped_dungeon_2(BYTE *pBuff, int sx, int sy, int skipChunks, int CelSkip, int dx, int dy, int eflag);
void scrollrt_draw_clipped_e_flag_2(BYTE *pBuff, int x, int y, int skipChunks, int CelSkip, int sx, int sy);
void scrollrt_draw_upper(int x, int y, int sx, int sy, int chunks, int capChunks, int eflag);
void scrollrt_draw_dungeon(BYTE *pBuff, int sx, int sy, int capChunks, int CelCap, int dx, int dy, int eflag);
void DrawMonster(int x, int y, int mx, int my, int m, int CelSkip, int CelCap);
void DrawObject(int x, int y, int ox, int oy, BOOL pre, int CelSkip, int capChunks);
void scrollrt_draw_e_flag(BYTE *pBuff, int x, int y, int capChunks, int CelCap, int sx, int sy);
void DrawZoom(int x, int y);
void ClearScreenBuffer();
#ifdef _DEBUG
void ScrollView();
void EnableFrameCount();
#endif
void scrollrt_draw_game_screen(BOOL draw_cursor);
void scrollrt_draw_cursor_back_buffer();
void scrollrt_draw_cursor_item();
void DrawMain(int dwHgt, BOOL draw_desc, BOOL draw_hp, BOOL draw_mana, BOOL draw_sbar, BOOL draw_btn);
#ifdef _DEBUG
void DrawFPS();
#endif
void DoBlitScreen(DWORD dwX, DWORD dwY, DWORD dwWdt, DWORD dwHgt);
void DrawAndBlit();

/* rdata */

/* data */

/* used in 1.00 debug */
extern char *szMonModeAssert[18];
extern char *szPlrModeAssert[12];

#endif /* __SCROLLRT_H__ */
