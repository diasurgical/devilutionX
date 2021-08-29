/**
 * @file debug.h
 *
 * Interface of debug functions.
 */
#pragma once

#include <string_view>
#include <unordered_map>

#include "engine.h"
#include "engine/cel_sprite.hpp"
#include "miniwin/miniwin.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

extern std::optional<CelSprite> pSquareCel;
extern bool DebugGodMode;
extern bool DebugVision;
extern bool DebugCoords;
extern bool DebugCursorCoords;
extern bool DebugGrid;
extern std::unordered_map<int, Point> DebugCoordsMap;

void FreeDebugGFX();
void LoadDebugGFX();
void PrintDebugPlayer(bool bNextPlayer);
void PrintDebugQuest();
void GetDebugMonster();
void NextDebugMonster();
void SetDebugLevelSeedInfos(uint32_t mid1Seed, uint32_t mid2Seed, uint32_t mid3Seed, uint32_t endSeed);
bool CheckDebugTextCommand(const std::string_view text);

} // namespace devilution
