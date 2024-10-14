/**
 * @file stores.h
 *
 * Interface of functionality for stores and towner dialogs.
 */
#pragma once

#include <cstdint>
#include <optional>

#include "DiabloUI/ui_flags.hpp"
#include "control.h"
#include "engine.h"
#include "engine/clx_sprite.hpp"
#include "engine/random.hpp"
#include "options.h"
#include "qol/stash.h"
#include "towners.h"
#include "utils/attributes.h"

namespace devilution {

/** @brief Number of player items that display in stores (Inventory slots and belt slots) */
const int NumPlayerItems = (NUM_XY_SLOTS - (SLOTXY_EQUIPPED_LAST + 1));

constexpr int NumSmithBasicItems = 19;
constexpr int NumSmithBasicItemsHf = 24;

constexpr int NumSmithItems = 6;
constexpr int NumSmithItemsHf = 15;

constexpr int NumHealerItems = 17;
constexpr int NumHealerItemsHf = 19;
constexpr int NumHealerPinnedItems = 2;
constexpr int NumHealerPinnedItemsMp = 3;

constexpr int NumWitchItems = 17;
constexpr int NumWitchItemsHf = 24;
constexpr int NumWitchPinnedItems = 3;

constexpr int NumBoyItems = 1;

constexpr int NumStoreLines = 104;

extern _talker_id TownerId;

extern Item TempItem; // Temporary item used to hold the item being traded

enum class TalkID : uint8_t {
	Exit,
	MainMenu,
	BasicBuy,
	Buy,
	Sell,
	Repair,
	Recharge,
	Identify,
	IdentifyShow,
	Stash,
	NoMoney,
	NoRoom,
	Confirm,
	Gossip,
	Invalid,
};

enum class ItemLocation {
	Inventory,
	Belt,
	Body
};

struct StoreMenuOption {
	TalkID action;
	std::string text;
};

struct TownerLine {
	const std::string menuHeader;
	const StoreMenuOption *menuOptions;
	size_t numOptions;
};

struct IndexedItem {
	Item *itemPtr;         // Pointer to the original item
	ItemLocation location; // Location in the player's inventory (Inventory, Belt, or Body)
	int index;             // Index in the corresponding array
};

enum class ResourceType {
	Life,
	Mana,
	Invalid,
};

extern TalkID ActiveStore;                                    // Currently active store
extern DVL_API_FOR_TEST std::vector<IndexedItem> playerItems; // Pointers to player items, coupled with necessary information

class TownerStore {
public:
	TownerStore(std::string name, TalkID buyBasic, TalkID buy, TalkID sell, TalkID special, ResourceType resource)
	    : name(name)
	    , buyBasic(buyBasic)
	    , buy(buy)
	    , sell(sell)
	    , special(special)
	    , resourceType(resource)
	{
	}

	std::string name;
	std::vector<Item> basicItems; // Used for the blacksmith store that only displays non-magical items
	std::vector<Item> items;
	uint8_t itemLevel;

	TalkID buyBasic;
	TalkID buy;
	TalkID sell;
	TalkID special;
	ResourceType resourceType; // Resource type to restore for stores that restore player's resources
};

extern TownerStore Blacksmith;
extern TownerStore Healer;
extern TownerStore Witch;
extern TownerStore Boy;
extern TownerStore Storyteller;
extern TownerStore Barmaid;

extern std::unordered_map<_talker_id, TownerStore *> townerStores;

/* Clears premium items sold by Griswold and Wirt. */
void InitStores();
/* Spawns items sold by vendors, including premium items sold by Griswold and Wirt. */
void SetupTownStores();
void FreeStoreMem();
void ExitStore();
void PrintSString(const Surface &out, int margin, int line, std::string_view text, UiFlags flags, int price = 0, int cursId = -1, bool cursIndent = false);
void DrawSLine(const Surface &out, int sy);
void DrawSTextHelp();
void ClearTextLines(int start, int end);
void StartStore(TalkID s = TalkID::MainMenu);
void DrawStore(const Surface &out);
void DrawGUIConfirm(const Surface &out);
void StoreESC();
void StoreUp();
void StoreDown();
void StorePrior();
void StoreNext();
void TakePlrsMoney(int cost);
void StoreEnter();
void CheckStoreButton();
void CheckGUIConfirm();
void ReleaseStoreButton();
bool IsPlayerInStore();
int GetItemBuyValue(const Item &item);
int GetItemSellValue(const Item &item);
int GetItemRepairCost(const Item &item);
int GetItemRechargeCost(const Item &item);
int GetItemIdentifyCost();
bool GiveItemToPlayer(Item &item, bool persistItem);
uint32_t GetTotalPlayerGold();
bool CanPlayerAfford(uint32_t price);

} // namespace devilution
