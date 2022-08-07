#include "qol/stash.h"

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

bool IsStashOpen;
StashStruct Stash;
bool IsWithdrawGoldOpen;
int WithdrawGoldValue;

namespace {

constexpr unsigned CountStashPages = 100;
constexpr unsigned LastStashPage = CountStashPages - 1;

int InitialWithdrawGoldValue;

constexpr Size ButtonSize { 27, 16 };
/** Contains mappings for the buttons in the stash (2 navigation buttons, withdraw gold buttons, 2 navigation buttons) */
constexpr Rectangle StashButtonRect[] = {
	// clang-format off
	{ {  19, 19 }, ButtonSize }, // 10 left
	{ {  56, 19 }, ButtonSize }, // 1 left
	{ {  93, 19 }, ButtonSize }, // withdraw gold
	{ { 242, 19 }, ButtonSize }, // 1 right
	{ { 279, 19 }, ButtonSize }  // 10 right
	// clang-format on
};

constexpr PointsInRectangleRange StashGridRange { { { 0, 0 }, Size { 10, 10 } } };

OptionalOwnedClxSpriteList StashPanelArt;
OptionalOwnedClxSpriteList StashNavButtonArt;

/**
 * @param page The stash page index.
 * @param position Position to add the item to.
 * @param stashListIndex The item's StashList index
 * @param itemSize Size of item
 */
void AddItemToStashGrid(unsigned page, Point position, uint16_t stashListIndex, Size itemSize)
{
	for (auto point : PointsInRectangleRange({ position, itemSize })) {
		Stash.stashGrids[page][point.x][point.y] = stashListIndex + 1;
	}
}

Point FindSlotUnderCursor(Point cursorPosition)
{
	for (auto point : StashGridRange) {
		Rectangle cell {
			GetStashSlotCoord(point),
			InventorySlotSizeInPixels + 1
		};

		if (cell.contains(cursorPosition)) {
			return point;
		}
	}

	return InvalidStashPoint;
}

void CheckStashPaste(Point cursorPosition)
{
	Player &player = *MyPlayer;

	const Size itemSize = GetInventorySize(player.HoldItem);
	const Displacement hotPixelOffset = Displacement(itemSize * INV_SLOT_HALF_SIZE_PX);
	if (IsHardwareCursor()) {
		// It's more natural to select the top left cell of the region the sprite is overlapping when putting an item
		//  into an inventory grid, so compensate for the adjusted hot pixel of hardware cursors.
		cursorPosition -= hotPixelOffset;
	}

	if (player.HoldItem._itype == ItemType::Gold) {
		Stash.gold += player.HoldItem._ivalue;
		player.HoldItem.clear();
		PlaySFX(IS_GOLD);
		Stash.dirty = true;
		if (!IsHardwareCursor()) {
			// To make software cursors behave like hardware cursors we need to adjust the hand cursor position manually
			SetCursorPos(cursorPosition + hotPixelOffset);
		}
		NewCursor(CURSOR_HAND);
		return;
	}

	// Make the hot pixel the center of the top-left cell of the item, this favors the cell which contains more of the
	//  item sprite
	Point firstSlot = FindSlotUnderCursor(cursorPosition + Displacement(INV_SLOT_HALF_SIZE_PX));
	if (firstSlot == InvalidStashPoint)
		return;

	if (firstSlot.x + itemSize.width > 10 || firstSlot.y + itemSize.height > 10) {
		return; // Item does not fit
	}

	// Check that no more than 1 item is replaced by the move
	StashStruct::StashCell stashIndex = StashStruct::EmptyCell;
	for (auto point : PointsInRectangleRange({ firstSlot, itemSize })) {
		StashStruct::StashCell iv = Stash.GetItemIdAtPosition(point);
		if (iv == StashStruct::EmptyCell || stashIndex == iv)
			continue;
		if (stashIndex == StashStruct::EmptyCell) {
			stashIndex = iv; // Found first item
			continue;
		}
		return; // Found a second item
	}

	PlaySFX(ItemInvSnds[ItemCAnimTbl[player.HoldItem._iCurs]]);

	player.HoldItem.position = firstSlot + Displacement { 0, itemSize.height - 1 };

	if (stashIndex == StashStruct::EmptyCell) {
		Stash.stashList.emplace_back(player.HoldItem.pop());
		// stashList will have at most 10 000 items, up to 65 535 are supported with uint16_t indexes
		stashIndex = static_cast<uint16_t>(Stash.stashList.size() - 1);
	} else {
		// remove item from stash grid
		std::swap(Stash.stashList[stashIndex], player.HoldItem);
		for (auto &row : Stash.GetCurrentGrid()) {
			for (auto &itemId : row) {
				if (itemId - 1 == stashIndex)
					itemId = 0;
			}
		}
	}

	AddItemToStashGrid(Stash.GetPage(), firstSlot, stashIndex, itemSize);

	Stash.dirty = true;

	if (player.HoldItem.isEmpty() && !IsHardwareCursor()) {
		// To make software cursors behave like hardware cursors we need to adjust the hand cursor position manually
		SetCursorPos(cursorPosition + hotPixelOffset);
	}
	NewCursor(player.HoldItem);
}

void CheckStashCut(Point cursorPosition, bool automaticMove)
{
	Player &player = *MyPlayer;

	if (IsWithdrawGoldOpen) {
		IsWithdrawGoldOpen = false;
		WithdrawGoldValue = 0;
	}

	Point slot = InvalidStashPoint;

	for (auto point : StashGridRange) {
		Rectangle cell {
			GetStashSlotCoord(point),
			InventorySlotSizeInPixels + 1
		};

		// check which inventory rectangle the mouse is in, if any
		if (cell.contains(cursorPosition)) {
			slot = point;
			break;
		}
	}

	if (slot == InvalidStashPoint) {
		return;
	}

	Item &holdItem = player.HoldItem;
	holdItem.clear();

	bool automaticallyMoved = false;
	bool automaticallyEquipped = false;

	StashStruct::StashCell iv = Stash.GetItemIdAtPosition(slot);
	if (iv != StashStruct::EmptyCell) {
		holdItem = Stash.stashList[iv];
		if (automaticMove) {
			if (CanBePlacedOnBelt(holdItem)) {
				automaticallyMoved = AutoPlaceItemInBelt(player, holdItem, true);
			} else {
				automaticallyMoved = automaticallyEquipped = AutoEquip(player, holdItem);
			}
		}

		if (!automaticMove || automaticallyMoved) {
			Stash.RemoveStashItem(iv);
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

void WithdrawGold(Player &player, int amount)
{
	AddGoldToInventory(player, amount);
	Stash.gold -= amount;
	Stash.dirty = true;
}

} // namespace

Point GetStashSlotCoord(Point slot)
{
	constexpr int StashNextCell = INV_SLOT_SIZE_PX + 1; // spacing between each cell

	return GetPanelPosition(UiPanels::Stash, slot * StashNextCell + Displacement { 17, 48 });
}

void FreeStashGFX()
{
	StashNavButtonArt = std::nullopt;
	StashPanelArt = std::nullopt;
}

void InitStash()
{
	InitialWithdrawGoldValue = 0;

	StashPanelArt = LoadClx("data\\stash.clx");
	StashNavButtonArt = LoadClx("data\\stashnavbtns.clx");
}

void TransferItemToInventory(Player &player, uint16_t itemId)
{
	if (itemId == uint16_t(-1)) {
		return;
	}

	Item &item = Stash.stashList[itemId];
	if (item.isEmpty()) {
		return;
	}

	if (!AutoPlaceItemInInventory(player, item, true)) {
		player.SaySpecific(HeroSpeech::IHaveNoRoom);
		return;
	}

	PlaySFX(ItemInvSnds[ItemCAnimTbl[item._iCurs]]);

	Stash.RemoveStashItem(itemId);
}

int StashButtonPressed = -1;

void CheckStashButtonRelease(Point mousePosition)
{
	if (StashButtonPressed == -1)
		return;

	Rectangle stashButton = StashButtonRect[StashButtonPressed];
	stashButton.position = GetPanelPosition(UiPanels::Stash, stashButton.position);
	if (stashButton.contains(mousePosition)) {
		switch (StashButtonPressed) {
		case 0:
			Stash.PreviousPage(10);
			break;
		case 1:
			Stash.PreviousPage();
			break;
		case 2:
			StartGoldWithdraw();
			break;
		case 3:
			Stash.NextPage();
			break;
		case 4:
			Stash.NextPage(10);
			break;
		}
	}

	StashButtonPressed = -1;
}

void CheckStashButtonPress(Point mousePosition)
{
	Rectangle stashButton;

	for (int i = 0; i < 5; i++) {
		stashButton = StashButtonRect[i];
		stashButton.position = GetPanelPosition(UiPanels::Stash, stashButton.position);
		if (stashButton.contains(mousePosition)) {
			StashButtonPressed = i;
			return;
		}
	}

	StashButtonPressed = -1;
}

void DrawStash(const Surface &out)
{
	RenderClxSprite(out, (*StashPanelArt)[0], GetPanelPosition(UiPanels::Stash));

	if (StashButtonPressed != -1) {
		Point stashButton = GetPanelPosition(UiPanels::Stash, StashButtonRect[StashButtonPressed].position);
		RenderClxSprite(out, (*StashNavButtonArt)[StashButtonPressed], stashButton);
	}

	constexpr Displacement offset { 0, INV_SLOT_SIZE_PX - 1 };

	for (auto slot : StashGridRange) {
		if (Stash.IsItemAtPosition(slot)) {
			InvDrawSlotBack(out, GetStashSlotCoord(slot) + offset, InventorySlotSizeInPixels);
		}
	}

	for (auto slot : StashGridRange) {
		StashStruct::StashCell itemId = Stash.GetItemIdAtPosition(slot);
		if (itemId == StashStruct::EmptyCell) {
			continue; // No item in the given slot
		}

		Item &item = Stash.stashList[itemId];
		if (item.position != slot) {
			continue; // Not the first slot of the item
		}

		int frame = item._iCurs + CURSOR_FIRSTITEM;

		const Point position = GetStashSlotCoord(item.position) + offset;
		const ClxSprite sprite = GetInvItemSprite(frame);

		if (pcursstashitem == itemId) {
			uint8_t color = GetOutlineColor(item, true);
			ClxDrawOutline(out, color, position, sprite);
		}

		DrawItem(item, out, position, sprite);
	}

	Point position = GetPanelPosition(UiPanels::Stash);
	UiFlags style = UiFlags::VerticalCenter | UiFlags::ColorWhite;

	DrawString(out, StrCat(Stash.GetPage() + 1), { position + Displacement { 132, 0 }, { 57, 11 } }, UiFlags::AlignCenter | style);
	DrawString(out, FormatInteger(Stash.gold), { position + Displacement { 122, 19 }, { 107, 13 } }, UiFlags::AlignRight | style);
}

void CheckStashItem(Point mousePosition, bool isShiftHeld, bool isCtrlHeld)
{
	if (!MyPlayer->HoldItem.isEmpty()) {
		CheckStashPaste(mousePosition);
	} else if (isCtrlHeld) {
		TransferItemToInventory(*MyPlayer, pcursstashitem);
	} else {
		CheckStashCut(mousePosition, isShiftHeld);
	}
}

uint16_t CheckStashHLight(Point mousePosition)
{
	Point slot = InvalidStashPoint;
	for (auto point : StashGridRange) {
		Rectangle cell {
			GetStashSlotCoord(point),
			InventorySlotSizeInPixels + 1
		};

		if (cell.contains(mousePosition)) {
			slot = point;
			break;
		}
	}

	if (slot == InvalidStashPoint)
		return -1;

	InfoColor = UiFlags::ColorWhite;

	ClearPanel();

	StashStruct::StashCell itemId = Stash.GetItemIdAtPosition(slot);
	if (itemId == StashStruct::EmptyCell) {
		return -1;
	}

	Item &item = Stash.stashList[itemId];
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

bool UseStashItem(uint16_t c)
{
	if (MyPlayer->_pInvincible && MyPlayer->_pHitPoints == 0)
		return true;
	if (pcurs != CURSOR_HAND)
		return true;
	if (stextflag != STORE_NONE)
		return true;

	Item *item = &Stash.stashList[c];

	constexpr int SpeechDelay = 10;
	if (item->IDidx == IDI_MUSHROOM) {
		MyPlayer->Say(HeroSpeech::NowThatsOneBigMushroom, SpeechDelay);
		return true;
	}
	if (item->IDidx == IDI_FUNGALTM) {
		PlaySFX(IS_IBOOK);
		MyPlayer->Say(HeroSpeech::ThatDidntDoAnything, SpeechDelay);
		return true;
	}

	if (!AllItemsList[item->IDidx].iUsable)
		return false;

	if (!MyPlayer->CanUseItem(*item)) {
		MyPlayer->Say(HeroSpeech::ICantUseThisYet);
		return true;
	}

	if (IsWithdrawGoldOpen) {
		IsWithdrawGoldOpen = false;
		WithdrawGoldValue = 0;
	}

	if (item->isScroll()) {
		return true;
	}

	if (item->_iMiscId > IMISC_RUNEFIRST && item->_iMiscId < IMISC_RUNELAST && leveltype == DTYPE_TOWN) {
		return true;
	}

	if (item->_iMiscId == IMISC_BOOK)
		PlaySFX(IS_RBOOK);
	else
		PlaySFX(ItemInvSnds[ItemCAnimTbl[item->_iCurs]]);

	UseItem(MyPlayerId, item->_iMiscId, item->_iSpell);

	if (Stash.stashList[c]._iMiscId == IMISC_MAPOFDOOM)
		return true;
	if (Stash.stashList[c]._iMiscId == IMISC_NOTE) {
		InitQTextMsg(TEXT_BOOK9);
		CloseInventory();
		return true;
	}
	Stash.RemoveStashItem(c);

	return true;
}

void StashStruct::RemoveStashItem(StashStruct::StashCell iv)
{
	// Iterate through stashGrid and remove every reference to item
	for (auto &row : Stash.GetCurrentGrid()) {
		for (StashStruct::StashCell &itemId : row) {
			if (itemId - 1 == iv) {
				itemId = 0;
			}
		}
	}

	if (stashList.empty()) {
		return;
	}

	// If the item at the end of stash array isn't the one we removed, we need to swap its position in the array with the removed item
	StashStruct::StashCell lastItemIndex = static_cast<StashStruct::StashCell>(stashList.size() - 1);
	if (lastItemIndex != iv) {
		stashList[iv] = stashList[lastItemIndex];

		for (auto &pair : Stash.stashGrids) {
			auto &grid = pair.second;
			for (auto &row : grid) {
				for (StashStruct::StashCell &itemId : row) {
					if (itemId == lastItemIndex + 1) {
						itemId = iv + 1;
					}
				}
			}
		}
	}
	stashList.pop_back();
	Stash.dirty = true;
}

void StashStruct::SetPage(unsigned newPage)
{
	page = std::min(newPage, LastStashPage);
	dirty = true;
}

void StashStruct::NextPage(unsigned offset)
{
	if (page <= LastStashPage) {
		page += std::min(offset, LastStashPage - page);
	} else {
		page = LastStashPage;
	}
	dirty = true;
}

void StashStruct::PreviousPage(unsigned offset)
{
	if (page <= LastStashPage) {
		page -= std::min(offset, page);
	} else {
		page = LastStashPage;
	}
	dirty = true;
}

void StashStruct::RefreshItemStatFlags()
{
	for (auto &item : Stash.stashList) {
		item.updateRequiredStatsCacheForPlayer(*MyPlayer);
	}
}

void StartGoldWithdraw()
{
	CloseGoldDrop();

	InitialWithdrawGoldValue = std::min(RoomForGold(), Stash.gold);

	if (talkflag)
		control_reset_talk();

	Point start = GetPanelPosition(UiPanels::Stash, { 67, 128 });
	SDL_Rect rect = MakeSdlRect(start.x, start.y, 180, 20);
	SDL_SetTextInputRect(&rect);

	IsWithdrawGoldOpen = true;
	WithdrawGoldValue = 0;
	SDL_StartTextInput();
}

void WithdrawGoldKeyPress(SDL_Keycode vkey)
{
	Player &myPlayer = *MyPlayer;

	if (myPlayer._pHitPoints >> 6 <= 0) {
		CloseGoldWithdraw();
		return;
	}

	if ((vkey == SDLK_RETURN) || (vkey == SDLK_KP_ENTER)) {
		if (WithdrawGoldValue > 0) {
			WithdrawGold(myPlayer, WithdrawGoldValue);
			PlaySFX(IS_GOLD);
		}
		CloseGoldWithdraw();
	} else if (vkey == SDLK_ESCAPE) {
		CloseGoldWithdraw();
	} else if (vkey == SDLK_BACKSPACE) {
		WithdrawGoldValue /= 10;
	}
}

void DrawGoldWithdraw(const Surface &out, int amount)
{
	if (!IsWithdrawGoldOpen) {
		return;
	}

	const int dialogX = 30;

	ClxDraw(out, GetPanelPosition(UiPanels::Stash, { dialogX, 178 }), (*pGBoxBuff)[0]);

	// Pre-wrap the string at spaces, otherwise DrawString would hard wrap in the middle of words
	const std::string wrapped = WordWrapString(_("How many gold pieces do you want to withdraw?"), 200);

	// The split gold dialog is roughly 4 lines high, but we need at least one line for the player to input an amount.
	// Using a clipping region 50 units high (approx 3 lines with a lineheight of 17) to ensure there is enough room left
	//  for the text entered by the player.
	DrawString(out, wrapped, { GetPanelPosition(UiPanels::Stash, { dialogX + 31, 75 }), { 200, 50 } }, UiFlags::ColorWhitegold | UiFlags::AlignCenter, 1, 17);

	std::string value = "";
	if (amount > 0) {
		value = StrCat(amount);
	}
	// Even a ten digit amount of gold only takes up about half a line. There's no need to wrap or clip text here so we
	// use the Point form of DrawString.
	DrawString(out, value, GetPanelPosition(UiPanels::Stash, { dialogX + 37, 128 }), UiFlags::ColorWhite | UiFlags::PentaCursor);
}

void CloseGoldWithdraw()
{
	if (!IsWithdrawGoldOpen)
		return;
	IsWithdrawGoldOpen = false;
	WithdrawGoldValue = 0;
	SDL_StopTextInput();
}

void GoldWithdrawNewText(string_view text)
{
	for (char vkey : text) {
		int digit = vkey - '0';
		if (digit >= 0 && digit <= 9) {
			int newGoldValue = WithdrawGoldValue * 10;
			newGoldValue += digit;
			if (newGoldValue <= InitialWithdrawGoldValue) {
				WithdrawGoldValue = newGoldValue;
			}
		}
	}
}

bool AutoPlaceItemInStash(Player &player, const Item &item, bool persistItem)
{
	if (item._itype == ItemType::Gold) {
		if (persistItem) {
			Stash.gold += item._ivalue;
			Stash.dirty = true;
		}
		return true;
	}

	Size itemSize = GetInventorySize(item);

	// Try to add the item to the current active page and if it's not possible move forward
	for (unsigned pageCounter = 0; pageCounter < CountStashPages; pageCounter++) {
		unsigned pageIndex = Stash.GetPage() + pageCounter;
		// Wrap around if needed
		if (pageIndex >= CountStashPages)
			pageIndex -= CountStashPages;
		// Search all possible position in stash grid
		for (auto stashPosition : PointsInRectangleRange({ { 0, 0 }, Size { 10 - (itemSize.width - 1), 10 - (itemSize.height - 1) } })) {
			// Check that all needed slots are free
			bool isSpaceFree = true;
			for (auto itemPoint : PointsInRectangleRange({ stashPosition, itemSize })) {
				uint16_t iv = Stash.stashGrids[pageIndex][itemPoint.x][itemPoint.y];
				if (iv != 0) {
					isSpaceFree = false;
					break;
				}
			}
			if (!isSpaceFree)
				continue;
			if (persistItem) {
				Stash.stashList.push_back(item);
				uint16_t stashIndex = static_cast<uint16_t>(Stash.stashList.size() - 1);
				Stash.stashList[stashIndex].position = stashPosition + Displacement { 0, itemSize.height - 1 };
				AddItemToStashGrid(pageIndex, stashPosition, stashIndex, itemSize);
				Stash.dirty = true;
			}
			return true;
		}
	}

	return false;
}

} // namespace devilution
