#pragma once

#include <stdint.h>
#include <array>
#include <cstddef>
#include <SDL.h>

#include "DiabloUI/art.h"
#include "DiabloUI/ui_item.h"
#include "display.h"

namespace devilution {

extern std::size_t SelectedItem;
extern bool textInputActive;

enum _artFocus : uint8_t {
	FOCUS_SMALL,
	FOCUS_MED,
	FOCUS_BIG,
};

enum _artLogo : uint8_t {
	LOGO_SMALL,
	LOGO_MED,
	LOGO_BIG,
};

extern std::array<Art, 3> ArtLogos;
extern std::array<Art, 3> ArtFocus;
extern Art ArtBackground;
extern Art ArtBackgroundWidescreen;
extern Art ArtCursor;
extern Art ArtHero;
extern bool gbSpawned;

extern void (*gfnSoundFunction)(const char *file);
extern bool (*gfnHeroInfo)(bool (*fninfofunc)(_uiheroinfo *));

inline SDL_Surface *DiabloUiSurface()
{
	return GetOutputSurface();
}

void UiFadeIn();
void UiHandleEvents(SDL_Event *event);
bool UiItemMouseEvents(SDL_Event *event, std::vector<UiItemBase *> items);
Sint16 GetCenterOffset(Sint16 w, Sint16 bw = 0);
void LoadPalInMem(const SDL_Color *pPal);
void DrawMouse();
void LoadBackgroundArt(const char *pszFile, int frames = 1);
void UiAddBackground(std::vector<UiItemBase *> *vecDialog);
void UiAddLogo(std::vector<UiItemBase *> *vecDialog, int size = LOGO_MED, int y = 0);
void UiFocusNavigationSelect();
void UiFocusNavigationEsc();
void UiFocusNavigationYesNo();
void UiInitList(int count, void (*fnFocus)(int value), void (*fnSelect)(int value), void (*fnEsc)(), std::vector<UiItemBase *> items, bool wraps = false, bool (*fnYesNo)() = NULL);
void UiInitScrollBar(UiScrollBar *ui_sb, std::size_t viewport_size, const std::size_t *current_offset);
void UiClearScreen();
void UiPollAndRender();
void UiRenderItems(std::vector<UiItemBase *> items);
void UiInitList_clear();

void mainmenu_restart_repintro();
} // namespace devilution
