#include <gtest/gtest.h>

#include "cursor.h"
#include "inv.h"
#include "player.h"

using namespace devilution;

/* Set up a given item as a spell scroll, allowing for its usage. */
void set_up_scroll(ItemStruct &item, spell_id spell)
{
	pcurs = CURSOR_HAND;
	leveltype = DTYPE_CATACOMBS;
	plr[myplr]._pRSpell = static_cast<spell_id>(spell);
	item._itype = ITYPE_MISC;
	item._iMiscId = IMISC_SCROLL;
	item._iSpell = spell;
}

/* Clear the inventory of myplr. */
void clear_inventory()
{
	for (int i = 0; i < NUM_INV_GRID_ELEM; i++) {
		memset(&plr[myplr].InvList[i], 0, sizeof(ItemStruct));
		plr[myplr].InvGrid[i] = 0;
	}
	plr[myplr]._pNumInv = 0;
}

// Test that the scroll is used in the inventory in correct conditions
TEST(Inv, UseScroll_from_inventory)
{
	set_up_scroll(plr[myplr].InvList[2], SPL_FIREBOLT);
	plr[myplr]._pNumInv = 5;
	EXPECT_TRUE(UseScroll());
}

// Test that the scroll is used in the belt in correct conditions
TEST(Inv, UseScroll_from_belt)
{
	set_up_scroll(plr[myplr].SpdList[2], SPL_FIREBOLT);
	EXPECT_TRUE(UseScroll());
}

// Test that the scroll is not used in the inventory for each invalid condition
TEST(Inv, UseScroll_from_inventory_invalid_conditions)
{
	// Empty the belt to prevent using a scroll from the belt
	for (int i = 0; i < MAXBELTITEMS; i++) {
		plr[myplr].SpdList[i]._itype = ITYPE_NONE;
	}

	set_up_scroll(plr[myplr].InvList[2], SPL_FIREBOLT);
	pcurs = CURSOR_IDENTIFY;
	EXPECT_FALSE(UseScroll());

	set_up_scroll(plr[myplr].InvList[2], SPL_FIREBOLT);
	leveltype = DTYPE_TOWN;
	EXPECT_FALSE(UseScroll());

	set_up_scroll(plr[myplr].InvList[2], SPL_FIREBOLT);
	plr[myplr]._pRSpell = static_cast<spell_id>(SPL_HEAL);
	EXPECT_FALSE(UseScroll());

	set_up_scroll(plr[myplr].InvList[2], SPL_FIREBOLT);
	plr[myplr].InvList[2]._iMiscId = IMISC_STAFF;
	EXPECT_FALSE(UseScroll());

	set_up_scroll(plr[myplr].InvList[2], SPL_FIREBOLT);
	plr[myplr].InvList[2]._itype = ITYPE_NONE;
	EXPECT_FALSE(UseScroll());
}

// Test that the scroll is not used in the belt for each invalid condition
TEST(Inv, UseScroll_from_belt_invalid_conditions)
{
	// Disable the inventory to prevent using a scroll from the inventory
	plr[myplr]._pNumInv = 0;

	set_up_scroll(plr[myplr].SpdList[2], SPL_FIREBOLT);
	pcurs = CURSOR_IDENTIFY;
	EXPECT_FALSE(UseScroll());

	set_up_scroll(plr[myplr].SpdList[2], SPL_FIREBOLT);
	leveltype = DTYPE_TOWN;
	EXPECT_FALSE(UseScroll());

	set_up_scroll(plr[myplr].SpdList[2], SPL_FIREBOLT);
	plr[myplr]._pRSpell = static_cast<spell_id>(SPL_HEAL);
	EXPECT_FALSE(UseScroll());

	set_up_scroll(plr[myplr].SpdList[2], SPL_FIREBOLT);
	plr[myplr].SpdList[2]._iMiscId = IMISC_STAFF;
	EXPECT_FALSE(UseScroll());

	set_up_scroll(plr[myplr].SpdList[2], SPL_FIREBOLT);
	plr[myplr].SpdList[2]._itype = ITYPE_NONE;
	EXPECT_FALSE(UseScroll());
}

// Test gold calculation
TEST(Inv, CalculateGold)
{
	plr[myplr]._pNumInv = 10;
	// Set up two slots of gold both in the belt and inventory
	plr[myplr].SpdList[1]._itype = ITYPE_GOLD;
	plr[myplr].SpdList[5]._itype = ITYPE_GOLD;
	plr[myplr].InvList[2]._itype = ITYPE_GOLD;
	plr[myplr].InvList[3]._itype = ITYPE_GOLD;
	// Set the gold amount to arbitrary values
	plr[myplr].SpdList[1]._ivalue = 100;
	plr[myplr].SpdList[5]._ivalue = 200;
	plr[myplr].InvList[2]._ivalue = 3;
	plr[myplr].InvList[3]._ivalue = 30;

	EXPECT_EQ(CalculateGold(myplr), 333);
}

// Test automatic gold placing
TEST(Inv, GoldAutoPlace)
{
	// Empty the inventory
	clear_inventory();

	// Put gold into the inventory:
	// | 1000 | ... | ...
	plr[myplr].InvList[0]._itype = ITYPE_GOLD;
	plr[myplr].InvList[0]._ivalue = 1000;
	plr[myplr]._pNumInv = 1;
	// Put (max gold - 100) gold, which is 4900, into the player's hand
	plr[myplr].HoldItem._itype = ITYPE_GOLD;
	plr[myplr].HoldItem._ivalue = GOLD_MAX_LIMIT - 100;

	GoldAutoPlace(myplr);
	// We expect the inventory:
	// | 5000 | 900 | ...
	EXPECT_EQ(plr[myplr].InvList[0]._ivalue, GOLD_MAX_LIMIT);
	EXPECT_EQ(plr[myplr].InvList[1]._ivalue, 900);
}

// Test removing an item from inventory with no other items.
TEST(Inv, RemoveInvItem)
{
	clear_inventory();
	// Put a two-slot misc item into the inventory:
	// | (item) | (item) | ... | ...
	plr[myplr]._pNumInv = 1;
	plr[myplr].InvGrid[0] = 1;
	plr[myplr].InvGrid[1] = -1;
	plr[myplr].InvList[0]._itype = ITYPE_MISC;

	plr[myplr].RemoveInvItem(0);
	EXPECT_EQ(plr[myplr].InvGrid[0], 0);
	EXPECT_EQ(plr[myplr].InvGrid[1], 0);
	EXPECT_EQ(plr[myplr]._pNumInv, 0);
}

// Test removing an item from inventory with other items in it.
TEST(Inv, RemoveInvItem_other_item)
{
	clear_inventory();
	// Put a two-slot misc item and a ring into the inventory:
	// | (item) | (item) | (ring) | ...
	plr[myplr]._pNumInv = 2;
	plr[myplr].InvGrid[0] = 1;
	plr[myplr].InvGrid[1] = -1;
	plr[myplr].InvList[0]._itype = ITYPE_MISC;

	plr[myplr].InvGrid[2] = 2;
	plr[myplr].InvList[1]._itype = ITYPE_RING;

	plr[myplr].RemoveInvItem(0);
	EXPECT_EQ(plr[myplr].InvGrid[0], 0);
	EXPECT_EQ(plr[myplr].InvGrid[1], 0);
	EXPECT_EQ(plr[myplr].InvGrid[2], 1);
	EXPECT_EQ(plr[myplr].InvList[0]._itype, ITYPE_RING);
	EXPECT_EQ(plr[myplr]._pNumInv, 1);
}

// Test removing an item from the belt
TEST(Inv, RemoveSpdBarItem)
{
	// Clear the belt
	for (int i = 0; i < MAXBELTITEMS; i++) {
		plr[myplr].SpdList[i]._itype = ITYPE_NONE;
	}
	// Put an item in the belt: | x | x | item | x | x | x | x | x |
	plr[myplr].SpdList[3]._itype = ITYPE_MISC;

	RemoveSpdBarItem(myplr, 3);
	EXPECT_EQ(plr[myplr].SpdList[3]._itype, ITYPE_NONE);
}

// Test removing a scroll from the inventory
TEST(Inv, RemoveScroll_inventory)
{
	clear_inventory();

	// Put a firebolt scroll into the inventory
	plr[myplr]._pNumInv = 1;
	plr[myplr]._pRSpell = static_cast<spell_id>(SPL_FIREBOLT);
	plr[myplr].InvList[0]._itype = ITYPE_MISC;
	plr[myplr].InvList[0]._iMiscId = IMISC_SCROLL;
	plr[myplr].InvList[0]._iSpell = SPL_FIREBOLT;

	RemoveScroll(myplr);
	EXPECT_EQ(plr[myplr].InvGrid[0], 0);
	EXPECT_EQ(plr[myplr]._pNumInv, 0);
}

// Test removing a scroll from the belt
TEST(Inv, RemoveScroll_belt)
{
	// Clear the belt
	for (int i = 0; i < MAXBELTITEMS; i++) {
		plr[myplr].SpdList[i]._itype = ITYPE_NONE;
	}
	// Put a firebolt scroll into the belt
	plr[myplr]._pSpell = static_cast<spell_id>(SPL_FIREBOLT);
	plr[myplr].SpdList[3]._itype = ITYPE_MISC;
	plr[myplr].SpdList[3]._iMiscId = IMISC_SCROLL;
	plr[myplr].SpdList[3]._iSpell = SPL_FIREBOLT;

	RemoveScroll(myplr);
	EXPECT_EQ(plr[myplr].SpdList[3]._itype, ITYPE_NONE);
}
