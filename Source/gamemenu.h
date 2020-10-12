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
extern BOOL jogging_opt;
#endif

void gamemenu_on();
void gamemenu_update_single(TMenuItem *pMenuItems);
void gamemenu_update_multi(TMenuItem *pMenuItems);
void gamemenu_off();
void gamemenu_handle_previous();
void gamemenu_previous(BOOL bActivate);
void gamemenu_new_game(BOOL bActivate);
void gamemenu_quit_game(BOOL bActivate);
void gamemenu_load_game(BOOL bActivate);
void gamemenu_save_game(BOOL bActivate);
void gamemenu_restart_town(BOOL bActivate);
void gamemenu_options(BOOL bActivate);
void gamemenu_get_music();
void gamemenu_sound_music_toggle(char **names, TMenuItem *menu_item, int gamma);
void gamemenu_get_sound();
#ifdef HELLFIRE
void gamemenu_jogging();
#endif
#ifndef HELLFIRE
void gamemenu_get_color_cycling();
#endif
void gamemenu_get_gamma();
void gamemenu_music_volume(BOOL bActivate);
int gamemenu_slider_music_sound(TMenuItem *menu_item);
void gamemenu_sound_volume(BOOL bActivate);
#ifdef HELLFIRE
void gamemenu_loadjog(BOOL bActivate);
#endif
void gamemenu_gamma(BOOL bActivate);
int gamemenu_slider_gamma();
#ifndef HELLFIRE
void gamemenu_color_cycling(BOOL bActivate);
#endif

void gamemenu_get_speed();
void gamemenu_speed(BOOL bActivate);

/* rdata */
extern char *music_toggle_names[];
extern char *sound_toggle_names[];
#ifndef HELLFIRE
extern char *color_cycling_toggle_names[];
#endif

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __GAMEMENU_H__ */
