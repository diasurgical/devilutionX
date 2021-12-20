#include "DiabloUI/selok.h"

#include "DiabloUI/diabloui.h"
#include "control.h"
#include "engine/render/text_render.hpp"
#include "utils/language.h"

namespace devilution {

namespace {

char dialogText[256];

} // namespace

bool selok_endMenu;

std::vector<std::unique_ptr<UiListItem>> vecSelOkDialogItems;
std::vector<std::unique_ptr<UiItemBase>> vecSelOkDialog;

#define MESSAGE_WIDTH 400

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
		if (!gbIsSpawn) {
			LoadBackgroundArt("ui_art\\mainmenu.pcx");
		} else {
			LoadBackgroundArt("ui_art\\swmmenu.pcx");
		}
	}

	UiAddBackground(&vecSelOkDialog);
	UiAddLogo(&vecSelOkDialog);

	if (title != nullptr) {
		SDL_Rect rect1 = { (Sint16)(PANEL_LEFT + 24), (Sint16)(UI_OFFSET_Y + 161), 590, 35 };
		vecSelOkDialog.push_back(std::make_unique<UiArtText>(title, rect1, UiFlags::AlignCenter | UiFlags::FontSize30 | UiFlags::ColorUiSilver, 3));

		SDL_Rect rect2 = { (Sint16)(PANEL_LEFT + 140), (Sint16)(UI_OFFSET_Y + 210), 560, 168 };
		vecSelOkDialog.push_back(std::make_unique<UiArtText>(dialogText, rect2, UiFlags::FontSize24 | UiFlags::ColorUiSilver));
	} else {
		SDL_Rect rect1 = { (Sint16)(PANEL_LEFT + 140), (Sint16)(UI_OFFSET_Y + 197), 560, 168 };
		vecSelOkDialog.push_back(std::make_unique<UiArtText>(dialogText, rect1, UiFlags::FontSize24 | UiFlags::ColorUiSilver));
	}

	vecSelOkDialogItems.push_back(std::make_unique<UiListItem>(_("OK"), 0));
	vecSelOkDialog.push_back(std::make_unique<UiList>(vecSelOkDialogItems, 0, PANEL_LEFT + 230, (UI_OFFSET_Y + 390), 180, 35, UiFlags::AlignCenter | UiFlags::FontSize30 | UiFlags::ColorUiGold));

	strcpy(dialogText, WordWrapString(body, MESSAGE_WIDTH, GameFont24).c_str());

	UiInitList(nullptr, selok_Select, selok_Esc, vecSelOkDialog, false);

	selok_endMenu = false;
	while (!selok_endMenu) {
		UiClearScreen();
		UiRenderItems(vecSelOkDialog);
		UiPollAndRender();
	}

	selok_Free();
}
} // namespace devilution
