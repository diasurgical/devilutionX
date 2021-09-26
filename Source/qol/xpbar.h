/**
* @file xpbar.h
*
* Adds XP bar QoL feature
*/
#pragma once

namespace devilution {

struct Surface;

void InitXPBar();
void FreeXPBar();

void DrawXPBar(const Surface &out, int playerId);
bool CheckXPBarInfo();

} // namespace devilution
