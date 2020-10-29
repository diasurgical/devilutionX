/**
 * @file diablo.h
 *
 * Interface of the main game initialization functions.
 */
#ifndef __DIABLO_H__
#define __DIABLO_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern SDL_Window *ghMainWnd;
extern DWORD glSeedTbl[NUMLEVELS];
extern int gnLevelTypeTbl[NUMLEVELS];
#ifndef HELLFIRE
extern int glEndSeed[NUMLEVELS];
extern int glMid1Seed[NUMLEVELS];
extern int glMid2Seed[NUMLEVELS];
extern int glMid3Seed[NUMLEVELS];
#else
extern int glEndSeed[NUMLEVELS + 1];
extern int glMid1Seed[NUMLEVELS + 1];
extern int glMid2Seed[NUMLEVELS + 1];
extern int glMid3Seed[NUMLEVELS + 1];
#endif
extern int MouseX;
extern int MouseY;
extern BOOL gbGameLoopStartup;
extern BOOL gbRunGame;
extern BOOL gbRunGameResult;
extern BOOL zoomflag;
extern BOOL gbProcessPlayers;
extern BOOL gbLoadGame;
extern HINSTANCE ghInst;
extern int DebugMonsters[10];
extern BOOLEAN cineflag;
extern int force_redraw;
extern BOOL visiondebug;
/* These are defined in fonts.h */
extern BOOL was_fonts_init;
extern void FontsCleanup();
/** unused */
extern BOOL scrollflag;
extern BOOL light4flag;
extern BOOL leveldebug;
extern BOOL monstdebug;
/** unused */
extern BOOL trigdebug;
extern int setseed;
extern int debugmonsttypes;
extern int PauseMode;
#ifdef HELLFIRE
extern BOOLEAN UseTheoQuest;
extern BOOLEAN UseCowFarmer;
extern BOOLEAN UseNestArt;
extern BOOLEAN UseBardTest;
extern BOOLEAN UseBarbarianTest;
extern BOOLEAN UseMultiTest;
#endif
extern char sgbMouseDown;
extern int color_cycle_timer;
extern int ticks_per_sec;
extern WORD tick_delay;

void FreeGameMem();
BOOL StartGame(BOOL bNewGame, BOOL bSinglePlayer);
void run_game_loop(unsigned int uMsg);
void start_game(unsigned int uMsg);
void free_game();
int DiabloMain(int argc, char **argv);
void diablo_parse_flags(int argc, char **argv);
void diablo_init_screen();
void diablo_reload_process(HINSTANCE hInstance);
void diablo_quit(int exitStatus);
BOOL PressEscKey();
void DisableInputWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void GM_Game(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL LeftMouseDown(int wParam);
BOOL LeftMouseCmd(BOOL bShift);
BOOL TryIconCurs();
void LeftMouseUp();
void RightMouseDown();
void j_gmenu_on_mouse_move(LPARAM lParam);
BOOL PressSysKey(int wParam);
void diablo_hotkey_msg(DWORD dwMsg);
void ReleaseKey(int vkey);
void PressKey(int vkey);
void diablo_pause_game();
void PressChar(int vkey);
void LoadLvlGFX();
void LoadAllGFX();
void CreateLevel(int lvldir);
void LoadGameLevel(BOOL firstflag, int lvldir);
void game_loop(BOOL bStartup);
void game_logic();
void timeout_cursor(BOOL bTimeout);
void diablo_color_cyc_logic();

/* data */

/* rdata */

extern BOOL fullscreen;
extern int showintrodebug;
#ifdef _DEBUG
extern int questdebug;
extern int debug_mode_key_s;
extern int debug_mode_key_w;
extern int debug_mode_key_inverted_v;
extern int debug_mode_dollar_sign;
extern int debug_mode_key_d;
extern int debug_mode_key_i;
extern int dbgplr;
extern int dbgqst;
extern int dbgmon;
extern int arrowdebug;
#endif
extern int frameflag;
extern int frameend;
extern int framerate;
extern int framestart;
extern BOOL FriendlyMode;
extern const char *const spszMsgTbl[4];
extern const char *const spszMsgHotKeyTbl[4];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __DIABLO_H__ */
