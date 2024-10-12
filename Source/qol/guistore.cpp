#include "qol/guistore.h"

#include <algorithm>
#include <cstdint>
#include <utility>

#include <fmt/format.h>

#include "DiabloUI/text_input.hpp"
#include "control.h"
#include "controls/plrctrls.h"
#include "cursor.h"
#include "engine/clx_sprite.hpp"
#include "engine/load_clx.hpp"
#include "engine/points_in_rectangle_range.hpp"
#include "engine/rectangle.hpp"
#include "engine/render/clx_render.hpp"
#include "engine/render/text_render.hpp"
#include "engine/size.hpp"
#include "hwcursor.hpp"
#include "inv.h"
#include "minitext.h"
#include "utils/format_int.hpp"
#include "utils/language.h"
#include "utils/str_cat.hpp"
#include "utils/utf8.hpp"

namespace devilution {

bool IsStoreOpen;
StoreStruct Store;

namespace {

constexpr unsigned CountStorePages = 100;
constexpr unsigned LastStorePage = CountStorePages - 1;

constexpr Size ButtonSize { 27, 16 };
/** Contains mappings for the buttons in the store (2 navigation buttons, withdraw gold buttons, 2 navigation buttons) */
constexpr Rectangle StoreButtonRect[] = {
	// clang-format off
	{ {  19, 19 }, ButtonSize }, // 10 left
	{ {  56, 19 }, ButtonSize }, // 1 left
	{ {  93, 19 }, ButtonSize }, // withdraw gold
	{ { 242, 19 }, ButtonSize }, // 1 right
	{ { 279, 19 }, ButtonSize }  // 10 right
	// clang-format on
};

constexpr Size StoreGridSize { 10, 10 };
constexpr PointsInRectangle<int> StoreGridRange { { { 0, 0 }, StoreGridSize } };

OptionalOwnedClxSpriteList StorePanelArt;
/**
 * @param page The store page index.
 * @param position Position to add the item to.
 * @param storeListIndex The item's StoreList index
 * @param itemSize Size of item
 */
void AddItemToStoreGrid(unsigned page, Point position, uint16_t storeListIndex, Size itemSize)
{
	for (Point point : PointsInRectangle(Rectangle { position, itemSize })) {
		Store.storeGrids[page][point.x][point.y] = storeListIndex + 1;
	}
}

std::optional<Point> FindTargetSlotUnderItemCursor(Point cursorPosition, Size itemSize)
{
	for (auto point : StoreGridRange) {
		Rectangle cell {
			GetStoreSlotCoord(point),
			InventorySlotSizeInPixels + 1
		};

		if (cell.contains(cursorPosition)) {
			// When trying to paste into the store we need to determine the top left cell of the nearest area that could fit the item, not the slot under the center/hot pixel.
			if (itemSize.height <= 1 && itemSize.width <= 1) {
				// top left cell of a 1x1 item is the same cell as the hot pixel, no work to do
				return point;
			}
			// Otherwise work out how far the central cell is from the top-left cell
			Displacement hotPixelCellOffset = { (itemSize.width - 1) / 2, (itemSize.height - 1) / 2 };
			// For even dimension items we need to work out if the cursor is in the left/right (or top/bottom) half of the central cell and adjust the offset so the item lands in the area most covered by the cursor.
			if (itemSize.width % 2 == 0 && cell.contains(cursorPosition + Displacement { INV_SLOT_HALF_SIZE_PX, 0 })) {
				// hot pixel was in the left half of the cell, so we want to increase the offset to preference the column to the left
				hotPixelCellOffset.deltaX++;
			}
			if (itemSize.height % 2 == 0 && cell.contains(cursorPosition + Displacement { 0, INV_SLOT_HALF_SIZE_PX })) {
				// hot pixel was in the top half of the cell, so we want to increase the offset to preference the row above
				hotPixelCellOffset.deltaY++;
			}
			// Then work out the top left cell of the nearest area that could fit this item (as pasting on the edge of the store would otherwise put it out of bounds)
			point.y = std::clamp(point.y - hotPixelCellOffset.deltaY, 0, StoreGridSize.height - itemSize.height);
			point.x = std::clamp(point.x - hotPixelCellOffset.deltaX, 0, StoreGridSize.width - itemSize.width);
			return point;
		}
	}

	return {};
}

// GUISTORE: Borrow from filter function
bool IsItemAllowedInStore(const Item &item)
{
	return item._iMiscId != IMISC_ARENAPOT && IsNoneOf(item._iClass, ICLASS_GOLD, ICLASS_QUEST, ICLASS_NONE);
}

void CheckStorePaste(Point cursorPosition)
{
	Player &player = *MyPlayer;

	if (!IsItemAllowedInStore(player.HoldItem))
		return;

	const Size itemSize = GetInventorySize(player.HoldItem);

	std::optional<Point> targetSlot = FindTargetSlotUnderItemCursor(cursorPosition, itemSize);
	if (!targetSlot)
		return;

	PlaySFX(ItemInvSnds[ItemCAnimTbl[ICURS_GOLD_SMALL]]);

	Item &itemToSell = player.HoldItem;

	int price = GetItemSellValue(itemToSell);

	// Add the gold to the player's inventory
	AddGoldToInventory(*MyPlayer, price);
	MyPlayer->_pGold += price;

	player.HoldItem.clear();

	NewCursor(player.HoldItem);
}

void CheckStoreCut(Point cursorPosition, bool automaticMove)
{
	Player &player = *MyPlayer;

	Point slot = InvalidStorePoint;

	for (auto point : StoreGridRange) {
		Rectangle cell {
			GetStoreSlotCoord(point),
			InventorySlotSizeInPixels + 1
		};

		// check which inventory rectangle the mouse is in, if any
		if (cell.contains(cursorPosition)) {
			slot = point;
			break;
		}
	}

	if (slot == InvalidStorePoint) {
		return;
	}

	Item &holdItem = player.HoldItem;
	holdItem.clear();

	bool automaticallyMoved = false;
	bool automaticallyEquipped = false;

	StoreStruct::StoreCell iv = Store.GetItemIdAtPosition(slot);
	if (iv != StoreStruct::EmptyCell) {
		holdItem = Store.storeList[iv];
		if (automaticMove) {
			if (CanBePlacedOnBelt(player, holdItem)) {
				automaticallyMoved = AutoPlaceItemInBelt(player, holdItem, true, true);
			} else {
				automaticallyMoved = AutoEquip(player, holdItem, true, true);
			}
		}

		if (!automaticMove || automaticallyMoved) {
			Store.RemoveStoreItem(iv);
		}
	}

	if (!holdItem.isEmpty()) {
		CalcPlrInv(player, true);
		holdItem._iStatFlag = player.CanUseItem(holdItem);
		if (automaticallyEquipped) {
			PlaySFX(ItemInvSnds[ItemCAnimTbl[holdItem._iCurs]]);
		} else if (!automaticMove || automaticallyMoved) {
			PlaySFX(SfxID::GrabItem);
		}

		if (automaticMove) {
			if (!automaticallyMoved) {
				if (CanBePlacedOnBelt(player, holdItem)) {
					player.SaySpecific(HeroSpeech::IHaveNoRoom);
				} else {
					player.SaySpecific(HeroSpeech::ICantDoThat);
				}
			}

			holdItem.clear();
		} else {
			NewCursor(holdItem);
		}
	}
}

} // namespace

Point GetStoreSlotCoord(Point slot)
{
	constexpr int StoreNextCell = INV_SLOT_SIZE_PX + 1; // spacing between each cell

	return GetPanelPosition(UiPanels::Store, slot * StoreNextCell + Displacement { 17, 48 });
}

void FreeStoreGFX()
{
	StorePanelArt = std::nullopt;
}

void InitStore()
{
	if (!HeadlessMode) {
		StorePanelArt = LoadClx("data\\store.clx");
	}
}

void GUIBuyItem(Player &player, uint16_t itemId)
{
	if (itemId == StoreStruct::EmptyCell) {
		return;
	}

	Item &item = Store.storeList[itemId];
	if (item.isEmpty()) {
		return;
	}

	if (!AutoPlaceItemInInventory(player, item, true)) {
		player.SaySpecific(HeroSpeech::IHaveNoRoom);
		return;
	}

	PlaySFX(ItemInvSnds[ItemCAnimTbl[item._iCurs]]);

	Store.RemoveStoreItem(itemId);
}

int StoreButtonPressed = -1;

void CheckGUIStoreButtonRelease(Point mousePosition)
{
	if (StoreButtonPressed == -1)
		return;

	Rectangle storeButton = StoreButtonRect[StoreButtonPressed];
	storeButton.position = GetPanelPosition(UiPanels::Store, storeButton.position);
	if (storeButton.contains(mousePosition)) {
		switch (StoreButtonPressed) {
		case 0:
			Store.PreviousPage(10);
			break;
		case 1:
			Store.PreviousPage();
			break;
		case 2:
			break;
		case 3:
			Store.NextPage();
			break;
		case 4:
			Store.NextPage(10);
			break;
		}
	}

	StoreButtonPressed = -1;
}

void CheckGUIStoreButtonPress(Point mousePosition)
{
	Rectangle storeButton;

	for (int i = 0; i < 5; i++) {
		storeButton = StoreButtonRect[i];
		storeButton.position = GetPanelPosition(UiPanels::Store, storeButton.position);
		if (storeButton.contains(mousePosition)) {
			StoreButtonPressed = i;
			return;
		}
	}

	StoreButtonPressed = -1;
}

void DrawGUIStore(const Surface &out)
{
	RenderClxSprite(out, (*StorePanelArt)[0], GetPanelPosition(UiPanels::Store));

	if (StoreButtonPressed != -1) {
		Point storeButton = GetPanelPosition(UiPanels::Store, StoreButtonRect[StoreButtonPressed].position);
		// RenderClxSprite(out, (*StoreNavButtonArt)[StoreButtonPressed], storeButton);
	}

	constexpr Displacement offset { 0, INV_SLOT_SIZE_PX - 1 };

	for (auto slot : StoreGridRange) {
		StoreStruct::StoreCell itemId = Store.GetItemIdAtPosition(slot);
		if (itemId == StoreStruct::EmptyCell) {
			continue; // No item in the given slot
		}
		Item &item = Store.storeList[itemId];
		InvDrawSlotBack(out, GetStoreSlotCoord(slot) + offset, InventorySlotSizeInPixels, item._iMagical);
	}

	for (auto slot : StoreGridRange) {
		StoreStruct::StoreCell itemId = Store.GetItemIdAtPosition(slot);
		if (itemId == StoreStruct::EmptyCell) {
			continue; // No item in the given slot
		}

		Item &item = Store.storeList[itemId];
		if (item.position != slot) {
			continue; // Not the first slot of the item
		}

		int frame = item._iCurs + CURSOR_FIRSTITEM;

		const Point position = GetStoreSlotCoord(item.position) + offset;
		const ClxSprite sprite = GetInvItemSprite(frame);

		if (pcursstashitem == itemId) {
			uint8_t color = GetOutlineColor(item, true);
			ClxDrawOutline(out, color, position, sprite);
		}

		DrawItem(item, out, position, sprite);
	}

	Point position = GetPanelPosition(UiPanels::Store);
	UiFlags style = UiFlags::VerticalCenter | UiFlags::ColorWhite;

	DrawString(out, StrCat(Store.GetPage() + 1), { position + Displacement { 132, 0 }, { 57, 11 } },
	    { .flags = UiFlags::AlignCenter | style });
}

void CheckStoreItem(Point mousePosition, bool isShiftHeld, bool isCtrlHeld)
{
	if (!MyPlayer->HoldItem.isEmpty()) {
		CheckStorePaste(mousePosition);
	} else if (isCtrlHeld) {
		GUIBuyItem(*MyPlayer, pcursstoreitem);
	} else {
		CheckStoreCut(mousePosition, isShiftHeld);
	}
}

uint16_t CheckStoreHLight(Point mousePosition)
{
	Point slot = InvalidStorePoint;
	for (auto point : StoreGridRange) {
		Rectangle cell {
			GetStoreSlotCoord(point),
			InventorySlotSizeInPixels + 1
		};

		if (cell.contains(mousePosition)) {
			slot = point;
			break;
		}
	}

	if (slot == InvalidStorePoint)
		return -1;

	InfoColor = UiFlags::ColorWhite;

	StoreStruct::StoreCell itemId = Store.GetItemIdAtPosition(slot);
	if (itemId == StoreStruct::EmptyCell) {
		return -1;
	}

	Item &item = Store.storeList[itemId];
	if (item.isEmpty()) {
		return -1;
	}

	InfoColor = item.getTextColor();
	InfoString = item.getName();
	if (item._iIdentified) {
		PrintItemDetails(item);
	} else {
		PrintItemDur(item);
	}

	return itemId;
}

void StoreStruct::RemoveStoreItem(StoreStruct::StoreCell iv)
{
	// Iterate through storeGrid and remove every reference to item
	for (auto &row : Store.GetCurrentGrid()) {
		for (StoreStruct::StoreCell &itemId : row) {
			if (itemId - 1 == iv) {
				itemId = 0;
			}
		}
	}

	if (storeList.empty()) {
		return;
	}

	// If the item at the end of store array isn't the one we removed, we need to swap its position in the array with the removed item
	StoreStruct::StoreCell lastItemIndex = static_cast<StoreStruct::StoreCell>(storeList.size() - 1);
	if (lastItemIndex != iv) {
		storeList[iv] = storeList[lastItemIndex];

		for (auto &[_, grid] : Store.storeGrids) {
			for (auto &row : grid) {
				for (StoreStruct::StoreCell &itemId : row) {
					if (itemId == lastItemIndex + 1) {
						itemId = iv + 1;
					}
				}
			}
		}
	}
	storeList.pop_back();
	Store.dirty = true;
}

void StoreStruct::SetPage(unsigned newPage)
{
	page = std::min(newPage, LastStorePage);
	dirty = true;
}

void StoreStruct::NextPage(unsigned offset)
{
	if (page <= LastStorePage) {
		page += std::min(offset, LastStorePage - page);
	} else {
		page = LastStorePage;
	}
	dirty = true;
}

void StoreStruct::PreviousPage(unsigned offset)
{
	if (page <= LastStorePage) {
		page -= std::min(offset, page);
	} else {
		page = LastStorePage;
	}
	dirty = true;
}

void StoreStruct::RefreshItemStatFlags()
{
	for (auto &item : Store.storeList) {
		item.updateRequiredStatsCacheForPlayer(*MyPlayer);
	}
}

bool AutoSellItemToStore(Player &player, const Item &item, bool persistItem)
{
	if (!IsItemAllowedInStore(item))
		return false;

	Size itemSize = GetInventorySize(item);

	// Try to add the item to the current active page and if it's not possible move forward
	for (unsigned pageCounter = 0; pageCounter < CountStorePages; pageCounter++) {
		unsigned pageIndex = Store.GetPage() + pageCounter;
		// Wrap around if needed
		if (pageIndex >= CountStorePages)
			pageIndex -= CountStorePages;
		// Search all possible position in store grid
		for (auto storePosition : PointsInRectangle(Rectangle { { 0, 0 }, Size { 10 - (itemSize.width - 1), 10 - (itemSize.height - 1) } })) {
			// Check that all needed slots are free
			bool isSpaceFree = true;
			for (auto itemPoint : PointsInRectangle(Rectangle { storePosition, itemSize })) {
				uint16_t iv = Store.storeGrids[pageIndex][itemPoint.x][itemPoint.y];
				if (iv != 0) {
					isSpaceFree = false;
					break;
				}
			}
			if (!isSpaceFree)
				continue;
			if (persistItem) {
				Store.storeList.push_back(item);
				uint16_t storeIndex = static_cast<uint16_t>(Store.storeList.size() - 1);
				Store.storeList[storeIndex].position = storePosition + Displacement { 0, itemSize.height - 1 };
				AddItemToStoreGrid(pageIndex, storePosition, storeIndex, itemSize);
				Store.dirty = true;
			}
			return true;
		}
	}

	return false;
}

void PopulateStoreGrid(TalkID talkId)
{
	// Get the current towner store (determined by global TownerId)
	TownerStore *towner = townerStores[TownerId];

	// Clear the store grids to start fresh
	Store.storeGrids.clear();
	Store.storeList.clear();

	// Function to add an item to the store grid for the given page
	auto addItemToGrid = [](unsigned pageIndex, const Item &item) {
		Size itemSize = GetInventorySize(item);

		// Search for a position in the page's grid
		for (auto storePosition : PointsInRectangle(Rectangle { { 0, 0 }, Size { 10 - (itemSize.width - 1), 10 - (itemSize.height - 1) } })) {
			// Check if all needed slots for the item are free
			bool isSpaceFree = true;
			for (auto itemPoint : PointsInRectangle(Rectangle { storePosition, itemSize })) {
				if (Store.storeGrids[pageIndex][itemPoint.x][itemPoint.y] != 0) {
					isSpaceFree = false; // Slot is occupied
					break;
				}
			}

			if (!isSpaceFree)
				continue;

			// Place the item in the grid if space is available
			Store.storeList.push_back(item);
			uint16_t storeIndex = static_cast<uint16_t>(Store.storeList.size() - 1);

			// Set the item's position in the grid
			Store.storeList[storeIndex].position = storePosition + Displacement { 0, itemSize.height - 1 };

			// Mark the grid cells as occupied by this item
			AddItemToStoreGrid(pageIndex, storePosition, storeIndex, itemSize);

			Store.dirty = true;
			return;
		}
	};

	// Determine the item vector to use based on the TalkID
	const std::vector<Item> &itemsToProcess = (talkId == TalkID::BasicBuy) ? towner->basicItems : towner->items;

	// Iterate through the selected item vector and place them in the correct page
	for (const Item &item : itemsToProcess) {
		// Determine the page based on the item class
		unsigned pageIndex;
		switch (item._iClass) {
		case ICLASS_ARMOR:
			pageIndex = 0; // Armor tab
			break;
		case ICLASS_WEAPON:
			pageIndex = 1; // Weapon tab
			break;
		case ICLASS_MISC:
			pageIndex = 2; // Misc tab
			break;
		default:
			continue; // Skip items that don't belong in any of these categories
		}

		// Add the item to the correct grid page
		addItemToGrid(pageIndex, item);
	}
}

} // namespace devilution
