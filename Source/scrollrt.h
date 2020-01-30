//HEADER_GOES_HERE
#ifndef __SCROLLRT_H__
#define __SCROLLRT_H__

extern bool sgbControllerActive;
extern int light_table_index;
#ifdef PIXEL_LIGHT
extern int testvar1;
extern int testvar2;
extern int testvar3;
extern int testvar4;
extern int testvar5;
extern LightListStruct staticLights[25][100];
extern int spellColors[100];
extern int redrawLights;
extern BOOLEAN sgbFadedIn;
extern SDL_Surface *pal_surface;
extern SDL_Surface *ui_surface;
extern SDL_Surface *tmp_surface;
#endif
extern BYTE *gpBufStart;
extern BYTE *gpBufEnd;
extern DWORD level_cel_block;
extern char arch_draw_type;
extern int cel_transparency_active;
extern int level_piece_id;
extern void (*DrawPlrProc)(int, int, int, int, int, BYTE *, int, int, int, int);

void ClearCursor();
void DrawMissile(int x, int y, int sx, int sy, BOOL pre);
void DrawDeadPlayer(int x, int y, int sx, int sy);
void DrawView(int StartX, int StartY);
void ClearScreenBuffer();
#ifdef _DEBUG
void ScrollView();
#endif
void EnableFrameCount();
void scrollrt_draw_game_screen(BOOL draw_cursor);
void DrawAndBlit();

/* rdata */

/* data */

/* used in 1.00 debug */
extern char *szMonModeAssert[18];
extern char *szPlrModeAssert[12];

#endif /* __SCROLLRT_H__ */
