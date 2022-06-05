#include <fmt/format.h>
#include <gtest/gtest.h>

#include "drlg_l4.h"
#include "engine/load_file.hpp"
#include "gendung.h"
#include "quests.h"
#include "utils/paths.h"

using namespace devilution;

namespace {

void TestCreateL4Dungeon(bool changed, int level, uint32_t seed, lvl_entry entry)
{
	pMegaTiles = std::make_unique<MegaTile[]>(137);
	leveltype = DTYPE_HELL;

	currlevel = level;
	CreateL4Dungeon(seed, entry);

	std::string path = paths::BasePath();

	paths::SetPrefPath(path);
	std::string dunPath;
	if (changed)
		dunPath = fmt::format("test/fixtures/diablo/{}-{}-changed.dun", level, seed);
	else
		dunPath = fmt::format("test/fixtures/diablo/{}-{}.dun", level, seed);
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

TEST(Drlg_l4, CreateL4Dungeon_diablo_13_428074402)
{
	TestCreateL4Dungeon(false, 13, 428074402, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(26, 64));
	TestCreateL4Dungeon(false, 13, 428074402, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(47, 79));
}

TEST(Drlg_l4, CreateL4Dungeon_diablo_14_717625719)
{
	TestCreateL4Dungeon(false, 14, 717625719, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(90, 64));
	TestCreateL4Dungeon(false, 14, 717625719, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(49, 31));
}

TEST(Drlg_l4, CreateL4Dungeon_diablo_15_1583642716)
{
	Quests[Q_DIABLO]._qactive = QUEST_INIT;
	TestCreateL4Dungeon(false, 15, 1583642716, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(44, 26));
	TestCreateL4Dungeon(false, 15, 1583642716, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(87, 69));

	Quests[Q_DIABLO]._qactive = QUEST_ACTIVE;
	TestCreateL4Dungeon(true, 15, 1583642716, ENTRY_MAIN);
	EXPECT_EQ(ViewPosition, Point(44, 26));
	TestCreateL4Dungeon(true, 15, 1583642716, ENTRY_PREV);
	EXPECT_EQ(ViewPosition, Point(87, 69));
}

} // namespace
