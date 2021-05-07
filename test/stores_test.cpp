#include <gtest/gtest.h>

#include "stores.h"

using namespace devilution;

namespace {

TEST(Stores, AddStoreHoldRepair_magic)
{
	ItemStruct *item;

	item = &storehold[0];

	item->_iMaxDur = 60;
	item->_iDurability = item->_iMaxDur;
	item->_iMagical = ITEM_QUALITY_MAGIC;
	item->_iIdentified = true;
	item->_ivalue = 2000;
	item->_iIvalue = 19000;

	for (int i = 1; i < item->_iMaxDur; i++) {
		item->_ivalue = 2000;
		item->_iIvalue = 19000;
		item->_iDurability = i;
		storenumh = 0;
		AddStoreHoldRepair(item, 0);
		EXPECT_EQ(1, storenumh);
		EXPECT_EQ(95 * (item->_iMaxDur - i) / 2, item->_ivalue);
	}

	item->_iDurability = 59;
	storenumh = 0;
	item->_ivalue = 500;
	item->_iIvalue = 30; // To cheap to repair
	AddStoreHoldRepair(item, 0);
	EXPECT_EQ(0, storenumh);
	EXPECT_EQ(30, item->_iIvalue);
	EXPECT_EQ(500, item->_ivalue);
}

TEST(Stores, AddStoreHoldRepair_normal)
{
	ItemStruct *item;

	item = &storehold[0];

	item->_iMaxDur = 20;
	item->_iDurability = item->_iMaxDur;
	item->_iMagical = ITEM_QUALITY_NORMAL;
	item->_iIdentified = true;
	item->_ivalue = 2000;
	item->_iIvalue = item->_ivalue;

	for (int i = 1; i < item->_iMaxDur; i++) {
		item->_ivalue = 2000;
		item->_iIvalue = item->_ivalue;
		item->_iDurability = i;
		storenumh = 0;
		AddStoreHoldRepair(item, 0);
		EXPECT_EQ(1, storenumh);
		EXPECT_EQ(50 * (item->_iMaxDur - i), item->_ivalue);
	}

	item->_iDurability = 19;
	storenumh = 0;
	item->_ivalue = 10; // less then 1 per dur
	item->_iIvalue = item->_ivalue;
	AddStoreHoldRepair(item, 0);
	EXPECT_EQ(1, storenumh);
	EXPECT_EQ(1, item->_ivalue);
	EXPECT_EQ(1, item->_iIvalue);
}
} // namespace
