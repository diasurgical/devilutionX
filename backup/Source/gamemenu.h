//HEADER_GOES_HERE
#ifndef __GAMEMENU_H__
#define __GAMEMENU_H__

void gamemenu_previous();
void gamemenu_enable_single(TMenuItem *pMenuItems);
void gamemenu_enable_multi(TMenuItem *pMenuItems);
void gamemenu_off();
void gamemenu_handle_previous();
void j_gamemenu_previous(BOOL bActivate);
void gamemenu_new_game(BOOL bActivate);
void gamemenu_quit_game(BOOL bActivate);
void gamemenu_load_game(BOOL bActivate);
void gamemenu_save_game(BOOL bActivate);
void gamemenu_restart_town(BOOL bActivate);
void gamemenu_options(BOOL bActivate);
void gamemenu_get_music();
void gamemenu_sound_music_toggle(char **names, TMenuItem *menu_item, int gamma);
void gamemenu_get_sound();
void gamemenu_get_color_cycling();
void gamemenu_get_gamma();
void gamemenu_music_volume(BOOL bActivate);
int gamemenu_slider_music_sound(TMenuItem *menu_item);
void gamemenu_sound_volume(BOOL bActivate);
void gamemenu_gamma(BOOL bActivate);
int gamemenu_slider_gamma();
void gamemenu_color_cycling(BOOL bActivate);

/* rdata */
extern char *music_toggle_names[];
extern char *sound_toggle_names[];
extern char *color_cycling_toggle_names[];

#endif /* __GAMEMENU_H__ */
