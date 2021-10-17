/**
 * @file stash.cpp
 *
 * Implementation of player stash.
 */
#include <utility>

#include <algorithm>
#include <fmt/format.h>

#include "cursor.h"
#include "engine/cel_sprite.hpp"
#include "engine/load_cel.hpp"
#include "engine/render/cel_render.hpp"
#include "engine/render/text_render.hpp"
#include "engine/size.hpp"
#include "hwcursor.hpp"
#include "inv_iterators.hpp"
#include "minitext.h"
#include "options.h"
#include "plrmsg.h"
#include "stores.h"
#include "town.h"
#include "towners.h"
#include "controls/plrctrls.h"
#include "utils/language.h"
#include "utils/sdl_geometry.h"
#include "utils/stdcompat/optional.hpp"
#include <fstream>
#include <iostream>

namespace devilution {

bool stashflag;

std::streampos size;

StashStruct Stash;

StashStruct EmptyStash;

int tab;

int stashx = 16;                         // initial stash cell x coordinate
int stashy = 105;                        // initial stash cell y coordinate
int stashslotadj = INV_SLOT_SIZE_PX + 1; // spacing between each cell

const Point StashRect[] = { // Contains mappings for each cell in the stash (10x10)
	// clang-format off
	//  X,   Y
	// row 1
	{ stashx + stashslotadj * 0,  stashy + stashslotadj * 0 },
	{ stashx + stashslotadj * 1,  stashy + stashslotadj * 0 },
	{ stashx + stashslotadj * 2,  stashy + stashslotadj * 0 },
	{ stashx + stashslotadj * 3,  stashy + stashslotadj * 0 },
	{ stashx + stashslotadj * 4,  stashy + stashslotadj * 0 },
	{ stashx + stashslotadj * 5,  stashy + stashslotadj * 0 },
	{ stashx + stashslotadj * 6,  stashy + stashslotadj * 0 },
	{ stashx + stashslotadj * 7,  stashy + stashslotadj * 0 },
	{ stashx + stashslotadj * 8,  stashy + stashslotadj * 0 },
	{ stashx + stashslotadj * 9,  stashy + stashslotadj * 0 },
	// row 2
	{ stashx + stashslotadj * 0,  stashy + stashslotadj * 1 },
	{ stashx + stashslotadj * 1,  stashy + stashslotadj * 1 },
	{ stashx + stashslotadj * 2,  stashy + stashslotadj * 1 },
	{ stashx + stashslotadj * 3,  stashy + stashslotadj * 1 },
	{ stashx + stashslotadj * 4,  stashy + stashslotadj * 1 },
	{ stashx + stashslotadj * 5,  stashy + stashslotadj * 1 },
	{ stashx + stashslotadj * 6,  stashy + stashslotadj * 1 },
	{ stashx + stashslotadj * 7,  stashy + stashslotadj * 1 },
	{ stashx + stashslotadj * 8,  stashy + stashslotadj * 1 },
	{ stashx + stashslotadj * 9,  stashy + stashslotadj * 1 },
	// row 3
	{ stashx + stashslotadj * 0,  stashy + stashslotadj * 2 },
	{ stashx + stashslotadj * 1,  stashy + stashslotadj * 2 },
	{ stashx + stashslotadj * 2,  stashy + stashslotadj * 2 },
	{ stashx + stashslotadj * 3,  stashy + stashslotadj * 2 },
	{ stashx + stashslotadj * 4,  stashy + stashslotadj * 2 },
	{ stashx + stashslotadj * 5,  stashy + stashslotadj * 2 },
	{ stashx + stashslotadj * 6,  stashy + stashslotadj * 2 },
	{ stashx + stashslotadj * 7,  stashy + stashslotadj * 2 },
	{ stashx + stashslotadj * 8,  stashy + stashslotadj * 2 },
	{ stashx + stashslotadj * 9,  stashy + stashslotadj * 2 },
	// row 4
	{ stashx + stashslotadj * 0,  stashy + stashslotadj * 3 },
	{ stashx + stashslotadj * 1,  stashy + stashslotadj * 3 },
	{ stashx + stashslotadj * 2,  stashy + stashslotadj * 3 },
	{ stashx + stashslotadj * 3,  stashy + stashslotadj * 3 },
	{ stashx + stashslotadj * 4,  stashy + stashslotadj * 3 },
	{ stashx + stashslotadj * 5,  stashy + stashslotadj * 3 },
	{ stashx + stashslotadj * 6,  stashy + stashslotadj * 3 },
	{ stashx + stashslotadj * 7,  stashy + stashslotadj * 3 },
	{ stashx + stashslotadj * 8,  stashy + stashslotadj * 3 },
	{ stashx + stashslotadj * 9,  stashy + stashslotadj * 3 },
	// row 5
	{ stashx + stashslotadj * 0,  stashy + stashslotadj * 4 },
	{ stashx + stashslotadj * 1,  stashy + stashslotadj * 4 },
	{ stashx + stashslotadj * 2,  stashy + stashslotadj * 4 },
	{ stashx + stashslotadj * 3,  stashy + stashslotadj * 4 },
	{ stashx + stashslotadj * 4,  stashy + stashslotadj * 4 },
	{ stashx + stashslotadj * 5,  stashy + stashslotadj * 4 },
	{ stashx + stashslotadj * 6,  stashy + stashslotadj * 4 },
	{ stashx + stashslotadj * 7,  stashy + stashslotadj * 4 },
	{ stashx + stashslotadj * 8,  stashy + stashslotadj * 4 },
	{ stashx + stashslotadj * 9,  stashy + stashslotadj * 4 },
	// row 6
	{ stashx + stashslotadj * 0,  stashy + stashslotadj * 5 },
	{ stashx + stashslotadj * 1,  stashy + stashslotadj * 5 },
	{ stashx + stashslotadj * 2,  stashy + stashslotadj * 5 },
	{ stashx + stashslotadj * 3,  stashy + stashslotadj * 5 },
	{ stashx + stashslotadj * 4,  stashy + stashslotadj * 5 },
	{ stashx + stashslotadj * 5,  stashy + stashslotadj * 5 },
	{ stashx + stashslotadj * 6,  stashy + stashslotadj * 5 },
	{ stashx + stashslotadj * 7,  stashy + stashslotadj * 5 },
	{ stashx + stashslotadj * 8,  stashy + stashslotadj * 5 },
	{ stashx + stashslotadj * 9,  stashy + stashslotadj * 5 },
	// row 7
	{ stashx + stashslotadj * 0,  stashy + stashslotadj * 6 },
	{ stashx + stashslotadj * 1,  stashy + stashslotadj * 6 },
	{ stashx + stashslotadj * 2,  stashy + stashslotadj * 6 },
	{ stashx + stashslotadj * 3,  stashy + stashslotadj * 6 },
	{ stashx + stashslotadj * 4,  stashy + stashslotadj * 6 },
	{ stashx + stashslotadj * 5,  stashy + stashslotadj * 6 },
	{ stashx + stashslotadj * 6,  stashy + stashslotadj * 6 },
	{ stashx + stashslotadj * 7,  stashy + stashslotadj * 6 },
	{ stashx + stashslotadj * 8,  stashy + stashslotadj * 6 },
	{ stashx + stashslotadj * 9,  stashy + stashslotadj * 6 },
	// row 8
	{ stashx + stashslotadj * 0,  stashy + stashslotadj * 7 },
	{ stashx + stashslotadj * 1,  stashy + stashslotadj * 7 },
	{ stashx + stashslotadj * 2,  stashy + stashslotadj * 7 },
	{ stashx + stashslotadj * 3,  stashy + stashslotadj * 7 },
	{ stashx + stashslotadj * 4,  stashy + stashslotadj * 7 },
	{ stashx + stashslotadj * 5,  stashy + stashslotadj * 7 },
	{ stashx + stashslotadj * 6,  stashy + stashslotadj * 7 },
	{ stashx + stashslotadj * 7,  stashy + stashslotadj * 7 },
	{ stashx + stashslotadj * 8,  stashy + stashslotadj * 7 },
	{ stashx + stashslotadj * 9,  stashy + stashslotadj * 7 },
	// row 9
	{ stashx + stashslotadj * 0,  stashy + stashslotadj * 8 },
	{ stashx + stashslotadj * 1,  stashy + stashslotadj * 8 },
	{ stashx + stashslotadj * 2,  stashy + stashslotadj * 8 },
	{ stashx + stashslotadj * 3,  stashy + stashslotadj * 8 },
	{ stashx + stashslotadj * 4,  stashy + stashslotadj * 8 },
	{ stashx + stashslotadj * 5,  stashy + stashslotadj * 8 },
	{ stashx + stashslotadj * 6,  stashy + stashslotadj * 8 },
	{ stashx + stashslotadj * 7,  stashy + stashslotadj * 8 },
	{ stashx + stashslotadj * 8,  stashy + stashslotadj * 8 },
	{ stashx + stashslotadj * 9,  stashy + stashslotadj * 8 },
	// row 10
	{ stashx + stashslotadj * 0,  stashy + stashslotadj * 9 },
	{ stashx + stashslotadj * 1,  stashy + stashslotadj * 9 },
	{ stashx + stashslotadj * 2,  stashy + stashslotadj * 9 },
	{ stashx + stashslotadj * 3,  stashy + stashslotadj * 9 },
	{ stashx + stashslotadj * 4,  stashy + stashslotadj * 9 },
	{ stashx + stashslotadj * 5,  stashy + stashslotadj * 9 },
	{ stashx + stashslotadj * 6,  stashy + stashslotadj * 9 },
	{ stashx + stashslotadj * 7,  stashy + stashslotadj * 9 },
	{ stashx + stashslotadj * 8,  stashy + stashslotadj * 9 },
	{ stashx + stashslotadj * 9,  stashy + stashslotadj * 9 }

	// clang-format on
};

void LoadStash(int page)
{
	// Loads stash from file

	// Clear the stash first, in case we fail
	Stash._pNumStash = 0;
	memset(Stash.StashGrid, 0, sizeof(Stash.StashGrid));

	char szStash[512];
	sprintf(szStash, "%s%i%s", "stash_", page, ".dxs");

	std::ifstream fin(szStash, std::ifstream::binary);

	if (fin.is_open()) {
		fin.read((char *)&Stash._pNumStash, sizeof(Stash._pNumStash));
		fin.read((char *)&Stash.StashGrid, sizeof(Stash.StashGrid));

		if (fin) {
			// Continue reading items
			for (int i = 0; i < Stash._pNumStash; i++) {
				fin.read((char *)Stash.StashList, static_cast<std::streamsize>(sizeof(Item)) * Stash._pNumStash);
			}
		}
		fin.close();
	}
	//CalcPlrStash(myplr, TRUE);
}

void SaveStash(int page)
{
	// Saves stash to file

	char szStash[512];
	sprintf(szStash, "%s%i%s", "stash_", page, ".dxs");

	std::ofstream fout(szStash, std::ofstream::binary);

	if (fout.is_open()) {
		fout.write((char *)&Stash._pNumStash, sizeof(Stash._pNumStash));
		fout.write((char *)&Stash.StashGrid, sizeof(Stash.StashGrid));
		fout.write((char *)Stash.StashList, static_cast<std::streamsize>(sizeof(Item)) * Stash._pNumStash);
		fout.close();
	}
}

namespace {
enum class stash_xy_slot;

std::optional<CelSprite> pStashCels;

void StashDrawSlotBack(const Surface &out, Point targetPosition, Size size)
{
	SDL_Rect srcRect = MakeSdlRect(0, 0, size.width, size.height);
	out.Clip(&srcRect, &targetPosition);
	if (size.width <= 0 || size.height <= 0)
		return;

	std::uint8_t *dst = &out[targetPosition];
	const auto dstPitch = out.pitch();

	for (int hgt = size.height; hgt != 0; hgt--, dst -= dstPitch + size.width) {
		for (int wdt = size.width; wdt != 0; wdt--) {
			std::uint8_t pix = *dst;
			if (pix >= PAL16_BLUE) {
				if (pix <= PAL16_BLUE + 15)
					pix -= PAL16_BLUE - PAL16_BEIGE;
				else if (pix >= PAL16_GRAY)
					pix -= PAL16_GRAY - PAL16_BEIGE;
			}
			*dst++ = pix;
		}
	}
}

/**
 * @brief Adds an item to a player's StashGrid array
 * @param stashGridIndex Item's position in StashGrid (this should be the item's topleft grid tile)
 * @param stashListIndex The item's StashList index (it's expected this already has +1 added to it since StashGrid can't store a 0 index)
 * @param itemSize Size of item
 */
void AddItemToStashGrid(int stashGridIndex, int stashListIndex, Size itemSize)
{
	const int pitch = 10;
	for (int y = 0; y < itemSize.height; y++) {
		for (int x = 0; x < itemSize.width; x++) {
			if (x == 0 && y == itemSize.height - 1)
				Stash.StashGrid[stashGridIndex + x] = stashListIndex;
			else
				Stash.StashGrid[stashGridIndex + x] = -stashListIndex;
		}
		stashGridIndex += pitch;
	}
}

/**
 * @brief Gets the size, in stash cells, of the given item.
 * @param item The item whose size is to be determined.
 * @return The size, in stash cells, of the item.
 */
Size GetStashSize(const Item &item)
{
	int itemSizeIndex = item._iCurs + CURSOR_FIRSTITEM;
	auto size = GetInvItemSize(itemSizeIndex);

	return { size.width / InventorySlotSizeInPixels.width, size.height / InventorySlotSizeInPixels.height };
}

/**
 * @brief Checks whether the given item can fit in a belt slot (i.e. the item's size in inventory cells is 1x1).
 * @param item The item to be checked.
 * @return 'True' in case the item can fit a belt slot and 'False' otherwise.
 */
bool FitsInBeltSlot(const Item &item)
{
	return GetStashSize(item) == Size { 1, 1 };
}

/**
 * @brief Checks whether the given item can be placed on the belt. Takes item size as well as characteristics into account. Items
 * that cannot be placed on the belt have to be placed in the inventory instead.
 * @param item The item to be checked.
 * @return 'True' in case the item can be placed on the belt and 'False' otherwise.
 */
bool CanBePlacedOnBelt(const Item &item)
{
	return FitsInBeltSlot(item)
	    && item._itype != ItemType::Gold
	    && item._iStatFlag
	    && AllItemsList[item.IDidx].iUsable;
}

int SwapItem(Item *a, Item *b)
{
	std::swap(*a, *b);

	return b->_iCurs + CURSOR_FIRSTITEM;
}

void CheckStashPaste(int pnum, Point cursorPosition)
{
	auto &player = Players[pnum];

	SetICursor(player.HoldItem._iCurs + CURSOR_FIRSTITEM);
	int i = cursorPosition.x + (IsHardwareCursor() ? 0 : (icursSize.width / 2));
	int j = cursorPosition.y + (IsHardwareCursor() ? 0 : (icursSize.height / 2));
	Size itemSize { icursSize28 };
	bool done = false;
	int r = 0;
	for (; r < STASH_NUM_XY_SLOTS && !done; r++) {
		int xo = LeftPanel.position.x;
		int yo = LeftPanel.position.y;

		if (i >= StashRect[r].x + xo && i <= StashRect[r].x + xo + InventorySlotSizeInPixels.width) {
			if (j >= StashRect[r].y + yo - InventorySlotSizeInPixels.height - 1 && j < StashRect[r].y + yo) {
				done = true;
				r--;
			}
		}
		if (r == SLOTXY_STASH_LAST && (itemSize.height & 1) == 0)
			j += INV_SLOT_HALF_SIZE_PX;
	}
	if (!done)
		return;

	item_equip_type il = ILOC_UNEQUIPABLE;

	done = player.HoldItem._iLoc == il;


	int8_t it = 0;
	if (il == ILOC_UNEQUIPABLE) {
		done = true;
		int ii = r - SLOTXY_STASH_FIRST;
		if (player.HoldItem._itype == ItemType::Gold) {
			if (Stash.StashGrid[ii] != 0) {
				int8_t iv = Stash.StashGrid[ii];
				if (iv > 0) {
					if (Stash.StashList[iv - 1]._itype != ItemType::Gold) {
						it = iv;
					}
				} else {
					it = -iv;
				}
			}
		} else {
			int yy = std::max(INV_ROW_SLOT_SIZE * ((ii / INV_ROW_SLOT_SIZE) - ((itemSize.height - 1) / 2)), 0);
			for (j = 0; j < itemSize.height && done; j++) {
				if (yy >= NUM_STASH_GRID_ELEM)
					done = false;
				int xx = std::max((ii % INV_ROW_SLOT_SIZE) - ((itemSize.width - 1) / 2), 0);
				for (i = 0; i < itemSize.width && done; i++) {
					if (xx >= INV_ROW_SLOT_SIZE) {
						done = false;
					} else {
						if (Stash.StashGrid[xx + yy] != 0) {
							int8_t iv = abs(Stash.StashGrid[xx + yy]);
							if (it != 0) {
								if (it != iv)
									done = false;
							} else {
								it = iv;
							}
						}
					}
					xx++;
				}
				yy += INV_ROW_SLOT_SIZE;
			}
		}
	}

	if (!done)
		return;

	if (IsNoneOf(il, ILOC_UNEQUIPABLE, ILOC_BELT) && !player.HoldItem._iStatFlag) {
		done = false;
		player.Say(HeroSpeech::ICantUseThisYet);
	}

	if (!done)
		return;

	if (pnum == MyPlayerId)
		PlaySFX(ItemInvSnds[ItemCAnimTbl[player.HoldItem._iCurs]]);

	int cn = CURSOR_HAND;
	
	CalcPlrInv(player, true);
	if (pnum == MyPlayerId) {
		if (cn == CURSOR_HAND && !IsHardwareCursor())
			SetCursorPos(MousePosition + Displacement(cursSize / 2));
		NewCursor(cn);
	}
}

void CheckStashCut(int pnum, Point cursorPosition, bool automaticMove, bool dropItem)
{
	auto &player = Players[pnum];

	if (player._pmode > PM_WALK3) {
		return;
	}

	if (dropGoldFlag) {
		dropGoldFlag = false;
		dropGoldValue = 0;
	}

	bool done = false;

	uint32_t r = 0;
	for (; r < NUM_XY_SLOTS; r++) {
		int xo = LeftPanel.position.x;
		int yo = LeftPanel.position.y;

		// check which inventory rectangle the mouse is in, if any
		if (cursorPosition.x >= StashRect[r].x + xo
		    && cursorPosition.x < StashRect[r].x + xo + (InventorySlotSizeInPixels.width + 1)
		    && cursorPosition.y >= StashRect[r].y + yo - (InventorySlotSizeInPixels.height + 1)
		    && cursorPosition.y < StashRect[r].y + yo) {
			done = true;
			break;
		}
	}

	if (!done) {
		// not on an inventory slot rectangle
		return;
	}

	Item &holdItem = player.HoldItem;
	holdItem._itype = ItemType::None;

	bool automaticallyMoved = false;
	bool automaticallyEquipped = false;
	bool automaticallyUnequip = false;

	if (r >= SLOTXY_STASH_FIRST && r <= SLOTXY_STASH_LAST) {
		int ig = r - SLOTXY_STASH_FIRST;
		int8_t ii = Stash.StashGrid[ig];
		if (ii != 0) {
			int iv = (ii < 0) ? -ii : ii;

			holdItem = Stash.StashList[iv - 1];
			if (automaticMove) {
				if (CanBePlacedOnBelt(holdItem)) {
					automaticallyMoved = AutoPlaceItemInBelt(player, holdItem, true);
				} else {
					automaticallyMoved = automaticallyEquipped = AutoEquip(pnum, holdItem);
				}
			}

			if (!automaticMove || automaticallyMoved) {
				Stash.RemoveStashItem(iv - 1, false);
			}
		}
	}

	if (!holdItem.isEmpty()) {

		CalcPlrInv(player, true);
		CheckItemStats(player);

		if (pnum == MyPlayerId) {
			if (automaticallyEquipped) {
				PlaySFX(ItemInvSnds[ItemCAnimTbl[holdItem._iCurs]]);
			} else if (!automaticMove || automaticallyMoved) {
				PlaySFX(IS_IGRAB);
			}

			if (automaticMove) {
				if (!automaticallyMoved) {
					if (CanBePlacedOnBelt(holdItem) || automaticallyUnequip) {
						player.SaySpecific(HeroSpeech::IHaveNoRoom);
					} else {
						player.SaySpecific(HeroSpeech::ICantDoThat);
					}
				}

				holdItem._itype = ItemType::None;
			} else {
				NewCursor(holdItem._iCurs + CURSOR_FIRSTITEM);
				if (!IsHardwareCursor() && !dropItem) {
					// For a hardware cursor, we set the "hot point" to the center of the item instead.
					SetCursorPos(cursorPosition - Displacement(cursSize / 2));
				}
			}
		}
	}

	if (dropItem) {
		TryDropItem();
	}
}

} // namespace

void FreeStashGFX()
{
	pStashCels = std::nullopt;
}

void InitStash()
{
	//pStashCels = LoadCel("data\\Stash\\Stash.CEL", SPANEL_WIDTH);
	pStashCels = LoadCel("Data\\Inv\\Inv.CEL", SPANEL_WIDTH);
	tab = 0;
	stashflag = false;
}

void DrawStash(const Surface &out)
{
	CelDrawTo(out, GetPanelPosition(UiPanels::Stash, { 0, 351 }), *pStashCels, 1);
	
	for (int i = 0; i < NUM_STASH_GRID_ELEM; i++) {
		if (Stash.StashGrid[i] != 0) {
			StashDrawSlotBack(
			    out,
			    GetPanelPosition(UiPanels::Stash, StashRect[i + SLOTXY_STASH_FIRST]) + Displacement { 0, -1 },
			    InventorySlotSizeInPixels);
		}
	}

	for (int j = 0; j < NUM_STASH_GRID_ELEM; j++) {
		if (Stash.StashGrid[j] > 0) { // first slot of an item
			int ii = Stash.StashGrid[j] - 1;
			int frame = Stash.StashList[ii]._iCurs + CURSOR_FIRSTITEM;

			const auto &cel = GetInvItemSprite(frame);
			const int celFrame = GetInvItemFrame(frame);
			const Point position = GetPanelPosition(UiPanels::Stash, StashRect[j + SLOTXY_STASH_FIRST]) + Displacement { 0, -1 };
			if (pcursstashitem == ii + STASHITEM_STASH_FIRST) {
				CelBlitOutlineTo(
				    out,
				    GetOutlineColor(Stash.StashList[ii], true),
				    position,
				    cel, celFrame, false);
			}

			CelDrawItem(
			    Stash.StashList[ii],
			    out,
			    position,
			    cel, celFrame);
		}
	}
}

void CheckStashSwap(Player &player, inv_body_loc bLoc, int idx, uint16_t wCI, int seed, bool bId, uint32_t dwBuff)
{
	auto &item = Items[MAXITEMS];
	memset(&item, 0, sizeof(item));
	RecreateItem(item, idx, wCI, seed, 0, (dwBuff & CF_HELLFIRE) != 0);

	player.HoldItem = item;

	if (bId) {
		player.HoldItem._iIdentified = true;
	}

	player.InvBody[bLoc] = player.HoldItem;

	if (bLoc == INVLOC_HAND_LEFT && player.HoldItem._iLoc == ILOC_TWOHAND) {
		player.InvBody[INVLOC_HAND_RIGHT]._itype = ItemType::None;
	} else if (bLoc == INVLOC_HAND_RIGHT && player.HoldItem._iLoc == ILOC_TWOHAND) {
		player.InvBody[INVLOC_HAND_LEFT]._itype = ItemType::None;
	}

	CalcPlrInv(player, true);
}

void CheckStashItem(bool isShiftHeld, bool isCtrlHeld)
{
	if (pcurs >= CURSOR_FIRSTITEM) {
		CheckStashPaste(MyPlayerId, MousePosition);
	} else {
		CheckStashCut(MyPlayerId, MousePosition, isShiftHeld, isCtrlHeld);
	}
}

void CheckStashScrn(bool isShiftHeld, bool isCtrlHeld)
{
	if (MousePosition.x > 0 && MousePosition.x < 320
	    && MousePosition.y > PANEL_TOP && MousePosition.y < 0 + PANEL_TOP) {
		CheckStashItem(isShiftHeld, isCtrlHeld);
	}
}

int8_t CheckStashHLight()
{
	int8_t r = 0;
	for (; r < STASH_NUM_XY_SLOTS; r++) {
		int xo = LeftPanel.position.x;
		int yo = LeftPanel.position.y;


		if (MousePosition.x >= StashRect[r].x + xo
		    && MousePosition.x < StashRect[r].x + xo + (InventorySlotSizeInPixels.width + 1)
		    && MousePosition.y >= StashRect[r].y + yo - (InventorySlotSizeInPixels.height + 1)
		    && MousePosition.y < StashRect[r].y + yo) {
			break;
		}
	}

	int8_t rv = -1;
	InfoColor = UiFlags::ColorWhite;
	Item *pi = nullptr;
	auto &myPlayer = Players[MyPlayerId];

	ClearPanel();
	
	if (r >= SLOTXY_STASH_FIRST && r <= SLOTXY_STASH_LAST) {
		int8_t itemId = abs(Stash.StashGrid[r - SLOTXY_STASH_FIRST]);
		if (itemId == 0)
			return -1;
		int ii = itemId - 1;
		rv = ii + STASHITEM_STASH_FIRST;
		pi = &Stash.StashList[ii];
	}

	if (pi->isEmpty())
		return -1;

	if (pi->_itype == ItemType::Gold) {
		int nGold = pi->_ivalue;
		strcpy(infostr, fmt::format(ngettext("{:d} gold piece", "{:d} gold pieces", nGold), nGold).c_str());
	} else {
		InfoColor = pi->getTextColor();
		if (pi->_iIdentified) {
			strcpy(infostr, pi->_iIName);
			PrintItemDetails(pi);
		} else {
			strcpy(infostr, pi->_iName);
			PrintItemDur(pi);
		}
	}

	return rv;
}

bool UseStashItem(int pnum, int cii)
{
	int c;
	Item *item;

	auto &player = Players[pnum];

	if (player._pInvincible && player._pHitPoints == 0 && pnum == MyPlayerId)
		return true;
	if (pcurs != CURSOR_HAND)
		return true;
	if (stextflag != STORE_NONE)
		return true;
	if (cii < STASHITEM_STASH_FIRST)
		return false;

	bool speedlist = false;
	if (cii <= STASHITEM_STASH_LAST) {
		c = cii - STASHITEM_STASH_FIRST;
		item = &Stash.StashList[c];
	}

	constexpr int SpeechDelay = 10;
	if (item->IDidx == IDI_MUSHROOM) {
		player.Say(HeroSpeech::NowThatsOneBigMushroom, SpeechDelay);
		return true;
	}
	if (item->IDidx == IDI_FUNGALTM) {
		PlaySFX(IS_IBOOK);
		player.Say(HeroSpeech::ThatDidntDoAnything, SpeechDelay);
		return true;
	}

	if (!AllItemsList[item->IDidx].iUsable)
		return false;

	if (!item->_iStatFlag) {
		player.Say(HeroSpeech::ICantUseThisYet);
		return true;
	}

	if (dropGoldFlag) {
		dropGoldFlag = false;
		dropGoldValue = 0;
	}

	if (item->IsScroll() && currlevel == 0 && !spelldata[item->_iSpell].sTownSpell) {
		return true;
	}

	if (item->_iMiscId > IMISC_RUNEFIRST && item->_iMiscId < IMISC_RUNELAST && currlevel == 0) {
		return true;
	}

	int idata = ItemCAnimTbl[item->_iCurs];
	if (item->_iMiscId == IMISC_BOOK)
		PlaySFX(IS_RBOOK);
	else if (pnum == MyPlayerId)
		PlaySFX(ItemInvSnds[idata]);

	UseItem(pnum, item->_iMiscId, item->_iSpell);

	if (Stash.StashList[c]._iMiscId == IMISC_MAPOFDOOM)
		return true;
	if (Stash.StashList[c]._iMiscId == IMISC_NOTE) {
		InitQTextMsg(TEXT_BOOK9);
		stashflag = false;
		return true;
	}
	Stash.RemoveStashItem(c, false);

	return true;
}

} // namespace devilution
