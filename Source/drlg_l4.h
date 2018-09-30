//HEADER_GOES_HERE
#ifndef __DRLG_L4_H__
#define __DRLG_L4_H__

extern int diabquad1x; // weak
extern int diabquad1y; // weak
extern int diabquad3x; // idb
extern int diabquad3y; // idb
extern int diabquad2x; // idb
extern int diabquad2y; // idb
extern int diabquad4x; // idb
extern int diabquad4y; // idb
extern int hallok[20];
extern int l4holdx; // weak
extern int l4holdy; // weak
extern int SP4x1; // idb
extern int SP4x2; // weak
extern int SP4y1; // idb
extern int SP4y2; // weak
extern char L4dungeon[80][80];
extern char dung[20][20];
//int dword_52A4DC; // weak

void __cdecl DRLG_LoadL4SP();
void __cdecl DRLG_FreeL4SP();
void __fastcall DRLG_L4SetSPRoom(int rx1, int ry1);
void __cdecl L4SaveQuads();
void __fastcall DRLG_L4SetRoom(unsigned char *pSetPiece, int rx1, int ry1);
void __fastcall DRLG_LoadDiabQuads(bool preflag);
bool __fastcall IsDURWall(char d);
bool __fastcall IsDLLWall(char dd);
void __cdecl L4FixRim();
void __cdecl DRLG_L4GeneralFix();
void __fastcall CreateL4Dungeon(int rseed, int entry);
void __fastcall DRLG_L4(int entry);
void __cdecl DRLG_L4Shadows();
void __cdecl InitL4Dungeon();
void __cdecl L4makeDmt();
void __cdecl L4AddWall();
int __fastcall L4HWallOk(int i, int j);
int __fastcall L4VWallOk(int i, int j);
void __fastcall L4HorizWall(int i, int j, int dx);
void __fastcall L4VertWall(int i, int j, int dy);
void __cdecl L4tileFix();
void __cdecl DRLG_L4Subs();
void __cdecl L4makeDungeon();
void __cdecl uShape();
int __cdecl GetArea();
void __cdecl L4firstRoom();
void __fastcall L4drawRoom(int x, int y, int width, int height);
void __fastcall L4roomGen(int x, int y, int w, int h, int dir);
bool __fastcall L4checkRoom(int x, int y, int width, int height);
bool __fastcall DRLG_L4PlaceMiniSet(const unsigned char *miniset, int tmin, int tmax, int cx, int cy, int setview, int ldir);
void __cdecl DRLG_L4FloodTVal();
void __fastcall DRLG_L4FTVR(int i, int j, int x, int y, int d);
void __cdecl DRLG_L4TransFix();
void __cdecl DRLG_L4Corners();
void __cdecl DRLG_L4Pass3();

/* rdata */
extern const unsigned char L4ConvTbl[16];
extern const unsigned char L4USTAIRS[42];
extern const unsigned char L4TWARP[42];
extern const unsigned char L4DSTAIRS[52];
extern const unsigned char L4PENTA[52];
extern const unsigned char L4PENTA2[52];
extern const unsigned char L4BTYPES[140];

#endif /* __DRLG_L4_H__ */
