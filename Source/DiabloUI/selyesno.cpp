#include "selyesno.h"

#include "DiabloUI/diabloui.h"
#include "control.h"
#include "utils/language.h"

namespace devilution {
namespace {

bool selyesno_endMenu;
bool selyesno_value;
char selyesno_confirmationMessage[256];

std::vector<std::unique_ptr<UiListItem>> vecSelYesNoDialogItems;
std::vector<std::unique_ptr<UiItemBase>> vecSelYesNoDialog;

#define MESSAGE_WIDTH 400

void SelyesnoFree()
{
	ArtBackground.Unload();

	vecSelYesNoDialogItems.clear();

	vecSelYesNoDialog.clear();
}

void SelyesnoSelect(int value)
{
	selyesno_value = vecSelYesNoDialogItems[value]->m_value == 0;
	selyesno_endMenu = true;
}

void SelyesnoEsc()
{
	selyesno_value = false;
	selyesno_endMenu = true;
}

} // namespace

bool UiSelHeroYesNoDialog(const char *title, const char *body)
{
	LoadBackgroundArt("ui_art\\black.pcx");
	UiAddBackground(&vecSelYesNoDialog);
	UiAddLogo(&vecSelYesNoDialog);

	SDL_Rect rect1 = { (Sint16)(PANEL_LEFT + 24), (Sint16)(UI_OFFSET_Y + 161), 590, 35 };
	vecSelYesNoDialog.push_back(std::make_unique<UiArtText>(title, rect1, UiFlags::AlignCenter | UiFlags::FontSize30 | UiFlags::ColorUiSilver, 3));

	SDL_Rect rect2 = { (Sint16)(PANEL_LEFT + 120), (Sint16)(UI_OFFSET_Y + 236), MESSAGE_WIDTH, 168 };
	vecSelYesNoDialog.push_back(std::make_unique<UiArtText>(selyesno_confirmationMessage, rect2, UiFlags::FontSize24 | UiFlags::ColorUiSilver));

	vecSelYesNoDialogItems.push_back(std::make_unique<UiListItem>(_("Yes"), 0));
	vecSelYesNoDialogItems.push_back(std::make_unique<UiListItem>(_("No"), 1));
	vecSelYesNoDialog.push_back(std::make_unique<UiList>(vecSelYesNoDialogItems, vecSelYesNoDialogItems.size(), PANEL_LEFT + 230, (UI_OFFSET_Y + 390), 180, 35, UiFlags::AlignCenter | UiFlags::FontSize30 | UiFlags::ColorUiGold));

	strcpy(selyesno_confirmationMessage, WordWrapString(body, MESSAGE_WIDTH, GameFont24).c_str());

	UiInitList(nullptr, SelyesnoSelect, SelyesnoEsc, vecSelYesNoDialog, true);

	selyesno_value = true;
	selyesno_endMenu = false;
	while (!selyesno_endMenu) {
		UiClearScreen();
		UiRenderItems(vecSelYesNoDialog);
		UiPollAndRender();
	}

	SelyesnoFree();

	return selyesno_value;
}
} // namespace devilution
