#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

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
	UIS_NULL     = 0,
	UIS_SMALL    = 0x1,
	UIS_MED      = 0x10,
	UIS_BIG      = 0x100,
	UIS_HUGE     = 0x1000,
	UIS_CENTER   = 0x10000,
	UIS_RIGHT    = 0x100000,
	UIS_VCENTER  = 0x1000000,
	UIS_SILVER   = 0x10000000,
	UIS_GOLD     = 0x100000000,
	UIS_SML1     = 0x1000000000,
	UIS_SML2     = 0x10000000000,
	UIS_LIST     = 0x100000000000,
	UIS_DISABLED = 0x1000000000000,
	UIS_HIDDEN   = 0x10000000000000,
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
	UiFlags flags;
	int value;
	char *caption;
	void *context;
} UI_Item;

extern TTF_Font *font;

extern BYTE *FontTables[4];
extern Art ArtFonts[4][2];
extern Art ArtLogos[3];
extern Art ArtFocus[3];
extern Art ArtBackground;
extern Art ArtCursor;
extern Art ArtHero;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

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

extern void(__stdcall *gfnSoundFunction)(char *file);

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
void UiInitList(int min, int max, void(__stdcall *fnFocus)(int value), void(__stdcall *fnSelect)(int value), void(__stdcall *fnEsc)(), UI_Item *items, int size, bool wraps = false);
void UiRender();
void UiRenderItems(UI_Item *items, int size);
void WordWrap(UI_Item *item);
