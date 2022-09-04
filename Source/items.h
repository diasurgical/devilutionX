/**
 * @file items.h
 *
 * Interface of item functionality.
 */
#pragma once

#include <cstdint>

#include "DiabloUI/ui_flags.hpp"
#include "engine.h"
#include "engine/animationinfo.h"
#include "engine/point.hpp"
#include "itemdat.h"
#include "monster.h"
#include "utils/stdcompat/optional.hpp"
#include "utils/string_or_view.hpp"

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

// All item animation frames have this width.
constexpr int ItemAnimWidth = 96;

// Defined in player.h, forward declared here to allow for functions which operate in the context of a player.
struct Player;

struct Item {
	/** Randomly generated identifier */
	int32_t _iSeed = 0;
	uint16_t _iCreateInfo = 0;
	enum ItemType _itype = ItemType::None;
	Point position = { 0, 0 };
	bool _iAnimFlag = false;
	/*
	 * @brief Contains Information for current Animation
	 */
	AnimationInfo AnimInfo;
	bool _iDelFlag = false; // set when item is flagged for deletion, deprecated in 1.02
	uint8_t _iSelFlag = 0;
	bool _iPostDraw = false;
	bool _iIdentified = false;
	item_quality _iMagical = ITEM_QUALITY_NORMAL;
	char _iName[64] = {};
	char _iIName[64] = {};
	enum item_equip_type _iLoc = ILOC_NONE;
	enum item_class _iClass = ICLASS_NONE;
	uint8_t _iCurs = 0;
	int _ivalue = 0;
	int _iIvalue = 0;
	uint8_t _iMinDam = 0;
	uint8_t _iMaxDam = 0;
	int16_t _iAC = 0;
	ItemSpecialEffect _iFlags = ItemSpecialEffect::None;
	enum item_misc_id _iMiscId = IMISC_NONE;
	enum spell_id _iSpell = SPL_NULL;
	int _iCharges = 0;
	int _iMaxCharges = 0;
	int _iDurability = 0;
	int _iMaxDur = 0;
	int16_t _iPLDam = 0;
	int16_t _iPLToHit = 0;
	int16_t _iPLAC = 0;
	int16_t _iPLStr = 0;
	int16_t _iPLMag = 0;
	int16_t _iPLDex = 0;
	int16_t _iPLVit = 0;
	int16_t _iPLFR = 0;
	int16_t _iPLLR = 0;
	int16_t _iPLMR = 0;
	int16_t _iPLMana = 0;
	int16_t _iPLHP = 0;
	int16_t _iPLDamMod = 0;
	int16_t _iPLGetHit = 0;
	int16_t _iPLLight = 0;
	int8_t _iSplLvlAdd = 0;
	bool _iRequest = false;
	/** Unique item ID, used as an index into UniqueItemList */
	int _iUid = 0;
	int16_t _iFMinDam = 0;
	int16_t _iFMaxDam = 0;
	int16_t _iLMinDam = 0;
	int16_t _iLMaxDam = 0;
	int16_t _iPLEnAc = 0;
	enum item_effect_type _iPrePower = IPL_INVALID;
	enum item_effect_type _iSufPower = IPL_INVALID;
	int _iVAdd1 = 0;
	int _iVMult1 = 0;
	int _iVAdd2 = 0;
	int _iVMult2 = 0;
	int8_t _iMinStr = 0;
	uint8_t _iMinMag = 0;
	int8_t _iMinDex = 0;
	bool _iStatFlag = false;
	_item_indexes IDidx = IDI_NONE;
	uint32_t dwBuff = 0;
	ItemSpecialEffectHf _iDamAcFlags = ItemSpecialEffectHf::None;

	/**
	 * @brief Clears this item and returns the old value
	 */
	Item pop() &
	{
		Item temp = std::move(*this);
		clear();
		return temp;
	}

	/**
	 * @brief Resets the item so isEmpty() returns true without needing to reinitialise the whole object
	 */
	DVL_REINITIALIZES void clear()
	{
		this->_itype = ItemType::None;
	}

	/**
	 * @brief Checks whether this item is empty or not.
	 * @return 'True' in case the item is empty and 'False' otherwise.
	 */
	bool isEmpty() const
	{
		return this->_itype == ItemType::None;
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
		case ItemType::Axe:
		case ItemType::Bow:
		case ItemType::Mace:
		case ItemType::Staff:
		case ItemType::Sword:
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
		case ItemType::HeavyArmor:
		case ItemType::LightArmor:
		case ItemType::MediumArmor:
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
		return !this->isEmpty() && this->_itype == ItemType::Helm;
	}

	/**
	 * @brief Checks whether this item is a shield.
	 * @return 'True' in case the item is a shield and 'False' otherwise.
	 */
	bool isShield() const
	{
		return !this->isEmpty() && this->_itype == ItemType::Shield;
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
		case ItemType::Amulet:
		case ItemType::Ring:
			return true;

		default:
			return false;
		}
	}

	[[nodiscard]] bool isScroll() const
	{
		return IsAnyOf(_iMiscId, IMISC_SCROLL, IMISC_SCROLLT);
	}

	[[nodiscard]] bool isScrollOf(spell_id spellId) const
	{
		return isScroll() && _iSpell == spellId;
	}

	[[nodiscard]] bool isRune() const
	{
		return _iMiscId > IMISC_RUNEFIRST && _iMiscId < IMISC_RUNELAST;
	}

	[[nodiscard]] bool isRuneOf(spell_id spellId) const
	{
		if (!isRune())
			return false;
		switch (_iMiscId) {
		case IMISC_RUNEF:
			return spellId == SPL_RUNEFIRE;
		case IMISC_RUNEL:
			return spellId == SPL_RUNELIGHT;
		case IMISC_GR_RUNEL:
			return spellId == SPL_RUNENOVA;
		case IMISC_GR_RUNEF:
			return spellId == SPL_RUNEIMMOLAT;
		case IMISC_RUNES:
			return spellId == SPL_RUNESTONE;
		default:
			return false;
		}
	}

	[[nodiscard]] bool keyAttributesMatch(int32_t seed, _item_indexes itemIndex, uint16_t createInfo) const
	{
		return _iSeed == seed && IDidx == itemIndex && _iCreateInfo == createInfo;
	}

	UiFlags getTextColor() const
	{
		switch (_iMagical) {
		case ITEM_QUALITY_MAGIC:
			return UiFlags::ColorBlue;
		case ITEM_QUALITY_UNIQUE:
			return UiFlags::ColorWhitegold;
		default:
			return UiFlags::ColorWhite;
		}
	}

	UiFlags getTextColorWithStatCheck() const
	{
		if (!_iStatFlag)
			return UiFlags::ColorRed;
		return getTextColor();
	}

	/**
	 * @brief Sets the current Animation for the Item
	 * @param showAnimation Definies if the Animation (Flipping) is shown or if only the final Frame (item on the ground) is shown
	 */
	void setNewAnimation(bool showAnimation);

	/**
	 * @brief If this item is a spell book, calculates the magic requirement to learn a new level, then for all items sets _iStatFlag
	 * @param player Player to compare stats against requirements
	 */
	void updateRequiredStatsCacheForPlayer(const Player &player);
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
	Item item;
};

/** Contains the items on ground in the current game. */
extern Item Items[MAXITEMS + 1];
extern uint8_t ActiveItems[MAXITEMS];
extern uint8_t ActiveItemCount;
/** Contains the location of dropped items. */
extern int8_t dItem[MAXDUNX][MAXDUNY];
extern bool ShowUniqueItemInfoBox;
extern CornerStoneStruct CornerStone;
extern bool UniqueItemFlags[128];

uint8_t GetOutlineColor(const Item &item, bool checkReq);
bool IsItemAvailable(int i);
bool IsUniqueAvailable(int i);
void InitItemGFX();
void InitItems();
void CalcPlrItemVals(Player &player, bool Loadgfx);
void CalcPlrInv(Player &player, bool Loadgfx);
void InitializeItem(Item &item, int itemData);
void GenerateNewSeed(Item &h);
int GetGoldCursor(int value);

/**
 * @brief Update the gold cursor on the given gold item
 * @param gold The item to update
 */
void SetPlrHandGoldCurs(Item &gold);
void CreatePlrItems(Player &player);
bool ItemSpaceOk(Point position);
int AllocateItem();
Point GetSuperItemLoc(Point position);
void GetItemAttrs(Item &item, int itemData, int lvl);
void SetupItem(Item &item);
int RndItem(const Monster &monster);
void SpawnUnique(_unique_items uid, Point position);
void SpawnItem(Monster &monster, Point position, bool sendmsg);
void CreateRndItem(Point position, bool onlygood, bool sendmsg, bool delta);
void CreateRndUseful(Point position, bool sendmsg);
void CreateTypeItem(Point position, bool onlygood, ItemType itemType, int imisc, bool sendmsg, bool delta);
void RecreateItem(const Player &player, Item &item, int idx, uint16_t icreateinfo, int iseed, int ivalue, bool isHellfire);
void RecreateEar(Item &item, uint16_t ic, int iseed, uint8_t bCursval, string_view heroName);
void CornerstoneSave();
void CornerstoneLoad(Point position);
void SpawnQuestItem(int itemid, Point position, int randarea, int selflag);
void SpawnRewardItem(int itemid, Point position, bool sendmsg);
void SpawnMapOfDoom(Point position, bool sendmsg);
void SpawnRuneBomb(Point position, bool sendmsg);
void SpawnTheodore(Point position, bool sendmsg);
void RespawnItem(Item &item, bool FlipFlag);
void DeleteItem(int i);
void ProcessItems();
void FreeItemGFX();
void GetItemFrm(Item &item);
void GetItemStr(Item &item);
void CheckIdentify(Player &player, int cii);
void DoRepair(Player &player, int cii);
void DoRecharge(Player &player, int cii);
bool DoOil(Player &player, int cii);
[[nodiscard]] StringOrView PrintItemPower(char plidx, const Item &item);
void DrawUniqueInfo(const Surface &out);
void PrintItemDetails(const Item &item);
void PrintItemDur(const Item &item);
void UseItem(size_t pnum, item_misc_id Mid, spell_id spl);
bool UseItemOpensHive(const Item &item, Point position);
bool UseItemOpensCrypt(const Item &item, Point position);
void SpawnSmith(int lvl);
void SpawnPremium(const Player &player);
void SpawnWitch(int lvl);
void SpawnBoy(int lvl);
void SpawnHealer(int lvl);
void MakeGoldStack(Item &goldItem, int value);
int ItemNoFlippy();
void CreateSpellBook(Point position, spell_id ispell, bool sendmsg, bool delta);
void CreateMagicArmor(Point position, ItemType itemType, int icurs, bool sendmsg, bool delta);
void CreateAmulet(Point position, int lvl, bool sendmsg, bool delta);
void CreateMagicWeapon(Point position, ItemType itemType, int icurs, bool sendmsg, bool delta);
bool GetItemRecord(int nSeed, uint16_t wCI, int nIndex);
void SetItemRecord(int nSeed, uint16_t wCI, int nIndex);
void PutItemRecord(int nSeed, uint16_t wCI, int nIndex);

/**
 * @brief Resets item get records.
 */
void initItemGetRecords();

void RepairItem(Item &item, int lvl);
void RechargeItem(Item &item, Player &player);
bool ApplyOilToItem(Item &item, Player &player);

#ifdef _DEBUG
std::string DebugSpawnItem(std::string itemName);
std::string DebugSpawnUniqueItem(std::string itemName);
#endif
/* data */

extern int MaxGold;

extern int8_t ItemCAnimTbl[];
extern _sfx_id ItemInvSnds[];

} // namespace devilution
