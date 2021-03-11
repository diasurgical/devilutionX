/**
 * @file monster.h
 *
 * Interface of monster functionality, AI, actions, spawning, loading, etc.
 */
#ifndef __MONSTER_H__
#define __MONSTER_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

typedef enum MON_MODE {
	MM_STAND,
	/** Movement towards N, NW, or NE */
	MM_WALK,
	/** Movement towards S, SW, or SE */
	MM_WALK2,
	/** Movement towards W or E */
	MM_WALK3,
	MM_ATTACK,
	MM_GOTHIT,
	MM_DEATH,
	MM_SATTACK,
	MM_FADEIN,
	MM_FADEOUT,
	MM_RATTACK,
	MM_SPSTAND,
	MM_RSPATTACK,
	MM_DELAY,
	MM_CHARGE,
	MM_STONE,
	MM_HEAL,
	MM_TALK,
} MON_MODE;

typedef struct CMonster {
	_monster_id mtype;
	/** placeflag enum as a flags*/
	Uint8 mPlaceFlags;
	AnimStruct Anims[6];
	TSnd *Snds[4][2];
	Sint32 width;
	Sint32 width2;
	Uint8 mMinHP;
	Uint8 mMaxHP;
	bool has_special;
	Uint8 mAFNum;
	Sint8 mdeadval;
	const MonsterData *MData;
	/**
	 * A TRN file contains a sequence of color transitions, represented
	 * as indexes into a palette. (a 256 byte array of palette indices)
	 */
	Uint8 *trans_file;
} CMonster;

typedef struct MonsterStruct { // note: missing field _mAFNum
	Sint32 _mMTidx;
	MON_MODE _mmode;
	Uint8 _mgoal;
	Sint32 _mgoalvar1;
	Sint32 _mgoalvar2;
	Sint32 _mgoalvar3;
	Uint8 _pathcount;
	/** Tile X-position of monster */
	Sint32 _mx;
	/** Tile Y-position of monster */
	Sint32 _my;
	/** Future tile X-position of monster. Set at start of walking animation */
	Sint32 _mfutx;
	/** Future tile Y-position of monster. Set at start of walking animation */
	Sint32 _mfuty;
	/** Most recent X-position in dMonster. */
	Sint32 _moldx;
	/** Most recent Y-position in dMonster. */
	Sint32 _moldy;
	/** Monster sprite's pixel X-offset from tile. */
	Sint32 _mxoff;
	/** Monster sprite's pixel Y-offset from tile. */
	Sint32 _myoff;
	/** Pixel X-velocity while walking. Applied to _mxoff */
	Sint32 _mxvel;
	/** Pixel Y-velocity while walking. Applied to _myoff */
	Sint32 _myvel;
	/** Direction faced by monster (direction enum) */
	Sint32 _mdir;
	/** The current target of the mosnter. An index in to either the plr or monster array based on the _meflag value. */
	Sint32 _menemy;
	/** X-coordinate of enemy (usually correspond's to the enemy's futx value) */
	Uint8 _menemyx;
	/** Y-coordinate of enemy (usually correspond's to the enemy's futy value) */
	Uint8 _menemyy;
	Uint8 *_mAnimData;
	/** Tick length of each frame in the current animation */
	Sint32 _mAnimDelay;
	/** Increases by one each game tick, counting how close we are to _pAnimDelay */
	Sint32 _mAnimCnt;
	/** Number of frames in current animation */
	Sint32 _mAnimLen;
	/** Current frame of animation. */
	Sint32 _mAnimFrame;
	bool _mDelFlag;
	Sint32 _mVar1;
	Sint32 _mVar2;
	Sint32 _mVar3;
	Sint32 _mVar4;
	Sint32 _mVar5;
	/** Used as _mxoff but with a higher range so that we can correctly apply velocities of a smaller number */
	Sint32 _mVar6;
	/** Used as _myoff but with a higher range so that we can correctly apply velocities of a smaller number */
	Sint32 _mVar7;
	/** Value used to measure progress for moving from one tile to another */
	Sint32 _mVar8;
	Sint32 _mmaxhp;
	Sint32 _mhitpoints;
	_mai_id _mAi;
	Uint8 _mint;
	Uint32 _mFlags;
	Uint8 _msquelch;
	Sint32 _lastx;
	Sint32 _lasty;
	Sint32 _mRndSeed;
	Sint32 _mAISeed;
	Uint8 _uniqtype;
	Uint8 _uniqtrans;
	Sint8 _udeadval;
	Sint8 mWhoHit;
	Sint8 mLevel;
	Uint16 mExp;
	Uint16 mHit;
	Uint8 mMinDamage;
	Uint8 mMaxDamage;
	Uint16 mHit2;
	Uint8 mMinDamage2;
	Uint8 mMaxDamage2;
	Uint8 mArmorClass;
	Uint16 mMagicRes;
	Sint32 mtalkmsg;
	Uint8 leader;
	Uint8 leaderflag;
	Uint8 packsize;
	Sint8 mlid; // BUGFIX -1 is used when not emitting light this should be signed (fixed)
	const char *mName;
	CMonster *MType;
	const MonsterData *MData;
} MonsterStruct;

extern int monstkills[MAXMONSTERS];
extern int monstactive[MAXMONSTERS];
extern int nummonsters;
extern BOOLEAN sgbSaveSoundOn;
extern MonsterStruct monster[MAXMONSTERS];
extern CMonster Monsters[MAX_LVLMTYPES];
extern int nummtypes;

void InitLevelMonsters();
void GetLevelMTypes();
void InitMonsterGFX(int monst);
void InitMonster(int i, int rd, int mtype, int x, int y);
void ClrAllMonsters();
void monster_some_crypt();
void PlaceGroup(int mtype, int num, int leaderf, int leader);
void InitMonsters();
void SetMapMonsters(BYTE *pMap, int startx, int starty);
void DeleteMonster(int i);
int AddMonster(int x, int y, int dir, int mtype, BOOL InMap);
void monster_43C785(int i);
BOOL M_Talker(int i);
void M_StartStand(int i, int md);
void M_ClearSquares(int i);
void M_GetKnockback(int i);
void M_StartHit(int i, int pnum, int dam);
void M_StartKill(int i, int pnum);
void M_SyncStartKill(int i, int x, int y, int pnum);
void M_Teleport(int i);
void M_UpdateLeader(int i);
void DoEnding();
void PrepDoEnding();
void M_WalkDir(int i, int md);
void MAI_Zombie(int i);
void MAI_SkelSd(int i);
void MAI_Snake(int i);
void MAI_Bat(int i);
void MAI_SkelBow(int i);
void MAI_Fat(int i);
void MAI_Sneak(int i);
void MAI_Fireman(int i);
void MAI_Fallen(int i);
void MAI_Cleaver(int i);
void MAI_Round(int i, BOOL special);
void MAI_GoatMc(int i);
void MAI_Ranged(int i, int missile_type, BOOL special);
void MAI_GoatBow(int i);
void MAI_Succ(int i);
void MAI_Lich(int i);
void MAI_ArchLich(int i);
void MAI_Psychorb(int i);
void MAI_Necromorb(int i);
void MAI_AcidUniq(int i);
void MAI_Firebat(int i);
void MAI_Torchant(int i);
void MAI_Scav(int i);
void MAI_Garg(int i);
void MAI_RoundRanged(int i, int missile_type, BOOL checkdoors, int dam, int lessmissiles);
void MAI_Magma(int i);
void MAI_Storm(int i);
void MAI_BoneDemon(int i);
void MAI_Acid(int i);
void MAI_Diablo(int i);
void MAI_Mega(int i);
void MAI_Golum(int i);
void MAI_SkelKing(int i);
void MAI_Rhino(int i);
void MAI_HorkDemon(int i);
void MAI_Counselor(int i);
void MAI_Garbud(int i);
void MAI_Zhar(int i);
void MAI_SnotSpil(int i);
void MAI_Lazurus(int i);
void MAI_Lazhelp(int i);
void MAI_Lachdanan(int i);
void MAI_Warlord(int i);
void DeleteMonsterList();
void ProcessMonsters();
void FreeMonsters();
BOOL DirOK(int i, int mdir);
BOOL PosOkMissile(int x, int y);
BOOL CheckNoSolid(int x, int y);
BOOL LineClearF(BOOL (*Clear)(int, int), int x1, int y1, int x2, int y2);
BOOL LineClear(int x1, int y1, int x2, int y2);
BOOL LineClearF1(BOOL (*Clear)(int, int, int), int monst, int x1, int y1, int x2, int y2);
void SyncMonsterAnim(int i);
void M_FallenFear(int x, int y);
void PrintMonstHistory(int mt);
void PrintUniqueHistory();
void MissToMonst(int i, int x, int y);
BOOL PosOkMonst(int i, int x, int y);
BOOLEAN monster_posok(int i, int x, int y);
BOOL PosOkMonst2(int i, int x, int y);
BOOL PosOkMonst3(int i, int x, int y);
BOOL IsSkel(int mt);
BOOL IsGoat(int mt);
int M_SpawnSkel(int x, int y, int dir);
BOOL SpawnSkeleton(int ii, int x, int y);
int PreSpawnSkeleton();
void TalktoMonster(int i);
void SpawnGolum(int i, int x, int y, int mi);
BOOL CanTalkToMonst(int m);
BOOL CheckMonsterHit(int m, BOOL *ret);
int encode_enemy(int m);
void decode_enemy(int m, int enemy);

/* data */

extern int opposite[8];
extern int offset_x[8];
extern int offset_y[8];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __MONSTER_H__ */
