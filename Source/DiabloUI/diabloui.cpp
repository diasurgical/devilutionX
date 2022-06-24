#include "DiabloUI/diabloui.h"

#include <algorithm>
#include <string>

#include "DiabloUI/art_draw.h"
#include "DiabloUI/button.h"
#include "DiabloUI/dialogs.h"
#include "DiabloUI/scrollbar.h"
#include "controls/controller.h"
#include "controls/input.h"
#include "controls/menu_controls.h"
#include "controls/plrctrls.h"
#include "discord/discord.h"
#include "engine/cel_sprite.hpp"
#include "engine/dx.h"
#include "engine/load_pcx.hpp"
#include "engine/load_pcx_as_cel.hpp"
#include "engine/palette.h"
#include "engine/pcx_sprite.hpp"
#include "engine/render/cel_render.hpp"
#include "engine/render/pcx_render.hpp"
#include "hwcursor.hpp"
#include "utils/display.h"
#include "utils/language.h"
#include "utils/log.hpp"
#include "utils/sdl_compat.h"
#include "utils/sdl_geometry.h"
#include "utils/sdl_wrap.h"
#include "utils/stubs.h"
#include "utils/utf8.hpp"

#ifdef __SWITCH__
// for virtual keyboard on Switch
#include "platform/switch/keyboard.h"
#endif
#ifdef __vita__
// for virtual keyboard on Vita
#include "platform/vita/keyboard.h"
#endif
#ifdef __3DS__
// for virtual keyboard on 3DS
#include "platform/ctr/keyboard.h"
#endif

namespace devilution {

// These are stored as PCX but we load them as CEL to reduce memory usage.
std::array<std::optional<OwnedCelSpriteWithFrameHeight>, 3> ArtLogos;
std::array<std::optional<OwnedCelSpriteWithFrameHeight>, 3> ArtFocus;

std::optional<OwnedPcxSprite> ArtBackgroundWidescreen;
std::optional<OwnedPcxSpriteSheet> ArtBackground;
Art ArtCursor;
Art ArtHero;

void (*gfnSoundFunction)(const char *file);
bool textInputActive = true;
std::size_t SelectedItem = 0;

namespace {

std::size_t SelectedItemMax;
std::size_t ListViewportSize = 1;
std::size_t listOffset = 0;

void (*gfnListFocus)(int value);
void (*gfnListSelect)(int value);
void (*gfnListEsc)();
void (*gfnFullscreen)();
bool (*gfnListYesNo)();
std::vector<UiItemBase *> gUiItems;
UiList *gUiList = nullptr;
bool UiItemsWraps;
char *UiTextInput;
int UiTextInputLen;
bool allowEmptyTextInput = false;

uint32_t fadeTc;
int fadeValue = 0;

struct ScrollBarState {
	bool upArrowPressed;
	bool downArrowPressed;

	ScrollBarState()
	{
		upArrowPressed = false;
		downArrowPressed = false;
	}
} scrollBarState;

void AdjustListOffset(std::size_t itemIndex)
{
	if (itemIndex >= listOffset + ListViewportSize)
		listOffset = itemIndex - (ListViewportSize - 1);
	if (itemIndex < listOffset)
		listOffset = itemIndex;
}

} // namespace

void UiInitList(void (*fnFocus)(int value), void (*fnSelect)(int value), void (*fnEsc)(), const std::vector<std::unique_ptr<UiItemBase>> &items, bool itemsWraps, void (*fnFullscreen)(), bool (*fnYesNo)(), size_t selectedItem /*= 0*/)
{
	SelectedItem = selectedItem;
	SelectedItemMax = 0;
	ListViewportSize = 0;
	gfnListFocus = fnFocus;
	gfnListSelect = fnSelect;
	gfnListEsc = fnEsc;
	gfnFullscreen = fnFullscreen;
	gfnListYesNo = fnYesNo;
	gUiItems.clear();
	for (const auto &item : items)
		gUiItems.push_back(item.get());
	UiItemsWraps = itemsWraps;
	listOffset = 0;
	if (fnFocus != nullptr)
		fnFocus(selectedItem);

#ifndef __SWITCH__
	SDL_StopTextInput(); // input is enabled by default
#endif
	textInputActive = false;
	UiScrollbar *uiScrollbar = nullptr;
	for (const auto &item : items) {
		if (item->IsType(UiType::Edit)) {
			auto *pItemUIEdit = static_cast<UiEdit *>(item.get());
			SDL_SetTextInputRect(&item->m_rect);
			textInputActive = true;
			allowEmptyTextInput = pItemUIEdit->m_allowEmpty;
#ifdef __SWITCH__
			switch_start_text_input(pItemUIEdit->m_hint, pItemUIEdit->m_value, pItemUIEdit->m_max_length);
#elif defined(__vita__)
			vita_start_text_input(pItemUIEdit->m_hint, pItemUIEdit->m_value, pItemUIEdit->m_max_length);
#elif defined(__3DS__)
			ctr_vkbdInput(pItemUIEdit->m_hint, pItemUIEdit->m_value, pItemUIEdit->m_value, pItemUIEdit->m_max_length);
#else
			SDL_StartTextInput();
#endif
			UiTextInput = pItemUIEdit->m_value;
			UiTextInputLen = pItemUIEdit->m_max_length;
		} else if (item->IsType(UiType::List)) {
			auto *uiList = static_cast<UiList *>(item.get());
			SelectedItemMax = std::max(uiList->m_vecItems.size() - 1, static_cast<size_t>(0));
			ListViewportSize = uiList->viewportSize;
			gUiList = uiList;
			if (selectedItem <= SelectedItemMax && HasAnyOf(uiList->GetItem(selectedItem)->uiFlags, UiFlags::NeedsNextElement))
				AdjustListOffset(selectedItem + 1);
		} else if (item->IsType(UiType::Scrollbar)) {
			uiScrollbar = static_cast<UiScrollbar *>(item.get());
		}
	}

	AdjustListOffset(selectedItem);

	if (uiScrollbar != nullptr) {
		if (ListViewportSize >= static_cast<std::size_t>(SelectedItemMax + 1)) {
			uiScrollbar->Hide();
		} else {
			uiScrollbar->Show();
		}
	}
}

void UiInitList_clear()
{
	SelectedItem = 0;
	SelectedItemMax = 0;
	ListViewportSize = 1;
	gfnListFocus = nullptr;
	gfnListSelect = nullptr;
	gfnListEsc = nullptr;
	gfnFullscreen = nullptr;
	gfnListYesNo = nullptr;
	gUiList = nullptr;
	gUiItems.clear();
	UiItemsWraps = false;
}

void UiPlayMoveSound()
{
	if (gfnSoundFunction != nullptr)
		gfnSoundFunction("sfx\\items\\titlemov.wav");
}

void UiPlaySelectSound()
{
	if (gfnSoundFunction != nullptr)
		gfnSoundFunction("sfx\\items\\titlslct.wav");
}

namespace {

void UiFocus(std::size_t itemIndex, bool checkUp, bool ignoreItemsWraps = false)
{
	if (SelectedItem == itemIndex)
		return;

	AdjustListOffset(itemIndex);

	const auto *pItem = gUiList->GetItem(itemIndex);
	while (HasAnyOf(pItem->uiFlags, UiFlags::ElementHidden | UiFlags::ElementDisabled)) {
		if (checkUp) {
			if (itemIndex > 0)
				itemIndex -= 1;
			else if (UiItemsWraps && !ignoreItemsWraps)
				itemIndex = SelectedItemMax;
			else
				checkUp = false;
		} else {
			if (itemIndex < SelectedItemMax)
				itemIndex += 1;
			else if (UiItemsWraps && !ignoreItemsWraps)
				itemIndex = 0;
			else
				checkUp = true;
		}
		pItem = gUiList->GetItem(itemIndex);
	}

	if (HasAnyOf(pItem->uiFlags, UiFlags::NeedsNextElement))
		AdjustListOffset(itemIndex + 1);
	AdjustListOffset(itemIndex);

	SelectedItem = itemIndex;

	UiPlayMoveSound();

	if (gfnListFocus != nullptr)
		gfnListFocus(itemIndex);
}

void UiFocusUp()
{
	if (SelectedItem > 0)
		UiFocus(SelectedItem - 1, true);
	else if (UiItemsWraps)
		UiFocus(SelectedItemMax, true);
}

void UiFocusDown()
{
	if (SelectedItem < SelectedItemMax)
		UiFocus(SelectedItem + 1, false);
	else if (UiItemsWraps)
		UiFocus(0, false);
}

// UiFocusPageUp/Down mimics the slightly weird behaviour of actual Diablo.

void UiFocusPageUp()
{
	if (listOffset == 0) {
		UiFocus(0, true, true);
	} else {
		const std::size_t relpos = SelectedItem - listOffset;
		std::size_t prevPageStart = SelectedItem - relpos;
		if (prevPageStart >= ListViewportSize)
			prevPageStart -= ListViewportSize;
		else
			prevPageStart = 0;
		AdjustListOffset(prevPageStart);
		UiFocus(listOffset + relpos, true, true);
	}
}

void UiFocusPageDown()
{
	if (listOffset + ListViewportSize > static_cast<std::size_t>(SelectedItemMax)) {
		UiFocus(SelectedItemMax, false, true);
	} else {
		const std::size_t relpos = SelectedItem - listOffset;
		std::size_t nextPageEnd = SelectedItem + (ListViewportSize - relpos - 1);
		if (nextPageEnd + ListViewportSize <= static_cast<std::size_t>(SelectedItemMax))
			nextPageEnd += ListViewportSize;
		else
			nextPageEnd = SelectedItemMax;
		AdjustListOffset(nextPageEnd);
		UiFocus(listOffset + relpos, false, true);
	}
}

void SelheroCatToName(const char *inBuf, char *outBuf, int cnt)
{
	size_t outLen = strlen(outBuf);
	char *dest = outBuf + outLen;
	size_t destCount = cnt - outLen;
	CopyUtf8(dest, inBuf, destCount);
}

bool HandleMenuAction(MenuAction menuAction)
{
	switch (menuAction) {
	case MenuAction_SELECT:
		UiFocusNavigationSelect();
		return true;
	case MenuAction_UP:
		UiFocusUp();
		return true;
	case MenuAction_DOWN:
		UiFocusDown();
		return true;
	case MenuAction_PAGE_UP:
		UiFocusPageUp();
		return true;
	case MenuAction_PAGE_DOWN:
		UiFocusPageDown();
		return true;
	case MenuAction_DELETE:
		UiFocusNavigationYesNo();
		return true;
	case MenuAction_BACK:
		if (gfnListEsc == nullptr)
			return false;
		UiFocusNavigationEsc();
		return true;
	default:
		return false;
	}
}

} // namespace

void UiFocusNavigation(SDL_Event *event)
{
	switch (event->type) {
	case SDL_KEYUP:
	case SDL_MOUSEBUTTONUP:
	case SDL_MOUSEMOTION:
#ifndef USE_SDL1
	case SDL_MOUSEWHEEL:
#endif
	case SDL_JOYBUTTONUP:
	case SDL_JOYAXISMOTION:
	case SDL_JOYBALLMOTION:
	case SDL_JOYHATMOTION:
#ifndef USE_SDL1
	case SDL_FINGERUP:
	case SDL_FINGERMOTION:
	case SDL_CONTROLLERBUTTONUP:
	case SDL_CONTROLLERAXISMOTION:
	case SDL_WINDOWEVENT:
#endif
	case SDL_SYSWMEVENT:
		mainmenu_restart_repintro();
		break;
	}

	if (HandleMenuAction(GetMenuAction(*event)))
		return;

#if SDL_VERSION_ATLEAST(2, 0, 0)
	if (event->type == SDL_MOUSEWHEEL) {
		if (event->wheel.y > 0) {
			UiFocusUp();
		} else if (event->wheel.y < 0) {
			UiFocusDown();
		}
		return;
	}
#else
	if (event->type == SDL_MOUSEBUTTONDOWN) {
		switch (event->button.button) {
		case SDL_BUTTON_WHEELUP:
			UiFocusUp();
			return;
		case SDL_BUTTON_WHEELDOWN:
			UiFocusDown();
			return;
		}
	}
#endif

	if (textInputActive) {
		switch (event->type) {
		case SDL_KEYDOWN: {
			switch (event->key.keysym.sym) {
#ifndef USE_SDL1
			case SDLK_v:
				if ((SDL_GetModState() & KMOD_CTRL) != 0) {
					char *clipboard = SDL_GetClipboardText();
					if (clipboard == nullptr) {
						Log("{}", SDL_GetError());
					} else {
						SelheroCatToName(clipboard, UiTextInput, UiTextInputLen);
					}
				}
				return;
#endif
			case SDLK_BACKSPACE:
			case SDLK_LEFT: {
				UiTextInput[FindLastUtf8Symbols(UiTextInput)] = '\0';
				return;
			}
			default:
				break;
			}
#ifdef USE_SDL1
			if ((event->key.keysym.mod & KMOD_CTRL) == 0) {
				std::string utf8;
				AppendUtf8(event->key.keysym.unicode, utf8);
				SelheroCatToName(utf8.c_str(), UiTextInput, UiTextInputLen);
			}
#endif
			break;
		}
#ifndef USE_SDL1
		case SDL_TEXTINPUT:
			if (textInputActive) {
#ifdef __vita__
				CopyUtf8(UiTextInput, event->text.text, UiTextInputLen);
#else
				SelheroCatToName(event->text.text, UiTextInput, UiTextInputLen);
#endif
			}
			return;
#endif
		default:
			break;
		}
	}

	if (event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP) {
		if (UiItemMouseEvents(event, gUiItems))
			return;
	}
}

void UiHandleEvents(SDL_Event *event)
{
	if (event->type == SDL_MOUSEMOTION) {
#ifdef USE_SDL1
		OutputToLogical(&event->motion.x, &event->motion.y);
#endif
		MousePosition = { event->motion.x, event->motion.y };
		return;
	}

	if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_RETURN) {
		const Uint8 *state = SDLC_GetKeyState();
		if (state[SDLC_KEYSTATE_LALT] != 0 || state[SDLC_KEYSTATE_RALT] != 0) {
			sgOptions.Graphics.fullscreen.SetValue(!IsFullScreen());
			SaveOptions();
			if (gfnFullscreen != nullptr)
				gfnFullscreen();
			return;
		}
	}

	if (event->type == SDL_QUIT)
		diablo_quit(0);

#ifndef USE_SDL1
	HandleControllerAddedOrRemovedEvent(*event);

	if (event->type == SDL_WINDOWEVENT) {
		if (event->window.event == SDL_WINDOWEVENT_SHOWN) {
			gbActive = true;
		} else if (event->window.event == SDL_WINDOWEVENT_HIDDEN) {
			gbActive = false;
		} else if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
			ReinitializeHardwareCursor();
		} else if (event->window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
			music_mute();
		} else if (event->window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
			diablo_focus_unpause();
		}
	}
#else
	if (event->type == SDL_ACTIVEEVENT && (event->active.state & SDL_APPINPUTFOCUS) != 0) {
		if (event->active.gain == 0)
			music_mute();
		else
			diablo_focus_unpause();
	}
#endif
}

void UiFocusNavigationSelect()
{
	UiPlaySelectSound();
	if (textInputActive) {
		if (!allowEmptyTextInput && strlen(UiTextInput) == 0) {
			return;
		}
#ifndef __SWITCH__
		SDL_StopTextInput();
#endif
		UiTextInput = nullptr;
		UiTextInputLen = 0;
	}
	if (gfnListSelect != nullptr)
		gfnListSelect(SelectedItem);
}

void UiFocusNavigationEsc()
{
	UiPlaySelectSound();
	if (textInputActive) {
#ifndef __SWITCH__
		SDL_StopTextInput();
#endif
		UiTextInput = nullptr;
		UiTextInputLen = 0;
	}
	if (gfnListEsc != nullptr)
		gfnListEsc();
}

void UiFocusNavigationYesNo()
{
	if (gfnListYesNo == nullptr)
		return;

	if (gfnListYesNo())
		UiPlaySelectSound();
}

namespace {

bool IsInsideRect(const SDL_Event &event, const SDL_Rect &rect)
{
	const SDL_Point point = { event.button.x, event.button.y };
	return SDL_PointInRect(&point, &rect) == SDL_TRUE;
}

void LoadHeros()
{
	LoadArt("ui_art\\heros.pcx", &ArtHero);

	const int portraitHeight = 76;
	int portraitOrder[enum_size<HeroClass>::value + 1] = { 0, 1, 2, 2, 1, 0, 3 };
	if (ArtHero.h() >= portraitHeight * 6) {
		portraitOrder[static_cast<std::size_t>(HeroClass::Monk)] = 3;
		portraitOrder[static_cast<std::size_t>(HeroClass::Bard)] = 4;
		portraitOrder[enum_size<HeroClass>::value] = 5;
	}
	if (ArtHero.h() >= portraitHeight * 7) {
		portraitOrder[static_cast<std::size_t>(HeroClass::Barbarian)] = 6;
	}

	SDLSurfaceUniquePtr heros = SDLWrap::CreateRGBSurfaceWithFormat(0, ArtHero.w(), portraitHeight * (static_cast<int>(enum_size<HeroClass>::value) + 1), 8, SDL_PIXELFORMAT_INDEX8);

	for (int i = 0; i <= static_cast<int>(enum_size<HeroClass>::value); i++) {
		int offset = portraitOrder[i] * portraitHeight;
		if (offset + portraitHeight > ArtHero.h()) {
			offset = 0;
		}
		SDL_Rect srcRect = MakeSdlRect(0, offset, ArtHero.w(), portraitHeight);
		SDL_Rect dstRect = MakeSdlRect(0, i * portraitHeight, ArtHero.w(), portraitHeight);
		SDL_BlitSurface(ArtHero.surface.get(), &srcRect, heros.get(), &dstRect);
	}

	for (int i = 0; i <= static_cast<int>(enum_size<HeroClass>::value); i++) {
		Art portrait;
		char portraitPath[18];
		sprintf(portraitPath, "ui_art\\hero%i.pcx", i);
		LoadArt(portraitPath, &portrait);
		if (portrait.surface == nullptr)
			continue;

		SDL_Rect dstRect = MakeSdlRect(0, i * portraitHeight, portrait.w(), portraitHeight);
		SDL_BlitSurface(portrait.surface.get(), nullptr, heros.get(), &dstRect);
	}

	ArtHero.surface = std::move(heros);
	ArtHero.frame_height = portraitHeight;
	ArtHero.frames = static_cast<int>(enum_size<HeroClass>::value);
}

void LoadUiGFX()
{
	if (gbIsHellfire) {
		ArtLogos[LOGO_MED] = LoadPcxAssetAsCel("ui_art\\hf_logo2.pcx", /*numFrames=*/16);
	} else {
		ArtLogos[LOGO_MED] = LoadPcxAssetAsCel("ui_art\\smlogo.pcx", /*numFrames=*/15, /*generateFrameHeaders=*/false, /*transparentColorIndex=*/250);
	}
	ArtFocus[FOCUS_SMALL] = LoadPcxAssetAsCel("ui_art\\focus16.pcx", /*numFrames=*/8, /*generateFrameHeaders=*/false, /*transparentColorIndex=*/250);
	ArtFocus[FOCUS_MED] = LoadPcxAssetAsCel("ui_art\\focus.pcx", /*numFrames=*/8, /*generateFrameHeaders=*/false, /*transparentColorIndex=*/250);
	ArtFocus[FOCUS_BIG] = LoadPcxAssetAsCel("ui_art\\focus42.pcx", /*numFrames=*/8, /*generateFrameHeaders=*/false, /*transparentColorIndex=*/250);

	LoadMaskedArt("ui_art\\cursor.pcx", &ArtCursor, 1, 0);

#if SDL_VERSION_ATLEAST(2, 0, 0)
	// Set the palette because `ArtCursor` may be used as the hardware cursor.
	if (ArtCursor.surface != nullptr) {
		SDL_SetSurfacePalette(ArtCursor.surface.get(), Palette.get());
	}
#endif

	LoadHeros();
}

} // namespace

void UnloadUiGFX()
{
	ArtHero.Unload();
	ArtCursor.Unload();
	for (auto &art : ArtFocus)
		art = std::nullopt;
	for (auto &art : ArtLogos)
		art = std::nullopt;
}

void UiInitialize()
{
	LoadUiGFX();

	if (ArtCursor.surface != nullptr) {
		if (SDL_ShowCursor(SDL_DISABLE) <= -1) {
			ErrSdl();
		}
	}
}

void UiDestroy()
{
	UnloadFonts();
	UnloadUiGFX();
}

bool UiValidPlayerName(string_view name)
{
	if (name.empty())
		return false;

	// Currently only allow saving PLR_NAME_LEN bytes as a player name, so if the name is too long we'd have to truncate it.
	// That said the input buffer is only 16 bytes long...
	if (name.size() > PLR_NAME_LEN)
		return false;

	if (name.find_first_of(",<>%&\\\"?*#/: ") != name.npos)
		return false;

	// Only basic latin alphabet is supported for multiplayer characters to avoid rendering issues for players who do
	// not have fonts.mpq installed
	if (!std::all_of(name.begin(), name.end(), IsBasicLatin))
		return false;

	string_view bannedNames[] = {
		"gvdl",
		"dvou",
		"tiju",
		"cjudi",
		"bttipmf",
		"ojhhfs",
		"cmj{{bse",
		"benjo",
	};

	std::string buffer { name };
	for (char &character : buffer)
		character++;

	string_view tempName { buffer };
	for (string_view bannedName : bannedNames) {
		if (tempName.find(bannedName) != tempName.npos)
			return false;
	}

	return true;
}

Sint16 GetCenterOffset(Sint16 w, Sint16 bw)
{
	if (bw == 0) {
		bw = gnScreenWidth;
	}

	return (bw - w) / 2;
}

void LoadBackgroundArt(const char *pszFile, int frames)
{
	SDL_Color pPal[256];
	ArtBackground = LoadPcxSpriteSheetAsset(pszFile, static_cast<uint16_t>(frames), /*transparentColor=*/std::nullopt, pPal);
	if (!ArtBackground)
		return;

	LoadPalInMem(pPal);
	ApplyGamma(logical_palette, orig_palette, 256);

	fadeTc = 0;
	fadeValue = 0;

	if (IsHardwareCursorEnabled() && ArtCursor.surface != nullptr && ControlDevice == ControlTypes::KeyboardAndMouse && GetCurrentCursorInfo().type() != CursorType::UserInterface) {
		SetHardwareCursor(CursorInfo::UserInterfaceCursor());
	}

	BlackPalette();

	SDL_FillRect(DiabloUiSurface(), nullptr, 0x000000);
	if (DiabloUiSurface() == PalSurface)
		BltFast(nullptr, nullptr);
	RenderPresent();
}

void UiAddBackground(std::vector<std::unique_ptr<UiItemBase>> *vecDialog)
{
	int uiPositionY = GetUIRectangle().position.y;
	if (ArtBackgroundWidescreen) {
		SDL_Rect rectw = MakeSdlRect(0, uiPositionY, 0, 0);
		vecDialog->push_back(std::make_unique<UiImagePcx>(PcxSprite { *ArtBackgroundWidescreen }, rectw, UiFlags::AlignCenter));
	}

	SDL_Rect rect = MakeSdlRect(0, uiPositionY, 0, 0);
	vecDialog->push_back(std::make_unique<UiImagePcx>(PcxSpriteSheet { *ArtBackground }.sprite(0), rect, UiFlags::AlignCenter));
}

void UiAddLogo(std::vector<std::unique_ptr<UiItemBase>> *vecDialog, int size, int y)
{
	SDL_Rect rect = MakeSdlRect(0, GetUIRectangle().position.y + y, 0, 0);
	vecDialog->push_back(std::make_unique<UiImageCel>(
	    CelSpriteWithFrameHeight { CelSprite { ArtLogos[size]->sprite }, ArtLogos[size]->frameHeight }, rect, UiFlags::AlignCenter, /*bAnimated=*/true));
}

void UiFadeIn()
{
	if (fadeValue < 256) {
		if (fadeValue == 0 && fadeTc == 0)
			fadeTc = SDL_GetTicks();
		const int prevFadeValue = fadeValue;
		fadeValue = static_cast<int>((SDL_GetTicks() - fadeTc) / 2.083); // 32 frames @ 60hz
		if (fadeValue > 256) {
			fadeValue = 256;
			fadeTc = 0;
		}
		if (fadeValue != prevFadeValue)
			SetFadeLevel(fadeValue);
	}

	if (DiabloUiSurface() == PalSurface)
		BltFast(nullptr, nullptr);
	RenderPresent();
}

void DrawCel(CelSpriteWithFrameHeight sprite, Point p)
{
	const Surface &out = Surface(DiabloUiSurface());
	CelDrawTo(out, { p.x, static_cast<int>(p.y + sprite.frameHeight) }, sprite.sprite, 0);
}

void DrawAnimatedCel(CelSpriteWithFrameHeight sprite, Point p)
{
	const Surface &out = Surface(DiabloUiSurface());
	const int frame = GetAnimationFrame(LoadLE32(sprite.sprite.Data()));
	CelDrawTo(out, { p.x, static_cast<int>(p.y + sprite.frameHeight) }, sprite.sprite, frame);
}

void DrawSelector(const SDL_Rect &rect)
{
	int size = FOCUS_SMALL;
	if (rect.h >= 42)
		size = FOCUS_BIG;
	else if (rect.h >= 30)
		size = FOCUS_MED;
	CelSpriteWithFrameHeight sprite { CelSprite { ArtFocus[size]->sprite }, ArtFocus[size]->frameHeight };

	// TODO FOCUS_MED appares higher than the box
	const int y = rect.y + (rect.h - static_cast<int>(sprite.frameHeight)) / 2;

	DrawAnimatedCel(sprite, { rect.x, y });
	DrawAnimatedCel(sprite, { rect.x + rect.w - sprite.sprite.Width(), y });
}

void UiClearScreen()
{
	if (gnScreenWidth > 640) // Background size
		SDL_FillRect(DiabloUiSurface(), nullptr, 0x000000);
}

void UiPollAndRender(std::function<bool(SDL_Event &)> eventHandler)
{
	SDL_Event event;
	while (PollEvent(&event) != 0) {
		if (eventHandler && eventHandler(event))
			continue;
		UiFocusNavigation(&event);
		UiHandleEvents(&event);
	}
	HandleMenuAction(GetMenuHeldUpDownAction());
	UiRenderItems(gUiItems);
	DrawMouse();
	UiFadeIn();

	// Must happen after the very first UiFadeIn, which sets the cursor.
	if (IsHardwareCursor())
		SetHardwareCursorVisible(ControlDevice == ControlTypes::KeyboardAndMouse);

#ifdef __3DS__
	// Keyboard blocks until input is finished
	// so defer until after render and fade-in
	ctr_vkbdFlush();
#endif

	discord_manager::UpdateMenu();
}

namespace {

void Render(const UiText *uiText)
{
	const Surface &out = Surface(DiabloUiSurface());
	DrawString(out, uiText->GetText(), MakeRectangle(uiText->m_rect), uiText->GetFlags() | UiFlags::FontSizeDialog);
}

void Render(const UiArtText *uiArtText)
{
	const Surface &out = Surface(DiabloUiSurface());
	DrawString(out, uiArtText->GetText(), MakeRectangle(uiArtText->m_rect), uiArtText->GetFlags(), uiArtText->GetSpacing(), uiArtText->GetLineHeight());
}

void Render(const UiImage *uiImage)
{
	int x = uiImage->m_rect.x;
	if (uiImage->IsCentered() && uiImage->GetArt() != nullptr) {
		x += GetCenterOffset(uiImage->GetArt()->w(), uiImage->m_rect.w);
	}
	if (uiImage->IsAnimated()) {
		DrawAnimatedArt(uiImage->GetArt(), { x, uiImage->m_rect.y });
	} else {
		DrawArt({ x, uiImage->m_rect.y }, uiImage->GetArt(), uiImage->GetFrame(), uiImage->m_rect.w);
	}
}

void Render(const UiImageCel *uiImage)
{
	const CelSpriteWithFrameHeight &sprite = uiImage->GetSprite();
	int x = uiImage->m_rect.x;
	if (uiImage->IsCentered()) {
		x += GetCenterOffset(sprite.sprite.Width(), uiImage->m_rect.w);
	}
	if (uiImage->IsAnimated()) {
		DrawAnimatedCel(sprite, { x, uiImage->m_rect.y });
	} else {
		DrawCel(sprite, { x, uiImage->m_rect.y });
	}
}

void Render(const UiImagePcx *uiImage)
{
	PcxSprite sprite = uiImage->GetSprite();
	int x = uiImage->m_rect.x;
	if (uiImage->IsCentered()) {
		x += GetCenterOffset(sprite.width(), uiImage->m_rect.w);
	}
	RenderPcxSprite(Surface(DiabloUiSurface()), sprite, { x, uiImage->m_rect.y });
}

void Render(const UiImageAnimatedPcx *uiImage)
{
	PcxSprite sprite = uiImage->GetSprite(GetAnimationFrame(uiImage->NumFrames()));
	int x = uiImage->m_rect.x;
	if (uiImage->IsCentered()) {
		x += GetCenterOffset(sprite.width(), uiImage->m_rect.w);
	}
	RenderPcxSprite(Surface(DiabloUiSurface()), sprite, { x, uiImage->m_rect.y });
}

void Render(const UiArtTextButton *uiButton)
{
	const Surface &out = Surface(DiabloUiSurface());
	DrawString(out, uiButton->GetText(), MakeRectangle(uiButton->m_rect), uiButton->GetFlags());
}

void Render(const UiList *uiList)
{
	const Surface &out = Surface(DiabloUiSurface());

	for (std::size_t i = listOffset; i < uiList->m_vecItems.size() && (i - listOffset) < ListViewportSize; ++i) {
		SDL_Rect rect = uiList->itemRect(i - listOffset);
		const UiListItem *item = uiList->GetItem(i);
		if (i == SelectedItem)
			DrawSelector(rect);

		Rectangle rectangle = MakeRectangle(rect);
		if (item->args.empty())
			DrawString(out, item->m_text, rectangle, uiList->GetFlags() | item->uiFlags, uiList->GetSpacing());
		else
			DrawStringWithColors(out, item->m_text, item->args, rectangle, uiList->GetFlags() | item->uiFlags, uiList->GetSpacing());
	}
}

void Render(const UiScrollbar *uiSb)
{
	const Surface out = Surface(DiabloUiSurface());

	// Bar background (tiled):
	{
		const int bgY = uiSb->m_rect.y + uiSb->m_arrow.frameHeight();
		const int bgH = DownArrowRect(uiSb).y - bgY;
		const Surface backgroundOut = out.subregion(uiSb->m_rect.x, bgY, SCROLLBAR_BG_WIDTH, bgH);
		int y = 0;
		while (y < bgH) {
			RenderPcxSprite(backgroundOut, uiSb->m_bg, { 0, y });
			y += uiSb->m_bg.height();
		}
	}

	// Arrows:
	{
		const SDL_Rect rect = UpArrowRect(uiSb);
		const auto frame = static_cast<uint16_t>(scrollBarState.upArrowPressed ? ScrollBarArrowFrame_UP_ACTIVE : ScrollBarArrowFrame_UP);
		RenderPcxSprite(out.subregion(rect.x, 0, SCROLLBAR_ARROW_WIDTH, out.h()), uiSb->m_arrow.sprite(frame), { 0, rect.y });
	}
	{
		const SDL_Rect rect = DownArrowRect(uiSb);
		const auto frame = static_cast<uint16_t>(scrollBarState.downArrowPressed ? ScrollBarArrowFrame_DOWN_ACTIVE : ScrollBarArrowFrame_DOWN);
		RenderPcxSprite(out.subregion(rect.x, 0, SCROLLBAR_ARROW_WIDTH, out.h()), uiSb->m_arrow.sprite(frame), { 0, rect.y });
	}

	// Thumb:
	if (SelectedItemMax > 0) {
		const SDL_Rect rect = ThumbRect(uiSb, SelectedItem, SelectedItemMax + 1);
		RenderPcxSprite(out, uiSb->m_thumb, { rect.x, rect.y });
	}
}

void Render(const UiEdit *uiEdit)
{
	DrawSelector(uiEdit->m_rect);

	// To simulate padding we inset the region used to draw text in an edit control
	Rectangle rect = MakeRectangle(uiEdit->m_rect).inset({ 43, 1 });

	const Surface &out = Surface(DiabloUiSurface());
	DrawString(out, uiEdit->m_value, rect, uiEdit->GetFlags() | UiFlags::TextCursor);
}

void RenderItem(UiItemBase *item)
{
	if (item->IsHidden())
		return;
	switch (item->GetType()) {
	case UiType::Text:
		Render(static_cast<UiText *>(item));
		break;
	case UiType::ArtText:
		Render(static_cast<UiArtText *>(item));
		break;
	case UiType::Image:
		Render(static_cast<UiImage *>(item));
		break;
	case UiType::ImageCel:
		Render(static_cast<UiImageCel *>(item));
		break;
	case UiType::ImagePcx:
		Render(static_cast<UiImagePcx *>(item));
		break;
	case UiType::ImageAnimatedPcx:
		Render(static_cast<UiImageAnimatedPcx *>(item));
		break;
	case UiType::ArtTextButton:
		Render(static_cast<UiArtTextButton *>(item));
		break;
	case UiType::Button:
		RenderButton(static_cast<UiButton *>(item));
		break;
	case UiType::List:
		Render(static_cast<UiList *>(item));
		break;
	case UiType::Scrollbar:
		Render(static_cast<UiScrollbar *>(item));
		break;
	case UiType::Edit:
		Render(static_cast<UiEdit *>(item));
		break;
	}
}

bool HandleMouseEventArtTextButton(const SDL_Event &event, const UiArtTextButton *uiButton)
{
	if (event.type != SDL_MOUSEBUTTONUP || event.button.button != SDL_BUTTON_LEFT) {
		return false;
	}

	uiButton->Activate();
	return true;
}

#ifdef USE_SDL1
Uint32 dbClickTimer;
#endif

bool HandleMouseEventList(const SDL_Event &event, UiList *uiList)
{
	if (event.button.button != SDL_BUTTON_LEFT)
		return false;

	if (event.type != SDL_MOUSEBUTTONUP && event.type != SDL_MOUSEBUTTONDOWN)
		return false;

	std::size_t index = uiList->indexAt(event.button.y);
	if (event.type == SDL_MOUSEBUTTONDOWN) {
		uiList->Press(index);
		return true;
	}

	if (event.type == SDL_MOUSEBUTTONUP && !uiList->IsPressed(index))
		return false;

	index += listOffset;

	if (gfnListFocus != nullptr && SelectedItem != index) {
		UiFocus(index, true, false);
#ifdef USE_SDL1
		dbClickTimer = SDL_GetTicks();
	} else if (gfnListFocus == NULL || dbClickTimer + 500 >= SDL_GetTicks()) {
#else
	} else if (gfnListFocus == nullptr || event.button.clicks >= 2) {
#endif
		if (HasAnyOf(uiList->GetItem(index)->uiFlags, UiFlags::ElementHidden | UiFlags::ElementDisabled))
			return false;
		SelectedItem = index;
		UiFocusNavigationSelect();
#ifdef USE_SDL1
	} else {
		dbClickTimer = SDL_GetTicks();
#endif
	}

	return true;
}

bool HandleMouseEventScrollBar(const SDL_Event &event, const UiScrollbar *uiSb)
{
	if (event.button.button != SDL_BUTTON_LEFT)
		return false;
	if (event.type == SDL_MOUSEBUTTONUP) {
		if (scrollBarState.upArrowPressed && IsInsideRect(event, UpArrowRect(uiSb))) {
			UiFocusUp();
			return true;
		}
		if (scrollBarState.downArrowPressed && IsInsideRect(event, DownArrowRect(uiSb))) {
			UiFocusDown();
			return true;
		}
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		if (IsInsideRect(event, BarRect(uiSb))) {
			// Scroll up or down based on thumb position.
			const SDL_Rect thumbRect = ThumbRect(uiSb, SelectedItem, SelectedItemMax + 1);
			if (event.button.y < thumbRect.y) {
				UiFocusPageUp();
			} else if (event.button.y > thumbRect.y + thumbRect.h) {
				UiFocusPageDown();
			}
			return true;
		}
		if (IsInsideRect(event, UpArrowRect(uiSb))) {
			scrollBarState.upArrowPressed = true;
			return true;
		}
		if (IsInsideRect(event, DownArrowRect(uiSb))) {
			scrollBarState.downArrowPressed = true;
			return true;
		}
	}
	return false;
}

bool HandleMouseEvent(const SDL_Event &event, UiItemBase *item)
{
	if (item->IsNotInteractive() || !IsInsideRect(event, item->m_rect))
		return false;
	switch (item->GetType()) {
	case UiType::ArtTextButton:
		return HandleMouseEventArtTextButton(event, static_cast<UiArtTextButton *>(item));
	case UiType::Button:
		return HandleMouseEventButton(event, static_cast<UiButton *>(item));
	case UiType::List:
		return HandleMouseEventList(event, static_cast<UiList *>(item));
	case UiType::Scrollbar:
		return HandleMouseEventScrollBar(event, static_cast<UiScrollbar *>(item));
	default:
		return false;
	}
}

} // namespace

void LoadPalInMem(const SDL_Color *pPal)
{
	for (int i = 0; i < 256; i++) {
		orig_palette[i] = pPal[i];
	}
}

void UiRenderItems(const std::vector<UiItemBase *> &items)
{
	for (const auto &item : items)
		RenderItem(item);
}

void UiRenderItems(const std::vector<std::unique_ptr<UiItemBase>> &items)
{
	for (const auto &item : items)
		RenderItem(item.get());
}

bool UiItemMouseEvents(SDL_Event *event, const std::vector<UiItemBase *> &items)
{
	if (items.empty()) {
		return false;
	}

	// In SDL2 mouse events already use logical coordinates.
#ifdef USE_SDL1
	OutputToLogical(&event->button.x, &event->button.y);
#endif

	bool handled = false;
	for (const auto &item : items) {
		if (HandleMouseEvent(*event, item)) {
			handled = true;
			break;
		}
	}

	if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
		scrollBarState.downArrowPressed = scrollBarState.upArrowPressed = false;
		for (const auto &item : items) {
			if (item->IsType(UiType::Button)) {
				HandleGlobalMouseUpButton(static_cast<UiButton *>(item));
			} else if (item->IsType(UiType::List)) {
				static_cast<UiList *>(item)->Release();
			}
		}
	}

	return handled;
}

bool UiItemMouseEvents(SDL_Event *event, const std::vector<std::unique_ptr<UiItemBase>> &items)
{
	if (items.empty()) {
		return false;
	}

	// In SDL2 mouse events already use logical coordinates.
#ifdef USE_SDL1
	OutputToLogical(&event->button.x, &event->button.y);
#endif

	bool handled = false;
	for (const auto &item : items) {
		if (HandleMouseEvent(*event, item.get())) {
			handled = true;
			break;
		}
	}

	if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
		scrollBarState.downArrowPressed = scrollBarState.upArrowPressed = false;
		for (const auto &item : items) {
			if (item->IsType(UiType::Button)) {
				HandleGlobalMouseUpButton(static_cast<UiButton *>(item.get()));
			} else if (item->IsType(UiType::List)) {
				static_cast<UiList *>(item.get())->Release();
			}
		}
	}

	return handled;
}

void DrawMouse()
{
	if (ControlDevice != ControlTypes::KeyboardAndMouse || IsHardwareCursor())
		return;

	DrawArt(MousePosition, &ArtCursor);
}
} // namespace devilution
