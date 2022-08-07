#include "DiabloUI/dialogs.h"

#include <utility>

#include "DiabloUI/button.h"
#include "DiabloUI/diabloui.h"
#include "control.h"
#include "controls/input.h"
#include "controls/menu_controls.h"
#include "engine/clx_sprite.hpp"
#include "engine/dx.h"
#include "engine/load_clx.hpp"
#include "engine/load_pcx.hpp"
#include "engine/palette.h"
#include "hwcursor.hpp"
#include "utils/display.h"
#include "utils/language.h"
#include "utils/log.hpp"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

namespace {

OptionalOwnedClxSpriteList ownedDialogSprite;
std::string wrappedText;

bool dialogEnd;

void DialogActionOK()
{
	dialogEnd = true;
}

std::vector<std::unique_ptr<UiItemBase>> vecNULL;
std::vector<std::unique_ptr<UiItemBase>> vecOkDialog;

OptionalClxSprite LoadDialogSprite(bool hasCaption, bool isError)
{
	constexpr uint8_t TransparentColor = 255;
	if (!hasCaption) {
		ownedDialogSprite = LoadPcx(isError ? "ui_art\\srpopup.pcx" : "ui_art\\spopup.pcx", TransparentColor);
	} else if (isError) {
		ownedDialogSprite = LoadOptionalClx("ui_art\\dvl_lrpopup.clx");
		if (!ownedDialogSprite) {
			ownedDialogSprite = LoadPcx("ui_art\\lrpopup.pcx", TransparentColor);
		}
	} else {
		ownedDialogSprite = LoadPcx("ui_art\\lpopup.pcx", TransparentColor);
	}
	if (!ownedDialogSprite)
		return std::nullopt;
	return (*ownedDialogSprite)[0];
}

bool Init(string_view caption, string_view text, bool error, bool renderBehind)
{
	if (!renderBehind) {
		if (!UiLoadBlackBackground()) {
			if (SDL_ShowCursor(SDL_ENABLE) <= -1)
				LogError("{}", SDL_GetError());
		}
	}
	LoadDialogButtonGraphics();

	OptionalClxSprite dialogSprite = LoadDialogSprite(!caption.empty(), error);
	if (!dialogSprite)
		return false;

	const int dialogWidth = dialogSprite->width();
	const int textWidth = dialogWidth - 40;

	wrappedText = WordWrapString(text, textWidth, FontSizeDialog);

	const Point uiPosition = GetUIRectangle().position;
	if (caption.empty()) {
		SDL_Rect rect1 = MakeSdlRect(uiPosition.x + 180, uiPosition.y + 168, dialogSprite->width(), dialogSprite->height());
		vecOkDialog.push_back(std::make_unique<UiImageClx>(*dialogSprite, rect1));
		SDL_Rect rect2 = MakeSdlRect(uiPosition.x + 200, uiPosition.y + 211, textWidth, 80);
		vecOkDialog.push_back(std::make_unique<UiText>(wrappedText, rect2, UiFlags::AlignCenter | UiFlags::ColorDialogWhite));

		SDL_Rect rect3 = MakeSdlRect(uiPosition.x + 265, uiPosition.y + 265, DialogButtonWidth, DialogButtonHeight);
		vecOkDialog.push_back(std::make_unique<UiButton>(_("OK"), &DialogActionOK, rect3));
	} else {
		SDL_Rect rect1 = MakeSdlRect(uiPosition.x + 127, uiPosition.y + 100, dialogSprite->width(), dialogSprite->height());
		vecOkDialog.push_back(std::make_unique<UiImageClx>(*dialogSprite, rect1));

		SDL_Rect rect2 = MakeSdlRect(uiPosition.x + 147, uiPosition.y + 110, textWidth, 20);
		vecOkDialog.push_back(std::make_unique<UiText>(caption, rect2, UiFlags::AlignCenter | UiFlags::ColorDialogYellow));

		SDL_Rect rect3 = MakeSdlRect(uiPosition.x + 147, uiPosition.y + 141, textWidth, 190);
		vecOkDialog.push_back(std::make_unique<UiText>(wrappedText, rect3, UiFlags::AlignCenter | UiFlags::ColorDialogWhite));

		SDL_Rect rect4 = MakeSdlRect(uiPosition.x + 264, uiPosition.y + 335, DialogButtonWidth, DialogButtonHeight);
		vecOkDialog.push_back(std::make_unique<UiButton>(_("OK"), &DialogActionOK, rect4));
	}
	return true;
}

void Deinit()
{
	ownedDialogSprite = std::nullopt;
	vecOkDialog.clear();
	ArtBackground = std::nullopt;
	FreeDialogButtonGraphics();
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
		if (!HeadlessMode) {
			if (SDL_ShowCursor(SDL_ENABLE) <= -1)
				LogError("{}", SDL_GetError());
			std::string captionStr = std::string(caption);
			std::string textStr = std::string(text);
			if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, captionStr.c_str(), textStr.c_str(), nullptr) <= -1) {
				LogError("{}", SDL_GetError());
			}
		}
		return;
	}

	if (IsHardwareCursor()) {
		if (SDL_ShowCursor(SDL_ENABLE) <= -1)
			LogError("{}", SDL_GetError());
	}

	if (!Init(caption, text, error, !renderBehind.empty())) {
		LogError("{}\n{}", caption, text);
		std::string captionStr = std::string(caption);
		std::string textStr = std::string(text);
		if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, captionStr.c_str(), textStr.c_str(), nullptr) <= -1) {
			LogError("{}", SDL_GetError());
		}
	}

	inDialog = true;
	SDL_SetClipRect(DiabloUiSurface(), nullptr);
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
