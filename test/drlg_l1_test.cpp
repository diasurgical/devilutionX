#include <fmt/format.h>
#include <gtest/gtest.h>

#include "diablo.h"
#include "drlg_l1.h"
#include "engine/load_file.hpp"
#include "gendung.h"
#include "lighting.h"
#include "player.h"
#include "quests.h"
#include "utils/paths.h"

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

void TestCreateL5Dungeon(bool hellfire, int level, uint32_t seed, lvl_entry entry)
{
	if (level >= 1 && level <= 4) {
		MyPlayer->pOriginalCathedral = !hellfire;
		pMegaTiles = std::make_unique<MegaTile[]>(206);
		leveltype = DTYPE_CATHEDRAL;
	} else if (level >= 21 && level <= 24) {
		pMegaTiles = std::make_unique<MegaTile[]>(217);
		leveltype = DTYPE_CRYPT;
	}

	currlevel = level;
	CreateL5Dungeon(seed, entry);

	std::string path = paths::BasePath();

	paths::SetPrefPath(path);
	std::string dunPath;
	if (hellfire)
		dunPath = fmt::format("../test/fixtures/hellfire/{}-{}.dun", level, seed);
	else
		dunPath = fmt::format("../test/fixtures/diablo/{}-{}.dun", level, seed);
	auto dunData = LoadFileInMem<uint16_t>(dunPath.c_str());
	ASSERT_EQ(DMAXX, dunData[0]);
	ASSERT_EQ(DMAXY, dunData[1]);

	const uint16_t *tileLayer = &dunData[2];

	for (int y = 0; y < DMAXY; y++) {
		for (int x = 0; x < DMAXX; x++) {
			auto tileId = static_cast<uint8_t>(SDL_SwapLE16(*tileLayer));
			tileLayer++;
			ASSERT_EQ(dungeon[x][y], tileId) << "Tiles don't match at " << x << "x" << y;
		}
	}

	const uint16_t *transparentLayer = &dunData[2 + DMAXX * DMAXY * 13];

	for (int y = 16; y < 16 + DMAXY * 2; y++) {
		for (int x = 16; x < 16 + DMAXX * 2; x++) {
			auto sectorId = static_cast<uint8_t>(SDL_SwapLE16(*transparentLayer));
			transparentLayer++;
			ASSERT_EQ(dTransVal[x][y], sectorId) << "Room/region indexes don't match at " << x << "x" << y;
		}
	}
}

TEST(Drlg_l1, CreateL5Dungeon_diablo_1_743271966)
{
	TestCreateL5Dungeon(false, 1, 743271966, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(51, 82));
	TestCreateL5Dungeon(false, 1, 743271966, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(81, 47));
}

TEST(Drlg_l1, CreateL5Dungeon_diablo_2_1383137027)
{
	Quests[Q_PWATER]._qlevel = 2;
	Quests[Q_PWATER]._qactive = QUEST_INIT;

	TestCreateL5Dungeon(false, 2, 1383137027, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(57, 74));
	TestCreateL5Dungeon(false, 2, 1383137027, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(57, 79));
}

TEST(Drlg_l1, CreateL5Dungeon_diablo_3_844660068)
{
	TestCreateL5Dungeon(false, 3, 844660068, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(67, 52));
	TestCreateL5Dungeon(false, 3, 844660068, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(85, 45));
}

TEST(Drlg_l1, CreateL5Dungeon_diablo_4_609325643)
{
	TestCreateL5Dungeon(false, 4, 609325643, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(85, 78));
	TestCreateL5Dungeon(false, 4, 609325643, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(55, 47));
}

TEST(Drlg_l1, CreateL5Dungeon_hellfire_1_401921334)
{
	TestCreateL5Dungeon(true, 1, 401921334, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(79, 80));
	TestCreateL5Dungeon(true, 1, 401921334, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(49, 63));
}

TEST(Drlg_l1, CreateL5Dungeon_hellfire_2_128964898)
{
	Quests[Q_PWATER]._qactive = QUEST_NOTAVAIL;

	TestCreateL5Dungeon(true, 2, 128964898, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(55, 68));
	TestCreateL5Dungeon(true, 2, 128964898, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(49, 63));
}

TEST(Drlg_l1, CreateL5Dungeon_hellfire_3_1799396623)
{
	TestCreateL5Dungeon(true, 3, 1799396623, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(59, 68));
	TestCreateL5Dungeon(true, 3, 1799396623, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(47, 55));
}

TEST(Drlg_l1, CreateL5Dungeon_hellfire_4_1190318991)
{
	TestCreateL5Dungeon(true, 4, 1190318991, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(67, 80));
	TestCreateL5Dungeon(true, 4, 1190318991, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(77, 45));
}

TEST(Drlg_l1, CreateL5Dungeon_crypt_1_2122696790)
{
	TestCreateL5Dungeon(true, 21, 2122696790, ENTRY_TWARPUP);
	EXPECT_EQ(ViewPosition, Point(61, 80));
	TestCreateL5Dungeon(true, 21, 2122696790, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(53, 67));
}

TEST(Drlg_l1, CreateL5Dungeon_crypt_2_1191662129)
{
	Quests[Q_PWATER]._qactive = QUEST_NOTAVAIL;

	TestCreateL5Dungeon(true, 22, 1191662129, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(71, 47));
	TestCreateL5Dungeon(true, 22, 1191662129, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(85, 71));
}

TEST(Drlg_l1, CreateL5Dungeon_crypt_3_97055268)
{
	TestCreateL5Dungeon(true, 23, 97055268, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(71, 57));
	TestCreateL5Dungeon(true, 23, 97055268, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(81, 59));
}

TEST(Drlg_l1, CreateL5Dungeon_crypt_4_1324803725)
{
	TestCreateL5Dungeon(true, 24, 1324803725, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(79, 47));
}

} // namespace
