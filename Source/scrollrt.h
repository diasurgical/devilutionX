//HEADER_GOES_HERE
#ifndef __SCROLLRT_H__
#define __SCROLLRT_H__

extern int light_table_index; // weak
extern int screen_y_times_768[1024];
extern unsigned char *gpBufEnd; // weak
extern DWORD level_cel_block;   // weak
extern char arch_draw_type;     // weak
extern DDSURFACEDESC DDS_desc;
extern int cel_transparency_active; // weak
extern int level_piece_id;          // weak
extern void (__fastcall *DrawPlrProc)(int, int, int, int, int, BYTE *, int, int, int, int);
extern int draw_monster_num; // weak

void __cdecl ClearCursor();
void __fastcall DrawMissile(int x, int y, int sx, int sy, int a5, int a6, BOOL pre);
void __fastcall DrawClippedMissile(int x, int y, int sx, int sy, int a5, int a6, BOOL pre);
void __fastcall DrawDeadPlayer(int x, int y, int sx, int sy, int a5, int a6, BOOL clipped);
void __fastcall DrawPlayer(int pnum, int x, int y, int px, int py, BYTE *pCelBuff, int nCel, int nWidth, int a9, int a10);
void __fastcall DrawClippedPlayer(int pnum, int x, int y, int px, int py, BYTE *pCelBuff, int nCel, int nWidth, int a9, int a10);
void __fastcall DrawView(int StartX, int StartY);
void __fastcall DrawGame(int x, int y);
void __fastcall scrollrt_draw_lower(int x, int y, int sx, int sy, int a5, int some_flag);
void __fastcall scrollrt_draw_clipped_dungeon(char *a1, int sx, int sy, int a4, int a5, int a6);
void __fastcall DrawClippedMonster(int x, int y, int mx, int my, int m, int a6, int a7);
void __fastcall DrawClippedObject(int x, int y, int ox, int oy, BOOL pre, int a6, int dir);
void __fastcall scrollrt_draw_clipped_e_flag(BYTE *pBuff, int x, int y, int a4, int a5);
void __fastcall scrollrt_draw_lower_2(int x, int y, int sx, int sy, int a5, int a6, int some_flag);
void __fastcall scrollrt_draw_clipped_dungeon_2(char *buffer, int x, int y, int a4, int a5, int sx, int sy, int me_flag);
void __fastcall scrollrt_draw_clipped_e_flag_2(BYTE *pBuff, int x, int y, int a4, signed int a5, int sx, int sy);
void __fastcall scrollrt_draw_upper(int x, int y, int sx, int sy, int a5, int a6, int some_flag);
void __fastcall scrollrt_draw_dungeon(BYTE *pBuff, int sx, int sy, int a4, int a5, int dx, int dy, int me_flag);
void __fastcall DrawMonster(int x, int y, int mx, int my, int m, int a6, int a7);
void __fastcall DrawObject(int x, int y, int ox, int oy, BOOL pre, int a6, int dir);
void __fastcall scrollrt_draw_e_flag(BYTE *pBuff, int x, int y, int a4, int a5, int sx, int sy);
void __fastcall DrawZoom(int x, int y);
void __cdecl ClearScreenBuffer();
#ifdef _DEBUG
void __cdecl ScrollView();
void __cdecl EnableFrameCount();
#endif
void __fastcall scrollrt_draw_game_screen(BOOL draw_cursor);
void __cdecl scrollrt_draw_cursor_back_buffer();
void __cdecl scrollrt_draw_cursor_item();
void __fastcall DrawMain(int dwHgt, int draw_desc, int draw_hp, int draw_mana, int draw_sbar, int draw_btn);
#ifdef _DEBUG
void __cdecl DrawFPS();
#endif
void __fastcall DoBlitScreen(DWORD dwX, DWORD dwY, DWORD dwWdt, DWORD dwHgt);
void __cdecl DrawAndBlit();

/* rdata */

/* data */

/* used in 1.00 debug */
extern char *szMonModeAssert[18];
extern char *szPlrModeAssert[12];

#endif /* __SCROLLRT_H__ */
