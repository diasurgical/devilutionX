#include <fmt/format.h>
#include <gtest/gtest.h>

#include "drlg_l3.h"
#include "engine/load_file.hpp"
#include "gendung.h"
#include "quests.h"
#include "utils/paths.h"

using namespace devilution;

namespace {

void TestCreateL3Dungeon(int level, uint32_t seed, lvl_entry entry)
{
	paths::SetPrefPath(paths::BasePath());

	std::string dunPath;

	if (level >= 9 && level <= 12) {
		dunPath = fmt::format("test/fixtures/diablo/{}-{}.dun", level, seed);
		pMegaTiles = std::make_unique<MegaTile[]>(206);
		leveltype = DTYPE_CAVES;
	} else if (level >= 17 && level <= 20) {
		dunPath = fmt::format("test/fixtures/hellfire/{}-{}.dun", level, seed);
		pMegaTiles = std::make_unique<MegaTile[]>(166);
		leveltype = DTYPE_NEST;
	}

	currlevel = level;
	CreateL3Dungeon(seed, entry);

	auto dunData = LoadFileInMem<uint16_t>(dunPath.c_str());
	ASSERT_NE(dunData, nullptr) << "Unable to load test fixture " << dunPath;
	ASSERT_EQ(Size(DMAXX, DMAXY), Size(dunData[0], dunData[1]));

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

TEST(Drlg_l3, CreateL3Dungeon_diablo_9_262005438)
{
	TestCreateL3Dungeon(9, 262005438, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(41, 73));
	TestCreateL3Dungeon(9, 262005438, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(73, 59));
}

TEST(Drlg_l3, CreateL3Dungeon_diablo_10_1630062353)
{
	Quests[Q_ANVIL]._qactive = QUEST_NOTAVAIL;

	TestCreateL3Dungeon(10, 1630062353, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(55, 37));
	TestCreateL3Dungeon(10, 1630062353, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(19, 47));
}

TEST(Drlg_l3, CreateL3Dungeon_diablo_11_384626536)
{
	TestCreateL3Dungeon(11, 384626536, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(29, 19));
	TestCreateL3Dungeon(11, 384626536, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(65, 65));
}

TEST(Drlg_l3, CreateL3Dungeon_diablo_12_2104541047)
{
	TestCreateL3Dungeon(12, 2104541047, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(35, 23));
	TestCreateL3Dungeon(12, 2104541047, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(21, 83));
}

TEST(Drlg_l3, CreateL3Dungeon_hive_1_19770182)
{
	TestCreateL3Dungeon(17, 19770182, ENTRY_TWARPUP);
	EXPECT_EQ(ViewPosition, Point(75, 81));
	TestCreateL3Dungeon(17, 19770182, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(59, 41));
}

TEST(Drlg_l3, CreateL3Dungeon_hive_2_1522546307)
{
	TestCreateL3Dungeon(18, 1522546307, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(47, 19));
	TestCreateL3Dungeon(18, 1522546307, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(33, 35));
}

TEST(Drlg_l3, CreateL3Dungeon_hive_3_125121312)
{
	TestCreateL3Dungeon(19, 125121312, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(61, 25));
	TestCreateL3Dungeon(19, 125121312, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(21, 85));
}

TEST(Drlg_l3, CreateL3Dungeon_hive_4_1511478689)
{
	TestCreateL3Dungeon(20, 1511478689, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(65, 41));
	TestCreateL3Dungeon(20, 1511478689, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(65, 41));
}

} // namespace
