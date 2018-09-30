//HEADER_GOES_HERE
#ifndef __THEMES_H__
#define __THEMES_H__

extern int numthemes; // idb
extern bool armorFlag; // weak
extern int ThemeGoodIn[4];
extern bool weaponFlag; // weak
extern bool treasureFlag; // weak
extern bool mFountainFlag; // weak
extern bool cauldronFlag; // weak
extern bool tFountainFlag; // weak
extern int zharlib; // weak
extern int themex; // idb
extern int themey; // idb
extern int themeVar1; // idb
extern ThemeStruct themes[MAXTHEMES];
extern bool pFountainFlag; // weak
extern bool bFountainFlag; // weak
extern bool bCrossFlag; // weak

bool __fastcall TFit_Shrine(int i);
bool __fastcall TFit_Obj5(int t);
bool __fastcall TFit_SkelRoom(int t);
bool __fastcall TFit_GoatShrine(int t);
bool __fastcall CheckThemeObj3(int xp, int yp, int t, int f);
bool __fastcall TFit_Obj3(int t);
bool __fastcall CheckThemeReqs(int t);
bool __fastcall SpecialThemeFit(int i, int t);
bool __fastcall CheckThemeRoom(int tv);
void __cdecl InitThemes();
void __cdecl HoldThemeRooms();
void __fastcall PlaceThemeMonsts(int t, int f);
void __fastcall Theme_Barrel(int t);
void __fastcall Theme_Shrine(int t);
void __fastcall Theme_MonstPit(int t);
void __fastcall Theme_SkelRoom(int t);
void __fastcall Theme_Treasure(int t);
void __fastcall Theme_Library(int t);
void __fastcall Theme_Torture(int t);
void __fastcall Theme_BloodFountain(int t);
void __fastcall Theme_Decap(int t);
void __fastcall Theme_PurifyingFountain(int t);
void __fastcall Theme_ArmorStand(int t);
void __fastcall Theme_GoatShrine(int t);
void __fastcall Theme_Cauldron(int t);
void __fastcall Theme_MurkyFountain(int t);
void __fastcall Theme_TearFountain(int t);
void __fastcall Theme_BrnCross(int t);
void __fastcall Theme_WeaponRack(int t);
void __cdecl UpdateL4Trans();
void __cdecl CreateThemeRooms();

/* rdata */

extern int ThemeGood[4];
extern int trm5x[25];
extern int trm5y[25];
extern int trm3x[9];
extern int trm3y[9];

#endif /* __THEMES_H__ */
