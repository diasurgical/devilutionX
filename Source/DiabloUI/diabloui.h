#pragma once

#include <SDL.h>
#include <array>
#include <cstddef>
#include <cstdint>

#include "DiabloUI/art.h"
#include "DiabloUI/ui_item.h"
#include "player.h"
#include "utils/display.h"

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

enum _mainmenu_selections : uint8_t {
	MAINMENU_NONE,
	MAINMENU_SINGLE_PLAYER,
	MAINMENU_MULTIPLAYER,
	MAINMENU_SHOW_SUPPORT,
	MAINMENU_SETTINGS,
	MAINMENU_SHOW_CREDITS,
	MAINMENU_EXIT_DIABLO,
	MAINMENU_ATTRACT_MODE,
};

enum _selhero_selections : uint8_t {
	SELHERO_NEW_DUNGEON,
	SELHERO_CONTINUE,
	SELHERO_CONNECT,
	SELHERO_PREVIOUS,
};

struct _uidefaultstats {
	uint16_t strength;
	uint16_t magic;
	uint16_t dexterity;
	uint16_t vitality;
};

struct _uiheroinfo {
	uint32_t saveNumber;
	char name[16];
	uint8_t level;
	HeroClass heroclass;
	uint8_t herorank;
	uint16_t strength;
	uint16_t magic;
	uint16_t dexterity;
	uint16_t vitality;
	bool hassaved;
	bool spawned;
};

extern std::array<Art, 3> ArtLogos;
extern std::array<Art, 3> ArtFocus;
extern Art ArtBackground;
extern Art ArtBackgroundWidescreen;
extern Art ArtCursor;
extern Art ArtHero;

extern void (*gfnSoundFunction)(const char *file);
extern bool (*gfnHeroInfo)(bool (*fninfofunc)(_uiheroinfo *));

inline SDL_Surface *DiabloUiSurface()
{
	auto *output_surface = GetOutputSurface();

#ifdef USE_SDL1
	// When using a non double-buffered hardware surface, render the UI
	// to an off-screen surface first to avoid flickering / tearing.
	if ((output_surface->flags & SDL_HWSURFACE) != 0
	    && (output_surface->flags & SDL_DOUBLEBUF) == 0) {
		return PalSurface;
	}
#endif

	return output_surface;
}

void UiDestroy();
void UiTitleDialog();
void UiInitialize();
bool UiValidPlayerName(const char *name); /* check */
void UiSelHeroMultDialog(bool (*fninfo)(bool (*fninfofunc)(_uiheroinfo *)), bool (*fncreate)(_uiheroinfo *), bool (*fnremove)(_uiheroinfo *), void (*fnstats)(unsigned int, _uidefaultstats *), _selhero_selections *dlgresult, uint32_t *saveNumber);
void UiSelHeroSingDialog(bool (*fninfo)(bool (*fninfofunc)(_uiheroinfo *)), bool (*fncreate)(_uiheroinfo *), bool (*fnremove)(_uiheroinfo *), void (*fnstats)(unsigned int, _uidefaultstats *), _selhero_selections *dlgresult, uint32_t *saveNumber, _difficulty *difficulty);
bool UiCreditsDialog();
bool UiSupportDialog();
bool UiMainMenuDialog(const char *name, _mainmenu_selections *pdwResult, void (*fnSound)(const char *file), int attractTimeOut);
bool UiProgressDialog(const char *msg, int (*fnfunc)());
bool UiSelectGame(GameData *gameData, int *playerId);
bool UiSelectProvider(GameData *gameData);
void UiFadeIn();
void UiHandleEvents(SDL_Event *event);
bool UiItemMouseEvents(SDL_Event *event, const std::vector<UiItemBase *> &items);
bool UiItemMouseEvents(SDL_Event *event, const std::vector<std::unique_ptr<UiItemBase>> &items);
Sint16 GetCenterOffset(Sint16 w, Sint16 bw = 0);
void LoadPalInMem(const SDL_Color *pPal);
void DrawMouse();
void LoadBackgroundArt(const char *pszFile, int frames = 1);
void UiAddBackground(std::vector<std::unique_ptr<UiItemBase>> *vecDialog);
void UiAddLogo(std::vector<std::unique_ptr<UiItemBase>> *vecDialog, int size = LOGO_MED, int y = 0);
void UiFocusNavigationSelect();
void UiFocusNavigationEsc();
void UiFocusNavigationYesNo();
void UiInitList(void (*fnFocus)(int value), void (*fnSelect)(int value), void (*fnEsc)(), const std::vector<std::unique_ptr<UiItemBase>> &items, bool wraps = false, bool (*fnYesNo)() = NULL, size_t selectedItem = 0);
void UiClearScreen();
void UiPollAndRender();
void UiRenderItems(const std::vector<UiItemBase *> &items);
void UiRenderItems(const std::vector<std::unique_ptr<UiItemBase>> &items);
void UiInitList_clear();

void mainmenu_restart_repintro();
} // namespace devilution
