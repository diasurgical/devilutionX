#pragma once

#include <cstddef>
#include <SDL.h>
#include <SDL_ttf.h>

#include "art.h"
#include "ui_item.h"

namespace dvl {

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

typedef enum _artFontTables {
	AFT_SMALL,
	AFT_MED,
	AFT_BIG,
	AFT_HUGE,
} _artFontTables;

typedef enum _artFontColors {
	AFC_SILVER,
	AFC_GOLD,
} _artFontColors;

extern TTF_Font *font;

extern BYTE *FontTables[4];
extern Art ArtFonts[4][2];
extern Art ArtLogos[3];
extern Art ArtFocus[3];
extern Art ArtBackground;
extern Art ArtCursor;
extern Art ArtHero;
extern bool gbSpawned;

typedef enum TXT_JUST {
	JustLeft = 0,
	JustCentre = 1,
	JustRight = 2,
} TXT_JUST;

template <class T, size_t N>
constexpr size_t size(T (&)[N])
{
	return N;
}

extern void (*gfnSoundFunction)(char *file);

bool IsInsideRect(const SDL_Event &event, const SDL_Rect &rect);
void UiFadeIn(int steps = 16);
bool UiFocusNavigation(SDL_Event *event);
bool UiItemMouseEvents(SDL_Event *event, UiItem *items, int size);
int GetAnimationFrame(int frames, int fps = 60);
int GetCenterOffset(int w, int bw = 0);
void DrawArt(int screenX, int screenY, Art *art, int nFrame = 0, DWORD drawW = 0);
void DrawLogo(int t = 0, int size = LOGO_MED);
void DrawMouse();
void LoadBackgroundArt(char *pszFile);
void LoadMaskedArtFont(char *pszFile, Art *art, int frames, int mask = 250);
void SetMenu(int MenuId);
void UiFocusNavigationSelect();
void UiFocusNavigationEsc();
void UiFocusNavigationYesNo();
void UiInitList(int min, int max, void (*fnFocus)(int value), void (*fnSelect)(int value), void (*fnEsc)(), UiItem *items, int size, bool wraps = false, bool (*fnYesNo)() = NULL);
void UiRender();
void UiRenderItems(UiItem *items, int size);
void WordWrap(UiText *item);

void DvlIntSetting(const char *valuename, int *value);
void DvlStringSetting(const char *valuename, char *string, int len);

void mainmenu_restart_repintro();

}
