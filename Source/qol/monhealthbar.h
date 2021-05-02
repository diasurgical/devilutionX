/**
* @file monhealthbar.h
*
* Adds monster health bar QoL feature
*/
#pragma once

namespace devilution {

struct CelOutputBuffer;

void InitMonsterHealthBar();
void FreeMonsterHealthBar();

void DrawMonsterHealthBar(const CelOutputBuffer &out);

} // namespace devilution
