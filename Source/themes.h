/**
 * @file themes.h
 *
 * Interface of the theme room placing algorithms.
 */
#pragma once

namespace devilution {

struct ThemeStruct {
	theme_id ttype;
	Sint16 ttval;
};

extern int numthemes;
extern bool armorFlag;
extern bool weaponFlag;
extern int zharlib;
extern ThemeStruct themes[MAXTHEMES];

void InitThemes();
void HoldThemeRooms();
void CreateThemeRooms();

}
