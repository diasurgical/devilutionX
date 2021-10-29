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

extern std::optional<CelSprite> pSquareCel;
extern bool DebugToggle;
extern bool DebugGodMode;
extern bool DebugVision;
extern bool DebugGrid;
extern std::unordered_map<int, Point> DebugCoordsMap;
extern bool DebugScrollViewEnabled;

void FreeDebugGFX();
void LoadDebugGFX();
void GetDebugMonster();
void NextDebugMonster();
void SetDebugLevelSeedInfos(uint32_t mid1Seed, uint32_t mid2Seed, uint32_t mid3Seed, uint32_t endSeed);
bool CheckDebugTextCommand(const string_view text);
bool IsDebugGridTextNeeded();
bool IsDebugGridInMegatiles();
bool GetDebugGridText(Point dungeonCoords, char *debugGridTextBuffer);

} // namespace devilution
