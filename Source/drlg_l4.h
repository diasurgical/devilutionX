/**
 * @file drlg_l4.h
 *
 * Interface of the hell level generation algorithms.
 */
#pragma once

#include "gendung.h"

namespace devilution {

extern int diabquad1x;
extern int diabquad1y;
extern int diabquad2x;
extern int diabquad2y;
extern int diabquad3x;
extern int diabquad3y;
extern int diabquad4x;
extern int diabquad4y;
bool IsDURWall(char d);
bool IsDLLWall(char dd);
void CreateL4Dungeon(uint32_t rseed, lvl_entry entry);

} // namespace devilution
