//HEADER_GOES_HERE
#ifndef __RENDER_H__
#define __RENDER_H__

void drawTopArchesUpperScreen(BYTE *pBuff);
void drawBottomArchesUpperScreen(BYTE *pBuff, DWORD *pMask);
void drawUpperScreen(BYTE *pBuff);
void drawTopArchesLowerScreen(BYTE *pBuff);
void drawBottomArchesLowerScreen(BYTE *pBuff, DWORD *pMask);
void drawLowerScreen(BYTE *pBuff);
void world_draw_black_tile(BYTE *pBuff);

/* rdata */

extern int WorldBoolFlag;
extern DWORD gdwCurrentMask;
// extern char world_4B3264;
extern BYTE *gpCelFrame;
extern DWORD *gpDrawMask;
// extern char world_4B326D[16];
extern DWORD RightMask[32];
extern DWORD LeftMask[32];
extern DWORD WallMask[32];
extern int WorldTbl3x16[48];
extern int WorldTbl17_1[17];
extern int WorldTbl17_2[17];

#endif /* __RENDER_H__ */
