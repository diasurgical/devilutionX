//HEADER_GOES_HERE
#ifndef __DRLG_L3_H__
#define __DRLG_L3_H__

extern char lavapool; // weak
extern int abyssx; // weak
extern int lockoutcnt; // weak
extern char lockout[40][40];

void __cdecl AddFenceDoors();
void __cdecl FenceDoorFix();
int __cdecl DRLG_L3Anvil();
void __cdecl FixL3Warp();
void __cdecl FixL3HallofHeroes();
void __fastcall DRLG_L3LockRec(int x, int y);
bool __cdecl DRLG_L3Lockout();
void __fastcall CreateL3Dungeon(int rseed, int entry);
void __fastcall DRLG_L3(int entry);
void __cdecl InitL3Dungeon();
int __fastcall DRLG_L3FillRoom(int x1, int y1, int x2, int y2);
void __fastcall DRLG_L3CreateBlock(int x, int y, int obs, int dir);
void __fastcall DRLG_L3FloorArea(int x1, int y1, int x2, int y2);
void __cdecl DRLG_L3FillDiags();
void __cdecl DRLG_L3FillSingles();
void __cdecl DRLG_L3FillStraights();
void __cdecl DRLG_L3Edges();
int __cdecl DRLG_L3GetFloorArea();
void __cdecl DRLG_L3MakeMegas();
void __cdecl DRLG_L3River();
void __cdecl DRLG_L3Pool();
int __fastcall DRLG_L3SpawnEdge(int x, int y, int *totarea);
int __fastcall DRLG_L3Spawn(int x, int y, int *totarea);
void __cdecl DRLG_L3PoolFix();
int __fastcall DRLG_L3PlaceMiniSet(const unsigned char *miniset, int tmin, int tmax, int cx, int cy, bool setview, int ldir);
void __fastcall DRLG_L3PlaceRndSet(const unsigned char *miniset, int rndper);
void __cdecl DRLG_L3Wood();
bool __fastcall WoodVertU(int i, int y);
bool __fastcall WoodVertD(int i, int y);
bool __fastcall WoodHorizL(int x, int j);
bool __fastcall WoodHorizR(int x, int j);
void __cdecl DRLG_L3Pass3();
void __fastcall LoadL3Dungeon(char *sFileName, int vx, int vy);
void __fastcall LoadPreL3Dungeon(char *sFileName, int vx, int vy);

/* rdata */
extern const unsigned char L3ConvTbl[16];
extern const unsigned char L3UP[20];
extern const unsigned char L3DOWN[20];
extern const unsigned char L3HOLDWARP[20];
extern const unsigned char L3TITE1[34];
extern const unsigned char L3TITE2[34];
extern const unsigned char L3TITE3[34];
extern const unsigned char L3TITE6[42];
extern const unsigned char L3TITE7[42];
extern const unsigned char L3TITE8[20];
extern const unsigned char L3TITE9[20];
extern const unsigned char L3TITE10[20];
extern const unsigned char L3TITE11[20];
extern const unsigned char L3TITE12[6];
extern const unsigned char L3TITE13[6];
extern const unsigned char L3CREV1[6];
extern const unsigned char L3CREV2[6];
extern const unsigned char L3CREV3[6];
extern const unsigned char L3CREV4[6];
extern const unsigned char L3CREV5[6];
extern const unsigned char L3CREV6[6];
extern const unsigned char L3CREV7[6];
extern const unsigned char L3CREV8[6];
extern const unsigned char L3CREV9[6];
extern const unsigned char L3CREV10[6];
extern const unsigned char L3CREV11[6];
extern const unsigned char L3ISLE1[14];
extern const unsigned char L3ISLE2[14];
extern const unsigned char L3ISLE3[14];
extern const unsigned char L3ISLE4[14];
extern const unsigned char L3ISLE5[10];
extern const unsigned char L3XTRA1[4];
extern const unsigned char L3XTRA2[4];
extern const unsigned char L3XTRA3[4];
extern const unsigned char L3XTRA4[4];
extern const unsigned char L3XTRA5[4];
extern const unsigned char L3ANVIL[244];
extern const unsigned char L3SpawnTbl1[15]; /* local spawntable? */
extern const unsigned char L3SpawnTbl2[15]; /* local spawntable? */
extern const unsigned char L3PoolSub[15]; /* local poolsub? */

#endif /* __DRLG_L3_H__ */
