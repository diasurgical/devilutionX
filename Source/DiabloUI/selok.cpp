#include "DiabloUI/selok.h"

#include "DiabloUI/diabloui.h"
#include "DiabloUI/text.h"
#include "control.h"
#include "utils/language.h"

namespace devilution {

namespace {

char dialogText[256];

} // namespace

bool selok_endMenu;

std::vector<std::unique_ptr<UiListItem>> vecSelOkDialogItems;
std::vector<std::unique_ptr<UiItemBase>> vecSelOkDialog;

#define MESSAGE_WIDTH 280

void selok_Free()
{
	ArtBackground.Unload();

	vecSelOkDialogItems.clear();

	vecSelOkDialog.clear();
}

void selok_Select(int /*value*/)
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

	if (title != nullptr) {
		SDL_Rect rect1 = { (Sint16)(PANEL_LEFT + 24), (Sint16)(UI_OFFSET_Y + 161), 590, 35 };
		vecSelOkDialog.push_back(std::make_unique<UiArtText>(title, rect1, UiFlags::UIS_CENTER | UiFlags::UIS_BIG));

		SDL_Rect rect2 = { (Sint16)(PANEL_LEFT + 140), (Sint16)(UI_OFFSET_Y + 210), 560, 168 };
		vecSelOkDialog.push_back(std::make_unique<UiArtText>(dialogText, rect2, UiFlags::UIS_MED));
	} else {
		SDL_Rect rect1 = { (Sint16)(PANEL_LEFT + 140), (Sint16)(UI_OFFSET_Y + 197), 560, 168 };
		vecSelOkDialog.push_back(std::make_unique<UiArtText>(dialogText, rect1, UiFlags::UIS_MED));
	}

	vecSelOkDialogItems.push_back(std::make_unique<UiListItem>(_("OK"), 0));
	vecSelOkDialog.push_back(std::make_unique<UiList>(vecSelOkDialogItems, PANEL_LEFT + 230, (UI_OFFSET_Y + 390), 180, 35, UiFlags::UIS_CENTER | UiFlags::UIS_BIG | UiFlags::UIS_GOLD));

	strncpy(dialogText, body, sizeof(dialogText) - 1);
	WordWrapArtStr(dialogText, MESSAGE_WIDTH);

	UiInitList(0, nullptr, selok_Select, selok_Esc, vecSelOkDialog, false, nullptr);

	selok_endMenu = false;
	while (!selok_endMenu) {
		UiClearScreen();
		UiRenderItems(vecSelOkDialog);
		UiPollAndRender();
	}

	selok_Free();
}
} // namespace devilution
