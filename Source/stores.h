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
#include "utils/attributes.h"

namespace devilution {

#define WITCH_ITEMS 25
#define SMITH_ITEMS 25
#define SMITH_PREMIUM_ITEMS 15
#define STORE_LINES 104

enum class TalkID : uint8_t {
	None,
	Smith,
	SmithBuy,
	SmithSell,
	SmithRepair,
	Witch,
	WitchBuy,
	WitchSell,
	WitchRecharge,
	NoMoney,
	NoRoom,
	Confirm,
	Boy,
	BoyBuy,
	Healer,
	Storyteller,
	HealerBuy,
	StorytellerIdentify,
	SmithPremiumBuy,
	Gossip,
	StorytellerIdentifyShow,
	Tavern,
	Drunk,
	Barmaid,
};

/** Currently active store */
extern TalkID activeStore;

/** Current index into playerItemIndexes/playerItem */
extern DVL_API_FOR_TEST int currentItemIndex;
/** Map of inventory items being presented in the store */
extern int8_t playerItemIndexes[48];
/** Copies of the players items as presented in the store */
extern DVL_API_FOR_TEST Item playerItem[48];

/** Items sold by Griswold */
extern Item smithItem[SMITH_ITEMS];
/** Number of premium items for sale by Griswold */
extern int numPremiumItems;
/** Base level of current premium items sold by Griswold */
extern int premiumItemLevel;
/** Premium items sold by Griswold */
extern Item premiumItem[SMITH_PREMIUM_ITEMS];

/** Items sold by Pepin */
extern Item healerItem[20];

/** Items sold by Adria */
extern Item witchItem[WITCH_ITEMS];

/** Current level of the item sold by Wirt */
extern int boyItemLevel;
/** Current item sold by Wirt */
extern Item boyItem;

void AddStoreHoldRepair(Item *itm, int8_t i);

/** Clears premium items sold by Griswold and Wirt. */
void InitStores();

/** Spawns items sold by vendors, including premium items sold by Griswold and Wirt. */
void SetupTownStores();

void FreeStoreMem();

void PrintSString(const Surface &out, int margin, int line, std::string_view text, UiFlags flags, int price = 0, int cursId = -1, bool cursIndent = false);
void DrawSLine(const Surface &out, int sy);
void DrawSTextHelp();
void ClearSText(int s, int e);
void StartStore(TalkID s);
void DrawSText(const Surface &out);
void StoreESC();
void StoreUp();
void StoreDown();
void StorePrior();
void StoreNext();
void TakePlrsMoney(int cost);
void StoreEnter();
void CheckStoreBtn();
void ReleaseStoreBtn();

} // namespace devilution
