/**
 * @file inv.h
 *
 * Interface of player inventory.
 */
#pragma once

#include <stdint.h>

#include "items.h"
#include "palette.h"
#include "player.h"

namespace devilution {

#define INV_SLOT_SIZE_PX 28

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
	NUM_INVELEM,
	INVITEM_INVALID = -1,
};

// identifiers for each of the inventory squares
// see https://github.com/sanctuary/graphics/blob/master/inventory.png
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
	SLOTXY_INV_FIRST = 25,
	SLOTXY_INV_LAST  = 64,

	// belt items
	SLOTXY_BELT_FIRST = 65,
	SLOTXY_BELT_LAST  = 72,
	NUM_XY_SLOTS      = 73
	// clang-format on
};

enum item_color : uint8_t {
	// clang-format off
	ICOL_YELLOW = PAL16_YELLOW + 5,
	ICOL_WHITE  = PAL16_GRAY + 5,
	ICOL_BLUE   = PAL16_BLUE + 5,
	ICOL_RED    = PAL16_RED + 5,
	// clang-format on
};

struct InvXY {
	Sint32 X;
	Sint32 Y;
};

extern bool invflag;
extern bool drawsbarflag;
extern const InvXY InvRect[73];

class Inventory {

public:
	void FreeGFX();
	void Init();

	/**
	 * @brief Render the inventory panel to the given buffer.
	 */
	void DrawInv(CelOutputBuffer out);
	void DrawInvBelt(CelOutputBuffer out);
	void DrawSlotBack(CelOutputBuffer out, int X, int Y, int W, int H);

	bool AutoEquipEnabled(const PlayerStruct &player, const ItemStruct &item);
	bool AutoEquip(int playerNumber, const ItemStruct &item, bool persistItem = true);
	bool AutoPlaceItem(int playerNumber, const ItemStruct &item, bool persistItem = false);
	bool AutoPlaceItemInSlot(int playerNumber, int slotIndex, const ItemStruct &item, bool persistItem);
	bool AutoPlaceItemInBelt(int playerNumber, const ItemStruct &item, bool persistItem = false);
	bool GoldAutoPlace(int pnum);
	void CheckSwap(int pnum, BYTE bLoc, int idx, WORD wCI, int seed, bool bId, uint32_t dwBuff);
	void update_rem_item(int pnum, BYTE iv);

	void AddItemToGrid(int playerNumber, int invGridIndex, int invListIndex, int sizeX, int sizeY);
	InvXY GetInventorySize(const ItemStruct &item);

	/**
	 * @brief Remove an item from player inventory
	 * @param pnum Player index
	 * @param iv invList index of item to be removed
	 * @param calcPlrScrolls If true, CalcPlrScrolls() gets called after removing item
	 */
	void RemoveItem(int pnum, int iv, bool calcPlrScrolls = true);

	void RemoveSpdBarItem(int pnum, int iv);
	void CheckItem(bool isShiftHeld = false);
	void CheckScrn(bool isShiftHeld);
	void CheckItemStats(int pnum);
	void GetItem(int pnum, ItemStruct *item, int ii);
	void AutoGetItem(int pnum, ItemStruct *item, int ii);
	int FindGetItem(int idx, WORD ci, int iseed);
	void SyncGetItem(int x, int y, int idx, WORD ci, int iseed);
	bool CanPut(int x, int y);
	bool TryPut();
	void DrawMsg(const char *msg);
	int PutItem(int pnum, int x, int y);
	int SyncPutItem(int pnum, int x, int y, int idx, WORD icreateinfo, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, DWORD ibuff, int to_hit, int max_dam, int min_str, int min_mag, int min_dex, int ac);
	char CheckHLight();
	void RemoveScroll(int pnum);
	void StartGoldDrop();
	bool UseScroll();
	void UseStaffCharge(int pnum);
	bool UseStaff();
	bool UseInvItem(int pnum, int cii);
	void DoTelekinesis();
	int CalculateGold(int pnum);
	bool DropItemBeforeTrig();

private:
	bool AutoEquip(int playerNumber, const ItemStruct &item, inv_body_loc bodyLocation, bool persistItem);
	bool CanEquip(const ItemStruct &item);
	bool CanEquip(int playerNumber, const ItemStruct &item, inv_body_loc bodyLocation);
	bool CanWield(int playerNumber, const ItemStruct &item);
	bool CanBePlacedOnBelt(const ItemStruct &item);
	void CheckBookLevel(int pnum);
	void CheckCut(int pnum, int mx, int my, bool automaticMove);
	void CheckPaste(int pnum, int mx, int my);
	void CheckQuestItem(int pnum);
	void CleanupItems(ItemStruct *item, int ii);
	bool FitsInBeltSlot(const ItemStruct &item);
	int SwapItem(ItemStruct *a, ItemStruct *b);
	bool WeaponAutoPlace(int pnum);

};

extern Inventory *inventory;

/* data */

extern int AP2x2Tbl[10];

} // namespace devilution
