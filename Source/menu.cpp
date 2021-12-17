#include "menu.h"

#include "DiabloUI/diabloui.h"
#include "DiabloUI/settingsmenu.h"
#include "engine/demomode.h"
#include "init.h"
#include "main_loop.hpp"
#include "movie.h"
#include "options.h"
#include "pfile.h"
#include "storm/storm_net.hpp"
#include "utils/language.h"

namespace devilution {

uint32_t gSaveNumber;

namespace {

/** The active music track id for the main menu. */
uint8_t menu_music_track_id = TMUSIC_INTRO;

void RefreshMusic()
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

bool InitMenu(_selhero_selections type)
{
	bool success;

	if (type == SELHERO_PREVIOUS)
		return true;

	music_stop();

	success = StartGame(type != SELHERO_CONTINUE, type != SELHERO_CONNECT);
	if (success)
		RefreshMusic();

	return success;
}

bool InitSinglePlayerMenu()
{
	gbIsMultiplayer = false;
	return InitMenu(SELHERO_NEW_DUNGEON);
}

bool InitMultiPlayerMenu()
{
	gbIsMultiplayer = true;
	return InitMenu(SELHERO_CONNECT);
}

void PlayIntro()
{
	music_stop();
	if (gbIsHellfire)
		play_movie("gendata\\Hellfire.smk", true);
	else
		play_movie("gendata\\diablo1.smk", true);
	RefreshMusic();
}

bool DummyGetHeroInfo(_uiheroinfo * /*pInfo*/)
{
	return true;
}

void WaitForButtonSound()
{
	SDL_FillRect(DiabloUiSurface(), nullptr, 0x000000);
	UiFadeIn();
	SDL_Delay(350); // delay to let button pressed sound finish playing
}

class MainMenuSelectedHandler : public MainLoopHandler {
public:
	MainMenuSelectedHandler()
	{
		RefreshMusic();
		AddNextMainLoopHandler(MainMenuSelectedHandler::Create);

		if (selected_ == MAINMENU_NONE) {
			if (!UiMainMenuDialog(gszProductName, &selected_, effects_play_sound, 5))
				app_fatal("%s", _("Unable to display mainmenu"));
			return;
		}

		switch (selected_) {
		case MAINMENU_NONE:
			break;
		case MAINMENU_SINGLE_PLAYER:
			if (!InitSinglePlayerMenu()) {
				SetMainLoopHandler(nullptr);
			} else {
				selected_ = MAINMENU_NONE;
				NextMainLoopHandler();
			}
			break;
		case MAINMENU_MULTIPLAYER:
			if (!InitMultiPlayerMenu()) {
				SetMainLoopHandler(nullptr);
			} else {
				selected_ = MAINMENU_NONE;
				NextMainLoopHandler();
			}
			break;
		case MAINMENU_ATTRACT_MODE:
			if (gbActive) {
				selected_ = MAINMENU_NONE;
				PlayIntro();
			}
			break;
		case MAINMENU_SHOW_CREDITS:
			UiCreditsDialog();
			selected_ = MAINMENU_NONE;
			NextMainLoopHandler();
			break;
		case MAINMENU_SHOW_SUPPORT:
			UiSupportDialog();
			selected_ = MAINMENU_NONE;
			NextMainLoopHandler();
			break;
		case MAINMENU_EXIT_DIABLO:
			WaitForButtonSound();
			SetMainLoopHandler(nullptr);
			break;
		case MAINMENU_SETTINGS:
			UiSettingsMenu();
			selected_ = MAINMENU_NONE;
			NextMainLoopHandler();
			break;
		}
	}

	~MainMenuSelectedHandler() override
	{
		music_stop();
	}

	static std::unique_ptr<MainMenuSelectedHandler> Create()
	{
		return std::make_unique<MainMenuSelectedHandler>();
	}

	static void SetSelected(_mainmenu_selections value)
	{
		selected_ = value;
	}

private:
	static _mainmenu_selections selected_;
};

_mainmenu_selections MainMenuSelectedHandler::selected_ = MAINMENU_NONE;

} // namespace

bool mainmenu_select_hero_dialog(GameData *gameData)
{
	uint32_t *pSaveNumberFromOptions = nullptr;
	_selhero_selections dlgresult = SELHERO_NEW_DUNGEON;
	if (demo::IsRunning()) {
		pfile_ui_set_hero_infos(DummyGetHeroInfo);
		gbLoadGame = true;
	} else if (!gbIsMultiplayer) {
		pSaveNumberFromOptions = gbIsHellfire ? &sgOptions.Hellfire.lastSinglePlayerHero : &sgOptions.Diablo.lastSinglePlayerHero;
		gSaveNumber = *pSaveNumberFromOptions;
		UiSelHeroSingDialog(
		    pfile_ui_set_hero_infos,
		    pfile_ui_save_create,
		    pfile_delete_save,
		    pfile_ui_set_class_stats,
		    &dlgresult,
		    &gSaveNumber,
		    &gameData->nDifficulty);

		gbLoadGame = (dlgresult == SELHERO_CONTINUE);
	} else {
		pSaveNumberFromOptions = gbIsHellfire ? &sgOptions.Hellfire.lastMultiplayerHero : &sgOptions.Diablo.lastMultiplayerHero;
		gSaveNumber = *pSaveNumberFromOptions;
		UiSelHeroMultDialog(
		    pfile_ui_set_hero_infos,
		    pfile_ui_save_create,
		    pfile_delete_save,
		    pfile_ui_set_class_stats,
		    &dlgresult,
		    &gSaveNumber);
	}
	if (dlgresult == SELHERO_PREVIOUS) {
		SErrSetLastError(1223);
		return false;
	}

	if (pSaveNumberFromOptions != nullptr)
		*pSaveNumberFromOptions = gSaveNumber;

	pfile_read_player_from_save(gSaveNumber, Players[MyPlayerId]);

	return true;
}

std::unique_ptr<MainLoopHandler> CreateMenuLoopHandler()
{
	MainMenuSelectedHandler::SetSelected(demo::IsRunning() ? MAINMENU_SINGLE_PLAYER : MAINMENU_NONE);
	return std::make_unique<MainMenuSelectedHandler>();
}

} // namespace devilution
