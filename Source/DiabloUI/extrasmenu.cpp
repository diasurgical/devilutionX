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
	endMenu = true;
}

} // namespace

_mainmenu_selections UiExtrasMenu()
{
	constexpr int MessageWidth = 560;

	LoadBackgroundArt("ui_art\\black.pcx");
	UiAddBackground(&vecDialog);
	UiAddLogo(&vecDialog);

	if (!gbIsSpawn && diabdat_mpq != nullptr && hellfire_mpq != nullptr)
		vecDialogItems.push_back(std::make_unique<UiListItem>(gbIsHellfire ? _("Switch to Diablo") : _("Switch to Hellfire"), MAINMENU_SWITCHGAME));
	vecDialogItems.push_back(std::make_unique<UiListItem>(_("Replay Intro"), MAINMENU_REPLAY_INTRO));
	vecDialogItems.push_back(std::make_unique<UiListItem>(_("Support"), MAINMENU_SHOW_SUPPORT));
	vecDialogItems.push_back(std::make_unique<UiListItem>(_("Previous Menu"), MAINMENU_NONE));
	vecDialog.push_back(std::make_unique<UiList>(vecDialogItems, PANEL_LEFT + 64, (UI_OFFSET_Y + 240), 510, 43, UiFlags::AlignCenter | UiFlags::FontSize42 | UiFlags::ColorUiGold, 5));

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
