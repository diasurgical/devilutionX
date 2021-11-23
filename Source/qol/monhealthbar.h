/**
 * @file monhealthbar.h
 *
 * Adds monster health bar QoL feature
 */
#pragma once

namespace devilution {

struct Surface;

void InitMonsterHealthBar();
void FreeMonsterHealthBar();

void DrawMonsterHealthBar(const Surface &out);

} // namespace devilution
