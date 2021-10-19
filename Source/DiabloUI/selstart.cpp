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

void ItemSelected(int value)
{
	auto option = static_cast<StartUpGameOption>(vecDialogItems[value]->m_value);
	sgOptions.Hellfire.startUpGameOption = option;
	gbIsHellfire = option == StartUpGameOption::Hellfire;
	endMenu = true;
}

void EscPressed()
{
	endMenu = true;
}

void FocusChanged(int value)
{
	auto option = static_cast<StartUpGameOption>(vecDialogItems[value]->m_value);
	gbIsHellfire = option == StartUpGameOption::Hellfire;
	ArtBackground.Unload();
	ArtBackgroundWidescreen.Unload();
	LoadBackgroundArt("ui_art\\mainmenu.pcx", 1, false);
	SetFadeLevel(256);
	artLogo.Unload();
	if (gbIsHellfire) {
		LoadArt("ui_art\\mainmenuw.pcx", &ArtBackgroundWidescreen);
		LoadMaskedArt("ui_art\\hf_logo2.pcx", &artLogo, 16);
	} else {
		LoadMaskedArt("ui_art\\smlogo.pcx", &artLogo, 15);
	}
	gbIsHellfire = true;
}

} // namespace

void UiSelStartUpGameOption()
{
	LoadArt("ui_art\\mainmenuw.pcx", &ArtBackgroundWidescreen);
	LoadBackgroundArt("ui_art\\mainmenu.pcx");
	UiAddBackground(&vecDialog);

	SDL_Rect rect = { 0, (Sint16)(UI_OFFSET_Y), 0, 0 };
	vecDialog.push_back(std::make_unique<UiImage>(&artLogo, rect, UiFlags::AlignCenter, /*bAnimated=*/true));

	vecDialogItems.push_back(std::make_unique<UiListItem>(_("Enter Hellfire"), static_cast<int>(StartUpGameOption::Hellfire)));
	vecDialogItems.push_back(std::make_unique<UiListItem>(_("Switch to Diablo"), static_cast<int>(StartUpGameOption::Diablo)));
	vecDialog.push_back(std::make_unique<UiList>(vecDialogItems, PANEL_LEFT + 64, (UI_OFFSET_Y + 240), 510, 43, UiFlags::AlignCenter | UiFlags::FontSize42 | UiFlags::ColorUiGold, 5));

	UiInitList(vecDialogItems.size(), FocusChanged, ItemSelected, EscPressed, vecDialog, true);

	endMenu = false;
	while (!endMenu) {
		UiClearScreen();
		UiRenderItems(vecDialog);
		UiPollAndRender();
	}

	artLogo.Unload();
	ArtBackground.Unload();
	ArtBackgroundWidescreen.Unload();
	vecDialogItems.clear();
	vecDialog.clear();
}

} // namespace devilution
