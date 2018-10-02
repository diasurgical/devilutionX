//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
int mainmenu_cpp_init_value; // weak

char chr_name_str[16];
#endif

const int mainmenu_inf = 0x7F800000; // weak

/* data */

int menu_music_track_id = 5; // idb

struct mainmenu_cpp_init
{
	mainmenu_cpp_init()
	{
		mainmenu_cpp_init_value = mainmenu_inf;
	}
} _mainmenu_cpp_init;
// 47F074: using guessed type int mainmenu_inf;
// 646CE0: using guessed type int mainmenu_cpp_init_value;

void __cdecl mainmenu_refresh_music()
{
	int v0; // eax

	music_start(menu_music_track_id);
	v0 = menu_music_track_id;
	do
	{
		if ( ++v0 == 6 )
			v0 = 0;
	}
	while ( !v0 || v0 == 1 );
	menu_music_track_id = v0;
}

void __stdcall mainmenu_create_hero(char *a1, char *a2)
{
	// char *v2; // [esp-14h] [ebp-14h]

	if ( UiValidPlayerName(a1) ) /* v2 */
		pfile_create_save_file(a1, a2);
}

int __stdcall mainmenu_select_hero_dialog(int u1, int u2, int u3, int u4, int mode, char *cname, int clen, char *cdesc, int cdlen, int *multi) /* fix args */
{
	int v10; // eax
	int a6; // [esp+8h] [ebp-8h]
	int a5; // [esp+Ch] [ebp-4h]

	a6 = 1;
	a5 = 0;
	if ( gbMaxPlayers == 1 )
	{
		if ( !UiSelHeroSingDialog(
				  pfile_ui_set_hero_infos,
				  pfile_ui_save_create,
				  pfile_delete_save,
				  pfile_ui_set_class_stats,
				  &a5,
				  chr_name_str,
				  &gnDifficulty) )
			TermMsg("Unable to display SelHeroSing");
		if ( a5 == 2 )
		{
			dword_5256E8 = 1;
			goto LABEL_6;
		}
		dword_5256E8 = 0;
	}
	else if ( !UiSelHeroMultDialog(
				   pfile_ui_set_hero_infos,
				   pfile_ui_save_create,
				   pfile_delete_save,
				   pfile_ui_set_class_stats,
				   &a5,
				   &a6,
				   chr_name_str) )
	{
		TermMsg("Can't load multiplayer dialog");
	}
	if ( a5 == 4 )
	{
		SErrSetLastError(1223);
		return 0;
	}
LABEL_6:
	pfile_create_player_description(cdesc, cdlen);
	if ( multi )
	{
		if ( mode == 'BNET' )
			v10 = a6 || !plr[myplr].pBattleNet;
		else
			v10 = a6;
		*multi = v10;
	}
	if ( cname )
	{
		if ( clen )
			SStrCopy(cname, chr_name_str, clen);
	}
	return 1;
}
// 5256E8: using guessed type int dword_5256E8;
// 679660: using guessed type char gbMaxPlayers;

void __cdecl mainmenu_loop()
{
	int v1; // eax
	int a2; // [esp+0h] [ebp-4h]

	mainmenu_refresh_music();
	do
	{
		while ( 1 )
		{
			a2 = 0;
			if ( !UiMainMenuDialog("Diablo v1.09", &a2, effects_play_sound, 30) )
				TermMsg("Unable to display mainmenu");
			if ( a2 == 1 )
				break;
			switch ( a2 )
			{
				case MAINMENU_MULTIPLAYER:
					v1 = mainmenu_multi_player();
					goto LABEL_15;
				case MAINMENU_REPLAY_INTRO:
					goto LABEL_10;
				case MAINMENU_SHOW_CREDITS:
					UiCreditsDialog(16);
					break;
				case MAINMENU_EXIT_DIABLO:
					goto LABEL_16;
				case MAINMENU_ATTRACT_MODE:
LABEL_10:
					if ( gbActive )
						mainmenu_play_intro();
					break;
			}
		}
		v1 = mainmenu_single_player();
LABEL_15:
		;
	}
	while ( v1 );
LABEL_16:
	music_stop();
}
// 634980: using guessed type int gbActive;

int __cdecl mainmenu_single_player()
{
	gbMaxPlayers = 1;
	return mainmenu_init_menu(1);
}
// 679660: using guessed type char gbMaxPlayers;

int __fastcall mainmenu_init_menu(int a1)
{
	int v1; // esi
	int v3; // esi

	v1 = a1;
	if ( a1 == 4 )
		return 1;
	music_stop();
	v3 = diablo_init_menu(v1 != 2, v1 != 3);
	if ( v3 )
		mainmenu_refresh_music();
	return v3;
}

int __cdecl mainmenu_multi_player()
{
	gbMaxPlayers = MAX_PLRS;
	return mainmenu_init_menu(3);
}
// 679660: using guessed type char gbMaxPlayers;

void __cdecl mainmenu_play_intro()
{
	music_stop();
	play_movie("gendata\\diablo1.smk", 1);
	mainmenu_refresh_music();
}
