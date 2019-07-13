#include "devilution.h"
#include "DiabloUI/diabloui.h"

namespace dvl {

int mainmenu_attract_time_out; //seconds
DWORD dwAttractTicks;

int MainMenuResult;
UiListItem MAINMENU_DIALOG_ITEMS[] = {
	{"Single Player", MAINMENU_SINGLE_PLAYER},
	{"Multi Player", MAINMENU_MULTIPLAYER},
	{"Replay Intro", MAINMENU_REPLAY_INTRO},
	{"Show Credits", MAINMENU_SHOW_CREDITS},
	{"Exit Diablo", MAINMENU_EXIT_DIABLO}
};
UiItem MAINMENU_DIALOG[] = {
	MAINMENU_BACKGROUND,
	MAINMENU_LOGO,
	UiList(MAINMENU_DIALOG_ITEMS, 64, 192, 510, 43, UIS_HUGE | UIS_GOLD | UIS_CENTER),
	UiArtText(nullptr, { 17, 444, 605, 21 }, UIS_SMALL)
};

void UiMainMenuSelect(int value)
{
	MainMenuResult = value;
}

void mainmenu_Esc()
{
#ifndef SWITCH
	UiMainMenuSelect(MAINMENU_EXIT_DIABLO);
#endif
}

void mainmenu_restart_repintro()
{
	dwAttractTicks = GetTickCount() + mainmenu_attract_time_out * 1000;
}

void mainmenu_Load(char *name, void (*fnSound)(char *file))
{
	gfnSoundFunction = fnSound;
	MAINMENU_DIALOG[size(MAINMENU_DIALOG) - 1].art_text.text = name;

	MainMenuResult = 0;

	char *pszFile = "ui_art\\mainmenu.pcx";
	if (false) //DiabloUI_GetSpawned()
		pszFile = "ui_art\\swmmenu.pcx";
	LoadBackgroundArt(pszFile);

	UiInitList(MAINMENU_SINGLE_PLAYER, MAINMENU_EXIT_DIABLO, NULL, UiMainMenuSelect, mainmenu_Esc, MAINMENU_DIALOG, size(MAINMENU_DIALOG), true);
}

void mainmenu_Free()
{
	ArtBackground.Unload();
}

BOOL UiMainMenuDialog(char *name, int *pdwResult, void (*fnSound)(char *file), int attractTimeOut)
{
	mainmenu_attract_time_out = attractTimeOut;
	mainmenu_Load(name, fnSound);

	mainmenu_restart_repintro(); // for automatic starts

	while (MainMenuResult == 0) {
		UiPollAndRender();
		if (GetTickCount() >= dwAttractTicks) {
			MainMenuResult = MAINMENU_ATTRACT_MODE;
		}
	}

	BlackPalette();
	mainmenu_Free();

	*pdwResult = MainMenuResult;
	return true;
}

}
