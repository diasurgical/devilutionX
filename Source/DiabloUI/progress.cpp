#include "DiabloUI/art_draw.h"
#include "DiabloUI/button.h"
#include "DiabloUI/diabloui.h"
#include "control.h"
#include "controls/input.h"
#include "controls/menu_controls.h"
#include "dx.h"
#include "engine/load_pcx.hpp"
#include "engine/pcx_sprite.hpp"
#include "engine/render/pcx_render.hpp"
#include "hwcursor.hpp"
#include "palette.h"
#include "utils/display.h"
#include "utils/language.h"

namespace devilution {
namespace {
Art dialogArt;
std::optional<OwnedPcxSprite> ArtPopupSm;
std::optional<OwnedPcxSprite> ArtProgBG;
std::optional<OwnedPcxSprite> ProgFil;
std::vector<std::unique_ptr<UiItemBase>> vecProgress;
bool endMenu;

void DialogActionCancel()
{
	endMenu = true;
}

void ProgressLoad()
{
	LoadBackgroundArt("ui_art\\black.pcx");
	ArtPopupSm = LoadPcxAsset("ui_art\\spopup.pcx");
	ArtProgBG = LoadPcxAsset("ui_art\\prog_bg.pcx");
	ProgFil = LoadPcxAsset("ui_art\\prog_fil.pcx");

	const Point uiPosition = GetUIRectangle().position;
	SDL_Rect rect3 = { (Sint16)(uiPosition.x + 265), (Sint16)(uiPosition.y + 267), SML_BUTTON_WIDTH, SML_BUTTON_HEIGHT };
	vecProgress.push_back(std::make_unique<UiButton>(_("Cancel"), &DialogActionCancel, rect3));
}

void ProgressFree()
{
	ArtBackground = std::nullopt;
	ArtPopupSm = std::nullopt;
	ArtProgBG = std::nullopt;
	ProgFil = std::nullopt;
}

void ProgressRender(BYTE progress)
{
	SDL_FillRect(DiabloUiSurface(), nullptr, 0x000000);

	const Surface &out = Surface(DiabloUiSurface());
	RenderPcxSprite(out, PcxSpriteSheet { *ArtBackground }.sprite(0), { 0, 0 });

	Point position = { GetCenterOffset(280), GetCenterOffset(144, gnScreenHeight) };

	RenderPcxSprite(out, PcxSprite { *ArtPopupSm }, position);
	RenderPcxSprite(out, PcxSprite { *ArtProgBG }, { GetCenterOffset(227), position.y + 52 });
	if (progress != 0) {
		const int x = GetCenterOffset(227);
		const int w = 227 * progress / 100;
		RenderPcxSprite(out.subregion(x, 0, w, out.h()), PcxSprite { *ProgFil }, { 0, position.y + 52 });
	}
	// Not rendering an actual button, only the top 2 rows of its graphics.
	RenderPcxSprite(out.subregionY(position.y + 99, 2), ButtonSprite(/*pressed=*/false), { GetCenterOffset(110), 0 });
}

} // namespace

bool UiProgressDialog(int (*fnfunc)())
{
	ProgressLoad();
	SetFadeLevel(256);

	endMenu = false;
	int progress = 0;

	SDL_Event event;
	while (!endMenu && progress < 100) {
		progress = fnfunc();
		ProgressRender(progress);
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
	ProgressFree();

	return progress == 100;
}

} // namespace devilution
