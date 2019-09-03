//HEADER_GOES_HERE
#ifndef __DRLG_L4_H__
#define __DRLG_L4_H__

extern int diabquad1x;
extern int diabquad1y;
extern int diabquad3x;
extern int diabquad3y;
extern int diabquad2x;
extern int diabquad2y;
extern int diabquad4x;
extern int diabquad4y;
extern BOOL hallok[20];
extern int l4holdx;
extern int l4holdy;
extern int SP4x1;
extern int SP4x2;
extern int SP4y1;
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
void DRLG_L4(int entry);
void DRLG_L4Shadows();
void InitL4Dungeon();
void L4makeDmt();
void L4AddWall();
int L4HWallOk(int i, int j);
int L4VWallOk(int i, int j);
void L4HorizWall(int i, int j, int dx);
void L4VertWall(int i, int j, int dy);
void L4tileFix();
void DRLG_L4Subs();
void L4makeDungeon();
void uShape();
long GetArea();
void L4firstRoom();
void L4drawRoom(int x, int y, int width, int height);
void L4roomGen(int x, int y, int w, int h, int dir);
BOOL L4checkRoom(int x, int y, int width, int height);
BOOL DRLG_L4PlaceMiniSet(const BYTE *miniset, int tmin, int tmax, int cx, int cy, BOOL setview, int ldir);
void DRLG_L4FloodTVal();
void DRLG_L4FTVR(int i, int j, int x, int y, int d);
void DRLG_L4TransFix();
void DRLG_L4Corners();
void DRLG_L4Pass3();

/* rdata */
extern const BYTE L4ConvTbl[16];
extern const BYTE L4USTAIRS[42];
extern const BYTE L4TWARP[42];
extern const BYTE L4DSTAIRS[52];
extern const BYTE L4PENTA[52];
extern const BYTE L4PENTA2[52];
extern const BYTE L4BTYPES[140];

#endif /* __DRLG_L4_H__ */
