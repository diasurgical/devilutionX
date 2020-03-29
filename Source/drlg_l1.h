/**
 * @file drlg_l1.h
 *
 * Interface of the cathedral level generation algorithms.
 */
#ifndef __DRLG_L1_H__
#define __DRLG_L1_H__

extern BYTE L5dungeon[80][80];
extern BYTE L5dflags[DMAXX][DMAXY];
extern BOOL L5setloadflag;
extern int HR1;
extern int HR2;
extern int HR3;
extern int VR1;
extern int VR2;
extern int VR3;
extern BYTE *L5pSetPiece;

void DRLG_Init_Globals();
void LoadL1Dungeon(char *sFileName, int vx, int vy);
void LoadPreL1Dungeon(char *sFileName, int vx, int vy);
void CreateL5Dungeon(DWORD rseed, int entry);

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

#endif /* __DRLG_L1_H__ */
