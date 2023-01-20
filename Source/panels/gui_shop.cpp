#include "panels/gui_shop.h"

#include <utility>

#include <fmt/format.h>

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
#include "minitext.h"
#include "miniwin/misc_msg.h"
#include "stores.h"
#include "utils/format_int.hpp"
#include "utils/language.h"
#include "utils/stdcompat/algorithm.hpp"
#include "utils/str_cat.hpp"
#include "utils/utf8.hpp"

namespace devilution {

bool IsShopOpen;
ShopStruct Shop;

namespace {

constexpr Size ButtonSize { 27, 16 };
/** Contains mappings for the buttons in the shop (2 navigation buttons, withdraw gold buttons, 2 navigation buttons) */
constexpr Rectangle ShopButtonRect[] = {
	// clang-format off
	{ {  19, 19 }, ButtonSize }, // 10 left
	{ {  56, 19 }, ButtonSize }, // 1 left
	{ {  93, 19 }, ButtonSize }, // withdraw gold
	{ { 242, 19 }, ButtonSize }, // 1 right
	{ { 279, 19 }, ButtonSize }  // 10 right
	// clang-format on
};

constexpr PointsInRectangleRange<int> ShopGridRange { { { 0, 0 }, Size { 10, 10 } } };

OptionalOwnedClxSpriteList ShopPanelArt;
OptionalOwnedClxSpriteList ShopNavButtonArt;


Point FindSlotUnderCursor(Point cursorPosition)
{
	for (auto point : ShopGridRange) {
		Rectangle cell {
			GetShopSlotCoord(point),
			InventorySlotSizeInPixels + 1
		};

		if (cell.contains(cursorPosition)) {
			return point;
		}
	}

	return InvalidShopPoint;
}

void CheckShopCut(Point cursorPosition, bool automaticMove)
{
	Player &player = *MyPlayer;

	if (IsWithdrawGoldOpen) {
		IsWithdrawGoldOpen = false;
		WithdrawGoldValue = 0;
	}

	Point slot = InvalidShopPoint;

	for (auto point : ShopGridRange) {
		Rectangle cell {
			GetShopSlotCoord(point),
			InventorySlotSizeInPixels + 1
		};

		// check which inventory rectangle the mouse is in, if any
		if (cell.contains(cursorPosition)) {
			slot = point;
			break;
		}
	}

	if (slot == InvalidShopPoint) {
		return;
	}

	Item &holdItem = player.HoldItem;
	holdItem.clear();

	bool automaticallyMoved = false;
	bool automaticallyEquipped = false;

	ShopStruct::ShopCell iv = Shop.GetItemIdAtPosition(slot);
	if (iv != ShopStruct::EmptyCell) {
		holdItem = Shop.shopList[iv];
		if (automaticMove) {
			if (CanBePlacedOnBelt(holdItem)) {
				automaticallyMoved = AutoPlaceItemInBelt(player, holdItem, true);
			} else {
				automaticallyMoved = automaticallyEquipped = AutoEquip(player, holdItem);
			}
		}

		if (!automaticMove || automaticallyMoved) {
			Shop.RemoveShopItem(iv);
		}
	}

	if (!holdItem.isEmpty()) {
		CalcPlrInv(player, true);
		holdItem._iStatFlag = player.CanUseItem(holdItem);
		if (automaticallyEquipped) {
			PlaySFX(ItemInvSnds[ItemCAnimTbl[holdItem._iCurs]]);
		} else if (!automaticMove || automaticallyMoved) {
			PlaySFX(IS_IGRAB);
		}

		if (automaticMove) {
			if (!automaticallyMoved) {
				if (CanBePlacedOnBelt(holdItem)) {
					player.SaySpecific(HeroSpeech::IHaveNoRoom);
				} else {
					player.SaySpecific(HeroSpeech::ICantDoThat);
				}
			}

			holdItem.clear();
		} else {
			NewCursor(holdItem);
			if (!IsHardwareCursor()) {
				// For a hardware cursor, we set the "hot point" to the center of the item instead.
				Size cursSize = GetInvItemSize(holdItem._iCurs + CURSOR_FIRSTITEM);
				SetCursorPos(cursorPosition - Displacement(cursSize / 2));
			}
		}
	}
}

} // namespace

Point GetShopSlotCoord(Point slot)
{
	constexpr int ShopNextCell = INV_SLOT_SIZE_PX + 1; // spacing between each cell

	return GetPanelPosition(UiPanels::Shop, slot * ShopNextCell + Displacement { 17, 48 });
}

void FreeShopGFX()
{
	ShopNavButtonArt = std::nullopt;
	ShopPanelArt = std::nullopt;
}

void InitShop()
{
	InitialWithdrawGoldValue = 0;

	if (!HeadlessMode) {
		ShopPanelArt = LoadClx("data\\stash.clx");
		ShopNavButtonArt = LoadClx("data\\stashnavbtns.clx");
	}
}

void TransferItemToInventory(Player &player, uint16_t itemId)
{
	if (itemId == uint16_t(-1)) {
		return;
	}

	Item &item = Shop.shopList[itemId];
	if (item.isEmpty()) {
		return;
	}

	if (!AutoPlaceItemInInventory(player, item, true)) {
		player.SaySpecific(HeroSpeech::IHaveNoRoom);
		return;
	}

	PlaySFX(ItemInvSnds[ItemCAnimTbl[item._iCurs]]);

	Shop.RemoveShopItem(itemId);
}

int ShopButtonPressed = -1;

void CheckShopButtonRelease(Point mousePosition)
{
	if (ShopButtonPressed == -1)
		return;

	Rectangle shopButton = ShopButtonRect[ShopButtonPressed];
	shopButton.position = GetPanelPosition(UiPanels::Shop, shopButton.position);
	if (shopButton.contains(mousePosition)) {
		switch (ShopButtonPressed) {
		case 0:
			Shop.PreviousPage(10);
			break;
		case 1:
			Shop.PreviousPage();
			break;
		case 2:
			break;
		case 3:
			Shop.NextPage();
			break;
		case 4:
			Shop.NextPage(10);
			break;
		}
	}

	ShopButtonPressed = -1;
}

void CheckShopButtonPress(Point mousePosition)
{
	Rectangle shopButton;

	for (int i = 0; i < 5; i++) {
		shopButton = ShopButtonRect[i];
		shopButton.position = GetPanelPosition(UiPanels::Shop, shopButton.position);
		if (shopButton.contains(mousePosition)) {
			ShopButtonPressed = i;
			return;
		}
	}

	ShopButtonPressed = -1;
}

void DrawShop(const Surface &out)
{
	RenderClxSprite(out, (*ShopPanelArt)[0], GetPanelPosition(UiPanels::Shop));

	if (ShopButtonPressed != -1) {
		Point shopButton = GetPanelPosition(UiPanels::Shop, ShopButtonRect[ShopButtonPressed].position);
		RenderClxSprite(out, (*ShopNavButtonArt)[ShopButtonPressed], shopButton);
	}

	constexpr Displacement offset { 0, INV_SLOT_SIZE_PX - 1 };

	for (auto slot : ShopGridRange) {
		ShopStruct::ShopCell itemId = Shop.GetItemIdAtPosition(slot);
		Item &item = Shop.shopList[itemId];
		if (Shop.IsItemAtPosition(slot)) {
			InvDrawSlotBack(out, GetShopSlotCoord(slot) + offset, InventorySlotSizeInPixels, item._iMagical);
		}
	}

	for (auto slot : ShopGridRange) {
		ShopStruct::ShopCell itemId = Shop.GetItemIdAtPosition(slot);
		if (itemId == ShopStruct::EmptyCell) {
			continue; // No item in the given slot
		}

		Item &item = Shop.shopList[itemId];
		if (item.position != slot) {
			continue; // Not the first slot of the item
		}

		int frame = item._iCurs + CURSOR_FIRSTITEM;

		const Point position = GetShopSlotCoord(item.position) + offset;
		const ClxSprite sprite = GetInvItemSprite(frame);

		if (pcursshopitem == itemId) {
			uint8_t color = GetOutlineColor(item, true);
			ClxDrawOutline(out, color, position, sprite);
		}

		DrawItem(item, out, position, sprite);
	}

	Point position = GetPanelPosition(UiPanels::Shop);
	UiFlags style = UiFlags::VerticalCenter | UiFlags::ColorWhite;

	DrawString(out, StrCat(Shop.GetPage() + 1), { position + Displacement { 132, 0 }, { 57, 11 } }, UiFlags::AlignCenter | style);
	DrawString(out, FormatInteger(Shop.gold), { position + Displacement { 122, 19 }, { 107, 13 } }, UiFlags::AlignRight | style);
}

void CheckShopItem(Point mousePosition, bool isShiftHeld, bool isCtrlHeld)
{
	if (isCtrlHeld) {
		TransferItemToInventory(*MyPlayer, pcursshopitem);
	} else {
		CheckShopCut(mousePosition, isShiftHeld);
	}
}

uint16_t CheckShopHLight(Point mousePosition)
{
	Point slot = InvalidShopPoint;
	for (auto point : ShopGridRange) {
		Rectangle cell {
			GetShopSlotCoord(point),
			InventorySlotSizeInPixels + 1
		};

		if (cell.contains(mousePosition)) {
			slot = point;
			break;
		}
	}

	if (slot == InvalidShopPoint)
		return -1;

	InfoColor = UiFlags::ColorWhite;

	ShopStruct::ShopCell itemId = Shop.GetItemIdAtPosition(slot);
	if (itemId == ShopStruct::EmptyCell) {
		return -1;
	}

	Item &item = Shop.shopList[itemId];
	if (item.isEmpty()) {
		return -1;
	}

	InfoColor = item.getTextColor();
	if (item._iIdentified) {
		InfoString = string_view(item._iIName);
		PrintItemDetails(item);
	} else {
		InfoString = string_view(item._iName);
		PrintItemDur(item);
	}

	return itemId;
}

void ShopStruct::RemoveShopItem(ShopStruct::ShopCell iv)
{
	// Iterate through shopGrid and remove every reference to item
	for (auto &row : Shop.GetCurrentGrid()) {
		for (ShopStruct::ShopCell &itemId : row) {
			if (itemId - 1 == iv) {
				itemId = 0;
			}
		}
	}

	if (shopList.empty()) {
		return;
	}

	// If the item at the end of shop array isn't the one we removed, we need to swap its position in the array with the removed item
	ShopStruct::ShopCell lastItemIndex = static_cast<ShopStruct::ShopCell>(shopList.size() - 1);
	if (lastItemIndex != iv) {
		shopList[iv] = shopList[lastItemIndex];

		for (auto &pair : Shop.shopGrids) {
			auto &grid = pair.second;
			for (auto &row : grid) {
				for (ShopStruct::ShopCell &itemId : row) {
					if (itemId == lastItemIndex + 1) {
						itemId = iv + 1;
					}
				}
			}
		}
	}
	shopList.pop_back();
	Shop.dirty = true;
}

void ShopStruct::RefreshItemStatFlags()
{
	for (auto &item : Shop.shopList) {
		item.updateRequiredStatsCacheForPlayer(*MyPlayer);
	}
}

bool AutoPlaceItemInShop(Player &player, const Item &item, bool persistItem)
{
	if (item._itype == ItemType::Gold) {
		if (persistItem) {
			Shop.gold += item._ivalue;
			Shop.dirty = true;
		}
		return true;
	}

	Size itemSize = GetInventorySize(item);

	// Try to add the item to the current active page and if it's not possible move forward
	for (unsigned pageCounter = 0; pageCounter < CountShopPages; pageCounter++) {
		unsigned pageIndex = Shop.GetPage() + pageCounter;
		// Wrap around if needed
		if (pageIndex >= CountShopPages)
			pageIndex -= CountShopPages;
		// Search all possible position in shop grid
		for (auto shopPosition : PointsInRectangle(Rectangle { { 0, 0 }, Size { 10 - (itemSize.width - 1), 10 - (itemSize.height - 1) } })) {
			// Check that all needed slots are free
			bool isSpaceFree = true;
			for (auto itemPoint : PointsInRectangle(Rectangle { shopPosition, itemSize })) {
				uint16_t iv = Shop.shopGrids[pageIndex][itemPoint.x][itemPoint.y];
				if (iv != 0) {
					isSpaceFree = false;
					break;
				}
			}
			if (!isSpaceFree)
				continue;
			if (persistItem) {
				Shop.shopList.push_back(item);
				uint16_t shopIndex = static_cast<uint16_t>(Shop.shopList.size() - 1);
				Shop.shopList[shopIndex].position = shopPosition + Displacement { 0, itemSize.height - 1 };
				AddItemToShopGrid(pageIndex, shopPosition, shopIndex, itemSize);
				Shop.dirty = true;
			}
			return true;
		}
	}

	return false;
}

} // namespace devilution
