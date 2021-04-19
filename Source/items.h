/**
 * @file items.h
 *
 * Interface of item functionality.
 */
#pragma once

#include <stdint.h>

#include "engine.h"
#include "itemdat.h"

namespace devilution {

#define MAXITEMS 127

#define ITEMTYPES 43

#define GOLD_SMALL_LIMIT 1000
#define GOLD_MEDIUM_LIMIT 2500
#define GOLD_MAX_LIMIT 5000

// Item indestructible durability
#define DUR_INDESTRUCTIBLE 255

enum item_quality : uint8_t {
	ITEM_QUALITY_NORMAL,
	ITEM_QUALITY_MAGIC,
	ITEM_QUALITY_UNIQUE,
};

enum _unique_items : int8_t {
	UITEM_CLEAVER,
	UITEM_SKCROWN,
	UITEM_INFRARING,
	UITEM_OPTAMULET,
	UITEM_TRING,
	UITEM_HARCREST,
	UITEM_STEELVEIL,
	UITEM_ARMOFVAL,
	UITEM_GRISWOLD,
	UITEM_BOVINE,
	UITEM_RIFTBOW,
	UITEM_NEEDLER,
	UITEM_CELESTBOW,
	UITEM_DEADLYHUNT,
	UITEM_BOWOFDEAD,
	UITEM_BLKOAKBOW,
	UITEM_FLAMEDART,
	UITEM_FLESHSTING,
	UITEM_WINDFORCE,
	UITEM_EAGLEHORN,
	UITEM_GONNAGALDIRK,
	UITEM_DEFENDER,
	UITEM_GRYPHONCLAW,
	UITEM_BLACKRAZOR,
	UITEM_GIBBOUSMOON,
	UITEM_ICESHANK,
	UITEM_EXECUTIONER,
	UITEM_BONESAW,
	UITEM_SHADHAWK,
	UITEM_WIZSPIKE,
	UITEM_LGTSABRE,
	UITEM_FALCONTALON,
	UITEM_INFERNO,
	UITEM_DOOMBRINGER,
	UITEM_GRIZZLY,
	UITEM_GRANDFATHER,
	UITEM_MANGLER,
	UITEM_SHARPBEAK,
	UITEM_BLOODLSLAYER,
	UITEM_CELESTAXE,
	UITEM_WICKEDAXE,
	UITEM_STONECLEAV,
	UITEM_AGUHATCHET,
	UITEM_HELLSLAYER,
	UITEM_MESSERREAVER,
	UITEM_CRACKRUST,
	UITEM_JHOLMHAMM,
	UITEM_CIVERBS,
	UITEM_CELESTSTAR,
	UITEM_BARANSTAR,
	UITEM_GNARLROOT,
	UITEM_CRANBASH,
	UITEM_SCHAEFHAMM,
	UITEM_DREAMFLANGE,
	UITEM_STAFFOFSHAD,
	UITEM_IMMOLATOR,
	UITEM_STORMSPIRE,
	UITEM_GLEAMSONG,
	UITEM_THUNDERCALL,
	UITEM_PROTECTOR,
	UITEM_NAJPUZZLE,
	UITEM_MINDCRY,
	UITEM_RODOFONAN,
	UITEM_SPIRITSHELM,
	UITEM_THINKINGCAP,
	UITEM_OVERLORDHELM,
	UITEM_FOOLSCREST,
	UITEM_GOTTERDAM,
	UITEM_ROYCIRCLET,
	UITEM_TORNFLESH,
	UITEM_GLADBANE,
	UITEM_RAINCLOAK,
	UITEM_LEATHAUT,
	UITEM_WISDWRAP,
	UITEM_SPARKMAIL,
	UITEM_SCAVCARAP,
	UITEM_NIGHTSCAPE,
	UITEM_NAJPLATE,
	UITEM_DEMONSPIKE,
	UITEM_DEFLECTOR,
	UITEM_SKULLSHLD,
	UITEM_DRAGONBRCH,
	UITEM_BLKOAKSHLD,
	UITEM_HOLYDEF,
	UITEM_STORMSHLD,
	UITEM_BRAMBLE,
	UITEM_REGHA,
	UITEM_BLEEDER,
	UITEM_CONSTRICT,
	UITEM_ENGAGE,
	UITEM_INVALID = -1,
};

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
enum icreateinfo_flag {
	// clang-format off
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
	// clang-format on
};

enum icreateinfo_flag2 {
	// clang-format off
	CF_HELLFIRE = 1,
	// clang-format on
};

struct ItemStruct {
	Sint32 _iSeed;
	Uint16 _iCreateInfo;
	enum item_type _itype;
	Sint16 _ix;
	Sint16 _iy;
	bool _iAnimFlag;
	Uint8 *_iAnimData; // PSX name -> ItemFrame
	Uint8 _iAnimLen;   // Number of frames in current animation
	Uint8 _iAnimFrame; // Current frame of animation.
	Sint32 _iAnimWidth;
	Sint32 _iAnimWidth2; // width 2?
	bool _iDelFlag;      // set when item is flagged for deletion, deprecated in 1.02
	Uint8 _iSelFlag;
	bool _iPostDraw;
	bool _iIdentified;
	Sint8 _iMagical;
	char _iName[64];
	char _iIName[64];
	enum item_equip_type _iLoc;
	enum item_class _iClass;
	Uint8 _iCurs;
	Sint32 _ivalue;
	Sint32 _iIvalue;
	Uint8 _iMinDam;
	Uint8 _iMaxDam;
	Sint16 _iAC;
	Sint32 _iFlags; // item_special_effect
	enum item_misc_id _iMiscId;
	enum spell_id _iSpell;
	Sint32 _iCharges;
	Sint32 _iMaxCharges;
	Uint8 _iDurability;
	Uint8 _iMaxDur;
	Sint16 _iPLDam;
	Sint16 _iPLToHit;
	Sint16 _iPLAC;
	Sint16 _iPLStr;
	Sint16 _iPLMag;
	Sint16 _iPLDex;
	Sint16 _iPLVit;
	Sint16 _iPLFR;
	Sint16 _iPLLR;
	Sint16 _iPLMR;
	Sint16 _iPLMana;
	Sint16 _iPLHP;
	Sint16 _iPLDamMod;
	Sint16 _iPLGetHit;
	Sint16 _iPLLight;
	Sint8 _iSplLvlAdd;
	Sint8 _iRequest;
	Sint32 _iUid;
	Sint16 _iFMinDam;
	Sint16 _iFMaxDam;
	Sint16 _iLMinDam;
	Sint16 _iLMaxDam;
	Sint16 _iPLEnAc;
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
	Uint32 dwBuff;
	Sint32 _iDamAcFlags;

	/**
	 * @brief Checks whether this item is empty or not.
	 * @return 'True' in case the item is empty and 'False' otherwise.
	 */
	bool isEmpty() const
	{
		return this->_itype == ITYPE_NONE;
	}

	/**
	 * @brief Checks whether this item is an equipment.
	 * @return 'True' in case the item is an equipment and 'False' otherwise.
	 */
	bool isEquipment() const
	{
		if (this->isEmpty()) {
			return false;
		}

		switch (this->_iLoc) {
		case ILOC_AMULET:
		case ILOC_ARMOR:
		case ILOC_HELM:
		case ILOC_ONEHAND:
		case ILOC_RING:
		case ILOC_TWOHAND:
			return true;

		default:
			return false;
		}
	}

	/**
	 * @brief Checks whether this item is a weapon.
	 * @return 'True' in case the item is a weapon and 'False' otherwise.
	 */
	bool isWeapon() const
	{
		if (this->isEmpty()) {
			return false;
		}

		switch (this->_itype) {
		case ITYPE_AXE:
		case ITYPE_BOW:
		case ITYPE_MACE:
		case ITYPE_STAFF:
		case ITYPE_SWORD:
			return true;

		default:
			return false;
		}
	}

	/**
	 * @brief Checks whether this item is an armor.
	 * @return 'True' in case the item is an armor and 'False' otherwise.
	 */
	bool isArmor() const
	{
		if (this->isEmpty()) {
			return false;
		}

		switch (this->_itype) {
		case ITYPE_HARMOR:
		case ITYPE_LARMOR:
		case ITYPE_MARMOR:
			return true;

		default:
			return false;
		}
	}

	/**
	 * @brief Checks whether this item is a helm.
	 * @return 'True' in case the item is a helm and 'False' otherwise.
	 */
	bool isHelm() const
	{
		return !this->isEmpty() && this->_itype == ITYPE_HELM;
	}

	/**
	 * @brief Checks whether this item is a shield.
	 * @return 'True' in case the item is a shield and 'False' otherwise.
	 */
	bool isShield() const
	{
		return !this->isEmpty() && this->_itype == ITYPE_SHIELD;
	}

	/**
	 * @brief Checks whether this item is a jewelry.
	 * @return 'True' in case the item is a jewelry and 'False' otherwise.
	 */
	bool isJewelry() const
	{
		if (this->isEmpty()) {
			return false;
		}

		switch (this->_itype) {
		case ITYPE_AMULET:
		case ITYPE_RING:
			return true;

		default:
			return false;
		}
	}
};

struct ItemGetRecordStruct {
	Sint32 nSeed;
	Uint16 wCI;
	Sint32 nIndex;
	Uint32 dwTimestamp;
};

struct CornerStoneStruct {
	Sint32 x;
	Sint32 y;
	bool activated;
	ItemStruct item;
};

extern int itemactive[MAXITEMS];
extern bool uitemflag;
extern int itemavail[MAXITEMS];
extern ItemStruct items[MAXITEMS + 1];
extern CornerStoneStruct CornerStone;
extern bool UniqueItemFlag[128];
extern int numitems;

BYTE GetOutlineColor(ItemStruct &item, bool checkReq);
bool IsItemAvailable(int i);
bool IsUniqueAvailable(int i);
void InitItemGFX();
void InitItems();
void CalcPlrItemVals(int p, bool Loadgfx);
void CalcPlrScrolls(int p);
void CalcPlrStaff(int p);
void CalcPlrInv(int p, bool Loadgfx);
void SetPlrHandItem(ItemStruct *h, int idata);
void GetPlrHandSeed(ItemStruct *h);
void GetGoldSeed(int pnum, ItemStruct *h);
int GetGoldCursor(int value);
void SetPlrHandGoldCurs(ItemStruct *h);
void CreatePlrItems(int p);
bool ItemSpaceOk(int i, int j);
int AllocateItem();
void GetSuperItemLoc(int x, int y, int *xx, int *yy);
void GetItemAttrs(int i, int idata, int lvl);
void SaveItemPower(int i, item_effect_type power, int param1, int param2, int minval, int maxval, int multval);
void GetItemPower(int i, int minlvl, int maxlvl, affix_item_type flgs, bool onlygood);
void SetupItem(int i);
int RndItem(int m);
void SpawnUnique(_unique_items uid, int x, int y);
void SpawnItem(int m, int x, int y, bool sendmsg);
void CreateRndItem(int x, int y, bool onlygood, bool sendmsg, bool delta);
void CreateRndUseful(int pnum, int x, int y, bool sendmsg);
void CreateTypeItem(int x, int y, bool onlygood, int itype, int imisc, bool sendmsg, bool delta);
void RecreateItem(int ii, int idx, WORD icreateinfo, int iseed, int ivalue, bool isHellfire);
void RecreateEar(int ii, WORD ic, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, int ibuff);
void items_427A72();
void items_427ABA(int x, int y);
void SpawnQuestItem(int itemid, int x, int y, int randarea, int selflag);
void SpawnRock();
void SpawnRewardItem(int itemid, int xx, int yy);
void SpawnMapOfDoom(int xx, int yy);
void SpawnRuneBomb(int xx, int yy);
void SpawnTheodore(int xx, int yy);
void RespawnItem(ItemStruct *item, bool FlipFlag);
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
void UseItem(int p, item_misc_id Mid, spell_id spl);
bool StoreStatOk(ItemStruct *h);
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
void CreateSpellBook(int x, int y, spell_id ispell, bool sendmsg, bool delta);
void CreateMagicArmor(int x, int y, int imisc, int icurs, bool sendmsg, bool delta);
void CreateAmulet(int x, int y, int curlv, bool sendmsg, bool delta);
void CreateMagicWeapon(int x, int y, int imisc, int icurs, bool sendmsg, bool delta);
bool GetItemRecord(int nSeed, WORD wCI, int nIndex);
void SetItemRecord(int nSeed, WORD wCI, int nIndex);
void PutItemRecord(int nSeed, WORD wCI, int nIndex);

/* data */

extern int MaxGold;

extern BYTE ItemCAnimTbl[];
extern _sfx_id ItemInvSnds[];

} // namespace devilution
