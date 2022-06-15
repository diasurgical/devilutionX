#include "selstart.h"

#include "DiabloUI/diabloui.h"
#include "control.h"
#include "engine/load_pcx.hpp"
#include "options.h"
#include "utils/language.h"
#include "utils/sdl_geometry.h"

namespace devilution {
namespace {

bool endMenu;

std::vector<std::unique_ptr<UiListItem>> vecDialogItems;
std::vector<std::unique_ptr<UiItemBase>> vecDialog;

Art artLogo;

void ItemSelected(int value)
{
	auto option = static_cast<StartUpGameMode>(vecDialogItems[value]->m_value);
	sgOptions.StartUp.gameMode.SetValue(option);
	SaveOptions();
	endMenu = true;
}

void EscPressed()
{
	endMenu = true;
}

} // namespace

void UiSelStartUpGameOption()
{
	ArtBackgroundWidescreen = LoadPcxAsset("ui_art\\mainmenuw.pcx");
	LoadBackgroundArt("ui_art\\mainmenu.pcx");
	LoadMaskedArt("ui_art\\hf_logo2.pcx", &artLogo, 16);
	UiAddBackground(&vecDialog);

	const Point uiPosition = GetUIRectangle().position;
	SDL_Rect rect = MakeSdlRect(0, uiPosition.y, 0, 0);
	vecDialog.push_back(std::make_unique<UiImage>(&artLogo, rect, UiFlags::AlignCenter, /*bAnimated=*/true));

	vecDialogItems.push_back(std::make_unique<UiListItem>(_("Enter Hellfire"), static_cast<int>(StartUpGameMode::Hellfire)));
	vecDialogItems.push_back(std::make_unique<UiListItem>(_("Switch to Diablo"), static_cast<int>(StartUpGameMode::Diablo)));
	vecDialog.push_back(std::make_unique<UiList>(vecDialogItems, vecDialogItems.size(), uiPosition.x + 64, uiPosition.y + 240, 510, 43, UiFlags::AlignCenter | UiFlags::FontSize42 | UiFlags::ColorUiGold, 5));

	UiInitList(nullptr, ItemSelected, EscPressed, vecDialog, true);

	endMenu = false;
	while (!endMenu) {
		UiClearScreen();
		UiRenderItems(vecDialog);
		UiPollAndRender();
	}

	artLogo.Unload();
	ArtBackground = std::nullopt;
	ArtBackgroundWidescreen = std::nullopt;
	vecDialogItems.clear();
	vecDialog.clear();
}

} // namespace devilution
