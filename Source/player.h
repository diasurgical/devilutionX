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

extern int myplr;
extern PlayerStruct plr[MAX_PLRS];
extern BOOL deathflag;
extern int ToBlkTbl[NUM_CLASSES];

void LoadPlrGFX(int pnum, player_graphic gfxflag);
void InitPlayerGFX(int pnum);
void InitPlrGFXMem(int pnum);
void FreePlayerGFX(int pnum);
void NewPlrAnim(int pnum, BYTE *Peq, int numFrames, int Delay, int width);
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
void PlrClrTrans(int x, int y);
void PlrDoTrans(int x, int y);
void SetPlayerOld(int pnum);
void FixPlayerLocation(int pnum, int bDir);
void StartStand(int pnum, int dir);
void StartAttack(int pnum, int d);
void StartPlrBlock(int pnum, int dir);
void FixPlrWalkTags(int pnum);
void RemovePlrFromMap(int pnum);
void StartPlrHit(int pnum, int dam, BOOL forcehit);
void StartPlayerKill(int pnum, int earflag);
void DropHalfPlayersGold(int pnum);
void StripTopGold(int pnum);
void SyncPlrKill(int pnum, int earflag);
void RemovePlrMissiles(int pnum);
void StartNewLvl(int pnum, int fom, int lvl);
void RestartTownLvl(int pnum);
void StartWarpLvl(int pnum, int pidx);
void ProcessPlayers();
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
int get_max_strength(int i);
int get_max_magic(int i);
int get_max_dexterity(int i);

/* data */

extern int plrxoff[9];
extern int plryoff[9];
extern int plrxoff2[9];
extern int plryoff2[9];
extern int StrengthTbl[NUM_CLASSES];
extern int MagicTbl[NUM_CLASSES];
extern int DexterityTbl[NUM_CLASSES];
extern int VitalityTbl[NUM_CLASSES];
extern int MaxStats[NUM_CLASSES][4];
extern const char *const ClassStrTblOld[];
extern const char *const ClassStrTbl[];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __PLAYER_H__ */
