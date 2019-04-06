//HEADER_GOES_HERE
#ifndef __RENDER_H__
#define __RENDER_H__

void __fastcall drawTopArchesUpperScreen(BYTE *pBuff);
void __fastcall drawBottomArchesUpperScreen(BYTE *pBuff, unsigned int *pMask);
void __fastcall drawUpperScreen(BYTE *pBuff);
void __fastcall drawTopArchesLowerScreen(BYTE *pBuff);
void __fastcall drawBottomArchesLowerScreen(BYTE *pBuff, unsigned int *pMask);
void __fastcall drawLowerScreen(BYTE *pBuff);
void __fastcall world_draw_black_tile(BYTE *pBuff);

/* rdata */

extern int WorldBoolFlag;
extern unsigned int gdwCurrentMask;
// extern char world_4B3264;
extern unsigned char *gpCelFrame;
extern unsigned int *gpDrawMask;
// extern char world_4B326D[16];
extern unsigned int RightMask[32];
extern unsigned int LeftMask[32];
extern unsigned int WallMask[32];
extern int WorldTbl3x16[48];
extern int WorldTbl17_1[17];
extern int WorldTbl17_2[17];

#endif /* __RENDER_H__ */
