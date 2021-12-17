#include "DiabloUI/dialogs.h"

#include <string>
#include <utility>

#include "DiabloUI/button.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/errorart.h"
#include "control.h"
#include "controls/menu_controls.h"
#include "dx.h"
#include "hwcursor.hpp"
#include "main_loop.hpp"
#include "palette.h"
#include "utils/display.h"
#include "utils/language.h"
#include "utils/log.hpp"

namespace devilution {

namespace {

bool InDialog = false;

void DialogActionOK()
{
	NextMainLoopHandler();
}

std::vector<std::unique_ptr<UiItemBase>> vecNULL;

// clang-format off
#define BLANKCOLOR { 0, 0xFF, 0, 0 }
// clang-format on

void LoadFallbackPalette()
{
	// clang-format off
	static const SDL_Color FallbackPalette[256] = {
		{ 0x00, 0x00, 0x00, 0 },
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR,
		{ 0xff, 0xfd, 0x9f, 0 },
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		{ 0xe8, 0xca, 0xca, 0 },
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		{ 0x05, 0x07, 0x0c, 0 },
		{ 0xff, 0xe3, 0xa4, 0 },
		{ 0xee, 0xd1, 0x8c, 0 },
		{ 0xdd, 0xc4, 0x7e, 0 },
		{ 0xcc, 0xb7, 0x75, 0 },
		{ 0xbc, 0xa8, 0x6c, 0 },
		{ 0xab, 0x9a, 0x63, 0 },
		{ 0x98, 0x8b, 0x5d, 0 },
		{ 0x87, 0x7e, 0x54, 0 },
		{ 0x78, 0x6f, 0x49, 0 },
		{ 0x69, 0x60, 0x3f, 0 },
		{ 0x5b, 0x51, 0x34, 0 },
		{ 0x48, 0x40, 0x27, 0 },
		{ 0x39, 0x31, 0x1d, 0 },
		{ 0x31, 0x28, 0x16, 0 },
		{ 0x1a, 0x14, 0x08, 0 },
		{ 0x14, 0x0b, 0x00, 0 },
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR,
		{ 0xff, 0xbd, 0xbd, 0 },
		{ 0xf4, 0x96, 0x96, 0 },
		{ 0xe8, 0x7d, 0x7d, 0 },
		{ 0xe0, 0x6c, 0x6c, 0 },
		{ 0xd8, 0x5b, 0x5b, 0 },
		{ 0xcf, 0x49, 0x49, 0 },
		{ 0xc7, 0x38, 0x38, 0 },
		{ 0xbf, 0x27, 0x27, 0 },
		{ 0xa9, 0x22, 0x22, 0 },
		{ 0x93, 0x1e, 0x1e, 0 },
		{ 0x7c, 0x19, 0x19, 0 },
		{ 0x66, 0x15, 0x15, 0 },
		{ 0x4f, 0x11, 0x11, 0 },
		{ 0x39, 0x0d, 0x0d, 0 },
		{ 0x23, 0x09, 0x09, 0 },
		{ 0x0c, 0x05, 0x05, 0 },
		{ 0xf3, 0xf3, 0xf3, 0 },
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR, BLANKCOLOR,
		{ 0xff, 0xff, 0x00, 0 },
		BLANKCOLOR, BLANKCOLOR, BLANKCOLOR,
		BLANKCOLOR,
	};
	// clang-format on
	ApplyGamma(logical_palette, FallbackPalette, 256);
	BlackPalette();
}

class Dialog : public MainLoopHandler {
public:
	// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
	Dialog(string_view caption, string_view text, bool error, const std::vector<std::unique_ptr<UiItemBase>> &renderBehind)
	    : caption_(caption)
	    , renderBehind_(renderBehind)
	{
		SDL_SetClipRect(DiabloUiSurface(), nullptr);
		if (renderBehind_.empty()) {
			ArtBackground.Unload();
			LoadBackgroundArt("ui_art\\black.pcx");
			if (ArtBackground.surface == nullptr) {
				LoadFallbackPalette();
				if (SDL_ShowCursor(SDL_ENABLE) <= -1)
					Log("{}", SDL_GetError());
			}
		}

		if (caption_.empty()) {
			LoadMaskedArt(error ? "ui_art\\srpopup.pcx" : "ui_art\\spopup.pcx", &dialogArt_);
		} else if (error) {
			LoadArt(&dialogArt_, PopupData, 385, 280);
		} else {
			LoadMaskedArt("ui_art\\lpopup.pcx", &dialogArt_);
		}
		LoadSmlButtonArt();

		const int textWidth = dialogArt_.w() - 40;

		wrappedText_ = WordWrapString(text, textWidth, FontSizeDialog);

		if (caption_.empty()) {
			SDL_Rect rect1 = MakeSdlRect(PANEL_LEFT + 180, UI_OFFSET_Y + 168, dialogArt_.w(), dialogArt_.h());
			items_.push_back(std::make_unique<UiImage>(&dialogArt_, rect1));

			SDL_Rect rect2 = MakeSdlRect(PANEL_LEFT + 200, UI_OFFSET_Y + 211, textWidth, 80);
			items_.push_back(std::make_unique<UiText>(wrappedText_.c_str(), rect2, UiFlags::AlignCenter | UiFlags::ColorDialogWhite));

			SDL_Rect rect3 = MakeSdlRect(PANEL_LEFT + 265, UI_OFFSET_Y + 265, SML_BUTTON_WIDTH, SML_BUTTON_HEIGHT);
			items_.push_back(std::make_unique<UiButton>(&SmlButton, _("OK"), &DialogActionOK, rect3));
		} else {
			SDL_Rect rect1 = MakeSdlRect(PANEL_LEFT + 127, UI_OFFSET_Y + 100, dialogArt_.w(), dialogArt_.h());
			items_.push_back(std::make_unique<UiImage>(&dialogArt_, rect1));

			SDL_Rect rect2 = MakeSdlRect(PANEL_LEFT + 147, UI_OFFSET_Y + 110, textWidth, 20);
			items_.push_back(std::make_unique<UiText>(caption_.c_str(), rect2, UiFlags::AlignCenter | UiFlags::ColorDialogYellow));

			SDL_Rect rect3 = MakeSdlRect(PANEL_LEFT + 147, UI_OFFSET_Y + 141, textWidth, 190);
			items_.push_back(std::make_unique<UiText>(wrappedText_.c_str(), rect3, UiFlags::AlignCenter | UiFlags::ColorDialogWhite));

			SDL_Rect rect4 = MakeSdlRect(PANEL_LEFT + 264, UI_OFFSET_Y + 335, SML_BUTTON_WIDTH, SML_BUTTON_HEIGHT);
			items_.push_back(std::make_unique<UiButton>(&SmlButton, _("OK"), &DialogActionOK, rect4));
		}

		InDialog = true;
	}

	~Dialog() override
	{
		UnloadSmlButtonArt();
		ArtBackground.Unload();
		InDialog = false;
	}

	void HandleEvent(SDL_Event &event) override
	{
		switch (event.type) {
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			UiItemMouseEvents(&event, items_);
			break;
		default:
			switch (GetMenuAction(event)) {
			case MenuAction_BACK:
			case MenuAction_SELECT:
				NextMainLoopHandler();
				break;
			default:
				break;
			}
			break;
		}
		UiHandleEvents(&event);
	}

	void Render() override
	{
		if (renderBehind_.empty()) {
			SDL_FillRect(DiabloUiSurface(), nullptr, 0x000000);
		} else {
			UiRenderItems(renderBehind_);
		}
		UiRenderItems(items_);
		if (ArtBackground.surface != nullptr) {
			DrawMouse();
		}
		UiFadeIn();
	}

private:
	std::string caption_;
	std::string wrappedText_;
	Art dialogArt_;
	std::vector<std::unique_ptr<UiItemBase>> items_;
	const std::vector<std::unique_ptr<UiItemBase>> &renderBehind_;
};

void UiOkDialog(string_view caption, string_view text, bool error, const std::vector<std::unique_ptr<UiItemBase>> &renderBehind)
{
	if (!caption.empty()) {
		LogError("{}\n{}", caption, text);
	} else {
		LogError("{}", text);
	}

	if (!gbActive || InDialog) {
		if (!gbQuietMode) {
			if (SDL_ShowCursor(SDL_ENABLE) <= -1)
				Log("{}", SDL_GetError());
			const std::string caption_str = std::string(caption);
			const std::string text_str = std::string(text);
			if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, caption_str.c_str(), text_str.c_str(), nullptr) <= -1) {
				Log("{}", SDL_GetError());
			}
		}
		NextMainLoopHandler();
	}

	if (IsHardwareCursor()) {
		if (SDL_ShowCursor(SDL_ENABLE) <= -1)
			Log("{}", SDL_GetError());
	}

	InDialog = true;
	SetMainLoopHandler(std::make_unique<Dialog>(caption, text, error, renderBehind));
}

} // namespace

void UiErrorOkDialog(string_view caption, string_view text, const std::vector<std::unique_ptr<UiItemBase>> &renderBehind)
{
	UiOkDialog(caption, text, /*error=*/true, renderBehind);
}

void UiErrorOkDialog(string_view caption, string_view text, bool error)
{
	UiOkDialog(caption, text, error, vecNULL);
}

void UiErrorOkDialog(string_view text, const std::vector<std::unique_ptr<UiItemBase>> &renderBehind)
{
	UiErrorOkDialog("", text, renderBehind);
}

} // namespace devilution
