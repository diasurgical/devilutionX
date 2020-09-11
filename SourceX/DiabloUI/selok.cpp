#include "all.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/text.h"
#include "DiabloUI/selok.h"

namespace dvl {

namespace {

char dialogText[256];

} // namespace

int selok_endMenu;

std::vector<UiListItem *> vecSelOkDialogItems;
std::vector<UiItemBase *> vecSelOkDialog;

#define MESSAGE_WIDTH 280

void selok_Free()
{
	ArtBackground.Unload();

	for (std::size_t i = 0; i < vecSelOkDialogItems.size(); i++) {
		UiListItem *pUIListItem = vecSelOkDialogItems[i];
		if (pUIListItem)
			delete pUIListItem;
	}
	vecSelOkDialogItems.clear();

	for (std::size_t i = 0; i < vecSelOkDialog.size(); i++) {
		UiItemBase *pUIItem = vecSelOkDialog[i];
		delete pUIItem;
	}
	vecSelOkDialog.clear();
}

void selok_Select(int value)
{
	selok_endMenu = true;
}

void selok_Esc()
{
	selok_endMenu = true;
}

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

	UiAddBackground(&vecSelOkDialog);
	UiAddLogo(&vecSelOkDialog);

	if (title != NULL) {
		SDL_Rect rect1 = { PANEL_LEFT + 24, (UI_OFFSET_Y + 161), 590, 35 };
		vecSelOkDialog.push_back(new UiArtText(title, rect1, UIS_CENTER | UIS_BIG));

		SDL_Rect rect2 = { PANEL_LEFT + 140, (UI_OFFSET_Y + 210), 560, 168 };
		vecSelOkDialog.push_back(new UiArtText(dialogText, rect2, UIS_MED));
	} else {
		SDL_Rect rect1 = { PANEL_LEFT + 140, (UI_OFFSET_Y + 197), 560, 168 };
		vecSelOkDialog.push_back(new UiArtText(dialogText, rect1, UIS_MED));
	}

	SDL_Rect rect3 = { PANEL_LEFT + 140, (UI_OFFSET_Y + 210), 560, 168 };
	vecSelOkDialog.push_back(new UiArtText(dialogText, rect3, UIS_MED));

	vecSelOkDialogItems.push_back(new UiListItem("OK", 0));
	vecSelOkDialog.push_back(new UiList(vecSelOkDialogItems, PANEL_LEFT + 230, (UI_OFFSET_Y + 390), 180, 35, UIS_CENTER | UIS_BIG | UIS_GOLD));

	strncpy(dialogText, body, sizeof(dialogText) - 1);
	WordWrapArtStr(dialogText, MESSAGE_WIDTH);

	UiInitList(0, 0, NULL, selok_Select, selok_Esc, vecSelOkDialog, false, NULL);

	selok_endMenu = false;
	while (!selok_endMenu) {
		UiClearScreen();
		UiRenderItems(vecSelOkDialog);
		UiPollAndRender();
	}

	selok_Free();
}
} // namespace dvl
