/**
 * @file player.h
 *
 * Interface of player functionality, leveling, actions, creation, loading, etc.
 */
#ifndef __PLAYER_H__
#define __PLAYER_H__

DEVILUTION_BEGIN_NAMESPACE

typedef enum PLR_MODE {
	PM_STAND,
	PM_WALK,  //Movement towards N, NW, or NE
	PM_WALK2, //Movement towards S, SW, or SE
	PM_WALK3, //Movement towards W or E
	PM_ATTACK,
	PM_RATTACK,
	PM_BLOCK,
	PM_GOTHIT,
	PM_DEATH,
	PM_SPELL,
	PM_NEWLVL,
	PM_QUIT,
} PLR_MODE;

typedef enum action_id {
	// clang-format off
	ACTION_WALK        = -2, // Automatic walk when using gamepad
	ACTION_NONE        = -1,
	ACTION_ATTACK      = 9,
	ACTION_RATTACK     = 10,
	ACTION_SPELL       = 12,
	ACTION_OPERATE     = 13,
	ACTION_DISARM      = 14,
	ACTION_PICKUPITEM  = 15, // put item in hand (inventory screen open)
	ACTION_PICKUPAITEM = 16, // put item in inventory
	ACTION_TALK        = 17,
	ACTION_OPERATETK   = 18, // operate via telekinesis
	ACTION_ATTACKMON   = 20,
	ACTION_ATTACKPLR   = 21,
	ACTION_RATTACKMON  = 22,
	ACTION_RATTACKPLR  = 23,
	ACTION_SPELLMON    = 24,
	ACTION_SPELLPLR    = 25,
	ACTION_SPELLWALL   = 26,
	// clang-format on
} action_id;

typedef enum player_weapon_type {
	WT_MELEE,
	WT_RANGED,
} player_weapon_type;

typedef struct PlayerStruct {
	PLR_MODE _pmode;
	Sint8 walkpath[MAX_PATH_LENGTH];
	bool plractive;
	action_id destAction;
	Sint32 destParam1;
	Sint32 destParam2;
	direction destParam3;
	Sint32 destParam4;
	Sint32 plrlevel;
	Sint32 _px;      // Tile X-position of player
	Sint32 _py;      // Tile Y-position of player
	Sint32 _pfutx;   // Future tile X-position of player. Set at start of walking animation
	Sint32 _pfuty;   // Future tile Y-position of player. Set at start of walking animation
	Sint32 _ptargx;  // Target tile X-position for player movment. Set during pathfinding
	Sint32 _ptargy;  // Target tile Y-position for player movment. Set during pathfinding
	Sint32 _pownerx; // Tile X-position of player. Set via network on player input
	Sint32 _pownery; // Tile X-position of player. Set via network on player input
	Sint32 _poldx;   // Most recent X-position in dPlayer.
	Sint32 _poldy;   // Most recent Y-position in dPlayer.
	Sint32 _pxoff;   // Player sprite's pixel X-offset from tile.
	Sint32 _pyoff;   // Player sprite's pixel Y-offset from tile.
	Sint32 _pxvel;   // Pixel X-velocity while walking. Indirectly applied to _pxoff via _pvar6
	Sint32 _pyvel;   // Pixel Y-velocity while walking. Indirectly applied to _pyoff via _pvar7
	direction _pdir; // Direction faced by player (direction enum)
	Sint32 _pgfxnum; // Bitmask indicating what variant of the sprite the player is using. Lower byte define weapon (anim_weapon_id) and higher values define armour (starting with anim_armor_id)
	Uint8 *_pAnimData;
	Sint32 _pAnimDelay; // Tick length of each frame in the current animation
	Sint32 _pAnimCnt;   // Increases by one each game tick, counting how close we are to _pAnimDelay
	Sint32 _pAnimLen;   // Number of frames in current animation
	Sint32 _pAnimFrame; // Current frame of animation.
	Sint32 _pAnimWidth;
	Sint32 _pAnimWidth2;
	Sint32 _plid;
	Sint32 _pvid;
	spell_id _pSpell;
	spell_type _pSplType;
	Sint8 _pSplFrom; // TODO Create enum
	spell_id _pTSpell;
	spell_type _pTSplType;
	spell_id _pRSpell;
	// enum spell_type
	spell_type _pRSplType;
	spell_id _pSBkSpell;
	spell_type _pSBkSplType;
	Sint8 _pSplLvl[64];
	Uint64 _pMemSpells;  // Bitmask of learned spells
	Uint64 _pAblSpells;  // Bitmask of abilities
	Uint64 _pScrlSpells; // Bitmask of spells available via scrolls
	Uint8 _pSpellFlags;
	spell_id _pSplHotKey[4];
	spell_type _pSplTHotKey[4];
	player_weapon_type _pwtype;
	bool _pBlockFlag;
	bool _pInvincible;
	Sint8 _pLightRad;
	bool _pLvlChanging; // True when the player is transitioning between levels
	char _pName[PLR_NAME_LEN];
	plr_class _pClass;
	Sint32 _pStrength;
	Sint32 _pBaseStr;
	Sint32 _pMagic;
	Sint32 _pBaseMag;
	Sint32 _pDexterity;
	Sint32 _pBaseDex;
	Sint32 _pVitality;
	Sint32 _pBaseVit;
	Sint32 _pStatPts;
	Sint32 _pDamageMod;
	Sint32 _pBaseToBlk;
	Sint32 _pHPBase;
	Sint32 _pMaxHPBase;
	Sint32 _pHitPoints;
	Sint32 _pMaxHP;
	Sint32 _pHPPer;
	Sint32 _pManaBase;
	Sint32 _pMaxManaBase;
	Sint32 _pMana;
	Sint32 _pMaxMana;
	Sint32 _pManaPer;
	Sint8 _pLevel;
	Sint8 _pMaxLvl;
	Sint32 _pExperience;
	Sint32 _pMaxExp;
	Sint32 _pNextExper;
	Sint8 _pArmorClass;
	Sint8 _pMagResist;
	Sint8 _pFireResist;
	Sint8 _pLghtResist;
	Sint32 _pGold;
	bool _pInfraFlag;
	Sint32 _pVar1;    // Used for referring to X-position of player when finishing moving one tile (also used to define target coordinates for spells and ranged attacks)
	Sint32 _pVar2;    // Used for referring to Y-position of player when finishing moving one tile (also used to define target coordinates for spells and ranged attacks)
	direction _pVar3; // Player's direction when ending movement. Also used for casting direction of SPL_FIREWALL.
	Sint32 _pVar4;    // Used for storing X-position of a tile which should have its BFLAG_PLAYERLR flag removed after walking. When starting to walk the game places the player in the dPlayer array -1 in the Y coordinate, and uses BFLAG_PLAYERLR to check if it should be using -1 to the Y coordinate when rendering the player (also used for storing the level of a spell when the player casts it)
	Sint32 _pVar5;    // Used for storing Y-position of a tile which should have its BFLAG_PLAYERLR flag removed after walking. When starting to walk the game places the player in the dPlayer array -1 in the Y coordinate, and uses BFLAG_PLAYERLR to check if it should be using -1 to the Y coordinate when rendering the player (also used for storing the level of a spell when the player casts it)
	Sint32 _pVar6;    // Same as _pxoff but contains the value in a higher range
	Sint32 _pVar7;    // Same as _pyoff but contains the value in a higher range
	Sint32 _pVar8;    // Used for counting how close we are to reaching the next tile when walking (usually counts to 8, which is equal to the walk animation length). Also used for stalling the appearance of the options screen after dying in singleplayer
	bool _pLvlVisited[NUMLEVELS];
	bool _pSLvlVisited[NUMLEVELS]; // only 10 used
	                               /** Using player_graphic as bitflags */
	Sint32 _pGFXLoad;
	Uint8 *_pNAnim[8]; // Stand animations
	Sint32 _pNFrames;
	Sint32 _pNWidth;
	Uint8 *_pWAnim[8]; // Walk animations
	Sint32 _pWFrames;
	Sint32 _pWWidth;
	Uint8 *_pAAnim[8]; // Attack animations
	Sint32 _pAFrames;
	Sint32 _pAWidth;
	Sint32 _pAFNum;
	Uint8 *_pLAnim[8]; // Lightning spell cast animations
	Uint8 *_pFAnim[8]; // Fire spell cast animations
	Uint8 *_pTAnim[8]; // Generic spell cast animations
	Sint32 _pSFrames;
	Sint32 _pSWidth;
	Sint32 _pSFNum;
	Uint8 *_pHAnim[8]; // Getting hit animations
	Sint32 _pHFrames;
	Sint32 _pHWidth;
	Uint8 *_pDAnim[8]; // Death animations
	Sint32 _pDFrames;
	Sint32 _pDWidth;
	Uint8 *_pBAnim[8]; // Block animations
	Sint32 _pBFrames;
	Sint32 _pBWidth;
	ItemStruct InvBody[NUM_INVLOC];
	ItemStruct InvList[NUM_INV_GRID_ELEM];
	Sint32 _pNumInv;
	Sint8 InvGrid[NUM_INV_GRID_ELEM];
	ItemStruct SpdList[MAXBELTITEMS];
	ItemStruct HoldItem;
	Sint32 _pIMinDam;
	Sint32 _pIMaxDam;
	Sint32 _pIAC;
	Sint32 _pIBonusDam;
	Sint32 _pIBonusToHit;
	Sint32 _pIBonusAC;
	Sint32 _pIBonusDamMod;
	/** Bitmask of staff spell */
	Uint64 _pISpells;
	/** Bitmask using item_special_effect */
	Sint32 _pIFlags;
	Sint32 _pIGetHit;
	Sint8 _pISplLvlAdd;
	Sint32 _pISplDur;
	Sint32 _pIEnAc;
	Sint32 _pIFMinDam;
	Sint32 _pIFMaxDam;
	Sint32 _pILMinDam;
	Sint32 _pILMaxDam;
	item_misc_id _pOilType;
	Uint8 pTownWarps;
	Uint8 pDungMsgs;
	Uint8 pLvlLoad;
	bool pBattleNet;
	bool pManaShield;
	Uint8 pDungMsgs2;
	bool pOriginalCathedral;
	Uint16 wReflections;
	Uint32 pDiabloKillLevel;
	Uint32 pDifficulty;
	Uint32 pDamAcFlags;
	Uint8 *_pNData;
	Uint8 *_pWData;
	Uint8 *_pAData;
	Uint8 *_pLData;
	Uint8 *_pFData;
	Uint8 *_pTData;
	Uint8 *_pHData;
	Uint8 *_pDData;
	Uint8 *_pBData;

	/**
	 * @brief Gets the base value of the player's specified attribute.
	 * @param attribute The attribute to retrieve the base value for
	 * @return The base value for the requested attribute.
	*/
	Sint32 GetBaseAttributeValue(attribute_id attribute) const;

	/**
	 * @brief Gets the maximum value of the player's specified attribute.
	 * @param attribute The attribute to retrieve the maximum value for
	 * @return The maximum value for the requested attribute.
	*/
	Sint32 GetMaximumAttributeValue(attribute_id attribute) const;
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
void CreatePlayer(int pnum, plr_class c);
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
void FixPlayerLocation(int pnum, direction bDir);
void StartStand(int pnum, direction dir);
void StartAttack(int pnum, direction d);
void StartPlrBlock(int pnum, direction dir);
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

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __PLAYER_H__ */
