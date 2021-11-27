/**
 * @file inv.h
 *
 * Interface of player inventory.
 */
#pragma once

#include <cstdint>

#include "engine/point.hpp"
#include "items.h"
#include "palette.h"
#include "player.h"

namespace devilution {

#define INV_SLOT_SIZE_PX 28
#define INV_SLOT_HALF_SIZE_PX (INV_SLOT_SIZE_PX / 2)
#define INV_ROW_SLOT_SIZE 10
constexpr Size InventorySlotSizeInPixels { INV_SLOT_SIZE_PX, INV_SLOT_SIZE_PX };

enum inv_item : int8_t {
	// clang-format off
	INVITEM_HEAD       = 0,
	INVITEM_RING_LEFT  = 1,
	INVITEM_RING_RIGHT = 2,
	INVITEM_AMULET     = 3,
	INVITEM_HAND_LEFT  = 4,
	INVITEM_HAND_RIGHT = 5,
	INVITEM_CHEST      = 6,
	INVITEM_INV_FIRST  = 7,
	INVITEM_INV_LAST   = 46,
	INVITEM_BELT_FIRST = 47,
	INVITEM_BELT_LAST  = 54,
	// clang-format on
};

/**
 * identifiers for each of the inventory squares
 * @see #InvRect
 */
enum inv_xy_slot : uint8_t {
	// clang-format off
	SLOTXY_HEAD_FIRST       = 0,
	SLOTXY_HEAD_LAST        = 3,
	SLOTXY_RING_LEFT        = 4,
	SLOTXY_RING_RIGHT       = 5,
	SLOTXY_AMULET           = 6,
	SLOTXY_HAND_LEFT_FIRST  = 7,
	SLOTXY_HAND_LEFT_LAST   = 12,
	SLOTXY_HAND_RIGHT_FIRST = 13,
	SLOTXY_HAND_RIGHT_LAST  = 18,
	SLOTXY_CHEST_FIRST      = 19,
	SLOTXY_CHEST_LAST       = 24,

	// regular inventory
	SLOTXY_INV_FIRST        = 25,
	SLOTXY_INV_ROW1_FIRST   = SLOTXY_INV_FIRST,
	SLOTXY_INV_ROW1_LAST    = 34,
	SLOTXY_INV_ROW2_FIRST   = 35,
	SLOTXY_INV_ROW2_LAST    = 44,
	SLOTXY_INV_ROW3_FIRST   = 45,
	SLOTXY_INV_ROW3_LAST    = 54,
	SLOTXY_INV_ROW4_FIRST   = 55,
	SLOTXY_INV_ROW4_LAST    = 64,
	SLOTXY_INV_LAST         = SLOTXY_INV_ROW4_LAST,

	// belt items
	SLOTXY_BELT_FIRST       = 65,
	SLOTXY_BELT_LAST        = 72,
	NUM_XY_SLOTS            = 73
	// clang-format on
};

enum item_color : uint8_t {
	// clang-format off
	ICOL_YELLOW = PAL16_YELLOW + 5,
	ICOL_WHITE  = PAL16_GRAY   + 5,
	ICOL_BLUE   = PAL16_BLUE   + 5,
	ICOL_RED    = PAL16_RED    + 5,
	// clang-format on
};

extern bool invflag;
extern bool drawsbarflag;
extern const Point InvRect[73];

/**
 * @brief Function type which performs an operation on the given item.
 */
using ItemFunc = void (*)(Item &);

void FreeInvGFX();
void InitInv();

/**
 * @brief Render the inventory panel to the given buffer.
 */
void DrawInv(const Surface &out);

void DrawInvBelt(const Surface &out);

/**
 * @brief Checks whether or not auto-equipping behavior is enabled for the given player and item.
 * @param player The player to check.
 * @param item The item to check.
 * @return 'True' if auto-equipping behavior is enabled for the player and item and 'False' otherwise.
 */
bool AutoEquipEnabled(const Player &player, const Item &item);

/**
 * @brief Automatically attempts to equip the specified item in the most appropriate location in the player's body.
 * @note On success, this will broadcast an equipment_change event to let other players know about the equipment change.
 * @param playerId The player number whose inventory will be checked for compatibility with the item.
 * @param item The item to equip.
 * @param persistItem Indicates whether or not the item should be persisted in the player's body. Pass 'False' to check
 * whether the player can equip the item but you don't want the item to actually be equipped. 'True' by default.
 * @return 'True' if the item was equipped and 'False' otherwise.
 */
bool AutoEquip(int playerId, const Item &item, bool persistItem = true);

/**
 * @brief Checks whether the given item can be placed on the specified player's inventory.
 * If 'persistItem' is 'True', the item is also placed in the inventory.
 * @param item The item to be checked.
 * @param persistItem Pass 'True' to actually place the item in the inventory. The default is 'False'.
 * @return 'True' in case the item can be placed on the player's inventory and 'False' otherwise.
 */
bool AutoPlaceItemInInventory(Player &player, const Item &item, bool persistItem = false);

/**
 * @brief Checks whether the given item can be placed on the specified player's inventory slot.
 * If 'persistItem' is 'True', the item is also placed in the inventory slot.
 * @param slotIndex The 0-based index of the slot to put the item on.
 * @param item The item to be checked.
 * @param persistItem Pass 'True' to actually place the item in the inventory slot. The default is 'False'.
 * @return 'True' in case the item can be placed on the specified player's inventory slot and 'False' otherwise.
 */
bool AutoPlaceItemInInventorySlot(Player &player, int slotIndex, const Item &item, bool persistItem);

/**
 * @brief Checks whether the given item can be placed on the specified player's belt. Returns 'True' when the item can be placed
 * on belt slots and the player has at least one empty slot in his belt.
 * If 'persistItem' is 'True', the item is also placed in the belt.
 * @param player The player on whose belt will be checked.
 * @param item The item to be checked.
 * @param persistItem Pass 'True' to actually place the item in the belt. The default is 'False'.
 * @return 'True' in case the item can be placed on the player's belt and 'False' otherwise.
 */
bool AutoPlaceItemInBelt(Player &player, const Item &item, bool persistItem = false);
bool GoldAutoPlace(Player &player);
bool GoldAutoPlaceInInventorySlot(Player &player, int slotIndex);
void CheckInvSwap(Player &player, inv_body_loc bLoc, int idx, uint16_t wCI, int seed, bool bId, uint32_t dwBuff);
void inv_update_rem_item(Player &player, inv_body_loc iv);
void CheckInvItem(bool isShiftHeld = false, bool isCtrlHeld = false);

/**
 * Check for interactions with belt
 */
void CheckInvScrn(bool isShiftHeld, bool isCtrlHeld);
void CheckItemStats(Player &player);
void InvGetItem(int pnum, int ii);
void AutoGetItem(int pnum, Item *item, int ii);

/**
 * @brief Searches for a dropped item with the same type/createInfo/seed
 * @param iseed The value used to initialise the RNG when generating the item
 * @param idx The overarching type of the target item
 * @param createInfo Flags used to describe the specific subtype of the target item
 * @return An index into ActiveItems or -1 if no matching item was found
 */
int FindGetItem(int32_t iseed, _item_indexes idx, uint16_t ci);
void SyncGetItem(Point position, int32_t iseed, _item_indexes idx, uint16_t ci);
bool CanPut(Point position);
bool TryInvPut();
int InvPutItem(Player &player, Point position);
int SyncPutItem(Player &player, Point position, int idx, uint16_t icreateinfo, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, uint32_t ibuff, int toHit, int maxDam, int minStr, int minMag, int minDex, int ac);
int8_t CheckInvHLight();
void RemoveScroll(Player &player);
bool UseScroll();
void UseStaffCharge(Player &player);
bool UseStaff();
bool UseInvItem(int pnum, int cii);
void DoTelekinesis();
int CalculateGold(Player &player);
bool DropItemBeforeTrig();

/* data */

} // namespace devilution
