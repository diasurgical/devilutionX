//HEADER_GOES_HERE
#ifndef __MAINMENU_H__
#define __MAINMENU_H__

extern int mainmenu_cpp_init_value; // weak
extern char chr_name_str[16];

void __cdecl mainmenu_cpp_init();
void __cdecl mainmenu_refresh_music();
void __stdcall mainmenu_create_hero(char *, char *);
int __stdcall mainmenu_select_hero_dialog(int u1, int u2, int u3, int u4, int mode, char *cname, int clen, char *cdesc, int cdlen, int *multi);
void __cdecl mainmenu_loop();
int __cdecl mainmenu_single_player();
int __fastcall mainmenu_init_menu(int a1);
int __cdecl mainmenu_multi_player();
void __cdecl mainmenu_play_intro();

/* rdata */

extern const int mainmenu_inf; // weak

/* data */

extern int menu_music_track_id; // idb

#endif /* __MAINMENU_H__ */
