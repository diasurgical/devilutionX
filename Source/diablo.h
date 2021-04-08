/**
 * @file diablo.h
 *
 * Interface of the main game initialization functions.
 */
#pragma once

#include "pack.h"
#ifdef _DEBUG
#include "monstdat.h"
#endif

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

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
extern BOOL gbRunGame;
extern BOOL gbRunGameResult;
extern BOOL zoomflag;
extern BOOL gbProcessPlayers;
extern bool gbLoadGame;
extern BOOLEAN cineflag;
extern int force_redraw;
/* These are defined in fonts.h */
extern BOOL was_fonts_init;
extern void FontsCleanup();
extern BOOL light4flag;
extern int PauseMode;
extern bool gbTheoQuest;
extern bool gbCowQuest;
extern bool gbNestArt;
extern bool gbBard;
extern bool gbBarbarian;
extern char sgbMouseDown;
extern int gnTickRate;
extern WORD gnTickDelay;

void FreeGameMem();
BOOL StartGame(BOOL bNewGame, BOOL bSinglePlayer);
[[noreturn]] void diablo_quit(int exitStatus);
int DiabloMain(int argc, char **argv);
BOOL TryIconCurs();
void diablo_pause_game();
bool PressEscKey();
void DisableInputWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
void GM_Game(UINT uMsg, WPARAM wParam, LPARAM lParam);
void LoadGameLevel(BOOL firstflag, int lvldir);
void game_loop(BOOL bStartup);
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
extern bool gbFriendlyFire;

#ifdef __cplusplus
}
#endif

}
