#include "DiabloUI/dialogs.h"

#include <utility>

#include "DiabloUI/button.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/errorart.h"
#include "control.h"
#include "controls/input.h"
#include "controls/menu_controls.h"
#include "dx.h"
#include "engine/load_pcx.hpp"
#include "engine/pcx_sprite.hpp"
#include "hwcursor.hpp"
#include "palette.h"
#include "utils/display.h"
#include "utils/language.h"
#include "utils/log.hpp"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

namespace {

Art dialogArt;
std::optional<OwnedPcxSprite> dialogPcx;
std::string wrappedText;

bool dialogEnd;

void DialogActionOK()
{
	dialogEnd = true;
}

std::vector<std::unique_ptr<UiItemBase>> vecNULL;
std::vector<std::unique_ptr<UiItemBase>> vecOkDialog;

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

std::optional<PcxSprite> LoadDialogSprite(bool hasCaption, bool isError)
{
	constexpr uint8_t TransparentColor = 255;
	if (!hasCaption) {
		dialogPcx = LoadPcxAsset(isError ? "ui_art\\srpopup.pcx" : "ui_art\\spopup.pcx", TransparentColor);
		return PcxSprite { *dialogPcx };
	}
	if (isError) {
		LoadArt(&dialogArt, PopupData, 385, 280);
		return std::nullopt;
	}
	dialogPcx = LoadPcxAsset("ui_art\\lpopup.pcx", TransparentColor);
	return PcxSprite { *dialogPcx };
}

void Init(string_view caption, string_view text, bool error, bool renderBehind)
{
	if (!renderBehind) {
		ArtBackground = std::nullopt;
		LoadBackgroundArt("ui_art\\black.pcx");
		if (!ArtBackground) {
			LoadFallbackPalette();
			if (SDL_ShowCursor(SDL_ENABLE) <= -1)
				Log("{}", SDL_GetError());
		}
	}

	std::optional<PcxSprite> dialogSprite = LoadDialogSprite(!caption.empty(), error);
	const int dialogWidth = dialogSprite ? dialogSprite->width() : dialogArt.w();
	const int textWidth = dialogWidth - 40;

	wrappedText = WordWrapString(text, textWidth, FontSizeDialog);

	const Point uiPosition = GetUIRectangle().position;
	if (caption.empty()) {
		if (dialogSprite) {
			SDL_Rect rect1 = MakeSdlRect(uiPosition.x + 180, uiPosition.y + 168, dialogSprite->width(), dialogSprite->height());
			vecOkDialog.push_back(std::make_unique<UiImagePcx>(*dialogSprite, rect1));
		} else {
			SDL_Rect rect1 = MakeSdlRect(uiPosition.x + 180, uiPosition.y + 168, dialogArt.w(), dialogArt.h());
			vecOkDialog.push_back(std::make_unique<UiImage>(&dialogArt, rect1));
		}

		SDL_Rect rect2 = MakeSdlRect(uiPosition.x + 200, uiPosition.y + 211, textWidth, 80);
		vecOkDialog.push_back(std::make_unique<UiText>(wrappedText, rect2, UiFlags::AlignCenter | UiFlags::ColorDialogWhite));

		SDL_Rect rect3 = MakeSdlRect(uiPosition.x + 265, uiPosition.y + 265, SML_BUTTON_WIDTH, SML_BUTTON_HEIGHT);
		vecOkDialog.push_back(std::make_unique<UiButton>(_("OK"), &DialogActionOK, rect3));
	} else {
		if (dialogSprite) {
			SDL_Rect rect1 = MakeSdlRect(uiPosition.x + 127, uiPosition.y + 100, dialogSprite->width(), dialogSprite->height());
			vecOkDialog.push_back(std::make_unique<UiImagePcx>(*dialogSprite, rect1));
		} else {
			SDL_Rect rect1 = MakeSdlRect(uiPosition.x + 127, uiPosition.y + 100, dialogArt.w(), dialogArt.h());
			vecOkDialog.push_back(std::make_unique<UiImage>(&dialogArt, rect1));
		}

		SDL_Rect rect2 = MakeSdlRect(uiPosition.x + 147, uiPosition.y + 110, textWidth, 20);
		vecOkDialog.push_back(std::make_unique<UiText>(caption, rect2, UiFlags::AlignCenter | UiFlags::ColorDialogYellow));

		SDL_Rect rect3 = MakeSdlRect(uiPosition.x + 147, uiPosition.y + 141, textWidth, 190);
		vecOkDialog.push_back(std::make_unique<UiText>(wrappedText, rect3, UiFlags::AlignCenter | UiFlags::ColorDialogWhite));

		SDL_Rect rect4 = MakeSdlRect(uiPosition.x + 264, uiPosition.y + 335, SML_BUTTON_WIDTH, SML_BUTTON_HEIGHT);
		vecOkDialog.push_back(std::make_unique<UiButton>(_("OK"), &DialogActionOK, rect4));
	}
}

void Deinit()
{
	dialogArt.Unload();
	dialogPcx = std::nullopt;
	vecOkDialog.clear();
	ArtBackground = std::nullopt;
}

void DialogLoop(const std::vector<std::unique_ptr<UiItemBase>> &items, const std::vector<std::unique_ptr<UiItemBase>> &renderBehind)
{
	SDL_Event event;
	dialogEnd = false;
	do {
		while (PollEvent(&event) != 0) {
			switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				UiItemMouseEvents(&event, items);
				break;
			default:
				switch (GetMenuAction(event)) {
				case MenuAction_BACK:
				case MenuAction_SELECT:
					dialogEnd = true;
					break;
				default:
					break;
				}
				break;
			}
			UiHandleEvents(&event);
		}

		if (renderBehind.empty()) {
			SDL_FillRect(DiabloUiSurface(), nullptr, 0x000000);
		} else {
			UiRenderItems(renderBehind);
		}
		UiRenderItems(items);
		if (ArtBackground) {
			DrawMouse();
		}
		UiFadeIn();
	} while (!dialogEnd);
}

void UiOkDialog(string_view caption, string_view text, bool error, const std::vector<std::unique_ptr<UiItemBase>> &renderBehind)
{
	static bool inDialog = false;

	if (!caption.empty()) {
		LogError("{}\n{}", caption, text);
	} else {
		LogError("{}", text);
	}

	if (!gbActive || inDialog) {
		if (!gbQuietMode) {
			if (SDL_ShowCursor(SDL_ENABLE) <= -1)
				Log("{}", SDL_GetError());
			std::string captionStr = std::string(caption);
			std::string textStr = std::string(caption);
			if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, captionStr.c_str(), textStr.c_str(), nullptr) <= -1) {
				Log("{}", SDL_GetError());
			}
		}
		return;
	}

	if (IsHardwareCursor()) {
		if (SDL_ShowCursor(SDL_ENABLE) <= -1)
			Log("{}", SDL_GetError());
	}

	inDialog = true;
	SDL_SetClipRect(DiabloUiSurface(), nullptr);
	Init(caption, text, error, !renderBehind.empty());
	DialogLoop(vecOkDialog, renderBehind);
	Deinit();
	inDialog = false;
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
	UiErrorOkDialog({}, text, renderBehind);
}

} // namespace devilution
