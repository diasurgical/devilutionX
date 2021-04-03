#include <gtest/gtest.h>
#include "all.h"

/* Set up a given item as a spell scroll, allowing for its usage. */
void set_up_scroll(dvl::ItemStruct &item, dvl::spell_id spell)
{
	dvl::pcurs = dvl::CURSOR_HAND;
	dvl::leveltype = dvl::DTYPE_CATACOMBS;
	dvl::plr[dvl::myplr]._pRSpell = static_cast<dvl::spell_id>(spell);
	item._itype = dvl::ITYPE_MISC;
	item._iMiscId = dvl::IMISC_SCROLL;
	item._iSpell = spell;
}

/* Clear the inventory of dvl::myplr. */
void clear_inventory()
{
	for (int i = 0; i < NUM_INV_GRID_ELEM; i++) {
		memset(&dvl::plr[dvl::myplr].InvList[i], 0, sizeof(dvl::ItemStruct));
		dvl::plr[dvl::myplr].InvGrid[i] = 0;
	}
	dvl::plr[dvl::myplr]._pNumInv = 0;
}

// Test that the scroll is used in the inventory in correct conditions
TEST(Inv, UseScroll_from_inventory)
{
	set_up_scroll(dvl::plr[dvl::myplr].InvList[2], dvl::SPL_FIREBOLT);
	dvl::plr[dvl::myplr]._pNumInv = 5;
	EXPECT_TRUE(dvl::UseScroll());
}

// Test that the scroll is used in the belt in correct conditions
TEST(Inv, UseScroll_from_belt)
{
	set_up_scroll(dvl::plr[dvl::myplr].SpdList[2], dvl::SPL_FIREBOLT);
	EXPECT_TRUE(dvl::UseScroll());
}

// Test that the scroll is not used in the inventory for each invalid condition
TEST(Inv, UseScroll_from_inventory_invalid_conditions)
{
	// Empty the belt to prevent using a scroll from the belt
	for (int i = 0; i < MAXBELTITEMS; i++) {
		dvl::plr[dvl::myplr].SpdList[i]._itype = dvl::ITYPE_NONE;
	}

	set_up_scroll(dvl::plr[dvl::myplr].InvList[2], dvl::SPL_FIREBOLT);
	dvl::pcurs = dvl::CURSOR_IDENTIFY;
	EXPECT_FALSE(dvl::UseScroll());

	set_up_scroll(dvl::plr[dvl::myplr].InvList[2], dvl::SPL_FIREBOLT);
	dvl::leveltype = dvl::DTYPE_TOWN;
	EXPECT_FALSE(dvl::UseScroll());

	set_up_scroll(dvl::plr[dvl::myplr].InvList[2], dvl::SPL_FIREBOLT);
	dvl::plr[dvl::myplr]._pRSpell = static_cast<dvl::spell_id>(dvl::SPL_HEAL);
	EXPECT_FALSE(dvl::UseScroll());

	set_up_scroll(dvl::plr[dvl::myplr].InvList[2], dvl::SPL_FIREBOLT);
	dvl::plr[dvl::myplr].InvList[2]._iMiscId = dvl::IMISC_STAFF;
	EXPECT_FALSE(dvl::UseScroll());

	set_up_scroll(dvl::plr[dvl::myplr].InvList[2], dvl::SPL_FIREBOLT);
	dvl::plr[dvl::myplr].InvList[2]._itype = dvl::ITYPE_NONE;
	EXPECT_FALSE(dvl::UseScroll());
}

// Test that the scroll is not used in the belt for each invalid condition
TEST(Inv, UseScroll_from_belt_invalid_conditions)
{
	// Disable the inventory to prevent using a scroll from the inventory
	dvl::plr[dvl::myplr]._pNumInv = 0;

	set_up_scroll(dvl::plr[dvl::myplr].SpdList[2], dvl::SPL_FIREBOLT);
	dvl::pcurs = dvl::CURSOR_IDENTIFY;
	EXPECT_FALSE(dvl::UseScroll());

	set_up_scroll(dvl::plr[dvl::myplr].SpdList[2], dvl::SPL_FIREBOLT);
	dvl::leveltype = dvl::DTYPE_TOWN;
	EXPECT_FALSE(dvl::UseScroll());

	set_up_scroll(dvl::plr[dvl::myplr].SpdList[2], dvl::SPL_FIREBOLT);
	dvl::plr[dvl::myplr]._pRSpell = static_cast<dvl::spell_id>(dvl::SPL_HEAL);
	EXPECT_FALSE(dvl::UseScroll());

	set_up_scroll(dvl::plr[dvl::myplr].SpdList[2], dvl::SPL_FIREBOLT);
	dvl::plr[dvl::myplr].SpdList[2]._iMiscId = dvl::IMISC_STAFF;
	EXPECT_FALSE(dvl::UseScroll());

	set_up_scroll(dvl::plr[dvl::myplr].SpdList[2], dvl::SPL_FIREBOLT);
	dvl::plr[dvl::myplr].SpdList[2]._itype = dvl::ITYPE_NONE;
	EXPECT_FALSE(dvl::UseScroll());
}

// Test gold calculation
TEST(Inv, CalculateGold)
{
	dvl::plr[dvl::myplr]._pNumInv = 10;
	// Set up two slots of gold both in the belt and inventory
	dvl::plr[dvl::myplr].SpdList[1]._itype = dvl::ITYPE_GOLD;
	dvl::plr[dvl::myplr].SpdList[5]._itype = dvl::ITYPE_GOLD;
	dvl::plr[dvl::myplr].InvList[2]._itype = dvl::ITYPE_GOLD;
	dvl::plr[dvl::myplr].InvList[3]._itype = dvl::ITYPE_GOLD;
	// Set the gold amount to arbitrary values
	dvl::plr[dvl::myplr].SpdList[1]._ivalue = 100;
	dvl::plr[dvl::myplr].SpdList[5]._ivalue = 200;
	dvl::plr[dvl::myplr].InvList[2]._ivalue = 3;
	dvl::plr[dvl::myplr].InvList[3]._ivalue = 30;

	EXPECT_EQ(dvl::CalculateGold(dvl::myplr), 333);
}

// Test automatic gold placing
TEST(Inv, GoldAutoPlace)
{
	// Empty the inventory
	clear_inventory();

	// Put gold into the inventory:
	// | 1000 | ... | ...
	dvl::plr[dvl::myplr].InvList[0]._itype = dvl::ITYPE_GOLD;
	dvl::plr[dvl::myplr].InvList[0]._ivalue = 1000;
	dvl::plr[dvl::myplr]._pNumInv = 1;
	// Put (max gold - 100) gold, which is 4900, into the player's hand
	dvl::plr[dvl::myplr].HoldItem._itype = dvl::ITYPE_GOLD;
	dvl::plr[dvl::myplr].HoldItem._ivalue = GOLD_MAX_LIMIT - 100;

	dvl::GoldAutoPlace(dvl::myplr);
	// We expect the inventory:
	// | 5000 | 900 | ...
	EXPECT_EQ(dvl::plr[dvl::myplr].InvList[0]._ivalue, GOLD_MAX_LIMIT);
	EXPECT_EQ(dvl::plr[dvl::myplr].InvList[1]._ivalue, 900);
}

// Test removing an item from inventory with no other items.
TEST(Inv, RemoveInvItem)
{
	clear_inventory();
	// Put a two-slot misc item into the inventory:
	// | (item) | (item) | ... | ...
	dvl::plr[dvl::myplr]._pNumInv = 1;
	dvl::plr[dvl::myplr].InvGrid[0] = 1;
	dvl::plr[dvl::myplr].InvGrid[1] = -1;
	dvl::plr[dvl::myplr].InvList[0]._itype = dvl::ITYPE_MISC;

	dvl::RemoveInvItem(dvl::myplr, 0);
	EXPECT_EQ(dvl::plr[dvl::myplr].InvGrid[0], 0);
	EXPECT_EQ(dvl::plr[dvl::myplr].InvGrid[1], 0);
	EXPECT_EQ(dvl::plr[dvl::myplr]._pNumInv, 0);
}

// Test removing an item from inventory with other items in it.
TEST(Inv, RemoveInvItem_other_item)
{
	clear_inventory();
	// Put a two-slot misc item and a ring into the inventory:
	// | (item) | (item) | (ring) | ...
	dvl::plr[dvl::myplr]._pNumInv = 2;
	dvl::plr[dvl::myplr].InvGrid[0] = 1;
	dvl::plr[dvl::myplr].InvGrid[1] = -1;
	dvl::plr[dvl::myplr].InvList[0]._itype = dvl::ITYPE_MISC;

	dvl::plr[dvl::myplr].InvGrid[2] = 2;
	dvl::plr[dvl::myplr].InvList[1]._itype = dvl::ITYPE_RING;

	dvl::RemoveInvItem(dvl::myplr, 0);
	EXPECT_EQ(dvl::plr[dvl::myplr].InvGrid[0], 0);
	EXPECT_EQ(dvl::plr[dvl::myplr].InvGrid[1], 0);
	EXPECT_EQ(dvl::plr[dvl::myplr].InvGrid[2], 1);
	EXPECT_EQ(dvl::plr[dvl::myplr].InvList[0]._itype, dvl::ITYPE_RING);
	EXPECT_EQ(dvl::plr[dvl::myplr]._pNumInv, 1);
}

// Test removing an item from the belt
TEST(Inv, RemoveSpdBarItem)
{
	// Clear the belt
	for (int i = 0; i < MAXBELTITEMS; i++) {
		dvl::plr[dvl::myplr].SpdList[i]._itype = dvl::ITYPE_NONE;
	}
	// Put an item in the belt: | x | x | item | x | x | x | x | x |
	dvl::plr[dvl::myplr].SpdList[3]._itype = dvl::ITYPE_MISC;

	dvl::RemoveSpdBarItem(dvl::myplr, 3);
	EXPECT_EQ(dvl::plr[dvl::myplr].SpdList[3]._itype, dvl::ITYPE_NONE);
}

// Test removing a scroll from the inventory
TEST(Inv, RemoveScroll_inventory)
{
	clear_inventory();

	// Put a firebolt scroll into the inventory
	dvl::plr[dvl::myplr]._pNumInv = 1;
	dvl::plr[dvl::myplr]._pRSpell = static_cast<dvl::spell_id>(dvl::SPL_FIREBOLT);
	dvl::plr[dvl::myplr].InvList[0]._itype = dvl::ITYPE_MISC;
	dvl::plr[dvl::myplr].InvList[0]._iMiscId = dvl::IMISC_SCROLL;
	dvl::plr[dvl::myplr].InvList[0]._iSpell = dvl::SPL_FIREBOLT;

	dvl::RemoveScroll(dvl::myplr);
	EXPECT_EQ(dvl::plr[dvl::myplr].InvGrid[0], 0);
	EXPECT_EQ(dvl::plr[dvl::myplr]._pNumInv, 0);
}

// Test removing a scroll from the belt
TEST(Inv, RemoveScroll_belt)
{
	// Clear the belt
	for (int i = 0; i < MAXBELTITEMS; i++) {
		dvl::plr[dvl::myplr].SpdList[i]._itype = dvl::ITYPE_NONE;
	}
	// Put a firebolt scroll into the belt
	dvl::plr[dvl::myplr]._pSpell = static_cast<dvl::spell_id>(dvl::SPL_FIREBOLT);
	dvl::plr[dvl::myplr].SpdList[3]._itype = dvl::ITYPE_MISC;
	dvl::plr[dvl::myplr].SpdList[3]._iMiscId = dvl::IMISC_SCROLL;
	dvl::plr[dvl::myplr].SpdList[3]._iSpell = dvl::SPL_FIREBOLT;

	dvl::RemoveScroll(dvl::myplr);
	EXPECT_EQ(dvl::plr[dvl::myplr].SpdList[3]._itype, dvl::ITYPE_NONE);
}
