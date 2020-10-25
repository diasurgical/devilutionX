/**
 * @file player.h
 *
 * Interface of player functionality, leveling, actions, creation, loading, etc.
 */
#ifndef __PLAYER_H__
#define __PLAYER_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern int plr_lframe_size;
extern int plr_wframe_size;
extern BYTE plr_gfx_flag;
extern int plr_aframe_size;
extern int myplr;
extern PlayerStruct plr[MAX_PLRS];
extern int plr_fframe_size;
extern int plr_qframe_size;
extern BOOL deathflag;
extern int plr_hframe_size;
extern int plr_bframe_size;
extern BYTE plr_gfx_bflag;
extern int plr_sframe_size;
extern int deathdelay;
extern int plr_dframe_size;

void SetPlayerGPtrs(BYTE *pData, BYTE **pAnim);
void LoadPlrGFX(int pnum, player_graphic gfxflag);
void InitPlayerGFX(int pnum);
void InitPlrGFXMem(int pnum);
DWORD GetPlrGFXSize(const char *szCel);
void FreePlayerGFX(int pnum);
void NewPlrAnim(int pnum, BYTE *Peq, int numFrames, int Delay, int width);
void ClearPlrPVars(int pnum);
void SetPlrAnims(int pnum);
void ClearPlrRVars(PlayerStruct *p);
void CreatePlayer(int pnum, char c);
int CalcStatDiff(int pnum);
void NextPlrLevel(int pnum);
void AddPlrExperience(int pnum, int lvl, int exp);
void AddPlrMonstExper(int lvl, int exp, char pmask);
void InitPlayer(int pnum, BOOL FirstTime);
void InitMultiView();
void CheckEFlag(int pnum, BOOL flag);
BOOL SolidLoc(int x, int y);
BOOL PlrDirOK(int pnum, int dir);
void PlrClrTrans(int x, int y);
void PlrDoTrans(int x, int y);
void SetPlayerOld(int pnum);
void FixPlayerLocation(int pnum, int bDir);
void StartStand(int pnum, int dir);
void StartWalkStand(int pnum);
void PM_ChangeLightOff(int pnum);
void PM_ChangeOffset(int pnum);
void StartWalk(int pnum, int xvel, int yvel, int xadd, int yadd, int EndDir, int sdir);
void StartWalk2(int pnum, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int EndDir, int sdir);
void StartWalk3(int pnum, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int mapx, int mapy, int EndDir, int sdir);
void StartAttack(int pnum, int d);
void StartRangeAttack(int pnum, int d, int cx, int cy);
void StartPlrBlock(int pnum, int dir);
void StartSpell(int pnum, int d, int cx, int cy);
void FixPlrWalkTags(int pnum);
void RemovePlrFromMap(int pnum);
void StartPlrHit(int pnum, int dam, BOOL forcehit);
void RespawnDeadItem(ItemStruct *itm, int x, int y);
void StartPlayerKill(int pnum, int earflag);
void PlrDeadItem(int pnum, ItemStruct *itm, int xx, int yy);
void DropHalfPlayersGold(int pnum);
#ifdef HELLFIRE
void StripTopGold(int pnum);
#endif
void SyncPlrKill(int pnum, int earflag);
void j_StartPlayerKill(int pnum, int earflag);
void RemovePlrMissiles(int pnum);
void InitLevelChange(int pnum);
void StartNewLvl(int pnum, int fom, int lvl);
void RestartTownLvl(int pnum);
void StartWarpLvl(int pnum, int pidx);
BOOL PM_DoStand(int pnum);
BOOL PM_DoNewLvl(int pnum);
BOOL PM_DoWalk(int pnum);
BOOL PM_DoWalk2(int pnum);
BOOL PM_DoWalk3(int pnum);
BOOL WeaponDur(int pnum, int durrnd);
BOOL PlrHitMonst(int pnum, int m);
BOOL PlrHitPlr(int pnum, char p);
BOOL PlrHitObj(int pnum, int mx, int my);
BOOL PM_DoAttack(int pnum);
BOOL PM_DoRangeAttack(int pnum);
void ShieldDur(int pnum);
BOOL PM_DoBlock(int pnum);
BOOL PM_DoSpell(int pnum);
BOOL PM_DoGotHit(int pnum);
void ArmorDur(int pnum);
BOOL PM_DoDeath(int pnum);
void CheckNewPath(int pnum);
BOOL PlrDeathModeOK(int p);
void ValidatePlayer();
void ProcessPlayers();
void CheckCheatStats(int pnum);
void ClrPlrPath(int pnum);
BOOL PosOkPlayer(int pnum, int x, int y);
void MakePlrPath(int pnum, int xx, int yy, BOOL endspace);
void CheckPlrSpell();
void SyncPlrAnim(int pnum);
void SyncInitPlrPos(int pnum);
void SyncInitPlr(int pnum);
void CheckStats(int p);
void ModifyPlrStr(int p, int l);
void ModifyPlrMag(int p, int l);
void ModifyPlrDex(int p, int l);
void ModifyPlrVit(int p, int l);
void SetPlayerHitPoints(int pnum, int val);
void SetPlrStr(int p, int v);
void SetPlrMag(int p, int v);
void SetPlrDex(int p, int v);
void SetPlrVit(int p, int v);
void InitDungMsgs(int pnum);
void PlayDungMsgs();
#ifdef HELLFIRE
int player_45EFA1(int i);
int player_45EFAB(int i);
int player_45EFB5(int i);
#endif

/* rdata */

extern const char ArmourChar[4];
extern const char WepChar[10];
extern const char CharChar[];

/* data */

extern int plrxoff[9];
extern int plryoff[9];
extern int plrxoff2[9];
extern int plryoff2[9];
extern char PlrGFXAnimLens[NUM_CLASSES][11];
extern int PWVel[NUM_CLASSES][3];
extern int AnimLenFromClass[NUM_CLASSES];
extern int StrengthTbl[NUM_CLASSES];
extern int MagicTbl[NUM_CLASSES];
extern int DexterityTbl[NUM_CLASSES];
extern int VitalityTbl[NUM_CLASSES];
extern int ToBlkTbl[NUM_CLASSES];
extern const char *const ClassStrTblOld[];
extern int MaxStats[NUM_CLASSES][4];
extern int ExpLvlsTbl[MAXCHARLEVEL];
extern const char *const ClassStrTbl[NUM_CLASSES];
extern BYTE fix[9];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __PLAYER_H__ */
