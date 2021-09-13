#include <gtest/gtest.h>

#include "cursor.h"
#include "inv.h"
#include "player.h"

using namespace devilution;

/* Set up a given item as a spell scroll, allowing for its usage. */
void set_up_scroll(Item &item, spell_id spell)
{
	pcurs = CURSOR_HAND;
	leveltype = DTYPE_CATACOMBS;
	Players[MyPlayerId]._pRSpell = static_cast<spell_id>(spell);
	item._itype = ItemType::Misc;
	item._iMiscId = IMISC_SCROLL;
	item._iSpell = spell;
}

/* Clear the inventory of MyPlayerId. */
void clear_inventory()
{
	for (int i = 0; i < NUM_INV_GRID_ELEM; i++) {
		memset(&Players[MyPlayerId].InvList[i], 0, sizeof(Item));
		Players[MyPlayerId].InvGrid[i] = 0;
	}
	Players[MyPlayerId]._pNumInv = 0;
}

// Test that the scroll is used in the inventory in correct conditions
TEST(Inv, UseScroll_from_inventory)
{
	set_up_scroll(Players[MyPlayerId].InvList[2], SPL_FIREBOLT);
	Players[MyPlayerId]._pNumInv = 5;
	EXPECT_TRUE(UseScroll());
}

// Test that the scroll is used in the belt in correct conditions
TEST(Inv, UseScroll_from_belt)
{
	set_up_scroll(Players[MyPlayerId].SpdList[2], SPL_FIREBOLT);
	EXPECT_TRUE(UseScroll());
}

// Test that the scroll is not used in the inventory for each invalid condition
TEST(Inv, UseScroll_from_inventory_invalid_conditions)
{
	// Empty the belt to prevent using a scroll from the belt
	for (int i = 0; i < MAXBELTITEMS; i++) {
		Players[MyPlayerId].SpdList[i]._itype = ItemType::None;
	}

	set_up_scroll(Players[MyPlayerId].InvList[2], SPL_FIREBOLT);
	pcurs = CURSOR_IDENTIFY;
	EXPECT_FALSE(UseScroll());

	set_up_scroll(Players[MyPlayerId].InvList[2], SPL_FIREBOLT);
	leveltype = DTYPE_TOWN;
	EXPECT_FALSE(UseScroll());

	set_up_scroll(Players[MyPlayerId].InvList[2], SPL_FIREBOLT);
	Players[MyPlayerId]._pRSpell = static_cast<spell_id>(SPL_HEAL);
	EXPECT_FALSE(UseScroll());

	set_up_scroll(Players[MyPlayerId].InvList[2], SPL_FIREBOLT);
	Players[MyPlayerId].InvList[2]._iMiscId = IMISC_STAFF;
	EXPECT_FALSE(UseScroll());

	set_up_scroll(Players[MyPlayerId].InvList[2], SPL_FIREBOLT);
	Players[MyPlayerId].InvList[2]._itype = ItemType::None;
	EXPECT_FALSE(UseScroll());
}

// Test that the scroll is not used in the belt for each invalid condition
TEST(Inv, UseScroll_from_belt_invalid_conditions)
{
	// Disable the inventory to prevent using a scroll from the inventory
	Players[MyPlayerId]._pNumInv = 0;

	set_up_scroll(Players[MyPlayerId].SpdList[2], SPL_FIREBOLT);
	pcurs = CURSOR_IDENTIFY;
	EXPECT_FALSE(UseScroll());

	set_up_scroll(Players[MyPlayerId].SpdList[2], SPL_FIREBOLT);
	leveltype = DTYPE_TOWN;
	EXPECT_FALSE(UseScroll());

	set_up_scroll(Players[MyPlayerId].SpdList[2], SPL_FIREBOLT);
	Players[MyPlayerId]._pRSpell = static_cast<spell_id>(SPL_HEAL);
	EXPECT_FALSE(UseScroll());

	set_up_scroll(Players[MyPlayerId].SpdList[2], SPL_FIREBOLT);
	Players[MyPlayerId].SpdList[2]._iMiscId = IMISC_STAFF;
	EXPECT_FALSE(UseScroll());

	set_up_scroll(Players[MyPlayerId].SpdList[2], SPL_FIREBOLT);
	Players[MyPlayerId].SpdList[2]._itype = ItemType::None;
	EXPECT_FALSE(UseScroll());
}

// Test gold calculation
TEST(Inv, CalculateGold)
{
	Players[MyPlayerId]._pNumInv = 10;
	// Set up 4 slots of gold in the inventory
	Players[MyPlayerId].InvList[1]._itype = ItemType::Gold;
	Players[MyPlayerId].InvList[5]._itype = ItemType::Gold;
	Players[MyPlayerId].InvList[2]._itype = ItemType::Gold;
	Players[MyPlayerId].InvList[3]._itype = ItemType::Gold;
	// Set the gold amount to arbitrary values
	Players[MyPlayerId].InvList[1]._ivalue = 100;
	Players[MyPlayerId].InvList[5]._ivalue = 200;
	Players[MyPlayerId].InvList[2]._ivalue = 3;
	Players[MyPlayerId].InvList[3]._ivalue = 30;

	EXPECT_EQ(CalculateGold(Players[MyPlayerId]), 333);
}

// Test automatic gold placing
TEST(Inv, GoldAutoPlace)
{
	// Empty the inventory
	clear_inventory();

	// Put gold into the inventory:
	// | 1000 | ... | ...
	Players[MyPlayerId].InvList[0]._itype = ItemType::Gold;
	Players[MyPlayerId].InvList[0]._ivalue = 1000;
	Players[MyPlayerId]._pNumInv = 1;
	// Put (max gold - 100) gold, which is 4900, into the player's hand
	Players[MyPlayerId].HoldItem._itype = ItemType::Gold;
	Players[MyPlayerId].HoldItem._ivalue = GOLD_MAX_LIMIT - 100;

	GoldAutoPlace(Players[MyPlayerId]);
	// We expect the inventory:
	// | 5000 | 900 | ...
	EXPECT_EQ(Players[MyPlayerId].InvList[0]._ivalue, GOLD_MAX_LIMIT);
	EXPECT_EQ(Players[MyPlayerId].InvList[1]._ivalue, 900);
}

// Test removing an item from inventory with no other items.
TEST(Inv, RemoveInvItem)
{
	clear_inventory();
	// Put a two-slot misc item into the inventory:
	// | (item) | (item) | ... | ...
	Players[MyPlayerId]._pNumInv = 1;
	Players[MyPlayerId].InvGrid[0] = 1;
	Players[MyPlayerId].InvGrid[1] = -1;
	Players[MyPlayerId].InvList[0]._itype = ItemType::Misc;

	Players[MyPlayerId].RemoveInvItem(0);
	EXPECT_EQ(Players[MyPlayerId].InvGrid[0], 0);
	EXPECT_EQ(Players[MyPlayerId].InvGrid[1], 0);
	EXPECT_EQ(Players[MyPlayerId]._pNumInv, 0);
}

// Test removing an item from inventory with other items in it.
TEST(Inv, RemoveInvItem_other_item)
{
	clear_inventory();
	// Put a two-slot misc item and a ring into the inventory:
	// | (item) | (item) | (ring) | ...
	Players[MyPlayerId]._pNumInv = 2;
	Players[MyPlayerId].InvGrid[0] = 1;
	Players[MyPlayerId].InvGrid[1] = -1;
	Players[MyPlayerId].InvList[0]._itype = ItemType::Misc;

	Players[MyPlayerId].InvGrid[2] = 2;
	Players[MyPlayerId].InvList[1]._itype = ItemType::Ring;

	Players[MyPlayerId].RemoveInvItem(0);
	EXPECT_EQ(Players[MyPlayerId].InvGrid[0], 0);
	EXPECT_EQ(Players[MyPlayerId].InvGrid[1], 0);
	EXPECT_EQ(Players[MyPlayerId].InvGrid[2], 1);
	EXPECT_EQ(Players[MyPlayerId].InvList[0]._itype, ItemType::Ring);
	EXPECT_EQ(Players[MyPlayerId]._pNumInv, 1);
}

// Test removing an item from the belt
TEST(Inv, RemoveSpdBarItem)
{
	// Clear the belt
	for (int i = 0; i < MAXBELTITEMS; i++) {
		Players[MyPlayerId].SpdList[i]._itype = ItemType::None;
	}
	// Put an item in the belt: | x | x | item | x | x | x | x | x |
	Players[MyPlayerId].SpdList[3]._itype = ItemType::Misc;

	Players[MyPlayerId].RemoveSpdBarItem(3);
	EXPECT_EQ(Players[MyPlayerId].SpdList[3]._itype, ItemType::None);
}

// Test removing a scroll from the inventory
TEST(Inv, RemoveScroll_inventory)
{
	clear_inventory();

	// Put a firebolt scroll into the inventory
	Players[MyPlayerId]._pNumInv = 1;
	Players[MyPlayerId]._pSpell = static_cast<spell_id>(SPL_FIREBOLT);
	Players[MyPlayerId].InvList[0]._itype = ItemType::Misc;
	Players[MyPlayerId].InvList[0]._iMiscId = IMISC_SCROLL;
	Players[MyPlayerId].InvList[0]._iSpell = SPL_FIREBOLT;

	RemoveScroll(Players[MyPlayerId]);
	EXPECT_EQ(Players[MyPlayerId].InvGrid[0], 0);
	EXPECT_EQ(Players[MyPlayerId]._pNumInv, 0);
}

// Test removing a scroll from the belt
TEST(Inv, RemoveScroll_belt)
{
	// Clear the belt
	for (int i = 0; i < MAXBELTITEMS; i++) {
		Players[MyPlayerId].SpdList[i]._itype = ItemType::None;
	}
	// Put a firebolt scroll into the belt
	Players[MyPlayerId]._pSpell = static_cast<spell_id>(SPL_FIREBOLT);
	Players[MyPlayerId].SpdList[3]._itype = ItemType::Misc;
	Players[MyPlayerId].SpdList[3]._iMiscId = IMISC_SCROLL;
	Players[MyPlayerId].SpdList[3]._iSpell = SPL_FIREBOLT;

	RemoveScroll(Players[MyPlayerId]);
	EXPECT_EQ(Players[MyPlayerId].SpdList[3]._itype, ItemType::None);
}
