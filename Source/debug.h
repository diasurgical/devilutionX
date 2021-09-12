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
	dPiece    = 1 << 0,
	dTransVal = 1 << 1,
	dLight    = 1 << 2,
	dPreLight = 1 << 3,
	dFlags    = 1 << 4,
	dPlayer   = 1 << 5,
	dMonster  = 1 << 6,
	dCorpse   = 1 << 7,
	dObject   = 1 << 8,
	dItem     = 1 << 9,
	dSpecial  = 1 << 10,
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
