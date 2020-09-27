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

extern void (*gfnSoundFunction)(char *file);
extern BOOL (*gfnHeroInfo)(BOOL (*fninfofunc)(_uiheroinfo *));

void UiFadeIn();
void UiHandleEvents(SDL_Event *event);
bool UiItemMouseEvents(SDL_Event *event, std::vector<UiItemBase *> items);
int GetCenterOffset(int w, int bw = 0);
void LoadPalInMem(const SDL_Color *pPal);
void DrawMouse();
void LoadBackgroundArt(const char *pszFile);
void UiAddBackground(std::vector<UiItemBase *> *vecDialog);
void UiAddLogo(std::vector<UiItemBase *> *vecDialog, int size = LOGO_MED, int height = 0);
void UiFocusNavigationSelect();
void UiFocusNavigationEsc();
void UiFocusNavigationYesNo();
void UiInitList(int min, int max, void (*fnFocus)(int value), void (*fnSelect)(int value), void (*fnEsc)(), std::vector<UiItemBase *> items, bool wraps = false, bool (*fnYesNo)() = NULL);
void UiInitScrollBar(UiScrollBar *ui_sb, std::size_t viewport_size, const std::size_t *current_offset);
void UiClearScreen();
void UiPollAndRender();
void UiRenderItems(std::vector<UiItemBase *> items);
void UiInitList_clear();

void DvlIntSetting(const char *valuename, int *value);
void DvlStringSetting(const char *valuename, char *string, int len);

void mainmenu_restart_repintro();
} // namespace dvl
