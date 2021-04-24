/**
 * @file themes.h
 *
 * Interface of the theme room placing algorithms.
 */
#pragma once

#include <cstdint>

#include "gendung.h"
#include "objdat.h"

namespace devilution {

struct ThemeStruct {
	theme_id ttype;
	int16_t ttval;
};

extern int numthemes;
extern bool armorFlag;
extern bool weaponFlag;
extern int zharlib;
extern ThemeStruct themes[MAXTHEMES];

void InitThemes();
void HoldThemeRooms();
void CreateThemeRooms();

} // namespace devilution
