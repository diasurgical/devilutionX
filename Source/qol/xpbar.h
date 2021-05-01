/**
* @file xpbar.h
*
* Adds XP bar QoL feature
*/
#pragma once

namespace devilution {

struct CelOutputBuffer;

void InitXPBar();
void FreeXPBar();

void DrawXPBar(const CelOutputBuffer &out);
bool CheckXPBarInfo();

} // namespace devilution
