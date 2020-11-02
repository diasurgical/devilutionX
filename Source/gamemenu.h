/**
 * @file gamemenu.h
 *
 * Interface of the in-game menu functions.
 */
#ifndef __GAMEMENU_H__
#define __GAMEMENU_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HELLFIRE
extern char *jogging_title;
extern BOOL jogging_opt;
#endif

void gamemenu_on();
void gamemenu_off();
void gamemenu_handle_previous();
void gamemenu_previous(BOOL bActivate);
void gamemenu_new_game(BOOL bActivate);
void gamemenu_quit_game(BOOL bActivate);
void gamemenu_load_game(BOOL bActivate);
void gamemenu_save_game(BOOL bActivate);
void gamemenu_restart_town(BOOL bActivate);
void gamemenu_options(BOOL bActivate);
void gamemenu_music_volume(BOOL bActivate);
void gamemenu_sound_volume(BOOL bActivate);
#ifdef HELLFIRE
void gamemenu_loadjog(BOOL bActivate);
#endif
void gamemenu_gamma(BOOL bActivate);
#ifndef HELLFIRE
void gamemenu_speed(BOOL bActivate);
void gamemenu_color_cycling(BOOL bActivate);
#endif

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __GAMEMENU_H__ */
