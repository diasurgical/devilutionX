/**
 * @file themes.h
 *
 * Interface of the theme room placing algorithms.
 */
#ifndef __THEMES_H__
#define __THEMES_H__

namespace dvl {

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ThemeStruct {
	theme_id ttype;
	Sint32 ttval;
} ThemeStruct;

extern int numthemes;
extern BOOL armorFlag;
extern BOOL weaponFlag;
extern int zharlib;
extern ThemeStruct themes[MAXTHEMES];

void InitThemes();
void HoldThemeRooms();
void CreateThemeRooms();

#ifdef __cplusplus
}
#endif
}

#endif /* __THEMES_H__ */
