//HEADER_GOES_HERE
#ifndef __DRLG_L1_H__
#define __DRLG_L1_H__

extern char L5dungeon[80][80];
extern char mydflags[40][40];
extern int setloadflag; // weak
extern int HR1;
extern int HR2;
extern int HR3;
extern int VR1;
extern int VR2;
extern int VR3;
extern void *pSetPiece; // idb

void __cdecl DRLG_Init_Globals();
void __fastcall LoadL1Dungeon(char *sFileName, int vx, int vy);
void __cdecl DRLG_L1Floor();
void __cdecl DRLG_L1Pass3();
void __cdecl DRLG_InitL1Vals();
void __fastcall LoadPreL1Dungeon(char *sFileName, int vx, int vy);
void __fastcall CreateL5Dungeon(int rseed, int entry);
void __cdecl DRLG_LoadL1SP();
void __cdecl DRLG_FreeL1SP();
void __fastcall DRLG_L5(int entry);
void __fastcall DRLG_PlaceDoor(int x, int y);
void __cdecl DRLG_L1Shadows();
int __fastcall DRLG_PlaceMiniSet(const unsigned char *miniset, int tmin, int tmax, int cx, int cy, bool setview, int noquad, int ldir);
void __cdecl InitL5Dungeon();
void __cdecl L5ClearFlags();
void __cdecl L5firstRoom();
void __fastcall L5drawRoom(int x, int y, int w, int h);
void __fastcall L5roomGen(int x, int y, int w, int h, BOOL dir);
bool __fastcall L5checkRoom(int x, int y, int width, int height);
int __cdecl L5GetArea();
void __cdecl L5makeDungeon();
void __cdecl L5makeDmt();
void __cdecl L5AddWall();
int __fastcall L5HWallOk(int i, int j);
int __fastcall L5VWallOk(int i, int j);
void __fastcall L5HorizWall(int i, int j, char p, int dx);
void __fastcall L5VertWall(int i, int j, char p, int dy);
void __cdecl L5tileFix();
void __cdecl DRLG_L5Subs();
void __cdecl L5FillChambers();
void __fastcall DRLG_L5GChamber(int sx, int sy, bool topflag, bool bottomflag, bool leftflag, bool rightflag);
void __fastcall DRLG_L5GHall(int x1, int y1, int x2, int y2);
void __fastcall DRLG_L5SetRoom(int rx1, int ry1);
void __cdecl DRLG_L5FloodTVal();
void __fastcall DRLG_L5FTVR(int i, int j, int x, int y, int d);
void __cdecl DRLG_L5TransFix();
void __cdecl DRLG_L5DirtFix();
void __cdecl DRLG_L5CornerFix();

/* rdata */
extern const ShadowStruct SPATS[37];
extern const unsigned char BSTYPES[206];
extern const unsigned char L5BTYPES[206];
extern const unsigned char STAIRSUP[];
extern const unsigned char L5STAIRSUP[];
extern const unsigned char STAIRSDOWN[];
extern const unsigned char LAMPS[];
extern const unsigned char PWATERIN[];

/* data */
extern unsigned char L5ConvTbl[16];

#endif /* __DRLG_L1_H__ */
