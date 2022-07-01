#include <SDL.h>

#include "DiabloUI/art_draw.h"
#include "DiabloUI/button.h"
#include "DiabloUI/diabloui.h"
#include "control.h"
#include "controls/input.h"
#include "controls/menu_controls.h"
#include "engine/dx.h"
#include "engine/load_pcx.hpp"
#include "engine/palette.h"
#include "engine/pcx_sprite.hpp"
#include "engine/render/pcx_render.hpp"
#include "hwcursor.hpp"
#include "utils/display.h"
#include "utils/language.h"

namespace devilution {
namespace {
std::optional<OwnedPcxSprite> ArtPopupSm;
std::optional<OwnedPcxSprite> ArtProgBG;
std::optional<OwnedPcxSprite> ProgFil;
std::vector<std::unique_ptr<UiItemBase>> vecProgress;
bool endMenu;

void DialogActionCancel()
{
	endMenu = true;
}

void ProgressLoadBackground()
{
	UiLoadBlackBackground();
	ArtPopupSm = LoadPcxAsset("ui_art\\spopup.pcx");
	ArtProgBG = LoadPcxAsset("ui_art\\prog_bg.pcx");
}

void ProgressLoadForeground()
{
	LoadDialogButtonGraphics();
	ProgFil = LoadPcxAsset("ui_art\\prog_fil.pcx");

	const Point uiPosition = GetUIRectangle().position;
	SDL_Rect rect3 = { (Sint16)(uiPosition.x + 265), (Sint16)(uiPosition.y + 267), DialogButtonWidth, DialogButtonHeight };
	vecProgress.push_back(std::make_unique<UiButton>(_("Cancel"), &DialogActionCancel, rect3));
}

void ProgressFreeBackground()
{
	ArtBackground = std::nullopt;
	ArtPopupSm = std::nullopt;
	ArtProgBG = std::nullopt;
}

void ProgressFreeForeground()
{
	vecProgress.clear();
	ProgFil = std::nullopt;
	FreeDialogButtonGraphics();
}

Point GetPosition()
{
	return { GetCenterOffset(280), GetCenterOffset(144, gnScreenHeight) };
}

void ProgressRenderBackground()
{
	SDL_FillRect(DiabloUiSurface(), nullptr, 0x000000);

	const Surface &out = Surface(DiabloUiSurface());
	RenderPcxSprite(out, PcxSpriteSheet { *ArtBackground }.sprite(0), { 0, 0 });

	const Point position = GetPosition();
	RenderPcxSprite(out, PcxSprite { *ArtPopupSm }, position);
	RenderPcxSprite(out, PcxSprite { *ArtProgBG }, { GetCenterOffset(227), position.y + 52 });
}

void ProgressRenderForeground(int progress)
{
	const Surface &out = Surface(DiabloUiSurface());
	const Point position = GetPosition();
	if (progress != 0) {
		const int x = GetCenterOffset(227);
		const int w = 227 * progress / 100;
		RenderPcxSprite(out.subregion(x, 0, w, out.h()), PcxSprite { *ProgFil }, { 0, position.y + 52 });
	}
	// Not rendering an actual button, only the top 2 rows of its graphics.
	RenderPcxSprite(
	    out.subregion(GetCenterOffset(110), position.y + 99, DialogButtonWidth, 2),
	    ButtonSprite(/*pressed=*/false), { 0, 0 });
}

} // namespace

bool UiProgressDialog(int (*fnfunc)())
{
	SetFadeLevel(256);

	// Blit the background once and then free it.
	ProgressLoadBackground();
	ProgressRenderBackground();
	if (RenderDirectlyToOutputSurface && IsDoubleBuffered()) {
		// Blit twice for triple buffering.
		for (unsigned i = 0; i < 2; ++i) {
			UiFadeIn();
			ProgressRenderBackground();
		}
	}
	ProgressFreeBackground();

	ProgressLoadForeground();

	endMenu = false;
	int progress = 0;

	SDL_Event event;
	while (!endMenu && progress < 100) {
		progress = fnfunc();
		ProgressRenderForeground(progress);
		UiRenderItems(vecProgress);
		DrawMouse();
		UiFadeIn();

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
	ProgressFreeForeground();

	return progress == 100;
}

} // namespace devilution
