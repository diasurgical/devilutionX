#include <fmt/format.h>
#include <gtest/gtest.h>

#include "drlg_test.hpp"
#include "gendung.h"

using namespace devilution;

namespace {

TEST(Drlg_l2, CreateL2Dungeon_diablo_5_1677631846)
{
	LoadExpectedLevelData("diablo/5-1677631846.dun");

	TestCreateDungeon(5, 1677631846, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(27, 28));
	TestCreateDungeon(5, 1677631846, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(26, 62));
}

TEST(Drlg_l2, CreateL2Dungeon_diablo_6_2034738122)
{
	LoadExpectedLevelData("diablo/6-2034738122.dun");

	TestCreateDungeon(6, 2034738122, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(33, 26));
	TestCreateDungeon(6, 2034738122, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(34, 52));
}

TEST(Drlg_l2, CreateL2Dungeon_diablo_7_680552750)
{
	LoadExpectedLevelData("diablo/7-680552750.dun");

	TestCreateDungeon(7, 680552750, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(27, 26));
	TestCreateDungeon(7, 680552750, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(78, 52));
}

TEST(Drlg_l2, CreateL2Dungeon_diablo_8_1999936419)
{
	LoadExpectedLevelData("diablo/8-1999936419.dun");

	TestCreateDungeon(8, 1999936419, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(39, 74));
	TestCreateDungeon(8, 1999936419, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(48, 46));
}

} // namespace
