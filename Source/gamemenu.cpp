/**
 * @file gamemenu.cpp
 *
 * Implementation of the in-game menu functions.
 */
#include "gamemenu.h"

#include "cursor.h"
#include "error.h"
#include "gmenu.h"
#include "init.h"
#include "loadsave.h"
#include "options.h"
#include "pfile.h"
#include "utils/language.h"

#ifndef NOSOUND
#include "sound.h"
#endif

namespace devilution {
namespace {

// Forward-declare menu handlers, used by the global menu structs below.
void gamemenu_previous(bool bActivate);
void gamemenu_new_game(bool bActivate);
void gamemenu_restart_town(bool bActivate);
void gamemenu_options(bool bActivate);
void gamemenu_music_volume(bool bActivate);
void gamemenu_sound_volume(bool bActivate);
void gamemenu_gamma(bool bActivate);
void gamemenu_speed(bool bActivate);

/** Contains the game menu items of the single player menu. */
TMenuItem sgSingleMenu[] = {
	// clang-format off
    // dwFlags,      pszStr,              fnMenu
	{ GMENU_ENABLED, N_("Save Game"),     &gamemenu_save_game  },
	{ GMENU_ENABLED, N_("Options"),       &gamemenu_options    },
	{ GMENU_ENABLED, N_("New Game"),      &gamemenu_new_game   },
	{ GMENU_ENABLED, N_("Load Game"),     &gamemenu_load_game  },
	{ GMENU_ENABLED, N_("Quit Game"),     &gamemenu_quit_game  },
	{ GMENU_ENABLED, nullptr,              nullptr             }
	// clang-format on
};
/** Contains the game menu items of the multi player menu. */
TMenuItem sgMultiMenu[] = {
	// clang-format off
    // dwFlags,      pszStr,                fnMenu
	{ GMENU_ENABLED, N_("Options"),         &gamemenu_options      },
	{ GMENU_ENABLED, N_("New Game"),        &gamemenu_new_game     },
	{ GMENU_ENABLED, N_("Restart In Town"), &gamemenu_restart_town },
	{ GMENU_ENABLED, N_("Quit Game"),       &gamemenu_quit_game    },
	{ GMENU_ENABLED, nullptr,                nullptr               },
	// clang-format on
};
TMenuItem sgOptionsMenu[] = {
	// clang-format off
    // dwFlags,                     pszStr,              fnMenu
	{ GMENU_ENABLED | GMENU_SLIDER, nullptr,             &gamemenu_music_volume  },
	{ GMENU_ENABLED | GMENU_SLIDER, nullptr,             &gamemenu_sound_volume  },
	{ GMENU_ENABLED | GMENU_SLIDER, N_("Gamma"),         &gamemenu_gamma         },
	{ GMENU_ENABLED | GMENU_SLIDER, N_("Speed"),         &gamemenu_speed         },
	{ GMENU_ENABLED               , N_("Previous Menu"), &gamemenu_previous      },
	{ GMENU_ENABLED               , nullptr,              nullptr                },
	// clang-format on
};
/** Specifies the menu names for music enabled and disabled. */
const char *const MusicToggleNames[] = {
	N_("Music"),
	N_("Music Disabled"),
};
/** Specifies the menu names for sound enabled and disabled. */
const char *const SoundToggleNames[] = {
	N_("Sound"),
	N_("Sound Disabled"),
};

void gamemenu_update_single()
{
	gmenu_enable(&sgSingleMenu[3], gbValidSaveFile);

	bool enable = plr[myplr]._pmode != PM_DEATH && !deathflag;

	gmenu_enable(&sgSingleMenu[0], enable);
}

void gamemenu_update_multi()
{
	gmenu_enable(&sgMultiMenu[2], deathflag);
}

void gamemenu_previous(bool /*bActivate*/)
{
	gamemenu_on();
}

void gamemenu_new_game(bool /*bActivate*/)
{
	for (auto &player : plr) {
		player._pmode = PM_QUIT;
		player._pInvincible = true;
	}

	deathflag = false;
	force_redraw = 255;
	scrollrt_draw_game_screen();
	CornerStone.activated = false;
	gbRunGame = false;
	gamemenu_off();
}

void gamemenu_restart_town(bool /*bActivate*/)
{
	NetSendCmd(true, CMD_RETOWN);
}

void gamemenu_sound_music_toggle(const char *const *names, TMenuItem *menu_item, int volume)
{
#ifndef NOSOUND
	if (gbSndInited) {
		menu_item->dwFlags |= GMENU_ENABLED | GMENU_SLIDER;
		menu_item->pszStr = names[0];
		gmenu_slider_steps(menu_item, VOLUME_STEPS);
		gmenu_slider_set(menu_item, VOLUME_MIN, VOLUME_MAX, volume);
		return;
	}
#endif

	menu_item->dwFlags &= ~(GMENU_ENABLED | GMENU_SLIDER);
	menu_item->pszStr = names[1];
}

int gamemenu_slider_music_sound(TMenuItem *menu_item)
{
	return gmenu_slider_get(menu_item, VOLUME_MIN, VOLUME_MAX);
}

void gamemenu_get_music()
{
	gamemenu_sound_music_toggle(MusicToggleNames, sgOptionsMenu, sound_get_or_set_music_volume(1));
}

void gamemenu_get_sound()
{
	gamemenu_sound_music_toggle(SoundToggleNames, &sgOptionsMenu[1], sound_get_or_set_sound_volume(1));
}

void gamemenu_get_gamma()
{
	gmenu_slider_steps(&sgOptionsMenu[2], 15);
	gmenu_slider_set(&sgOptionsMenu[2], 30, 100, UpdateGamma(0));
}

void gamemenu_get_speed()
{
	if (gbIsMultiplayer) {
		sgOptionsMenu[3].dwFlags &= ~(GMENU_ENABLED | GMENU_SLIDER);
		if (sgGameInitInfo.nTickRate >= 50)
			sgOptionsMenu[3].pszStr = _("Speed: Fastest");
		else if (sgGameInitInfo.nTickRate >= 40)
			sgOptionsMenu[3].pszStr = _("Speed: Faster");
		else if (sgGameInitInfo.nTickRate >= 30)
			sgOptionsMenu[3].pszStr = _("Speed: Fast");
		else if (sgGameInitInfo.nTickRate == 20)
			sgOptionsMenu[3].pszStr = _("Speed: Normal");
		return;
	}

	sgOptionsMenu[3].dwFlags |= GMENU_ENABLED | GMENU_SLIDER;

	sgOptionsMenu[3].pszStr = _("Speed");
	gmenu_slider_steps(&sgOptionsMenu[3], 46);
	gmenu_slider_set(&sgOptionsMenu[3], 20, 50, sgGameInitInfo.nTickRate);
}

int gamemenu_slider_gamma()
{
	return gmenu_slider_get(&sgOptionsMenu[2], 30, 100);
}

void gamemenu_options(bool /*bActivate*/)
{
	gamemenu_get_music();
	gamemenu_get_sound();
	gamemenu_get_gamma();
	gamemenu_get_speed();
	gmenu_set_items(sgOptionsMenu, nullptr);
}

void gamemenu_music_volume(bool bActivate)
{
	int volume;

#ifndef NOSOUND
	if (bActivate) {
		if (gbMusicOn) {
			gbMusicOn = false;
			music_stop();
			sound_get_or_set_music_volume(VOLUME_MIN);
		} else {
			gbMusicOn = true;
			sound_get_or_set_music_volume(VOLUME_MAX);
			int lt;
			if (currlevel >= 17) {
				if (currlevel > 20)
					lt = TMUSIC_L5;
				else
					lt = TMUSIC_L6;
			} else
				lt = leveltype;
			music_start(lt);
		}
	} else {
		volume = gamemenu_slider_music_sound(&sgOptionsMenu[0]);
		sound_get_or_set_music_volume(volume);
		if (volume == VOLUME_MIN) {
			if (gbMusicOn) {
				gbMusicOn = false;
				music_stop();
			}
		} else if (!gbMusicOn) {
			gbMusicOn = true;
			int lt;
			if (currlevel >= 17) {
				if (currlevel > 20)
					lt = TMUSIC_L5;
				else
					lt = TMUSIC_L6;
			} else
				lt = leveltype;
			music_start(lt);
		}
	}
#endif
	gamemenu_get_music();
}

void gamemenu_sound_volume(bool bActivate)
{
#ifndef NOSOUND
	int volume;
	if (bActivate) {
		if (gbSoundOn) {
			gbSoundOn = false;
			sound_stop();
			sound_get_or_set_sound_volume(VOLUME_MIN);
		} else {
			gbSoundOn = true;
			sound_get_or_set_sound_volume(VOLUME_MAX);
		}
	} else {
		volume = gamemenu_slider_music_sound(&sgOptionsMenu[1]);
		sound_get_or_set_sound_volume(volume);
		if (volume == VOLUME_MIN) {
			if (gbSoundOn) {
				gbSoundOn = false;
				sound_stop();
			}
		} else if (!gbSoundOn) {
			gbSoundOn = true;
		}
	}
	PlaySFX(IS_TITLEMOV);
	gamemenu_get_sound();
#endif
}

void gamemenu_gamma(bool bActivate)
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

void gamemenu_speed(bool bActivate)
{
	if (bActivate) {
		if (sgGameInitInfo.nTickRate != 20)
			sgGameInitInfo.nTickRate = 20;
		else
			sgGameInitInfo.nTickRate = 50;
		gmenu_slider_set(&sgOptionsMenu[3], 20, 50, sgGameInitInfo.nTickRate);
	} else {
		sgGameInitInfo.nTickRate = gmenu_slider_get(&sgOptionsMenu[3], 20, 50);
	}

	sgOptions.Gameplay.nTickRate = sgGameInitInfo.nTickRate;
	gnTickDelay = 1000 / sgGameInitInfo.nTickRate;
}

} // namespace

void gamemenu_quit_game(bool bActivate)
{
	gamemenu_new_game(bActivate);
	gbRunGameResult = false;
}

void gamemenu_load_game(bool /*bActivate*/)
{
	WNDPROC saveProc = SetWindowProc(DisableInputWndProc);
	gamemenu_off();
	NewCursor(CURSOR_NONE);
	InitDiabloMsg(EMSG_LOADING);
	force_redraw = 255;
	DrawAndBlit();
	LoadGame(false);
	ClrDiabloMsg();
	CornerStone.activated = false;
	PaletteFadeOut(8);
	deathflag = false;
	force_redraw = 255;
	DrawAndBlit();
	LoadPWaterPalette();
	PaletteFadeIn(8);
	NewCursor(CURSOR_HAND);
	interface_msg_pump();
	SetWindowProc(saveProc);
}

void gamemenu_save_game(bool /*bActivate*/)
{
	if (pcurs != CURSOR_HAND) {
		return;
	}

	if (plr[myplr]._pmode == PM_DEATH || deathflag) {
		gamemenu_off();
		return;
	}

	WNDPROC saveProc = SetWindowProc(DisableInputWndProc);
	NewCursor(CURSOR_NONE);
	gamemenu_off();
	InitDiabloMsg(EMSG_SAVING);
	force_redraw = 255;
	DrawAndBlit();
	SaveGame();
	ClrDiabloMsg();
	force_redraw = 255;
	NewCursor(CURSOR_HAND);
	if (CornerStone.activated) {
		CornerstoneSave();
	}
	interface_msg_pump();
	SetWindowProc(saveProc);
}

void gamemenu_on()
{
	if (!gbIsMultiplayer) {
		gmenu_set_items(sgSingleMenu, gamemenu_update_single);
	} else {
		gmenu_set_items(sgMultiMenu, gamemenu_update_multi);
	}
	PressEscKey();
}

void gamemenu_off()
{
	gmenu_set_items(nullptr, nullptr);
}

void gamemenu_handle_previous()
{
	if (gmenu_is_active())
		gamemenu_off();
	else
		gamemenu_on();
}

} // namespace devilution
