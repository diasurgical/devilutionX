/**
 * @file gamemenu.h
 *
 * Interface of the in-game menu functions.
 */
#pragma once

namespace devilution {

void gamemenu_on();
void gamemenu_off();
void gamemenu_handle_previous();
void gamemenu_quit_game(bool bActivate);
void gamemenu_load_game(bool bActivate);
void gamemenu_save_game(bool bActivate);
void gamemenu_return_to_main_menu(bool bActivate);

extern bool isGameMenuOpen;

} // namespace devilution
