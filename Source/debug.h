/**
 * @file debug.h
 *
 * Interface of debug functions.
 */
#pragma once

#include <unordered_map>

#include "engine.h"
#include "engine/cel_sprite.hpp"
#include "miniwin/miniwin.h"
#include "utils/stdcompat/optional.hpp"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

enum class DebugInfoFlags : uint16_t {
	// clang-format off
	empty     = 0,
	dungeon   = 1 << 0,
	pdungeon  = 1 << 1,
	dflags    = 1 << 2,
	dPiece    = 1 << 3,
	dTransVal = 1 << 4,
	dLight    = 1 << 5,
	dPreLight = 1 << 6,
	dFlags    = 1 << 7,
	dPlayer   = 1 << 8,
	dMonster  = 1 << 9,
	dCorpse   = 1 << 10,
	dObject   = 1 << 11,
	dItem     = 1 << 12,
	dSpecial  = 1 << 13,
	// clang-format on
};

extern std::optional<CelSprite> pSquareCel;
extern bool DebugToggle;
extern bool DebugGodMode;
extern bool DebugVision;
extern bool DebugCoords;
extern bool DebugCursorCoords;
extern bool DebugGrid;
extern std::unordered_map<int, Point> DebugCoordsMap;
extern DebugInfoFlags DebugInfoFlag;

void FreeDebugGFX();
void LoadDebugGFX();
void PrintDebugPlayer(bool bNextPlayer);
void PrintDebugQuest();
void GetDebugMonster();
void NextDebugMonster();
void SetDebugLevelSeedInfos(uint32_t mid1Seed, uint32_t mid2Seed, uint32_t mid3Seed, uint32_t endSeed);
bool CheckDebugTextCommand(const string_view text);
int DebugGetTileData(Point dungeonCoords);

} // namespace devilution
