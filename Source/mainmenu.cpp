/**
 * @file mainmenu.cpp
 *
 * Implementation of functions for interacting with the main menu.
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"
#include "../DiabloUI/diabloui.h"

DEVILUTION_BEGIN_NAMESPACE

char gszHero[16];

/* data */

/** The active music track id for the main menu. */
int menu_music_track_id = TMUSIC_INTRO;

void mainmenu_refresh_music()
{
	music_start(menu_music_track_id);

	if (gbIsSpawn && !gbIsHellfire) {
		return;
	}

	do {
		menu_music_track_id++;
		if (menu_music_track_id == NUM_MUSIC || (!gbIsHellfire && menu_music_track_id > TMUSIC_L4))
			menu_music_track_id = TMUSIC_L2;
		if (gbIsSpawn && menu_music_track_id > TMUSIC_L1)
			menu_music_track_id = TMUSIC_L5;
	} while (menu_music_track_id == TMUSIC_TOWN || menu_music_track_id == TMUSIC_L1);
}

static BOOL mainmenu_init_menu(int type)
{
	BOOL success;

	if (type == SELHERO_PREVIOUS)
		return TRUE;

	music_stop();

	success = StartGame(type != SELHERO_CONTINUE, type != SELHERO_CONNECT);
	if (success)
		mainmenu_refresh_music();

	return success;
}

static BOOL mainmenu_single_player()
{
	gbIsMultiplayer = false;

	gbJogInTown = sgOptions.bJogInTown;
	gnTickRate = sgOptions.nTickRate;
	gbTheoQuest = sgOptions.bTheoQuest;
	gbCowQuest = sgOptions.bCowQuest;

	return mainmenu_init_menu(SELHERO_NEW_DUNGEON);
}

static BOOL mainmenu_multi_player()
{
	gbIsMultiplayer = true;
	return mainmenu_init_menu(SELHERO_CONNECT);
}

static void mainmenu_play_intro()
{
	music_stop();
	if (gbIsHellfire)
		play_movie("gendata\\Hellfire.smk", TRUE);
	else
		play_movie("gendata\\diablo1.smk", TRUE);
	mainmenu_refresh_music();
}
void mainmenu_change_name(int arg1, int arg2, int arg3, int arg4, char *name_1, char *name_2)
{
	if (UiValidPlayerName(name_2))
		pfile_rename_hero(name_1, name_2);
}

BOOL mainmenu_select_hero_dialog(
    const _SNETPROGRAMDATA *client_info,
    const _SNETPLAYERDATA *user_info,
    const _SNETUIDATA *ui_info,
    const _SNETVERSIONDATA *fileinfo,
    DWORD mode,
    char *cname, DWORD clen,
    char *cdesc, DWORD cdlen,
    BOOL *multi)
{
	BOOL hero_is_created = TRUE;
	int dlgresult = 0;
	if (!gbIsMultiplayer) {
		UiSelHeroSingDialog(
		    pfile_ui_set_hero_infos,
		    pfile_ui_save_create,
		    pfile_delete_save,
		    pfile_ui_set_class_stats,
		    &dlgresult,
		    &gszHero,
		    &gnDifficulty);
		client_info->initdata->nDifficulty = gnDifficulty;

		if (dlgresult == SELHERO_CONTINUE)
			gbLoadGame = TRUE;
		else
			gbLoadGame = FALSE;
	} else {
		UiSelHeroMultDialog(
		    pfile_ui_set_hero_infos,
		    pfile_ui_save_create,
		    pfile_delete_save,
		    pfile_ui_set_class_stats,
		    &dlgresult,
		    &hero_is_created,
		    &gszHero);
	}
	if (dlgresult == SELHERO_PREVIOUS) {
		SErrSetLastError(1223);
		return FALSE;
	}

	pfile_create_player_description(cdesc, cdlen);
	if (multi) {
		*multi = hero_is_created;
	}
	if (cname && clen)
		SStrCopy(cname, gszHero, clen);

	return TRUE;
}

void mainmenu_loop()
{
	BOOL done;
	int menu;

	mainmenu_refresh_music();
	done = FALSE;

	do {
		menu = 0;
		if (!UiMainMenuDialog(gszProductName, &menu, effects_play_sound, 30))
			app_fatal("Unable to display mainmenu");

		switch (menu) {
		case MAINMENU_SINGLE_PLAYER:
			if (!mainmenu_single_player())
				done = TRUE;
			break;
		case MAINMENU_MULTIPLAYER:
			if (!mainmenu_multi_player())
				done = TRUE;
			break;
		case MAINMENU_ATTRACT_MODE:
		case MAINMENU_REPLAY_INTRO:
			if (gbIsSpawn && !gbIsHellfire)
				done = FALSE;
			else if (gbActive)
				mainmenu_play_intro();
			break;
		case MAINMENU_SHOW_CREDITS:
			UiCreditsDialog();
			break;
		case MAINMENU_SHOW_SUPPORT:
			UiSupportDialog();
			break;
		case MAINMENU_EXIT_DIABLO:
			done = TRUE;
			break;
		}
	} while (!done);

	music_stop();
}

DEVILUTION_END_NAMESPACE
