#pragma once

#include <cstddef>
#include <SDL.h>

#include "DiabloUI/art.h"
#include "DiabloUI/ui_item.h"

namespace dvl {

extern int SelectedItem;

typedef enum _artFocus {
	FOCUS_SMALL,
	FOCUS_MED,
	FOCUS_BIG,
} _artFocus;

typedef enum _artLogo {
	LOGO_SMALL,
	LOGO_MED,
	LOGO_BIG,
} _artLogo;

extern Art ArtLogos[3];
extern Art ArtFocus[3];
extern Art ArtBackground;
extern Art ArtCursor;
extern Art ArtHero;
extern bool gbSpawned;

constexpr auto MAINMENU_BACKGROUND = UiImage(&ArtBackground, { PANEL_LEFT, 0, 640, 480 });
constexpr auto MAINMENU_LOGO = UiImage(&ArtLogos[LOGO_MED], /*animated=*/true, /*frame=*/0, { 0, 0, 0, 0 }, UIS_CENTER);

template <class T, size_t N>
constexpr size_t size(T (&)[N])
{
	return N;
}

extern void (*gfnSoundFunction)(char *file);
extern BOOL (*gfnHeroInfo)(BOOL (*fninfofunc)(_uiheroinfo *));

bool IsInsideRect(const SDL_Event &event, const SDL_Rect &rect);
void UiFadeIn();
void UiFocusNavigation(SDL_Event *event);
void UiHandleEvents(SDL_Event *event);
bool UiItemMouseEvents(SDL_Event *event, UiItem *items, std::size_t size);
int GetCenterOffset(int w, int bw = 0);
void LoadPalInMem(const SDL_Color *pPal);
void DrawMouse();
void LoadBackgroundArt(const char *pszFile);
void SetMenu(int MenuId);
void UiFocusNavigationSelect();
void UiFocusNavigationEsc();
void UiFocusNavigationYesNo();
void UiInitList(int min, int max, void (*fnFocus)(int value), void (*fnSelect)(int value), void (*fnEsc)(), UiItem *items, int size, bool wraps = false, bool (*fnYesNo)() = NULL);
void UiInitScrollBar(UiScrollBar *ui_sb, std::size_t viewport_size, const std::size_t *current_offset);
void UiClearScreen();
void UiPollAndRender();
void UiRenderItems(UiItem *items, std::size_t size);

void DvlIntSetting(const char *valuename, int *value);
void DvlStringSetting(const char *valuename, char *string, int len);

void mainmenu_restart_repintro();
} // namespace dvl
