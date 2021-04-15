/**
 * @file mainmenu.h
 *
 * Interface of functions for interacting with the main menu.
 */
#pragma once

#include "multi.h"

namespace devilution {

extern char gszHero[16];

bool mainmenu_select_hero_dialog(GameData *gameData);
void mainmenu_loop();

}
