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
void gamemenu_previous(bool bActivate);
void gamemenu_new_game(bool bActivate);
void gamemenu_quit_game(bool bActivate);
void gamemenu_load_game(bool bActivate);
void gamemenu_save_game(bool bActivate);
void gamemenu_restart_town(bool bActivate);
void gamemenu_options(bool bActivate);
void gamemenu_music_volume(bool bActivate);
void gamemenu_sound_volume(bool bActivate);
void gamemenu_loadjog(bool bActivate);
void gamemenu_gamma(bool bActivate);
void gamemenu_speed(bool bActivate);
void gamemenu_color_cycling(bool bActivate);

}
