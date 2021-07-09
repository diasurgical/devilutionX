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
void CreateL4Dungeon(uint32_t rseed, lvl_entry entry);
void LoadL4Dungeon(const char *path, int vx, int vy);
void LoadPreL4Dungeon(const char *path);

} // namespace devilution
