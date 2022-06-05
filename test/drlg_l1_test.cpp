#include <gtest/gtest.h>

#include "drlg_test.hpp"
#include "gendung.h"
#include "lighting.h"
#include "player.h"
#include "quests.h"

using namespace devilution;

namespace {

TEST(Drlg_l1, DRLG_Init_Globals_noflag)
{
	DisableLighting = false;
	DRLG_Init_Globals();
	EXPECT_EQ(dLight[0][0], 15);
}

TEST(Drlg_l1, DRLG_Init_Globals)
{
	DisableLighting = true;
	DRLG_Init_Globals();
	EXPECT_EQ(dLight[0][0], 0);
}

TEST(Drlg_l1, CreateL5Dungeon_diablo_1_743271966)
{
	LoadExpectedLevelData("diablo/1-743271966.dun");

	MyPlayer->pOriginalCathedral = true;

	TestCreateDungeon(1, 743271966, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(51, 82));
	TestCreateDungeon(1, 743271966, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(81, 47));
}

TEST(Drlg_l1, CreateL5Dungeon_diablo_2_1383137027)
{
	LoadExpectedLevelData("diablo/2-1383137027.dun");

	MyPlayer->pOriginalCathedral = true;

	Quests[Q_PWATER]._qlevel = 2;
	Quests[Q_PWATER]._qactive = QUEST_INIT;
	Quests[Q_BUTCHER]._qactive = QUEST_NOTAVAIL;

	TestCreateDungeon(2, 1383137027, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(57, 74));
	TestCreateDungeon(2, 1383137027, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(57, 79));
}

TEST(Drlg_l1, CreateL5Dungeon_diablo_3_844660068)
{
	LoadExpectedLevelData("diablo/3-844660068.dun");

	MyPlayer->pOriginalCathedral = true;
	Quests[Q_SKELKING]._qactive = QUEST_NOTAVAIL;

	TestCreateDungeon(3, 844660068, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(67, 52));
	TestCreateDungeon(3, 844660068, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(85, 45));
}

TEST(Drlg_l1, CreateL5Dungeon_diablo_4_609325643)
{
	LoadExpectedLevelData("diablo/4-609325643.dun");

	MyPlayer->pOriginalCathedral = true;
	Quests[Q_LTBANNER]._qactive = QUEST_NOTAVAIL;

	TestCreateDungeon(4, 609325643, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(85, 78));
	TestCreateDungeon(4, 609325643, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(55, 47));
}

TEST(Drlg_l1, CreateL5Dungeon_hellfire_1_401921334)
{
	LoadExpectedLevelData("hellfire/1-401921334.dun");

	MyPlayer->pOriginalCathedral = false;

	TestCreateDungeon(1, 401921334, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(79, 80));
	TestCreateDungeon(1, 401921334, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(49, 63));
}

TEST(Drlg_l1, CreateL5Dungeon_hellfire_2_128964898)
{
	LoadExpectedLevelData("hellfire/2-128964898.dun");

	MyPlayer->pOriginalCathedral = false;
	Quests[Q_PWATER]._qactive = QUEST_NOTAVAIL;
	Quests[Q_BUTCHER]._qactive = QUEST_NOTAVAIL;

	TestCreateDungeon(2, 128964898, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(55, 68));
	TestCreateDungeon(2, 128964898, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(49, 63));
}

TEST(Drlg_l1, CreateL5Dungeon_hellfire_3_1799396623)
{
	LoadExpectedLevelData("hellfire/3-1799396623.dun");

	MyPlayer->pOriginalCathedral = false;
	Quests[Q_SKELKING]._qactive = QUEST_NOTAVAIL;

	TestCreateDungeon(3, 1799396623, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(59, 68));
	TestCreateDungeon(3, 1799396623, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(47, 55));
}

TEST(Drlg_l1, CreateL5Dungeon_hellfire_3_1512491184)
{
	LoadExpectedLevelData("hellfire/3-1512491184.dun");

	MyPlayer->pOriginalCathedral = false;
	Quests[Q_SKELKING]._qlevel = 3;
	Quests[Q_SKELKING]._qactive = QUEST_INIT;

	TestCreateDungeon(3, 1512491184, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(47, 72));
	TestCreateDungeon(3, 1512491184, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(79, 45));
}

TEST(Drlg_l1, CreateL5Dungeon_hellfire_4_1190318991)
{
	LoadExpectedLevelData("hellfire/4-1190318991.dun");

	MyPlayer->pOriginalCathedral = false;
	Quests[Q_LTBANNER]._qactive = QUEST_NOTAVAIL;

	TestCreateDungeon(4, 1190318991, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(67, 80));
	TestCreateDungeon(4, 1190318991, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(77, 45));
}

TEST(Drlg_l1, CreateL5Dungeon_hellfire_4_1924296259)
{
	LoadExpectedLevelData("hellfire/4-1924296259.dun");

	MyPlayer->pOriginalCathedral = false;
	Quests[Q_LTBANNER]._qlevel = 4;
	Quests[Q_LTBANNER]._qactive = QUEST_INIT;

	TestCreateDungeon(4, 1924296259, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(83, 54));
	TestCreateDungeon(4, 1924296259, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(52, 88));
}

TEST(Drlg_l1, CreateL5Dungeon_crypt_1_2122696790)
{
	LoadExpectedLevelData("hellfire/21-2122696790.dun");

	TestCreateDungeon(21, 2122696790, ENTRY_TWARPUP);
	EXPECT_EQ(ViewPosition, Point(61, 80));
	TestCreateDungeon(21, 2122696790, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(53, 67));
}

TEST(Drlg_l1, CreateL5Dungeon_crypt_2_1191662129)
{
	LoadExpectedLevelData("hellfire/22-1191662129.dun");

	TestCreateDungeon(22, 1191662129, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(71, 47));
	TestCreateDungeon(22, 1191662129, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(85, 71));
}

TEST(Drlg_l1, CreateL5Dungeon_crypt_3_97055268)
{
	LoadExpectedLevelData("hellfire/23-97055268.dun");

	TestCreateDungeon(23, 97055268, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(71, 57));
	TestCreateDungeon(23, 97055268, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(81, 59));
}

TEST(Drlg_l1, CreateL5Dungeon_crypt_4_1324803725)
{
	LoadExpectedLevelData("hellfire/24-1324803725.dun");

	TestCreateDungeon(24, 1324803725, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(79, 47));
}

} // namespace
