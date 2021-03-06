/**
 * @file player.h
 *
 * Interface of player functionality, leveling, actions, creation, loading, etc.
 */
#ifndef __PLAYER_H__
#define __PLAYER_H__

DEVILUTION_BEGIN_NAMESPACE

typedef struct PlayerStruct {
	int _pmode;
	char walkpath[MAX_PATH_LENGTH];
	BOOLEAN plractive;
	int destAction;
	int destParam1;
	int destParam2;
	int destParam3;
	int destParam4;
	int plrlevel;
	int _px;      // Tile X-position of player
	int _py;      // Tile Y-position of player
	int _pfutx;   // Future tile X-position of player. Set at start of walking animation
	int _pfuty;   // Future tile Y-position of player. Set at start of walking animation
	int _ptargx;  // Target tile X-position for player movment. Set during pathfinding
	int _ptargy;  // Target tile Y-position for player movment. Set during pathfinding
	int _pownerx; // Tile X-position of player. Set via network on player input
	int _pownery; // Tile X-position of player. Set via network on player input
	int _poldx;   // Most recent X-position in dPlayer.
	int _poldy;   // Most recent Y-position in dPlayer.
	int _pxoff;   // Player sprite's pixel X-offset from tile.
	int _pyoff;   // Player sprite's pixel Y-offset from tile.
	int _pxvel;   // Pixel X-velocity while walking. Indirectly applied to _pxoff via _pvar6
	int _pyvel;   // Pixel Y-velocity while walking. Indirectly applied to _pyoff via _pvar7
	int _pdir;    // Direction faced by player (direction enum)
	int _nextdir; // Unused
	int _pgfxnum; // Bitmask indicating what variant of the sprite the player is using. Lower byte define weapon (anim_weapon_id) and higher values define armour (starting with anim_armor_id)
	unsigned char *_pAnimData;
	int _pAnimDelay; // Tick length of each frame in the current animation
	int _pAnimCnt;   // Increases by one each game tick, counting how close we are to _pAnimDelay
	int _pAnimLen;   // Number of frames in current animation
	int _pAnimFrame; // Current frame of animation.
	int _pAnimWidth;
	int _pAnimWidth2;
	int _peflag;
	int _plid;
	int _pvid;
	int _pSpell;
	char _pSplType;
	char _pSplFrom;
	int _pTSpell;
	char _pTSplType;
	int _pRSpell;
	// enum spell_type
	char _pRSplType;
	int _pSBkSpell;
	char _pSBkSplType;
	char _pSplLvl[64];
	uint64_t _pMemSpells;  // Bitmask of learned spells
	uint64_t _pAblSpells;  // Bitmask of abilities
	uint64_t _pScrlSpells; // Bitmask of spells available via scrolls
	UCHAR _pSpellFlags;
	int _pSplHotKey[4];
	char _pSplTHotKey[4];
	int _pwtype;
	BOOLEAN _pBlockFlag;
	BOOLEAN _pInvincible;
	char _pLightRad;
	BOOLEAN _pLvlChanging; // True when the player is transitioning between levels
	char _pName[PLR_NAME_LEN];
	// plr_class enum value.
	// TODO: this could very well be `enum plr_class _pClass`
	// since there are 3 bytes of alingment after this field.
	// it could just be that the compiler optimized away all accesses to
	// the higher bytes by using byte instructions, since all possible values
	// of plr_class fit into one byte.
	char _pClass;
	int _pStrength;
	int _pBaseStr;
	int _pMagic;
	int _pBaseMag;
	int _pDexterity;
	int _pBaseDex;
	int _pVitality;
	int _pBaseVit;
	int _pStatPts;
	int _pDamageMod;
	int _pBaseToBlk;
	int _pHPBase;
	int _pMaxHPBase;
	int _pHitPoints;
	int _pMaxHP;
	int _pHPPer;
	int _pManaBase;
	int _pMaxManaBase;
	int _pMana;
	int _pMaxMana;
	int _pManaPer;
	char _pLevel;
	char _pMaxLvl;
	int _pExperience;
	int _pMaxExp;
	int _pNextExper;
	char _pArmorClass;
	char _pMagResist;
	char _pFireResist;
	char _pLghtResist;
	int _pGold;
	BOOL _pInfraFlag;
	int _pVar1; // Used for referring to X-position of player when finishing moving one tile (also used to define target coordinates for spells and ranged attacks)
	int _pVar2; // Used for referring to Y-position of player when finishing moving one tile (also used to define target coordinates for spells and ranged attacks)
	int _pVar3; // Player's direction when ending movement. Also used for casting direction of SPL_FIREWALL.
	int _pVar4; // Used for storing X-position of a tile which should have its BFLAG_PLAYERLR flag removed after walking. When starting to walk the game places the player in the dPlayer array -1 in the Y coordinate, and uses BFLAG_PLAYERLR to check if it should be using -1 to the Y coordinate when rendering the player (also used for storing the level of a spell when the player casts it)
	int _pVar5; // Used for storing Y-position of a tile which should have its BFLAG_PLAYERLR flag removed after walking. When starting to walk the game places the player in the dPlayer array -1 in the Y coordinate, and uses BFLAG_PLAYERLR to check if it should be using -1 to the Y coordinate when rendering the player (also used for storing the level of a spell when the player casts it)
	int _pVar6; // Same as _pxoff but contains the value in a higher range
	int _pVar7; // Same as _pyoff but contains the value in a higher range
	int _pVar8; // Used for counting how close we are to reaching the next tile when walking (usually counts to 8, which is equal to the walk animation length). Also used for stalling the appearance of the options screen after dying in singleplayer
	BOOLEAN _pLvlVisited[NUMLEVELS];
	BOOLEAN _pSLvlVisited[NUMLEVELS]; // only 10 used
	int _pGFXLoad;
	unsigned char *_pNAnim[8]; // Stand animations
	int _pNFrames;
	int _pNWidth;
	unsigned char *_pWAnim[8]; // Walk animations
	int _pWFrames;
	int _pWWidth;
	unsigned char *_pAAnim[8]; // Attack animations
	int _pAFrames;
	int _pAWidth;
	int _pAFNum;
	unsigned char *_pLAnim[8]; // Lightning spell cast animations
	unsigned char *_pFAnim[8]; // Fire spell cast animations
	unsigned char *_pTAnim[8]; // Generic spell cast animations
	int _pSFrames;
	int _pSWidth;
	int _pSFNum;
	unsigned char *_pHAnim[8]; // Getting hit animations
	int _pHFrames;
	int _pHWidth;
	unsigned char *_pDAnim[8]; // Death animations
	int _pDFrames;
	int _pDWidth;
	unsigned char *_pBAnim[8]; // Block animations
	int _pBFrames;
	int _pBWidth;
	ItemStruct InvBody[NUM_INVLOC];
	ItemStruct InvList[NUM_INV_GRID_ELEM];
	int _pNumInv;
	char InvGrid[NUM_INV_GRID_ELEM];
	ItemStruct SpdList[MAXBELTITEMS];
	ItemStruct HoldItem;
	int _pIMinDam;
	int _pIMaxDam;
	int _pIAC;
	int _pIBonusDam;
	int _pIBonusToHit;
	int _pIBonusAC;
	int _pIBonusDamMod;
	uint64_t _pISpells; // Bitmask of staff spell
	int _pIFlags;
	int _pIGetHit;
	char _pISplLvlAdd;
	char _pISplCost;
	int _pISplDur;
	int _pIEnAc;
	int _pIFMinDam;
	int _pIFMaxDam;
	int _pILMinDam;
	int _pILMaxDam;
	int _pOilType;
	unsigned char pTownWarps;
	unsigned char pDungMsgs;
	unsigned char pLvlLoad;
	unsigned char pBattleNet;
	BOOLEAN pManaShield;
	unsigned char pDungMsgs2;
	BOOLEAN pOriginalCathedral;
	WORD wReflections;
	DWORD pDiabloKillLevel;
	int pDifficulty;
	int pDamAcFlags;
	unsigned char *_pNData;
	unsigned char *_pWData;
	unsigned char *_pAData;
	unsigned char *_pLData;
	unsigned char *_pFData;
	unsigned char *_pTData;
	unsigned char *_pHData;
	unsigned char *_pDData;
	unsigned char *_pBData;
} PlayerStruct;

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
void CreatePlayer(int pnum, char c);
int CalcStatDiff(int pnum);
#ifdef _DEBUG
void NextPlrLevel(int pnum);
#endif
void AddPlrExperience(int pnum, int lvl, int exp);
void AddPlrMonstExper(int lvl, int exp, char pmask);
void InitPlayer(int pnum, BOOL FirstTime);
void InitMultiView();
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
extern int ExpLvlsTbl[MAXCHARLEVEL];
extern const char *const ClassStrTblOld[];
extern const char *const ClassStrTbl[];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __PLAYER_H__ */
