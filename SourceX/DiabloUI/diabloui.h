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

#ifndef _XBOX
#define MAINMENU_BACKGROUND UiImage(&ArtBackground, { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
#define MAINMENU_LOGO UiImage(&ArtLogos[LOGO_MED], /*animated=*/true, /*frame=*/0, { 0, 0, 0, 0 }, UIS_CENTER);

template <class T, size_t N>
constexpr size_t size(T (&)[N])
{
	return N;
}
#endif

extern void (*gfnSoundFunction)(char *file);

void UiFadeIn();
bool UiItemMouseEvents(SDL_Event *event, vUiItemBase items, std::size_t size);
void UiHandleEvents(SDL_Event *event);
bool UiItemMouseEvents(SDL_Event *event, vUiItemBase items, std::size_t size);
int GetCenterOffset(int w, int bw = 0);
void LoadPalInMem(const SDL_Color *pPal);
void DrawMouse();
void LoadBackgroundArt(const char *pszFile);
void UiFocusNavigationSelect();
void UiFocusNavigationEsc();
void UiFocusNavigationYesNo();
void UiInitList(int min, int max, void (*fnFocus)(int value), void (*fnSelect)(int value), void (*fnEsc)(), vUiItemBase items, int size, bool wraps = false, bool (*fnYesNo)() = NULL);
void UiInitScrollBar(UiScrollBar *ui_sb, std::size_t viewport_size, const std::size_t *current_offset);
void UiPollAndRender();
void UiRenderItems(vUiItemBase items, std::size_t size);
void UiInitList_clear();

void DvlIntSetting(const char *valuename, int *value);
void DvlStringSetting(const char *valuename, char *string, int len);

void mainmenu_restart_repintro();
} // namespace dvl
