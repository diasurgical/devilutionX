
/**
 * @file gamemenu.cpp
 *
 * Implementation of the in-game menu functions.
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

#ifdef HELLFIRE
BOOL jogging_opt = TRUE;
#endif

/** Contains the game menu items of the single player menu. */
TMenuItem sgSingleMenu[] = {
// clang-format off
//	  dwFlags,       pszStr,         fnMenu
	{ GMENU_ENABLED, "Save Game",     &gamemenu_save_game  },
	{ GMENU_ENABLED, "Options",       &gamemenu_options    },
	{ GMENU_ENABLED, "New Game",      &gamemenu_new_game   },
	{ GMENU_ENABLED, "Load Game",     &gamemenu_load_game  },
#ifndef HELLFIRE
	{ GMENU_ENABLED, "Quit Diablo",   &gamemenu_quit_game  },
#else
	{ GMENU_ENABLED, "Quit Hellfire", &gamemenu_quit_game  },
#endif
	{ GMENU_ENABLED, NULL,            NULL }
// clang-format on
};
/** Contains the game menu items of the multi player menu. */
TMenuItem sgMultiMenu[] = {
// clang-format off
//	  dwFlags,       pszStr,            fnMenu
	{ GMENU_ENABLED, "Options",         &gamemenu_options      },
	{ GMENU_ENABLED, "New Game",        &gamemenu_new_game     },
	{ GMENU_ENABLED, "Restart In Town", &gamemenu_restart_town },
#ifndef HELLFIRE
	{ GMENU_ENABLED, "Quit Diablo",     &gamemenu_quit_game    },
#else
	{ GMENU_ENABLED, "Quit Hellfire",   &gamemenu_quit_game    },
#endif
	{ GMENU_ENABLED, NULL,              NULL                   },
// clang-format on
};
TMenuItem sgOptionsMenu[] = {
// clang-format off
//	  dwFlags,                      pszStr,          fnMenu
	{ GMENU_ENABLED | GMENU_SLIDER, NULL,            &gamemenu_music_volume  },
	{ GMENU_ENABLED | GMENU_SLIDER, NULL,            &gamemenu_sound_volume  },
	{ GMENU_ENABLED | GMENU_SLIDER, "Gamma",         &gamemenu_gamma         },
#ifndef HELLFIRE
//	{ GMENU_ENABLED               , NULL,            &gamemenu_color_cycling },
	{ GMENU_ENABLED | GMENU_SLIDER, "Speed",         &gamemenu_speed         },
#else
	{ GMENU_ENABLED | GMENU_SLIDER, NULL,            &gamemenu_loadjog       },
#endif
	{ GMENU_ENABLED               , "Previous Menu", &gamemenu_previous      },
	{ GMENU_ENABLED               , NULL,            NULL                    },
// clang-format on
};
/** Specifies the menu names for music enabled and disabled. */
const char *const music_toggle_names[] = {
	"Music",
	"Music Disabled",
};
/** Specifies the menu names for sound enabled and disabled. */
const char *const sound_toggle_names[] = {
	"Sound",
	"Sound Disabled",
};
#ifdef HELLFIRE
char *jogging_toggle_names[] = {
	"Jog",
	"Walk",
};
char *jogging_title = "Fast Walk";
#endif
/** Specifies the menu names for colour cycling disabled and enabled. */
const char *const color_cycling_toggle_names[] = { "Color Cycling Off", "Color Cycling On" };

void gamemenu_on()
{
	if (gbMaxPlayers == 1) {
		gmenu_set_items(sgSingleMenu, gamemenu_update_single);
	} else {
		gmenu_set_items(sgMultiMenu, gamemenu_update_multi);
	}
	PressEscKey();
}

void gamemenu_update_single(TMenuItem *pMenuItems)
{
	BOOL enable;

	gmenu_enable(&sgSingleMenu[3], gbValidSaveFile);

	enable = FALSE;
	if (plr[myplr]._pmode != PM_DEATH && !deathflag)
		enable = TRUE;

	gmenu_enable(&sgSingleMenu[0], enable);
}

void gamemenu_update_multi(TMenuItem *pMenuItems)
{
	gmenu_enable(&sgMultiMenu[2], deathflag);
}

void gamemenu_off()
{
	gmenu_set_items(NULL, NULL);
}

void gamemenu_handle_previous()
{
	if (gmenu_is_active())
		gamemenu_off();
	else
		gamemenu_on();
}

void gamemenu_previous(BOOL bActivate)
{
	gamemenu_on();
}

void gamemenu_new_game(BOOL bActivate)
{
	int i;

	for (i = 0; i < MAX_PLRS; i++) {
		plr[i]._pmode = PM_QUIT;
		plr[i]._pInvincible = TRUE;
	}

	deathflag = FALSE;
	force_redraw = 255;
	scrollrt_draw_game_screen(TRUE);
#ifdef HELLFIRE
	CornerStone.activated = FALSE;
#endif
	gbRunGame = FALSE;
	gamemenu_off();
}

void gamemenu_quit_game(BOOL bActivate)
{
	gamemenu_new_game(bActivate);
	gbRunGameResult = FALSE;
}

void gamemenu_load_game(BOOL bActivate)
{
	WNDPROC saveProc = SetWindowProc(DisableInputWndProc);
	gamemenu_off();
	SetCursor_(CURSOR_NONE);
	InitDiabloMsg(EMSG_LOADING);
	force_redraw = 255;
	DrawAndBlit();
	LoadGame(FALSE);
	ClrDiabloMsg();
#ifdef HELLFIRE
	CornerStone.activated = FALSE;
#endif
	PaletteFadeOut(8);
	deathflag = FALSE;
	force_redraw = 255;
	DrawAndBlit();
	PaletteFadeIn(8);
	SetCursor_(CURSOR_HAND);
	interface_msg_pump();
	SetWindowProc(saveProc);
}

void gamemenu_save_game(BOOL bActivate)
{
	if (pcurs != CURSOR_HAND) {
		return;
	}

	if (plr[myplr]._pmode == PM_DEATH || deathflag) {
		gamemenu_off();
		return;
	}

	WNDPROC saveProc = SetWindowProc(DisableInputWndProc);
	SetCursor_(CURSOR_NONE);
	gamemenu_off();
	InitDiabloMsg(EMSG_SAVING);
	force_redraw = 255;
	DrawAndBlit();
	SaveGame();
	ClrDiabloMsg();
	force_redraw = 255;
	SetCursor_(CURSOR_HAND);
#ifdef HELLFIRE
	if (CornerStone.activated) {
		items_427A72();
	}
#endif
	interface_msg_pump();
	SetWindowProc(saveProc);
}

void gamemenu_restart_town(BOOL bActivate)
{
	NetSendCmd(TRUE, CMD_RETOWN);
}

void gamemenu_options(BOOL bActivate)
{
	gamemenu_get_music();
	gamemenu_get_sound();
#ifdef HELLFIRE
	gamemenu_jogging();
#endif
	gamemenu_get_gamma();
#ifndef HELLFIRE
	gamemenu_get_speed();
	//gamemenu_get_color_cycling();
#endif
	gmenu_set_items(sgOptionsMenu, NULL);
}

void gamemenu_get_music()
{
	gamemenu_sound_music_toggle(music_toggle_names, sgOptionsMenu, sound_get_or_set_music_volume(1));
}

void gamemenu_sound_music_toggle(const char *const *names, TMenuItem *menu_item, int volume)
{
	if (gbSndInited) {
		menu_item->dwFlags |= GMENU_ENABLED | GMENU_SLIDER;
		menu_item->pszStr = *names;
		gmenu_slider_steps(menu_item, 17);
		gmenu_slider_set(menu_item, VOLUME_MIN, VOLUME_MAX, volume);
		return;
	}

	menu_item->dwFlags &= ~(GMENU_ENABLED | GMENU_SLIDER);
	menu_item->pszStr = names[1];
}

void gamemenu_get_sound()
{
	gamemenu_sound_music_toggle(sound_toggle_names, &sgOptionsMenu[1], sound_get_or_set_sound_volume(1));
}

#ifdef HELLFIRE
void gamemenu_jogging()
{
	gmenu_slider_steps(&sgOptionsMenu[3], 2);
	gmenu_slider_set(&sgOptionsMenu[3], 0, 1, jogging_opt);
	sgOptionsMenu[3].pszStr = jogging_toggle_names[!jogging_opt ? 1 : 0];
}
#endif

void gamemenu_get_color_cycling()
{
	sgOptionsMenu[3].pszStr = color_cycling_toggle_names[palette_get_color_cycling() & 1];
}

void gamemenu_get_gamma()
{
	gmenu_slider_steps(&sgOptionsMenu[2], 15);
	gmenu_slider_set(&sgOptionsMenu[2], 30, 100, UpdateGamma(0));
}

void gamemenu_music_volume(BOOL bActivate)
{
	int volume;

	if (bActivate) {
		if (gbMusicOn) {
			gbMusicOn = FALSE;
			music_stop();
			sound_get_or_set_music_volume(VOLUME_MIN);
		} else {
			gbMusicOn = TRUE;
			sound_get_or_set_music_volume(VOLUME_MAX);
#ifdef HELLFIRE
			int lt;
			if (currlevel >= 17) {
				if (currlevel > 20)
					lt = DTYPE_NEST;
				else
					lt = DTYPE_CRYPT;
			} else
				lt = leveltype;
			music_start(lt);
#else
			music_start(leveltype);
#endif
        }
    } else {
        volume = gamemenu_slider_music_sound(&sgOptionsMenu[0]);
        sound_get_or_set_music_volume(volume);
        if (volume == VOLUME_MIN) {
            if (gbMusicOn) {
                gbMusicOn = FALSE;
                music_stop();
            }
        } else if (!gbMusicOn) {
            gbMusicOn = TRUE;
#ifdef HELLFIRE
			int lt;
			if (currlevel >= 17) {
				if (currlevel > 20)
					lt = DTYPE_NEST;
				else
					lt = DTYPE_CRYPT;
			} else
				lt = leveltype;
			music_start(lt);
#else
			music_start(leveltype);
#endif
        }
    }
    gamemenu_get_music();
}

int gamemenu_slider_music_sound(TMenuItem *menu_item)
{
	return gmenu_slider_get(menu_item, VOLUME_MIN, VOLUME_MAX);
}

void gamemenu_sound_volume(BOOL bActivate)
{
	int volume;
	if (bActivate) {
		if (gbSoundOn) {
			gbSoundOn = FALSE;
			sound_stop();
			sound_get_or_set_sound_volume(VOLUME_MIN);
		} else {
			gbSoundOn = TRUE;
			sound_get_or_set_sound_volume(VOLUME_MAX);
		}
	} else {
		volume = gamemenu_slider_music_sound(&sgOptionsMenu[1]);
		sound_get_or_set_sound_volume(volume);
		if (volume == VOLUME_MIN) {
			if (gbSoundOn) {
				gbSoundOn = FALSE;
				sound_stop();
			}
		} else if (!gbSoundOn) {
			gbSoundOn = TRUE;
		}
	}
	PlaySFX(IS_TITLEMOV);
	gamemenu_get_sound();
}

#ifdef HELLFIRE
void gamemenu_loadjog(BOOL bActivate)
{
	if (gbMaxPlayers == 1) {
		jogging_opt = !jogging_opt;
		SRegSaveValue(APP_NAME, jogging_title, FALSE, jogging_opt);
		PlaySFX(IS_TITLEMOV);
		gamemenu_jogging();
	}
}
#endif

void gamemenu_gamma(BOOL bActivate)
{
	int gamma;
	if (bActivate) {
		gamma = UpdateGamma(0);
		if (gamma == 30)
			gamma = 100;
		else
			gamma = 30;
	} else {
		gamma = gamemenu_slider_gamma();
	}

	UpdateGamma(gamma);
	gamemenu_get_gamma();
}

int gamemenu_slider_gamma()
{
	return gmenu_slider_get(&sgOptionsMenu[2], 30, 100);
}

void gamemenu_get_speed()
{
	if (gbMaxPlayers != 1) {
		sgOptionsMenu[3].dwFlags &= ~(GMENU_ENABLED | GMENU_SLIDER);
		if (ticks_per_sec >= 50)
			sgOptionsMenu[3].pszStr = "Speed: Fastest";
		else if (ticks_per_sec >= 40)
			sgOptionsMenu[3].pszStr = "Speed: Faster";
		else if (ticks_per_sec >= 30)
			sgOptionsMenu[3].pszStr = "Speed: Fast";
		else if (ticks_per_sec == 20)
			sgOptionsMenu[3].pszStr = "Speed: Normal";
		return;
	}

	sgOptionsMenu[3].dwFlags |= GMENU_ENABLED | GMENU_SLIDER;

	sgOptionsMenu[3].pszStr = "Speed";
	gmenu_slider_steps(&sgOptionsMenu[3], 46);
	gmenu_slider_set(&sgOptionsMenu[3], 20, 50, ticks_per_sec);
}

void gamemenu_speed(BOOL bActivate)
{
	if (bActivate) {
		if (ticks_per_sec != 20)
			ticks_per_sec = 20;
		else
			ticks_per_sec = 50;
		gmenu_slider_set(&sgOptionsMenu[3], 20, 50, ticks_per_sec);
	} else {
		ticks_per_sec = gmenu_slider_get(&sgOptionsMenu[3], 20, 50);
	}

	SRegSaveValue("devilutionx", "game speed", 0, ticks_per_sec);
	tick_delay = 1000 / ticks_per_sec;
}

void gamemenu_color_cycling(BOOL bActivate)
{
	BOOL color_cycling;

	color_cycling = palette_set_color_cycling(palette_get_color_cycling() == 0);
	sgOptionsMenu[3].pszStr = color_cycling_toggle_names[color_cycling & 1];
}

DEVILUTION_END_NAMESPACE
