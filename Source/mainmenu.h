//HEADER_GOES_HERE
#ifndef __MAINMENU_H__
#define __MAINMENU_H__

extern char gszHero[16];

void mainmenu_refresh_music();
void __stdcall mainmenu_change_name(int arg1, int arg2, int arg3, int arg4, char *name_1, char *name_2);
int __stdcall mainmenu_select_hero_dialog(
    const _SNETPROGRAMDATA *client_info,
    const _SNETPLAYERDATA *user_info,
    const _SNETUIDATA *ui_info,
    const _SNETVERSIONDATA *fileinfo,
    DWORD mode,               // 4 chars, e.g. 'IPXN', 'BNET' etc. */
    char *cname, DWORD clen,  // character name will be copied here
    char *cdesc, DWORD cdlen, // character "description" will be copied here (used to advertise games)
    BOOL *multi);             // new character? - unsure about this
void mainmenu_loop();
BOOL mainmenu_single_player();
BOOL mainmenu_init_menu(int type);
BOOL mainmenu_multi_player();
void mainmenu_play_intro();

/* rdata */

/* data */

extern int menu_music_track_id;

#endif /* __MAINMENU_H__ */
