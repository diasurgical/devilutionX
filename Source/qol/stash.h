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
	using StashCell = uint16_t;
	using StashGrid = std::array<std::array<StashCell, 10>, 10>;
	static constexpr StashCell EmptyCell = -1; // uint16_t(-1) or 65535
	std::map<unsigned, StashGrid> stashGrids;  // the stash represented in pages containing a grid of itemIds
	std::vector<Item> stashList;               // list of all items stored in the stash
	int gold;
	bool dirty = false;

	/**
	 * @brief Remove the passed item from the Stash, handling grid cleanup and index updates.
	 * @param item pointer to item being removed.
	 * @param itemPosition position of the item as it may not currently be (ex: autoequipped).
	 * @param itemIndex 0-based index of the item as it exists in the stashList.
	 */
	void RemoveStashItem(Item *item, Point itemPosition, StashCell itemIndex);

	/**
	 * @brief Get the current stash page.
	 * @return page number as unsigned int.
	 */
	unsigned GetPage() const
	{
		return page;
	}

	/**
	 * @brief Get the current grid by reference.
	 * @return current StashGrid reference.
	 */
	StashGrid &GetCurrentGrid()
	{
		return stashGrids[GetPage()];
	}

	/**
	 * @brief Returns the 0-based index of the item at the specified position, or EmptyCell if no item occupies that slot.
	 * @param gridPosition x,y coordinate of the current stash page.
	 * @return a value which can be used to index into stashList or StashStruct::EmptyCell.
	 */
	StashCell GetItemIdAtPosition(Point gridPosition)
	{
		// Because StashCell is an unsigned type we can let this underflow
		return GetCurrentGrid()[gridPosition.x][gridPosition.y] - 1;
	}

	/**
	 * @brief Return status of item existing at specified position (x,y).
	 * @param gridPosition x,y coordinate to check against.
	 * @return whether item exists at provided position.
	 */
	bool IsItemAtPosition(Point gridPosition)
	{
		return GetItemIdAtPosition(gridPosition) != EmptyCell;
	}

	/**
	 * @brief Return the 0-based index of the page on which the provided itemId resides.
	 * @param itemId 0-based index of the item as it exists in the stashList.
	 * @return page number if found or -1.
	 */
	unsigned GetPageByItemId(StashCell itemId);

	/**
	 * @brief Set stash page to the passed page number or last stash page (whichever is lower).
	 * @param newPage page number to set as active stash page.
	 */
	void SetPage(unsigned newPage);

	/**
	 * @brief Set stash to next page(s) or last stash page (whichever is lower).
	 * @param offset number of pages to step forward through.
	 */
	void NextPage(unsigned offset = 1);

	/**
	 * @brief Set stash to previous page(s), stopping at the first page.
	 * @param offset number of pages to step back through.
	 */
	void PreviousPage(unsigned offset = 1);

	/**
	 * @brief Updates _iStatFlag for all stash items.
	 */
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

/**
 * @brief Return screen coordinates of stash cell (x,y).
 * @param slot x,y coordinates of stash cell (as it exists in stash grid).
 * @return x,y screen space coordinates of the provided slot.
 */
Point GetStashSlotCoord(Point slot);

/**
 * @brief Initialize stash, setting gold to 0 and loading panel/button art.
 */
void InitStash();

/**
 * @brief Release stash art assets.
 */
void FreeStashGFX();

/**
 * @brief Transfer item by itemIndex (itemId-1) to player, updating stash grid as well.
 * @param player reference to active player.
 * @param itemIndex 0-based index of the item as it exists in the stashList.
 */
void TransferItemToInventory(Player &player, uint16_t itemIndex);

/**
 * @brief Render the inventory panel to the given buffer.
 */
void DrawStash(const Surface &out);

/**
 * @brief Responds to action against a screen position within the stash panel (ex: mouse click).
 * @param mousePosition x,y screen coordinates of mouse during action.
 * @param isShiftHeld bool for whether shift key is down (auto transfer to belt).
 * @param isCtrlHeld bool for whether ctrl key is down (auto transfer to inventory).
 */
void CheckStashItem(Point mousePosition, bool isShiftHeld = false, bool isCtrlHeld = false);

/**
 * @brief Attempt to use item from stash.
 * @param itemIndex 0-based index of the item as it exists in the stashList.
 * @return 'True' if item used, 'False' if failure to use item.
 */
bool UseStashItem(uint16_t itemIndex);

/**
 * @brief Handle stash highlight and stats pop-up, if an item is under the cursor.
 * @param mousePosition x,y screen coordinates of mouse.
 * @return 0-based index of the item as it exists in the stashList, or InvalidItemId if nothing is under the cursor.
 */
uint16_t CheckStashHLight(Point mousePosition);

/**
 * @brief Perform UI action based on x,y screen position of mouse, responding to StashButtonPressed value.
 * @param mousePosition x,y screen coordinates of mouse at release.
 */
void CheckStashButtonRelease(Point mousePosition);

/**
 * @brief Store x,y screen position of mouse, setting StashButtonPressed for use in CheckStashButtonRelease.
 * @param mousePosition x,y screen coordinates of mouse at press.
 */
void CheckStashButtonPress(Point mousePosition);

/**
 * @brief Being the gold withdraw process, initializing the withdraw dialog popup.
 */
void StartGoldWithdraw();

/**
 * @brief Respond to key press while the gold withdraw process is in progress.
 * @param vkey SDL keycode of the pressed key.
 */
void WithdrawGoldKeyPress(SDL_Keycode vkey);

/**
 * @brief Rendering the gold withdraw popup and the current value.
 * @param out SDL surface upon which to draw the output.
 * @param amount entered withdraw amount to populate in the withdraw popup.
 */
void DrawGoldWithdraw(const Surface &out, int amount);

/**
 * @brief Stop the withdraw process, ending text input, without gold transfer.
 */
void CloseGoldWithdraw();

/**
 * @brief Update gold withdraw amount in gold withdraw popup.
 * @param text Text value of the entered withdraw amount.
 */
void GoldWithdrawNewText(string_view text);

/**
 * @brief Update the StashGrid on the provided page to account for item movement.
 * @param page stashGrid page to act upon.
 * @param startPoint x,y stashGrid position (from the bottom-left cell of the item!).
 * @param itemSize x,y stashGrid size of the item (ex: 2x3).
 * @param itemId item id exactly as it should be stored in the StashGrid.
 */
void UpdateStashGrid(unsigned page, Point startPoint, Size itemSize, StashStruct::StashCell itemId = 0);

/**
 * @brief Checks whether the given item can be placed on the specified player's stash.
 * If 'persistItem' is 'True', the item is also placed in the inventory.
 * @param player The player to check.
 * @param item The item to be checked.
 * @param persistItem Pass 'True' to actually place the item in the inventory. The default is 'False'.
 * @return 'True' in case the item can be placed on the player's inventory and 'False' otherwise.
 */
bool AutoPlaceItemInStash(Player &player, const Item &item, bool persistItem);

} // namespace devilution
