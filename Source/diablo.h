//HEADER_GOES_HERE
#ifndef __DIABLO_H__
#define __DIABLO_H__

#include "../types.h"

//#ifdef __cplusplus
//extern "C" {
//#endif
#include "appfat.h"
#include "automap.h"
#include "capture.h"
#include "codec.h"
#include "control.h"
#include "cursor.h"
#include "dead.h"
#include "debug.h"
#include "doom.h"
#include "drlg_l1.h"
#include "drlg_l2.h"
#include "drlg_l3.h"
#include "drlg_l4.h"
#include "dthread.h"
#include "dx.h"
#include "effects.h"
#include "encrypt.h"
#include "engine.h"
#include "error.h"
#include "fault.h"
#include "gamemenu.h"
#include "gendung.h"
#include "gmenu.h"
#include "help.h"
#include "init.h"
#include "interfac.h"
#include "inv.h"
#include "itemdat.h"
#include "items.h"
#include "lighting.h"
#include "loadsave.h"
#include "logging.h"
#include "mainmenu.h"
#include "minitext.h"
#include "misdat.h"
#include "missiles.h"
#include "monstdat.h"
#include "monster.h"
#include "movie.h"
#include "mpqapi.h"
#include "msg.h"
#include "msgcmd.h"
#include "multi.h"
#include "nthread.h"
#include "objdat.h"
#include "objects.h"
#include "pack.h"
#include "palette.h"
#include "path.h"
#include "pfile.h"
#include "player.h"
#include "plrmsg.h"
#include "portal.h"
#include "quests.h"
#include "restrict.h"
#include "scrollrt.h"
#include "setmaps.h"
#include "sha.h"
#include "sound.h"
#include "spelldat.h"
#include "spells.h"
#include "stores.h"
#include "sync.h"
#include "textdat.h" // check file name
#include "themes.h"
#include "tmsg.h"
#include "town.h"
#include "towners.h"
#include "track.h"
#include "trigs.h"
#include "wave.h"
#include "render.h" // linked last, likely .s/.asm
//#ifdef __cplusplus
//}
//#endif

extern HWND ghMainWnd;
extern int glMid1Seed[NUMLEVELS];
extern int glMid2Seed[NUMLEVELS];
extern int gnLevelTypeTbl[NUMLEVELS];
extern int MouseY;
extern int MouseX;
extern BOOL gbGameLoopStartup;
extern DWORD glSeedTbl[NUMLEVELS];
extern BOOL gbRunGame;
extern int glMid3Seed[NUMLEVELS];
extern BOOL gbRunGameResult;
extern int zoomflag;
extern BOOL gbProcessPlayers;
extern int glEndSeed[NUMLEVELS];
extern BOOL gbLoadGame;
extern HINSTANCE ghInst;
extern int DebugMonsters[10];
extern BOOLEAN cineflag;
extern int drawpanflag;
extern BOOL visiondebug;
extern BOOL scrollflag; /* unused */
extern BOOL light4flag;
extern BOOL leveldebug;
extern BOOL monstdebug;
extern BOOL trigdebug; /* unused */
extern int setseed;
extern int debugmonsttypes;
extern int PauseMode;
extern char sgbMouseDown;
extern int color_cycle_timer;

void FreeGameMem();
BOOL StartGame(BOOL bNewGame, BOOL bSinglePlayer);
void run_game_loop(unsigned int uMsg);
void start_game(unsigned int uMsg);
void free_game();
BOOL diablo_get_not_running();
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
void diablo_parse_flags(char *args);
void diablo_init_screen();
BOOL diablo_find_window(LPCSTR lpClassName);
void diablo_reload_process(HINSTANCE hInstance);
BOOL PressEscKey();
LRESULT CALLBACK DisableInputWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK GM_Game(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
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
#ifdef _DEBUG
extern int showintrodebug;
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
extern int frameflag;
extern int frameend;
extern int framerate;
extern int framestart;
#endif
extern BOOL FriendlyMode;
extern char *spszMsgTbl[4];
extern char *spszMsgKeyTbl[4];

#ifdef DEVILUTION_STUB
#include "miniwin/popdecl.inc"
#endif

DEVILUTION_END_NAMESPACE

#endif /* __DIABLO_H__ */
