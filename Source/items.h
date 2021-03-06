/**
 * @file items.h
 *
 * Interface of item functionality.
 */
#ifndef __ITEMS_H__
#define __ITEMS_H__

#include "itemdat.h"

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

typedef enum item_quality {
	ITEM_QUALITY_NORMAL,
	ITEM_QUALITY_MAGIC,
	ITEM_QUALITY_UNIQUE,
} item_quality;

/*
 First 5 bits store level
 6th bit stores onlygood flag
 7th bit stores uper15 flag - uper means unique percent, this flag is true for unique monsters and loot from them has 15% to become unique
 8th bit stores uper1 flag - this is loot from normal monsters, which has 1% to become unique
 9th bit stores info if item is unique
 10th bit stores info if item is a basic one from griswold
 11th bit stores info if item is a premium one from griswold
 12th bit stores info if item is from wirt
 13th bit stores info if item is from adria
 14th bit stores info if item is from pepin
 15th bit stores pregen flag

 combining CF_UPER15 and CF_UPER1 flags (CF_USEFUL) is used to mark potions and town portal scrolls created on the ground
 CF_TOWN is combining all store flags and indicates if item has been bought from a NPC
 */
typedef enum icreateinfo_flag {
	CF_LEVEL        = (1 << 6) - 1,
	CF_ONLYGOOD     = 1 << 6,
	CF_UPER15       = 1 << 7,
	CF_UPER1        = 1 << 8,
	CF_UNIQUE       = 1 << 9,
	CF_SMITH        = 1 << 10,
	CF_SMITHPREMIUM = 1 << 11,
	CF_BOY          = 1 << 12,
	CF_WITCH        = 1 << 13,
	CF_HEALER       = 1 << 14,
	CF_PREGEN       = 1 << 15,

	CF_USEFUL = CF_UPER15 | CF_UPER1,
	CF_TOWN   = CF_SMITH | CF_SMITHPREMIUM | CF_BOY | CF_WITCH | CF_HEALER,
} icreateinfo_flag;

typedef struct ItemStruct {
	Sint32 _iSeed;
	Uint16 _iCreateInfo;
	enum item_type _itype;
	Sint32 _ix;
	Sint32 _iy;
	bool _iAnimFlag;
	Uint8 *_iAnimData;  // PSX name -> ItemFrame
	Sint32 _iAnimLen;   // Number of frames in current animation
	Sint32 _iAnimFrame; // Current frame of animation.
	Sint32 _iAnimWidth;
	Sint32 _iAnimWidth2; // width 2?
	bool _iDelFlag;      // set when item is flagged for deletion, deprecated in 1.02
	Sint8 _iSelFlag;
	bool _iPostDraw;
	bool _iIdentified;
	Sint8 _iMagical;
	char _iName[64];
	char _iIName[64];
	enum item_equip_type _iLoc;
	enum item_class _iClass;
	Sint32 _iCurs;
	Sint32 _ivalue;
	Sint32 _iIvalue;
	Sint32 _iMinDam;
	Sint32 _iMaxDam;
	Sint32 _iAC;
	Sint32 _iFlags; // item_special_effect
	enum item_misc_id _iMiscId;
	enum spell_id _iSpell;
	Sint32 _iCharges;
	Sint32 _iMaxCharges;
	Sint32 _iDurability;
	Sint32 _iMaxDur;
	Sint32 _iPLDam;
	Sint32 _iPLToHit;
	Sint32 _iPLAC;
	Sint32 _iPLStr;
	Sint32 _iPLMag;
	Sint32 _iPLDex;
	Sint32 _iPLVit;
	Sint32 _iPLFR;
	Sint32 _iPLLR;
	Sint32 _iPLMR;
	Sint32 _iPLMana;
	Sint32 _iPLHP;
	Sint32 _iPLDamMod;
	Sint32 _iPLGetHit;
	Sint32 _iPLLight;
	Sint8 _iSplLvlAdd;
	Sint8 _iRequest;
	Sint32 _iUid;
	Sint32 _iFMinDam;
	Sint32 _iFMaxDam;
	Sint32 _iLMinDam;
	Sint32 _iLMaxDam;
	Sint32 _iPLEnAc;
	enum item_effect_type _iPrePower;
	enum item_effect_type _iSufPower;
	Sint32 _iVAdd1;
	Sint32 _iVMult1;
	Sint32 _iVAdd2;
	Sint32 _iVMult2;
	Sint8 _iMinStr;
	Uint8 _iMinMag;
	Sint8 _iMinDex;
	bool _iStatFlag;
	Sint32 IDidx;
	Sint32 _iDamAcFlags;

	/**
	 * @brief Checks whether this item is empty or not.
	 * @return 'True' in case the item is empty and 'False' otherwise.
	 */
	bool isEmpty() const
	{
		return this->_itype == ITYPE_NONE;
	}

} ItemStruct;

typedef struct ItemGetRecordStruct {
	Sint32 nSeed;
	Uint16 wCI;
	Sint32 nIndex;
	Uint32 dwTimestamp;
} ItemGetRecordStruct;

typedef struct CornerStoneStruct {
	Sint32 x;
	Sint32 y;
	bool activated;
	ItemStruct item;
} CornerStoneStruct;

extern int itemactive[MAXITEMS];
extern BOOL uitemflag;
extern int itemavail[MAXITEMS];
extern ItemStruct item[MAXITEMS + 1];
extern CornerStoneStruct CornerStone;
extern BOOL UniqueItemFlag[128];
extern int auricGold;
extern int numitems;

bool IsItemAvailable(int i);
void InitItemGFX();
void InitItems();
void CalcPlrItemVals(int p, BOOL Loadgfx);
void CalcPlrScrolls(int p);
void CalcPlrStaff(int p);
void CalcPlrInv(int p, BOOL Loadgfx);
void SetPlrHandItem(ItemStruct *h, int idata);
void GetPlrHandSeed(ItemStruct *h);
void GetGoldSeed(int pnum, ItemStruct *h);
int GetGoldCursor(int value);
void SetPlrHandGoldCurs(ItemStruct *h);
void CreatePlrItems(int p);
BOOL ItemSpaceOk(int i, int j);
int AllocateItem();
void GetSuperItemLoc(int x, int y, int *xx, int *yy);
void GetItemAttrs(int i, int idata, int lvl);
void SaveItemPower(int i, int power, int param1, int param2, int minval, int maxval, int multval);
void GetItemPower(int i, int minlvl, int maxlvl, int flgs, BOOL onlygood);
void SetupItem(int i);
int RndItem(int m);
void SpawnUnique(int uid, int x, int y);
void SpawnItem(int m, int x, int y, BOOL sendmsg);
void CreateRndItem(int x, int y, BOOL onlygood, BOOL sendmsg, BOOL delta);
void CreateRndUseful(int pnum, int x, int y, BOOL sendmsg);
void CreateTypeItem(int x, int y, BOOL onlygood, int itype, int imisc, BOOL sendmsg, BOOL delta);
void RecreateItem(int ii, int idx, WORD icreateinfo, int iseed, int ivalue);
void RecreateEar(int ii, WORD ic, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, int ibuff);
void items_427A72();
void items_427ABA(int x, int y);
void SpawnQuestItem(int itemid, int x, int y, int randarea, int selflag);
void SpawnRock();
void SpawnRewardItem(int itemid, int xx, int yy);
void SpawnMapOfDoom(int xx, int yy);
void SpawnRuneBomb(int xx, int yy);
void SpawnTheodore(int xx, int yy);
void RespawnItem(int i, BOOL FlipFlag);
void DeleteItem(int ii, int i);
void ProcessItems();
void FreeItemGFX();
void GetItemFrm(int i);
void GetItemStr(int i);
void CheckIdentify(int pnum, int cii);
void DoRepair(int pnum, int cii);
void DoRecharge(int pnum, int cii);
void DoOil(int pnum, int cii);
void PrintItemPower(char plidx, ItemStruct *x);
void DrawUniqueInfo(CelOutputBuffer out);
void PrintItemDetails(ItemStruct *x);
void PrintItemDur(ItemStruct *x);
void UseItem(int p, int Mid, int spl);
BOOL StoreStatOk(ItemStruct *h);
void SpawnSmith(int lvl);
void SpawnPremium(int pnum);
void WitchBookLevel(int ii);
void SpawnWitch(int lvl);
void SpawnBoy(int lvl);
void SpawnHealer(int lvl);
void SpawnStoreGold();
void RecreateTownItem(int ii, int idx, WORD icreateinfo, int iseed, int ivalue);
void RecalcStoreStats();
int ItemNoFlippy();
void CreateSpellBook(int x, int y, int ispell, BOOL sendmsg, BOOL delta);
void CreateMagicArmor(int x, int y, int imisc, int icurs, BOOL sendmsg, BOOL delta);
void CreateAmulet(int x, int y, int curlv, BOOL sendmsg, BOOL delta);
void CreateMagicWeapon(int x, int y, int imisc, int icurs, BOOL sendmsg, BOOL delta);
BOOL GetItemRecord(int nSeed, WORD wCI, int nIndex);
void SetItemRecord(int nSeed, WORD wCI, int nIndex);
void PutItemRecord(int nSeed, WORD wCI, int nIndex);

/* data */

extern int MaxGold;

extern BYTE ItemCAnimTbl[];
extern int ItemInvSnds[];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __ITEMS_H__ */
