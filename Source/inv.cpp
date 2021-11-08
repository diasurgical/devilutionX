/**
 * @file inv.cpp
 *
 * Implementation of player inventory.
 */
#include <utility>

#include <algorithm>
#include <fmt/format.h>

#include "controls/plrctrls.h"
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
#include "utils/language.h"
#include "utils/sdl_geometry.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

bool invflag;
bool drawsbarflag;

/**
 * Maps from inventory slot to screen position. The inventory slots are
 * arranged as follows:
 *                          00 01
 *                          02 03   06
 *
 *              07 08       19 20       13 14
 *              09 10       21 22       15 16
 *              11 12       23 24       17 18
 *
 *                 04                   05
 *
 *              25 26 27 28 29 30 31 32 33 34
 *              35 36 37 38 39 40 41 42 43 44
 *              45 46 47 48 49 50 51 52 53 54
 *              55 56 57 58 59 60 61 62 63 64
 *
 * 65 66 67 68 69 70 71 72
 */
const Point InvRect[] = {
	// clang-format off
	//  X,   Y
	{ 132,  31 }, // helmet
	{ 160,  31 }, // helmet
	{ 132,  59 }, // helmet
	{ 160,  59 }, // helmet
	{  45, 205 }, // left ring
	{ 247, 205 }, // right ring
	{ 204,  59 }, // amulet
	{  17, 104 }, // left hand
	{  46, 104 }, // left hand
	{  17, 132 }, // left hand
	{  46, 132 }, // left hand
	{  17, 160 }, // left hand
	{  46, 160 }, // left hand
	{ 247, 104 }, // right hand
	{ 276, 104 }, // right hand
	{ 247, 132 }, // right hand
	{ 276, 132 }, // right hand
	{ 247, 160 }, // right hand
	{ 276, 160 }, // right hand
	{ 132, 104 }, // chest
	{ 160, 104 }, // chest
	{ 132, 132 }, // chest
	{ 160, 132 }, // chest
	{ 132, 160 }, // chest
	{ 160, 160 }, // chest
	{  17, 250 }, // inv row 1
	{  46, 250 }, // inv row 1
	{  75, 250 }, // inv row 1
	{ 104, 250 }, // inv row 1
	{ 133, 250 }, // inv row 1
	{ 162, 250 }, // inv row 1
	{ 191, 250 }, // inv row 1
	{ 220, 250 }, // inv row 1
	{ 249, 250 }, // inv row 1
	{ 278, 250 }, // inv row 1
	{  17, 279 }, // inv row 2
	{  46, 279 }, // inv row 2
	{  75, 279 }, // inv row 2
	{ 104, 279 }, // inv row 2
	{ 133, 279 }, // inv row 2
	{ 162, 279 }, // inv row 2
	{ 191, 279 }, // inv row 2
	{ 220, 279 }, // inv row 2
	{ 249, 279 }, // inv row 2
	{ 278, 279 }, // inv row 2
	{  17, 308 }, // inv row 3
	{  46, 308 }, // inv row 3
	{  75, 308 }, // inv row 3
	{ 104, 308 }, // inv row 3
	{ 133, 308 }, // inv row 3
	{ 162, 308 }, // inv row 3
	{ 191, 308 }, // inv row 3
	{ 220, 308 }, // inv row 3
	{ 249, 308 }, // inv row 3
	{ 278, 308 }, // inv row 3
	{  17, 337 }, // inv row 4
	{  46, 337 }, // inv row 4
	{  75, 337 }, // inv row 4
	{ 104, 337 }, // inv row 4
	{ 133, 337 }, // inv row 4
	{ 162, 337 }, // inv row 4
	{ 191, 337 }, // inv row 4
	{ 220, 337 }, // inv row 4
	{ 249, 337 }, // inv row 4
	{ 278, 337 }, // inv row 4
	{ 205,  33 }, // belt
	{ 234,  33 }, // belt
	{ 263,  33 }, // belt
	{ 292,  33 }, // belt
	{ 321,  33 }, // belt
	{ 350,  33 }, // belt
	{ 379,  33 }, // belt
	{ 408,  33 }  // belt
	// clang-format on
};

namespace {

std::optional<CelSprite> pInvCels;

void InvDrawSlotBack(const Surface &out, Point targetPosition, Size size)
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
 * @brief Adds an item to a player's InvGrid array
 * @param invGridIndex Item's position in InvGrid (this should be the item's topleft grid tile)
 * @param invListIndex The item's InvList index (it's expected this already has +1 added to it since InvGrid can't store a 0 index)
 * @param itemSize Size of item
 */
void AddItemToInvGrid(Player &player, int invGridIndex, int invListIndex, Size itemSize)
{
	const int pitch = 10;
	for (int y = 0; y < itemSize.height; y++) {
		for (int x = 0; x < itemSize.width; x++) {
			if (x == 0 && y == itemSize.height - 1)
				player.InvGrid[invGridIndex + x] = invListIndex;
			else
				player.InvGrid[invGridIndex + x] = -invListIndex;
		}
		invGridIndex += pitch;
	}
}

/**
 * @brief Gets the size, in inventory cells, of the given item.
 * @param item The item whose size is to be determined.
 * @return The size, in inventory cells, of the item.
 */
Size GetInventorySize(const Item &item)
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
	return GetInventorySize(item) == Size { 1, 1 };
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

/**
 * @brief Checks whether the given item can be equipped. Since this overload doesn't take player information, it only considers
 * general aspects about the item, like if its requirements are met and if the item's target location is valid for the body.
 * @param item The item to check.
 * @return 'True' in case the item could be equipped in a player, and 'False' otherwise.
 */
bool CanEquip(const Item &item)
{
	return item.isEquipment()
	    && item._iStatFlag;
}

/**
 * @brief A specialized version of 'CanEquip(int, Item&, int)' that specifically checks whether the item can be equipped
 * in one/both of the player's hands.
 * @param player The player whose inventory will be checked for compatibility with the item.
 * @param item The item to check.
 * @return 'True' if the player can currently equip the item in either one of his hands (i.e. the required hands are empty and
 * allow the item), and 'False' otherwise.
 */
bool CanWield(Player &player, const Item &item)
{
	if (!CanEquip(item) || IsNoneOf(item._iLoc, ILOC_ONEHAND, ILOC_TWOHAND))
		return false;

	Item &leftHandItem = player.InvBody[INVLOC_HAND_LEFT];
	Item &rightHandItem = player.InvBody[INVLOC_HAND_RIGHT];

	if (leftHandItem.isEmpty() && rightHandItem.isEmpty()) {
		return true;
	}

	if (!leftHandItem.isEmpty() && !rightHandItem.isEmpty()) {
		return false;
	}

	Item &occupiedHand = !leftHandItem.isEmpty() ? leftHandItem : rightHandItem;

	// Barbarian can wield two handed swords and maces in one hand, so we allow equiping any sword/mace as long as his occupied
	// hand has a shield (i.e. no dual wielding allowed)
	if (player._pClass == HeroClass::Barbarian) {
		if (occupiedHand._itype == ItemType::Shield && IsAnyOf(item._itype, ItemType::Sword, ItemType::Mace))
			return true;
	}

	// Bard can dual wield swords and maces, so we allow equiping one-handed weapons in her free slot as long as her occupied
	// slot is another one-handed weapon.
	if (player._pClass == HeroClass::Bard) {
		bool occupiedHandIsOneHandedSwordOrMace = occupiedHand._iLoc == ILOC_ONEHAND
		    && IsAnyOf(occupiedHand._itype, ItemType::Sword, ItemType::Mace);

		bool weaponToEquipIsOneHandedSwordOrMace = item._iLoc == ILOC_ONEHAND
		    && IsAnyOf(item._itype, ItemType::Sword, ItemType::Mace);

		if (occupiedHandIsOneHandedSwordOrMace && weaponToEquipIsOneHandedSwordOrMace) {
			return true;
		}
	}

	return item._iLoc == ILOC_ONEHAND
	    && occupiedHand._iLoc == ILOC_ONEHAND
	    && item._iClass != occupiedHand._iClass;
}

/**
 * @brief Checks whether the specified item can be equipped in the desired body location on the player.
 * @param player The player whose inventory will be checked for compatibility with the item.
 * @param item The item to check.
 * @param bodyLocation The location in the inventory to be checked against.
 * @return 'True' if the player can currently equip the item in the specified body location (i.e. the body location is empty and
 * allows the item), and 'False' otherwise.
 */
bool CanEquip(Player &player, const Item &item, inv_body_loc bodyLocation)
{
	if (!CanEquip(item) || player._pmode > PM_WALK3 || !player.InvBody[bodyLocation].isEmpty()) {
		return false;
	}

	switch (bodyLocation) {
	case INVLOC_AMULET:
		return item._iLoc == ILOC_AMULET;

	case INVLOC_CHEST:
		return item._iLoc == ILOC_ARMOR;

	case INVLOC_HAND_LEFT:
	case INVLOC_HAND_RIGHT:
		return CanWield(player, item);

	case INVLOC_HEAD:
		return item._iLoc == ILOC_HELM;

	case INVLOC_RING_LEFT:
	case INVLOC_RING_RIGHT:
		return item._iLoc == ILOC_RING;

	default:
		return false;
	}
}

bool AutoEquip(int playerId, const Item &item, inv_body_loc bodyLocation, bool persistItem)
{
	auto &player = Players[playerId];

	if (!CanEquip(player, item, bodyLocation)) {
		return false;
	}

	if (persistItem) {
		player.InvBody[bodyLocation] = item;

		if (sgOptions.Audio.bAutoEquipSound && playerId == MyPlayerId) {
			PlaySFX(ItemInvSnds[ItemCAnimTbl[item._iCurs]]);
		}

		NetSendCmdChItem(false, bodyLocation);
		CalcPlrInv(player, true);
	}

	return true;
}

int SwapItem(Item *a, Item *b)
{
	std::swap(*a, *b);

	return b->_iCurs + CURSOR_FIRSTITEM;
}

void CheckInvPaste(int pnum, Point cursorPosition)
{
	auto &player = Players[pnum];

	SetICursor(player.HoldItem._iCurs + CURSOR_FIRSTITEM);
	int i = cursorPosition.x + (IsHardwareCursor() ? 0 : (icursSize.width / 2));
	int j = cursorPosition.y + (IsHardwareCursor() ? 0 : (icursSize.height / 2));
	Size itemSize { icursSize28 };
	bool done = false;
	int r = 0;
	for (; r < NUM_XY_SLOTS && !done; r++) {
		int xo = RightPanel.position.x;
		int yo = RightPanel.position.y;
		if (r >= SLOTXY_BELT_FIRST) {
			xo = PANEL_LEFT;
			yo = PANEL_TOP;
		}

		if (i >= InvRect[r].x + xo && i <= InvRect[r].x + xo + InventorySlotSizeInPixels.width) {
			if (j >= InvRect[r].y + yo - InventorySlotSizeInPixels.height - 1 && j < InvRect[r].y + yo) {
				done = true;
				r--;
			}
		}
		if (r == SLOTXY_CHEST_LAST) {
			if ((itemSize.width & 1) == 0)
				i -= INV_SLOT_HALF_SIZE_PX;
			if ((itemSize.height & 1) == 0)
				j -= INV_SLOT_HALF_SIZE_PX;
		}
		if (r == SLOTXY_INV_LAST && (itemSize.height & 1) == 0)
			j += INV_SLOT_HALF_SIZE_PX;
	}
	if (!done)
		return;

	item_equip_type il = ILOC_UNEQUIPABLE;
	if (r >= SLOTXY_HEAD_FIRST && r <= SLOTXY_HEAD_LAST)
		il = ILOC_HELM;
	if (r >= SLOTXY_RING_LEFT && r <= SLOTXY_RING_RIGHT)
		il = ILOC_RING;
	if (r == SLOTXY_AMULET)
		il = ILOC_AMULET;
	if (r >= SLOTXY_HAND_LEFT_FIRST && r <= SLOTXY_HAND_RIGHT_LAST)
		il = ILOC_ONEHAND;
	if (r >= SLOTXY_CHEST_FIRST && r <= SLOTXY_CHEST_LAST)
		il = ILOC_ARMOR;
	if (r >= SLOTXY_BELT_FIRST && r <= SLOTXY_BELT_LAST)
		il = ILOC_BELT;

	done = player.HoldItem._iLoc == il;

	if (il == ILOC_ONEHAND && player.HoldItem._iLoc == ILOC_TWOHAND) {
		if (player._pClass == HeroClass::Barbarian
		    && IsAnyOf(player.HoldItem._itype, ItemType::Sword, ItemType::Mace))
			il = ILOC_ONEHAND;
		else
			il = ILOC_TWOHAND;
		done = true;
	}
	if (player.HoldItem._iLoc == ILOC_UNEQUIPABLE && il == ILOC_BELT) {
		if (itemSize == Size { 1, 1 }) {
			done = true;
			if (!AllItemsList[player.HoldItem.IDidx].iUsable)
				done = false;
			if (!player.HoldItem._iStatFlag)
				done = false;
			if (player.HoldItem._itype == ItemType::Gold)
				done = false;
		}
	}

	int8_t it = 0;
	if (il == ILOC_UNEQUIPABLE) {
		done = true;
		int ii = r - SLOTXY_INV_FIRST;
		if (player.HoldItem._itype == ItemType::Gold) {
			if (player.InvGrid[ii] != 0) {
				int8_t iv = player.InvGrid[ii];
				if (iv > 0) {
					if (player.InvList[iv - 1]._itype != ItemType::Gold) {
						it = iv;
					}
				} else {
					it = -iv;
				}
			}
		} else {
			int yy = std::max(INV_ROW_SLOT_SIZE * ((ii / INV_ROW_SLOT_SIZE) - ((itemSize.height - 1) / 2)), 0);
			for (j = 0; j < itemSize.height && done; j++) {
				if (yy >= NUM_INV_GRID_ELEM)
					done = false;
				int xx = std::max((ii % INV_ROW_SLOT_SIZE) - ((itemSize.width - 1) / 2), 0);
				for (i = 0; i < itemSize.width && done; i++) {
					if (xx >= INV_ROW_SLOT_SIZE) {
						done = false;
					} else {
						if (player.InvGrid[xx + yy] != 0) {
							int8_t iv = abs(player.InvGrid[xx + yy]);
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

	if (player._pmode > PM_WALK3 && IsNoneOf(il, ILOC_UNEQUIPABLE, ILOC_BELT))
		return;

	if (pnum == MyPlayerId)
		PlaySFX(ItemInvSnds[ItemCAnimTbl[player.HoldItem._iCurs]]);

	int cn = CURSOR_HAND;
	switch (il) {
	case ILOC_HELM:
	case ILOC_RING:
	case ILOC_AMULET:
	case ILOC_ARMOR: {
		auto iLocToInvLoc = [&r](item_equip_type loc) {
			switch (loc) {
			case ILOC_HELM:
				return INVLOC_HEAD;
			case ILOC_RING:
				return (r == SLOTXY_RING_LEFT ? INVLOC_RING_LEFT : INVLOC_RING_RIGHT);
			case ILOC_AMULET:
				return INVLOC_AMULET;
			case ILOC_ARMOR:
				return INVLOC_CHEST;
			default:
				app_fatal("Unexpected equipment type");
			}
		};
		inv_body_loc slot = iLocToInvLoc(il);
		NetSendCmdChItem(false, slot);
		if (player.InvBody[slot].isEmpty())
			player.InvBody[slot] = player.HoldItem;
		else
			cn = SwapItem(&player.InvBody[slot], &player.HoldItem);
		break;
	}
	case ILOC_ONEHAND:
		if (r <= SLOTXY_HAND_LEFT_LAST) {
			if (player.InvBody[INVLOC_HAND_LEFT].isEmpty()) {
				if ((player.InvBody[INVLOC_HAND_RIGHT].isEmpty() || player.InvBody[INVLOC_HAND_RIGHT]._iClass != player.HoldItem._iClass)
				    || (player._pClass == HeroClass::Bard && player.InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON && player.HoldItem._iClass == ICLASS_WEAPON)) {
					NetSendCmdChItem(false, INVLOC_HAND_LEFT);
					player.InvBody[INVLOC_HAND_LEFT] = player.HoldItem;
				} else {
					NetSendCmdChItem(false, INVLOC_HAND_RIGHT);
					cn = SwapItem(&player.InvBody[INVLOC_HAND_RIGHT], &player.HoldItem);
				}
				break;
			}
			if ((player.InvBody[INVLOC_HAND_RIGHT].isEmpty() || player.InvBody[INVLOC_HAND_RIGHT]._iClass != player.HoldItem._iClass)
			    || (player._pClass == HeroClass::Bard && player.InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON && player.HoldItem._iClass == ICLASS_WEAPON)) {
				NetSendCmdChItem(false, INVLOC_HAND_LEFT);
				cn = SwapItem(&player.InvBody[INVLOC_HAND_LEFT], &player.HoldItem);
				break;
			}

			NetSendCmdChItem(false, INVLOC_HAND_RIGHT);
			cn = SwapItem(&player.InvBody[INVLOC_HAND_RIGHT], &player.HoldItem);
			break;
		}
		if (player.InvBody[INVLOC_HAND_RIGHT].isEmpty()) {
			if ((player.InvBody[INVLOC_HAND_LEFT].isEmpty() || player.InvBody[INVLOC_HAND_LEFT]._iLoc != ILOC_TWOHAND)
			    || (player._pClass == HeroClass::Barbarian && (player.InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Sword || player.InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Mace))) {
				if ((player.InvBody[INVLOC_HAND_LEFT].isEmpty() || player.InvBody[INVLOC_HAND_LEFT]._iClass != player.HoldItem._iClass)
				    || (player._pClass == HeroClass::Bard && player.InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON && player.HoldItem._iClass == ICLASS_WEAPON)) {
					NetSendCmdChItem(false, INVLOC_HAND_RIGHT);
					player.InvBody[INVLOC_HAND_RIGHT] = player.HoldItem;
					break;
				}
				NetSendCmdChItem(false, INVLOC_HAND_LEFT);
				cn = SwapItem(&player.InvBody[INVLOC_HAND_LEFT], &player.HoldItem);
				break;
			}
			NetSendCmdDelItem(false, INVLOC_HAND_LEFT);
			NetSendCmdChItem(false, INVLOC_HAND_RIGHT);
			SwapItem(&player.InvBody[INVLOC_HAND_RIGHT], &player.InvBody[INVLOC_HAND_LEFT]);
			cn = SwapItem(&player.InvBody[INVLOC_HAND_RIGHT], &player.HoldItem);
			break;
		}

		if ((!player.InvBody[INVLOC_HAND_LEFT].isEmpty() && player.InvBody[INVLOC_HAND_LEFT]._iClass == player.HoldItem._iClass)
		    && !(player._pClass == HeroClass::Bard && player.InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON && player.HoldItem._iClass == ICLASS_WEAPON)) {
			NetSendCmdChItem(false, INVLOC_HAND_LEFT);
			cn = SwapItem(&player.InvBody[INVLOC_HAND_LEFT], &player.HoldItem);
			break;
		}
		NetSendCmdChItem(false, INVLOC_HAND_RIGHT);
		cn = SwapItem(&player.InvBody[INVLOC_HAND_RIGHT], &player.HoldItem);
		break;
	case ILOC_TWOHAND:
		if (!player.InvBody[INVLOC_HAND_LEFT].isEmpty() && !player.InvBody[INVLOC_HAND_RIGHT].isEmpty()) {
			Item tempitem = player.HoldItem;
			if (player.InvBody[INVLOC_HAND_RIGHT]._itype == ItemType::Shield)
				player.HoldItem = player.InvBody[INVLOC_HAND_RIGHT];
			else
				player.HoldItem = player.InvBody[INVLOC_HAND_LEFT];
			if (pnum == MyPlayerId)
				NewCursor(player.HoldItem._iCurs + CURSOR_FIRSTITEM);
			else
				SetICursor(player.HoldItem._iCurs + CURSOR_FIRSTITEM);
			bool done2h = AutoPlaceItemInInventory(player, player.HoldItem, true);
			player.HoldItem = tempitem;
			if (pnum == MyPlayerId)
				NewCursor(player.HoldItem._iCurs + CURSOR_FIRSTITEM);
			else
				SetICursor(player.HoldItem._iCurs + CURSOR_FIRSTITEM);
			if (!done2h)
				return;

			if (player.InvBody[INVLOC_HAND_RIGHT]._itype == ItemType::Shield)
				player.InvBody[INVLOC_HAND_RIGHT]._itype = ItemType::None;
			else
				player.InvBody[INVLOC_HAND_LEFT]._itype = ItemType::None;
		}

		NetSendCmdDelItem(false, INVLOC_HAND_RIGHT);

		if (!player.InvBody[INVLOC_HAND_LEFT].isEmpty() || !player.InvBody[INVLOC_HAND_RIGHT].isEmpty()) {
			NetSendCmdChItem(false, INVLOC_HAND_LEFT);
			if (player.InvBody[INVLOC_HAND_LEFT].isEmpty())
				SwapItem(&player.InvBody[INVLOC_HAND_LEFT], &player.InvBody[INVLOC_HAND_RIGHT]);
			cn = SwapItem(&player.InvBody[INVLOC_HAND_LEFT], &player.HoldItem);
		} else {
			NetSendCmdChItem(false, INVLOC_HAND_LEFT);
			player.InvBody[INVLOC_HAND_LEFT] = player.HoldItem;
		}
		break;
	case ILOC_UNEQUIPABLE:
		if (player.HoldItem._itype == ItemType::Gold && it == 0) {
			int ii = r - SLOTXY_INV_FIRST;
			if (player.InvGrid[ii] > 0) {
				int invIndex = player.InvGrid[ii] - 1;
				int gt = player.InvList[invIndex]._ivalue;
				int ig = player.HoldItem._ivalue + gt;
				if (ig <= MaxGold) {
					player.InvList[invIndex]._ivalue = ig;
					player._pGold += player.HoldItem._ivalue;
					SetPlrHandGoldCurs(player.InvList[invIndex]);
				} else {
					ig = MaxGold - gt;
					player._pGold += ig;
					player.HoldItem._ivalue -= ig;
					player.InvList[invIndex]._ivalue = MaxGold;
					player.InvList[invIndex]._iCurs = ICURS_GOLD_LARGE;
					// BUGFIX: incorrect values here are leftover from beta (fixed)
					cn = GetGoldCursor(player.HoldItem._ivalue);
					cn += CURSOR_FIRSTITEM;
				}
			} else {
				int invIndex = player._pNumInv;
				player.InvList[invIndex] = player.HoldItem;
				player._pNumInv++;
				player.InvGrid[ii] = player._pNumInv;
				player._pGold += player.HoldItem._ivalue;
				SetPlrHandGoldCurs(player.InvList[invIndex]);
			}
		} else {
			if (it == 0) {
				player.InvList[player._pNumInv] = player.HoldItem;
				player._pNumInv++;
				it = player._pNumInv;
			} else {
				int invIndex = it - 1;
				if (player.HoldItem._itype == ItemType::Gold)
					player._pGold += player.HoldItem._ivalue;
				cn = SwapItem(&player.InvList[invIndex], &player.HoldItem);
				if (player.HoldItem._itype == ItemType::Gold)
					player._pGold = CalculateGold(player);
				for (auto &itemId : player.InvGrid) {
					if (itemId == it)
						itemId = 0;
					if (itemId == -it)
						itemId = 0;
				}
			}
			int ii = r - SLOTXY_INV_FIRST;

			// Calculate top-left position of item for InvGrid and then add item to InvGrid

			int xx = std::max(ii % INV_ROW_SLOT_SIZE - ((itemSize.width - 1) / 2), 0);
			int yy = std::max(INV_ROW_SLOT_SIZE * (ii / INV_ROW_SLOT_SIZE - ((itemSize.height - 1) / 2)), 0);
			AddItemToInvGrid(player, xx + yy, it, itemSize);
		}
		break;
	case ILOC_BELT: {
		int ii = r - SLOTXY_BELT_FIRST;
		if (player.SpdList[ii].isEmpty()) {
			player.SpdList[ii] = player.HoldItem;
		} else {
			cn = SwapItem(&player.SpdList[ii], &player.HoldItem);
			if (player.HoldItem._itype == ItemType::Gold)
				player._pGold = CalculateGold(player);
		}
		drawsbarflag = true;
	} break;
	case ILOC_NONE:
	case ILOC_INVALID:
		break;
	}
	CalcPlrInv(player, true);
	if (pnum == MyPlayerId) {
		if (cn == CURSOR_HAND && !IsHardwareCursor())
			SetCursorPos(MousePosition + Displacement(cursSize / 2));
		NewCursor(cn);
	}
}

void CheckInvCut(int pnum, Point cursorPosition, bool automaticMove, bool dropItem)
{
	auto &player = Players[pnum];

	if (player._pmode > PM_WALK3) {
		return;
	}

	if (dropGoldFlag) {
		CloseGoldDrop();
		dropGoldValue = 0;
	}

	bool done = false;

	uint32_t r = 0;
	for (; r < NUM_XY_SLOTS; r++) {
		int xo = RightPanel.position.x;
		int yo = RightPanel.position.y;
		if (r >= SLOTXY_BELT_FIRST) {
			xo = PANEL_LEFT;
			yo = PANEL_TOP;
		}

		// check which inventory rectangle the mouse is in, if any
		if (cursorPosition.x >= InvRect[r].x + xo
		    && cursorPosition.x < InvRect[r].x + xo + (InventorySlotSizeInPixels.width + 1)
		    && cursorPosition.y >= InvRect[r].y + yo - (InventorySlotSizeInPixels.height + 1)
		    && cursorPosition.y < InvRect[r].y + yo) {
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

	Item &headItem = player.InvBody[INVLOC_HEAD];
	if (r >= SLOTXY_HEAD_FIRST && r <= SLOTXY_HEAD_LAST && !headItem.isEmpty()) {
		holdItem = headItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(player, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			NetSendCmdDelItem(false, INVLOC_HEAD);
			headItem._itype = ItemType::None;
		}
	}

	Item &leftRingItem = player.InvBody[INVLOC_RING_LEFT];
	if (r == SLOTXY_RING_LEFT && !leftRingItem.isEmpty()) {
		holdItem = leftRingItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(player, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			NetSendCmdDelItem(false, INVLOC_RING_LEFT);
			leftRingItem._itype = ItemType::None;
		}
	}

	Item &rightRingItem = player.InvBody[INVLOC_RING_RIGHT];
	if (r == SLOTXY_RING_RIGHT && !rightRingItem.isEmpty()) {
		holdItem = rightRingItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(player, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			NetSendCmdDelItem(false, INVLOC_RING_RIGHT);
			rightRingItem._itype = ItemType::None;
		}
	}

	Item &amuletItem = player.InvBody[INVLOC_AMULET];
	if (r == SLOTXY_AMULET && !amuletItem.isEmpty()) {
		holdItem = amuletItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(player, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			NetSendCmdDelItem(false, INVLOC_AMULET);
			amuletItem._itype = ItemType::None;
		}
	}

	Item &leftHandItem = player.InvBody[INVLOC_HAND_LEFT];
	if (r >= SLOTXY_HAND_LEFT_FIRST && r <= SLOTXY_HAND_LEFT_LAST && !leftHandItem.isEmpty()) {
		holdItem = leftHandItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(player, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			NetSendCmdDelItem(false, INVLOC_HAND_LEFT);
			leftHandItem._itype = ItemType::None;
		}
	}

	Item &rightHandItem = player.InvBody[INVLOC_HAND_RIGHT];
	if (r >= SLOTXY_HAND_RIGHT_FIRST && r <= SLOTXY_HAND_RIGHT_LAST && !rightHandItem.isEmpty()) {
		holdItem = rightHandItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(player, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			NetSendCmdDelItem(false, INVLOC_HAND_RIGHT);
			rightHandItem._itype = ItemType::None;
		}
	}

	Item &chestItem = player.InvBody[INVLOC_CHEST];
	if (r >= SLOTXY_CHEST_FIRST && r <= SLOTXY_CHEST_LAST && !chestItem.isEmpty()) {
		holdItem = chestItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(player, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			NetSendCmdDelItem(false, INVLOC_CHEST);
			chestItem._itype = ItemType::None;
		}
	}

	if (r >= SLOTXY_INV_FIRST && r <= SLOTXY_INV_LAST) {
		int ig = r - SLOTXY_INV_FIRST;
		int8_t ii = player.InvGrid[ig];
		if (ii != 0) {
			int iv = (ii < 0) ? -ii : ii;

			holdItem = player.InvList[iv - 1];
			if (automaticMove) {
				if (CanBePlacedOnBelt(holdItem)) {
					automaticallyMoved = AutoPlaceItemInBelt(player, holdItem, true);
				} else if (CanEquip(holdItem)) {
					/*
					 * Move the respective InvBodyItem to inventory before moving the item from inventory
					 * to InvBody with AutoEquip. AutoEquip requires the InvBody slot to be empty.
					 * First identify the correct InvBody slot and store it in invloc.
					 */
					automaticallyUnequip = true; // Switch to say "I have no room when inventory is too full"
					int invloc = NUM_INVLOC;
					switch (holdItem._iLoc) {
					case ILOC_ARMOR:
						invloc = INVLOC_CHEST;
						break;
					case ILOC_HELM:
						invloc = INVLOC_HEAD;
						break;
					case ILOC_AMULET:
						invloc = INVLOC_AMULET;
						break;
					case ILOC_ONEHAND:
						// Ensure shield is on the right, weapon on the left.
						if (player.InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Shield) {
							SwapItem(&player.InvBody[INVLOC_HAND_RIGHT], &player.InvBody[INVLOC_HAND_LEFT]);
						}
						// User is attempting to move a weapon (left hand)
						if (player.InvList[iv - 1]._iClass == player.InvBody[INVLOC_HAND_LEFT]._iClass
						    && player.InvList[iv - 1]._iLoc == player.InvBody[INVLOC_HAND_LEFT]._iLoc) {
							invloc = INVLOC_HAND_LEFT;
						}
						// User is attempting to move a shield (right hand)
						if (player.InvList[iv - 1]._iClass == player.InvBody[INVLOC_HAND_RIGHT]._iClass
						    && player.InvList[iv - 1]._iLoc == player.InvBody[INVLOC_HAND_RIGHT]._iLoc) {
							invloc = INVLOC_HAND_RIGHT;
						}
						// A two-hand item can always be replaced with a one-hand item
						if (player.InvBody[INVLOC_HAND_LEFT]._iLoc == ILOC_TWOHAND) {
							invloc = INVLOC_HAND_LEFT;
						}
						break;
					case ILOC_TWOHAND:
						// Moving a two-hand item from inventory to InvBody requires emptying both hands
						if (!player.InvBody[INVLOC_HAND_RIGHT].isEmpty()) {
							holdItem = player.InvBody[INVLOC_HAND_RIGHT];
							if (!AutoPlaceItemInInventory(player, holdItem, true)) {
								// No space to  move right hand item to inventory, abort.
								break;
							}
							holdItem = player.InvBody[INVLOC_HAND_LEFT];
							if (!AutoPlaceItemInInventory(player, holdItem, false)) {
								// No space for left item. Move back right item to right hand and abort.
								player.InvBody[INVLOC_HAND_RIGHT] = player.InvList[player._pNumInv - 1];
								player.RemoveInvItem(player._pNumInv - 1, false);
								break;
							}
							player.InvBody[INVLOC_HAND_RIGHT]._itype = ItemType::None;
							invloc = INVLOC_HAND_LEFT;
						} else {
							invloc = INVLOC_HAND_LEFT;
						}
						break;
					default:
						automaticallyUnequip = false; // Switch to say "I can't do that"
						invloc = NUM_INVLOC;
						break;
					}
					// Empty the identified InvBody slot (invloc) and hand over to AutoEquip
					holdItem = player.InvBody[invloc];
					if (player.InvBody[invloc]._itype != ItemType::None) {
						if (invloc != NUM_INVLOC && AutoPlaceItemInInventory(player, holdItem, true)) {
							player.InvBody[invloc]._itype = ItemType::None;
						}
					}
					holdItem = player.InvList[iv - 1];
					automaticallyMoved = automaticallyEquipped = AutoEquip(pnum, holdItem);
				}
			}

			if (!automaticMove || automaticallyMoved) {
				player.RemoveInvItem(iv - 1, false);
			}
		}
	}

	if (r >= SLOTXY_BELT_FIRST) {
		Item &beltItem = player.SpdList[r - SLOTXY_BELT_FIRST];
		if (!beltItem.isEmpty()) {
			holdItem = beltItem;
			if (automaticMove) {
				automaticallyMoved = AutoPlaceItemInInventory(player, holdItem, true);
			}

			if (!automaticMove || automaticallyMoved) {
				beltItem._itype = ItemType::None;
				drawsbarflag = true;
			}
		}
	}

	if (!holdItem.isEmpty()) {
		if (holdItem._itype == ItemType::Gold) {
			player._pGold = CalculateGold(player);
		}

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

void CheckBookLevel(Player &player)
{
	if (player.HoldItem._iMiscId != IMISC_BOOK)
		return;

	player.HoldItem._iMinMag = spelldata[player.HoldItem._iSpell].sMinInt;
	int8_t spellLevel = player._pSplLvl[player.HoldItem._iSpell];
	while (spellLevel != 0) {
		player.HoldItem._iMinMag += 20 * player.HoldItem._iMinMag / 100;
		spellLevel--;
		if (player.HoldItem._iMinMag + 20 * player.HoldItem._iMinMag / 100 > 255) {
			player.HoldItem._iMinMag = -1;
			spellLevel = 0;
		}
	}
}

void CheckNaKrulNotes(Player &player)
{
	int idx = player.HoldItem.IDidx;
	_item_indexes notes[] = { IDI_NOTE1, IDI_NOTE2, IDI_NOTE3 };

	if (IsNoneOf(idx, IDI_NOTE1, IDI_NOTE2, IDI_NOTE3)) {
		return;
	}

	for (auto note : notes) {
		if (idx != note && !player.HasItem(note)) {
			return; // the player doesn't have all notes
		}
	}

	Players[MyPlayerId].Say(HeroSpeech::JustWhatIWasLookingFor, 10);

	for (auto note : notes) {
		if (idx != note) {
			player.TryRemoveInvItemById(note);
		}
	}

	int itemNum = ActiveItems[0];
	Item tmp = Items[itemNum];
	memset(&Items[itemNum], 0, sizeof(*Items));
	GetItemAttrs(Items[itemNum], IDI_FULLNOTE, 16);
	SetupItem(Items[itemNum]);
	player.HoldItem = Items[itemNum];
	Items[itemNum] = tmp;
}

void CheckQuestItem(Player &player)
{
	auto &myPlayer = Players[MyPlayerId];

	if (player.HoldItem.IDidx == IDI_OPTAMULET && Quests[Q_BLIND]._qactive == QUEST_ACTIVE)
		Quests[Q_BLIND]._qactive = QUEST_DONE;

	if (player.HoldItem.IDidx == IDI_MUSHROOM && Quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE && Quests[Q_MUSHROOM]._qvar1 == QS_MUSHSPAWNED) {
		player.Say(HeroSpeech::NowThatsOneBigMushroom, 10); // BUGFIX: Voice for this quest might be wrong in MP
		Quests[Q_MUSHROOM]._qvar1 = QS_MUSHPICKED;
	}

	if (player.HoldItem.IDidx == IDI_ANVIL && Quests[Q_ANVIL]._qactive != QUEST_NOTAVAIL) {
		if (Quests[Q_ANVIL]._qactive == QUEST_INIT) {
			Quests[Q_ANVIL]._qactive = QUEST_ACTIVE;
		}
		if (Quests[Q_ANVIL]._qlog) {
			myPlayer.Say(HeroSpeech::INeedToGetThisToGriswold, 10);
		}
	}

	if (player.HoldItem.IDidx == IDI_GLDNELIX && Quests[Q_VEIL]._qactive != QUEST_NOTAVAIL) {
		myPlayer.Say(HeroSpeech::INeedToGetThisToLachdanan, 30);
	}

	if (player.HoldItem.IDidx == IDI_ROCK && Quests[Q_ROCK]._qactive != QUEST_NOTAVAIL) {
		if (Quests[Q_ROCK]._qactive == QUEST_INIT) {
			Quests[Q_ROCK]._qactive = QUEST_ACTIVE;
		}
		if (Quests[Q_ROCK]._qlog) {
			myPlayer.Say(HeroSpeech::ThisMustBeWhatGriswoldWanted, 10);
		}
	}

	if (player.HoldItem.IDidx == IDI_ARMOFVAL && Quests[Q_BLOOD]._qactive == QUEST_ACTIVE) {
		Quests[Q_BLOOD]._qactive = QUEST_DONE;
		myPlayer.Say(HeroSpeech::MayTheSpiritOfArkaineProtectMe, 20);
	}

	if (player.HoldItem.IDidx == IDI_MAPOFDOOM) {
		Quests[Q_GRAVE]._qlog = false;
		Quests[Q_GRAVE]._qactive = QUEST_ACTIVE;
		if (Quests[Q_GRAVE]._qvar1 != 1) {
			Players[MyPlayerId].Say(HeroSpeech::UhHuh, 10);
			Quests[Q_GRAVE]._qvar1 = 1;
		}
	}

	CheckNaKrulNotes(player);
}

void OpenHive()
{
	NetSendCmd(false, CMD_OPENHIVE);
	auto &quest = Quests[Q_FARMER];
	quest._qactive = QUEST_DONE;
	if (gbIsMultiplayer)
		NetSendCmdQuest(true, quest);
}

void OpenCrypt()
{
	NetSendCmd(false, CMD_OPENCRYPT);
	auto &quest = Quests[Q_GRAVE];
	quest._qactive = QUEST_DONE;
	if (gbIsMultiplayer)
		NetSendCmdQuest(true, quest);
}

void CleanupItems(Item *item, int ii)
{
	dItem[item->position.x][item->position.y] = 0;

	if (currlevel == 21 && item->position == CornerStone.position) {
		CornerStone.item._itype = ItemType::None;
		CornerStone.item._iSelFlag = 0;
		CornerStone.item.position = { 0, 0 };
		CornerStone.item._iAnimFlag = false;
		CornerStone.item._iIdentified = false;
		CornerStone.item._iPostDraw = false;
	}

	int i = 0;
	while (i < ActiveItemCount) {
		if (ActiveItems[i] == ii) {
			DeleteItem(ActiveItems[i], i);
			i = 0;
			continue;
		}

		i++;
	}
}

bool PutItem(Player &player, Point &position)
{
	if (ActiveItemCount >= MAXITEMS)
		return false;

	Direction d = GetDirection(player.position.tile, position);

	if (position.WalkingDistance(player.position.tile) > 1) {
		position = player.position.tile + d;
	}
	if (CanPut(position))
		return true;

	position = player.position.tile + Left(d);
	if (CanPut(position))
		return true;

	position = player.position.tile + Right(d);
	if (CanPut(position))
		return true;

	std::optional<Point> itemPosition = FindClosestValidPosition(CanPut, player.position.tile, 1, 50);

	if (itemPosition) {
		position = *itemPosition;
		return true;
	}

	return false;
}

bool CanUseStaff(Item &staff, spell_id spell)
{
	return !staff.isEmpty()
	    && IsAnyOf(staff._iMiscId, IMISC_STAFF, IMISC_UNIQUE)
	    && staff._iSpell == spell
	    && staff._iCharges > 0;
}

void StartGoldDrop()
{
	initialDropGoldIndex = pcursinvitem;

	auto &myPlayer = Players[MyPlayerId];

	if (pcursinvitem <= INVITEM_INV_LAST)
		initialDropGoldValue = myPlayer.InvList[pcursinvitem - INVITEM_INV_FIRST]._ivalue;
	else
		initialDropGoldValue = myPlayer.SpdList[pcursinvitem - INVITEM_BELT_FIRST]._ivalue;

	if (talkflag)
		control_reset_talk();

	Point start = GetPanelPosition(UiPanels::Inventory, { 67, 128 });
	SDL_Rect rect = MakeSdlRect(start.x, start.y, 180, 20);
	SDL_SetTextInputRect(&rect);

	dropGoldFlag = true;
	dropGoldValue = 0;
	SDL_StartTextInput();
}

} // namespace

void FreeInvGFX()
{
	pInvCels = std::nullopt;
}

void InitInv()
{
	switch (Players[MyPlayerId]._pClass) {
	case HeroClass::Warrior:
	case HeroClass::Barbarian:
		pInvCels = LoadCel("Data\\Inv\\Inv.CEL", SPANEL_WIDTH);
		break;
	case HeroClass::Rogue:
	case HeroClass::Bard:
		pInvCels = LoadCel("Data\\Inv\\Inv_rog.CEL", SPANEL_WIDTH);
		break;
	case HeroClass::Sorcerer:
		pInvCels = LoadCel("Data\\Inv\\Inv_Sor.CEL", SPANEL_WIDTH);
		break;
	case HeroClass::Monk:
		pInvCels = LoadCel(!gbIsSpawn ? "Data\\Inv\\Inv_Sor.CEL" : "Data\\Inv\\Inv.CEL", SPANEL_WIDTH);
		break;
	}

	invflag = false;
	drawsbarflag = false;
}

void DrawInv(const Surface &out)
{
	CelDrawTo(out, GetPanelPosition(UiPanels::Inventory, { 0, 351 }), *pInvCels, 1);

	Size slotSize[] = {
		{ 2, 2 }, //head
		{ 1, 1 }, //left ring
		{ 1, 1 }, //right ring
		{ 1, 1 }, //amulet
		{ 2, 3 }, //left hand
		{ 2, 3 }, //right hand
		{ 2, 3 }, // chest
	};

	Point slotPos[] = {
		{ 133, 59 },  //head
		{ 48, 205 },  //left ring
		{ 249, 205 }, //right ring
		{ 205, 60 },  //amulet
		{ 17, 160 },  //left hand
		{ 248, 160 }, //right hand
		{ 133, 160 }, // chest
	};

	auto &myPlayer = Players[MyPlayerId];

	for (int slot = INVLOC_HEAD; slot < NUM_INVLOC; slot++) {
		if (!myPlayer.InvBody[slot].isEmpty()) {
			int screenX = slotPos[slot].x;
			int screenY = slotPos[slot].y;
			InvDrawSlotBack(out, GetPanelPosition(UiPanels::Inventory, { screenX, screenY }), { slotSize[slot].width * InventorySlotSizeInPixels.width, slotSize[slot].height * InventorySlotSizeInPixels.height });

			int frame = myPlayer.InvBody[slot]._iCurs + CURSOR_FIRSTITEM;

			auto frameSize = GetInvItemSize(frame);

			// calc item offsets for weapons smaller than 2x3 slots
			if (slot == INVLOC_HAND_LEFT) {
				screenX += frameSize.width == InventorySlotSizeInPixels.width ? INV_SLOT_HALF_SIZE_PX : 0;
				screenY += frameSize.height == (3 * InventorySlotSizeInPixels.height) ? 0 : -INV_SLOT_HALF_SIZE_PX;
			} else if (slot == INVLOC_HAND_RIGHT) {
				screenX += frameSize.width == InventorySlotSizeInPixels.width ? (INV_SLOT_HALF_SIZE_PX - 1) : 1;
				screenY += frameSize.height == (3 * InventorySlotSizeInPixels.height) ? 0 : -INV_SLOT_HALF_SIZE_PX;
			}

			const auto &cel = GetInvItemSprite(frame);
			const int celFrame = GetInvItemFrame(frame);
			const Point position = GetPanelPosition(UiPanels::Inventory, { screenX, screenY });

			if (pcursinvitem == slot) {
				CelBlitOutlineTo(out, GetOutlineColor(myPlayer.InvBody[slot], true), position, cel, celFrame, false);
			}

			CelDrawItem(myPlayer.InvBody[slot], out, position, cel, celFrame);

			if (slot == INVLOC_HAND_LEFT) {
				if (myPlayer.InvBody[slot]._iLoc == ILOC_TWOHAND) {
					if (myPlayer._pClass != HeroClass::Barbarian
					    || IsNoneOf(myPlayer.InvBody[slot]._itype, ItemType::Sword, ItemType::Mace)) {
						InvDrawSlotBack(out, GetPanelPosition(UiPanels::Inventory, slotPos[INVLOC_HAND_RIGHT]), { slotSize[INVLOC_HAND_RIGHT].width * InventorySlotSizeInPixels.width, slotSize[INVLOC_HAND_RIGHT].height * InventorySlotSizeInPixels.height });
						LightTableIndex = 0;
						cel_transparency_active = true;

						const int dstX = RightPanel.position.x + slotPos[INVLOC_HAND_RIGHT].x + (frameSize.width == InventorySlotSizeInPixels.width ? INV_SLOT_HALF_SIZE_PX : 0) - 1;
						const int dstY = RightPanel.position.y + slotPos[INVLOC_HAND_RIGHT].y;
						CelClippedBlitLightTransTo(out, { dstX, dstY }, cel, celFrame);

						cel_transparency_active = false;
					}
				}
			}
		}
	}

	for (int i = 0; i < NUM_INV_GRID_ELEM; i++) {
		if (myPlayer.InvGrid[i] != 0) {
			InvDrawSlotBack(
			    out,
			    GetPanelPosition(UiPanels::Inventory, InvRect[i + SLOTXY_INV_FIRST]) + Displacement { 0, -1 },
			    InventorySlotSizeInPixels);
		}
	}

	for (int j = 0; j < NUM_INV_GRID_ELEM; j++) {
		if (myPlayer.InvGrid[j] > 0) { // first slot of an item
			int ii = myPlayer.InvGrid[j] - 1;
			int frame = myPlayer.InvList[ii]._iCurs + CURSOR_FIRSTITEM;

			const auto &cel = GetInvItemSprite(frame);
			const int celFrame = GetInvItemFrame(frame);
			const Point position = GetPanelPosition(UiPanels::Inventory, InvRect[j + SLOTXY_INV_FIRST]) + Displacement { 0, -1 };
			if (pcursinvitem == ii + INVITEM_INV_FIRST) {
				CelBlitOutlineTo(
				    out,
				    GetOutlineColor(myPlayer.InvList[ii], true),
				    position,
				    cel, celFrame, false);
			}

			CelDrawItem(
			    myPlayer.InvList[ii],
			    out,
			    position,
			    cel, celFrame);
		}
	}
}

void DrawInvBelt(const Surface &out)
{
	if (talkflag) {
		return;
	}

	DrawPanelBox(out, { 205, 21, 232, 28 }, { PANEL_X + 205, PANEL_Y + 5 });

	auto &myPlayer = Players[MyPlayerId];

	for (int i = 0; i < MAXBELTITEMS; i++) {
		if (myPlayer.SpdList[i].isEmpty()) {
			continue;
		}

		const Point position { InvRect[i + SLOTXY_BELT_FIRST].x + PANEL_X, InvRect[i + SLOTXY_BELT_FIRST].y + PANEL_Y - 1 };
		InvDrawSlotBack(out, position, InventorySlotSizeInPixels);
		int frame = myPlayer.SpdList[i]._iCurs + CURSOR_FIRSTITEM;

		const auto &cel = GetInvItemSprite(frame);
		const int celFrame = GetInvItemFrame(frame);

		if (pcursinvitem == i + INVITEM_BELT_FIRST) {
			if (!sgbControllerActive || invflag) {
				CelBlitOutlineTo(out, GetOutlineColor(myPlayer.SpdList[i], true), position, cel, celFrame, false);
			}
		}

		CelDrawItem(myPlayer.SpdList[i], out, position, cel, celFrame);

		if (AllItemsList[myPlayer.SpdList[i].IDidx].iUsable
		    && myPlayer.SpdList[i]._itype != ItemType::Gold) {
			snprintf(tempstr, sizeof(tempstr) / sizeof(*tempstr), "%i", i + 1);
			DrawString(out, tempstr, { position - Displacement { 0, 12 }, InventorySlotSizeInPixels }, UiFlags::ColorWhite | UiFlags::AlignRight);
		}
	}
}

bool AutoPlaceItemInBelt(Player &player, const Item &item, bool persistItem)
{
	if (!CanBePlacedOnBelt(item)) {
		return false;
	}

	for (auto &beltItem : player.SpdList) {
		if (beltItem.isEmpty()) {
			if (persistItem) {
				beltItem = item;
				player.CalcScrolls();
				drawsbarflag = true;
			}

			return true;
		}
	}

	return false;
}

bool AutoEquip(int playerId, const Item &item, bool persistItem)
{
	if (!CanEquip(item)) {
		return false;
	}

	for (int bodyLocation = INVLOC_HEAD; bodyLocation < NUM_INVLOC; bodyLocation++) {
		if (AutoEquip(playerId, item, (inv_body_loc)bodyLocation, persistItem)) {
			return true;
		}
	}

	return false;
}

bool AutoEquipEnabled(const Player &player, const Item &item)
{
	if (item.isWeapon()) {
		// Monk can use unarmed attack as an encouraged option, thus we do not automatically equip weapons on him so as to not
		// annoy players who prefer that playstyle.
		return player._pClass != HeroClass::Monk && sgOptions.Gameplay.bAutoEquipWeapons;
	}

	if (item.isArmor()) {
		return sgOptions.Gameplay.bAutoEquipArmor;
	}

	if (item.isHelm()) {
		return sgOptions.Gameplay.bAutoEquipHelms;
	}

	if (item.isShield()) {
		return sgOptions.Gameplay.bAutoEquipShields;
	}

	if (item.isJewelry()) {
		return sgOptions.Gameplay.bAutoEquipJewelry;
	}

	return true;
}

bool AutoPlaceItemInInventory(Player &player, const Item &item, bool persistItem)
{
	Size itemSize = GetInventorySize(item);

	if (itemSize.height == 1) {
		for (int i = 30; i <= 39; i++) {
			if (AutoPlaceItemInInventorySlot(player, i, item, persistItem))
				return true;
		}
		for (int x = 9; x >= 0; x--) {
			for (int y = 2; y >= 0; y--) {
				if (AutoPlaceItemInInventorySlot(player, 10 * y + x, item, persistItem))
					return true;
			}
		}
		return false;
	}

	if (itemSize.height == 2) {
		for (int x = 10 - itemSize.width; x >= 0; x -= itemSize.width) {
			for (int y = 0; y < 3; y++) {
				if (AutoPlaceItemInInventorySlot(player, 10 * y + x, item, persistItem))
					return true;
			}
		}
		if (itemSize.width == 2) {
			for (int x = 7; x >= 0; x -= 2) {
				for (int y = 0; y < 3; y++) {
					if (AutoPlaceItemInInventorySlot(player, 10 * y + x, item, persistItem))
						return true;
				}
			}
		}
		return false;
	}

	if (itemSize == Size { 1, 3 }) {
		for (int i = 0; i < 20; i++) {
			if (AutoPlaceItemInInventorySlot(player, i, item, persistItem))
				return true;
		}
		return false;
	}

	if (itemSize == Size { 2, 3 }) {
		for (int i = 0; i < 9; i++) {
			if (AutoPlaceItemInInventorySlot(player, i, item, persistItem))
				return true;
		}

		for (int i = 10; i < 19; i++) {
			if (AutoPlaceItemInInventorySlot(player, i, item, persistItem))
				return true;
		}
		return false;
	}

	app_fatal("Unknown item size: %ix%i", itemSize.width, itemSize.height);
}

bool AutoPlaceItemInInventorySlot(Player &player, int slotIndex, const Item &item, bool persistItem)
{
	int yy = (slotIndex > 0) ? (10 * (slotIndex / 10)) : 0;

	Size itemSize = GetInventorySize(item);
	for (int j = 0; j < itemSize.height; j++) {
		if (yy >= NUM_INV_GRID_ELEM) {
			return false;
		}
		int xx = (slotIndex > 0) ? (slotIndex % 10) : 0;
		for (int i = 0; i < itemSize.width; i++) {
			if (xx >= 10 || player.InvGrid[xx + yy] != 0) {
				return false;
			}
			xx++;
		}
		yy += 10;
	}

	if (persistItem) {
		player.InvList[player._pNumInv] = player.HoldItem;
		player._pNumInv++;

		AddItemToInvGrid(player, slotIndex, player._pNumInv, itemSize);
		player.CalcScrolls();
	}

	return true;
}

bool GoldAutoPlace(Player &player)
{
	bool done = false;

	for (int i = 0; i < player._pNumInv && !done; i++) {
		if (player.InvList[i]._itype != ItemType::Gold)
			continue;
		if (player.InvList[i]._ivalue >= MaxGold)
			continue;

		player.InvList[i]._ivalue += player.HoldItem._ivalue;
		if (player.InvList[i]._ivalue > MaxGold) {
			player.HoldItem._ivalue = player.InvList[i]._ivalue - MaxGold;
			SetPlrHandGoldCurs(player.HoldItem);
			player.InvList[i]._ivalue = MaxGold;
			if (gbIsHellfire)
				GetPlrHandSeed(&player.HoldItem);
		} else {
			player.HoldItem._ivalue = 0;
			done = true;
		}

		SetPlrHandGoldCurs(player.InvList[i]);
		player._pGold = CalculateGold(player);
	}

	for (int i = 39; i >= 30 && !done; i--) {
		done = GoldAutoPlaceInInventorySlot(player, i);
	}
	for (int x = 9; x >= 0 && !done; x--) {
		for (int y = 2; y >= 0 && !done; y--) {
			done = GoldAutoPlaceInInventorySlot(player, 10 * y + x);
		}
	}

	return done;
}

bool GoldAutoPlaceInInventorySlot(Player &player, int slotIndex)
{
	if (player.InvGrid[slotIndex] != 0) {
		return false;
	}

	int ii = player._pNumInv;
	player.InvList[ii] = player.HoldItem;
	player._pNumInv++;
	player.InvGrid[slotIndex] = player._pNumInv;
	GetPlrHandSeed(&player.InvList[ii]);

	int gold = player.HoldItem._ivalue;
	if (gold > MaxGold) {
		gold -= MaxGold;
		player.HoldItem._ivalue = gold;
		GetPlrHandSeed(&player.HoldItem);
		player.InvList[ii]._ivalue = MaxGold;
		return false;
	}

	player.HoldItem._ivalue = 0;
	player._pGold = CalculateGold(player);
	NewCursor(CURSOR_HAND);

	return true;
}

void CheckInvSwap(Player &player, inv_body_loc bLoc, int idx, uint16_t wCI, int seed, bool bId, uint32_t dwBuff)
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

void inv_update_rem_item(Player &player, inv_body_loc iv)
{
	player.InvBody[iv]._itype = ItemType::None;

	CalcPlrInv(player, player._pmode != PM_DEATH);
}

void CheckInvItem(bool isShiftHeld, bool isCtrlHeld)
{
	if (pcurs >= CURSOR_FIRSTITEM) {
		CheckInvPaste(MyPlayerId, MousePosition);
	} else {
		CheckInvCut(MyPlayerId, MousePosition, isShiftHeld, isCtrlHeld);
	}
}

void CheckInvScrn(bool isShiftHeld, bool isCtrlHeld)
{
	if (MousePosition.x > 190 + PANEL_LEFT && MousePosition.x < 437 + PANEL_LEFT
	    && MousePosition.y > PANEL_TOP && MousePosition.y < 33 + PANEL_TOP) {
		CheckInvItem(isShiftHeld, isCtrlHeld);
	}
}

void CheckItemStats(Player &player)
{
	Item &item = player.HoldItem;

	item._iStatFlag = false;

	if (player._pStrength >= item._iMinStr
	    && player._pMagic >= item._iMinMag
	    && player._pDexterity >= item._iMinDex) {
		item._iStatFlag = true;
	}
}

void InvGetItem(int pnum, Item *item, int ii)
{
	if (dropGoldFlag) {
		CloseGoldDrop();
		dropGoldValue = 0;
	}

	if (dItem[item->position.x][item->position.y] == 0)
		return;

	auto &player = Players[pnum];

	if (MyPlayerId == pnum && pcurs >= CURSOR_FIRSTITEM)
		NetSendCmdPItem(true, CMD_SYNCPUTITEM, player.position.tile);

	item->_iCreateInfo &= ~CF_PREGEN;
	player.HoldItem = *item;
	CheckQuestItem(player);
	CheckBookLevel(player);
	CheckItemStats(player);
	bool cursorUpdated = false;
	if (player.HoldItem._itype == ItemType::Gold && GoldAutoPlace(player))
		cursorUpdated = true;
	CleanupItems(item, ii);
	pcursitem = -1;
	if (!cursorUpdated)
		NewCursor(player.HoldItem._iCurs + CURSOR_FIRSTITEM);
}

void AutoGetItem(int pnum, Item *item, int ii)
{
	bool done;
	bool autoEquipped = false;

	if (pcurs != CURSOR_HAND) {
		return;
	}

	if (dropGoldFlag) {
		CloseGoldDrop();
		dropGoldValue = 0;
	}

	if (dItem[item->position.x][item->position.y] == 0)
		return;

	auto &player = Players[pnum];

	item->_iCreateInfo &= ~CF_PREGEN;
	player.HoldItem = *item; /// BUGFIX: overwrites cursor item, allowing for belt dupe bug
	CheckQuestItem(player);
	CheckBookLevel(player);
	CheckItemStats(player);
	SetICursor(player.HoldItem._iCurs + CURSOR_FIRSTITEM);
	if (player.HoldItem._itype == ItemType::Gold) {
		done = GoldAutoPlace(player);
		if (!done) {
			item->_ivalue = player.HoldItem._ivalue;
			SetPlrHandGoldCurs(*item);
		}
	} else {
		done = AutoEquipEnabled(player, player.HoldItem) && AutoEquip(pnum, player.HoldItem);
		if (done) {
			autoEquipped = true;
		}

		if (!done) {
			done = AutoPlaceItemInBelt(player, player.HoldItem, true);
		}
		if (!done) {
			done = AutoPlaceItemInInventory(player, player.HoldItem, true);
		}
	}

	if (done) {
		if (!autoEquipped && sgOptions.Audio.bItemPickupSound && pnum == MyPlayerId) {
			PlaySFX(IS_IGRAB);
		}

		CleanupItems(&Items[ii], ii);
		return;
	}

	if (pnum == MyPlayerId) {
		player.Say(HeroSpeech::ICantCarryAnymore);
	}
	player.HoldItem = *item;
	RespawnItem(item, true);
	NetSendCmdPItem(true, CMD_RESPAWNITEM, item->position);
	player.HoldItem._itype = ItemType::None;
}

int FindGetItem(int idx, uint16_t ci, int iseed)
{
	if (ActiveItemCount <= 0)
		return -1;

	int ii;
	int i = 0;
	while (true) {
		ii = ActiveItems[i];
		if (Items[ii].IDidx == idx && Items[ii]._iSeed == iseed && Items[ii]._iCreateInfo == ci)
			break;

		i++;

		if (i >= ActiveItemCount)
			return -1;
	}

	return ii;
}

void SyncGetItem(Point position, int idx, uint16_t ci, int iseed)
{
	int ii;

	if (dItem[position.x][position.y] != 0) {
		ii = dItem[position.x][position.y] - 1;
		if (Items[ii].IDidx == idx
		    && Items[ii]._iSeed == iseed
		    && Items[ii]._iCreateInfo == ci) {
			FindGetItem(idx, ci, iseed);
		} else {
			ii = FindGetItem(idx, ci, iseed);
		}
	} else {
		ii = FindGetItem(idx, ci, iseed);
	}

	if (ii == -1)
		return;

	CleanupItems(&Items[ii], ii);
}

bool CanPut(Point position)
{
	if (dItem[position.x][position.y] != 0)
		return false;
	if (nSolidTable[dPiece[position.x][position.y]])
		return false;
	if (dObject[position.x][position.y] != 0 && Objects[abs(dObject[position.x][position.y]) - 1]._oSolidFlag)
		return false;

	if (dObject[position.x + 1][position.y + 1] != 0) {
		int8_t oi = abs(dObject[position.x + 1][position.y + 1]) - 1;
		if (Objects[oi]._oSelFlag != 0) {
			return false;
		}
	}

	if (dObject[position.x + 1][position.y] > 0 && dObject[position.x][position.y + 1] > 0) {
		if (Objects[dObject[position.x + 1][position.y] - 1]._oSelFlag != 0 && Objects[dObject[position.x][position.y + 1] - 1]._oSelFlag != 0)
			return false;
	}

	if (currlevel == 0 && dMonster[position.x][position.y] != 0)
		return false;
	if (currlevel == 0 && dMonster[position.x + 1][position.y + 1] != 0)
		return false;

	return true;
}

bool TryInvPut()
{
	if (ActiveItemCount >= MAXITEMS)
		return false;

	auto &myPlayer = Players[MyPlayerId];

	Direction dir = GetDirection(myPlayer.position.tile, cursPosition);
	if (CanPut(myPlayer.position.tile + dir)) {
		return true;
	}

	if (CanPut(myPlayer.position.tile + Left(dir))) {
		return true;
	}

	if (CanPut(myPlayer.position.tile + Right(dir))) {
		return true;
	}

	return CanPut(myPlayer.position.tile);
}

int InvPutItem(Player &player, Point position)
{
	if (player.plrlevel == 0) {
		if (player.HoldItem.IDidx == IDI_RUNEBOMB && OpensHive(position)) {
			OpenHive();
			return -1;
		}
		if (player.HoldItem.IDidx == IDI_MAPOFDOOM && OpensGrave(position)) {
			OpenCrypt();
			return -1;
		}
	}

	if (!PutItem(player, position))
		return -1;

	assert(CanPut(position));

	int ii = AllocateItem();

	dItem[position.x][position.y] = ii + 1;
	Items[ii] = player.HoldItem;
	Items[ii].position = position;
	RespawnItem(&Items[ii], true);

	if (currlevel == 21 && position == CornerStone.position) {
		CornerStone.item = Items[ii];
		InitQTextMsg(TEXT_CORNSTN);
		Quests[Q_CORNSTN]._qlog = false;
		Quests[Q_CORNSTN]._qactive = QUEST_DONE;
	}

	NewCursor(CURSOR_HAND);
	return ii;
}

int SyncPutItem(Player &player, Point position, int idx, uint16_t icreateinfo, int iseed, int id, int dur, int mdur, int ch, int mch, int ivalue, uint32_t ibuff, int toHit, int maxDam, int minStr, int minMag, int minDex, int ac)
{
	if (player.plrlevel == 0) {
		if (idx == IDI_RUNEBOMB && OpensHive(position))
			return -1;
		if (idx == IDI_MAPOFDOOM && OpensGrave(position))
			return -1;
	}

	if (!PutItem(player, position))
		return -1;

	assert(CanPut(position));

	int ii = AllocateItem();
	auto &item = Items[ii];

	dItem[position.x][position.y] = ii + 1;

	if (idx == IDI_EAR) {
		RecreateEar(item, icreateinfo, iseed, id, dur, mdur, ch, mch, ivalue, ibuff);
	} else {
		RecreateItem(item, idx, icreateinfo, iseed, ivalue, (ibuff & CF_HELLFIRE) != 0);
		if (id != 0)
			item._iIdentified = true;
		item._iDurability = dur;
		item._iMaxDur = mdur;
		item._iCharges = ch;
		item._iMaxCharges = mch;
		item._iPLToHit = toHit;
		item._iMaxDam = maxDam;
		item._iMinStr = minStr;
		item._iMinMag = minMag;
		item._iMinDex = minDex;
		item._iAC = ac;
		item.dwBuff = ibuff;
	}

	item.position = position;
	RespawnItem(&Items[ii], true);

	if (currlevel == 21 && position == CornerStone.position) {
		CornerStone.item = Items[ii];
		InitQTextMsg(TEXT_CORNSTN);
		Quests[Q_CORNSTN]._qlog = false;
		Quests[Q_CORNSTN]._qactive = QUEST_DONE;
	}
	return ii;
}

int8_t CheckInvHLight()
{
	int8_t r = 0;
	for (; r < NUM_XY_SLOTS; r++) {
		int xo = RightPanel.position.x;
		int yo = RightPanel.position.y;
		if (r >= SLOTXY_BELT_FIRST) {
			xo = PANEL_LEFT;
			yo = PANEL_TOP;
		}

		if (MousePosition.x >= InvRect[r].x + xo
		    && MousePosition.x < InvRect[r].x + xo + (InventorySlotSizeInPixels.width + 1)
		    && MousePosition.y >= InvRect[r].y + yo - (InventorySlotSizeInPixels.height + 1)
		    && MousePosition.y < InvRect[r].y + yo) {
			break;
		}
	}

	if (r >= NUM_XY_SLOTS)
		return -1;

	int8_t rv = -1;
	InfoColor = UiFlags::ColorWhite;
	Item *pi = nullptr;
	auto &myPlayer = Players[MyPlayerId];

	ClearPanel();
	if (r >= SLOTXY_HEAD_FIRST && r <= SLOTXY_HEAD_LAST) {
		rv = INVLOC_HEAD;
		pi = &myPlayer.InvBody[rv];
	} else if (r == SLOTXY_RING_LEFT) {
		rv = INVLOC_RING_LEFT;
		pi = &myPlayer.InvBody[rv];
	} else if (r == SLOTXY_RING_RIGHT) {
		rv = INVLOC_RING_RIGHT;
		pi = &myPlayer.InvBody[rv];
	} else if (r == SLOTXY_AMULET) {
		rv = INVLOC_AMULET;
		pi = &myPlayer.InvBody[rv];
	} else if (r >= SLOTXY_HAND_LEFT_FIRST && r <= SLOTXY_HAND_LEFT_LAST) {
		rv = INVLOC_HAND_LEFT;
		pi = &myPlayer.InvBody[rv];
	} else if (r >= SLOTXY_HAND_RIGHT_FIRST && r <= SLOTXY_HAND_RIGHT_LAST) {
		pi = &myPlayer.InvBody[INVLOC_HAND_LEFT];
		if (pi->isEmpty() || pi->_iLoc != ILOC_TWOHAND
		    || (myPlayer._pClass == HeroClass::Barbarian && IsAnyOf(myPlayer.InvBody[INVLOC_HAND_LEFT]._itype, ItemType::Sword, ItemType::Mace))) {
			rv = INVLOC_HAND_RIGHT;
			pi = &myPlayer.InvBody[rv];
		} else {
			rv = INVLOC_HAND_LEFT;
		}
	} else if (r >= SLOTXY_CHEST_FIRST && r <= SLOTXY_CHEST_LAST) {
		rv = INVLOC_CHEST;
		pi = &myPlayer.InvBody[rv];
	} else if (r >= SLOTXY_INV_FIRST && r <= SLOTXY_INV_LAST) {
		int8_t itemId = abs(myPlayer.InvGrid[r - SLOTXY_INV_FIRST]);
		if (itemId == 0)
			return -1;
		int ii = itemId - 1;
		rv = ii + INVITEM_INV_FIRST;
		pi = &myPlayer.InvList[ii];
	} else if (r >= SLOTXY_BELT_FIRST) {
		r -= SLOTXY_BELT_FIRST;
		drawsbarflag = true;
		pi = &myPlayer.SpdList[r];
		if (pi->isEmpty())
			return -1;
		rv = r + INVITEM_BELT_FIRST;
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

void RemoveScroll(Player &player)
{
	const spell_id spellId = player._pSpell;
	const auto isCurrentSpell = [spellId](const Item &item) {
		return item.IsScrollOf(spellId);
	};
	{
		const InventoryPlayerItemsRange items { player };
		const auto scrollIt = std::find_if(items.begin(), items.end(), isCurrentSpell);
		if (scrollIt != items.end()) {
			player.RemoveInvItem(static_cast<int>(scrollIt.Index()));
			return;
		}
	}
	{
		const BeltPlayerItemsRange items { player };
		const auto scrollIt = std::find_if(items.begin(), items.end(), isCurrentSpell);
		if (scrollIt != items.end()) {
			player.RemoveSpdBarItem(static_cast<int>(scrollIt.Index()));
			return;
		}
	}
}

bool UseScroll()
{
	if (pcurs != CURSOR_HAND)
		return false;

	Player &myPlayer = Players[MyPlayerId];
	const spell_id spellId = myPlayer._pRSpell;

	if (leveltype == DTYPE_TOWN && !spelldata[spellId].sTownSpell)
		return false;

	const InventoryAndBeltPlayerItemsRange items { myPlayer };
	return std::any_of(items.begin(), items.end(), [spellId](const Item &item) {
		return item.IsScrollOf(spellId);
	});
}

void UseStaffCharge(Player &player)
{
	auto &staff = player.InvBody[INVLOC_HAND_LEFT];

	if (!CanUseStaff(staff, player._pSpell))
		return;

	staff._iCharges--;
	CalcPlrStaff(player);
}

bool UseStaff()
{
	if (pcurs != CURSOR_HAND) {
		return false;
	}

	auto &myPlayer = Players[MyPlayerId];

	return CanUseStaff(myPlayer.InvBody[INVLOC_HAND_LEFT], myPlayer._pRSpell);
}

bool UseInvItem(int pnum, int cii)
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
	if (cii < INVITEM_INV_FIRST)
		return false;

	bool speedlist = false;
	if (cii <= INVITEM_INV_LAST) {
		c = cii - INVITEM_INV_FIRST;
		item = &player.InvList[c];
	} else {
		if (talkflag)
			return true;
		c = cii - INVITEM_BELT_FIRST;

		item = &player.SpdList[c];
		speedlist = true;

		//If selected speedlist item exists in InvList, use the InvList item.
		for (int i = 0; i < player._pNumInv && sgOptions.Gameplay.bAutoRefillBelt; i++) {
			if (player.InvList[i]._iMiscId == item->_iMiscId && player.InvList[i]._iSpell == item->_iSpell) {
				c = i;
				item = &player.InvList[c];
				speedlist = false;
				break;
			}
		}
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

	if (player.plrlevel == 0) {
		if (UseItemOpensHive(*item, player.position.tile)) {
			OpenHive();
			player.RemoveInvItem(c);
			return true;
		}
		if (UseItemOpensCrypt(*item, player.position.tile)) {
			OpenCrypt();
			player.RemoveInvItem(c);
			return true;
		}
	}

	if (!AllItemsList[item->IDidx].iUsable)
		return false;

	if (!item->_iStatFlag) {
		player.Say(HeroSpeech::ICantUseThisYet);
		return true;
	}

	if (item->_iMiscId == IMISC_NONE && item->_itype == ItemType::Gold) {
		StartGoldDrop();
		return true;
	}

	if (dropGoldFlag) {
		CloseGoldDrop();
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

	if (speedlist) {
		if (player.SpdList[c]._iMiscId == IMISC_NOTE) {
			InitQTextMsg(TEXT_BOOK9);
			invflag = false;
			return true;
		}
		player.RemoveSpdBarItem(c);
		return true;
	}
	if (player.InvList[c]._iMiscId == IMISC_MAPOFDOOM)
		return true;
	if (player.InvList[c]._iMiscId == IMISC_NOTE) {
		InitQTextMsg(TEXT_BOOK9);
		invflag = false;
		return true;
	}
	player.RemoveInvItem(c);

	return true;
}

void DoTelekinesis()
{
	if (pcursobj != -1)
		NetSendCmdParam1(true, CMD_OPOBJT, pcursobj);
	if (pcursitem != -1)
		NetSendCmdGItem(true, CMD_REQUESTAGITEM, MyPlayerId, MyPlayerId, pcursitem);
	if (pcursmonst != -1) {
		auto &monter = Monsters[pcursmonst];
		if (!M_Talker(monter) && monter.mtalkmsg == TEXT_NONE)
			NetSendCmdParam1(true, CMD_KNOCKBACK, pcursmonst);
	}
	NewCursor(CURSOR_HAND);
}

int CalculateGold(Player &player)
{
	int gold = 0;

	for (int i = 0; i < player._pNumInv; i++) {
		if (player.InvList[i]._itype == ItemType::Gold)
			gold += player.InvList[i]._ivalue;
	}

	return gold;
}

bool DropItemBeforeTrig()
{
	if (!TryInvPut()) {
		return false;
	}

	NetSendCmdPItem(true, CMD_PUTITEM, cursPosition);
	NewCursor(CURSOR_HAND);
	return true;
}

} // namespace devilution
