#include "all.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/selok.h"

namespace dvl {

int mainmenu_attract_time_out; //seconds
DWORD dwAttractTicks;

std::vector<UiItemBase *> vecMainMenuDialog;
std::vector<UiListItem *> vecMenuItems;

int MainMenuResult;

void UiMainMenuSelect(int value)
{
	MainMenuResult = value;
}

void mainmenu_Esc()
{
	if (SelectedItem == MAINMENU_EXIT_DIABLO) {
		UiMainMenuSelect(MAINMENU_EXIT_DIABLO);
	} else {
		SelectedItem = MAINMENU_EXIT_DIABLO;
	}
}

void mainmenu_restart_repintro()
{
	dwAttractTicks = SDL_GetTicks() + mainmenu_attract_time_out * 1000;
}

void mainmenu_Load(char *name, void (*fnSound)(char *file))
{
	gfnSoundFunction = fnSound;

	vecMenuItems.push_back(new UiListItem("Single Player", MAINMENU_SINGLE_PLAYER));
	vecMenuItems.push_back(new UiListItem("Multi Player", MAINMENU_MULTIPLAYER));
	vecMenuItems.push_back(new UiListItem("Replay Intro", MAINMENU_REPLAY_INTRO));
	vecMenuItems.push_back(new UiListItem("Show Credits", MAINMENU_SHOW_CREDITS));
	vecMenuItems.push_back(new UiListItem("Exit Diablo", MAINMENU_EXIT_DIABLO));

	UiAddBackground(&vecMainMenuDialog);
	UiAddLogo(&vecMainMenuDialog);

	vecMainMenuDialog.push_back(new UiList(vecMenuItems, PANEL_LEFT + 64, 192, 510, 43, UIS_HUGE | UIS_GOLD | UIS_CENTER));

	SDL_Rect rect = { 17, 444, 605, 21 };
	vecMainMenuDialog.push_back(new UiArtText(name, rect, UIS_SMALL));

	if (!gbSpawned) {
		LoadBackgroundArt("ui_art\\mainmenu.pcx");
	} else {
		LoadBackgroundArt("ui_art\\swmmenu.pcx");
	}

	UiInitList(MAINMENU_SINGLE_PLAYER, MAINMENU_EXIT_DIABLO, NULL, UiMainMenuSelect, mainmenu_Esc, vecMainMenuDialog, true);
}

void mainmenu_Free()
{
	ArtBackground.Unload();

	for (std::size_t i = 0; i < vecMainMenuDialog.size(); i++) {
		UiItemBase *pUIItem = vecMainMenuDialog[i];
		delete pUIItem;
	}
	vecMainMenuDialog.clear();

	for (std::size_t i = 0; i < vecMenuItems.size(); i++) {
		UiListItem *pUIMenuItem = vecMenuItems[i];
		if (pUIMenuItem)
			delete pUIMenuItem;
	}
	vecMenuItems.clear();
}

BOOL UiMainMenuDialog(char *name, int *pdwResult, void (*fnSound)(char *file), int attractTimeOut)
{
	MainMenuResult = 0;
	while (MainMenuResult == 0) {
		mainmenu_attract_time_out = attractTimeOut;
		mainmenu_Load(name, fnSound);

		mainmenu_restart_repintro(); // for automatic starts

		while (MainMenuResult == 0) {
			UiClearScreen();
			UiPollAndRender();
			if (!gbSpawned && SDL_GetTicks() >= dwAttractTicks) {
				MainMenuResult = MAINMENU_ATTRACT_MODE;
			}
		}

		mainmenu_Free();

		if (gbSpawned && MainMenuResult == MAINMENU_REPLAY_INTRO) {
			UiSelOkDialog(NULL, "The Diablo introduction cinematic is only available in the full retail version of Diablo. Visit https://www.gog.com/game/diablo to purchase.", true);
			MainMenuResult = 0;
		}
	}

	*pdwResult = MainMenuResult;
	return true;
}

} // namespace dvl
