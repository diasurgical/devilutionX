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
	MyPlayer->_pRSpell = static_cast<spell_id>(spell);
	item._itype = ItemType::Misc;
	item._iMiscId = IMISC_SCROLL;
	item._iSpell = spell;
}

/* Clear the inventory of MyPlayerId. */
void clear_inventory()
{
	for (int i = 0; i < NUM_INV_GRID_ELEM; i++) {
		MyPlayer->InvList[i] = {};
		MyPlayer->InvGrid[i] = 0;
	}
	MyPlayer->_pNumInv = 0;
}

// Test that the scroll is used in the inventory in correct conditions
TEST(Inv, UseScroll_from_inventory)
{
	set_up_scroll(MyPlayer->InvList[2], SPL_FIREBOLT);
	MyPlayer->_pNumInv = 5;
	EXPECT_TRUE(UseScroll(MyPlayer->_pRSpell));
}

// Test that the scroll is used in the belt in correct conditions
TEST(Inv, UseScroll_from_belt)
{
	set_up_scroll(MyPlayer->SpdList[2], SPL_FIREBOLT);
	EXPECT_TRUE(UseScroll(MyPlayer->_pRSpell));
}

// Test that the scroll is not used in the inventory for each invalid condition
TEST(Inv, UseScroll_from_inventory_invalid_conditions)
{
	// Empty the belt to prevent using a scroll from the belt
	for (int i = 0; i < MAXBELTITEMS; i++) {
		MyPlayer->SpdList[i].clear();
	}

	set_up_scroll(MyPlayer->InvList[2], SPL_FIREBOLT);
	pcurs = CURSOR_IDENTIFY;
	EXPECT_FALSE(UseScroll(MyPlayer->_pRSpell));

	set_up_scroll(MyPlayer->InvList[2], SPL_FIREBOLT);
	leveltype = DTYPE_TOWN;
	EXPECT_FALSE(UseScroll(MyPlayer->_pRSpell));

	set_up_scroll(MyPlayer->InvList[2], SPL_FIREBOLT);
	MyPlayer->_pRSpell = static_cast<spell_id>(SPL_HEAL);
	EXPECT_FALSE(UseScroll(MyPlayer->_pRSpell));

	set_up_scroll(MyPlayer->InvList[2], SPL_FIREBOLT);
	MyPlayer->InvList[2]._iMiscId = IMISC_STAFF;
	EXPECT_FALSE(UseScroll(MyPlayer->_pRSpell));

	set_up_scroll(MyPlayer->InvList[2], SPL_FIREBOLT);
	MyPlayer->InvList[2].clear();
	EXPECT_FALSE(UseScroll(MyPlayer->_pRSpell));
}

// Test that the scroll is not used in the belt for each invalid condition
TEST(Inv, UseScroll_from_belt_invalid_conditions)
{
	// Disable the inventory to prevent using a scroll from the inventory
	MyPlayer->_pNumInv = 0;

	set_up_scroll(MyPlayer->SpdList[2], SPL_FIREBOLT);
	pcurs = CURSOR_IDENTIFY;
	EXPECT_FALSE(UseScroll(MyPlayer->_pRSpell));

	set_up_scroll(MyPlayer->SpdList[2], SPL_FIREBOLT);
	leveltype = DTYPE_TOWN;
	EXPECT_FALSE(UseScroll(MyPlayer->_pRSpell));

	set_up_scroll(MyPlayer->SpdList[2], SPL_FIREBOLT);
	MyPlayer->_pRSpell = static_cast<spell_id>(SPL_HEAL);
	EXPECT_FALSE(UseScroll(MyPlayer->_pRSpell));

	set_up_scroll(MyPlayer->SpdList[2], SPL_FIREBOLT);
	MyPlayer->SpdList[2]._iMiscId = IMISC_STAFF;
	EXPECT_FALSE(UseScroll(MyPlayer->_pRSpell));

	set_up_scroll(MyPlayer->SpdList[2], SPL_FIREBOLT);
	MyPlayer->SpdList[2].clear();
	EXPECT_FALSE(UseScroll(MyPlayer->_pRSpell));
}

// Test gold calculation
TEST(Inv, CalculateGold)
{
	MyPlayer->_pNumInv = 10;
	// Set up 4 slots of gold in the inventory
	MyPlayer->InvList[1]._itype = ItemType::Gold;
	MyPlayer->InvList[5]._itype = ItemType::Gold;
	MyPlayer->InvList[2]._itype = ItemType::Gold;
	MyPlayer->InvList[3]._itype = ItemType::Gold;
	// Set the gold amount to arbitrary values
	MyPlayer->InvList[1]._ivalue = 100;
	MyPlayer->InvList[5]._ivalue = 200;
	MyPlayer->InvList[2]._ivalue = 3;
	MyPlayer->InvList[3]._ivalue = 30;

	EXPECT_EQ(CalculateGold(*MyPlayer), 333);
}

// Test automatic gold placing
TEST(Inv, GoldAutoPlace)
{
	// Empty the inventory
	clear_inventory();

	// Put gold into the inventory:
	// | 1000 | ... | ...
	MyPlayer->InvList[0]._itype = ItemType::Gold;
	MyPlayer->InvList[0]._ivalue = 1000;
	MyPlayer->_pNumInv = 1;
	// Put (max gold - 100) gold, which is 4900, into the player's hand
	MyPlayer->HoldItem._itype = ItemType::Gold;
	MyPlayer->HoldItem._ivalue = GOLD_MAX_LIMIT - 100;

	GoldAutoPlace(*MyPlayer, MyPlayer->HoldItem);
	// We expect the inventory:
	// | 5000 | 900 | ...
	EXPECT_EQ(MyPlayer->InvList[0]._ivalue, GOLD_MAX_LIMIT);
	EXPECT_EQ(MyPlayer->InvList[1]._ivalue, 900);
}

// Test removing an item from inventory with no other items.
TEST(Inv, RemoveInvItem)
{
	clear_inventory();
	// Put a two-slot misc item into the inventory:
	// | (item) | (item) | ... | ...
	MyPlayer->_pNumInv = 1;
	MyPlayer->InvGrid[0] = 1;
	MyPlayer->InvGrid[1] = -1;
	MyPlayer->InvList[0]._itype = ItemType::Misc;

	MyPlayer->RemoveInvItem(0);
	EXPECT_EQ(MyPlayer->InvGrid[0], 0);
	EXPECT_EQ(MyPlayer->InvGrid[1], 0);
	EXPECT_EQ(MyPlayer->_pNumInv, 0);
}

// Test removing an item from inventory with other items in it.
TEST(Inv, RemoveInvItem_other_item)
{
	clear_inventory();
	// Put a two-slot misc item and a ring into the inventory:
	// | (item) | (item) | (ring) | ...
	MyPlayer->_pNumInv = 2;
	MyPlayer->InvGrid[0] = 1;
	MyPlayer->InvGrid[1] = -1;
	MyPlayer->InvList[0]._itype = ItemType::Misc;

	MyPlayer->InvGrid[2] = 2;
	MyPlayer->InvList[1]._itype = ItemType::Ring;

	MyPlayer->RemoveInvItem(0);
	EXPECT_EQ(MyPlayer->InvGrid[0], 0);
	EXPECT_EQ(MyPlayer->InvGrid[1], 0);
	EXPECT_EQ(MyPlayer->InvGrid[2], 1);
	EXPECT_EQ(MyPlayer->InvList[0]._itype, ItemType::Ring);
	EXPECT_EQ(MyPlayer->_pNumInv, 1);
}

// Test removing an item from the belt
TEST(Inv, RemoveSpdBarItem)
{
	// Clear the belt
	for (int i = 0; i < MAXBELTITEMS; i++) {
		MyPlayer->SpdList[i].clear();
	}
	// Put an item in the belt: | x | x | item | x | x | x | x | x |
	MyPlayer->SpdList[3]._itype = ItemType::Misc;

	MyPlayer->RemoveSpdBarItem(3);
	EXPECT_TRUE(MyPlayer->SpdList[3].isEmpty());
}

// Test removing a scroll from the inventory
TEST(Inv, RemoveScroll_inventory)
{
	clear_inventory();

	// Put a firebolt scroll into the inventory
	MyPlayer->_pNumInv = 1;
	MyPlayer->_pSpell = static_cast<spell_id>(SPL_FIREBOLT);
	MyPlayer->InvList[0]._itype = ItemType::Misc;
	MyPlayer->InvList[0]._iMiscId = IMISC_SCROLL;
	MyPlayer->InvList[0]._iSpell = SPL_FIREBOLT;

	RemoveScroll(*MyPlayer);
	EXPECT_EQ(MyPlayer->InvGrid[0], 0);
	EXPECT_EQ(MyPlayer->_pNumInv, 0);
}

// Test removing a scroll from the belt
TEST(Inv, RemoveScroll_belt)
{
	// Clear the belt
	for (int i = 0; i < MAXBELTITEMS; i++) {
		MyPlayer->SpdList[i].clear();
	}
	// Put a firebolt scroll into the belt
	MyPlayer->_pSpell = static_cast<spell_id>(SPL_FIREBOLT);
	MyPlayer->SpdList[3]._itype = ItemType::Misc;
	MyPlayer->SpdList[3]._iMiscId = IMISC_SCROLL;
	MyPlayer->SpdList[3]._iSpell = SPL_FIREBOLT;

	RemoveScroll(*MyPlayer);
	EXPECT_TRUE(MyPlayer->SpdList[3].isEmpty());
}

TEST(Inv, ItemSize)
{
	Item testItem {};

	// Inventory sizes are currently determined by examining the sprite size
	// rune of stone and grey suit are adjacent in the sprite list so provide an easy check for off-by-one errors
	InitializeItem(testItem, IDI_RUNEOFSTONE);
	EXPECT_EQ(GetInventorySize(testItem), Size(1, 1));
	InitializeItem(testItem, IDI_GREYSUIT);
	EXPECT_EQ(GetInventorySize(testItem), Size(2, 2));

	// auric amulet is the first used hellfire sprite, but there's multiple unused sprites before it in the list.
	// unfortunately they're the same size so this is less valuable as a test.
	InitializeItem(testItem, IDI_AURIC);
	EXPECT_EQ(GetInventorySize(testItem), Size(1, 1));

	// gold is the last diablo sprite, off by ones will end up loading a 1x1 unused sprite from hellfire but maybe
	//  this'll segfault if we make a mistake in the future?
	InitializeItem(testItem, IDI_GOLD);
	EXPECT_EQ(GetInventorySize(testItem), Size(1, 1));
}
