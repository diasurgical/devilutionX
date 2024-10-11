/**
 * @file qol/guistore.h
 *
 * Interface of the GUI Store.
 */
#pragma once

#include <cstdint>
#include <vector>

#include <ankerl/unordered_dense.h>

#include "engine/point.hpp"
#include "items.h"

namespace devilution {

class StoreStruct {
public:
	using StoreCell = uint16_t;
	using StoreGrid = std::array<std::array<StoreCell, 10>, 10>;
	static constexpr StoreCell EmptyCell = -1;

	void RemoveStoreItem(StoreCell iv);
	ankerl::unordered_dense::map<unsigned, StoreGrid> storeGrids;
	std::vector<Item> storeList;
	int gold;
	bool dirty = false;

	unsigned GetPage() const
	{
		return page;
	}

	StoreGrid &GetCurrentGrid()
	{
		return storeGrids[GetPage()];
	}

	/**
	 * @brief Returns the 0-based index of the item at the specified position, or EmptyCell if no item occupies that slot
	 * @param gridPosition x,y coordinate of the current store page
	 * @return a value which can be used to index into storeList or StoreStruct::EmptyCell
	 */
	StoreCell GetItemIdAtPosition(Point gridPosition)
	{
		// Because StoreCell is an unsigned type we can let this underflow
		return GetCurrentGrid()[gridPosition.x][gridPosition.y] - 1;
	}

	bool IsItemAtPosition(Point gridPosition)
	{
		return GetItemIdAtPosition(gridPosition) != EmptyCell;
	}

	void SetPage(unsigned newPage);
	void NextPage(unsigned offset = 1);
	void PreviousPage(unsigned offset = 1);

	/** @brief Updates _iStatFlag for all store items. */
	void RefreshItemStatFlags();

private:
	/** Current Page */
	unsigned page;
};

constexpr Point InvalidStorePoint { -1, -1 };

extern bool IsStoreOpen;
extern StoreStruct Store;

extern bool IsWithdrawGoldOpen;
extern int WithdrawGoldValue;

Point GetStoreSlotCoord(Point slot);
void InitStore();
void FreeStoreGFX();
void TransferItemToInventory(Player &player, uint16_t itemId);
/**
 * @brief Render the inventory panel to the given buffer.
 */
void DrawStore(const Surface &out);
void CheckStoreItem(Point mousePosition, bool isShiftHeld = false, bool isCtrlHeld = false);
bool UseStoreItem(uint16_t cii);
uint16_t CheckStoreHLight(Point mousePosition);
void CheckStoreButtonRelease(Point mousePosition);
void CheckStoreButtonPress(Point mousePosition);

void StartGoldWithdraw();
void WithdrawGoldKeyPress(SDL_Keycode vkey);
void DrawGoldWithdraw(const Surface &out);
void CloseGoldWithdraw();
bool HandleGoldWithdrawTextInputEvent(const SDL_Event &event);

/**
 * @brief Checks whether the given item can be placed on the specified player's store.
 * If 'persistItem' is 'True', the item is also placed in the inventory.
 * @param player The player to check.
 * @param item The item to be checked.
 * @param persistItem Pass 'True' to actually place the item in the inventory. The default is 'False'.
 * @return 'True' in case the item can be placed on the player's inventory and 'False' otherwise.
 */
bool AutoPlaceItemInStore(Player &player, const Item &item, bool persistItem);

} // namespace devilution
