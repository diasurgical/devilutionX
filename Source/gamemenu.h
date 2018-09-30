//HEADER_GOES_HERE
#ifndef __GAMEMENU_H__
#define __GAMEMENU_H__

void __cdecl gamemenu_previous();
void __cdecl gamemenu_enable_single();
void __cdecl gamemenu_enable_multi();
void __cdecl gamemenu_off();
void __cdecl gamemenu_handle_previous();
void __cdecl gamemenu_new_game();
void __cdecl gamemenu_quit_game();
void __cdecl gamemenu_load_game(); // should have 1-2 args
void __cdecl gamemenu_save_game(); // should have 1-2 args
void __cdecl gamemenu_restart_town();
void __cdecl gamemenu_options();
void __cdecl gamemenu_get_music();
void __fastcall gamemenu_sound_music_toggle(char **names, TMenuItem *menu_item, int gamma);
void __cdecl gamemenu_get_sound();
void __cdecl gamemenu_get_color_cycling();
void __cdecl gamemenu_get_gamma();
void __fastcall gamemenu_music_volume(int a1);
int __fastcall gamemenu_slider_music_sound(TMenuItem *menu_item);
void __fastcall gamemenu_sound_volume(int a1);
void __fastcall gamemenu_gamma(int a1);
int __cdecl gamemenu_slider_gamma();
void __cdecl gamemenu_color_cycling();

/* rdata */
extern TMenuItem sgSingleMenu[6];
extern TMenuItem sgMultiMenu[5];
extern TMenuItem sgOptionMenu[6];
extern char *music_toggle_names[];
extern char *sound_toggle_names[];
extern char *color_cycling_toggle_names[];

#endif /* __GAMEMENU_H__ */
