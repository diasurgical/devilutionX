/**
 * @file mainmenu.h
 *
 * Interface of functions for interacting with the main menu.
 */
#ifndef __MAINMENU_H__
#define __MAINMENU_H__

#include "multi.h"

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

extern char gszHero[16];

bool mainmenu_select_hero_dialog(GameData *gameData);
void mainmenu_loop();

#ifdef __cplusplus
}
#endif

}

#endif /* __MAINMENU_H__ */
