#include "devilution.h"
#include "DiabloUI/diabloui.h"

namespace dvl {

int MainMenuResult;
UI_Item MAINMENU_DIALOG[] = {
	{ { 0, 0, 640, 480 }, UI_IMAGE, 0, 0, NULL, &ArtBackground },
	{ { 64, 192, 510, 43 }, UI_LIST, UIS_HUGE | UIS_GOLD | UIS_CENTER, MAINMENU_SINGLE_PLAYER, "Single Player" },
	{ { 64, 235, 510, 43 }, UI_LIST, UIS_HUGE | UIS_GOLD | UIS_CENTER, MAINMENU_MULTIPLAYER, "Multi Player" },
	{ { 64, 277, 510, 43 }, UI_LIST, UIS_HUGE | UIS_GOLD | UIS_CENTER, MAINMENU_REPLAY_INTRO, "Replay Intro" },
	{ { 64, 320, 510, 43 }, UI_LIST, UIS_HUGE | UIS_GOLD | UIS_CENTER, MAINMENU_SHOW_CREDITS, "Show Credits" },
	{ { 64, 363, 510, 43 }, UI_LIST, UIS_HUGE | UIS_GOLD | UIS_CENTER, MAINMENU_EXIT_DIABLO, "Exit Diablo" },
	{ { 17, 444, 605, 21 }, UI_TEXT, UIS_SMALL },
};

void UiMainMenuSelect(int value)
{
	MainMenuResult = value;
}

void mainmenu_Esc()
{
	UiMainMenuSelect(MAINMENU_EXIT_DIABLO);
}

void mainmenu_Load(char *name, void(*fnSound)(char *file))
{
	gfnSoundFunction = fnSound;
	MAINMENU_DIALOG[6].caption = name;

	MainMenuResult = 0;

	char *pszFile = "ui_art\\mainmenu.pcx";
	if (false) //DiabloUI_GetSpawned()
		pszFile = "ui_art\\swmmenu.pcx";
	LoadBackgroundArt(pszFile);

	UiInitList(MAINMENU_SINGLE_PLAYER, MAINMENU_EXIT_DIABLO, NULL, UiMainMenuSelect, mainmenu_Esc, MAINMENU_DIALOG, size(MAINMENU_DIALOG), true);
}

void mainmenu_Free()
{
	mem_free_dbg(ArtBackground.data);
	ArtBackground.data = NULL;
}

BOOL UiMainMenuDialog(char *name, int *pdwResult, void(*fnSound)(char *file), int a4)
{
	mainmenu_Load(name, fnSound);

	while (MainMenuResult == 0) {
		UiRender();
	}

	BlackPalette();
	mainmenu_Free();

	*pdwResult = MainMenuResult;
	return true;
}

}
