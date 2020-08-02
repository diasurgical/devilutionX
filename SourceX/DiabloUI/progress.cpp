#include "all.h"
#include "display.h"

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
SDL_Surface *cancleSurface;
int textWidth;
bool endMenu;

void DialogActionCancel()
{
	endMenu = true;
}

// TODO use PROGRESS_DIALOG for rendering the progressbar or delete it
UiItem PROGRESS_DIALOG[] = {
	UiImage(&dialogArt, { PANEL_LEFT + 180, 168, 280, 144 }),
	UiText(dialogText, { PANEL_LEFT + 180, 177, 280, 43 }, UIS_CENTER),
	UiImage(&progressArt, { PANEL_LEFT + 205, 220, 228, 38 }),
	MakeSmlButton("Cancel", &DialogActionCancel, 330, 265),
};

void progress_Load(char *msg)
{
	LoadBackgroundArt("ui_art\\black.pcx");
	LoadArt("ui_art\\spopup.pcx", &ArtPopupSm);
	LoadArt("ui_art\\prog_bg.pcx", &ArtProgBG);
	LoadArt("ui_art\\prog_fil.pcx", &ProgFil);
	LoadSmlButtonArt();
	LoadTtfFont();

	if (font != NULL) {
		SDL_Color color = { 243, 243, 243, 0 };

		msgSurface = TTF_RenderUTF8_Solid(font, msg, color);
		cancleSurface = TTF_RenderUTF8_Solid(font, "Cancel", color);
		TTF_SizeUTF8(font, "Cancel", &textWidth, NULL);
	}
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
	SDL_FreeSurface(cancleSurface);
	cancleSurface = NULL;
	UnloadTtfFont();
}

void progress_Render(BYTE progress)
{
	SDL_FillRect(GetOutputSurface(), NULL, 0x000000);
	DrawArt(0, 0, &ArtBackground);

	int x = GetCenterOffset(280);
	int y = GetCenterOffset(144, SCREEN_HEIGHT);

	DrawArt(x, y, &ArtPopupSm);
	DrawArt(GetCenterOffset(227), y + 52, &ArtProgBG, 0, 227);
	if (progress) {
		DrawArt(GetCenterOffset(227), y + 52, &ProgFil, 0, 227 * progress / 100);
	}
	DrawArt(GetCenterOffset(110), y + 99, &SmlButton, 2, 110);

	if (msgSurface) {
		SDL_Rect dsc_rect = {
			static_cast<Sint16>(x + 50),
			static_cast<Sint16>(y + 8),
			msgSurface->w,
			msgSurface->h
		};
		Blit(msgSurface, NULL, &dsc_rect);
		dsc_rect.x = GetCenterOffset(textWidth) - 1;
		dsc_rect.y = y + 99 + 4;
		Blit(cancleSurface, NULL, &dsc_rect);
	}
}

BOOL UiProgressDialog(char *msg, int enable, int (*fnfunc)(), int rate)
{
	progress_Load(msg);
	SetFadeLevel(256);

	endMenu = false;
	int progress = 0;

	SDL_Event event;
	while (!endMenu && progress < 100) {
		progress = fnfunc();
		progress_Render(progress);
		DrawMouse();
		RenderPresent();

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
				case SDLK_RETURN:
				case SDLK_KP_ENTER:
				case SDLK_SPACE:
					break;
				default:
					continue;
				}
				endMenu = true;
				break;
			case SDL_MOUSEBUTTONDOWN:
				endMenu = true;
				break;
			}
			UiHandleEvents(&event);
		}
	}
	progress_Free();

	return progress == 100;
}

} // namespace dvl
