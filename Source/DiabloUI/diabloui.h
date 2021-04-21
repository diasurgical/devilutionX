#pragma once

#include <stdint.h>
#include <array>
#include <cstddef>
#include <SDL.h>

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
	MAINMENU_REPLAY_INTRO,
	MAINMENU_SHOW_SUPPORT,
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
	Uint16 strength;
	Uint16 magic;
	Uint16 dexterity;
	Uint16 vitality;
};

struct _uiheroinfo {
	struct _uiheroinfo *next;
	char name[16];
	Uint16 level;
	HeroClass heroclass;
	Uint8 herorank;
	Uint16 strength;
	Uint16 magic;
	Uint16 dexterity;
	Uint16 vitality;
	Sint32 gold;
	Sint32 hassaved;
	bool spawned;
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
	auto *output_surface = GetOutputSurface();

#ifdef USE_SDL1
	// When using a non double-buffered hardware surface, render the UI
	// to an off-screen surface first to avoid flickering / tearing.
	if ((output_surface->flags & SDL_HWSURFACE) != 0
	    && (output_surface->flags & SDL_DOUBLEBUF) == 0) {
		return pal_surface;
	}
#endif

	return output_surface;
}

void UiDestroy();
void UiTitleDialog();
void UiSetSpawned(bool bSpawned);
void UiInitialize();
bool UiValidPlayerName(const char *name); /* check */
void UiSelHeroMultDialog(bool (*fninfo)(bool (*fninfofunc)(_uiheroinfo *)), bool (*fncreate)(_uiheroinfo *), bool (*fnremove)(_uiheroinfo *), void (*fnstats)(unsigned int, _uidefaultstats *), _selhero_selections *dlgresult, char (*name)[16]);
void UiSelHeroSingDialog(bool (*fninfo)(bool (*fninfofunc)(_uiheroinfo *)), bool (*fncreate)(_uiheroinfo *), bool (*fnremove)(_uiheroinfo *), void (*fnstats)(unsigned int, _uidefaultstats *), _selhero_selections *dlgresult, char (*name)[16], _difficulty *difficulty);
bool UiCreditsDialog();
bool UiSupportDialog();
bool UiMainMenuDialog(const char *name, _mainmenu_selections *pdwResult, void (*fnSound)(const char *file), int attractTimeOut);
bool UiProgressDialog(const char *msg, int (*fnfunc)(), int rate);
void UiProfileCallback();
void UiProfileDraw();
bool UiCategoryCallback(int a1, int a2, int a3, int a4, int a5, DWORD *a6, DWORD *a7);
bool UiGetDataCallback(int game_type, int data_code, void *a3, int a4, int a5);
bool UiAuthCallback(int a1, char *a2, char *a3, char a4, char *a5, char *lpBuffer, int cchBufferMax);
bool UiSoundCallback(int a1, int type, int a3);
bool UiDrawDescCallback(int game_type, DWORD color, const char *lpString, char *a4, int a5, UINT align, time_t a7, HDC *a8);
bool UiCreateGameCallback(int a1, int a2, int a3, int a4, int a5, int a6);
bool UiArtCallback(int game_type, unsigned int art_code, SDL_Color *pPalette, BYTE *pBuffer, DWORD dwBuffersize, DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwBpp);
int UiSelectGame(GameData *gameData, int *playerId);
int UiSelectProvider(GameData *gameData);
void UiSetupPlayerInfo(char *infostr, _uiheroinfo *pInfo, Uint32 type);
void UiFadeIn();
void UiHandleEvents(SDL_Event *event);
bool UiItemMouseEvents(SDL_Event *event, const std::vector<UiItemBase *> &items);
Sint16 GetCenterOffset(Sint16 w, Sint16 bw = 0);
void LoadPalInMem(const SDL_Color *pPal);
void DrawMouse();
void LoadBackgroundArt(const char *pszFile, int frames = 1);
void UiAddBackground(std::vector<UiItemBase *> *vecDialog);
void UiAddLogo(std::vector<UiItemBase *> *vecDialog, int size = LOGO_MED, int y = 0);
void UiFocusNavigationSelect();
void UiFocusNavigationEsc();
void UiFocusNavigationYesNo();
void UiInitList(int count, void (*fnFocus)(int value), void (*fnSelect)(int value), void (*fnEsc)(), const std::vector<UiItemBase *> &items, bool wraps = false, bool (*fnYesNo)() = NULL);
void UiInitScrollBar(UiScrollBar *ui_sb, std::size_t viewport_size, const std::size_t *current_offset);
void UiClearScreen();
void UiPollAndRender();
void UiRenderItems(const std::vector<UiItemBase *> &items);
void UiInitList_clear();

void mainmenu_restart_repintro();
} // namespace devilution
