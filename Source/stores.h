/**
 * @file stores.h
 *
 * Interface of functionality for stores and towner dialogs.
 */
#pragma once

#include "control.h"
#include "engine.h"
#include "engine/cel_sprite.hpp"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

#define WITCH_ITEMS 25
#define SMITH_ITEMS 25
#define SMITH_PREMIUM_ITEMS 15
#define STORE_LINES 104

enum talk_id : uint8_t {
	STORE_NONE,
	STORE_SMITH,
	STORE_SBUY,
	STORE_SSELL,
	STORE_SREPAIR,
	STORE_WITCH,
	STORE_WBUY,
	STORE_WSELL,
	STORE_WRECHARGE,
	STORE_NOMONEY,
	STORE_NOROOM,
	STORE_CONFIRM,
	STORE_BOY,
	STORE_BBOY,
	STORE_HEALER,
	STORE_STORY,
	STORE_HBUY,
	STORE_SIDENTIFY,
	STORE_SPBUY,
	STORE_GOSSIP,
	STORE_IDSHOW,
	STORE_TAVERN,
	STORE_DRUNK,
	STORE_BARMAID,
};

struct STextStruct {
	int _sx;
	int _syoff;
	StringVariant _sstr;
	UiFlags flags;
	int _sline;
	bool _ssel;
	int _sval;
};

/** Shop frame graphics */
extern std::optional<CelSprite> pSTextBoxCels;
/** Small text selection cursor */
extern std::optional<CelSprite> pSPentSpn2Cels;
/** Scrollbar graphics */
extern std::optional<CelSprite> pSTextSlidCels;

/** Currently active store */
extern talk_id stextflag;

/** Current index into storehidx/storehold */
extern int storenumh;
/** Map of inventory items being presented in the store */
extern char storehidx[48];
/** Copies of the players items as presented in the store */
extern ItemStruct storehold[48];

/** Temporary item used to generate gold piles by various function */
extern ItemStruct golditem;

/** Items sold by Griswold */
extern ItemStruct smithitem[SMITH_ITEMS];
/** Number of premium items for sale by Griswold */
extern int numpremium;
/** Base level of current premium items sold by Griswold */
extern int premiumlevel;
/** Premium items sold by Griswold */
extern ItemStruct premiumitems[SMITH_PREMIUM_ITEMS];

/** Items sold by Pepin */
extern ItemStruct healitem[20];

/** Items sold by Adria */
extern ItemStruct witchitem[WITCH_ITEMS];

/** Current level of the item sold by Wirt */
extern int boylevel;
/** Current item sold by Wirt */
extern ItemStruct boyitem;

void AddStoreHoldRepair(ItemStruct *itm, int8_t i);
void InitStores();
void SetupTownStores();
void FreeStoreMem();
void PrintSString(const Surface &out, int margin, int line, string_view text, UiFlags flags, int price = 0);
void DrawSLine(const Surface &out, int y);
void DrawSTextHelp();
void ClearSText(int s, int e);
void StartStore(talk_id s);
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
