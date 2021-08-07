/**
 * @file debug.h
 *
 * Interface of debug functions.
 */
#pragma once

#include "engine.h"
#include "miniwin/miniwin.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

extern std::optional<CelSprite> pSquareCel;

void FreeDebugGFX();
void LoadDebugGFX();
void GiveGoldCheat();
void TakeGoldCheat();
void MaxSpellsCheat();
void SetAllSpellsCheat();
void PrintDebugPlayer(bool bNextPlayer);
void PrintDebugQuest();
void GetDebugMonster();
void NextDebugMonster();

} // namespace devilution
