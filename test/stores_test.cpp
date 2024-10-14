#include <gtest/gtest.h>

#include "stores.h"

using namespace devilution;

namespace {

// Helper function to reset the playerItems vector before each test
void ResetPlayerItems()
{
	playerItems.clear();
}

// This is a direct copy of FilterRepairableItems logic for testing purposes
void Test_FilterRepairableItems()
{
	playerItems.erase(std::remove_if(playerItems.begin(), playerItems.end(),
	                      [](const IndexedItem &indexedItem) {
		                      const Item &itemPtr = *indexedItem.itemPtr;
		                      return itemPtr._iDurability == itemPtr._iMaxDur || itemPtr._iMaxDur == DUR_INDESTRUCTIBLE;
	                      }),
	    playerItems.end());
}

TEST(Stores, FilterRepairableItems_magic)
{
	// Reset playerItems before starting the test
	ResetPlayerItems();

	// Create a magic item with durability and add it to the player's inventory
	Item magicItem;
	magicItem._iMaxDur = 60;
	magicItem._iDurability = magicItem._iMaxDur - 1;
	magicItem._iMagical = ITEM_QUALITY_MAGIC;
	magicItem._iIdentified = true;
	magicItem._ivalue = 2000;
	magicItem._iIvalue = 19000;

	// Add the item to the player's inventory
	playerItems.push_back({ &magicItem, ItemLocation::Inventory, 0 });

	// Call the filtering function to remove non-repairable items
	Test_FilterRepairableItems();

	// Check that the playerItems vector contains the magic item and its values are correct
	ASSERT_EQ(playerItems.size(), 1);
	EXPECT_EQ(playerItems[0].itemPtr->_ivalue, 2000);    // Item's value should not change
	EXPECT_EQ(playerItems[0].itemPtr->_iDurability, 59); // Durability should match
}

TEST(Stores, FilterRepairableItems_normal)
{
	// Reset playerItems before starting the test
	ResetPlayerItems();

	// Create a normal item with durability and add it to the player's inventory
	Item normalItem;
	normalItem._iMaxDur = 20;
	normalItem._iDurability = normalItem._iMaxDur - 1;
	normalItem._iMagical = ITEM_QUALITY_NORMAL;
	normalItem._iIdentified = true;
	normalItem._ivalue = 2000;

	// Add the item to the player's inventory
	playerItems.push_back({ &normalItem, ItemLocation::Inventory, 0 });

	// Call the filtering function to remove non-repairable items
	Test_FilterRepairableItems();

	// Check that the playerItems vector contains the normal item and its values are correct
	ASSERT_EQ(playerItems.size(), 1);
	EXPECT_EQ(playerItems[0].itemPtr->_ivalue, 2000);    // Item's value should not change
	EXPECT_EQ(playerItems[0].itemPtr->_iDurability, 19); // Durability should match
}

TEST(Stores, FilterRepairableItems_no_repair)
{
	// Reset playerItems before starting the test
	ResetPlayerItems();

	// Create an item that cannot be repaired (already at max durability)
	Item indestructibleItem;
	indestructibleItem._iMaxDur = DUR_INDESTRUCTIBLE; // Indestructible item
	indestructibleItem._iDurability = 100;
	indestructibleItem._iMagical = ITEM_QUALITY_MAGIC;
	indestructibleItem._iIdentified = true;
	indestructibleItem._ivalue = 5000;

	// Add the item to the player's inventory
	playerItems.push_back({ &indestructibleItem, ItemLocation::Inventory, 0 });

	// Call the filtering function to remove non-repairable items
	Test_FilterRepairableItems();

	// Check that the playerItems vector is empty since the item is indestructible
	ASSERT_EQ(playerItems.size(), 0);
}

} // namespace
