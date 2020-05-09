#include "all.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/text.h"
#include "DiabloUI/selok.h"

namespace dvl {

namespace {

char dialogText[256];

} // namespace

int selok_endMenu;
char selok_title[32];

void selok_Free()
{
	ArtBackground.Unload();
}

void selok_Select(int value)
{
	selok_endMenu = true;
}

void selok_Esc()
{
	selok_endMenu = true;
}

UiListItem SELOK_DIALOG_ITEMS[] = {
	{ "OK", 0 }
};

UiItem SELOK_DIALOG[] = {
	MAINMENU_BACKGROUND,
	MAINMENU_LOGO,
	UiArtText(selok_title, { PANEL_LEFT + 24, 161, 590, 35 }, UIS_CENTER | UIS_BIG),
	UiArtText(dialogText, { PANEL_LEFT + 140, 210, 560, 168 }, UIS_MED),
	UiList(SELOK_DIALOG_ITEMS, PANEL_LEFT + 230, 390, 180, 35, UIS_CENTER | UIS_BIG | UIS_GOLD)
};

UiItem SPAWNERR_DIALOG[] = {
	MAINMENU_BACKGROUND,
	MAINMENU_LOGO,
	UiArtText(dialogText, { PANEL_LEFT + 140, 197, 560, 168 }, UIS_MED),
	UiList(SELOK_DIALOG_ITEMS, PANEL_LEFT + 230, 390, 180, 35, UIS_CENTER | UIS_BIG | UIS_GOLD)
};

void UiSelOkDialog(const char *title, const char *body, bool background)
{
	if (!background) {
		LoadBackgroundArt("ui_art\\black.pcx");
	} else {
		if (!gbSpawned) {
			LoadBackgroundArt("ui_art\\mainmenu.pcx");
		} else {
			LoadBackgroundArt("ui_art\\swmmenu.pcx");
		}
	}

	UiItem *items = SPAWNERR_DIALOG;
	int itemCnt = size(SPAWNERR_DIALOG);
	if (title != NULL) {
		strcpy(selok_title, title);
		items = SELOK_DIALOG;
		itemCnt = size(SELOK_DIALOG);
	}

	strcpy(dialogText, body);
	WordWrapArtStr(dialogText, 280);

	UiInitList(0, 0, NULL, selok_Select, selok_Esc, items, itemCnt, false, NULL);

	selok_endMenu = false;
	while (!selok_endMenu) {
		UiClearScreen();
		UiRenderItems(items, itemCnt);
		UiPollAndRender();
	}

	selok_Free();
}
} // namespace dvl
