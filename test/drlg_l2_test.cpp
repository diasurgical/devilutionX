#include <fmt/format.h>
#include <gtest/gtest.h>

#include "drlg_l2.h"
#include "engine/load_file.hpp"
#include "gendung.h"
#include "utils/paths.h"

using namespace devilution;

namespace {

void TestCreateL2Dungeon(int level, uint32_t seed, lvl_entry entry)
{
	pMegaTiles = std::make_unique<MegaTile[]>(160);
	leveltype = DTYPE_CATACOMBS;

	currlevel = level;
	CreateL2Dungeon(seed, entry);

	std::string path = paths::BasePath();

	paths::SetPrefPath(path);
	std::string dunPath = fmt::format("../test/fixtures/diablo/{}-{}.dun", level, seed);
	auto dunData = LoadFileInMem<uint16_t>(dunPath.c_str());
	ASSERT_EQ(DMAXX, dunData[0]);
	ASSERT_EQ(DMAXY, dunData[1]);

	const uint16_t *tileLayer = &dunData[2];

	for (int y = 0; y < DMAXY; y++) {
		for (int x = 0; x < DMAXX; x++) {
			auto tileId = static_cast<uint8_t>(SDL_SwapLE16(*tileLayer));
			tileLayer++;
			ASSERT_EQ(dungeon[x][y], tileId) << "Tile don't match at " << x << "x" << y;
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

TEST(Drlg_l1, CreateL2Dungeon_diablo_5_1677631846)
{
	TestCreateL2Dungeon(5, 1677631846, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition.x, 27);
	EXPECT_EQ(ViewPosition.y, 28);
	TestCreateL2Dungeon(5, 1677631846, ENTRY_PREV);
	EXPECT_EQ(ViewPosition.x, 26);
	EXPECT_EQ(ViewPosition.y, 62);
}

TEST(Drlg_l1, CreateL2Dungeon_diablo_6_2034738122)
{
	TestCreateL2Dungeon(6, 2034738122, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition.x, 33);
	EXPECT_EQ(ViewPosition.y, 26);
	TestCreateL2Dungeon(6, 2034738122, ENTRY_PREV);
	EXPECT_EQ(ViewPosition.x, 34);
	EXPECT_EQ(ViewPosition.y, 52);
}

TEST(Drlg_l1, CreateL2Dungeon_diablo_7_680552750)
{
	TestCreateL2Dungeon(7, 680552750, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition.x, 27);
	EXPECT_EQ(ViewPosition.y, 26);
	TestCreateL2Dungeon(7, 680552750, ENTRY_PREV);
	EXPECT_EQ(ViewPosition.x, 78);
	EXPECT_EQ(ViewPosition.y, 52);
}

TEST(Drlg_l1, CreateL2Dungeon_diablo_8_1999936419)
{
	TestCreateL2Dungeon(8, 1999936419, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition.x, 39);
	EXPECT_EQ(ViewPosition.y, 74);
	TestCreateL2Dungeon(8, 1999936419, ENTRY_PREV);
	EXPECT_EQ(ViewPosition.x, 48);
	EXPECT_EQ(ViewPosition.y, 46);
}

} // namespace
