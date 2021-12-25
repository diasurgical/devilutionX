#include "DiabloUI/art_draw.h"
#include "DiabloUI/button.h"
#include "DiabloUI/diabloui.h"
#include "control.h"
#include "controls/input.h"
#include "controls/menu_controls.h"
#include "dx.h"
#include "hwcursor.hpp"
#include "palette.h"
#include "utils/display.h"
#include "utils/language.h"

namespace devilution {
namespace {
Art dialogArt;
Art progressArt;
Art ArtPopupSm;
Art ArtProgBG;
Art ProgFil;
std::vector<std::unique_ptr<UiItemBase>> vecProgress;
bool endMenu;

void DialogActionCancel()
{
	endMenu = true;
}

void ProgressLoad(const char *msg)
{
	LoadBackgroundArt("ui_art\\black.pcx");
	LoadArt("ui_art\\spopup.pcx", &ArtPopupSm);
	LoadArt("ui_art\\prog_bg.pcx", &ArtProgBG);
	LoadArt("ui_art\\prog_fil.pcx", &ProgFil);
	LoadSmlButtonArt();

	SDL_Rect rect3 = { (Sint16)(PANEL_LEFT + 265), (Sint16)(UI_OFFSET_Y + 267), SML_BUTTON_WIDTH, SML_BUTTON_HEIGHT };
	vecProgress.push_back(std::make_unique<UiButton>(&SmlButton, _("Cancel"), &DialogActionCancel, rect3));
}

void ProgressFree()
{
	ArtBackground.Unload();
	ArtPopupSm.Unload();
	ArtProgBG.Unload();
	ProgFil.Unload();
	UnloadSmlButtonArt();
}

void ProgressRender(BYTE progress)
{
	SDL_FillRect(DiabloUiSurface(), nullptr, 0x000000);
	DrawArt({ 0, 0 }, &ArtBackground);

	Point position = { GetCenterOffset(280), GetCenterOffset(144, gnScreenHeight) };

	DrawArt(position, &ArtPopupSm, 0, 280, 140);
	DrawArt({ GetCenterOffset(227), position.y + 52 }, &ArtProgBG, 0, 227);
	if (progress != 0) {
		DrawArt({ GetCenterOffset(227), position.y + 52 }, &ProgFil, 0, 227 * progress / 100);
	}
	DrawArt({ GetCenterOffset(110), position.y + 99 }, &SmlButton, 2, 110);
}

} // namespace

bool UiProgressDialog(const char *msg, int (*fnfunc)())
{
	ProgressLoad(msg);
	SetFadeLevel(256);

	endMenu = false;
	int progress = 0;

	SDL_Event event;
	while (!endMenu && progress < 100) {
		progress = fnfunc();
		ProgressRender(progress);
		UiRenderItems(vecProgress);
		DrawMouse();
		RenderPresent();

		while (PollEvent(&event) != 0) {
			switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				UiItemMouseEvents(&event, vecProgress);
				break;
#ifndef USE_SDL1
			case SDLK_KP_ENTER:
#endif
			case SDLK_ESCAPE:
			case SDLK_RETURN:
			case SDLK_SPACE:
				endMenu = true;
				break;
			default:
				switch (GetMenuAction(event)) {
				case MenuAction_BACK:
				case MenuAction_SELECT:
					endMenu = true;
					break;
				default:
					break;
				}
				break;
			}
			UiHandleEvents(&event);
		}
	}
	ProgressFree();

	return progress == 100;
}

} // namespace devilution
