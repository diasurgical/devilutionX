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

#ifndef DEFAULT_WIDTH
#define DEFAULT_WIDTH 640
#endif
#ifndef DEFAULT_HEIGHT
#define DEFAULT_HEIGHT 480
#endif

typedef struct Options {
	/** @brief Movie and SFX volume. */
	Sint32 nSoundVolume;

	/** @brief Music volume. */
	Sint32 nMusicVolume;

	/** @brief Player emits sound when walking. */
	bool bWalkingSound;

	/** @brief Render width. */
	Sint32 nWidth;

	/** @brief Render height. */
	Sint32 nHeight;

	/** @brief Run in fullscreen or windowed mode. */
	bool bFullscreen;

	/** @brief Scale the image after rendering. */
	bool bUpscale;

	/** @brief Expand the aspect ratio to match the screen. */
	bool bFitToScreen;

	/** @brief See SDL_HINT_RENDER_SCALE_QUALITY. */
	char szScaleQuality[2];

	/** @brief Only scale by values divisible by the width and height. */
	bool bIntegerScaling;

	/** @brief Enable vsync on the output. */
	bool bVSync;

	/** @brief Use blended transparency rather than stippled. */
	bool bBlendedTransparancy;

	/** @brief Gamma correction level. */
	Sint32 nGammaCorrection;

	/** @brief Enable color cycling animations. */
	bool bColorCycling;

	/** @brief Game play ticks per secound. */
	Sint32 nTickRate;

	/** @brief Enable double walk speed when in town. */
	bool bJogInTown;

	/** @brief Do not let the mouse leave the application window. */
	bool bGrabInput;

	/** @brief Enable the Theo quest. */
	bool bTheoQuest;

	/** @brief Enable the cow quest. */
	bool bCowQuest;

	/** @brief Will players still damage other players in non-PvP mode. */
	bool bFriendlyFire;

	/** @brief Enable the bard hero class. */
	bool bTestBard;

	/** @brief Enable the babarian hero class. */
	bool bTestBarbarian;

	/** @brief Show the current level progress. */
	bool bExperienceBar;

	/** @brief Show enemy health at the top of the screen. */
	bool bEnemyHealthBar;

	/** @brief Automatically pick up goald when walking on to it. */
	bool bAutoGoldPickup;

	/** @brief Recover mana when talking to Adria. */
	bool bAdriaRefillsMana;

	/** @brief Automatically attempt to equip weapon-type items when picking them up. */
	bool bAutoEquipWeapons;

	/** @brief Automatically attempt to equip armor-type items when picking them up. */
	bool bAutoEquipArmor;

	/** @brief Automatically attempt to equip helm-type items when picking them up. */
	bool bAutoEquipHelms;

	/** @brief Automatically attempt to equip shield-type items when picking them up. */
	bool bAutoEquipShields;

	/** @brief Automatically attempt to equip jewelry-type items when picking them up. */
	bool bAutoEquipJewelry;

	/** @brief Optionally bind to a specific network interface. */
	char szBindAddress[129];
} Options;

extern SDL_Window *ghMainWnd;
extern DWORD glSeedTbl[NUMLEVELS];
extern int gnLevelTypeTbl[NUMLEVELS];
extern int MouseX;
extern int MouseY;
extern BOOL gbRunGame;
extern BOOL gbRunGameResult;
extern BOOL zoomflag;
extern BOOL gbProcessPlayers;
extern BOOL gbLoadGame;
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
extern Options sgOptions;

void FreeGameMem();
BOOL StartGame(BOOL bNewGame, BOOL bSinglePlayer);
void diablo_quit(int exitStatus);
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
extern BOOL leveldebug;
#ifdef _DEBUG
extern BOOL monstdebug;
extern int debugmonsttypes;
extern int DebugMonsters[10];
extern BOOL visiondebug;
extern int questdebug;
extern int debug_mode_key_w;
extern int debug_mode_key_inverted_v;
extern int debug_mode_dollar_sign;
extern int debug_mode_key_d;
extern int debug_mode_key_i;
extern int dbgplr;
extern int dbgqst;
extern int dbgmon;
#endif
extern bool gbFriendlyMode;
extern bool gbFriendlyFire;

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __DIABLO_H__ */
