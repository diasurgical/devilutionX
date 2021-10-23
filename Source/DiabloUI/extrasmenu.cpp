#include "selstart.h"

#include "control.h"
#include "DiabloUI/diabloui.h"
#include "options.h"
#include "utils/language.h"

namespace devilution {
namespace {

bool endMenu;

std::vector<std::unique_ptr<UiListItem>> vecDialogItems;
std::vector<std::unique_ptr<UiItemBase>> vecDialog;

Art artLogo;

_mainmenu_selections selectionExtrasMenu;

void ItemSelected(int value)
{
	selectionExtrasMenu = static_cast<_mainmenu_selections>(vecDialogItems[value]->m_value);
	endMenu = true;
}

void EscPressed()
{
	selectionExtrasMenu = MAINMENU_NONE;
	endMenu = true;
}

} // namespace

_mainmenu_selections UiExtrasMenu()
{
	LoadBackgroundArt("ui_art\\black.pcx");
	UiAddBackground(&vecDialog);
	UiAddLogo(&vecDialog);

	if (diabdat_mpq && hellfire_mpq)
		vecDialogItems.push_back(std::make_unique<UiListItem>(gbIsHellfire ? _("Switch to Diablo") : _("Switch to Hellfire"), MAINMENU_SWITCHGAME));
	if (diabdat_mpq)
		vecDialogItems.push_back(std::make_unique<UiListItem>(gbIsSpawn ? _("Switch to Fullgame") : _("Switch to Shareware"), MAINMENU_TOGGLESPAWN));
	vecDialogItems.push_back(std::make_unique<UiListItem>(_("Replay Intro"), MAINMENU_REPLAY_INTRO));
	vecDialogItems.push_back(std::make_unique<UiListItem>(_("Support"), MAINMENU_SHOW_SUPPORT));
	vecDialogItems.push_back(std::make_unique<UiListItem>(_("Previous Menu"), MAINMENU_NONE));
	vecDialog.push_back(std::make_unique<UiList>(vecDialogItems, PANEL_LEFT + 34, (UI_OFFSET_Y + 240), 570, 43, UiFlags::AlignCenter | UiFlags::FontSize42 | UiFlags::ColorUiGold, 5));

	UiInitList(vecDialogItems.size(), nullptr, ItemSelected, EscPressed, vecDialog, true);

	endMenu = false;
	while (!endMenu) {
		UiClearScreen();
		UiRenderItems(vecDialog);
		UiPollAndRender();
	}

	ArtBackground.Unload();
	ArtBackgroundWidescreen.Unload();
	vecDialogItems.clear();
	vecDialog.clear();

	return selectionExtrasMenu;
}

} // namespace devilution
