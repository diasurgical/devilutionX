/**
 * @file qol/stash.h
 *
 * Interface of player stash.
 */
#pragma once

#include <cstdint>
#include <map>
#include <vector>

#include "engine/point.hpp"
#include "items.h"

namespace devilution {

class StashStruct {
public:
	void RemoveStashItem(uint16_t iv);
	std::map<unsigned, std::array<std::array<uint16_t, 10>, 10>> stashGrids;
	std::vector<Item> stashList;
	int gold;
	bool dirty = false;

	unsigned GetPage() const
	{
		return page;
	}

	void SetPage(unsigned newPage);
	/** @brief Updates _iStatFlag for all stash items. */
	void RefreshItemStatFlags();

private:
	/** Current Page */
	unsigned page;
};

constexpr Point InvalidStashPoint { -1, -1 };

extern bool IsStashOpen;
extern StashStruct Stash;

extern bool IsWithdrawGoldOpen;
extern int WithdrawGoldValue;

Point GetStashSlotCoord(Point slot);
void InitStash();
void FreeStashGFX();
void TransferItemToInventory(Player &player, uint16_t itemId);
/**
 * @brief Render the inventory panel to the given buffer.
 */
void DrawStash(const Surface &out);
void CheckStashItem(Point mousePosition, bool isShiftHeld = false, bool isCtrlHeld = false);
bool UseStashItem(uint16_t cii);
uint16_t CheckStashHLight(Point mousePosition);
void CheckStashButtonRelease(Point mousePosition);
void CheckStashButtonPress(Point mousePosition);

void StartGoldWithdraw();
void WithdrawGoldKeyPress(char vkey);
void DrawGoldWithdraw(const Surface &out, int amount);
void CloseGoldWithdraw();
void GoldWithdrawNewText(string_view text);

/**
 * @brief Checks whether the given item can be placed on the specified player's stash.
 * If 'persistItem' is 'True', the item is also placed in the inventory.
 * @param item The item to be checked.
 * @param persistItem Pass 'True' to actually place the item in the inventory. The default is 'False'.
 * @return 'True' in case the item can be placed on the player's inventory and 'False' otherwise.
 */
bool AutoPlaceItemInStash(Player &player, const Item &item, bool persistItem);

} // namespace devilution
