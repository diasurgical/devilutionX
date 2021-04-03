#include "all.h"
#include "display.h"

#include "controls/menu_controls.h"
#include "DiabloUI/button.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/art_draw.h"
#include "DiabloUI/fonts.h"

namespace dvl {

Art dialogArt;
char dialogText[256];
Art progressArt;
Art ArtPopupSm;
Art ArtProgBG;
Art ProgFil;
SDL_Surface *msgSurface;
SDL_Surface *msgShadow;
std::vector<UiItemBase *> vecProgress;
bool endMenu;

void DialogActionCancel()
{
	endMenu = true;
}

void progress_Load(const char *msg)
{
	LoadBackgroundArt("ui_art\\black.pcx");
	LoadArt("ui_art\\spopup.pcx", &ArtPopupSm);
	LoadArt("ui_art\\prog_bg.pcx", &ArtProgBG);
	LoadArt("ui_art\\prog_fil.pcx", &ProgFil);
	LoadSmlButtonArt();
	LoadTtfFont();

	if (font != NULL) {
		SDL_Color color = { 243, 243, 243, 0 };
		SDL_Color black = { 0, 0, 0, 0 };

		msgSurface = TTF_RenderUTF8_Solid(font, msg, color);
		msgShadow = TTF_RenderUTF8_Solid(font, msg, black);
	}
	SDL_Rect rect3 = { (Sint16)(PANEL_LEFT + 265), (Sint16)(UI_OFFSET_Y + 267), SML_BUTTON_WIDTH, SML_BUTTON_HEIGHT };
	vecProgress.push_back(new UiButton(&SmlButton, "Cancel", &DialogActionCancel, rect3, 0));
}

void progress_Free()
{
	ArtBackground.Unload();
	ArtPopupSm.Unload();
	ArtProgBG.Unload();
	ProgFil.Unload();
	UnloadSmlButtonArt();
	SDL_FreeSurface(msgSurface);
	msgSurface = NULL;
	SDL_FreeSurface(msgShadow);
	msgShadow = NULL;
	UnloadTtfFont();
}

void progress_Render(BYTE progress)
{
	SDL_FillRect(GetOutputSurface(), NULL, 0x000000);
	DrawArt(0, 0, &ArtBackground);

	Sint16 x = GetCenterOffset(280);
	Sint16 y = GetCenterOffset(144, gnScreenHeight);

	DrawArt(x, y, &ArtPopupSm, 0, 280, 140);
	DrawArt(GetCenterOffset(227), y + 52, &ArtProgBG, 0, 227);
	if (progress) {
		DrawArt(GetCenterOffset(227), y + 52, &ProgFil, 0, 227 * progress / 100);
	}
	DrawArt(GetCenterOffset(110), y + 99, &SmlButton, 2, 110);

	if (msgSurface) {
		SDL_Rect dsc_rect = {
			static_cast<Sint16>(x + 50 + 1),
			static_cast<Sint16>(y + 8 + 1),
			static_cast<Uint16>(msgSurface->w),
			static_cast<Uint16>(msgSurface->h)
		};
		Blit(msgShadow, NULL, &dsc_rect);
		dsc_rect.x -= 1;
		dsc_rect.y -= 1;
		Blit(msgSurface, NULL, &dsc_rect);
	}
}

BOOL UiProgressDialog(const char *msg, int enable, int (*fnfunc)(), int rate)
{
	progress_Load(msg);
	SetFadeLevel(256);

	endMenu = false;
	int progress = 0;

	SDL_Event event;
	while (!endMenu && progress < 100) {
		progress = fnfunc();
		progress_Render(progress);
		UiRenderItems(vecProgress);
		DrawMouse();
		RenderPresent();

		while (SDL_PollEvent(&event)) {
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
	progress_Free();

	return progress == 100;
}

} // namespace dvl
