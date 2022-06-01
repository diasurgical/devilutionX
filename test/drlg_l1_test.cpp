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
	pMegaTiles = std::make_unique<MegaTile[]>(1648);

	MyPlayer->pOriginalCathedral = !hellfire;

	currlevel = level;
	leveltype = DTYPE_CATHEDRAL;

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
			ASSERT_EQ(dungeon[x][y], tileId);
		}
	}

	const uint16_t *transparentLayer = &dunData[2 + DMAXX * DMAXY * 13];

	for (int y = 16; y < 16 + DMAXY * 2; y++) {
		for (int x = 16; x < 16 + DMAXX * 2; x++) {
			auto sectorId = static_cast<uint8_t>(SDL_SwapLE16(*transparentLayer));
			transparentLayer++;
			ASSERT_EQ(dTransVal[x][y], sectorId) << "Room/region indexes don't match";
		}
	}
}

TEST(Drlg_l1, CreateL5Dungeon_diablo_1_743271966)
{
	TestCreateL5Dungeon(false, 1, 743271966, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition.x, 51);
	EXPECT_EQ(ViewPosition.y, 82);
	TestCreateL5Dungeon(false, 1, 743271966, ENTRY_PREV);
	EXPECT_EQ(ViewPosition.x, 81);
	EXPECT_EQ(ViewPosition.y, 47);
}

TEST(Drlg_l1, CreateL5Dungeon_diablo_2_1383137027)
{
	Quests[Q_PWATER]._qlevel = 2;
	Quests[Q_PWATER]._qactive = QUEST_INIT;

	TestCreateL5Dungeon(false, 2, 1383137027, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition.x, 57);
	EXPECT_EQ(ViewPosition.y, 74);
	TestCreateL5Dungeon(false, 2, 1383137027, ENTRY_PREV);
	EXPECT_EQ(ViewPosition.x, 57);
	EXPECT_EQ(ViewPosition.y, 79);
}

TEST(Drlg_l1, CreateL5Dungeon_diablo_3_844660068)
{
	TestCreateL5Dungeon(false, 3, 844660068, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition.x, 67);
	EXPECT_EQ(ViewPosition.y, 52);
	TestCreateL5Dungeon(false, 3, 844660068, ENTRY_PREV);
	EXPECT_EQ(ViewPosition.x, 85);
	EXPECT_EQ(ViewPosition.y, 45);
}

TEST(Drlg_l1, CreateL5Dungeon_diablo_4_609325643)
{
	TestCreateL5Dungeon(false, 4, 609325643, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition.x, 85);
	EXPECT_EQ(ViewPosition.y, 78);
	TestCreateL5Dungeon(false, 4, 609325643, ENTRY_PREV);
	EXPECT_EQ(ViewPosition.x, 55);
	EXPECT_EQ(ViewPosition.y, 47);
}

TEST(Drlg_l1, CreateL5Dungeon_hellfire_1_401921334)
{
	TestCreateL5Dungeon(true, 1, 401921334, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition.x, 79);
	EXPECT_EQ(ViewPosition.y, 80);
	TestCreateL5Dungeon(true, 1, 401921334, ENTRY_PREV);
	EXPECT_EQ(ViewPosition.x, 49);
	EXPECT_EQ(ViewPosition.y, 63);
}

TEST(Drlg_l1, CreateL5Dungeon_hellfire_2_128964898)
{
	Quests[Q_PWATER]._qactive = QUEST_NOTAVAIL;

	TestCreateL5Dungeon(true, 2, 128964898, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition.x, 55);
	EXPECT_EQ(ViewPosition.y, 68);
	TestCreateL5Dungeon(true, 2, 128964898, ENTRY_PREV);
	EXPECT_EQ(ViewPosition.x, 49);
	EXPECT_EQ(ViewPosition.y, 63);
}

TEST(Drlg_l1, CreateL5Dungeon_hellfire_3_1799396623)
{
	TestCreateL5Dungeon(true, 3, 1799396623, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition.x, 59);
	EXPECT_EQ(ViewPosition.y, 68);
	TestCreateL5Dungeon(true, 3, 1799396623, ENTRY_PREV);
	EXPECT_EQ(ViewPosition.x, 47);
	EXPECT_EQ(ViewPosition.y, 55);
}

TEST(Drlg_l1, CreateL5Dungeon_hellfire_4_1190318991)
{
	TestCreateL5Dungeon(true, 4, 1190318991, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition.x, 67);
	EXPECT_EQ(ViewPosition.y, 80);
	TestCreateL5Dungeon(true, 4, 1190318991, ENTRY_PREV);
	EXPECT_EQ(ViewPosition.x, 77);
	EXPECT_EQ(ViewPosition.y, 45);
}

} // namespace
