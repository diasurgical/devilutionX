#include "DiabloUI/diabloui.h"

#include <algorithm>
#include <string>
#ifdef USE_SDL1
#include <cassert>
#include <codecvt>
#include <locale>
#endif

#include "DiabloUI/art_draw.h"
#include "DiabloUI/button.h"
#include "DiabloUI/dialogs.h"
#include "DiabloUI/scrollbar.h"
#include "controls/controller.h"
#include "controls/menu_controls.h"
#include "dx.h"
#include "hwcursor.hpp"
#include "palette.h"
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

std::array<Art, 3> ArtLogos;
std::array<Art, 3> ArtFocus;
Art ArtBackgroundWidescreen;
Art ArtBackground;
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

void UiInitList(void (*fnFocus)(int value), void (*fnSelect)(int value), void (*fnEsc)(), const std::vector<std::unique_ptr<UiItemBase>> &items, bool itemsWraps, bool (*fnYesNo)(), size_t selectedItem /*= 0*/)
{
	SelectedItem = selectedItem;
	SelectedItemMax = 0;
	ListViewportSize = 0;
	gfnListFocus = fnFocus;
	gfnListSelect = fnSelect;
	gfnListEsc = fnEsc;
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
		if (item->m_type == UiType::Edit) {
			auto *pItemUIEdit = static_cast<UiEdit *>(item.get());
			SDL_SetTextInputRect(&item->m_rect);
			textInputActive = true;
			allowEmptyTextInput = pItemUIEdit->m_allowEmpty;
#ifdef __SWITCH__
			switch_start_text_input(pItemUIEdit->m_hint, pItemUIEdit->m_value, pItemUIEdit->m_max_length, /*multiline=*/0);
#elif defined(__vita__)
			vita_start_text_input(pItemUIEdit->m_hint, pItemUIEdit->m_value, pItemUIEdit->m_max_length);
#elif defined(__3DS__)
			ctr_vkbdInput(pItemUIEdit->m_hint, pItemUIEdit->m_value, pItemUIEdit->m_value, pItemUIEdit->m_max_length);
#else
			SDL_StartTextInput();
#endif
			UiTextInput = pItemUIEdit->m_value;
			UiTextInputLen = pItemUIEdit->m_max_length;
		} else if (item->m_type == UiType::List) {
			auto *uiList = static_cast<UiList *>(item.get());
			SelectedItemMax = std::max(uiList->m_vecItems.size() - 1, static_cast<size_t>(0));
			ListViewportSize = uiList->viewportSize;
			gUiList = uiList;
		} else if (item->m_type == UiType::Scrollbar) {
			uiScrollbar = static_cast<UiScrollbar *>(item.get());
		}
	}

	AdjustListOffset(selectedItem);

	if (uiScrollbar != nullptr) {
		if (ListViewportSize >= static_cast<std::size_t>(SelectedItemMax + 1)) {
			uiScrollbar->add_flag(UiFlags::ElementHidden);
		} else {
			uiScrollbar->remove_flag(UiFlags::ElementHidden);
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

void UiFocus(std::size_t itemIndex, bool checkUp)
{
	if (SelectedItem == itemIndex)
		return;

	AdjustListOffset(itemIndex);

	auto pItem = gUiList->GetItem(itemIndex);
	if (HasAnyOf(pItem->uiFlags, UiFlags::ElementHidden | UiFlags::ElementDisabled)) {
		if (checkUp) {
			if (itemIndex > 0)
				UiFocus(itemIndex - 1, checkUp);
			else if (UiItemsWraps)
				UiFocus(SelectedItemMax, checkUp);
			else
				UiFocus(itemIndex, false);
		} else {
			if (itemIndex < SelectedItemMax)
				UiFocus(itemIndex + 1, checkUp);
			else if (UiItemsWraps)
				UiFocus(0, checkUp);
			else
				UiFocus(itemIndex, true);
		}
		return;
	}
	if (HasAnyOf(pItem->uiFlags, UiFlags::NeedsNextElement)) {
		AdjustListOffset(itemIndex + 1);
	}

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
		UiFocus(0, true);
	} else {
		const std::size_t relpos = SelectedItem - listOffset;
		std::size_t prevPageStart = SelectedItem - relpos;
		if (prevPageStart >= ListViewportSize)
			prevPageStart -= ListViewportSize;
		else
			prevPageStart = 0;
		UiFocus(prevPageStart, true);
		UiFocus(listOffset + relpos, true);
	}
}

void UiFocusPageDown()
{
	if (listOffset + ListViewportSize > static_cast<std::size_t>(SelectedItemMax)) {
		UiFocus(SelectedItemMax, false);
	} else {
		const std::size_t relpos = SelectedItem - listOffset;
		std::size_t nextPageEnd = SelectedItem + (ListViewportSize - relpos - 1);
		if (nextPageEnd + ListViewportSize <= static_cast<std::size_t>(SelectedItemMax))
			nextPageEnd += ListViewportSize;
		else
			nextPageEnd = SelectedItemMax;
		UiFocus(nextPageEnd, false);
		UiFocus(listOffset + relpos, false);
	}
}

void SelheroCatToName(const char *inBuf, char *outBuf, int cnt)
{
	strncat(outBuf, inBuf, cnt - strlen(outBuf));
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

#ifndef USE_SDL1
	// SDL generates mouse events from touch-based inputs to provide basic
	// touchscreeen support for apps that don't explicitly handle touch events
	sgbTouchActive = false;
	if (IsAnyOf(event->type, SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP) && event->button.which == SDL_TOUCH_MOUSEID)
		sgbTouchActive = true;
	if (event->type == SDL_MOUSEMOTION && event->motion.which == SDL_TOUCH_MOUSEID)
		sgbTouchActive = true;
	if (event->type == SDL_MOUSEWHEEL && event->wheel.which == SDL_TOUCH_MOUSEID)
		sgbTouchActive = true;
	if (IsAnyOf(event->type, SDL_FINGERDOWN, SDL_FINGERUP, SDL_FINGERMOTION))
		sgbTouchActive = true;
#endif

	if (HandleMenuAction(GetMenuAction(*event)))
		return;

#ifndef USE_SDL1
	if (event->type == SDL_MOUSEWHEEL) {
		if (event->wheel.y > 0) {
			UiFocusUp();
		} else if (event->wheel.y < 0) {
			UiFocusDown();
		}
		return;
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
				Uint16 unicode = event->key.keysym.unicode;
				std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
				std::string utf8 = convert.to_bytes(unicode);
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
			dx_reinit();
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
			music_unmute();
		}
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
		LoadMaskedArt("ui_art\\hf_logo2.pcx", &ArtLogos[LOGO_MED], 16);
	} else {
		LoadMaskedArt("ui_art\\smlogo.pcx", &ArtLogos[LOGO_MED], 15);
	}
	LoadMaskedArt("ui_art\\focus16.pcx", &ArtFocus[FOCUS_SMALL], 8);
	LoadMaskedArt("ui_art\\focus.pcx", &ArtFocus[FOCUS_MED], 8);
	LoadMaskedArt("ui_art\\focus42.pcx", &ArtFocus[FOCUS_BIG], 8);
	LoadMaskedArt("ui_art\\cursor.pcx", &ArtCursor, 1, 0);

	LoadHeros();
}

} // namespace

void UnloadUiGFX()
{
	ArtHero.Unload();
	ArtCursor.Unload();
	for (auto &art : ArtFocus)
		art.Unload();
	for (auto &art : ArtLogos)
		art.Unload();
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

bool UiValidPlayerName(const char *name)
{
	if (strlen(name) == 0)
		return false;

	if (strpbrk(name, ",<>%&\\\"?*#/:") != nullptr || strpbrk(name, " ") != nullptr)
		return false;

	for (BYTE *letter = (BYTE *)name; *letter != '\0'; letter++)
		if (*letter < 0x20 || (*letter > 0x7E && *letter < 0xC0))
			return false;

	const char *const bannedNames[] = {
		"gvdl",
		"dvou",
		"tiju",
		"cjudi",
		"bttipmf",
		"ojhhfs",
		"cmj{{bse",
		"benjo",
	};

	char tmpname[PLR_NAME_LEN];
	CopyUtf8(tmpname, name, sizeof(tmpname));
	for (size_t i = 0, n = strlen(tmpname); i < n; i++)
		tmpname[i]++;

	for (const auto *bannedName : bannedNames) {
		if (strstr(tmpname, bannedName) != nullptr)
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
	LoadArt(pszFile, &ArtBackground, frames, pPal);
	if (ArtBackground.surface == nullptr)
		return;

	LoadPalInMem(pPal);
	ApplyGamma(logical_palette, orig_palette, 256);

	fadeTc = 0;
	fadeValue = 0;

	if (IsHardwareCursorEnabled() && ArtCursor.surface != nullptr && GetCurrentCursorInfo().type() != CursorType::UserInterface) {
#if SDL_VERSION_ATLEAST(2, 0, 0)
		SDL_SetSurfacePalette(ArtCursor.surface.get(), Palette.get());
		SDL_SetColorKey(ArtCursor.surface.get(), 1, 0);
#endif
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
	if (ArtBackgroundWidescreen.surface != nullptr) {
		SDL_Rect rectw = { 0, UI_OFFSET_Y, 0, 0 };
		vecDialog->push_back(std::make_unique<UiImage>(&ArtBackgroundWidescreen, rectw, UiFlags::AlignCenter));
	}

	SDL_Rect rect = { 0, UI_OFFSET_Y, 0, 0 };
	vecDialog->push_back(std::make_unique<UiImage>(&ArtBackground, rect, UiFlags::AlignCenter));
}

void UiAddLogo(std::vector<std::unique_ptr<UiItemBase>> *vecDialog, int size, int y)
{
	SDL_Rect rect = { 0, (Sint16)(UI_OFFSET_Y + y), 0, 0 };
	vecDialog->push_back(std::make_unique<UiImage>(&ArtLogos[size], rect, UiFlags::AlignCenter, /*bAnimated=*/true));
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

void DrawSelector(const SDL_Rect &rect)
{
	int size = FOCUS_SMALL;
	if (rect.h >= 42)
		size = FOCUS_BIG;
	else if (rect.h >= 30)
		size = FOCUS_MED;
	Art *art = &ArtFocus[size];

	int frame = GetAnimationFrame(art->frames);
	int y = rect.y + (rect.h - art->h()) / 2; // TODO FOCUS_MED appares higher than the box

	DrawArt({ rect.x, y }, art, frame);
	DrawArt({ rect.x + rect.w - art->w(), y }, art, frame);
}

void UiClearScreen()
{
	if (gnScreenWidth > 640) // Background size
		SDL_FillRect(DiabloUiSurface(), nullptr, 0x000000);
}

void UiPollAndRender()
{
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0) {
		UiFocusNavigation(&event);
		UiHandleEvents(&event);
	}
	HandleMenuAction(GetMenuHeldUpDownAction());
	UiRenderItems(gUiItems);
	DrawMouse();
	UiFadeIn();

	// Must happen after the very first UiFadeIn, which sets the cursor.
	if (IsHardwareCursor())
		SetHardwareCursorVisible(!sgbControllerActive);

#ifdef __3DS__
	// Keyboard blocks until input is finished
	// so defer until after render and fade-in
	ctr_vkbdFlush();
#endif
}

namespace {

void Render(UiText *uiText)
{
	Rectangle rect { { uiText->m_rect.x, uiText->m_rect.y }, { uiText->m_rect.w, uiText->m_rect.h } };

	const Surface &out = Surface(DiabloUiSurface());
	DrawString(out, uiText->m_text, rect, uiText->m_iFlags | UiFlags::FontSizeDialog);
}

void Render(const UiArtText *uiArtText)
{
	Rectangle rect { { uiArtText->m_rect.x, uiArtText->m_rect.y }, { uiArtText->m_rect.w, uiArtText->m_rect.h } };

	const Surface &out = Surface(DiabloUiSurface());
	DrawString(out, uiArtText->text(), rect, uiArtText->m_iFlags, uiArtText->spacing(), uiArtText->lineHeight());
}

void Render(const UiImage *uiImage)
{
	int x = uiImage->m_rect.x;
	if (HasAnyOf(uiImage->m_iFlags, UiFlags::AlignCenter) && uiImage->m_art != nullptr) {
		const int xOffset = GetCenterOffset(uiImage->m_art->w(), uiImage->m_rect.w);
		x += xOffset;
	}
	if (uiImage->m_animated) {
		DrawAnimatedArt(uiImage->m_art, { x, uiImage->m_rect.y });
	} else {
		DrawArt({ x, uiImage->m_rect.y }, uiImage->m_art, uiImage->m_frame, uiImage->m_rect.w);
	}
}

void Render(const UiArtTextButton *uiButton)
{
	Rectangle rect { { uiButton->m_rect.x, uiButton->m_rect.y }, { uiButton->m_rect.w, uiButton->m_rect.h } };

	const Surface &out = Surface(DiabloUiSurface());
	DrawString(out, uiButton->m_text, rect, uiButton->m_iFlags);
}

void Render(const UiList *uiList)
{
	const Surface &out = Surface(DiabloUiSurface());

	for (std::size_t i = listOffset; i < uiList->m_vecItems.size() && (i - listOffset) < ListViewportSize; ++i) {
		SDL_Rect rect = uiList->itemRect(i - listOffset);
		const UiListItem *item = uiList->GetItem(i);
		if (i == SelectedItem)
			DrawSelector(rect);

		Rectangle rectangle { { rect.x, rect.y }, { rect.w, rect.h } };
		if (item->args.size() == 0)
			DrawString(out, item->m_text, rectangle, uiList->m_iFlags | item->uiFlags, uiList->spacing());
		else
			DrawStringWithColors(out, item->m_text, item->args, rectangle, uiList->m_iFlags | item->uiFlags, uiList->spacing());
	}
}

void Render(const UiScrollbar *uiSb)
{
	// Bar background (tiled):
	{
		const int bgYEnd = DownArrowRect(uiSb).y;
		int bgY = uiSb->m_rect.y + uiSb->m_arrow->h();
		while (bgY < bgYEnd) {
			std::size_t drawH = std::min(bgY + uiSb->m_bg->h(), bgYEnd) - bgY;
			DrawArt({ uiSb->m_rect.x, bgY }, uiSb->m_bg, 0, SCROLLBAR_BG_WIDTH, drawH);
			bgY += drawH;
		}
	}

	// Arrows:
	{
		const SDL_Rect rect = UpArrowRect(uiSb);
		const int frame = static_cast<int>(scrollBarState.upArrowPressed ? ScrollBarArrowFrame_UP_ACTIVE : ScrollBarArrowFrame_UP);
		DrawArt({ rect.x, rect.y }, uiSb->m_arrow, frame, rect.w);
	}
	{
		const SDL_Rect rect = DownArrowRect(uiSb);
		const int frame = static_cast<int>(scrollBarState.downArrowPressed ? ScrollBarArrowFrame_DOWN_ACTIVE : ScrollBarArrowFrame_DOWN);
		DrawArt({ rect.x, rect.y }, uiSb->m_arrow, frame, rect.w);
	}

	// Thumb:
	if (SelectedItemMax > 0) {
		const SDL_Rect rect = ThumbRect(uiSb, SelectedItem, SelectedItemMax + 1);
		DrawArt({ rect.x, rect.y }, uiSb->m_thumb);
	}
}

void Render(const UiEdit *uiEdit)
{
	DrawSelector(uiEdit->m_rect);

	Rectangle rect { { uiEdit->m_rect.x + 43, uiEdit->m_rect.y + 1 }, { uiEdit->m_rect.w - 86, uiEdit->m_rect.h } };

	const Surface &out = Surface(DiabloUiSurface());
	DrawString(out, uiEdit->m_value, rect, uiEdit->m_iFlags | UiFlags::TextCursor);
}

void RenderItem(UiItemBase *item)
{
	if (item->has_flag(UiFlags::ElementHidden))
		return;
	switch (item->m_type) {
	case UiType::Text:
		Render(static_cast<UiText *>(item));
		break;
	case UiType::ArtText:
		Render(static_cast<UiArtText *>(item));
		break;
	case UiType::Image:
		Render(static_cast<UiImage *>(item));
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
	if (event.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT)
		return false;
	uiButton->m_action();
	return true;
}

#ifdef USE_SDL1
Uint32 dbClickTimer;
#endif

bool HandleMouseEventList(const SDL_Event &event, UiList *uiList)
{
	if (event.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT)
		return false;

	std::size_t index = uiList->indexAt(event.button.y);
	index += listOffset;

	if (gfnListFocus != nullptr && SelectedItem != index) {
		UiFocus(index, true);
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
	if (item->has_any_flag(UiFlags::ElementHidden | UiFlags::ElementDisabled) || !IsInsideRect(event, item->m_rect))
		return false;
	switch (item->m_type) {
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
			if (item->m_type == UiType::Button)
				HandleGlobalMouseUpButton(static_cast<UiButton *>(item));
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
			if (item->m_type == UiType::Button)
				HandleGlobalMouseUpButton(static_cast<UiButton *>(item.get()));
		}
	}

	return handled;
}

void DrawMouse()
{
	if (IsHardwareCursor() || sgbControllerActive || sgbTouchActive)
		return;

	DrawArt(MousePosition, &ArtCursor);
}
} // namespace devilution
