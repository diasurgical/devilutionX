/**
 * @file drlg_l3.h
 *
 * Interface of the caves level generation algorithms.
 */
#ifndef __DRLG_L3_H__
#define __DRLG_L3_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern BOOLEAN lavapool;
extern int abyssx;
extern int lockoutcnt;
extern BOOLEAN lockout[DMAXX][DMAXY];

void AddFenceDoors();
void FenceDoorFix();
BOOL DRLG_L3Anvil();
void FixL3Warp();
void FixL3HallofHeroes();
void DRLG_L3LockRec(int x, int y);
BOOL DRLG_L3Lockout();
void CreateL3Dungeon(DWORD rseed, int entry);
#ifdef HELLFIRE
BOOLEAN drlg_l3_hive_rnd_piece(const BYTE *miniset, int rndper);
#endif
void LoadL3Dungeon(const char *sFileName, int vx, int vy);
void LoadPreL3Dungeon(const char *sFileName, int vx, int vy);

/* rdata */
extern const BYTE L3ConvTbl[16];
extern const BYTE L3UP[20];
extern const BYTE L3DOWN[20];
extern const BYTE L3HOLDWARP[20];
extern const BYTE L3TITE1[34];
extern const BYTE L3TITE2[34];
extern const BYTE L3TITE3[34];
extern const BYTE L3TITE6[42];
extern const BYTE L3TITE7[42];
extern const BYTE L3TITE8[20];
extern const BYTE L3TITE9[20];
extern const BYTE L3TITE10[20];
extern const BYTE L3TITE11[20];
extern const BYTE L3TITE12[6];
extern const BYTE L3TITE13[6];
extern const BYTE L3CREV1[6];
extern const BYTE L3CREV2[6];
extern const BYTE L3CREV3[6];
extern const BYTE L3CREV4[6];
extern const BYTE L3CREV5[6];
extern const BYTE L3CREV6[6];
extern const BYTE L3CREV7[6];
extern const BYTE L3CREV8[6];
extern const BYTE L3CREV9[6];
extern const BYTE L3CREV10[6];
extern const BYTE L3CREV11[6];
extern const BYTE L3ISLE1[14];
extern const BYTE L3ISLE2[14];
extern const BYTE L3ISLE3[14];
extern const BYTE L3ISLE4[14];
extern const BYTE L3ISLE5[10];
extern const BYTE L3XTRA1[4];
extern const BYTE L3XTRA2[4];
extern const BYTE L3XTRA3[4];
extern const BYTE L3XTRA4[4];
extern const BYTE L3XTRA5[4];
extern const BYTE L3ANVIL[244];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __DRLG_L3_H__ */
