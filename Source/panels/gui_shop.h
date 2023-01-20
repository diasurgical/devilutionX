/**
 * @file panel/gui_shop.h
 *
 * Interface of GUI shops.
 */
#pragma once

#include <cstdint>
#include <map>
#include <vector>

#include "engine/point.hpp"
#include "items.h"

namespace devilution {

class ShopStruct {
public:
	using ShopCell = uint16_t;
	using ShopGrid = std::array<std::array<ShopCell, 10>, 10>;
	static constexpr ShopCell EmptyCell = -1;

	void RemoveShopItem(StashCell iv);
	std::map<unsigned, ShopGrid> shopGrids;
	std::vector<Item> shopList;
	bool dirty = false;

	ShopGrid &GetCurrentGrid()
	{
		return shopGrids[GetPage()];
	}

	/**
	 * @brief Returns the 0-based index of the item at the specified position, or EmptyCell if no item occupies that slot
	 * @param gridPosition x,y coordinate of the current stash page
	 * @return a value which can be used to index into stashList or StashStruct::EmptyCell
	 */
	ShopCell GetItemIdAtPosition(Point gridPosition)
	{
		// Because ShopCell is an unsigned type we can let this underflow
		return GetCurrentGrid()[gridPosition.x][gridPosition.y] - 1;
	}

	bool IsItemAtPosition(Point gridPosition)
	{
		return GetItemIdAtPosition(gridPosition) != EmptyCell;
	}

	/** @brief Updates _iStatFlag for all shop items. */
	void RefreshItemStatFlags();
};

constexpr Point InvalidShopPoint { -1, -1 };

extern bool IsShopOpen;
extern ShopStruct Shop;

Point GetShopSlotCoord(Point slot);
void InitShop();
void FreeShopGFX();
//void TransferItemToInventory(Player &player, uint16_t itemId);
/**
 * @brief Render the inventory panel to the given buffer.
 */
void DrawShop(const Surface &out);
void CheckShopItem(Point mousePosition, bool isShiftHeld = false, bool isCtrlHeld = false);
uint16_t CheckShopHLight(Point mousePosition);
void CheckStashButtonRelease(Point mousePosition);
void CheckShopButtonPress(Point mousePosition);

} // namespace devilution
