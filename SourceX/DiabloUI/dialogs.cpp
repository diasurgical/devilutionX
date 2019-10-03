#include "DiabloUI/dialogs.h"

#include "devilution.h"
#include "dx.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/button.h"
#include "DiabloUI/fonts.h"

namespace dvl {

extern HANDLE diabdat_mpq;
extern SDL_Surface *pal_surface;

namespace {

Art dialogArt;
Art progressArt;
char dialogText[256];
char dialogCaption[1024];
bool fontWasLoaded;
bool textInputWasActive;

UiItem *dialogItems;
std::size_t dialogItemsSize;

enum class State {
	DEFAULT = 0,
	OK,
	CANCEL,
};

State state;

void DialogActionOK()
{
	state = State::OK;
}

void DialogActionCancel()
{
	state = State::CANCEL;
}

constexpr auto DIALOG_ART_S = UiImage(&dialogArt, { 180, 168, 280, 144 });
constexpr auto DIALOG_ART_L = UiImage(&dialogArt, { 127, 100, 385, 280 });

UiItem OKCANCEL_DIALOG[] = {
	DIALOG_ART_S,
	UiText(dialogText, { 200, 211, 240, 80 }, UIS_CENTER),
	MakeSmlButton("OK", &DialogActionOK, 200, 265),
	MakeSmlButton("Cancel", &DialogActionCancel, 330, 265),
};

UiItem OK_DIALOG[] = {
	DIALOG_ART_S,
	UiText(dialogText, { 200, 211, 240, 80 }, UIS_CENTER),
	MakeSmlButton("OK", &DialogActionOK, 265, 265),
};

UiItem OK_DIALOG_WITH_CAPTION[] = {
	DIALOG_ART_L,
	UiText(dialogText, SDL_Color{ 255, 255, 0, 0 }, { 147, 110, 345, 20 }, UIS_CENTER),
	UiText(dialogCaption, { 147, 141, 345, 190 }, UIS_CENTER),
	MakeSmlButton("OK", &DialogActionOK, 264, 335),
};

UiItem PROGRESS_DIALOG[] = {
	DIALOG_ART_S,
	UiText(dialogText, { 180, 177, 280, 43 }, UIS_CENTER),
	UiImage(&progressArt, { 205, 220, 228, 38 }),
	MakeSmlButton("Cancel", &DialogActionCancel, 330, 265),
};

UiListItem SELOK_DIALOG_ITEMS[] = {
	{ "OK", 0 }
};
UiItem SELOK_DIALOG[] = {
	UiText(dialogText, { 140, 210, 400, 168 }, UIS_CENTER),
	UiList(SELOK_DIALOG_ITEMS, 230, 390, 180, 35, UIS_CENTER),
};

UiItem SPAWNERR_DIALOG[] = {
	UiText("The Rogue and Sorcerer are only available in the full retail version of Diablo. For ordering information call (800) 953-SNOW.", { 140, 199, 400, 177 }),
	UiArtTextButton("OK", &DialogActionOK, { 230, 407, 180, 43 }),
};

const std::uint_fast8_t DEFAULT_BG_DIALOG_IDX = 4;
const std::uint_fast8_t DEFAULT_BG_BUTTON_IDX = 5;

void LoadFallbackPalette()
{
	PALETTEENTRY fallback_palette[256] = { 0 };
	fallback_palette[1] = { 255, 255, 255, 0 };
	fallback_palette[2] = { 255, 255, 0, 0 };
	fallback_palette[3] = { 243, 243, 243, 0 };
	fallback_palette[DEFAULT_BG_DIALOG_IDX] = { 100, 20, 20, 0 };
	fallback_palette[DEFAULT_BG_BUTTON_IDX] = { 130, 110, 80, 0 };
	LoadPalInMem(fallback_palette);
	ApplyGamma(logical_palette, orig_palette, 256);
}

void StubArt(Art *art, int w, int h, std::uint_fast8_t color_idx)
{
	art->surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h * art->frames, 8, 0, 0, 0, 0);
	SDL_FillRect(art->surface, nullptr, color_idx);
	art->frame_height = h;
}

void Init(const char *text, const char *caption, bool error)
{
	strcpy(dialogText, text);
	if (caption == nullptr) {
		if (diabdat_mpq != nullptr) {
			LoadMaskedArt(error ? "ui_art\\srpopup.pcx" : "ui_art\\spopup.pcx", &dialogArt);
		} else {
			StubArt(&dialogArt, DIALOG_ART_S.rect.w, DIALOG_ART_S.rect.h, DEFAULT_BG_DIALOG_IDX);
		}
		dialogItems = OK_DIALOG;
		dialogItemsSize = size(OK_DIALOG);
	} else {
		if (diabdat_mpq != nullptr) {
			LoadMaskedArt(error ? "ui_art\\lrpopup.pcx" : "ui_art\\lpopup.pcx", &dialogArt);
		} else {
			StubArt(&dialogArt, DIALOG_ART_L.rect.w, DIALOG_ART_L.rect.h, DEFAULT_BG_DIALOG_IDX);
		}
		strcpy(dialogCaption, caption);
		dialogItems = OK_DIALOG_WITH_CAPTION;
		dialogItemsSize = size(OK_DIALOG_WITH_CAPTION);
	}
	LoadSmlButtonArt();

	if (diabdat_mpq == nullptr) {
		ShowCursor(true);
		LoadFallbackPalette();
		const auto &btn = dialogItems[dialogItemsSize - 1].button;
		StubArt(btn.art, btn.rect.w, btn.rect.h, DEFAULT_BG_BUTTON_IDX);
	}

	fontWasLoaded = font != nullptr;
	if (!fontWasLoaded)
		LoadTtfFont();
	textInputWasActive = SDL_IsTextInputActive();
	SDL_StopTextInput();
}

void Deinit()
{
	dialogArt.Unload();
	UnloadSmlButtonArt();
	if (!fontWasLoaded)
		UnloadTtfFont();
	if (textInputWasActive)
		SDL_StartTextInput();
	for (std::size_t i = 0; i < dialogItemsSize; ++i) {
		dialogItems[i].FreeCache();
	}
}

void DialogLoop(UiItem *items, std::size_t num_items, UiItem *render_behind, std::size_t render_behind_size)
{
	SDL_Event event;
	state = State::DEFAULT;
	if (render_behind_size == 0)
		SDL_FillRect(pal_surface, nullptr, 0);
	do {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
				case SDLK_RETURN:
				case SDLK_KP_ENTER:
				case SDLK_SPACE:
					state = State::OK;
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				UiItemMouseEvents(&event, items, num_items);
				break;
			case SDL_QUIT:
				exit(0);
			}
		}
		UiRenderItems(render_behind, render_behind_size);
		UiRenderItems(items, num_items);
		DrawMouse();
		UiFadeIn();
	} while (state == State::DEFAULT);
}

} // namespace

void UiOkDialog(const char *text, const char *caption, bool error, UiItem *render_behind, std::size_t render_behind_size)
{
	Init(text, caption, error);
	DialogLoop(dialogItems, dialogItemsSize, render_behind, render_behind_size);
	Deinit();
}

void UiErrorOkDialog(const char *text, const char *caption, UiItem *render_behind, std::size_t render_behind_size)
{
	UiOkDialog(text, caption, /*error=*/true, render_behind, render_behind_size);
}

void UiErrorOkDialog(const char *text, UiItem *render_behind, std::size_t render_behind_size)
{
	UiErrorOkDialog(text, nullptr, render_behind, render_behind_size);
}

} // namespace dvl
