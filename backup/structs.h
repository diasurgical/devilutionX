//////////////////////////////////////////////////
// control
//////////////////////////////////////////////////

typedef struct RECT32 {
	int x;
	int y;
	int w;
	int h;
} RECT32;

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
	char *UIName;
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
	char *iName;
	char *iSName;
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
	int _iAnimLen;
	int _iAnimFrame;
	int _iAnimWidth;
	int _iAnimWidth2; // width 2?
	BOOL _iDelFlag; // set when item is flagged for deletion, deprecated in 1.02
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
} ItemStruct;

//////////////////////////////////////////////////
// player
//////////////////////////////////////////////////

typedef struct PlayerStruct {
	int _pmode;
	char walkpath[25];
	BOOLEAN plractive;
	int destAction;
	int destParam1;
	int destParam2;
	int destParam3;
	int destParam4;
	int plrlevel;
	int WorldX;
	int WorldY;
	int _px;
	int _py;
	int _ptargx;
	int _ptargy;
	int _pownerx;
	int _pownery;
	int _poldx;
	int _poldy;
	int _pxoff;
	int _pyoff;
	int _pxvel;
	int _pyvel;
	int _pdir;
	int _nextdir;
	int _pgfxnum;
	unsigned char *_pAnimData;
	int _pAnimDelay;
	int _pAnimCnt;
	int _pAnimLen;
	int _pAnimFrame;
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
	uint64_t _pMemSpells;
	uint64_t _pAblSpells;
	uint64_t _pScrlSpells;
	UCHAR _pSpellFlags;
	int _pSplHotKey[4];
	char _pSplTHotKey[4];
	int _pwtype;
	BOOLEAN _pBlockFlag;
	BOOLEAN _pInvincible;
	char _pLightRad;
	BOOLEAN _pLvlChanging;
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
	int _pVar1;
	int _pVar2;
	int _pVar3;
	int _pVar4;
	int _pVar5;
	int _pVar6;
	int _pVar7;
	int _pVar8;
	BOOLEAN _pLvlVisited[NUMLEVELS];
	BOOLEAN _pSLvlVisited[NUMLEVELS]; // only 10 used
	int _pGFXLoad;
	unsigned char *_pNAnim[8];
	int _pNFrames;
	int _pNWidth;
	unsigned char *_pWAnim[8];
	int _pWFrames;
	int _pWWidth;
	unsigned char *_pAAnim[8];
	int _pAFrames;
	int _pAWidth;
	int _pAFNum;
	unsigned char *_pLAnim[8];
	unsigned char *_pFAnim[8];
	unsigned char *_pTAnim[8];
	int _pSFrames;
	int _pSWidth;
	int _pSFNum;
	unsigned char *_pHAnim[8];
	int _pHFrames;
	int _pHWidth;
	unsigned char *_pDAnim[8];
	int _pDFrames;
	int _pDWidth;
	unsigned char *_pBAnim[8];
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
	uint64_t _pISpells;
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
	char bReserved[3];
	short wReserved[8];
	DWORD pDiabloKillLevel;
	int dwReserved[7];
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
	char *txtstr;
	int scrlltxt;
	int txtspd;
	int sfxnr;
} TextDataStruct;

//////////////////////////////////////////////////
// missiles
//////////////////////////////////////////////////

// TPDEF PTR FCN VOID MIADDPRC
// TPDEF PTR FCN VOID MIPROC

typedef struct MissileData {
	unsigned char mName;
	void(* mAddProc)(int, int, int, int, int, int, char, int, int);
	void(* mProc)(int);
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
	char *mName;
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
	int _mitype;
	int _mix;
	int _miy;
	int _mixoff;
	int _miyoff;
	int _mixvel;
	int _miyvel;
	int _misx;
	int _misy;
	int _mitxoff;
	int _mityoff;
	int _mimfnum;
	int _mispllvl;
	BOOL _miDelFlag;
	BYTE _miAnimType;
	int _miAnimFlags;
	unsigned char *_miAnimData;
	int _miAnimDelay;
	int _miAnimLen;
	int _miAnimWidth;
	int _miAnimWidth2;
	int _miAnimCnt;
	int _miAnimAdd;
	int _miAnimFrame;
	BOOL _miDrawFlag;
	BOOL _miLightFlag;
	BOOL _miPreFlag;
	int _miUniqTrans;
	int _mirange;
	int _misource;
	int _micaster;
	int _midam;
	BOOL _miHitFlag;
	int _midist;
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

// ADDED
/*
#define DEVILUTION_MINIWIN_COM

#ifndef DEVILUTION_ENGINE
#pragma push_macro("DECLARE_INTERFACE_")
#pragma push_macro("STDMETHOD")
#pragma push_macro("STDMETHOD_")
#pragma push_macro("THIS_")
#pragma push_macro("THIS")
#pragma push_macro("PURE")
#pragma push_macro("REFIID")
#undef DECLARE_INTERFACE_
#undef STDMETHOD
#undef STDMETHOD_
#undef THIS_
#undef THIS
#undef PURE
#undef REFIID
#endif


#define DECLARE_INTERFACE_(name, base) struct name : public base
#define THIS_
#define THIS
#define PURE = 0

#define STDMETHOD(name) STDMETHOD_(HRESULT, name)
#define STDMETHOD_(type, name) virtual WINAPI type name

typedef void *DVL_REFIID;
#define REFIID DVL_REFIID

struct IUnknown {
	// clang-format off
	STDMETHOD_(ULONG, Release)(THIS) PURE;
	// clang-format on
};






typedef unsigned short WORD;
typedef unsigned int   DWORD;



DECLARE_INTERFACE_(IDirectSoundBuffer, IUnknown)
{
	// clang-format off
	STDMETHOD(GetStatus)(THIS_ LPDWORD pdwStatus) PURE;
	STDMETHOD(Lock)(THIS_ DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1,
			LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags) PURE;
	STDMETHOD(Play)(THIS_ DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags) PURE;
	STDMETHOD(SetFormat)(THIS_ LPCWAVEFORMATEX pcfxFormat) PURE;
	STDMETHOD(SetVolume)(THIS_ LONG lVolume) PURE;
	STDMETHOD(SetPan)(THIS_ LONG lPan) PURE;
	STDMETHOD(Stop)(THIS) PURE;
	STDMETHOD(Unlock)(THIS_ LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2) PURE;
	STDMETHOD(Restore)(THIS) PURE;
	// clang-format on
};

typedef IDirectSoundBuffer *LPDIRECTSOUNDBUFFER;


typedef struct tWAVEFORMATEX {
	WORD wFormatTag;
	WORD nChannels;
	DWORD nSamplesPerSec;
	DWORD nAvgBytesPerSec;
	WORD nBlockAlign;
	WORD wBitsPerSample;
	WORD cbSize;
} WAVEFORMATEX, *LPWAVEFORMATEX, *LPCWAVEFORMATEX;




typedef struct CKINFO {
	DWORD dwSize;
	DWORD dwOffset;
} CKINFO;

typedef struct TSnd {
	WAVEFORMATEX fmt;
	CKINFO chunk;
	char *sound_path;
	LPDIRECTSOUNDBUFFER DSB;
	int start_tc;
} TSnd;

 */
//END


//Origional TSND 
 typedef struct TSnd {
 	char *sound_path;
 	LPDIRECTSOUNDBUFFER DSB;
 	int start_tc;
 } TSnd;

#pragma pack(push, 1)
typedef struct TSFX {
	unsigned char bFlags;
	char *pszName;
	TSnd *pSnd;
} TSFX;
#pragma pack(pop)

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
	char *GraphicType;
	BOOL has_special;
	char *sndfile;
	BOOL snd_special;
	BOOL has_trans;
	char *TransFile;
	int Frames[6];
	int Rate[6];
	char *mName;
	char mMinDLvl;
	char mMaxDLvl;
	char mLevel;
	int mMinHP;
	int mMaxHP;
	char mAi;
	int mFlags;
	unsigned char mInt;
	unsigned char mHit; // BUGFIX: Some monsters overflow this value on high difficulty
	unsigned char mAFNum;
	unsigned char mMinDamage;
	unsigned char mMaxDamage;
	unsigned char mHit2; // BUGFIX: Some monsters overflow this value on high difficulty
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
	unsigned char mtype;
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
	int _mx;
	int _my;
	int _mfutx;
	int _mfuty;
	int _moldx;
	int _moldy;
	int _mxoff;
	int _myoff;
	int _mxvel;
	int _myvel;
	int _mdir;
	int _menemy;
	unsigned char _menemyx;
	unsigned char _menemyy;
	short falign_52; // probably _mAFNum (unused)
	unsigned char *_mAnimData;
	int _mAnimDelay;
	int _mAnimCnt;
	int _mAnimLen;
	int _mAnimFrame;
	BOOL _meflag;
	BOOL _mDelFlag;
	int _mVar1;
	int _mVar2;
	int _mVar3;
	int _mVar4;
	int _mVar5;
	int _mVar6;
	int _mVar7;
	int _mVar8;
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
	unsigned char mHit;
	unsigned char mMinDamage;
	unsigned char mMaxDamage;
	unsigned char mHit2;
	unsigned char mMinDamage2;
	unsigned char mMaxDamage2;
	unsigned char mArmorClass;
	char falign_CB;
	unsigned short mMagicRes;
	int mtalkmsg;
	unsigned char leader;
	unsigned char leaderflag;
	unsigned char packsize;
	unsigned char mlid;
	char *mName;
	CMonster *MType;
	MonsterData *MData;
} MonsterStruct;

typedef struct UniqMonstStruct {
	char mtype;
	char *mName;
	char *mTrnName;
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
	int oAnimDelay;
	int oAnimLen;
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
	int _oAnimDelay;
	int _oAnimCnt;
	int _oAnimLen;
	int _oAnimFrame;
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
} TCmdPItem;

typedef struct TCmdChItem {
	BYTE bCmd;
	BYTE bLoc;
	WORD wIndx;
	WORD wCI;
	int dwSeed;
	BYTE bId;
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
// quests
//////////////////////////////////////////////////

typedef struct QuestStruct {
	unsigned char _qlevel;
	unsigned char _qtype;
	unsigned char _qactive;
	unsigned char _qlvltype;
	int _qtx;
	int _qty;
	unsigned char _qslvl;
	unsigned char _qidx;
	unsigned char _qmsg;
	unsigned char _qvar1;
	unsigned char _qvar2;
	int _qlog;
} QuestStruct;

typedef struct QuestData {
	unsigned char _qdlvl;
	char _qdmultlvl;
	unsigned char _qlvlt;
	unsigned char _qdtype;
	unsigned char _qdrnd;
	unsigned char _qslvl;
	int _qflags; /* unsigned char */
	int _qdmsg;
	char *_qlstr;
} QuestData;

//////////////////////////////////////////////////
// gamemenu/gmenu
//////////////////////////////////////////////////

// TPDEF PTR FCN VOID TMenuFcn

typedef struct TMenuItem {
	DWORD dwFlags;
	char *pszStr;
	void(* fnMenu)(BOOL); /* fix, should have one arg */
} TMenuItem;

// TPDEF PTR FCN VOID TMenuUpdateFcn

//////////////////////////////////////////////////
// spells
//////////////////////////////////////////////////

typedef struct SpellData {
	unsigned char sName;
	unsigned char sManaCost;
	unsigned char sType;
	char *sNameText;
	char *sSkillText;
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
// towners
//////////////////////////////////////////////////

typedef struct TNQ {
	unsigned char _qsttype;
	unsigned char _qstmsg;
	BOOLEAN _qstmsgact;
} TNQ;

typedef struct TownerStruct {
	int _tmode;
	int _ttype;
	int _tx;
	int _ty;
	int _txoff;
	int _tyoff;
	int _txvel;
	int _tyvel;
	int _tdir;
	unsigned char *_tAnimData;
	int _tAnimDelay;
	int _tAnimCnt;
	int _tAnimLen;
	int _tAnimFrame;
	int _tAnimFrameCnt;
	char _tAnimOrder;
	int _tAnimWidth;
	int _tAnimWidth2;
	int _tTenPer;
	int _teflag;
	int _tbtcnt;
	int _tSelFlag;
	int _tMsgSaid;
	TNQ qsts[16];
	int _tSeed;
	int _tVar1;
	int _tVar2;
	int _tVar3;
	int _tVar4;
	char _tName[PLR_NAME_LEN];
	unsigned char *_tNAnim[8];
	int _tNFrames;
	unsigned char *_tNData;
} TownerStruct;

typedef struct QuestTalkData {
	int _qinfra;
	int _qblkm;
	int _qgarb;
	int _qzhar;
	int _qveil;
	int _qmod;
	int _qbutch;
	int _qbol;
	int _qblind;
	int _qblood;
	int _qanvil;
	int _qwarlrd;
	int _qking;
	int _qpw;
	int _qbone;
	int _qvb;
} QuestTalkData;

//////////////////////////////////////////////////
// gendung
//////////////////////////////////////////////////

typedef struct ScrollStruct {
	int _sxoff;
	int _syoff;
	int _sdx;
	int _sdy;
	int _sdir;
} ScrollStruct;

typedef struct THEME_LOC {
	int x;
	int y;
	int ttval;
	int width;
	int height;
} THEME_LOC;

typedef struct MICROS {
	WORD mt[16];
} MICROS;

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
// lighting
//////////////////////////////////////////////////

typedef struct LightListStruct {
	int _lx;
	int _ly;
	int _lradius;
	int _lid;
	int _ldel;
	int _lunflag;
	int field_18;
	int _lunx;
	int _luny;
	int _lunr;
	int _xoff;
	int _yoff;
	int _lflags;
} LightListStruct;

//////////////////////////////////////////////////
// dead
//////////////////////////////////////////////////

typedef struct DeadStruct {
	unsigned char *_deadData[8];
	int _deadFrame;
	int _deadWidth;
	int _deadWidth2;
	char _deadtrans;
} DeadStruct;

//////////////////////////////////////////////////
// diabloui
//////////////////////////////////////////////////

// TPDEF PTR FCN VOID PLAYSND

typedef struct _gamedata {
	int dwSeed;
	BYTE bDiff;
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
	char *programname;
	char *programdescription;
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
	char *versionstring;
	char *executablefile;
	char *originalarchivefile;
	char *patcharchivefile;
} _SNETVERSIONDATA;

typedef struct _SNETUIDATA {
	int size;
	int uiflags;
	HWND parentwindow;
	void(* artcallback)();
	void(* authcallback)();
	void(* createcallback)();
	void(* drawdesccallback)();
	void(* selectedcallback)();
	void(* messageboxcallback)();
	void(* soundcallback)();
	void(* statuscallback)();
	void(* getdatacallback)();
	void(* categorycallback)();
	void(* categorylistcallback)();
	void(* newaccountcallback)();
	void(* profilecallback)();
	int profilefields;
	void(* profilebitmapcallback)();
	int(__stdcall *selectnamecallback)(
	    const struct _SNETPROGRAMDATA *,
	    const struct _SNETPLAYERDATA *,
	    const struct _SNETUIDATA *,
	    const struct _SNETVERSIONDATA *,
	    DWORD provider, /* e.g. 'BNET', 'IPXN', 'MODM', 'SCBL' */
	    char *, DWORD,  /* character name will be copied here */
	    char *, DWORD,  /* character "description" will be copied here (used to advertise games) */
	    BOOL *          /* new character? - unsure about this */
	    );
	void(* changenamecallback)();
} _SNETUIDATA;

// TPDEF PTR FCN UCHAR SNETSPIBIND
// TPDEF PTR FCN UCHAR SNETSPIQUERY

//////////////////////////////////////////////////
// pack
//////////////////////////////////////////////////

#pragma pack(push, 1)
typedef struct PkItemStruct {
	DWORD iSeed;
	WORD iCreateInfo;
	WORD idx;
	BYTE bId;
	BYTE bDur;
	BYTE bMDur;
	BYTE bCh;
	BYTE bMCh;
	WORD wValue;
	DWORD dwBuff;
} PkItemStruct;

typedef struct PkPlayerStruct {
	FILETIME archiveTime;
	char destAction;
	char destParam1;
	char destParam2;
	BYTE plrlevel;
	BYTE px;
	BYTE py;
	BYTE targx;
	BYTE targy;
	char pName[PLR_NAME_LEN];
	char pClass;
	BYTE pBaseStr;
	BYTE pBaseMag;
	BYTE pBaseDex;
	BYTE pBaseVit;
	char pLevel;
	BYTE pStatPts;
	int pExperience;
	int pGold;
	int pHPBase;
	int pMaxHPBase;
	int pManaBase;
	int pMaxManaBase;
	char pSplLvl[MAX_SPELLS];
	uint64_t pMemSpells;
	PkItemStruct InvBody[NUM_INVLOC];
	PkItemStruct InvList[NUM_INV_GRID_ELEM];
	char InvGrid[NUM_INV_GRID_ELEM];
	BYTE _pNumInv;
	PkItemStruct SpdList[MAXBELTITEMS];
	char pTownWarps;
	char pDungMsgs;
	char pLvlLoad;
	char pBattleNet;
	BOOLEAN pManaShield;
	char bReserved[3];
	short wReserved[8];
	int pDiabloKillLevel;
	int dwReserved[7];
} PkPlayerStruct;
#pragma pack(pop)

//////////////////////////////////////////////////
// path
//////////////////////////////////////////////////

typedef struct PATHNODE {
	char f;
	char h;
	char g;
	int x;
	int y;
	struct PATHNODE *Parent;
	struct PATHNODE *Child[8];
	struct PATHNODE *NextNode;
} PATHNODE;

// TPDEF PTR FCN UCHAR CHECKFUNC1

// TPDEF PTR FCN UCHAR CHECKFUNC

//////////////////////////////////////////////////
// sha
//////////////////////////////////////////////////

typedef struct SHA1Context {
	DWORD state[5];
	DWORD count[2];
	char buffer[64];
} SHA1Context;

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
// mpqapi
//////////////////////////////////////////////////

typedef struct _FILEHEADER {
	int signature;
	int headersize;
	int filesize;
	WORD version;
	short sectorsizeid;
	int hashoffset;
	int blockoffset;
	int hashcount;
	int blockcount;
	char pad[72];
} _FILEHEADER;

typedef struct _HASHENTRY {
	int hashcheck[2];
	int lcid;
	int block;
} _HASHENTRY;

typedef struct _BLOCKENTRY {
	int offset;
	int sizealloc;
	int sizefile;
	int flags;
} _BLOCKENTRY;

// TPDEF PTR FCN UCHAR TGetNameFcn

// TPDEF PTR FCN VOID TCrypt

//////////////////////////////////////////////////
// trigs
//////////////////////////////////////////////////

typedef struct TriggerStruct {
	int _tx;
	int _ty;
	int _tmsg;
	int _tlvl;
} TriggerStruct;

//////////////////////////////////////////////////
// stores
//////////////////////////////////////////////////

typedef struct STextStruct {
	int _sx;
	int _syoff;
	char _sstr[128];
	int _sjust;
	char _sclr;
	int _sline;
	int _ssel;
	int _sval;
} STextStruct;

//////////////////////////////////////////////////
// wave
//////////////////////////////////////////////////

typedef struct MEMFILE {
	DWORD end;
	LONG offset;
	DWORD buf_len;
	DWORD dist;
	DWORD bytes_to_read;
	BYTE *buf;
	HANDLE file;
} MEMFILE;

//////////////////////////////////////////////////
// plrmsg
//////////////////////////////////////////////////

typedef struct _plrmsg {
	DWORD time;
	unsigned char player;
	char str[144];
} _plrmsg;

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

//////////////////////////////////////////////////
// encrypt
//////////////////////////////////////////////////

typedef struct TDataInfo {
	BYTE *srcData;
	DWORD srcOffset;
	BYTE *destData;
	DWORD destOffset;
	DWORD size;
} TDataInfo;
