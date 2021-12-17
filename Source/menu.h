/**
 * @file menu.h
 *
 * Interface of functions for interacting with the main menu.
 */
#pragma once

#include "main_loop.hpp"
#include "multi.h"

namespace devilution {

extern uint32_t gSaveNumber;

bool mainmenu_select_hero_dialog(GameData *gameData);
std::unique_ptr<MainLoopHandler> CreateMenuLoopHandler();

} // namespace devilution
