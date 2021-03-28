/**
 * @file qol.h
 *
 * Quality of life features
 */
#ifndef __QOL_H__
#define __QOL_H__

#include "engine.h"

DEVILUTION_BEGIN_NAMESPACE

void FreeQol();
void InitQol();
void DrawMonsterHealthBar(CelOutputBuffer out);
void DrawXPBar(CelOutputBuffer out);
void AutoGoldPickup(int pnum);

DEVILUTION_END_NAMESPACE

#endif /* __QOL_H__ */
