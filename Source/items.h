/**
 * @file items.h
 *
 * Interface of item functionality.
 */
#pragma once

#include <cstdint>

#include "DiabloUI/ui_item.h"
#include "engine.h"
#include "itemdat.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

#define MAXITEMS 127

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

// All item animation frames have this width.
constexpr int ItemAnimWidth = 96;

struct ItemStruct {
	int32_t _iSeed;
	uint16_t _iCreateInfo;
	enum item_type _itype;
	Point position;
	bool _iAnimFlag;
	CelSprite *_iAnimData; // PSX name -> ItemFrame
	uint8_t _iAnimLen;     // Number of frames in current animation
	uint8_t _iAnimFrame;   // Current frame of animation.
	bool _iDelFlag;        // set when item is flagged for deletion, deprecated in 1.02
	uint8_t _iSelFlag;
	bool _iPostDraw;
	bool _iIdentified;
	int8_t _iMagical;
	char _iName[64];
	char _iIName[64];
	enum item_equip_type _iLoc;
	enum item_class _iClass;
	uint8_t _iCurs;
	int _ivalue;
	int _iIvalue;
	uint8_t _iMinDam;
	uint8_t _iMaxDam;
	int16_t _iAC;
	uint32_t _iFlags; // item_special_effect
	enum item_misc_id _iMiscId;
	enum spell_id _iSpell;
	int _iCharges;
	int _iMaxCharges;
	uint8_t _iDurability;
	uint8_t _iMaxDur;
	int16_t _iPLDam;
	int16_t _iPLToHit;
	int16_t _iPLAC;
	int16_t _iPLStr;
	int16_t _iPLMag;
	int16_t _iPLDex;
	int16_t _iPLVit;
	int16_t _iPLFR;
	int16_t _iPLLR;
	int16_t _iPLMR;
	int16_t _iPLMana;
	int16_t _iPLHP;
	int16_t _iPLDamMod;
	int16_t _iPLGetHit;
	int16_t _iPLLight;
	int8_t _iSplLvlAdd;
	int8_t _iRequest;
	int _iUid;
	int16_t _iFMinDam;
	int16_t _iFMaxDam;
	int16_t _iLMinDam;
	int16_t _iLMaxDam;
	int16_t _iPLEnAc;
	enum item_effect_type _iPrePower;
	enum item_effect_type _iSufPower;
	int _iVAdd1;
	int _iVMult1;
	int _iVAdd2;
	int _iVMult2;
	int8_t _iMinStr;
	uint8_t _iMinMag;
	int8_t _iMinDex;
	bool _iStatFlag;
	int IDidx;
	uint32_t dwBuff;
	uint32_t _iDamAcFlags;

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

	UiFlags getTextColor() const
	{
		if (!_iStatFlag)
			return UIS_RED;
		switch (_iMagical) {
		case ITEM_QUALITY_MAGIC:
			return UIS_BLUE;
		case ITEM_QUALITY_UNIQUE:
			return UIS_GOLD;
		default:
			return UIS_SILVER;
		}
	}
};

struct ItemGetRecordStruct {
	int32_t nSeed;
	uint16_t wCI;
	int nIndex;
	uint32_t dwTimestamp;
};

struct CornerStoneStruct {
	Point position;
	bool activated;
	ItemStruct item;
};

extern int itemactive[MAXITEMS];
extern bool uitemflag;
extern int itemavail[MAXITEMS];
extern ItemStruct items[MAXITEMS + 1];
extern CornerStoneStruct CornerStone;
extern bool UniqueItemFlags[128];
extern int numitems;

BYTE GetOutlineColor(const ItemStruct &item, bool checkReq);
bool IsItemAvailable(int i);
bool IsUniqueAvailable(int i);
void InitItemGFX();
void InitItems();
void CalcPlrItemVals(int p, bool Loadgfx);
void CalcPlrInv(int p, bool Loadgfx);
void SetPlrHandItem(ItemStruct *h, int idata);
void GetPlrHandSeed(ItemStruct *h);
void GetGoldSeed(int pnum, ItemStruct *h);
int GetGoldCursor(int value);
void SetPlrHandGoldCurs(ItemStruct *h);
void CreatePlrItems(int playerId);
bool ItemSpaceOk(int i, int j);
int AllocateItem();
Point GetSuperItemLoc(Point position);
void GetItemAttrs(int i, int idata, int lvl);
void SaveItemPower(int i, item_effect_type power, int param1, int param2, int minval, int maxval, int multval);
void GetItemPower(int i, int minlvl, int maxlvl, affix_item_type flgs, bool onlygood);
void SetupItem(int i);
int RndItem(int m);
void SpawnUnique(_unique_items uid, int x, int y);
void SpawnItem(int m, Point position, bool sendmsg);
void CreateRndItem(Point position, bool onlygood, bool sendmsg, bool delta);
void CreateRndUseful(Point position, bool sendmsg);
void CreateTypeItem(Point position, bool onlygood, int itype, int imisc, bool sendmsg, bool delta);
void RecreateItem(int ii, int idx, uint16_t icreateinfo, int iseed, int ivalue, bool isHellfire);
void RecreateEar(int ii, uint16_t ic, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, int ibuff);
void items_427A72();
void items_427ABA(Point position);
void SpawnQuestItem(int itemid, Point position, int randarea, int selflag);
void SpawnRock();
void SpawnRewardItem(int itemid, Point position);
void SpawnMapOfDoom(Point position);
void SpawnRuneBomb(Point position);
void SpawnTheodore(Point position);
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
void DrawUniqueInfo(const CelOutputBuffer &out);
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
void RecreateTownItem(int ii, int idx, uint16_t icreateinfo, int iseed);
void RecalcStoreStats();
int ItemNoFlippy();
void CreateSpellBook(Point position, spell_id ispell, bool sendmsg, bool delta);
void CreateMagicArmor(Point position, int imisc, int icurs, bool sendmsg, bool delta);
void CreateAmulet(Point position, int curlv, bool sendmsg, bool delta);
void CreateMagicWeapon(Point position, int imisc, int icurs, bool sendmsg, bool delta);
bool GetItemRecord(int nSeed, uint16_t wCI, int nIndex);
void SetItemRecord(int nSeed, uint16_t wCI, int nIndex);
void PutItemRecord(int nSeed, uint16_t wCI, int nIndex);

/* data */

extern int MaxGold;

extern BYTE ItemCAnimTbl[];
extern _sfx_id ItemInvSnds[];

} // namespace devilution
