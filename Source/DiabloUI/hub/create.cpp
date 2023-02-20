#include "DiabloUI/hub/create.h"

#include "DiabloUI/hub/hub.h"
#include "engine/load_pcx.hpp"

namespace devilution {

namespace {

OptionalOwnedClxSpriteList CreateButton;

void DialogActionOK()
{
}

} // namespace

void HubLoadCreate()
{
	Layout = LoadPcx("ui_art\\creat_bg", /*transparentColor=*/0);
	CreateButton = LoadPcxSpriteList("ui_art\\diffbtns", -68);
}

void HubInitCreate()
{
	const Point uiPosition = GetUIRectangle().position;

	const SDL_Rect rect0 = MakeSdlRect(uiPosition.x + 17, uiPosition.y + 154, 274, 26);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(_("Difficulty").data(), rect0, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite | UiFlags::AlignCenter, -1));

	vecHubMainDialog.push_back(std::make_unique<UiImageClx>((*CreateButton)[1], MakeSdlRect(uiPosition.x + 115, uiPosition.y + 187, 85, 68)));
	const SDL_Rect rect1 = MakeSdlRect(uiPosition.x + 115, uiPosition.y + 227, 85, 26);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(_("Normal").data(), rect1, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite | UiFlags::AlignCenter, -2));
	vecHubMainDialog.push_back(std::make_unique<UiImageClx>((*CreateButton)[2], MakeSdlRect(uiPosition.x + 62, uiPosition.y + 263, 85, 68)));
	const SDL_Rect rect2 = MakeSdlRect(uiPosition.x + 62, uiPosition.y + 303, 85, 26);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(_("Nightmare").data(), rect2, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite | UiFlags::AlignCenter, -2));
	vecHubMainDialog.push_back(std::make_unique<UiImageClx>((*CreateButton)[4], MakeSdlRect(uiPosition.x + 167, uiPosition.y + 263, 85, 68)));
	const SDL_Rect rect3 = MakeSdlRect(uiPosition.x + 167, uiPosition.y + 303, 85, 26);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(_("Hell").data(), rect3, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite | UiFlags::AlignCenter, -2));

	const SDL_Rect rect4 = MakeSdlRect(uiPosition.x + 26, uiPosition.y + 340, 262, 110);
	vecHubMainDialog.push_back(std::make_unique<UiArtText>(_("Normal Difficulty\nThis is where a starting\ncharacter should begin the quest\nto defeat Diablo.").data(), rect4, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite, -1, 19));

	const SDL_Rect rect7 = MakeSdlRect(uiPosition.x + 264 + 188, uiPosition.y + 335 + 103, 85, 35);
	vecHubMainDialog.push_back(std::make_unique<UiButton>(_("OK"), &DialogActionOK, rect7));

	const SDL_Rect rect8 = MakeSdlRect(uiPosition.x + 264 + 283, uiPosition.y + 335 + 103, 85, 35);
	vecHubMainDialog.push_back(std::make_unique<UiButton>(_("Cancel"), &DialogActionOK, rect8));
}

} // namespace devilution
