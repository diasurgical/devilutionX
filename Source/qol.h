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
void DrawMonsterHealthBar(const CelOutputBuffer &out);
void AutoGoldPickup(int pnum);

} // namespace devilution
