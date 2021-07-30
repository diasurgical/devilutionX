/**
 * @file diablo.h
 *
 * Interface of the main game initialization functions.
 */
#pragma once

#include <cstdint>

#include "utils/endian.hpp"

#include "controls/keymapper.hpp"
#ifdef _DEBUG
#include "monstdat.h"
#endif
#include "gendung.h"
#include "init.h"

namespace devilution {

#define GAME_ID (gbIsHellfire ? (gbIsSpawn ? LoadBE32("HSHR") : LoadBE32("HRTL")) : (gbIsSpawn ? LoadBE32("DSHR") : LoadBE32("DRTL")))

#define NUMLEVELS 25

enum clicktype : int8_t {
	CLICK_NONE,
	CLICK_LEFT,
	CLICK_RIGHT,
};

/**
 * @brief Specifices what game logic step is currently executed
 */
enum class GameLogicStep {
	None,
	ProcessPlayers,
	ProcessMonsters,
	ProcessObjects,
	ProcessMissiles,
	ProcessItems,
	ProcessTowners,
	ProcessItemsTown,
	ProcessMissilesTown,
};

enum class MouseActionType : int {
	None,
	Walk,
	Spell,
	SpellMonsterTarget,
	SpellPlayerTarget,
	Attack,
	AttackMonsterTarget,
	AttackPlayerTarget,
	OperateObject,
	Other,
};

extern SDL_Window *ghMainWnd;
extern uint32_t glSeedTbl[NUMLEVELS];
extern dungeon_type gnLevelTypeTbl[NUMLEVELS];
extern Point MousePosition;
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
extern int PauseMode;
extern bool gbNestArt;
extern bool gbBard;
extern bool gbBarbarian;
/**
 * @brief Don't show Messageboxes or other user-interaction. Needed for UnitTests.
 */
extern bool gbQuietMode;
extern clicktype sgbMouseDown;
extern uint16_t gnTickDelay;
extern char gszProductName[64];

extern MouseActionType LastMouseButtonAction;

void FreeGameMem();
bool StartGame(bool bNewGame, bool bSinglePlayer);
[[noreturn]] void diablo_quit(int exitStatus);
int DiabloMain(int argc, char **argv);
bool TryIconCurs();
void diablo_pause_game();
void diablo_focus_pause();
void diablo_focus_unpause();
bool PressEscKey();
void DisableInputWndProc(uint32_t uMsg, int32_t wParam, int32_t lParam);
void LoadGameLevel(bool firstflag, lvl_entry lvldir);
void game_loop(bool bStartup);
void diablo_color_cyc_logic();

/* rdata */

extern Keymapper keymapper;
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
extern bool debug_mode_key_i;
extern int debug_mode_key_j;
#endif

struct QuickMessage {
	/** Config variable names for quick message */
	const char *const key;
	/** Default quick message */
	const char *const message;
};

constexpr size_t QUICK_MESSAGE_OPTIONS = 4;
extern QuickMessage QuickMessages[QUICK_MESSAGE_OPTIONS];
extern bool gbFriendlyMode;
/**
 * @brief Specifices what game logic step is currently executed
 */
extern GameLogicStep gGameLogicStep;

} // namespace devilution
