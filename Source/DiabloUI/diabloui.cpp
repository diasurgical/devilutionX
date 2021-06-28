#include "DiabloUI/diabloui.h"

#include <algorithm>
#include <string>

#include "DiabloUI/art_draw.h"
#include "DiabloUI/button.h"
#include "DiabloUI/dialogs.h"
#include "DiabloUI/fonts.h"
#include "DiabloUI/scrollbar.h"
#include "DiabloUI/text_draw.h"
#include "controls/controller.h"
#include "controls/menu_controls.h"
#include "dx.h"
#include "hwcursor.hpp"
#include "palette.h"
#include "storm/storm.h"
#include "utils/display.h"
#include "utils/log.hpp"
#include "utils/sdl_compat.h"
#include "utils/sdl_ptrs.h"
#include "utils/stubs.h"
#include "utils/utf8.h"

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

std::size_t SelectedItemMax;
std::size_t ListViewportSize = 1;
const std::size_t *ListOffset = nullptr;

std::array<Art, 3> ArtLogos;
std::array<Art, 3> ArtFocus;
Art ArtBackgroundWidescreen;
Art ArtBackground;
Art ArtCursor;
Art ArtHero;
bool gbSpawned;

void (*gfnSoundFunction)(const char *file);
void (*gfnListFocus)(int value);
void (*gfnListSelect)(int value);
void (*gfnListEsc)();
bool (*gfnListYesNo)();
std::vector<UiItemBase *> gUiItems;
bool UiItemsWraps;
char *UiTextInput;
int UiTextInputLen;
bool textInputActive = true;

std::size_t SelectedItem = 0;

namespace {

DWORD fadeTc;
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

} // namespace

void UiInitList(int count, void (*fnFocus)(int value), void (*fnSelect)(int value), void (*fnEsc)(), const std::vector<UiItemBase *> &items, bool itemsWraps, bool (*fnYesNo)())
{
	SelectedItem = 0;
	SelectedItemMax = std::max(count - 1, 0);
	ListViewportSize = count;
	gfnListFocus = fnFocus;
	gfnListSelect = fnSelect;
	gfnListEsc = fnEsc;
	gfnListYesNo = fnYesNo;
	gUiItems = items;
	UiItemsWraps = itemsWraps;
	ListOffset = nullptr;
	if (fnFocus != nullptr)
		fnFocus(0);

#ifndef __SWITCH__
	SDL_StopTextInput(); // input is enabled by default
#endif
	textInputActive = false;
	for (const auto &item : items) {
		if (item->m_type == UI_EDIT) {
			auto *pItemUIEdit = static_cast<UiEdit *>(item);
			SDL_SetTextInputRect(&item->m_rect);
			textInputActive = true;
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
		}
	}
}

void UiInitScrollBar(UiScrollBar *uiSb, std::size_t viewportSize, const std::size_t *currentOffset)
{
	ListViewportSize = viewportSize;
	ListOffset = currentOffset;
	if (ListViewportSize >= static_cast<std::size_t>(SelectedItemMax + 1)) {
		uiSb->add_flag(UIS_HIDDEN);
	} else {
		uiSb->remove_flag(UIS_HIDDEN);
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

void UiFocus(std::size_t itemIndex)
{
	if (SelectedItem == itemIndex)
		return;

	SelectedItem = itemIndex;

	UiPlayMoveSound();

	if (gfnListFocus != nullptr)
		gfnListFocus(itemIndex);
}

void UiFocusUp()
{
	if (SelectedItem > 0)
		UiFocus(SelectedItem - 1);
	else if (UiItemsWraps)
		UiFocus(SelectedItemMax);
}

void UiFocusDown()
{
	if (SelectedItem < SelectedItemMax)
		UiFocus(SelectedItem + 1);
	else if (UiItemsWraps)
		UiFocus(0);
}

// UiFocusPageUp/Down mimics the slightly weird behaviour of actual Diablo.

void UiFocusPageUp()
{
	if (ListOffset == nullptr || *ListOffset == 0) {
		UiFocus(0);
	} else {
		const std::size_t relpos = SelectedItem - *ListOffset;
		std::size_t prevPageStart = SelectedItem - relpos;
		if (prevPageStart >= ListViewportSize)
			prevPageStart -= ListViewportSize;
		else
			prevPageStart = 0;
		UiFocus(prevPageStart);
		UiFocus(*ListOffset + relpos);
	}
}

void UiFocusPageDown()
{
	if (ListOffset == nullptr || *ListOffset + ListViewportSize > static_cast<std::size_t>(SelectedItemMax)) {
		UiFocus(SelectedItemMax);
	} else {
		const std::size_t relpos = SelectedItem - *ListOffset;
		std::size_t nextPageEnd = SelectedItem + (ListViewportSize - relpos - 1);
		if (nextPageEnd + ListViewportSize <= static_cast<std::size_t>(SelectedItemMax))
			nextPageEnd += ListViewportSize;
		else
			nextPageEnd = SelectedItemMax;
		UiFocus(nextPageEnd);
		UiFocus(*ListOffset + relpos);
	}
}

void SelheroCatToName(char *inBuf, char *outBuf, int cnt)
{
	std::string output = utf8_to_latin1(inBuf);
	strncat(outBuf, output.c_str(), cnt - strlen(outBuf));
}

#ifdef __vita__
void selhero_SetName(char *in_buf, char *out_buf, int cnt)
{
	std::string output = utf8_to_latin1(in_buf);
	strncpy(out_buf, output.c_str(), cnt);
}
#endif

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
				int nameLen = strlen(UiTextInput);
				if (nameLen > 0) {
					UiTextInput[nameLen - 1] = '\0';
				}
				return;
			}
			default:
				break;
			}
#ifdef USE_SDL1
			if ((event->key.keysym.mod & KMOD_CTRL) == 0) {
				Uint16 unicode = event->key.keysym.unicode;
				if (unicode && (unicode & 0xFF80) == 0) {
					char utf8[SDL_TEXTINPUTEVENT_TEXT_SIZE];
					utf8[0] = (char)unicode;
					utf8[1] = '\0';
					SelheroCatToName(utf8, UiTextInput, UiTextInputLen);
				}
			}
#endif
			break;
		}
#ifndef USE_SDL1
		case SDL_TEXTINPUT:
			if (textInputActive) {
#ifdef __vita__
				selhero_SetName(event->text.text, UiTextInput, UiTextInputLen);
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
		if (event->window.event == SDL_WINDOWEVENT_SHOWN)
			gbActive = true;
		else if (event->window.event == SDL_WINDOWEVENT_HIDDEN)
			gbActive = false;
		else if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			ReinitializeHardwareCursor();
	}
#endif
}

void UiFocusNavigationSelect()
{
	UiPlaySelectSound();
	if (textInputActive) {
		if (strlen(UiTextInput) == 0) {
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

// Equivalent to SDL_Rect { ... } but avoids -Wnarrowing.
inline SDL_Rect MakeRect(int x, int y, int w, int h)
{
	using Pos = decltype(SDL_Rect {}.x);
	using Len = decltype(SDL_Rect {}.w);
	return SDL_Rect { static_cast<Pos>(x), static_cast<Pos>(y),
		static_cast<Len>(w), static_cast<Len>(h) };
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

	SDL_Surface *heros = SDL_CreateRGBSurfaceWithFormat(0, ArtHero.w(), portraitHeight * (static_cast<int>(enum_size<HeroClass>::value) + 1), 8, SDL_PIXELFORMAT_INDEX8);

	for (int i = 0; i <= static_cast<int>(enum_size<HeroClass>::value); i++) {
		int offset = portraitOrder[i] * portraitHeight;
		if (offset + portraitHeight > ArtHero.h()) {
			offset = 0;
		}
		SDL_Rect srcRect = MakeRect(0, offset, ArtHero.w(), portraitHeight);
		SDL_Rect dstRect = MakeRect(0, i * portraitHeight, ArtHero.w(), portraitHeight);
		SDL_BlitSurface(ArtHero.surface.get(), &srcRect, heros, &dstRect);
	}

	for (int i = 0; i <= static_cast<int>(enum_size<HeroClass>::value); i++) {
		Art portrait;
		char portraitPath[18];
		sprintf(portraitPath, "ui_art\\hero%i.pcx", i);
		LoadArt(portraitPath, &portrait);
		if (portrait.surface == nullptr)
			continue;

		SDL_Rect dstRect = MakeRect(0, i * portraitHeight, portrait.w(), portraitHeight);
		SDL_BlitSurface(portrait.surface.get(), nullptr, heros, &dstRect);
	}

	ArtHero.surface = SDLSurfaceUniquePtr { heros };
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

void UnloadUiGFX()
{
	ArtHero.Unload();
	ArtCursor.Unload();
	for (auto &art : ArtFocus)
		art.Unload();
	for (auto &art : ArtLogos)
		art.Unload();
}

} // namespace

void UiInitialize()
{
	LoadUiGFX();
	LoadArtFonts();
	if (ArtCursor.surface != nullptr) {
		if (SDL_ShowCursor(SDL_DISABLE) <= -1) {
			ErrSdl();
		}
	}
}

void UiDestroy()
{
	UnloadTtfFont();
	UnloadArtFonts();
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
	strncpy(tmpname, name, PLR_NAME_LEN - 1);
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
		SDL_SetSurfacePalette(ArtCursor.surface.get(), palette);
		SDL_SetColorKey(ArtCursor.surface.get(), 1, 0);
#endif
		SetHardwareCursor(CursorInfo::UserInterfaceCursor());
	}

	BlackPalette();

	SDL_FillRect(DiabloUiSurface(), nullptr, 0x000000);
	if (DiabloUiSurface() == pal_surface)
		BltFast(nullptr, nullptr);
	RenderPresent();
}

void UiAddBackground(std::vector<UiItemBase *> *vecDialog)
{
	if (ArtBackgroundWidescreen.surface != nullptr) {
		SDL_Rect rectw = { 0, UI_OFFSET_Y, 0, 0 };
		vecDialog->push_back(new UiImage(&ArtBackgroundWidescreen, /*bAnimated=*/false, /*iFrame=*/0, rectw, UIS_CENTER));
	}

	SDL_Rect rect = { 0, UI_OFFSET_Y, 0, 0 };
	vecDialog->push_back(new UiImage(&ArtBackground, /*bAnimated=*/false, /*iFrame=*/0, rect, UIS_CENTER));
}

void UiAddLogo(std::vector<UiItemBase *> *vecDialog, int size, int y)
{
	SDL_Rect rect = { 0, (Sint16)(UI_OFFSET_Y + y), 0, 0 };
	vecDialog->push_back(new UiImage(&ArtLogos[size], /*bAnimated=*/true, /*iFrame=*/0, rect, UIS_CENTER));
}

void UiFadeIn()
{
	if (fadeValue < 256) {
		if (fadeValue == 0 && fadeTc == 0)
			fadeTc = SDL_GetTicks();
		const int prevFadeValue = fadeValue;
		fadeValue = (SDL_GetTicks() - fadeTc) / 2.083; // 32 frames @ 60hz
		if (fadeValue > 256) {
			fadeValue = 256;
			fadeTc = 0;
		}
		if (fadeValue != prevFadeValue)
			SetFadeLevel(fadeValue);
	}

	if (DiabloUiSurface() == pal_surface)
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
	DrawTTF(uiText->m_text,
	    uiText->m_rect,
	    uiText->m_iFlags,
	    uiText->m_color,
	    uiText->m_shadow_color,
	    uiText->m_render_cache);
}

void Render(const UiArtText *uiArtText)
{
	DrawArtStr(uiArtText->m_text, uiArtText->m_rect, uiArtText->m_iFlags);
}

void Render(const UiImage *uiImage)
{
	int x = uiImage->m_rect.x;
	if ((uiImage->m_iFlags & UIS_CENTER) != 0 && uiImage->m_art != nullptr) {
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
	DrawArtStr(uiButton->m_text, uiButton->m_rect, uiButton->m_iFlags);
}

void Render(const UiList *uiList)
{
	for (std::size_t i = 0; i < uiList->m_vecItems.size(); ++i) {
		SDL_Rect rect = uiList->itemRect(i);
		const UiListItem *item = uiList->GetItem(i);
		if (i + (ListOffset == nullptr ? 0 : *ListOffset) == SelectedItem)
			DrawSelector(rect);
		DrawArtStr(item->m_text, rect, uiList->m_iFlags);
	}
}

void Render(const UiScrollBar *uiSb)
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
	SDL_Rect rect = uiEdit->m_rect;
	rect.x += 43;
	rect.y += 1;
	rect.w -= 86;
	DrawArtStr(uiEdit->m_value, rect, uiEdit->m_iFlags, /*drawTextCursor=*/true);
}

void RenderItem(UiItemBase *item)
{
	if (item->has_flag(UIS_HIDDEN))
		return;
	switch (item->m_type) {
	case UI_TEXT:
		Render(static_cast<UiText *>(item));
		break;
	case UI_ART_TEXT:
		Render(static_cast<UiArtText *>(item));
		break;
	case UI_IMAGE:
		Render(static_cast<UiImage *>(item));
		break;
	case UI_ART_TEXT_BUTTON:
		Render(static_cast<UiArtTextButton *>(item));
		break;
	case UI_BUTTON:
		RenderButton(static_cast<UiButton *>(item));
		break;
	case UI_LIST:
		Render(static_cast<UiList *>(item));
		break;
	case UI_SCROLLBAR:
		Render(static_cast<UiScrollBar *>(item));
		break;
	case UI_EDIT:
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

	const std::size_t index = uiList->indexAt(event.button.y);

	if (gfnListFocus != nullptr && SelectedItem != index) {
		UiFocus(index);
#ifdef USE_SDL1
		dbClickTimer = SDL_GetTicks();
	} else if (gfnListFocus == NULL || dbClickTimer + 500 >= SDL_GetTicks()) {
#else
	} else if (gfnListFocus == nullptr || event.button.clicks >= 2) {
#endif
		SelectedItem = index;
		UiFocusNavigationSelect();
#ifdef USE_SDL1
	} else {
		dbClickTimer = SDL_GetTicks();
#endif
	}

	return true;
}

bool HandleMouseEventScrollBar(const SDL_Event &event, const UiScrollBar *uiSb)
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
	if (item->has_any_flag(UIS_HIDDEN | UIS_DISABLED) || !IsInsideRect(event, item->m_rect))
		return false;
	switch (item->m_type) {
	case UI_ART_TEXT_BUTTON:
		return HandleMouseEventArtTextButton(event, static_cast<UiArtTextButton *>(item));
	case UI_BUTTON:
		return HandleMouseEventButton(event, static_cast<UiButton *>(item));
	case UI_LIST:
		return HandleMouseEventList(event, static_cast<UiList *>(item));
	case UI_SCROLLBAR:
		return HandleMouseEventScrollBar(event, static_cast<UiScrollBar *>(item));
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
			if (item->m_type == UI_BUTTON)
				HandleGlobalMouseUpButton(static_cast<UiButton *>(item));
		}
	}

	return handled;
}

void DrawMouse()
{
	if (IsHardwareCursor() || sgbControllerActive)
		return;

	DrawArt(MousePosition, &ArtCursor);
}
} // namespace devilution
