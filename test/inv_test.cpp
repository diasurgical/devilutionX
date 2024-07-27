#include <gtest/gtest.h>

#include "cursor.h"
#include "inv.h"
#include "player.h"
#include "storm/storm_net.hpp"

namespace devilution {
namespace {

class InvTest : public ::testing::Test {
public:
	void SetUp() override
	{
		Players.resize(1);
		MyPlayer = &Players[0];
	}

	static void SetUpTestSuite()
	{
		LoadCoreArchives();
		LoadGameArchives();

		// The tests need spawn.mpq or diabdat.mpq
		// Please provide them so that the tests can run successfully
		ASSERT_TRUE(HaveSpawn() || HaveDiabdat());

		InitCursor();
		LoadSpellData();
		LoadItemData();
	}
};

/* Set up a given item as a spell scroll, allowing for its usage. */
void set_up_scroll(Item &item, SpellID spell)
{
	pcurs = CURSOR_HAND;
	leveltype = DTYPE_CATACOMBS;
	MyPlayer->selectedSpell = static_cast<SpellID>(spell);
	item._itype = ItemType::Misc;
	item._iMiscId = IMISC_SCROLL;
	item._iSpell = spell;
}

/* Clear the inventory of MyPlayerId. */
void clear_inventory()
{
	for (int i = 0; i < InventoryGridCells; i++) {
		MyPlayer->inventorySlot[i] = {};
		MyPlayer->inventoryGrid[i] = 0;
	}
	MyPlayer->numInventoryItems = 0;
}

// Test that the scroll is used in the inventory in correct conditions
TEST_F(InvTest, UseScroll_from_inventory)
{
	set_up_scroll(MyPlayer->inventorySlot[2], SpellID::Firebolt);
	MyPlayer->numInventoryItems = 5;
	EXPECT_TRUE(CanUseScroll(*MyPlayer, SpellID::Firebolt));
}

// Test that the scroll is used in the belt in correct conditions
TEST_F(InvTest, UseScroll_from_belt)
{
	set_up_scroll(MyPlayer->beltSlot[2], SpellID::Firebolt);
	EXPECT_TRUE(CanUseScroll(*MyPlayer, SpellID::Firebolt));
}

// Test that the scroll is not used in the inventory for each invalid condition
TEST_F(InvTest, UseScroll_from_inventory_invalid_conditions)
{
	// Empty the belt to prevent using a scroll from the belt
	for (int i = 0; i < MaxBeltItems; i++) {
		MyPlayer->beltSlot[i].clear();
	}

	// Adjust inventory size
	MyPlayer->numInventoryItems = 5;

	set_up_scroll(MyPlayer->inventorySlot[2], SpellID::Firebolt);
	leveltype = DTYPE_TOWN;
	EXPECT_FALSE(CanUseScroll(*MyPlayer, SpellID::Firebolt));

	set_up_scroll(MyPlayer->inventorySlot[2], SpellID::Firebolt);
	MyPlayer->selectedSpell = SpellID::Healing;
	EXPECT_FALSE(CanUseScroll(*MyPlayer, SpellID::Healing));

	set_up_scroll(MyPlayer->inventorySlot[2], SpellID::Firebolt);
	MyPlayer->inventorySlot[2]._iMiscId = IMISC_STAFF;
	EXPECT_FALSE(CanUseScroll(*MyPlayer, SpellID::Firebolt));

	set_up_scroll(MyPlayer->inventorySlot[2], SpellID::Firebolt);
	MyPlayer->inventorySlot[2].clear();
	EXPECT_FALSE(CanUseScroll(*MyPlayer, SpellID::Firebolt));
}

// Test that the scroll is not used in the belt for each invalid condition
TEST_F(InvTest, UseScroll_from_belt_invalid_conditions)
{
	// Disable the inventory to prevent using a scroll from the inventory
	MyPlayer->numInventoryItems = 0;

	set_up_scroll(MyPlayer->beltSlot[2], SpellID::Firebolt);
	leveltype = DTYPE_TOWN;
	EXPECT_FALSE(CanUseScroll(*MyPlayer, SpellID::Firebolt));

	set_up_scroll(MyPlayer->beltSlot[2], SpellID::Firebolt);
	MyPlayer->selectedSpell = SpellID::Healing;
	EXPECT_FALSE(CanUseScroll(*MyPlayer, SpellID::Healing));

	set_up_scroll(MyPlayer->beltSlot[2], SpellID::Firebolt);
	MyPlayer->beltSlot[2]._iMiscId = IMISC_STAFF;
	EXPECT_FALSE(CanUseScroll(*MyPlayer, SpellID::Firebolt));

	set_up_scroll(MyPlayer->beltSlot[2], SpellID::Firebolt);
	MyPlayer->beltSlot[2].clear();
	EXPECT_FALSE(CanUseScroll(*MyPlayer, SpellID::Firebolt));
}

// Test gold calculation
TEST_F(InvTest, CalculateGold)
{
	MyPlayer->numInventoryItems = 10;
	// Set up 4 slots of gold in the inventory
	MyPlayer->inventorySlot[1]._itype = ItemType::Gold;
	MyPlayer->inventorySlot[5]._itype = ItemType::Gold;
	MyPlayer->inventorySlot[2]._itype = ItemType::Gold;
	MyPlayer->inventorySlot[3]._itype = ItemType::Gold;
	// Set the gold amount to arbitrary values
	MyPlayer->inventorySlot[1]._ivalue = 100;
	MyPlayer->inventorySlot[5]._ivalue = 200;
	MyPlayer->inventorySlot[2]._ivalue = 3;
	MyPlayer->inventorySlot[3]._ivalue = 30;

	EXPECT_EQ(CalculateGold(*MyPlayer), 333);
}

// Test automatic gold placing
TEST_F(InvTest, GoldAutoPlace)
{
	SNetInitializeProvider(SELCONN_LOOPBACK, nullptr);

	// Empty the inventory
	clear_inventory();

	// Put gold into the inventory:
	// | 1000 | ... | ...
	MyPlayer->inventorySlot[0]._itype = ItemType::Gold;
	MyPlayer->inventorySlot[0]._ivalue = 1000;
	MyPlayer->numInventoryItems = 1;
	// Put (max gold - 100) gold, which is 4900, into the player's hand
	MyPlayer->heldItem._itype = ItemType::Gold;
	MyPlayer->heldItem._ivalue = GOLD_MAX_LIMIT - 100;

	GoldAutoPlace(*MyPlayer, MyPlayer->heldItem);
	// We expect the inventory:
	// | 5000 | 900 | ...
	EXPECT_EQ(MyPlayer->inventorySlot[0]._ivalue, GOLD_MAX_LIMIT);
	EXPECT_EQ(MyPlayer->inventorySlot[1]._ivalue, 900);
}

// Test removing an item from inventory with no other items.
TEST_F(InvTest, RemoveInvItem)
{
	SNetInitializeProvider(SELCONN_LOOPBACK, nullptr);

	clear_inventory();
	// Put a two-slot misc item into the inventory:
	// | (item) | (item) | ... | ...
	MyPlayer->numInventoryItems = 1;
	MyPlayer->inventoryGrid[0] = 1;
	MyPlayer->inventoryGrid[1] = -1;
	MyPlayer->inventorySlot[0]._itype = ItemType::Misc;

	MyPlayer->RemoveInvItem(0);
	EXPECT_EQ(MyPlayer->inventoryGrid[0], 0);
	EXPECT_EQ(MyPlayer->inventoryGrid[1], 0);
	EXPECT_EQ(MyPlayer->numInventoryItems, 0);
}

// Test removing an item from inventory with other items in it.
TEST_F(InvTest, RemoveInvItem_other_item)
{
	SNetInitializeProvider(SELCONN_LOOPBACK, nullptr);

	clear_inventory();
	// Put a two-slot misc item and a ring into the inventory:
	// | (item) | (item) | (ring) | ...
	MyPlayer->numInventoryItems = 2;
	MyPlayer->inventoryGrid[0] = 1;
	MyPlayer->inventoryGrid[1] = -1;
	MyPlayer->inventorySlot[0]._itype = ItemType::Misc;

	MyPlayer->inventoryGrid[2] = 2;
	MyPlayer->inventorySlot[1]._itype = ItemType::Ring;

	MyPlayer->RemoveInvItem(0);
	EXPECT_EQ(MyPlayer->inventoryGrid[0], 0);
	EXPECT_EQ(MyPlayer->inventoryGrid[1], 0);
	EXPECT_EQ(MyPlayer->inventoryGrid[2], 1);
	EXPECT_EQ(MyPlayer->inventorySlot[0]._itype, ItemType::Ring);
	EXPECT_EQ(MyPlayer->numInventoryItems, 1);
}

// Test removing an item from the belt
TEST_F(InvTest, RemoveSpdBarItem)
{
	SNetInitializeProvider(SELCONN_LOOPBACK, nullptr);

	// Clear the belt
	for (int i = 0; i < MaxBeltItems; i++) {
		MyPlayer->beltSlot[i].clear();
	}
	// Put an item in the belt: | x | x | item | x | x | x | x | x |
	MyPlayer->beltSlot[3]._itype = ItemType::Misc;

	MyPlayer->RemoveSpdBarItem(3);
	EXPECT_TRUE(MyPlayer->beltSlot[3].isEmpty());
}

// Test removing a scroll from the inventory
TEST_F(InvTest, RemoveCurrentSpellScrollFromInventory)
{
	clear_inventory();

	// Put a firebolt scroll into the inventory
	MyPlayer->numInventoryItems = 1;
	MyPlayer->executedSpell.spellId = SpellID::Firebolt;
	MyPlayer->executedSpell.spellFrom = INVITEM_INV_FIRST;
	MyPlayer->inventorySlot[0]._itype = ItemType::Misc;
	MyPlayer->inventorySlot[0]._iMiscId = IMISC_SCROLL;
	MyPlayer->inventorySlot[0]._iSpell = SpellID::Firebolt;

	ConsumeScroll(*MyPlayer);
	EXPECT_EQ(MyPlayer->inventoryGrid[0], 0);
	EXPECT_EQ(MyPlayer->numInventoryItems, 0);
}

// Test removing the first matching scroll from inventory
TEST_F(InvTest, RemoveCurrentSpellScrollFromInventoryFirstMatch)
{
	clear_inventory();

	// Put a firebolt scroll into the inventory
	MyPlayer->numInventoryItems = 1;
	MyPlayer->executedSpell.spellId = SpellID::Firebolt;
	MyPlayer->executedSpell.spellFrom = 0; // any matching scroll
	MyPlayer->inventorySlot[0]._itype = ItemType::Misc;
	MyPlayer->inventorySlot[0]._iMiscId = IMISC_SCROLL;
	MyPlayer->inventorySlot[0]._iSpell = SpellID::Firebolt;

	ConsumeScroll(*MyPlayer);
	EXPECT_EQ(MyPlayer->inventoryGrid[0], 0);
	EXPECT_EQ(MyPlayer->numInventoryItems, 0);
}

// Test removing a scroll from the belt
TEST_F(InvTest, RemoveCurrentSpellScroll_belt)
{
	SNetInitializeProvider(SELCONN_LOOPBACK, nullptr);

	// Clear the belt
	for (int i = 0; i < MaxBeltItems; i++) {
		MyPlayer->beltSlot[i].clear();
	}
	// Put a firebolt scroll into the belt
	MyPlayer->executedSpell.spellId = SpellID::Firebolt;
	MyPlayer->executedSpell.spellFrom = INVITEM_BELT_FIRST + 3;
	MyPlayer->beltSlot[3]._itype = ItemType::Misc;
	MyPlayer->beltSlot[3]._iMiscId = IMISC_SCROLL;
	MyPlayer->beltSlot[3]._iSpell = SpellID::Firebolt;

	ConsumeScroll(*MyPlayer);
	EXPECT_TRUE(MyPlayer->beltSlot[3].isEmpty());
}

// Test removing the first matching scroll from the belt
TEST_F(InvTest, RemoveCurrentSpellScrollFirstMatchFromBelt)
{
	SNetInitializeProvider(SELCONN_LOOPBACK, nullptr);

	// Clear the belt
	for (int i = 0; i < MaxBeltItems; i++) {
		MyPlayer->beltSlot[i].clear();
	}
	// Put a firebolt scroll into the belt
	MyPlayer->executedSpell.spellId = SpellID::Firebolt;
	MyPlayer->executedSpell.spellFrom = 0; // any matching scroll
	MyPlayer->beltSlot[3]._itype = ItemType::Misc;
	MyPlayer->beltSlot[3]._iMiscId = IMISC_SCROLL;
	MyPlayer->beltSlot[3]._iSpell = SpellID::Firebolt;

	ConsumeScroll(*MyPlayer);
	EXPECT_TRUE(MyPlayer->beltSlot[3].isEmpty());
}

TEST_F(InvTest, ItemSizeRuneOfStone)
{
	// Inventory sizes are currently determined by examining the sprite size
	// rune of stone and grey suit are adjacent in the sprite list so provide an easy check for off-by-one errors
	if (!gbIsHellfire) return;
	Item testItem {};
	InitializeItem(testItem, IDI_RUNEOFSTONE);
	EXPECT_EQ(GetInventorySize(testItem), Size(1, 1));
}

TEST_F(InvTest, ItemSizeGreySuit)
{
	if (!gbIsHellfire) return;
	Item testItem {};
	InitializeItem(testItem, IDI_GREYSUIT);
	EXPECT_EQ(GetInventorySize(testItem), Size(2, 2));
}

TEST_F(InvTest, ItemSizeAuric)
{
	// Auric amulet is the first used hellfire sprite, but there's multiple unused sprites before it in the list.
	// unfortunately they're the same size so this is less valuable as a test.
	if (!gbIsHellfire) return;
	Item testItem {};
	InitializeItem(testItem, IDI_AURIC);
	EXPECT_EQ(GetInventorySize(testItem), Size(1, 1));
}

TEST_F(InvTest, ItemSizeLastDiabloItem)
{
	// Short battle bow is the last diablo sprite, off by ones will end up loading a 1x1 unused sprite from hellfire,.
	Item testItem {};
	InitializeItem(testItem, IDI_SHORT_BATTLE_BOW);
	EXPECT_EQ(GetInventorySize(testItem), Size(2, 3));
}

} // namespace
} // namespace devilution
