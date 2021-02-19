/**
 * @file structs.h
 *
 * Various global structures.
 */

DEVILUTION_BEGIN_NAMESPACE

//////////////////////////////////////////////////
// items
//////////////////////////////////////////////////

typedef struct PLStruct {
	const char *PLName;
	int PLPower;
	int PLParam1;
	int PLParam2;
	char PLMinLvl;
	int PLIType;
	BYTE PLGOE;
	BOOL PLDouble;
	BOOL PLOk;
	int PLMinVal;
	int PLMaxVal;
	int PLMultVal;
} PLStruct;

typedef struct UItemStruct {
	const char *UIName;
	char UIItemId;
	char UIMinLvl;
	char UINumPL;
	int UIValue;
	char UIPower1;
	int UIParam1;
	int UIParam2;
	char UIPower2;
	int UIParam3;
	int UIParam4;
	char UIPower3;
	int UIParam5;
	int UIParam6;
	char UIPower4;
	int UIParam7;
	int UIParam8;
	char UIPower5;
	int UIParam9;
	int UIParam10;
	char UIPower6;
	int UIParam11;
	int UIParam12;
} UItemStruct;

typedef struct ItemDataStruct {
	int iRnd;
	char iClass;
	char iLoc;
	int iCurs;
	char itype;
	char iItemId;
	const char *iName;
	const char *iSName;
	char iMinMLvl;
	int iDurability;
	int iMinDam;
	int iMaxDam;
	int iMinAC;
	int iMaxAC;
	char iMinStr;
	char iMinMag;
	char iMinDex;
	// item_special_effect
	int iFlags;
	// item_misc_id
	int iMiscId;
	// spell_id
	int iSpell;
	BOOL iUsable;
	int iValue;
	int iMaxValue;
} ItemDataStruct;

typedef struct ItemGetRecordStruct {
	int nSeed;
	unsigned short wCI;
	int nIndex;
	unsigned int dwTimestamp;
} ItemGetRecordStruct;

typedef struct ItemStruct {
	int _iSeed;
	WORD _iCreateInfo;
	int _itype;
	int _ix;
	int _iy;
	BOOL _iAnimFlag;
	unsigned char *_iAnimData; // PSX name -> ItemFrame
	int _iAnimLen;             // Number of frames in current animation
	int _iAnimFrame;           // Current frame of animation.
	int _iAnimWidth;
	int _iAnimWidth2; // width 2?
	BOOL _iDelFlag;   // set when item is flagged for deletion, deprecated in 1.02
	char _iSelFlag;
	BOOL _iPostDraw;
	BOOL _iIdentified;
	char _iMagical;
	char _iName[64];
	char _iIName[64];
	char _iLoc;
	// item_class enum
	char _iClass;
	int _iCurs;
	int _ivalue;
	int _iIvalue;
	int _iMinDam;
	int _iMaxDam;
	int _iAC;
	// item_special_effect
	int _iFlags;
	// item_misc_id
	int _iMiscId;
	// spell_id
	int _iSpell;
	int _iCharges;
	int _iMaxCharges;
	int _iDurability;
	int _iMaxDur;
	int _iPLDam;
	int _iPLToHit;
	int _iPLAC;
	int _iPLStr;
	int _iPLMag;
	int _iPLDex;
	int _iPLVit;
	int _iPLFR;
	int _iPLLR;
	int _iPLMR;
	int _iPLMana;
	int _iPLHP;
	int _iPLDamMod;
	int _iPLGetHit;
	int _iPLLight;
	char _iSplLvlAdd;
	char _iRequest;
	int _iUid;
	int _iFMinDam;
	int _iFMaxDam;
	int _iLMinDam;
	int _iLMaxDam;
	int _iPLEnAc;
	char _iPrePower;
	char _iSufPower;
	int _iVAdd1;
	int _iVMult1;
	int _iVAdd2;
	int _iVMult2;
	char _iMinStr;
	unsigned char _iMinMag;
	char _iMinDex;
	BOOL _iStatFlag;
	int IDidx;
	int offs016C; // _oldlight or _iInvalid
	int _iDamAcFlags;
} ItemStruct;

//////////////////////////////////////////////////
// player
//////////////////////////////////////////////////

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
	uint64_t _pScrlSpells; // Bitmask of spells avalible via scrolls
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
	char bReserved[2];
	WORD wReflections;
	short wReserved[7];
	DWORD pDiabloKillLevel;
	int pDifficulty;
	int pDamAcFlags;
	int dwReserved[5];
	unsigned char *_pNData;
	unsigned char *_pWData;
	unsigned char *_pAData;
	unsigned char *_pLData;
	unsigned char *_pFData;
	unsigned char *_pTData;
	unsigned char *_pHData;
	unsigned char *_pDData;
	unsigned char *_pBData;
	void *pReserved;
} PlayerStruct;

//////////////////////////////////////////////////
// textdat
//////////////////////////////////////////////////

typedef struct TextDataStruct {
	const char *txtstr;
	int scrlltxt;
	int txtspd; /* calculated dynamically, 01/23/21 */
	int sfxnr;
} TextDataStruct;

//////////////////////////////////////////////////
// missiles
//////////////////////////////////////////////////

// TPDEF PTR FCN VOID MIADDPRC
// TPDEF PTR FCN VOID MIPROC

typedef struct MissileData {
	unsigned char mName;
	void (*mAddProc)(int, int, int, int, int, int, char, int, int);
	void (*mProc)(int);
	BOOL mDraw;
	unsigned char mType;
	unsigned char mResist;
	unsigned char mFileNum;
	int mlSFX;
	int miSFX;
} MissileData;

typedef struct MisFileData {
	unsigned char mAnimName;
	unsigned char mAnimFAmt;
	const char *mName;
	int mFlags;
	unsigned char *mAnimData[16];
	unsigned char mAnimDelay[16];
	unsigned char mAnimLen[16];
	int mAnimWidth[16];
	int mAnimWidth2[16];
} MisFileData;

typedef struct ChainStruct {
	int idx;
	int _mitype;
	int _mirange;
} ChainStruct;

typedef struct MissileStruct {
	int _mitype;  // Type of projectile (missile_id)
	int _mix;     // Tile X-position of the missile
	int _miy;     // Tile Y-position of the missile
	int _mixoff;  // Sprite pixel X-offset for the missile
	int _miyoff;  // Sprite pixel Y-offset for the missile
	int _mixvel;  // Missile tile X-velocity while walking. This gets added onto _mitxoff each game tick
	int _miyvel;  // Missile tile Y-velocity while walking. This gets added onto _mitxoff each game tick
	int _misx;    // Initial tile X-position for missile
	int _misy;    // Initial tile Y-position for missile
	int _mitxoff; // How far the missile has travelled in its lifespan along the X-axis. mix/miy/mxoff/myoff get updated every game tick based on this
	int _mityoff; // How far the missile has travelled in its lifespan along the Y-axis. mix/miy/mxoff/myoff get updated every game tick based on this
	int _mimfnum; // The direction of the missile (direction enum)
	int _mispllvl;
	BOOL _miDelFlag; // Indicate weather the missile should be deleted
	BYTE _miAnimType;
	int _miAnimFlags;
	unsigned char *_miAnimData;
	int _miAnimDelay; // Tick length of each frame in the current animation
	int _miAnimLen;   // Number of frames in current animation
	int _miAnimWidth;
	int _miAnimWidth2;
	int _miAnimCnt; // Increases by one each game tick, counting how close we are to _pAnimDelay
	int _miAnimAdd;
	int _miAnimFrame; // Current frame of animation.
	BOOL _miDrawFlag;
	BOOL _miLightFlag;
	BOOL _miPreFlag;
	int _miUniqTrans;
	int _mirange; // Time to live for the missile in game ticks, oncs 0 the missile will be marked for deletion via _miDelFlag
	int _misource;
	int _micaster;
	int _midam;
	BOOL _miHitFlag;
	int _midist; // Used for arrows to measure distance travelled (increases by 1 each game tick). Higher value is a penalty for accuracy calculation when hitting enemy
	int _mlid;
	int _mirnd;
	int _miVar1;
	int _miVar2;
	int _miVar3;
	int _miVar4;
	int _miVar5;
	int _miVar6;
	int _miVar7;
	int _miVar8;
} MissileStruct;

//////////////////////////////////////////////////
// effects/sound
//////////////////////////////////////////////////

typedef struct TSnd {
	const char *sound_path;
	SoundSample *DSB;
	int start_tc;
} TSnd;

typedef struct TSFX {
	unsigned char bFlags;
	const char *pszName;
	TSnd *pSnd;
} TSFX;

//////////////////////////////////////////////////
// monster
//////////////////////////////////////////////////

typedef struct AnimStruct {
	BYTE *CMem;
	BYTE *Data[8];
	int Frames;
	int Rate;
} AnimStruct;

typedef struct MonsterData {
	int width;
	int mImage;
	const char *GraphicType;
	BOOL has_special;
	const char *sndfile;
	BOOL snd_special;
	BOOL has_trans;
	const char *TransFile;
	int Frames[6];
	int Rate[6];
	const char *mName;
	char mMinDLvl;
	char mMaxDLvl;
	char mLevel;
	int mMinHP;
	int mMaxHP;
	char mAi;
	int mFlags;
	unsigned char mInt;
	unsigned short mHit; // BUGFIX: Some monsters overflow this value on high difficultys (fixed)
	unsigned char mAFNum;
	unsigned char mMinDamage;
	unsigned char mMaxDamage;
	unsigned short mHit2; // BUGFIX: Some monsters overflow this value on high difficulty (fixed)
	unsigned char mAFNum2;
	unsigned char mMinDamage2;
	unsigned char mMaxDamage2;
	unsigned char mArmorClass;
	char mMonstClass;
	unsigned short mMagicRes;
	unsigned short mMagicRes2;
	unsigned short mTreasure;
	char mSelFlag;
	unsigned short mExp;
} MonsterData;

typedef struct CMonster {
	int mtype;
	// TODO: Add enum for place flags
	unsigned char mPlaceFlags;
	AnimStruct Anims[6];
	TSnd *Snds[4][2];
	int width;
	int width2;
	unsigned char mMinHP;
	unsigned char mMaxHP;
	BOOL has_special;
	unsigned char mAFNum;
	char mdeadval;
	MonsterData *MData;
	// A TRN file contains a sequence of colour transitions, represented
	// as indexes into a palette. (a 256 byte array of palette indices)
	BYTE *trans_file;
} CMonster;

typedef struct MonsterStruct { // note: missing field _mAFNum
	int _mMTidx;
	int _mmode; /* MON_MODE */
	unsigned char _mgoal;
	int _mgoalvar1;
	int _mgoalvar2;
	int _mgoalvar3;
	int field_18;
	unsigned char _pathcount;
	int _mx;                // Tile X-position of monster
	int _my;                // Tile Y-position of monster
	int _mfutx;             // Future tile X-position of monster. Set at start of walking animation
	int _mfuty;             // Future tile Y-position of monster. Set at start of walking animation
	int _moldx;             // Most recent X-position in dMonster.
	int _moldy;             // Most recent Y-position in dMonster.
	int _mxoff;             // Monster sprite's pixel X-offset from tile.
	int _myoff;             // Monster sprite's pixel Y-offset from tile.
	int _mxvel;             // Pixel X-velocity while walking. Applied to _mxoff
	int _myvel;             // Pixel Y-velocity while walking. Applied to _myoff
	int _mdir;              // Direction faced by monster (direction enum)
	int _menemy;            // The current target of the mosnter. An index in to either the plr or monster array based on the _meflag value.
	unsigned char _menemyx; // X-coordinate of enemy (usually correspond's to the enemy's futx value)
	unsigned char _menemyy; // Y-coordinate of enemy (usually correspond's to the enemy's futy value)
	short falign_52;        // probably _mAFNum (unused)
	unsigned char *_mAnimData;
	int _mAnimDelay; // Tick length of each frame in the current animation
	int _mAnimCnt;   // Increases by one each game tick, counting how close we are to _pAnimDelay
	int _mAnimLen;   // Number of frames in current animation
	int _mAnimFrame; // Current frame of animation.
	BOOL _meflag;
	BOOL _mDelFlag;
	int _mVar1;
	int _mVar2;
	int _mVar3;
	int _mVar4;
	int _mVar5;
	int _mVar6; // Used as _mxoff but with a higher range so that we can correctly apply velocities of a smaller number
	int _mVar7; // Used as _myoff but with a higher range so that we can correctly apply velocities of a smaller number
	int _mVar8; // Value used to measure progress for moving from one tile to another
	int _mmaxhp;
	int _mhitpoints;
	unsigned char _mAi;
	unsigned char _mint;
	short falign_9A;
	int _mFlags;
	BYTE _msquelch;
	int falign_A4;
	int _lastx;
	int _lasty;
	int _mRndSeed;
	int _mAISeed;
	int falign_B8;
	unsigned char _uniqtype;
	unsigned char _uniqtrans;
	char _udeadval;
	char mWhoHit;
	char mLevel;
	unsigned short mExp;
	unsigned short mHit;
	unsigned char mMinDamage;
	unsigned char mMaxDamage;
	unsigned short mHit2;
	unsigned char mMinDamage2;
	unsigned char mMaxDamage2;
	unsigned char mArmorClass;
	char falign_CB;
	unsigned short mMagicRes;
	int mtalkmsg;
	unsigned char leader;
	unsigned char leaderflag;
	unsigned char packsize;
	Sint8 mlid; // BUGFIX -1 is used when not emitting light this should be signed (fixed)
	const char *mName;
	CMonster *MType;
	MonsterData *MData;
} MonsterStruct;

typedef struct UniqMonstStruct {
	int mtype;
	const char *mName;
	const char *mTrnName;
	unsigned char mlevel;
	unsigned short mmaxhp;
	unsigned char mAi;
	unsigned char mint;
	unsigned char mMinDamage;
	unsigned char mMaxDamage;
	unsigned short mMagicRes;
	unsigned short mUnqAttr;
	unsigned char mUnqVar1;
	unsigned char mUnqVar2;
	int mtalkmsg;
} UniqMonstStruct;

//////////////////////////////////////////////////
// objects
//////////////////////////////////////////////////

typedef struct ObjDataStruct {
	char oload;
	char ofindex;
	char ominlvl;
	char omaxlvl;
	char olvltype;
	char otheme;
	char oquest;
	int oAnimFlag;
	int oAnimDelay; // Tick length of each frame in the current animation
	int oAnimLen;   // Number of frames in current animation
	int oAnimWidth;
	BOOL oSolidFlag;
	BOOL oMissFlag;
	BOOL oLightFlag;
	char oBreak;
	char oSelFlag;
	BOOL oTrapFlag;
} ObjDataStruct;

typedef struct ObjectStruct {
	int _otype;
	int _ox;
	int _oy;
	int _oLight;
	int _oAnimFlag;
	unsigned char *_oAnimData;
	int _oAnimDelay; // Tick length of each frame in the current animation
	int _oAnimCnt;   // Increases by one each game tick, counting how close we are to _pAnimDelay
	int _oAnimLen;   // Number of frames in current animation
	int _oAnimFrame; // Current frame of animation.
	int _oAnimWidth;
	int _oAnimWidth2;
	BOOL _oDelFlag;
	char _oBreak; // check
	BOOL _oSolidFlag;
	BOOL _oMissFlag;
	char _oSelFlag; // check
	BOOL _oPreFlag;
	BOOL _oTrapFlag;
	BOOL _oDoorFlag;
	int _olid;
	int _oRndSeed;
	int _oVar1;
	int _oVar2;
	int _oVar3;
	int _oVar4;
	int _oVar5;
	int _oVar6;
	int _oVar7;
	int _oVar8;
} ObjectStruct;

//////////////////////////////////////////////////
// portal
//////////////////////////////////////////////////

typedef struct PortalStruct {
	BOOL open;
	int x;
	int y;
	int level;
	int ltype;
	BOOL setlvl;
} PortalStruct;

//////////////////////////////////////////////////
// msg
//////////////////////////////////////////////////

#pragma pack(push, 1)
typedef struct TCmd {
	BYTE bCmd;
} TCmd;

typedef struct TCmdLoc {
	BYTE bCmd;
	BYTE x;
	BYTE y;
} TCmdLoc;

typedef struct TCmdLocParam1 {
	BYTE bCmd;
	BYTE x;
	BYTE y;
	WORD wParam1;
} TCmdLocParam1;

typedef struct TCmdLocParam2 {
	BYTE bCmd;
	BYTE x;
	BYTE y;
	WORD wParam1;
	WORD wParam2;
} TCmdLocParam2;

typedef struct TCmdLocParam3 {
	BYTE bCmd;
	BYTE x;
	BYTE y;
	WORD wParam1;
	WORD wParam2;
	WORD wParam3;
} TCmdLocParam3;

typedef struct TCmdParam1 {
	BYTE bCmd;
	WORD wParam1;
} TCmdParam1;

typedef struct TCmdParam2 {
	BYTE bCmd;
	WORD wParam1;
	WORD wParam2;
} TCmdParam2;

typedef struct TCmdParam3 {
	BYTE bCmd;
	WORD wParam1;
	WORD wParam2;
	WORD wParam3;
} TCmdParam3;

typedef struct TCmdGolem {
	BYTE bCmd;
	BYTE _mx;
	BYTE _my;
	BYTE _mdir;
	char _menemy;
	int _mhitpoints;
	BYTE _currlevel;
} TCmdGolem;

typedef struct TCmdQuest {
	BYTE bCmd;
	BYTE q;
	BYTE qstate;
	BYTE qlog;
	BYTE qvar1;
} TCmdQuest;

typedef struct TCmdGItem {
	BYTE bCmd;
	BYTE bMaster;
	BYTE bPnum;
	BYTE bCursitem;
	BYTE bLevel;
	BYTE x;
	BYTE y;
	WORD wIndx;
	WORD wCI;
	int dwSeed;
	BYTE bId;
	BYTE bDur;
	BYTE bMDur;
	BYTE bCh;
	BYTE bMCh;
	WORD wValue;
	DWORD dwBuff;
	int dwTime;
	WORD wToHit;
	WORD wMaxDam;
	BYTE bMinStr;
	BYTE bMinMag;
	BYTE bMinDex;
	BYTE bAC;
} TCmdGItem;

typedef struct TCmdPItem {
	BYTE bCmd;
	BYTE x;
	BYTE y;
	WORD wIndx;
	WORD wCI;
	int dwSeed;
	BYTE bId;
	BYTE bDur;
	BYTE bMDur;
	BYTE bCh;
	BYTE bMCh;
	WORD wValue;
	DWORD dwBuff;
	WORD wToHit;
	WORD wMaxDam;
	BYTE bMinStr;
	BYTE bMinMag;
	BYTE bMinDex;
	BYTE bAC;
} TCmdPItem;

typedef struct TCmdChItem {
	BYTE bCmd;
	BYTE bLoc;
	WORD wIndx;
	WORD wCI;
	int dwSeed;
	Uint8 bId;
} TCmdChItem;

typedef struct TCmdDelItem {
	BYTE bCmd;
	BYTE bLoc;
} TCmdDelItem;

typedef struct TCmdDamage {
	BYTE bCmd;
	BYTE bPlr;
	DWORD dwDam;
} TCmdDamage;

typedef struct TCmdMonDamage {
	BYTE bCmd;
	WORD wMon;
	DWORD dwDam;
} TCmdMonDamage;

typedef struct TCmdPlrInfoHdr {
	BYTE bCmd;
	WORD wOffset;
	WORD wBytes;
} TCmdPlrInfoHdr;

typedef struct TCmdString {
	BYTE bCmd;
	char str[MAX_SEND_STR_LEN];
} TCmdString;

typedef struct TFakeCmdPlr {
	BYTE bCmd;
	BYTE bPlr;
} TFakeCmdPlr;

typedef struct TFakeDropPlr {
	BYTE bCmd;
	BYTE bPlr;
	DWORD dwReason;
} TFakeDropPlr;

typedef struct TSyncHeader {
	BYTE bCmd;
	BYTE bLevel;
	WORD wLen;
	BYTE bObjId;
	BYTE bObjCmd;
	BYTE bItemI;
	BYTE bItemX;
	BYTE bItemY;
	WORD wItemIndx;
	WORD wItemCI;
	DWORD dwItemSeed;
	BYTE bItemId;
	BYTE bItemDur;
	BYTE bItemMDur;
	BYTE bItemCh;
	BYTE bItemMCh;
	WORD wItemVal;
	DWORD dwItemBuff;
	BYTE bPInvLoc;
	WORD wPInvIndx;
	WORD wPInvCI;
	DWORD dwPInvSeed;
	BYTE bPInvId;
	WORD wToHit;
	WORD wMaxDam;
	BYTE bMinStr;
	BYTE bMinMag;
	BYTE bMinDex;
	BYTE bAC;
} TSyncHeader;

typedef struct TSyncMonster {
	BYTE _mndx;
	BYTE _mx;
	BYTE _my;
	BYTE _menemy;
	BYTE _mdelta;
} TSyncMonster;

typedef struct TPktHdr {
	BYTE px;
	BYTE py;
	BYTE targx;
	BYTE targy;
	int php;
	int pmhp;
	BYTE bstr;
	BYTE bmag;
	BYTE bdex;
	WORD wCheck;
	WORD wLen;
} TPktHdr;

typedef struct TPkt {
	TPktHdr hdr;
	BYTE body[493];
} TPkt;

typedef struct DMonsterStr {
	BYTE _mx;
	BYTE _my;
	BYTE _mdir;
	BYTE _menemy;
	BYTE _mactive;
	int _mhitpoints;
} DMonsterStr;

typedef struct DObjectStr {
	BYTE bCmd;
} DObjectStr;

typedef struct DLevel {
	TCmdPItem item[MAXITEMS];
	DObjectStr object[MAXOBJECTS];
	DMonsterStr monster[MAXMONSTERS];
} DLevel;

typedef struct LocalLevel {
	BYTE automapsv[DMAXX][DMAXY];
} LocalLevel;

typedef struct DPortal {
	BYTE x;
	BYTE y;
	BYTE level;
	BYTE ltype;
	BYTE setlvl;
} DPortal;

typedef struct MultiQuests {
	BYTE qstate;
	BYTE qlog;
	BYTE qvar1;
} MultiQuests;

typedef struct DJunk {
	DPortal portal[MAXPORTAL];
	MultiQuests quests[MAXMULTIQUESTS];
} DJunk;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct TMegaPkt {
	struct TMegaPkt *pNext;
	DWORD dwSpaceLeft;
	BYTE data[32000];
} TMegaPkt;
#pragma pack(pop)

typedef struct TBuffer {
	DWORD dwNextWriteOffset;
	BYTE bData[4096];
} TBuffer;

//////////////////////////////////////////////////
// gamemenu/gmenu
//////////////////////////////////////////////////

// TPDEF PTR FCN VOID TMenuFcn

typedef struct TMenuItem {
	DWORD dwFlags;
	const char *pszStr;
	void (*fnMenu)(BOOL); /* fix, should have one arg */
} TMenuItem;

// TPDEF PTR FCN VOID TMenuUpdateFcn

//////////////////////////////////////////////////
// spells
//////////////////////////////////////////////////

typedef struct SpellData {
	unsigned char sName;
	unsigned char sManaCost;
	unsigned char sType;
	const char *sNameText;
	const char *sSkillText;
	int sBookLvl;
	int sStaffLvl;
	BOOL sTargeted;
	BOOL sTownSpell;
	int sMinInt;
	unsigned char sSFX;
	unsigned char sMissiles[3];
	unsigned char sManaAdj;
	unsigned char sMinMana;
	int sStaffMin;
	int sStaffMax;
	int sBookCost;
	int sStaffCost;
} SpellData;

//////////////////////////////////////////////////
// drlg
//////////////////////////////////////////////////

typedef struct ShadowStruct {
	unsigned char strig;
	unsigned char s1;
	unsigned char s2;
	unsigned char s3;
	unsigned char nv1;
	unsigned char nv2;
	unsigned char nv3;
} ShadowStruct;

typedef struct HALLNODE {
	int nHallx1;
	int nHally1;
	int nHallx2;
	int nHally2;
	int nHalldir;
	struct HALLNODE *pNext;
} HALLNODE;

typedef struct ROOMNODE {
	int nRoomx1;
	int nRoomy1;
	int nRoomx2;
	int nRoomy2;
	int nRoomDest;
} ROOMNODE;

//////////////////////////////////////////////////
// themes
//////////////////////////////////////////////////

typedef struct ThemeStruct {
	char ttype; /* aligned 4 */
	int ttval;
} ThemeStruct;

//////////////////////////////////////////////////
// inv
//////////////////////////////////////////////////

typedef struct InvXY {
	int X;
	int Y;
} InvXY;

//////////////////////////////////////////////////
// diabloui
//////////////////////////////////////////////////

// TPDEF PTR FCN VOID PLAYSND

typedef struct _gamedata {
	int dwSeed;
	BYTE bDiff;
	BYTE bRate;
} _gamedata;

typedef struct _uidefaultstats {
	WORD strength;
	WORD magic;
	WORD dexterity;
	WORD vitality;
} _uidefaultstats;

typedef struct _uiheroinfo {
	struct _uiheroinfo *next;
	char name[16];
	WORD level;
	BYTE heroclass;
	BYTE herorank;
	WORD strength;
	WORD magic;
	WORD dexterity;
	WORD vitality;
	int gold;
	int hassaved;
	BOOL spawned;
} _uiheroinfo;

// TPDEF PTR FCN UCHAR ENUMHEROPROC
// TPDEF PTR FCN UCHAR ENUMHEROS
// TPDEF PTR FCN UCHAR CREATEHERO
// TPDEF PTR FCN UCHAR DELETEHERO
// TPDEF PTR FCN UCHAR GETDEFHERO

// TPDEF PTR FCN INT PROGRESSFCN

//////////////////////////////////////////////////
// storm
//////////////////////////////////////////////////

// TPDEF PTR FCN UCHAR SMSGIDLEPROC
// TPDEF PTR FCN VOID SMSGHANDLER

typedef struct _SNETCAPS {
	DWORD size;
	DWORD flags;
	DWORD maxmessagesize;
	DWORD maxqueuesize;
	DWORD maxplayers;
	DWORD bytessec;
	DWORD latencyms;
	DWORD defaultturnssec;
	DWORD defaultturnsintransit;
} _SNETCAPS;

typedef struct _SNETEVENT {
	DWORD eventid;
	DWORD playerid;
	void *data;
	DWORD databytes;
} _SNETEVENT;

// TPDEF PTR FCN UCHAR SNETABORTPROC
// TPDEF PTR FCN UCHAR SNETCATEGORYPROC
// TPDEF PTR FCN UCHAR SNETCHECKAUTHPROC
// TPDEF PTR FCN UCHAR SNETCREATEPROC
// TPDEF PTR FCN UCHAR SNETDRAWDESCPROC
// TPDEF PTR FCN UCHAR SNETENUMDEVICESPROC
// TPDEF PTR FCN UCHAR SNETENUMGAMESPROC
// TPDEF PTR FCN UCHAR SNETENUMPROVIDERSPROC
// TPDEF PTR FCN VOID SNETEVENTPROC
// TPDEF PTR FCN UCHAR SNETGETARTPROC
// TPDEF PTR FCN UCHAR SNETGETDATAPROC
// TPDEF PTR FCN INT SNETMESSAGEBOXPROC
// TPDEF PTR FCN UCHAR SNETPLAYSOUNDPROC
// TPDEF PTR FCN UCHAR SNETSELECTEDPROC
// TPDEF PTR FCN UCHAR SNETSTATUSPROC

typedef struct _SNETPLAYERDATA {
	int size;
	char *playername;
	char *playerdescription;
	int reserved;
} _SNETPLAYERDATA;

typedef struct _SNETPROGRAMDATA {
	int size;
	const char *programname;
	const char *programdescription;
	int programid;
	int versionid;
	int reserved1;
	int maxplayers;
	_gamedata *initdata;
	int initdatabytes;
	void *reserved2;
	int optcategorybits;
	char *cdkey;
	char *registereduser;
	int spawned;
	int lcid;
} _SNETPROGRAMDATA;

typedef struct _SNETVERSIONDATA {
	int size;
	const char *versionstring;
	const char *executablefile;
	const char *originalarchivefile;
	const char *patcharchivefile;
} _SNETVERSIONDATA;

typedef struct _SNETUIDATA {
	int size;
	int uiflags;
	HWND parentwindow;
	void (*artcallback)();
	void (*authcallback)();
	void (*createcallback)();
	void (*drawdesccallback)();
	void (*selectedcallback)();
	void (*messageboxcallback)();
	void (*soundcallback)();
	void (*statuscallback)();
	void (*getdatacallback)();
	void (*categorycallback)();
	void (*categorylistcallback)();
	void (*newaccountcallback)();
	void (*profilecallback)();
	const char **profilefields;
	void (*profilebitmapcallback)();
	int (*selectnamecallback)(
	    const struct _SNETPROGRAMDATA *,
	    const struct _SNETPLAYERDATA *,
	    const struct _SNETUIDATA *,
	    const struct _SNETVERSIONDATA *,
	    DWORD provider, /* e.g. 'BNET', 'IPXN', 'MODM', 'SCBL' */
	    char *, DWORD,  /* character name will be copied here */
	    char *, DWORD,  /* character "description" will be copied here (used to advertise games) */
	    BOOL *          /* new character? - unsure about this */
	);
	void (*changenamecallback)();
} _SNETUIDATA;

// TPDEF PTR FCN UCHAR SNETSPIBIND
// TPDEF PTR FCN UCHAR SNETSPIQUERY

//////////////////////////////////////////////////
// tmsg
//////////////////////////////////////////////////

#pragma pack(push, 1)
typedef struct TMsg TMsg;

typedef struct TMsgHdr {
	TMsg *pNext;
	int dwTime;
	BYTE bLen;
} TMsgHdr;

typedef struct TMsg {
	TMsgHdr hdr;
	// this is actually alignment padding, but the message body is appended to the struct
	// so it's convenient to use byte-alignment and name it "body"
	unsigned char body[3];
} TMsg;
#pragma pack(pop)

//////////////////////////////////////////////////
// capture
//////////////////////////////////////////////////

typedef struct _PcxHeader {
	BYTE Manufacturer;
	BYTE Version;
	BYTE Encoding;
	BYTE BitsPerPixel;
	WORD Xmin;
	WORD Ymin;
	WORD Xmax;
	WORD Ymax;
	WORD HDpi;
	WORD VDpi;
	BYTE Colormap[48];
	BYTE Reserved;
	BYTE NPlanes;
	WORD BytesPerLine;
	WORD PaletteInfo;
	WORD HscreenSize;
	WORD VscreenSize;
	BYTE Filler[54];
} PCXHEADER;

DEVILUTION_END_NAMESPACE
