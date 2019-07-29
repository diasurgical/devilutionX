#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

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

typedef enum UiTypes {
	UI_TEXT,
	UI_IMAGE,
	UI_BUTTON,
	UI_LIST,
	UI_EDIT,
} UiTypes;

typedef enum UiFlags {
	UIS_SMALL    = 1 << 0,
	UIS_MED      = 1 << 1,
	UIS_BIG      = 1 << 2,
	UIS_HUGE     = 1 << 3,
	UIS_CENTER   = 1 << 4,
	UIS_RIGHT    = 1 << 5,
	UIS_VCENTER  = 1 << 6,
	UIS_SILVER   = 1 << 7,
	UIS_GOLD     = 1 << 8,
	UIS_SML1     = 1 << 9,
	UIS_SML2     = 1 << 10,
	UIS_LIST     = 1 << 11,
	UIS_DISABLED = 1 << 12,
	UIS_HIDDEN   = 1 << 13,
} UiFlags;

typedef struct Art {
	BYTE *data;
	DWORD width;
	DWORD height;
	bool masked = false;
	BYTE mask;
} Art;

typedef struct UI_Item {
	SDL_Rect rect;
	UiTypes type;
	int flags;
	int value;
	char *caption;
	const void *context;
} UI_Item;

extern TTF_Font *font;

extern BYTE *FontTables[4];
extern Art ArtFonts[4][2];
extern Art ArtLogos[3];
extern Art ArtFocus[3];
extern Art ArtBackground;
extern Art ArtCursor;
extern Art ArtHero;

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

extern void(*gfnSoundFunction)(char *file);

bool IsInsideRect(const SDL_Event *event, const SDL_Rect *rect);
void UiFadeIn(int steps = 16);
bool UiFocusNavigation(SDL_Event *event);
bool UiItemMouseEvents(SDL_Event *event, UI_Item *items, int size);
int GetAnimationFrame(int frames, int fps = 60);
int GetCenterOffset(int w, int bw = 0);
void DrawArt(int screenX, int screenY, Art *art, int nFrame = 0, int drawW = 0);
void DrawLogo(int t = 0, int size = LOGO_MED);
void DrawMouse();
void LoadArt(char *pszFile, Art *art, int frames = 1, PALETTEENTRY *pPalette = NULL);
void LoadBackgroundArt(char *pszFile);
void LoadMaskedArtFont(char *pszFile, Art *art, int frames, int mask = 250);
void SetMenu(int MenuId);
void UiFocusNavigationSelect();
void UiFocusNavigationEsc();
void UiFocusNavigationYesNo();
void UiInitList(int min, int max, void (*fnFocus)(int value), void (*fnSelect)(int value), void (*fnEsc)(), UI_Item *items, int size, bool wraps = false, bool (*fnYesNo)() = NULL);
void UiRender();
void UiRenderItems(UI_Item *items, int size);
void WordWrap(UI_Item *item);

void DvlIntSetting(const char *valuename, int *value);
void DvlStringSetting(const char *valuename, char *string, int len);

void mainmenu_restart_repintro();

}
