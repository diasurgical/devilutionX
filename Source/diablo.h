/**
 * @file diablo.h
 *
 * Interface of the main game initialization functions.
 */
#pragma once

#include <stdint.h>

#include "pack.h"
#include "gendung.h"
#ifdef _DEBUG
#include "monstdat.h"
#endif

namespace devilution {

enum clicktype : int8_t {
	CLICK_NONE,
	CLICK_LEFT,
	CLICK_RIGHT,
};

#ifndef DEFAULT_WIDTH
#define DEFAULT_WIDTH 640
#endif
#ifndef DEFAULT_HEIGHT
#define DEFAULT_HEIGHT 480
#endif

extern SDL_Window *ghMainWnd;
extern DWORD glSeedTbl[NUMLEVELS];
extern dungeon_type gnLevelTypeTbl[NUMLEVELS];
extern int MouseX;
extern int MouseY;
extern bool gbRunGame;
extern bool gbRunGameResult;
extern bool zoomflag;
extern bool gbProcessPlayers;
extern bool gbLoadGame;
extern bool cineflag;
extern int force_redraw;
/* These are defined in fonts.h */
extern bool was_fonts_init;
extern void FontsCleanup();
extern bool light4flag;
extern int PauseMode;
extern bool gbNestArt;
extern bool gbBard;
extern bool gbBarbarian;
extern clicktype sgbMouseDown;
extern WORD gnTickDelay;

void FreeGameMem();
bool StartGame(bool bNewGame, bool bSinglePlayer);
[[noreturn]] void diablo_quit(int exitStatus);
int DiabloMain(int argc, char **argv);
bool TryIconCurs();
void diablo_pause_game();
bool PressEscKey();
void DisableInputWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
void GM_Game(UINT uMsg, WPARAM wParam, LPARAM lParam);
void LoadGameLevel(bool firstflag, lvl_entry lvldir);
void game_loop(bool bStartup);
void diablo_color_cyc_logic();

/* rdata */

extern bool gbForceWindowed;
extern bool leveldebug;
#ifdef _DEBUG
extern bool monstdebug;
extern _monster_id DebugMonsters[10];
extern int debugmonsttypes;
extern bool visiondebug;
extern int questdebug;
extern bool debug_mode_key_w;
extern bool debug_mode_key_inverted_v;
extern bool debug_mode_dollar_sign;
extern bool debug_mode_key_d;
extern bool debug_mode_key_i;
extern int debug_mode_key_j;
#endif
extern bool gbFriendlyMode;

}
