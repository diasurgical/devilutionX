/**
 * @file drlg_l4.h
 *
 * Interface of the hell level generation algorithms.
 */
#ifndef __DRLG_L4_H__
#define __DRLG_L4_H__

extern int diabquad1x;
extern int diabquad1y;
extern int diabquad2x;
extern int diabquad2y;
extern int diabquad3x;
extern int diabquad3y;
extern int diabquad4x;
extern int diabquad4y;
extern BOOL hallok[20];
extern int l4holdx;
extern int l4holdy;
extern int SP4x1;
extern int SP4y1;
extern int SP4x2;
extern int SP4y2;
extern BYTE L4dungeon[80][80];
extern BYTE dung[20][20];
//int dword_52A4DC;

void DRLG_LoadL4SP();
void DRLG_FreeL4SP();
void DRLG_L4SetSPRoom(int rx1, int ry1);
void L4SaveQuads();
void DRLG_L4SetRoom(BYTE *pSetPiece, int rx1, int ry1);
void DRLG_LoadDiabQuads(BOOL preflag);
BOOL IsDURWall(char d);
BOOL IsDLLWall(char dd);
void L4FixRim();
void DRLG_L4GeneralFix();
void CreateL4Dungeon(DWORD rseed, int entry);

/* rdata */
extern const BYTE L4ConvTbl[16];
extern const BYTE L4USTAIRS[42];
extern const BYTE L4TWARP[42];
extern const BYTE L4DSTAIRS[52];
extern const BYTE L4PENTA[52];
extern const BYTE L4PENTA2[52];
extern const BYTE L4BTYPES[140];

#endif /* __DRLG_L4_H__ */
