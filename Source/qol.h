/**
 * @file qol.h
 *
 * Quality of life features
 */
#pragma once

#include "engine.h"

namespace devilution {

void FreeQol();
void InitQol();
void DrawMonsterHealthBar(CelOutputBuffer out);
void DrawXPBar(CelOutputBuffer out);
bool CheckXPBarInfo(void);
void AutoGoldPickup(int pnum);

} // namespace devilution
