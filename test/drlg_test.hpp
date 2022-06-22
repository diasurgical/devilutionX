/**
 * @file drlg_test.hpp
 *
 * Helpers for dungeon related tests.
 */
#pragma once

#include "engine/load_file.hpp"
#include "levels/themes.h"
#include "utils/paths.h"

using namespace devilution;

int GetTileCount(dungeon_type levelType)
{
	switch (levelType) {
	case DTYPE_TOWN:
		return 376;
	case DTYPE_CATHEDRAL:
		return 206;
	case DTYPE_CATACOMBS:
		return 160;
	case DTYPE_CAVES:
		return 206;
	case DTYPE_HELL:
		return 137;
	case DTYPE_NEST:
		return 166;
	case DTYPE_CRYPT:
		return 217;
	default:
		app_fatal("Invalid level type");
	}
}

std::unique_ptr<uint16_t[]> DunData;

void LoadExpectedLevelData(const char *fixture)
{
	std::string dunPath = "test/fixtures/";

	paths::SetPrefPath(paths::BasePath());
	paths::SetAssetsPath(paths::BasePath() + "/" + dunPath);

	dunPath.append(fixture);
	DunData = LoadFileInMem<uint16_t>(dunPath.c_str());
	ASSERT_NE(DunData, nullptr) << "Unable to load test fixture " << dunPath;
	ASSERT_EQ(Size(DMAXX, DMAXY), Size(DunData[0], DunData[1]));
}

void TestCreateDungeon(int level, uint32_t seed, lvl_entry entry)
{
	currlevel = level;
	leveltype = GetLevelType(level);

	pMegaTiles = std::make_unique<MegaTile[]>(GetTileCount(leveltype));

	CreateDungeon(seed, entry);
	CreateThemeRooms();

	const uint16_t *tileLayer = &DunData[2];

	for (int y = 0; y < DMAXY; y++) {
		for (int x = 0; x < DMAXX; x++) {
			auto tileId = static_cast<uint8_t>(SDL_SwapLE16(*tileLayer));
			tileLayer++;
			ASSERT_EQ(dungeon[x][y], tileId) << "Tiles don't match at " << x << "x" << y;
		}
	}

	const uint16_t *transparentLayer = &DunData[2 + DMAXX * DMAXY * 13];

	for (int y = 16; y < 16 + DMAXY * 2; y++) {
		for (int x = 16; x < 16 + DMAXX * 2; x++) {
			auto sectorId = static_cast<uint8_t>(SDL_SwapLE16(*transparentLayer));
			transparentLayer++;
			ASSERT_EQ(dTransVal[x][y], sectorId) << "Room/region indexes don't match at " << x << "x" << y;
		}
	}
}
