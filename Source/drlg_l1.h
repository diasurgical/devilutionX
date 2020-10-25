/**
 * @file drlg_l1.h
 *
 * Interface of the cathedral level generation algorithms.
 */
#ifndef __DRLG_L1_H__
#define __DRLG_L1_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern BYTE L5dungeon[80][80];
extern BYTE L5dflags[DMAXX][DMAXY];
extern BOOL L5setloadflag;
extern int HR1;
extern int HR2;
extern int HR3;
extern int UberRow;
extern int UberCol;
extern int dword_577368;
extern int IsUberRoomOpened;
extern int UberLeverRow;
extern int UberLeverCol;
extern int IsUberLeverActivated;
extern int UberDiabloMonsterIndex;
extern int VR1;
extern int VR2;
extern int VR3;
extern BYTE *L5pSetPiece;

void DRLG_Init_Globals();
void LoadL1Dungeon(const char *sFileName, int vx, int vy);
void LoadPreL1Dungeon(const char *sFileName, int vx, int vy);
void CreateL5Dungeon(DWORD rseed, int entry);
#ifdef HELLFIRE
void DRLG_InitL5Vals();
void drlg_l1_crypt_lavafloor();
void drlg_l1_crypt_rndset(const BYTE *miniset, int rndper);
void drlg_l1_set_crypt_room(int rx1, int ry1);
void drlg_l1_set_corner_room(int rx1, int ry1);
void drlg_l1_crypt_pattern1(int rndper);
void drlg_l1_crypt_pattern2(int rndper);
void drlg_l1_crypt_pattern3(int rndper);
void drlg_l1_crypt_pattern4(int rndper);
void drlg_l1_crypt_pattern5(int rndper);
void drlg_l1_crypt_pattern6(int rndper);
void drlg_l1_crypt_pattern7(int rndper);
#endif

/* rdata */
extern const ShadowStruct SPATS[37];

// BUGFIX: These arrays should contain an additional 0 (207 elements).
extern const BYTE BSTYPES[207];
extern const BYTE L5BTYPES[207];

extern const BYTE STAIRSUP[];
extern const BYTE L5STAIRSUP[];
extern const BYTE STAIRSDOWN[];
extern const BYTE LAMPS[];
extern const BYTE PWATERIN[];

/* data */
extern BYTE L5ConvTbl[16];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __DRLG_L1_H__ */
