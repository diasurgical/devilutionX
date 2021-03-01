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
	Sint32 nSoundVolume; // Movie and SFX volume
	Sint32 nMusicVolume; // Music volume
	bool bWalkingSound;  // Player emits sound when walking

	Sint32 nWidth;             // Render width
	Sint32 nHeight;            // Render height
	bool bFullscreen;          // Run in fullscreen or windowed mode
	bool bUpscale;             // Scale the image after rendering
	bool bFitToScreen;         // Expand the aspect ratio to match the screen
	char szScaleQuality[2];    // See SDL_HINT_RENDER_SCALE_QUALITY
	bool bIntegerScaling;      // Only scale by values divisible by the width and height
	bool bVSync;               // Enable vsync on the output
	bool bBlendedTransparancy; // Use blended transparency rather than stippled
	Sint32 nGammaCorrection;   // Gamma correction level
	bool bColorCycling;        // Enable color cycling animations

	Sint32 nTickRate; // Game play ticks per secound
	bool bJogInTown;  // Enable double walk speed when in town
	bool bGrabInput;  // Do not let the mouse leave the application window
	bool bTheoQuest;  // Enable the Theo quest
	bool bCowQuest;   // Enable the cow quest

	char szBindAddress[129]; // Optionally bind to a specific network interface
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
BOOL PressEscKey();
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
extern BOOL FriendlyMode;

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __DIABLO_H__ */
