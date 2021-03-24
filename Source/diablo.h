/**
 * @file diablo.h
 *
 * Interface of the main game initialization functions.
 */
#ifndef __DIABLO_H__
#define __DIABLO_H__

#include "pack.h"
#ifdef _DEBUG
#include "monstdat.h"
#endif

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

typedef struct DiabloOptions {
	/** @brief Play game intro video on startup. */
	bool bInto;
} DiabloOptions;

typedef struct HellfireOptions {
	/** @brief Play game intro video on startup. */
	bool bInto;
	/** @brief Corner stone of the world item. */
	char szItem[sizeof(PkItemStruct) * 2 + 1];
} HellfireOptions;

typedef struct AudioOptions {
	/** @brief Movie and SFX volume. */
	Sint32 nSoundVolume;
	/** @brief Music volume. */
	Sint32 nMusicVolume;
	/** @brief Player emits sound when walking. */
	bool bWalkingSound;
	/** @brief Automatically equipping items on pickup emits the equipment sound. */
	bool bAutoEquipSound;
} AudioOptions;

typedef struct GraphicsOptions {
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
	/** @brief Enable FPS Limit. */
	bool bFPSLimit;
} GraphicsOptions;

typedef struct GameplayOptions {
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
	/** @brief Only enable 2/3 quests in each game sessoin */
	bool bRandomizeQuests;
	/** @brief Indicates whether or not mosnter type (Animal, Demon, Undead) is shown along with other monster information. */
	bool bShowMonsterType;
} GameplayOptions;

typedef struct ControllerOptions {
	/** @brief SDL Controller mapping, see SDL_GameControllerDB. */
	char szMapping[1024];
	/** @brief Use dpad for spell hotkeys without holding "start" */
	bool bDpadHotkeys;
	/** @brief Shoulder gamepad shoulder buttons act as potions by default */
	bool bSwapShoulderButtonMode;
#ifdef __vita__
	/** @brief Enable input via rear touchpad */
	bool bRearTouch;
#endif
} ControllerOptions;

typedef struct NetworkOptions {
	/** @brief Optionally bind to a specific network interface. */
	char szBindAddress[129];
	/** @brief Most recently entered Hostname in join dialog. */
	char szPreviousHost[129];
	/** @brief What network port to use. */
	Uint16 nPort;
} NetworkOptions;

typedef struct ChatOptions {
	/** @brief Quick chat messages. */
	char szHotKeyMsgs[4][MAX_SEND_STR_LEN];
} ChatOptions;

typedef struct Options {
	DiabloOptions Diablo;
	HellfireOptions Hellfire;
	AudioOptions Audio;
	GameplayOptions Gameplay;
	GraphicsOptions Graphics;
	ControllerOptions Controller;
	NetworkOptions Network;
	ChatOptions Chat;
} Options;

extern SDL_Window *ghMainWnd;
extern DWORD glSeedTbl[NUMLEVELS];
extern dungeon_type gnLevelTypeTbl[NUMLEVELS];
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

DEVILUTION_END_NAMESPACE

#endif /* __DIABLO_H__ */
