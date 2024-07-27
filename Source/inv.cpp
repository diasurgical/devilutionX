/**
 * @file inv.cpp
 *
 * Implementation of player inventory.
 */
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <optional>
#include <utility>

#include <fmt/format.h>

#include "DiabloUI/ui_flags.hpp"
#include "controls/plrctrls.h"
#include "cursor.h"
#include "engine/backbuffer_state.hpp"
#include "engine/clx_sprite.hpp"
#include "engine/load_cel.hpp"
#include "engine/render/clx_render.hpp"
#include "engine/render/text_render.hpp"
#include "engine/size.hpp"
#include "hwcursor.hpp"
#include "inv_iterators.hpp"
#include "levels/town.h"
#include "minitext.h"
#include "options.h"
#include "panels/ui_panels.hpp"
#include "plrmsg.h"
#include "qol/stash.h"
#include "stores.h"
#include "towners.h"
#include "utils/format_int.hpp"
#include "utils/language.h"
#include "utils/sdl_geometry.h"
#include "utils/str_cat.hpp"
#include "utils/utf8.hpp"

namespace devilution {

bool invflag;

/**
 * Maps from inventory slot to screen position. The inventory slots are
 * arranged as follows:
 *
 * @code{.unparsed}
 *                          00 00
 *                          00 00   03
 *
 *              04 04       06 06       05 05
 *              04 04       06 06       05 05
 *              04 04       06 06       05 05
 *
 *                 01                   02
 *
 *              07 08 09 10 11 12 13 14 15 16
 *              17 18 19 20 21 22 23 24 25 26
 *              27 28 29 30 31 32 33 34 35 36
 *              37 38 39 40 41 42 43 44 45 46
 *
 * 47 48 49 50 51 52 53 54
 * @endcode
 */
const Rectangle InvRect[] = {
	// clang-format off
	//{   X,   Y }, {  W,  H }
	{ { 132,   2 }, { 58, 59 } }, // helmet
	{ {  47, 177 }, { 28, 29 } }, // left ring
	{ { 248, 177 }, { 28, 29 } }, // right ring
	{ { 205,  32 }, { 28, 29 } }, // amulet
	{ {  17,  75 }, { 58, 86 } }, // left hand
	{ { 248,  75 }, { 58, 87 } }, // right hand
	{ { 132,  75 }, { 58, 87 } }, // chest
	{ {  17, 222 }, { 29, 29 } }, // inv row 1
	{ {  46, 222 }, { 29, 29 } }, // inv row 1
	{ {  75, 222 }, { 29, 29 } }, // inv row 1
	{ { 104, 222 }, { 29, 29 } }, // inv row 1
	{ { 133, 222 }, { 29, 29 } }, // inv row 1
	{ { 162, 222 }, { 29, 29 } }, // inv row 1
	{ { 191, 222 }, { 29, 29 } }, // inv row 1
	{ { 220, 222 }, { 29, 29 } }, // inv row 1
	{ { 249, 222 }, { 29, 29 } }, // inv row 1
	{ { 278, 222 }, { 29, 29 } }, // inv row 1
	{ {  17, 251 }, { 29, 29 } }, // inv row 2
	{ {  46, 251 }, { 29, 29 } }, // inv row 2
	{ {  75, 251 }, { 29, 29 } }, // inv row 2
	{ { 104, 251 }, { 29, 29 } }, // inv row 2
	{ { 133, 251 }, { 29, 29 } }, // inv row 2
	{ { 162, 251 }, { 29, 29 } }, // inv row 2
	{ { 191, 251 }, { 29, 29 } }, // inv row 2
	{ { 220, 251 }, { 29, 29 } }, // inv row 2
	{ { 249, 251 }, { 29, 29 } }, // inv row 2
	{ { 278, 251 }, { 29, 29 } }, // inv row 2
	{ {  17, 280 }, { 29, 29 } }, // inv row 3
	{ {  46, 280 }, { 29, 29 } }, // inv row 3
	{ {  75, 280 }, { 29, 29 } }, // inv row 3
	{ { 104, 280 }, { 29, 29 } }, // inv row 3
	{ { 133, 280 }, { 29, 29 } }, // inv row 3
	{ { 162, 280 }, { 29, 29 } }, // inv row 3
	{ { 191, 280 }, { 29, 29 } }, // inv row 3
	{ { 220, 280 }, { 29, 29 } }, // inv row 3
	{ { 249, 280 }, { 29, 29 } }, // inv row 3
	{ { 278, 280 }, { 29, 29 } }, // inv row 3
	{ {  17, 309 }, { 29, 29 } }, // inv row 4
	{ {  46, 309 }, { 29, 29 } }, // inv row 4
	{ {  75, 309 }, { 29, 29 } }, // inv row 4
	{ { 104, 309 }, { 29, 29 } }, // inv row 4
	{ { 133, 309 }, { 29, 29 } }, // inv row 4
	{ { 162, 309 }, { 29, 29 } }, // inv row 4
	{ { 191, 309 }, { 29, 29 } }, // inv row 4
	{ { 220, 309 }, { 29, 29 } }, // inv row 4
	{ { 249, 309 }, { 29, 29 } }, // inv row 4
	{ { 278, 309 }, { 29, 29 } }, // inv row 4
	{ { 205,   5 }, { 29, 29 } }, // belt
	{ { 234,   5 }, { 29, 29 } }, // belt
	{ { 263,   5 }, { 29, 29 } }, // belt
	{ { 292,   5 }, { 29, 29 } }, // belt
	{ { 321,   5 }, { 29, 29 } }, // belt
	{ { 350,   5 }, { 29, 29 } }, // belt
	{ { 379,   5 }, { 29, 29 } }, // belt
	{ { 408,   5 }, { 29, 29 } }  // belt
	// clang-format on
};

namespace {

OptionalOwnedClxSpriteList pInvCels;

/**
 * @brief Adds an item to a player's inventoryGrid array
 * @param player The player reference
 * @param invGridIndex Item's position in inventoryGrid (this should be the item's topleft grid tile)
 * @param invListIndex The item's inventorySlot index (it's expected this already has +1 added to it since inventoryGrid can't store a 0 index)
 * @param itemSize Size of item
 */
void AddItemToinventoryGrid(Player &player, int invGridIndex, int invListIndex, Size itemSize, bool sendNetworkMessage)
{
	const int pitch = 10;
	for (int y = 0; y < itemSize.height; y++) {
		int rowGridIndex = invGridIndex + pitch * y;
		for (int x = 0; x < itemSize.width; x++) {
			if (x == 0 && y == itemSize.height - 1)
				player.inventoryGrid[rowGridIndex + x] = invListIndex;
			else
				player.inventoryGrid[rowGridIndex + x] = -invListIndex;
		}
	}

	if (sendNetworkMessage) {
		NetSendCmdChInvItem(false, invGridIndex);
	}
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
	if (!CanEquip(item) || IsNoneOf(player.GetItemLocation(item), ILOC_ONEHAND, ILOC_TWOHAND))
		return false;

	Item &leftHandItem = player.bodySlot[INVLOC_HAND_LEFT];
	Item &rightHandItem = player.bodySlot[INVLOC_HAND_RIGHT];

	if (leftHandItem.isEmpty() && rightHandItem.isEmpty()) {
		return true;
	}

	if (!leftHandItem.isEmpty() && !rightHandItem.isEmpty()) {
		return false;
	}

	Item &occupiedHand = !leftHandItem.isEmpty() ? leftHandItem : rightHandItem;

	// Bard can dual wield swords and maces, so we allow equiping one-handed weapons in her free slot as long as her occupied
	// slot is another one-handed weapon.
	if (player.heroClass == HeroClass::Bard) {
		bool occupiedHandIsOneHandedSwordOrMace = player.GetItemLocation(occupiedHand) == ILOC_ONEHAND
		    && IsAnyOf(occupiedHand._itype, ItemType::Sword, ItemType::Mace);

		bool weaponToEquipIsOneHandedSwordOrMace = player.GetItemLocation(item) == ILOC_ONEHAND
		    && IsAnyOf(item._itype, ItemType::Sword, ItemType::Mace);

		if (occupiedHandIsOneHandedSwordOrMace && weaponToEquipIsOneHandedSwordOrMace) {
			return true;
		}
	}

	return player.GetItemLocation(item) == ILOC_ONEHAND
	    && player.GetItemLocation(occupiedHand) == ILOC_ONEHAND
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
	if (!CanEquip(item) || player.mode > PM_WALK_SIDEWAYS || !player.bodySlot[bodyLocation].isEmpty()) {
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

void ChangeEquipment(Player &player, inv_body_loc bodyLocation, const Item &item, bool sendNetworkMessage)
{
	player.bodySlot[bodyLocation] = item;

	if (sendNetworkMessage) {
		NetSendCmdChItem(false, bodyLocation, true);
	}
}

bool AutoEquip(Player &player, const Item &item, inv_body_loc bodyLocation, bool persistItem, bool sendNetworkMessage)
{
	if (!CanEquip(player, item, bodyLocation)) {
		return false;
	}

	if (persistItem) {
		ChangeEquipment(player, bodyLocation, item, sendNetworkMessage);

		if (sendNetworkMessage && *sgOptions.Audio.autoEquipSound) {
			PlaySFX(ItemInvSnds[ItemCAnimTbl[item._iCurs]]);
		}

		CalcPlrInv(player, true);
	}

	return true;
}

int FindTargetSlotUnderItemCursor(Point cursorPosition, Size itemSize)
{
	Displacement panelOffset = Point { 0, 0 } - GetRightPanel().position;
	for (int r = SLOTXY_EQUIPPED_FIRST; r <= SLOTXY_EQUIPPED_LAST; r++) {
		if (InvRect[r].contains(cursorPosition + panelOffset))
			return r;
	}
	for (int r = SLOTXY_INV_FIRST; r <= SLOTXY_INV_LAST; r++) {
		if (InvRect[r].contains(cursorPosition + panelOffset)) {
			// When trying to paste into the inventory we need to determine the top left cell of the nearest area that could fit the item, not the slot under the center/hot pixel.
			if (itemSize.height <= 1 && itemSize.width <= 1) {
				// top left cell of a 1x1 item is the same cell as the hot pixel, no work to do
				return r;
			}
			// Otherwise work out how far the central cell is from the top-left cell
			Displacement hotPixelCellOffset = { (itemSize.width - 1) / 2, (itemSize.height - 1) / 2 };
			// For even dimension items we need to work out if the cursor is in the left/right (or top/bottom) half of the central cell and adjust the offset so the item lands in the area most covered by the cursor.
			if (itemSize.width % 2 == 0 && InvRect[r].contains(cursorPosition + panelOffset + Displacement { INV_SLOT_HALF_SIZE_PX, 0 })) {
				// hot pixel was in the left half of the cell, so we want to increase the offset to preference the column to the left
				hotPixelCellOffset.deltaX++;
			}
			if (itemSize.height % 2 == 0 && InvRect[r].contains(cursorPosition + panelOffset + Displacement { 0, INV_SLOT_HALF_SIZE_PX })) {
				// hot pixel was in the top half of the cell, so we want to increase the offset to preference the row above
				hotPixelCellOffset.deltaY++;
			}
			// Then work out the top left cell of the nearest area that could fit this item (as pasting on the edge of the inventory would otherwise put it out of bounds)
			int hotPixelCell = r - SLOTXY_INV_FIRST;
			int targetRow = std::clamp((hotPixelCell / InventorySizeInSlots.width) - hotPixelCellOffset.deltaY, 0, InventorySizeInSlots.height - itemSize.height);
			int targetColumn = std::clamp((hotPixelCell % InventorySizeInSlots.width) - hotPixelCellOffset.deltaX, 0, InventorySizeInSlots.width - itemSize.width);
			return SLOTXY_INV_FIRST + targetRow * InventorySizeInSlots.width + targetColumn;
		}
	}

	panelOffset = Point { 0, 0 } - GetMainPanel().position;
	for (int r = SLOTXY_BELT_FIRST; r <= SLOTXY_BELT_LAST; r++) {
		if (InvRect[r].contains(cursorPosition + panelOffset))
			return r;
	}
	return NUM_XY_SLOTS;
}

void ChangeBodyEquipment(Player &player, int slot, item_equip_type location)
{
	const inv_body_loc bodyLocation = [&slot](item_equip_type location) {
		switch (location) {
		case ILOC_HELM:
			return INVLOC_HEAD;
		case ILOC_RING:
			return (slot == SLOTXY_RING_LEFT ? INVLOC_RING_LEFT : INVLOC_RING_RIGHT);
		case ILOC_AMULET:
			return INVLOC_AMULET;
		case ILOC_ARMOR:
			return INVLOC_CHEST;
		default:
			app_fatal("Unexpected equipment type");
		}
	}(location);
	const Item previouslyEquippedItem = player.bodySlot[slot];
	ChangeEquipment(player, bodyLocation, player.heldItem.pop(), &player == MyPlayer);
	if (!previouslyEquippedItem.isEmpty()) {
		player.heldItem = previouslyEquippedItem;
	}
}

void ChangeEquippedItem(Player &player, uint8_t slot)
{
	const inv_body_loc selectedHand = slot == SLOTXY_HAND_LEFT ? INVLOC_HAND_LEFT : INVLOC_HAND_RIGHT;
	const inv_body_loc otherHand = slot == SLOTXY_HAND_LEFT ? INVLOC_HAND_RIGHT : INVLOC_HAND_LEFT;

	const bool pasteIntoSelectedHand = (player.bodySlot[otherHand].isEmpty() || player.bodySlot[otherHand]._iClass != player.heldItem._iClass)
	    || (player.heroClass == HeroClass::Bard && player.bodySlot[otherHand]._iClass == ICLASS_WEAPON && player.heldItem._iClass == ICLASS_WEAPON);

	const bool dequipTwoHandedWeapon = (!player.bodySlot[otherHand].isEmpty() && player.GetItemLocation(player.bodySlot[otherHand]) == ILOC_TWOHAND);

	const inv_body_loc pasteHand = pasteIntoSelectedHand ? selectedHand : otherHand;
	const Item previouslyEquippedItem = dequipTwoHandedWeapon ? player.bodySlot[otherHand] : player.bodySlot[pasteHand];
	if (dequipTwoHandedWeapon) {
		RemoveEquipment(player, otherHand, false);
	}
	ChangeEquipment(player, pasteHand, player.heldItem.pop(), &player == MyPlayer);
	if (!previouslyEquippedItem.isEmpty()) {
		player.heldItem = previouslyEquippedItem;
	}
}

void ChangeTwoHandItem(Player &player)
{
	if (!player.bodySlot[INVLOC_HAND_LEFT].isEmpty() && !player.bodySlot[INVLOC_HAND_RIGHT].isEmpty()) {
		inv_body_loc locationToUnequip = INVLOC_HAND_LEFT;
		if (player.bodySlot[INVLOC_HAND_RIGHT]._itype == ItemType::Shield) {
			locationToUnequip = INVLOC_HAND_RIGHT;
		}
		if (!AutoPlaceItemInInventory(player, player.bodySlot[locationToUnequip], true)) {
			return;
		}

		if (locationToUnequip == INVLOC_HAND_RIGHT) {
			RemoveEquipment(player, INVLOC_HAND_RIGHT, false);
		} else {
			player.bodySlot[INVLOC_HAND_LEFT].clear();
		}
	}

	if (player.bodySlot[INVLOC_HAND_RIGHT].isEmpty()) {
		Item previouslyEquippedItem = player.bodySlot[INVLOC_HAND_LEFT];
		ChangeEquipment(player, INVLOC_HAND_LEFT, player.heldItem.pop(), &player == MyPlayer);
		if (!previouslyEquippedItem.isEmpty()) {
			player.heldItem = previouslyEquippedItem;
		}
	} else {
		Item previouslyEquippedItem = player.bodySlot[INVLOC_HAND_RIGHT];
		RemoveEquipment(player, INVLOC_HAND_RIGHT, false);
		ChangeEquipment(player, INVLOC_HAND_LEFT, player.heldItem, &player == MyPlayer);
		player.heldItem = previouslyEquippedItem;
	}
}

int8_t CheckOverlappingItems(int slot, const Player &player, Size itemSize)
{
	// check that the item we're pasting only overlaps one other item (or is going into empty space)
	const unsigned originCell = static_cast<unsigned>(slot - SLOTXY_INV_FIRST);

	int8_t overlappingId = 0;
	for (unsigned rowOffset = 0; rowOffset < static_cast<unsigned>(itemSize.height * InventorySizeInSlots.width); rowOffset += InventorySizeInSlots.width) {

		for (unsigned columnOffset = 0; columnOffset < static_cast<unsigned>(itemSize.width); columnOffset++) {
			unsigned testCell = originCell + rowOffset + columnOffset;
			// FindTargetSlotUnderItemCursor returns the top left slot of the inventory region that fits the item, we can be confident this calculation is not going to read out of range.
			assert(testCell < sizeof(player.inventoryGrid));
			if (player.inventoryGrid[testCell] != 0) {
				int8_t iv = std::abs(player.inventoryGrid[testCell]);
				if (overlappingId != 0) {
					if (overlappingId != iv) {
						// Found two different items that would be displaced by the held item, can't paste the item here.
						return -1;
					}
				} else {
					overlappingId = iv;
				}
			}
		}
	}

	return overlappingId;
}

int8_t GetPrevItemId(int slot, const Player &player, const Size &itemSize)
{
	if (player.heldItem._itype != ItemType::Gold)
		return CheckOverlappingItems(slot, player, itemSize);
	int8_t item_cell_begin = player.inventoryGrid[slot - SLOTXY_INV_FIRST];
	if (item_cell_begin == 0)
		return 0;
	if (item_cell_begin <= 0)
		return -item_cell_begin;
	if (player.inventorySlot[item_cell_begin - 1]._itype != ItemType::Gold)
		return item_cell_begin;
	return 0;
}

bool ChangeInvItem(Player &player, int slot, Size itemSize)
{
	int8_t prevItemId = GetPrevItemId(slot, player, itemSize);
	if (prevItemId < 0) return false;

	if (player.heldItem._itype == ItemType::Gold && prevItemId == 0) {
		const int ii = slot - SLOTXY_INV_FIRST;
		if (player.inventoryGrid[ii] > 0) {
			const int invIndex = player.inventoryGrid[ii] - 1;
			const int gt = player.inventorySlot[invIndex]._ivalue;
			int ig = player.heldItem._ivalue + gt;
			if (ig <= MaxGold) {
				player.inventorySlot[invIndex]._ivalue = ig;
				SetPlrHandGoldCurs(player.inventorySlot[invIndex]);
				player.gold += player.heldItem._ivalue;
				player.heldItem.clear();
			} else {
				ig = MaxGold - gt;
				player.gold += ig;
				player.heldItem._ivalue -= ig;
				SetPlrHandGoldCurs(player.heldItem);
				player.inventorySlot[invIndex]._ivalue = MaxGold;
				player.inventorySlot[invIndex]._iCurs = ICURS_GOLD_LARGE;
			}
		} else {
			const int invIndex = player.numInventoryItems;
			player.gold += player.heldItem._ivalue;
			player.inventorySlot[invIndex] = player.heldItem.pop();
			player.numInventoryItems++;
			player.inventoryGrid[ii] = player.numInventoryItems;
		}
		if (&player == MyPlayer) {
			NetSendCmdChInvItem(false, ii);
		}
	} else {
		if (prevItemId == 0) {
			player.inventorySlot[player.numInventoryItems] = player.heldItem.pop();
			player.numInventoryItems++;
			prevItemId = player.numInventoryItems;
		} else {
			const int invIndex = prevItemId - 1;
			if (player.heldItem._itype == ItemType::Gold)
				player.gold += player.heldItem._ivalue;
			std::swap(player.inventorySlot[invIndex], player.heldItem);
			if (player.heldItem._itype == ItemType::Gold)
				player.gold = CalculateGold(player);
			for (int8_t &itemIndex : player.inventoryGrid) {
				if (itemIndex == prevItemId)
					itemIndex = 0;
				if (itemIndex == -prevItemId)
					itemIndex = 0;
			}
		}

		AddItemToinventoryGrid(player, slot - SLOTXY_INV_FIRST, prevItemId, itemSize, &player == MyPlayer);
	}

	return true;
}

void ChangeBeltItem(Player &player, int slot)
{
	const int ii = slot - SLOTXY_BELT_FIRST;
	if (player.beltSlot[ii].isEmpty()) {
		player.beltSlot[ii] = player.heldItem.pop();
	} else {
		std::swap(player.beltSlot[ii], player.heldItem);

		if (player.heldItem._itype == ItemType::Gold)
			player.gold = CalculateGold(player);
	}
	if (&player == MyPlayer) {
		NetSendCmdChBeltItem(false, ii);
	}
	RedrawComponent(PanelDrawComponent::Belt);
}

item_equip_type GetItemEquipType(const Player &player, int slot, item_equip_type desiredLocation)
{
	if (slot == SLOTXY_HEAD)
		return ILOC_HELM;
	if (slot == SLOTXY_RING_LEFT || slot == SLOTXY_RING_RIGHT)
		return ILOC_RING;
	if (slot == SLOTXY_AMULET)
		return ILOC_AMULET;
	if (slot == SLOTXY_HAND_LEFT || slot == SLOTXY_HAND_RIGHT) {
		if (desiredLocation == ILOC_TWOHAND)
			return ILOC_TWOHAND;
		return ILOC_ONEHAND;
	}
	if (slot == SLOTXY_CHEST)
		return ILOC_ARMOR;
	if (slot >= SLOTXY_BELT_FIRST && slot <= SLOTXY_BELT_LAST)
		return ILOC_BELT;

	return ILOC_UNEQUIPABLE;
}

void CheckInvPaste(Player &player, Point cursorPosition)
{
	const Size itemSize = GetInventorySize(player.heldItem);

	const int slot = FindTargetSlotUnderItemCursor(cursorPosition, itemSize);
	if (slot == NUM_XY_SLOTS)
		return;

	const item_equip_type desiredLocation = player.GetItemLocation(player.heldItem);
	const item_equip_type location = GetItemEquipType(player, slot, desiredLocation);

	if (location == ILOC_BELT) {
		if (!CanBePlacedOnBelt(player, player.heldItem)) return;
	} else if (location != ILOC_UNEQUIPABLE) {
		if (desiredLocation != location) return;
	}

	if (IsNoneOf(location, ILOC_UNEQUIPABLE, ILOC_BELT)) {
		if (!player.CanUseItem(player.heldItem)) {
			player.Say(HeroSpeech::ICantUseThisYet);
			return;
		}
		if (player.mode > PM_WALK_SIDEWAYS)
			return;
	}

	// Select the parameters that go into
	// ChangeEquipment and add it to post switch
	switch (location) {
	case ILOC_HELM:
	case ILOC_RING:
	case ILOC_AMULET:
	case ILOC_ARMOR:
		ChangeBodyEquipment(player, slot, location);
		break;
	case ILOC_ONEHAND:
		ChangeEquippedItem(player, slot);
		break;
	case ILOC_TWOHAND:
		ChangeTwoHandItem(player);
		break;
	case ILOC_UNEQUIPABLE:
		if (!ChangeInvItem(player, slot, itemSize)) return;
		break;
	case ILOC_BELT:
		ChangeBeltItem(player, slot);
		break;
	case ILOC_NONE:
	case ILOC_INVALID:
		break;
	}

	CalcPlrInv(player, true);
	if (&player == MyPlayer) {
		PlaySFX(ItemInvSnds[ItemCAnimTbl[player.heldItem._iCurs]]);
		NewCursor(player.heldItem);
	}
}

void CheckInvCut(Player &player, Point cursorPosition, bool automaticMove, bool dropItem)
{
	if (player.mode > PM_WALK_SIDEWAYS) {
		return;
	}

	CloseGoldDrop();

	uint32_t r = 0;
	for (; r < NUM_XY_SLOTS; r++) {
		int xo = GetRightPanel().position.x;
		int yo = GetRightPanel().position.y;
		if (r >= SLOTXY_BELT_FIRST) {
			xo = GetMainPanel().position.x;
			yo = GetMainPanel().position.y;
		}

		// check which inventory rectangle the mouse is in, if any
		if (InvRect[r].contains(cursorPosition - Displacement(xo, yo))) {
			break;
		}
	}

	if (r == NUM_XY_SLOTS) {
		// not on an inventory slot rectangle
		return;
	}

	Item &holdItem = player.heldItem;
	holdItem.clear();

	bool automaticallyMoved = false;
	bool automaticallyEquipped = false;
	bool automaticallyUnequip = false;

	Item &headItem = player.bodySlot[INVLOC_HEAD];
	if (r == SLOTXY_HEAD && !headItem.isEmpty()) {
		holdItem = headItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(player, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			RemoveEquipment(player, INVLOC_HEAD, false);
		}
	}

	Item &leftRingItem = player.bodySlot[INVLOC_RING_LEFT];
	if (r == SLOTXY_RING_LEFT && !leftRingItem.isEmpty()) {
		holdItem = leftRingItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(player, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			RemoveEquipment(player, INVLOC_RING_LEFT, false);
		}
	}

	Item &rightRingItem = player.bodySlot[INVLOC_RING_RIGHT];
	if (r == SLOTXY_RING_RIGHT && !rightRingItem.isEmpty()) {
		holdItem = rightRingItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(player, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			RemoveEquipment(player, INVLOC_RING_RIGHT, false);
		}
	}

	Item &amuletItem = player.bodySlot[INVLOC_AMULET];
	if (r == SLOTXY_AMULET && !amuletItem.isEmpty()) {
		holdItem = amuletItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(player, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			RemoveEquipment(player, INVLOC_AMULET, false);
		}
	}

	Item &leftHandItem = player.bodySlot[INVLOC_HAND_LEFT];
	if (r == SLOTXY_HAND_LEFT && !leftHandItem.isEmpty()) {
		holdItem = leftHandItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(player, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			RemoveEquipment(player, INVLOC_HAND_LEFT, false);
		}
	}

	Item &rightHandItem = player.bodySlot[INVLOC_HAND_RIGHT];
	if (r == SLOTXY_HAND_RIGHT && !rightHandItem.isEmpty()) {
		holdItem = rightHandItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(player, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			RemoveEquipment(player, INVLOC_HAND_RIGHT, false);
		}
	}

	Item &chestItem = player.bodySlot[INVLOC_CHEST];
	if (r == SLOTXY_CHEST && !chestItem.isEmpty()) {
		holdItem = chestItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(player, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			RemoveEquipment(player, INVLOC_CHEST, false);
		}
	}

	if (r >= SLOTXY_INV_FIRST && r <= SLOTXY_INV_LAST) {
		int ig = r - SLOTXY_INV_FIRST;
		int8_t ii = player.inventoryGrid[ig];
		if (ii != 0) {
			int iv = (ii < 0) ? -ii : ii;

			holdItem = player.inventorySlot[iv - 1];
			if (automaticMove) {
				if (CanBePlacedOnBelt(player, holdItem)) {
					automaticallyMoved = AutoPlaceItemInBelt(player, holdItem, true, &player == MyPlayer);
				} else if (CanEquip(holdItem)) {
					/*
					 * Move the respective bodySlotItem to inventory before moving the item from inventory
					 * to bodySlot with AutoEquip. AutoEquip requires the bodySlot slot to be empty.
					 * First identify the correct bodySlot slot and store it in invloc.
					 */
					automaticallyUnequip = true; // Switch to say "I have no room when inventory is too full"
					int invloc = NUM_INVLOC;
					switch (player.GetItemLocation(holdItem)) {
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
						// User is attempting to move a weapon (left hand)
						if (player.inventorySlot[iv - 1]._iClass == player.bodySlot[INVLOC_HAND_LEFT]._iClass
						    && player.GetItemLocation(player.inventorySlot[iv - 1]) == player.GetItemLocation(player.bodySlot[INVLOC_HAND_LEFT])) {
							invloc = INVLOC_HAND_LEFT;
						}
						// User is attempting to move a shield (right hand)
						if (player.inventorySlot[iv - 1]._iClass == player.bodySlot[INVLOC_HAND_RIGHT]._iClass
						    && player.GetItemLocation(player.inventorySlot[iv - 1]) == player.GetItemLocation(player.bodySlot[INVLOC_HAND_RIGHT])) {
							invloc = INVLOC_HAND_RIGHT;
						}
						// A two-hand item can always be replaced with a one-hand item
						if (player.GetItemLocation(player.bodySlot[INVLOC_HAND_LEFT]) == ILOC_TWOHAND) {
							invloc = INVLOC_HAND_LEFT;
						}
						break;
					case ILOC_TWOHAND:
						// Moving a two-hand item from inventory to bodySlot requires emptying both hands
						if (!player.bodySlot[INVLOC_HAND_RIGHT].isEmpty()) {
							holdItem = player.bodySlot[INVLOC_HAND_RIGHT];
							if (!AutoPlaceItemInInventory(player, holdItem, true)) {
								// No space to  move right hand item to inventory, abort.
								break;
							}
							holdItem = player.bodySlot[INVLOC_HAND_LEFT];
							if (!AutoPlaceItemInInventory(player, holdItem, false)) {
								// No space for left item. Move back right item to right hand and abort.
								player.bodySlot[INVLOC_HAND_RIGHT] = player.inventorySlot[player.numInventoryItems - 1];
								player.RemoveInvItem(player.numInventoryItems - 1, false);
								break;
							}
							RemoveEquipment(player, INVLOC_HAND_RIGHT, false);
							invloc = INVLOC_HAND_LEFT;
						} else {
							invloc = INVLOC_HAND_LEFT;
						}
						break;
					default:
						automaticallyUnequip = false; // Switch to say "I can't do that"
						break;
					}
					// Empty the identified bodySlot slot (invloc) and hand over to AutoEquip
					if (invloc != NUM_INVLOC) {
						holdItem = player.bodySlot[invloc];
						if (player.bodySlot[invloc]._itype != ItemType::None) {
							if (AutoPlaceItemInInventory(player, holdItem, true)) {
								player.bodySlot[invloc].clear();
							}
						}
					}
					holdItem = player.inventorySlot[iv - 1];
					automaticallyMoved = automaticallyEquipped = AutoEquip(player, holdItem, true, &player == MyPlayer);
				}
			}

			if (!automaticMove || automaticallyMoved) {
				player.RemoveInvItem(iv - 1, false);
			}
		}
	}

	if (r >= SLOTXY_BELT_FIRST) {
		Item &beltItem = player.beltSlot[r - SLOTXY_BELT_FIRST];
		if (!beltItem.isEmpty()) {
			holdItem = beltItem;
			if (automaticMove) {
				automaticallyMoved = AutoPlaceItemInInventory(player, holdItem, true);
			}

			if (!automaticMove || automaticallyMoved) {
				player.RemoveSpdBarItem(r - SLOTXY_BELT_FIRST);
			}
		}
	}

	if (!holdItem.isEmpty()) {
		if (holdItem._itype == ItemType::Gold) {
			player.gold = CalculateGold(player);
		}

		CalcPlrInv(player, true);
		holdItem._iStatFlag = player.CanUseItem(holdItem);

		if (&player == MyPlayer) {
			if (automaticallyEquipped) {
				PlaySFX(ItemInvSnds[ItemCAnimTbl[holdItem._iCurs]]);
			} else if (!automaticMove || automaticallyMoved) {
				PlaySFX(SfxID::GrabItem);
			}

			if (automaticMove) {
				if (!automaticallyMoved) {
					if (CanBePlacedOnBelt(player, holdItem) || automaticallyUnequip) {
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

	if (dropItem && !holdItem.isEmpty()) {
		TryDropItem();
	}
}

void TryCombineNaKrulNotes(Player &player, Item &noteItem)
{
	int idx = noteItem.IDidx;
	_item_indexes notes[] = { IDI_NOTE1, IDI_NOTE2, IDI_NOTE3 };

	if (IsNoneOf(idx, IDI_NOTE1, IDI_NOTE2, IDI_NOTE3)) {
		return;
	}

	for (_item_indexes note : notes) {
		if (idx != note && !HasInventoryItemWithId(player, note)) {
			return; // the player doesn't have all notes
		}
	}

	MyPlayer->Say(HeroSpeech::JustWhatIWasLookingFor, 10);

	for (_item_indexes note : notes) {
		if (idx != note) {
			RemoveInventoryItemById(player, note);
		}
	}

	Point position = noteItem.position; // copy the position to restore it after re-initialising the item
	noteItem = {};
	GetItemAttrs(noteItem, IDI_FULLNOTE, 16);
	SetupItem(noteItem);
	noteItem.position = position; // this ensures CleanupItem removes the entry in the dropped items lookup table
}

void CheckQuestItem(Player &player, Item &questItem)
{
	Player &myPlayer = *MyPlayer;

	if (Quests[Q_BLIND]._qactive == QUEST_ACTIVE
	    && (questItem.IDidx == IDI_OPTAMULET
	        || (Quests[Q_BLIND].IsAvailable() && questItem.position == (SetPiece.position.megaToWorld() + Displacement { 5, 5 })))) {
		Quests[Q_BLIND]._qactive = QUEST_DONE;
		NetSendCmdQuest(true, Quests[Q_BLIND]);
	}

	if (questItem.IDidx == IDI_MUSHROOM && Quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE && Quests[Q_MUSHROOM]._qvar1 == QS_MUSHSPAWNED) {
		player.Say(HeroSpeech::NowThatsOneBigMushroom, 10); // BUGFIX: Voice for this quest might be wrong in MP
		Quests[Q_MUSHROOM]._qvar1 = QS_MUSHPICKED;
		NetSendCmdQuest(true, Quests[Q_MUSHROOM]);
	}

	if (questItem.IDidx == IDI_ANVIL && Quests[Q_ANVIL]._qactive != QUEST_NOTAVAIL) {
		if (Quests[Q_ANVIL]._qactive == QUEST_INIT) {
			Quests[Q_ANVIL]._qactive = QUEST_ACTIVE;
			NetSendCmdQuest(true, Quests[Q_ANVIL]);
		}
		if (Quests[Q_ANVIL]._qlog) {
			myPlayer.Say(HeroSpeech::INeedToGetThisToGriswold, 10);
		}
	}

	if (questItem.IDidx == IDI_GLDNELIX && Quests[Q_VEIL]._qactive != QUEST_NOTAVAIL) {
		myPlayer.Say(HeroSpeech::INeedToGetThisToLachdanan, 30);
	}

	if (questItem.IDidx == IDI_ROCK && Quests[Q_ROCK]._qactive != QUEST_NOTAVAIL) {
		if (Quests[Q_ROCK]._qactive == QUEST_INIT) {
			Quests[Q_ROCK]._qactive = QUEST_ACTIVE;
			NetSendCmdQuest(true, Quests[Q_ROCK]);
		}
		if (Quests[Q_ROCK]._qlog) {
			myPlayer.Say(HeroSpeech::ThisMustBeWhatGriswoldWanted, 10);
		}
	}

	if (Quests[Q_BLOOD]._qactive == QUEST_ACTIVE
	    && (questItem.IDidx == IDI_ARMOFVAL
	        || (Quests[Q_BLOOD].IsAvailable() && questItem.position == (SetPiece.position.megaToWorld() + Displacement { 9, 3 })))) {
		Quests[Q_BLOOD]._qactive = QUEST_DONE;
		NetSendCmdQuest(true, Quests[Q_BLOOD]);
		myPlayer.Say(HeroSpeech::MayTheSpiritOfArkaineProtectMe, 20);
	}

	if (questItem.IDidx == IDI_MAPOFDOOM) {
		Quests[Q_GRAVE]._qactive = QUEST_ACTIVE;
		if (Quests[Q_GRAVE]._qvar1 != 1) {
			MyPlayer->Say(HeroSpeech::UhHuh, 10);
			Quests[Q_GRAVE]._qvar1 = 1;
		}
	}

	TryCombineNaKrulNotes(player, questItem);
}

void CleanupItems(int ii)
{
	Item &item = Items[ii];
	dItem[item.position.x][item.position.y] = 0;

	if (CornerStone.isAvailable() && item.position == CornerStone.position) {
		CornerStone.item.clear();
		CornerStone.item._iSelFlag = 0;
		CornerStone.item.position = { 0, 0 };
		CornerStone.item._iAnimFlag = false;
		CornerStone.item._iIdentified = false;
		CornerStone.item._iPostDraw = false;
	}

	int i = 0;
	while (i < ActiveItemCount) {
		if (ActiveItems[i] == ii) {
			DeleteItem(i);
			i = 0;
			continue;
		}

		i++;
	}
}

bool CanUseStaff(Item &staff, SpellID spell)
{
	return !staff.isEmpty()
	    && IsAnyOf(staff._iMiscId, IMISC_STAFF, IMISC_UNIQUE)
	    && staff._iSpell == spell
	    && staff._iCharges > 0;
}

void StartGoldDrop()
{
	CloseGoldWithdraw();

	const int8_t invIndex = pcursinvitem;

	const Player &myPlayer = *MyPlayer;

	const int max = (invIndex <= INVITEM_INV_LAST)
	    ? myPlayer.inventorySlot[invIndex - INVITEM_INV_FIRST]._ivalue
	    : myPlayer.beltSlot[invIndex - INVITEM_BELT_FIRST]._ivalue;

	if (talkflag)
		control_reset_talk();

	Point start = GetPanelPosition(UiPanels::Inventory, { 67, 128 });
	SDL_Rect rect = MakeSdlRect(start.x, start.y, 180, 20);
	SDL_SetTextInputRect(&rect);

	OpenGoldDrop(invIndex, max);
}

int CreateGoldItemInInventorySlot(Player &player, int slotIndex, int value)
{
	if (player.inventoryGrid[slotIndex] != 0) {
		return value;
	}

	Item &goldItem = player.inventorySlot[player.numInventoryItems];
	MakeGoldStack(goldItem, std::min(value, MaxGold));
	player.numInventoryItems++;
	player.inventoryGrid[slotIndex] = player.numInventoryItems;
	if (&player == MyPlayer) {
		NetSendCmdChInvItem(false, slotIndex);
	}

	value -= goldItem._ivalue;

	return value;
}

} // namespace

void InvDrawSlotBack(const Surface &out, Point targetPosition, Size size, item_quality itemQuality)
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
			if (pix >= PAL16_GRAY) {
				switch (itemQuality) {
				case ITEM_QUALITY_MAGIC:
					pix -= PAL16_GRAY - (!IsInspectingPlayer() ? PAL16_BLUE : PAL16_ORANGE) - 1;
					break;
				case ITEM_QUALITY_UNIQUE:
					pix -= PAL16_GRAY - (!IsInspectingPlayer() ? PAL16_YELLOW : PAL16_ORANGE) - 1;
					break;
				default:
					pix -= PAL16_GRAY - (!IsInspectingPlayer() ? PAL16_BEIGE : PAL16_ORANGE) - 1;
					break;
				}
			}
			*dst++ = pix;
		}
	}
}

bool CanBePlacedOnBelt(const Player &player, const Item &item)
{
	return FitsInBeltSlot(item)
	    && item._itype != ItemType::Gold
	    && player.CanUseItem(item)
	    && item.isUsable();
}

void FreeInvGFX()
{
	pInvCels = std::nullopt;
}

void InitInv()
{
	switch (MyPlayer->heroClass) {
	case HeroClass::Warrior:
	case HeroClass::Barbarian:
		pInvCels = LoadCel("data\\inv\\inv", static_cast<uint16_t>(SidePanelSize.width));
		break;
	case HeroClass::Rogue:
	case HeroClass::Bard:
		pInvCels = LoadCel("data\\inv\\inv_rog", static_cast<uint16_t>(SidePanelSize.width));
		break;
	case HeroClass::Sorcerer:
		pInvCels = LoadCel("data\\inv\\inv_sor", static_cast<uint16_t>(SidePanelSize.width));
		break;
	case HeroClass::Monk:
		pInvCels = LoadCel(!gbIsSpawn ? "data\\inv\\inv_sor" : "data\\inv\\inv", static_cast<uint16_t>(SidePanelSize.width));
		break;
	}
}

void DrawInv(const Surface &out)
{
	ClxDraw(out, GetPanelPosition(UiPanels::Inventory, { 0, 351 }), (*pInvCels)[0]);

	Size slotSize[] = {
		{ 2, 2 }, // head
		{ 1, 1 }, // left ring
		{ 1, 1 }, // right ring
		{ 1, 1 }, // amulet
		{ 2, 3 }, // left hand
		{ 2, 3 }, // right hand
		{ 2, 3 }, // chest
	};

	Point slotPos[] = {
		{ 133, 59 },  // head
		{ 48, 205 },  // left ring
		{ 249, 205 }, // right ring
		{ 205, 60 },  // amulet
		{ 17, 160 },  // left hand
		{ 248, 160 }, // right hand
		{ 133, 160 }, // chest
	};

	Player &myPlayer = *InspectPlayer;

	for (int slot = INVLOC_HEAD; slot < NUM_INVLOC; slot++) {
		if (!myPlayer.bodySlot[slot].isEmpty()) {
			int screenX = slotPos[slot].x;
			int screenY = slotPos[slot].y;
			InvDrawSlotBack(out, GetPanelPosition(UiPanels::Inventory, { screenX, screenY }), { slotSize[slot].width * InventorySlotSizeInPixels.width, slotSize[slot].height * InventorySlotSizeInPixels.height }, myPlayer.bodySlot[slot]._iMagical);

			const int cursId = myPlayer.bodySlot[slot]._iCurs + CURSOR_FIRSTITEM;

			Size frameSize = GetInvItemSize(cursId);

			// calc item offsets for weapons/armor smaller than 2x3 slots
			if (IsAnyOf(slot, INVLOC_HAND_LEFT, INVLOC_HAND_RIGHT, INVLOC_CHEST)) {
				screenX += frameSize.width == InventorySlotSizeInPixels.width ? INV_SLOT_HALF_SIZE_PX : 0;
				screenY += frameSize.height == (3 * InventorySlotSizeInPixels.height) ? 0 : -INV_SLOT_HALF_SIZE_PX;
			}

			const ClxSprite sprite = GetInvItemSprite(cursId);
			const Point position = GetPanelPosition(UiPanels::Inventory, { screenX, screenY });

			if (pcursinvitem == slot) {
				ClxDrawOutline(out, GetOutlineColor(myPlayer.bodySlot[slot], true), position, sprite);
			}

			DrawItem(myPlayer.bodySlot[slot], out, position, sprite);

			if (slot == INVLOC_HAND_LEFT) {
				if (myPlayer.GetItemLocation(myPlayer.bodySlot[slot]) == ILOC_TWOHAND) {
					InvDrawSlotBack(out, GetPanelPosition(UiPanels::Inventory, slotPos[INVLOC_HAND_RIGHT]), { slotSize[INVLOC_HAND_RIGHT].width * InventorySlotSizeInPixels.width, slotSize[INVLOC_HAND_RIGHT].height * InventorySlotSizeInPixels.height }, myPlayer.bodySlot[slot]._iMagical);
					LightTableIndex = 0;

					const int dstX = GetRightPanel().position.x + slotPos[INVLOC_HAND_RIGHT].x + (frameSize.width == InventorySlotSizeInPixels.width ? INV_SLOT_HALF_SIZE_PX : 0) - 1;
					const int dstY = GetRightPanel().position.y + slotPos[INVLOC_HAND_RIGHT].y;
					ClxDrawLightBlended(out, { dstX, dstY }, sprite);
				}
			}
		}
	}

	for (int i = 0; i < InventoryGridCells; i++) {
		if (myPlayer.inventoryGrid[i] != 0) {
			InvDrawSlotBack(
			    out,
			    GetPanelPosition(UiPanels::Inventory, InvRect[i + SLOTXY_INV_FIRST].position) + Displacement { 0, InventorySlotSizeInPixels.height },
			    InventorySlotSizeInPixels,
			    myPlayer.inventorySlot[std::abs(myPlayer.inventoryGrid[i]) - 1]._iMagical);
		}
	}

	for (int j = 0; j < InventoryGridCells; j++) {
		if (myPlayer.inventoryGrid[j] > 0) { // first slot of an item
			int ii = myPlayer.inventoryGrid[j] - 1;
			int cursId = myPlayer.inventorySlot[ii]._iCurs + CURSOR_FIRSTITEM;

			const ClxSprite sprite = GetInvItemSprite(cursId);
			const Point position = GetPanelPosition(UiPanels::Inventory, InvRect[j + SLOTXY_INV_FIRST].position) + Displacement { 0, InventorySlotSizeInPixels.height };
			if (pcursinvitem == ii + INVITEM_INV_FIRST) {
				ClxDrawOutline(out, GetOutlineColor(myPlayer.inventorySlot[ii], true), position, sprite);
			}

			DrawItem(myPlayer.inventorySlot[ii], out, position, sprite);
		}
	}
}

void DrawInvBelt(const Surface &out)
{
	if (talkflag) {
		return;
	}

	const Point mainPanelPosition = GetMainPanel().position;

	DrawPanelBox(out, { 205, 21, 232, 28 }, mainPanelPosition + Displacement { 205, 5 });

	Player &myPlayer = *InspectPlayer;

	for (int i = 0; i < MaxBeltItems; i++) {
		if (myPlayer.beltSlot[i].isEmpty()) {
			continue;
		}

		const Point position { InvRect[i + SLOTXY_BELT_FIRST].position.x + mainPanelPosition.x, InvRect[i + SLOTXY_BELT_FIRST].position.y + mainPanelPosition.y + InventorySlotSizeInPixels.height };
		InvDrawSlotBack(out, position, InventorySlotSizeInPixels, myPlayer.beltSlot[i]._iMagical);
		const int cursId = myPlayer.beltSlot[i]._iCurs + CURSOR_FIRSTITEM;

		const ClxSprite sprite = GetInvItemSprite(cursId);

		if (pcursinvitem == i + INVITEM_BELT_FIRST) {
			if (ControlMode == ControlTypes::KeyboardAndMouse || invflag) {
				ClxDrawOutline(out, GetOutlineColor(myPlayer.beltSlot[i], true), position, sprite);
			}
		}

		DrawItem(myPlayer.beltSlot[i], out, position, sprite);

		if (myPlayer.beltSlot[i].isUsable()
		    && myPlayer.beltSlot[i]._itype != ItemType::Gold) {
			DrawString(out, StrCat(i + 1), { position - Displacement { 0, 12 }, InventorySlotSizeInPixels },
			    { .flags = UiFlags::ColorWhite | UiFlags::AlignRight });
		}
	}
}

void RemoveEquipment(Player &player, inv_body_loc bodyLocation, bool hiPri)
{
	if (&player == MyPlayer) {
		NetSendCmdDelItem(hiPri, bodyLocation);
	}

	player.bodySlot[bodyLocation].clear();
}

bool AutoPlaceItemInBelt(Player &player, const Item &item, bool persistItem, bool sendNetworkMessage)
{
	if (!CanBePlacedOnBelt(player, item)) {
		return false;
	}

	for (Item &beltItem : player.beltSlot) {
		if (beltItem.isEmpty()) {
			if (persistItem) {
				beltItem = item;
				player.CalcScrolls();
				RedrawComponent(PanelDrawComponent::Belt);
				if (sendNetworkMessage) {
					const auto beltIndex = static_cast<int>(std::distance<const Item *>(&player.beltSlot[0], &beltItem));
					NetSendCmdChBeltItem(false, beltIndex);
				}
			}

			return true;
		}
	}

	return false;
}

bool AutoEquip(Player &player, const Item &item, bool persistItem, bool sendNetworkMessage)
{
	if (!CanEquip(item)) {
		return false;
	}

	for (int bodyLocation = INVLOC_HEAD; bodyLocation < NUM_INVLOC; bodyLocation++) {
		if (AutoEquip(player, item, (inv_body_loc)bodyLocation, persistItem, sendNetworkMessage)) {
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
		return player.heroClass != HeroClass::Monk && *sgOptions.Gameplay.autoEquipWeapons;
	}

	if (item.isArmor()) {
		return *sgOptions.Gameplay.autoEquipArmor;
	}

	if (item.isHelm()) {
		return *sgOptions.Gameplay.autoEquipHelms;
	}

	if (item.isShield()) {
		return *sgOptions.Gameplay.autoEquipShields;
	}

	if (item.isJewelry()) {
		return *sgOptions.Gameplay.autoEquipJewelry;
	}

	return true;
}

namespace {
/**
 * @brief Checks whether the given item can be placed on the specified player's inventory slot.
 * If 'persistItem' is 'True', the item is also placed in the inventory slot.
 * @param player The player whose inventory will be checked.
 * @param slotIndex The 0-based index of the slot to put the item on.
 * @param item The item to be checked.
 * @param persistItem Pass 'True' to actually place the item in the inventory slot. The default is 'False'.
 * @return 'True' in case the item can be placed on the specified player's inventory slot and 'False' otherwise.
 */
bool AutoPlaceItemInInventorySlot(Player &player, int slotIndex, const Item &item, bool persistItem, bool sendNetworkMessage)
{
	int yy = (slotIndex > 0) ? (10 * (slotIndex / 10)) : 0;

	Size itemSize = GetInventorySize(item);
	for (int j = 0; j < itemSize.height; j++) {
		if (yy >= InventoryGridCells) {
			return false;
		}
		int xx = (slotIndex > 0) ? (slotIndex % 10) : 0;
		for (int i = 0; i < itemSize.width; i++) {
			if (xx >= 10 || player.inventoryGrid[xx + yy] != 0) {
				return false;
			}
			xx++;
		}
		yy += 10;
	}

	if (persistItem) {
		player.inventorySlot[player.numInventoryItems] = item;
		player.numInventoryItems++;

		AddItemToinventoryGrid(player, slotIndex, player.numInventoryItems, itemSize, sendNetworkMessage);
		player.CalcScrolls();
	}

	return true;
}
} // namespace

bool AutoPlaceItemInInventory(Player &player, const Item &item, bool persistItem, bool sendNetworkMessage)
{
	Size itemSize = GetInventorySize(item);

	if (itemSize.height == 1) {
		for (int i = 30; i <= 39; i++) {
			if (AutoPlaceItemInInventorySlot(player, i, item, persistItem, sendNetworkMessage))
				return true;
		}
		for (int x = 9; x >= 0; x--) {
			for (int y = 2; y >= 0; y--) {
				if (AutoPlaceItemInInventorySlot(player, 10 * y + x, item, persistItem, sendNetworkMessage))
					return true;
			}
		}
		return false;
	}

	if (itemSize.height == 2) {
		for (int x = 10 - itemSize.width; x >= 0; x -= itemSize.width) {
			for (int y = 0; y < 3; y++) {
				if (AutoPlaceItemInInventorySlot(player, 10 * y + x, item, persistItem, sendNetworkMessage))
					return true;
			}
		}
		if (itemSize.width == 2) {
			for (int x = 7; x >= 0; x -= 2) {
				for (int y = 0; y < 3; y++) {
					if (AutoPlaceItemInInventorySlot(player, 10 * y + x, item, persistItem, sendNetworkMessage))
						return true;
				}
			}
		}
		return false;
	}

	if (itemSize == Size { 1, 3 }) {
		for (int i = 0; i < 20; i++) {
			if (AutoPlaceItemInInventorySlot(player, i, item, persistItem, sendNetworkMessage))
				return true;
		}
		return false;
	}

	if (itemSize == Size { 2, 3 }) {
		for (int i = 0; i < 9; i++) {
			if (AutoPlaceItemInInventorySlot(player, i, item, persistItem, sendNetworkMessage))
				return true;
		}

		for (int i = 10; i < 19; i++) {
			if (AutoPlaceItemInInventorySlot(player, i, item, persistItem, sendNetworkMessage))
				return true;
		}
		return false;
	}

	app_fatal(StrCat("Unknown item size: ", itemSize.width, "x", itemSize.height));
}

std::vector<int> SortItemsBySize(Player &player)
{
	std::vector<std::pair<Size, int>> itemSizes; // Pair of item size and its index in inventorySlot
	itemSizes.reserve(player.numInventoryItems); // Reserves space for the number of items in the player's inventory

	for (int i = 0; i < player.numInventoryItems; i++) {
		Size size = GetInventorySize(player.inventorySlot[i]);
		itemSizes.emplace_back(size, i);
	}

	// Sort items by height first, then by width
	std::sort(itemSizes.begin(), itemSizes.end(), [](const auto &a, const auto &b) {
		if (a.first.height == b.first.height) return a.first.width > b.first.width;
		return a.first.height > b.first.height;
	});

	// Extract sorted indices
	std::vector<int> sortedIndices;
	sortedIndices.reserve(itemSizes.size()); // Pre-allocate the necessary capacity

	for (const auto &itemSize : itemSizes) {
		sortedIndices.push_back(itemSize.second);
	}

	return sortedIndices;
}

void ReorganizeInventory(Player &player)
{
	// Sort items by size
	std::vector<int> sortedIndices = SortItemsBySize(player);

	// Temporary storage for items and a copy of inventoryGrid
	std::vector<Item> tempStorage(player.numInventoryItems);
	std::array<int8_t, 40> originalinventoryGrid;                                                                   // Declare an array for inventoryGrid copy
	std::copy(std::begin(player.inventoryGrid), std::end(player.inventoryGrid), std::begin(originalinventoryGrid)); // Copy inventoryGrid to originalinventoryGrid

	// Move items to temporary storage and clear inventory slots
	for (int i = 0; i < player.numInventoryItems; ++i) {
		tempStorage[i] = player.inventorySlot[i];
		player.inventorySlot[i] = {};
	}
	player.numInventoryItems = 0;                                                   // Reset inventory count
	std::fill(std::begin(player.inventoryGrid), std::end(player.inventoryGrid), 0); // Clear inventoryGrid

	// Attempt to place items back, now from the temp storage
	bool reorganizationFailed = false;
	for (int index : sortedIndices) {
		Item &item = tempStorage[index];
		if (!AutoPlaceItemInInventory(player, item, true, false)) {
			reorganizationFailed = true;
			break;
		}
	}

	// If reorganization failed, restore items and inventoryGrid from tempStorage and originalinventoryGrid
	if (reorganizationFailed) {
		for (Item &item : tempStorage) {
			if (!item.isEmpty()) {
				player.inventorySlot[player.numInventoryItems++] = item;
			}
		}
		std::copy(std::begin(originalinventoryGrid), std::end(originalinventoryGrid), std::begin(player.inventoryGrid)); // Restore inventoryGrid
	}
}

int RoomForGold()
{
	int amount = 0;
	for (int8_t &itemIndex : MyPlayer->inventoryGrid) {
		if (itemIndex < 0) {
			continue;
		}
		if (itemIndex == 0) {
			amount += MaxGold;
			continue;
		}

		Item &goldItem = MyPlayer->inventorySlot[itemIndex - 1];
		if (goldItem._itype != ItemType::Gold || goldItem._ivalue == MaxGold) {
			continue;
		}

		amount += MaxGold - goldItem._ivalue;
	}

	return amount;
}

int AddGoldToInventory(Player &player, int value)
{
	// Top off existing piles
	for (int i = 0; i < player.numInventoryItems && value > 0; i++) {
		Item &goldItem = player.inventorySlot[i];
		if (goldItem._itype != ItemType::Gold || goldItem._ivalue >= MaxGold) {
			continue;
		}

		if (goldItem._ivalue + value > MaxGold) {
			value -= MaxGold - goldItem._ivalue;
			goldItem._ivalue = MaxGold;
		} else {
			goldItem._ivalue += value;
			value = 0;
		}

		NetSyncInvItem(player, i);
		SetPlrHandGoldCurs(goldItem);
	}

	// Last row right to left
	for (int i = 39; i >= 30 && value > 0; i--) {
		value = CreateGoldItemInInventorySlot(player, i, value);
	}

	// Remaining inventory in columns, bottom to top, right to left
	for (int x = 9; x >= 0 && value > 0; x--) {
		for (int y = 2; y >= 0 && value > 0; y--) {
			value = CreateGoldItemInInventorySlot(player, 10 * y + x, value);
		}
	}

	return value;
}

bool GoldAutoPlace(Player &player, Item &goldStack)
{
	goldStack._ivalue = AddGoldToInventory(player, goldStack._ivalue);
	SetPlrHandGoldCurs(goldStack);

	player.gold = CalculateGold(player);

	return goldStack._ivalue == 0;
}

void CheckInvSwap(Player &player, inv_body_loc bLoc)
{
	Item &item = player.bodySlot[bLoc];

	if (bLoc == INVLOC_HAND_LEFT && player.GetItemLocation(item) == ILOC_TWOHAND) {
		player.bodySlot[INVLOC_HAND_RIGHT].clear();
	} else if (bLoc == INVLOC_HAND_RIGHT && player.GetItemLocation(item) == ILOC_TWOHAND) {
		player.bodySlot[INVLOC_HAND_LEFT].clear();
	}

	CalcPlrInv(player, true);
}

void inv_update_rem_item(Player &player, inv_body_loc iv)
{
	player.bodySlot[iv].clear();

	CalcPlrInv(player, player.mode != PM_DEATH);
}

void CheckInvSwap(Player &player, const Item &item, int invGridIndex)
{
	Size itemSize = GetInventorySize(item);

	const int pitch = 10;
	int invListIndex = [&]() -> int {
		for (int y = 0; y < itemSize.height; y++) {
			int rowGridIndex = invGridIndex + pitch * y;
			for (int x = 0; x < itemSize.width; x++) {
				int gridIndex = rowGridIndex + x;
				if (player.inventoryGrid[gridIndex] != 0)
					return std::abs(player.inventoryGrid[gridIndex]);
			}
		}
		player.numInventoryItems++;
		return player.numInventoryItems;
	}();

	if (invListIndex < player.numInventoryItems) {
		for (int8_t &itemIndex : player.inventoryGrid) {
			if (itemIndex == invListIndex)
				itemIndex = 0;
			if (itemIndex == -invListIndex)
				itemIndex = 0;
		}
	}

	player.inventorySlot[invListIndex - 1] = item;

	for (int y = 0; y < itemSize.height; y++) {
		int rowGridIndex = invGridIndex + pitch * y;
		for (int x = 0; x < itemSize.width; x++) {
			if (x == 0 && y == itemSize.height - 1)
				player.inventoryGrid[rowGridIndex + x] = invListIndex;
			else
				player.inventoryGrid[rowGridIndex + x] = -invListIndex;
		}
	}

	CalcPlrInv(player, true);
}

void CheckInvRemove(Player &player, int invGridIndex)
{
	int invListIndex = std::abs(player.inventoryGrid[invGridIndex]) - 1;

	if (invListIndex >= 0) {
		player.RemoveInvItem(invListIndex);
	}
}

void TransferItemToStash(Player &player, int location)
{
	if (location == -1) {
		return;
	}

	Item &item = GetInventoryItem(player, location);
	if (!AutoPlaceItemInStash(player, item, true)) {
		player.SaySpecific(HeroSpeech::WhereWouldIPutThis);
		return;
	}

	PlaySFX(ItemInvSnds[ItemCAnimTbl[item._iCurs]]);

	if (location < INVITEM_INV_FIRST) {
		RemoveEquipment(player, static_cast<inv_body_loc>(location), false);
		CalcPlrInv(player, true);
	} else if (location <= INVITEM_INV_LAST)
		player.RemoveInvItem(location - INVITEM_INV_FIRST);
	else
		player.RemoveSpdBarItem(location - INVITEM_BELT_FIRST);
}

void CheckInvItem(bool isShiftHeld, bool isCtrlHeld)
{
	if (IsInspectingPlayer())
		return;
	if (!MyPlayer->heldItem.isEmpty()) {
		CheckInvPaste(*MyPlayer, MousePosition);
	} else if (IsStashOpen && isCtrlHeld) {
		TransferItemToStash(*MyPlayer, pcursinvitem);
	} else {
		CheckInvCut(*MyPlayer, MousePosition, isShiftHeld, isCtrlHeld);
	}
}

void CheckInvScrn(bool isShiftHeld, bool isCtrlHeld)
{
	const Point mainPanelPosition = GetMainPanel().position;
	if (MousePosition.x > 190 + mainPanelPosition.x && MousePosition.x < 437 + mainPanelPosition.x
	    && MousePosition.y > mainPanelPosition.y && MousePosition.y < 33 + mainPanelPosition.y) {
		CheckInvItem(isShiftHeld, isCtrlHeld);
	}
}

void InvGetItem(Player &player, int ii)
{
	Item &item = Items[ii];
	CloseGoldDrop();

	if (dItem[item.position.x][item.position.y] == 0)
		return;

	item._iCreateInfo &= ~CF_PREGEN;
	CheckQuestItem(player, item);
	item.updateRequiredStatsCacheForPlayer(player);

	if (item._itype == ItemType::Gold && GoldAutoPlace(player, item)) {
		if (MyPlayer == &player) {
			// Non-gold items (or gold when you have a full inventory) go to the hand then provide audible feedback on
			//  paste. To give the same feedback for auto-placed gold we play the sound effect now.
			PlaySFX(SfxID::ItemGold);
		}
	} else {
		// The item needs to go into the players hand
		if (MyPlayer == &player && !player.heldItem.isEmpty()) {
			// drop whatever the player is currently holding
			NetSendCmdPItem(true, CMD_SYNCPUTITEM, player.position.tile, player.heldItem);
		}

		// need to copy here instead of move so CleanupItems still has access to the position
		player.heldItem = item;
		NewCursor(player.heldItem);
	}

	// This potentially moves items in memory so must be done after we've made a copy
	CleanupItems(ii);
	pcursitem = -1;
}

std::optional<Point> FindAdjacentPositionForItem(Point origin, Direction facing)
{
	if (ActiveItemCount >= MAXITEMS)
		return {};

	if (CanPut(origin + facing))
		return origin + facing;

	if (CanPut(origin + Left(facing)))
		return origin + Left(facing);

	if (CanPut(origin + Right(facing)))
		return origin + Right(facing);

	if (CanPut(origin + Left(Left(facing))))
		return origin + Left(Left(facing));

	if (CanPut(origin + Right(Right(facing))))
		return origin + Right(Right(facing));

	if (CanPut(origin + Left(Left(Left(facing)))))
		return origin + Left(Left(Left(facing)));

	if (CanPut(origin + Right(Right(Right(facing)))))
		return origin + Right(Right(Right(facing)));

	if (CanPut(origin + Opposite(facing)))
		return origin + Opposite(facing);

	if (CanPut(origin))
		return origin;

	return {};
}

void AutoGetItem(Player &player, Item *itemPointer, int ii)
{
	Item &item = *itemPointer;

	CloseGoldDrop();

	if (dItem[item.position.x][item.position.y] == 0)
		return;

	item._iCreateInfo &= ~CF_PREGEN;
	CheckQuestItem(player, item);
	item.updateRequiredStatsCacheForPlayer(player);

	bool done;
	bool autoEquipped = false;

	if (item._itype == ItemType::Gold) {
		done = GoldAutoPlace(player, item);
		if (!done) {
			SetPlrHandGoldCurs(item);
		}
	} else {
		done = AutoEquipEnabled(player, item) && AutoEquip(player, item, true, &player == MyPlayer);
		if (done) {
			autoEquipped = true;
		}

		if (!done) {
			done = AutoPlaceItemInBelt(player, item, true, &player == MyPlayer);
		}
		if (!done) {
			done = AutoPlaceItemInInventory(player, item, true, &player == MyPlayer);
		}
	}

	if (done) {
		if (!autoEquipped && *sgOptions.Audio.itemPickupSound && &player == MyPlayer) {
			PlaySFX(SfxID::GrabItem);
		}

		CleanupItems(ii);
		return;
	}

	if (&player == MyPlayer) {
		player.Say(HeroSpeech::ICantCarryAnymore);
	}
	RespawnItem(item, true);
	NetSendCmdPItem(true, CMD_SPAWNITEM, item.position, item);
}

int FindGetItem(uint32_t iseed, _item_indexes idx, uint16_t createInfo)
{
	for (uint8_t i = 0; i < ActiveItemCount; i++) {
		Item &item = Items[ActiveItems[i]];
		if (item.keyAttributesMatch(iseed, idx, createInfo)) {
			return i;
		}
	}

	return -1;
}

void SyncGetItem(Point position, uint32_t iseed, _item_indexes idx, uint16_t ci)
{
	// Check what the local client has at the target position
	int ii = dItem[position.x][position.y] - 1;

	if (ii >= 0 && ii < MAXITEMS) {
		// If there was an item there, check that it's the same item as the remote player has
		if (!Items[ii].keyAttributesMatch(iseed, idx, ci)) {
			// Key attributes don't match so we must've desynced, ignore this index and try find a matching item via lookup
			ii = -1;
		}
	}

	if (ii == -1) {
		// Either there's no item at the expected position or it doesn't match what is being picked up, so look for an item that matches the key attributes
		ii = FindGetItem(iseed, idx, ci);

		if (ii != -1) {
			// Translate to Items index for CleanupItems, FindGetItem returns an ActiveItems index
			ii = ActiveItems[ii];
		}
	}

	if (ii == -1) {
		// Still can't find the expected item, assume it was collected earlier and this caused the desync
		return;
	}

	CleanupItems(ii);
}

bool CanPut(Point position)
{
	if (!InDungeonBounds(position)) {
		return false;
	}

	if (IsTileSolid(position)) {
		return false;
	}

	if (dItem[position.x][position.y] != 0) {
		return false;
	}

	if (leveltype == DTYPE_TOWN) {
		if (dMonster[position.x][position.y] != 0) {
			return false;
		}
		if (dMonster[position.x + 1][position.y + 1] != 0) {
			return false;
		}
	}

	if (IsItemBlockingObjectAtPosition(position)) {
		return false;
	}

	return true;
}

int ClampDurability(const Item &item, int durability)
{
	if (item._iMaxDur == 0)
		return 0;

	return std::clamp<int>(durability, 1, item._iMaxDur);
}

int16_t ClampToHit(const Item &item, int16_t toHit)
{
	if (toHit < item._iPLToHit || toHit > 51)
		return item._iPLToHit;

	return toHit;
}

uint8_t ClampMaxDam(const Item &item, uint8_t maxDam)
{
	if (maxDam < item._iMaxDam || maxDam - item._iMinDam > 30)
		return item._iMaxDam;

	return maxDam;
}

int SyncDropItem(Point position, _item_indexes idx, uint16_t icreateinfo, int iseed, int id, int dur, int mdur, int ch, int mch, int ivalue, uint32_t ibuff, int toHit, int maxDam)
{
	if (ActiveItemCount >= MAXITEMS)
		return -1;

	Item item;

	RecreateItem(*MyPlayer, item, idx, icreateinfo, iseed, ivalue, (ibuff & CF_HELLFIRE) != 0);
	if (id != 0)
		item._iIdentified = true;
	item._iMaxDur = mdur;
	item._iDurability = ClampDurability(item, dur);
	item._iMaxCharges = std::clamp<int>(mch, 0, item._iMaxCharges);
	item._iCharges = std::clamp<int>(ch, 0, item._iMaxCharges);
	if (gbIsHellfire) {
		item._iPLToHit = ClampToHit(item, toHit);
		item._iMaxDam = ClampMaxDam(item, maxDam);
	}
	item.dwBuff = ibuff;

	return PlaceItemInWorld(std::move(item), position);
}

int SyncDropEar(Point position, uint16_t icreateinfo, uint32_t iseed, uint8_t cursval, std::string_view heroname)
{
	if (ActiveItemCount >= MAXITEMS)
		return -1;

	Item item;
	RecreateEar(item, icreateinfo, iseed, cursval, heroname);

	return PlaceItemInWorld(std::move(item), position);
}

int8_t CheckInvHLight()
{
	int8_t r = 0;
	for (; r < NUM_XY_SLOTS; r++) {
		int xo = GetRightPanel().position.x;
		int yo = GetRightPanel().position.y;
		if (r >= SLOTXY_BELT_FIRST) {
			xo = GetMainPanel().position.x;
			yo = GetMainPanel().position.y;
		}

		if (InvRect[r].contains(MousePosition - Displacement(xo, yo))) {
			break;
		}
	}

	if (r >= NUM_XY_SLOTS)
		return -1;

	int8_t rv = -1;
	InfoColor = UiFlags::ColorWhite;
	Item *pi = nullptr;
	Player &myPlayer = *InspectPlayer;

	if (r == SLOTXY_HEAD) {
		rv = INVLOC_HEAD;
		pi = &myPlayer.bodySlot[rv];
	} else if (r == SLOTXY_RING_LEFT) {
		rv = INVLOC_RING_LEFT;
		pi = &myPlayer.bodySlot[rv];
	} else if (r == SLOTXY_RING_RIGHT) {
		rv = INVLOC_RING_RIGHT;
		pi = &myPlayer.bodySlot[rv];
	} else if (r == SLOTXY_AMULET) {
		rv = INVLOC_AMULET;
		pi = &myPlayer.bodySlot[rv];
	} else if (r == SLOTXY_HAND_LEFT) {
		rv = INVLOC_HAND_LEFT;
		pi = &myPlayer.bodySlot[rv];
	} else if (r == SLOTXY_HAND_RIGHT) {
		pi = &myPlayer.bodySlot[INVLOC_HAND_LEFT];
		if (pi->isEmpty() || myPlayer.GetItemLocation(*pi) != ILOC_TWOHAND) {
			rv = INVLOC_HAND_RIGHT;
			pi = &myPlayer.bodySlot[rv];
		} else {
			rv = INVLOC_HAND_LEFT;
		}
	} else if (r == SLOTXY_CHEST) {
		rv = INVLOC_CHEST;
		pi = &myPlayer.bodySlot[rv];
	} else if (r >= SLOTXY_INV_FIRST && r <= SLOTXY_INV_LAST) {
		int8_t itemId = std::abs(myPlayer.inventoryGrid[r - SLOTXY_INV_FIRST]);
		if (itemId == 0)
			return -1;
		int ii = itemId - 1;
		rv = ii + INVITEM_INV_FIRST;
		pi = &myPlayer.inventorySlot[ii];
	} else if (r >= SLOTXY_BELT_FIRST) {
		r -= SLOTXY_BELT_FIRST;
		RedrawComponent(PanelDrawComponent::Belt);
		pi = &myPlayer.beltSlot[r];
		if (pi->isEmpty())
			return -1;
		rv = r + INVITEM_BELT_FIRST;
	}

	if (pi->isEmpty())
		return -1;

	if (pi->_itype == ItemType::Gold) {
		int nGold = pi->_ivalue;
		InfoString = fmt::format(fmt::runtime(ngettext("{:s} gold piece", "{:s} gold pieces", nGold)), FormatInteger(nGold));
	} else {
		InfoColor = pi->getTextColor();
		InfoString = pi->getName();
		if (pi->_iIdentified) {
			PrintItemDetails(*pi);
		} else {
			PrintItemDur(*pi);
		}
	}

	return rv;
}

void ConsumeScroll(Player &player)
{
	const SpellID spellId = player.executedSpell.spellId;

	const auto isCurrentSpell = [spellId](const Item &item) -> bool {
		return item.isScrollOf(spellId) || item.isRuneOf(spellId);
	};

	// Try to remove the scroll from selected inventory slot
	const int8_t itemSlot = player.executedSpell.spellFrom;
	if (itemSlot >= INVITEM_INV_FIRST && itemSlot <= INVITEM_INV_LAST) {
		const int itemIndex = itemSlot - INVITEM_INV_FIRST;
		const Item *item = &player.inventorySlot[itemIndex];
		if (!item->isEmpty() && isCurrentSpell(*item)) {
			player.RemoveInvItem(itemIndex);
			return;
		}
	} else if (itemSlot >= INVITEM_BELT_FIRST && itemSlot <= INVITEM_BELT_LAST) {
		const int itemIndex = itemSlot - INVITEM_BELT_FIRST;
		const Item *item = &player.beltSlot[itemIndex];
		if (!item->isEmpty() && isCurrentSpell(*item)) {
			player.RemoveSpdBarItem(itemIndex);
			return;
		}
	} else if (itemSlot != 0) {
		app_fatal(StrCat("ConsumeScroll: Invalid item index ", itemSlot));
	}

	// Didn't find it at the selected slot, take the first one we find
	// This path is always used when the scroll is consumed via spell selection
	RemoveInventoryOrBeltItem(player, isCurrentSpell);
}

bool CanUseScroll(Player &player, SpellID spell)
{
	if (leveltype == DTYPE_TOWN && !GetSpellData(spell).isAllowedInTown())
		return false;

	return HasInventoryOrBeltItem(player, [spell](const Item &item) {
		return item.isScrollOf(spell) || item.isRuneOf(spell);
	});
}

void ConsumeStaffCharge(Player &player)
{
	Item &staff = player.bodySlot[INVLOC_HAND_LEFT];

	if (!CanUseStaff(staff, player.executedSpell.spellId))
		return;

	staff._iCharges--;
	CalcPlrStaff(player);
}

bool CanUseStaff(Player &player, SpellID spellId)
{
	return CanUseStaff(player.bodySlot[INVLOC_HAND_LEFT], spellId);
}

Item &GetInventoryItem(Player &player, int location)
{
	if (location < INVITEM_INV_FIRST)
		return player.bodySlot[location];

	if (location <= INVITEM_INV_LAST)
		return player.inventorySlot[location - INVITEM_INV_FIRST];

	return player.beltSlot[location - INVITEM_BELT_FIRST];
}

bool UseInvItem(int cii)
{
	if (IsInspectingPlayer())
		return false;

	Player &player = *MyPlayer;

	if (player.isInvincible && player.life == 0 && &player == MyPlayer)
		return true;
	if (pcurs != CURSOR_HAND)
		return true;
	if (stextflag != TalkID::None)
		return true;
	if (cii < INVITEM_INV_FIRST)
		return false;

	bool speedlist = false;
	int c;
	Item *item;
	if (cii <= INVITEM_INV_LAST) {
		c = cii - INVITEM_INV_FIRST;
		item = &player.inventorySlot[c];
	} else {
		if (talkflag)
			return true;
		c = cii - INVITEM_BELT_FIRST;

		item = &player.beltSlot[c];
		speedlist = true;

		// If selected speedlist item exists in inventorySlot, use the inventorySlot item.
		for (int i = 0; i < player.numInventoryItems && *sgOptions.Gameplay.autoRefillBelt; i++) {
			if (player.inventorySlot[i]._iMiscId == item->_iMiscId && player.inventorySlot[i]._iSpell == item->_iSpell) {
				c = i;
				item = &player.inventorySlot[c];
				cii = c + INVITEM_INV_FIRST;
				speedlist = false;
				break;
			}
		}

		// If speedlist item is not inventory, use same item at the end of the speedlist if exists.
		if (speedlist && *sgOptions.Gameplay.autoRefillBelt) {
			for (int i = INVITEM_BELT_LAST - INVITEM_BELT_FIRST; i > c; i--) {
				Item &candidate = player.beltSlot[i];

				if (!candidate.isEmpty() && candidate._iMiscId == item->_iMiscId && candidate._iSpell == item->_iSpell) {
					c = i;
					cii = c + INVITEM_BELT_FIRST;
					item = &candidate;
					break;
				}
			}
		}
	}

	constexpr int SpeechDelay = 10;
	if (item->IDidx == IDI_MUSHROOM) {
		player.Say(HeroSpeech::NowThatsOneBigMushroom, SpeechDelay);
		return true;
	}
	if (item->IDidx == IDI_FUNGALTM) {

		PlaySFX(SfxID::ItemBook);
		player.Say(HeroSpeech::ThatDidntDoAnything, SpeechDelay);
		return true;
	}

	if (player.isOnLevel(0)) {
		if (UseItemOpensHive(*item, player.position.tile)) {
			OpenHive();
			player.RemoveInvItem(c);
			return true;
		}
		if (UseItemOpensGrave(*item, player.position.tile)) {
			OpenGrave();
			player.RemoveInvItem(c);
			return true;
		}
	}

	if (!item->isUsable())
		return false;

	if (!player.CanUseItem(*item)) {
		player.Say(HeroSpeech::ICantUseThisYet);
		return true;
	}

	if (item->_iMiscId == IMISC_NONE && item->_itype == ItemType::Gold) {
		StartGoldDrop();
		return true;
	}

	CloseGoldDrop();

	if (item->isScroll() && leveltype == DTYPE_TOWN && !GetSpellData(item->_iSpell).isAllowedInTown()) {
		return true;
	}

	if (item->_iMiscId > IMISC_RUNEFIRST && item->_iMiscId < IMISC_RUNELAST && leveltype == DTYPE_TOWN) {
		return true;
	}

	if (item->_iMiscId == IMISC_ARENAPOT && !player.isOnArenaLevel()) {
		player.Say(HeroSpeech::ThatWontWorkHere);
		return true;
	}

	int idata = ItemCAnimTbl[item->_iCurs];
	if (item->_iMiscId == IMISC_BOOK)
		PlaySFX(SfxID::ReadBook);
	else if (&player == MyPlayer)
		PlaySFX(ItemInvSnds[idata]);

	UseItem(player, item->_iMiscId, item->_iSpell, cii);

	if (speedlist) {
		if (player.beltSlot[c]._iMiscId == IMISC_NOTE) {
			InitQTextMsg(TEXT_BOOK9);
			CloseInventory();
			return true;
		}
		if (!item->isScroll() && !item->isRune())
			player.RemoveSpdBarItem(c);
		return true;
	}
	if (player.inventorySlot[c]._iMiscId == IMISC_MAPOFDOOM)
		return true;
	if (player.inventorySlot[c]._iMiscId == IMISC_NOTE) {
		InitQTextMsg(TEXT_BOOK9);
		CloseInventory();
		return true;
	}
	if (!item->isScroll() && !item->isRune())
		player.RemoveInvItem(c);

	return true;
}

void CloseInventory()
{
	CloseGoldWithdraw();
	CloseStash();
	invflag = false;
}

void CloseStash()
{
	if (!IsStashOpen)
		return;

	Player &myPlayer = *MyPlayer;
	if (!myPlayer.heldItem.isEmpty()) {
		std::optional<Point> itemTile = FindAdjacentPositionForItem(myPlayer.position.future, myPlayer.direction);
		if (itemTile) {
			NetSendCmdPItem(true, CMD_PUTITEM, *itemTile, myPlayer.heldItem);
		} else {
			if (!AutoPlaceItemInBelt(myPlayer, myPlayer.heldItem, true, true)
			    && !AutoPlaceItemInInventory(myPlayer, myPlayer.heldItem, true, true)
			    && !AutoPlaceItemInStash(myPlayer, myPlayer.heldItem, true)) {
				// This can fail for max gold, arena potions and a stash that has been arranged
				// to not have room for the item all 3 cases are extremely unlikely
				app_fatal(_("No room for item"));
			}
			PlaySFX(ItemInvSnds[ItemCAnimTbl[myPlayer.heldItem._iCurs]]);
		}
		myPlayer.heldItem.clear();
		NewCursor(CURSOR_HAND);
	}

	IsStashOpen = false;
}

void DoTelekinesis()
{
	if (ObjectUnderCursor != nullptr && !ObjectUnderCursor->IsDisabled())
		NetSendCmdLoc(MyPlayerId, true, CMD_OPOBJT, cursPosition);
	if (pcursitem != -1)
		NetSendCmdGItem(true, CMD_REQUESTAGITEM, *MyPlayer, pcursitem);
	if (pcursmonst != -1) {
		Monster &monter = Monsters[pcursmonst];
		if (!M_Talker(monter) && monter.talkMsg == TEXT_NONE)
			NetSendCmdParam1(true, CMD_KNOCKBACK, pcursmonst);
	}
	NewCursor(CURSOR_HAND);
}

int CalculateGold(Player &player)
{
	int gold = 0;

	for (int i = 0; i < player.numInventoryItems; i++) {
		if (player.inventorySlot[i]._itype == ItemType::Gold)
			gold += player.inventorySlot[i]._ivalue;
	}

	return gold;
}

Size GetInventorySize(const Item &item)
{
	Size size = GetInvItemSize(item._iCurs + CURSOR_FIRSTITEM);

	return { size.width / InventorySlotSizeInPixels.width, size.height / InventorySlotSizeInPixels.height };
}

} // namespace devilution
