/**
 * @file diablo.cpp
 *
 * Implementation of the main game initialization functions.
 */
#include <array>

#include <fmt/format.h>

#include <config.h>

#include "automap.h"
#include "capture.h"
#include "control.h"
#include "cursor.h"
#include "dead.h"
#ifdef _DEBUG
#include "debug.h"
#endif
#include "DiabloUI/diabloui.h"
#include "controls/keymapper.hpp"
#include "doom.h"
#include "drlg_l1.h"
#include "drlg_l2.h"
#include "drlg_l3.h"
#include "drlg_l4.h"
#include "dx.h"
#include "encrypt.h"
#include "engine/cel_sprite.hpp"
#include "engine/load_cel.hpp"
#include "engine/load_file.hpp"
#include "engine/random.hpp"
#include "error.h"
#include "gamemenu.h"
#include "gmenu.h"
#include "help.h"
#include "init.h"
#include "lighting.h"
#include "loadsave.h"
#include "mainmenu.h"
#include "minitext.h"
#include "missiles.h"
#include "movie.h"
#include "multi.h"
#include "nthread.h"
#include "objects.h"
#include "options.h"
#include "pfile.h"
#include "plrmsg.h"
#include "qol/common.h"
#include "qol/itemlabels.h"
#include "restrict.h"
#include "setmaps.h"
#include "stores.h"
#include "storm/storm.h"
#include "themes.h"
#include "town.h"
#include "towners.h"
#include "track.h"
#include "trigs.h"
#include "utils/console.h"
#include "utils/language.h"
#include "utils/paths.h"

#ifndef NOSOUND
#include "sound.h"
#endif
#ifdef GPERF_HEAP_FIRST_GAME_ITERATION
#include <gperftools/heap-profiler.h>
#endif

namespace devilution {

SDL_Window *ghMainWnd;
DWORD glSeedTbl[NUMLEVELS];
dungeon_type gnLevelTypeTbl[NUMLEVELS];
Point MousePosition;
bool gbRunGame;
bool gbRunGameResult;
bool zoomflag;
/** Enable updating of player character, set to false once Diablo dies */
bool gbProcessPlayers;
bool gbLoadGame;
bool cineflag;
int force_redraw;
int PauseMode;
bool gbNestArt;
bool gbBard;
bool gbBarbarian;
bool gbQuietMode = false;
clicktype sgbMouseDown;
bool blockClicks = false;
uint16_t gnTickDelay = 50;
char gszProductName[64] = "DevilutionX vUnknown";
Keymapper keymapper {
	// Workaround: remove once the INI library has been replaced.
	[](const std::string &key, const std::string &value) {
	    SetIniValue("Keymapping", key.c_str(), value.c_str());
	},
	[](const std::string &key) -> std::string {
	    std::array<char, 64> result;
	    if (!GetIniValue("Keymapping", key.c_str(), result.data(), result.size()))
		    return {};
	    return { result.data() };
	}
};
std::array<Keymapper::ActionIndex, 4> quickSpellActionIndexes;

bool gbForceWindowed = false;
bool leveldebug = false;
#ifdef _DEBUG
bool monstdebug = false;
_monster_id DebugMonsters[10];
int debugmonsttypes = 0;
bool visiondebug = false;
int questdebug = -1;
bool debug_mode_key_w = false;
bool debug_mode_key_inverted_v = false;
bool debug_mode_dollar_sign = false;
bool debug_mode_key_i = false;
int debug_mode_key_j = 0;
#endif
/** Specifies whether players are in non-PvP mode. */
bool gbFriendlyMode = true;
GameLogicStep gGameLogicStep = GameLogicStep::None;
QuickMessage QuickMessages[QUICK_MESSAGE_OPTIONS] = {
	{ "QuickMessage1", N_("I need help! Come Here!") },
	{ "QuickMessage2", N_("Follow me.") },
	{ "QuickMessage3", N_("Here's something for you.") },
	{ "QuickMessage4", N_("Now you DIE!") }
};

// Controller support: Actions to run after updating the cursor state.
// Defined in SourceX/controls/plctrls.cpp.
extern void finish_simulated_mouse_clicks(int currentMouseX, int currentMouseY);
extern void plrctrls_after_check_curs_move();
extern void plrctrls_every_frame();
extern void plrctrls_after_game_logic();

namespace {

char gszVersionNumber[64] = "internal version unknown";

// Used for debugging level generation
uint32_t glEndSeed[NUMLEVELS];
uint32_t glMid1Seed[NUMLEVELS];
uint32_t glMid2Seed[NUMLEVELS];
uint32_t glMid3Seed[NUMLEVELS];

bool gbGameLoopStartup;
int setseed;
bool forceSpawn;
bool forceDiablo;
int sgnTimeoutCurs;
bool gbShowIntro = true;
int arrowdebug = 0;
/** To know if these things have been done when we get to the diablo_deinit() function */
bool was_archives_init = false;
/** To know if surfaces have been initialized or not */
bool was_window_init = false;
bool was_ui_init = false;
bool was_snd_init = false;

void StartGame(interface_mode uMsg)
{
	zoomflag = true;
	CalcViewportGeometry();
	cineflag = false;
	InitCursor();
#ifdef _DEBUG
	LoadDebugGFX();
#endif
	assert(ghMainWnd);
	music_stop();
	InitQol();
	ShowProgress(uMsg);
	gmenu_init_menu();
	InitLevelCursor();
	sgnTimeoutCurs = CURSOR_NONE;
	sgbMouseDown = CLICK_NONE;
	track_repeat_walk(false);
}

void FreeGame()
{
	FreeQol();
	FreeControlPan();
	FreeInvGFX();
	FreeGMenu();
	FreeQuestText();
	FreeStoreMem();

	for (auto &player : Players)
		ResetPlayerGFX(player);

	FreeCursor();
#ifdef _DEBUG
	FreeDebugGFX();
#endif
	FreeGameMem();
}

bool ProcessInput()
{
	if (PauseMode == 2) {
		return false;
	}

	plrctrls_every_frame();

	if (!gbIsMultiplayer && gmenu_is_active()) {
		force_redraw |= 1;
		return false;
	}

	if (!gmenu_is_active() && sgnTimeoutCurs == CURSOR_NONE) {
#ifndef USE_SDL1
		finish_simulated_mouse_clicks(MousePosition.x, MousePosition.y);
#endif
		CheckCursMove();
		plrctrls_after_check_curs_move();
		track_process();
	}

	return true;
}

void RunGameLoop(interface_mode uMsg)
{
	WNDPROC saveProc;
	tagMSG msg;

	nthread_ignore_mutex(true);
	StartGame(uMsg);
	assert(ghMainWnd);
	saveProc = SetWindowProc(GM_Game);
	run_delta_info();
	gbRunGame = true;
	gbProcessPlayers = true;
	gbRunGameResult = true;
	force_redraw = 255;
	DrawAndBlit();
	LoadPWaterPalette();
	PaletteFadeIn(8);
	force_redraw = 255;
	gbGameLoopStartup = true;
	nthread_ignore_mutex(false);

#ifdef GPERF_HEAP_FIRST_GAME_ITERATION
	unsigned run_game_iteration = 0;
#endif
	while (gbRunGame) {
		while (FetchMessage(&msg)) {
			if (msg.message == DVL_WM_QUIT) {
				gbRunGameResult = false;
				gbRunGame = false;
				break;
			}
			TranslateMessage(&msg);
			PushMessage(&msg);
		}
		if (!gbRunGame)
			break;
		if (!nthread_has_500ms_passed()) {
			ProcessInput();
			force_redraw |= 1;
			DrawAndBlit();
			continue;
		}
		diablo_color_cyc_logic();
		multi_process_network_packets();
		game_loop(gbGameLoopStartup);
		gbGameLoopStartup = false;
		DrawAndBlit();
#ifdef GPERF_HEAP_FIRST_GAME_ITERATION
		if (run_game_iteration++ == 0)
			HeapProfilerDump("first_game_iteration");
#endif
	}

	if (gbIsMultiplayer) {
		pfile_write_hero(/*writeGameData=*/false, /*clearTables=*/true);
	}

	PaletteFadeOut(8);
	NewCursor(CURSOR_NONE);
	ClearScreenBuffer();
	force_redraw = 255;
	scrollrt_draw_game_screen();
	saveProc = SetWindowProc(saveProc);
	assert(saveProc == GM_Game);
	FreeGame();

	if (cineflag) {
		cineflag = false;
		DoEnding();
	}
}

[[noreturn]] void PrintHelpAndExit()
{
	printInConsole("%s", _(/* TRANSLATORS: Commandline Option */ "Options:\n"));
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "-h, --help", _("Print this message and exit"));
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--version", _("Print the version and exit"));
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--data-dir", _("Specify the folder of diabdat.mpq"));
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--save-dir", _("Specify the folder of save files"));
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--config-dir", _("Specify the location of diablo.ini"));
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--ttf-dir", _("Specify the location of the .ttf font"));
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--ttf-name", _("Specify the name of a custom .ttf font"));
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "-n", _("Skip startup videos"));
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "-f", _("Display frames per second"));
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "-x", _("Run in windowed mode"));
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--verbose", _("Enable verbose logging"));
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--spawn", _("Force spawn mode even if diabdat.mpq is found"));
	printInConsole("%s", _(/* TRANSLATORS: Commandline Option */ "\nHellfire options:\n"));
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--diablo", _("Force diablo mode even if hellfire.mpq is found"));
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--nestart", _("Use alternate nest palette"));
#ifdef _DEBUG
	printInConsole("\nDebug options:\n");
	printInConsole("    %-20s %-30s\n", "-w", "Enable cheats");
	printInConsole("    %-20s %-30s\n", "-$", "Enable god mode");
	printInConsole("    %-20s %-30s\n", "-^", "Enable god mode and debug tools");
	printInConsole("    %-20s %-30s\n", "-v", "Highlight visibility");
	printInConsole("    %-20s %-30s\n", "-i", "Ignore network timeout");
	printInConsole("    %-20s %-30s\n", "-j <##>", "Mausoleum warps to given level");
	printInConsole("    %-20s %-30s\n", "-l <##> <##>", "Start in level as type");
	printInConsole("    %-20s %-30s\n", "-m <##>", "Add debug monster, up to 10 allowed");
	printInConsole("    %-20s %-30s\n", "-q <#>", "Force a certain quest");
	printInConsole("    %-20s %-30s\n", "-r <##########>", "Set map seed");
	printInConsole("    %-20s %-30s\n", "-t <##>", "Set current quest level");
#endif
	printInConsole("%s", _("\nReport bugs at https://github.com/diasurgical/devilutionX/\n"));
	diablo_quit(0);
}

void DiabloParseFlags(int argc, char **argv)
{
	for (int i = 1; i < argc; i++) {
		if (strcasecmp("-h", argv[i]) == 0 || strcasecmp("--help", argv[i]) == 0) {
			PrintHelpAndExit();
		} else if (strcasecmp("--version", argv[i]) == 0) {
			printInConsole("%s v%s\n", PROJECT_NAME, PROJECT_VERSION);
			diablo_quit(0);
		} else if (strcasecmp("--data-dir", argv[i]) == 0) {
			paths::SetBasePath(argv[++i]);
		} else if (strcasecmp("--save-dir", argv[i]) == 0) {
			paths::SetPrefPath(argv[++i]);
		} else if (strcasecmp("--config-dir", argv[i]) == 0) {
			paths::SetConfigPath(argv[++i]);
		} else if (strcasecmp("--lang-dir", argv[i]) == 0) {
			paths::SetLangPath(argv[++i]);
		} else if (strcasecmp("--ttf-dir", argv[i]) == 0) {
			paths::SetTtfPath(argv[++i]);
		} else if (strcasecmp("--ttf-name", argv[i]) == 0) {
			paths::SetTtfName(argv[++i]);
		} else if (strcasecmp("-n", argv[i]) == 0) {
			gbShowIntro = false;
		} else if (strcasecmp("-f", argv[i]) == 0) {
			EnableFrameCount();
		} else if (strcasecmp("-x", argv[i]) == 0) {
			gbForceWindowed = true;
		} else if (strcasecmp("--spawn", argv[i]) == 0) {
			forceSpawn = true;
		} else if (strcasecmp("--diablo", argv[i]) == 0) {
			forceDiablo = true;
		} else if (strcasecmp("--nestart", argv[i]) == 0) {
			gbNestArt = true;
		} else if (strcasecmp("--vanilla", argv[i]) == 0) {
			gbVanilla = true;
		} else if (strcasecmp("--verbose", argv[i]) == 0) {
			SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
#ifdef _DEBUG
		} else if (strcasecmp("-^", argv[i]) == 0) {
			debug_mode_key_inverted_v = true;
		} else if (strcasecmp("-$", argv[i]) == 0) {
			debug_mode_dollar_sign = true;
		} else if (strcasecmp("-i", argv[i]) == 0) {
			debug_mode_key_i = true;
		} else if (strcasecmp("-j", argv[i]) == 0) {
			debug_mode_key_j = SDL_atoi(argv[++i]);
		} else if (strcasecmp("-l", argv[i]) == 0) {
			setlevel = false;
			leveldebug = true;
			leveltype = (dungeon_type)SDL_atoi(argv[++i]);
			currlevel = SDL_atoi(argv[++i]);
			Players[0].plrlevel = currlevel;
		} else if (strcasecmp("-m", argv[i]) == 0) {
			monstdebug = true;
			DebugMonsters[debugmonsttypes++] = (_monster_id)SDL_atoi(argv[++i]);
		} else if (strcasecmp("-q", argv[i]) == 0) {
			questdebug = SDL_atoi(argv[++i]);
		} else if (strcasecmp("-r", argv[i]) == 0) {
			setseed = SDL_atoi(argv[++i]);
		} else if (strcasecmp("-t", argv[i]) == 0) {
			leveldebug = true;
			setlevel = true;
			setlvlnum = (_setlevels)SDL_atoi(argv[++i]);
		} else if (strcasecmp("-v", argv[i]) == 0) {
			visiondebug = true;
		} else if (strcasecmp("-w", argv[i]) == 0) {
			debug_mode_key_w = true;
#endif
		} else {
			printInConsole("%s", fmt::format(_("unrecognized option '{:s}'\n"), argv[i]).c_str());
			PrintHelpAndExit();
		}
	}
}

void DiabloInitScreen()
{
	MousePosition = { gnScreenWidth / 2, gnScreenHeight / 2 };
	if (!sgbControllerActive)
		SetCursorPos(MousePosition.x, MousePosition.y);
	ScrollInfo.tile = { 0, 0 };
	ScrollInfo.offset = { 0, 0 };
	ScrollInfo._sdir = SDIR_NONE;

	ClrDiabloMsg();
}

void SetApplicationVersions()
{
	snprintf(gszProductName, sizeof(gszProductName) / sizeof(char), "%s v%s", PROJECT_NAME, PROJECT_VERSION);
	strncpy(gszVersionNumber, fmt::format(_("version {:s}"), PROJECT_VERSION).c_str(), sizeof(gszVersionNumber) / sizeof(char));
}

void DiabloInit()
{
	if (sgOptions.Graphics.bShowFPS)
		EnableFrameCount();

	init_create_window();
	was_window_init = true;

	SFileEnableDirectAccess(true);
	init_archives();
	was_archives_init = true;

	if (forceSpawn)
		gbIsSpawn = true;
	if (forceDiablo)
		gbIsHellfire = false;

	gbIsHellfireSaveGame = gbIsHellfire;

	LanguageInitialize();

	SetApplicationVersions();

	for (size_t i = 0; i < QUICK_MESSAGE_OPTIONS; i++) {
		if (strlen(sgOptions.Chat.szHotKeyMsgs[i]) != 0) {
			continue;
		}
		strncpy(sgOptions.Chat.szHotKeyMsgs[i], _(QuickMessages[i].message), MAX_SEND_STR_LEN);
	}

	UiInitialize();
	UiSetSpawned(gbIsSpawn);
	was_ui_init = true;

	ReadOnlyTest();

	InitHash();

	DiabloInitScreen();

#ifndef NOSOUND
	snd_init();
	was_snd_init = true;
#endif

	ui_sound_init();

	// Item graphics are loaded early, they already get touched during hero selection.
	InitItemGFX();
}

void DiabloSplash()
{
	if (!gbShowIntro)
		return;

	play_movie("gendata\\logo.smk", true);

	if (gbIsHellfire && sgOptions.Hellfire.bIntro) {
		play_movie("gendata\\Hellfire.smk", true);
		sgOptions.Hellfire.bIntro = false;
	}
	if (!gbIsHellfire && !gbIsSpawn && sgOptions.Diablo.bIntro) {
		play_movie("gendata\\diablo1.smk", true);
		sgOptions.Diablo.bIntro = false;
	}

	UiTitleDialog();
}

void DiabloDeinit()
{
	FreeItemGFX();

	if (sbWasOptionsLoaded)
		SaveOptions();
	if (was_snd_init) {
		effects_cleanup_sfx();
	}
#ifndef NOSOUND
	Aulib::quit();
#endif
	if (was_ui_init)
		UiDestroy();
	if (was_archives_init)
		init_cleanup();
	if (was_window_init)
		dx_cleanup(); // Cleanup SDL surfaces stuff, so we have to do it before SDL_Quit().
	if (was_fonts_init)
		FontsCleanup();
	if (SDL_WasInit(SDL_INIT_EVERYTHING & ~SDL_INIT_HAPTIC) != 0)
		SDL_Quit();
}

bool LeftMouseCmd(bool bShift)
{
	bool bNear;

	assert(MousePosition.y < PANEL_TOP || MousePosition.x < PANEL_LEFT || MousePosition.x >= PANEL_LEFT + PANEL_WIDTH);

	if (leveltype == DTYPE_TOWN) {
		if (pcursitem != -1 && pcurs == CURSOR_HAND)
			NetSendCmdLocParam1(true, invflag ? CMD_GOTOGETITEM : CMD_GOTOAGETITEM, { cursmx, cursmy }, pcursitem);
		if (pcursmonst != -1)
			NetSendCmdLocParam1(true, CMD_TALKXY, { cursmx, cursmy }, pcursmonst);
		if (pcursitem == -1 && pcursmonst == -1 && pcursplr == -1)
			return true;
	} else {
		auto &myPlayer = Players[MyPlayerId];
		bNear = myPlayer.position.tile.WalkingDistance({ cursmx, cursmy }) < 2;
		if (pcursitem != -1 && pcurs == CURSOR_HAND && !bShift) {
			NetSendCmdLocParam1(true, invflag ? CMD_GOTOGETITEM : CMD_GOTOAGETITEM, { cursmx, cursmy }, pcursitem);
		} else if (pcursobj != -1 && (!objectIsDisabled(pcursobj)) && (!bShift || (bNear && Objects[pcursobj]._oBreak == 1))) {
			NetSendCmdLocParam1(true, pcurs == CURSOR_DISARM ? CMD_DISARMXY : CMD_OPOBJXY, { cursmx, cursmy }, pcursobj);
		} else if (myPlayer._pwtype == WT_RANGED) {
			if (bShift) {
				NetSendCmdLoc(MyPlayerId, true, CMD_RATTACKXY, { cursmx, cursmy });
			} else if (pcursmonst != -1) {
				if (CanTalkToMonst(pcursmonst)) {
					NetSendCmdParam1(true, CMD_ATTACKID, pcursmonst);
				} else {
					NetSendCmdParam1(true, CMD_RATTACKID, pcursmonst);
				}
			} else if (pcursplr != -1 && !gbFriendlyMode) {
				NetSendCmdParam1(true, CMD_RATTACKPID, pcursplr);
			}
		} else {
			if (bShift) {
				if (pcursmonst != -1) {
					if (CanTalkToMonst(pcursmonst)) {
						NetSendCmdParam1(true, CMD_ATTACKID, pcursmonst);
					} else {
						NetSendCmdLoc(MyPlayerId, true, CMD_SATTACKXY, { cursmx, cursmy });
					}
				} else {
					NetSendCmdLoc(MyPlayerId, true, CMD_SATTACKXY, { cursmx, cursmy });
				}
			} else if (pcursmonst != -1) {
				NetSendCmdParam1(true, CMD_ATTACKID, pcursmonst);
			} else if (pcursplr != -1 && !gbFriendlyMode) {
				NetSendCmdParam1(true, CMD_ATTACKPID, pcursplr);
			}
		}
		if (!bShift && pcursitem == -1 && pcursobj == -1 && pcursmonst == -1 && pcursplr == -1)
			return true;
	}

	return false;
}

void DiabloHotkeyMsg(DWORD dwMsg)
{
	if (!gbIsMultiplayer) {
		return;
	}

	assert(dwMsg < QUICK_MESSAGE_OPTIONS);

	NetSendCmdString(0xFFFFFF, sgOptions.Chat.szHotKeyMsgs[dwMsg]);
}

bool PressSysKey(int wParam)
{
	if (gmenu_is_active() || wParam != DVL_VK_F10)
		return false;
	DiabloHotkeyMsg(1);
	return true;
}

void ReleaseKey(int vkey)
{
	if (vkey == DVL_VK_SNAPSHOT)
		CaptureScreen();
	if (vkey == DVL_VK_MENU || vkey == DVL_VK_LMENU || vkey == DVL_VK_RMENU)
		AltPressed(false);
	if (vkey == DVL_VK_CONTROL || vkey == DVL_VK_LCONTROL || vkey == DVL_VK_RCONTROL)
		ToggleItemLabelHighlight();
}

void ClosePanels()
{
	if (CanPanelsCoverView()) {
		if (!chrflag && !QuestLogIsOpen && (invflag || sbookflag) && MousePosition.x < 480 && MousePosition.y < PANEL_TOP) {
			SetCursorPos(MousePosition.x + 160, MousePosition.y);
		} else if (!invflag && !sbookflag && (chrflag || QuestLogIsOpen) && MousePosition.x > 160 && MousePosition.y < PANEL_TOP) {
			SetCursorPos(MousePosition.x - 160, MousePosition.y);
		}
	}
	invflag = false;
	chrflag = false;
	sbookflag = false;
	QuestLogIsOpen = false;
}

void PressKey(int vkey)
{
	if (gmenu_presskeys(vkey) || control_presskeys(vkey)) {
		return;
	}

	if (vkey == DVL_VK_MENU || vkey == DVL_VK_LMENU || vkey == DVL_VK_RMENU)
		AltPressed(true);

	if (MyPlayerIsDead) {
		if (sgnTimeoutCurs != CURSOR_NONE) {
			return;
		}
		keymapper.KeyPressed(vkey);
		if (vkey == DVL_VK_RETURN) {
			if (GetAsyncKeyState(DVL_VK_MENU))
				dx_reinit();
			else
				control_type_message();
		}
		if (vkey != DVL_VK_ESCAPE) {
			return;
		}
	}
	if (vkey == DVL_VK_ESCAPE) {
		if (!PressEscKey()) {
			track_repeat_walk(false);
			gamemenu_on();
		}
		return;
	}

	if (sgnTimeoutCurs != CURSOR_NONE || dropGoldFlag) {
		return;
	}
	if (vkey == DVL_VK_PAUSE) {
		diablo_pause_game();
		return;
	}
	if (PauseMode == 2) {
		if (vkey == DVL_VK_RETURN && GetAsyncKeyState(DVL_VK_MENU))
			dx_reinit();
		return;
	}

	keymapper.KeyPressed(vkey);

	if (vkey == DVL_VK_RETURN) {
		if (GetAsyncKeyState(DVL_VK_MENU)) {
			dx_reinit();
		} else if (stextflag != STORE_NONE) {
			StoreEnter();
		} else if (QuestLogIsOpen) {
			QuestlogEnter();
		} else {
			control_type_message();
		}
	} else if (vkey == DVL_VK_UP) {
		if (stextflag != STORE_NONE) {
			StoreUp();
		} else if (QuestLogIsOpen) {
			QuestlogUp();
		} else if (HelpFlag) {
			HelpScrollUp();
		} else if (AutomapActive) {
			AutomapUp();
		}
	} else if (vkey == DVL_VK_DOWN) {
		if (stextflag != STORE_NONE) {
			StoreDown();
		} else if (QuestLogIsOpen) {
			QuestlogDown();
		} else if (HelpFlag) {
			HelpScrollDown();
		} else if (AutomapActive) {
			AutomapDown();
		}
	} else if (vkey == DVL_VK_PRIOR) {
		if (stextflag != STORE_NONE) {
			StorePrior();
		}
	} else if (vkey == DVL_VK_NEXT) {
		if (stextflag != STORE_NONE) {
			StoreNext();
		}
	} else if (vkey == DVL_VK_LEFT) {
		if (AutomapActive && !talkflag) {
			AutomapLeft();
		}
	} else if (vkey == DVL_VK_RIGHT) {
		if (AutomapActive && !talkflag) {
			AutomapRight();
		}
	} else if (vkey == DVL_VK_TAB) {
		DoAutoMap();
	} else if (vkey == DVL_VK_SPACE) {
		ClosePanels();
		HelpFlag = false;
		spselflag = false;
		if (qtextflag && leveltype == DTYPE_TOWN) {
			qtextflag = false;
			stream_stop();
		}
		AutomapActive = false;
		msgdelay = 0;
		gamemenu_off();
		doom_close();
	}
}

/**
 * @internal `return` must be used instead of `break` to be bin exact as C++
 */
void PressChar(int32_t vkey)
{
	if (gmenu_is_active() || control_talk_last_key(vkey) || sgnTimeoutCurs != CURSOR_NONE || MyPlayerIsDead) {
		return;
	}
	if ((char)vkey == 'p' || (char)vkey == 'P') {
		diablo_pause_game();
		return;
	}
	if (PauseMode == 2) {
		return;
	}
	if (DoomFlag) {
		doom_close();
		return;
	}
	if (dropGoldFlag) {
		control_drop_gold(vkey);
		return;
	}

	switch (vkey) {
	case '+':
	case '=':
		if (AutomapActive) {
			AutomapZoomIn();
		}
		return;
	case '-':
	case '_':
		if (AutomapActive) {
			AutomapZoomOut();
		}
		return;
#ifdef _DEBUG
	case ')':
	case '0':
		if (debug_mode_key_inverted_v) {
			if (arrowdebug > 2) {
				arrowdebug = 0;
			}
			auto &myPlayer = Players[MyPlayerId];
			if (arrowdebug == 0) {
				myPlayer._pIFlags &= ~ISPL_FIRE_ARROWS;
				myPlayer._pIFlags &= ~ISPL_LIGHT_ARROWS;
			}
			if (arrowdebug == 1) {
				myPlayer._pIFlags |= ISPL_FIRE_ARROWS;
			}
			if (arrowdebug == 2) {
				myPlayer._pIFlags |= ISPL_LIGHT_ARROWS;
			}
			arrowdebug++;
		}
		return;
	case ':':
		if (currlevel == 0 && debug_mode_key_w) {
			SetAllSpellsCheat();
		}
		return;
	case '[':
		if (currlevel == 0 && debug_mode_key_w) {
			TakeGoldCheat();
		}
		return;
	case ']':
		if (currlevel == 0 && debug_mode_key_w) {
			MaxSpellsCheat();
		}
		return;
	case 'a':
		if (debug_mode_key_inverted_v) {
			spelldata[SPL_TELEPORT].sTownSpell = true;
			auto &myPlayer = Players[MyPlayerId];
			myPlayer._pSplLvl[myPlayer._pSpell]++;
		}
		return;
	case 'D':
		PrintDebugPlayer(true);
		return;
	case 'd':
		PrintDebugPlayer(false);
		return;
	case 'L':
	case 'l':
		if (debug_mode_key_inverted_v) {
			ToggleLighting();
		}
		return;
	case 'M':
		NextDebugMonster();
		return;
	case 'm':
		GetDebugMonster();
		return;
	case 'R':
	case 'r':
		sprintf(tempstr, "seed = %i", glSeedTbl[currlevel]);
		NetSendCmdString(1 << MyPlayerId, tempstr);
		sprintf(tempstr, "Mid1 = %i : Mid2 = %i : Mid3 = %i", glMid1Seed[currlevel], glMid2Seed[currlevel], glMid3Seed[currlevel]);
		NetSendCmdString(1 << MyPlayerId, tempstr);
		sprintf(tempstr, "End = %i", glEndSeed[currlevel]);
		NetSendCmdString(1 << MyPlayerId, tempstr);
		return;
	case 'T':
	case 't':
		if (debug_mode_key_inverted_v) {
			auto &myPlayer = Players[MyPlayerId];
			sprintf(tempstr, "PX = %i  PY = %i", myPlayer.position.tile.x, myPlayer.position.tile.y);
			NetSendCmdString(1 << MyPlayerId, tempstr);
			sprintf(tempstr, "CX = %i  CY = %i  DP = %i", cursmx, cursmy, dungeon[cursmx][cursmy]);
			NetSendCmdString(1 << MyPlayerId, tempstr);
		}
		return;
	case '|':
		if (currlevel == 0 && debug_mode_key_w) {
			GiveGoldCheat();
		}
		return;
#endif
	}
}

void GetMousePos(int32_t lParam)
{
	MousePosition = { (std::int16_t)(lParam & 0xffff), (std::int16_t)((lParam >> 16) & 0xffff) };
}

void LoadLvlGFX()
{
	assert(pDungeonCels == nullptr);
	constexpr int SpecialCelWidth = 64;

	switch (leveltype) {
	case DTYPE_TOWN:
		if (gbIsHellfire) {
			pDungeonCels = LoadFileInMem("NLevels\\TownData\\Town.CEL");
			pMegaTiles = LoadFileInMem<MegaTile>("NLevels\\TownData\\Town.TIL");
			pLevelPieces = LoadFileInMem<uint16_t>("NLevels\\TownData\\Town.MIN");
		} else {
			pDungeonCels = LoadFileInMem("Levels\\TownData\\Town.CEL");
			pMegaTiles = LoadFileInMem<MegaTile>("Levels\\TownData\\Town.TIL");
			pLevelPieces = LoadFileInMem<uint16_t>("Levels\\TownData\\Town.MIN");
		}
		pSpecialCels = LoadCel("Levels\\TownData\\TownS.CEL", SpecialCelWidth);
		break;
	case DTYPE_CATHEDRAL:
		if (currlevel < 21) {
			pDungeonCels = LoadFileInMem("Levels\\L1Data\\L1.CEL");
			pMegaTiles = LoadFileInMem<MegaTile>("Levels\\L1Data\\L1.TIL");
			pLevelPieces = LoadFileInMem<uint16_t>("Levels\\L1Data\\L1.MIN");
			pSpecialCels = LoadCel("Levels\\L1Data\\L1S.CEL", SpecialCelWidth);
		} else {
			pDungeonCels = LoadFileInMem("NLevels\\L5Data\\L5.CEL");
			pMegaTiles = LoadFileInMem<MegaTile>("NLevels\\L5Data\\L5.TIL");
			pLevelPieces = LoadFileInMem<uint16_t>("NLevels\\L5Data\\L5.MIN");
			pSpecialCels = LoadCel("NLevels\\L5Data\\L5S.CEL", SpecialCelWidth);
		}
		break;
	case DTYPE_CATACOMBS:
		pDungeonCels = LoadFileInMem("Levels\\L2Data\\L2.CEL");
		pMegaTiles = LoadFileInMem<MegaTile>("Levels\\L2Data\\L2.TIL");
		pLevelPieces = LoadFileInMem<uint16_t>("Levels\\L2Data\\L2.MIN");
		pSpecialCels = LoadCel("Levels\\L2Data\\L2S.CEL", SpecialCelWidth);
		break;
	case DTYPE_CAVES:
		if (currlevel < 17) {
			pDungeonCels = LoadFileInMem("Levels\\L3Data\\L3.CEL");
			pMegaTiles = LoadFileInMem<MegaTile>("Levels\\L3Data\\L3.TIL");
			pLevelPieces = LoadFileInMem<uint16_t>("Levels\\L3Data\\L3.MIN");
		} else {
			pDungeonCels = LoadFileInMem("NLevels\\L6Data\\L6.CEL");
			pMegaTiles = LoadFileInMem<MegaTile>("NLevels\\L6Data\\L6.TIL");
			pLevelPieces = LoadFileInMem<uint16_t>("NLevels\\L6Data\\L6.MIN");
		}
		pSpecialCels = LoadCel("Levels\\L1Data\\L1S.CEL", SpecialCelWidth);
		break;
	case DTYPE_HELL:
		pDungeonCels = LoadFileInMem("Levels\\L4Data\\L4.CEL");
		pMegaTiles = LoadFileInMem<MegaTile>("Levels\\L4Data\\L4.TIL");
		pLevelPieces = LoadFileInMem<uint16_t>("Levels\\L4Data\\L4.MIN");
		pSpecialCels = LoadCel("Levels\\L2Data\\L2S.CEL", SpecialCelWidth);
		break;
	default:
		app_fatal("LoadLvlGFX");
	}
}

void LoadAllGFX()
{
	IncProgress();
	IncProgress();
	InitObjectGFX();
	IncProgress();
	InitMissileGFX();
	IncProgress();
}

/**
 * @param lvldir method of entry
 */
void CreateLevel(lvl_entry lvldir)
{
	switch (leveltype) {
	case DTYPE_TOWN:
		CreateTown(lvldir);
		InitTownTriggers();
		LoadRndLvlPal(DTYPE_TOWN);
		break;
	case DTYPE_CATHEDRAL:
		CreateL5Dungeon(glSeedTbl[currlevel], lvldir);
		InitL1Triggers();
		Freeupstairs();
		if (currlevel < 21) {
			LoadRndLvlPal(DTYPE_CATHEDRAL);
		} else {
			LoadRndLvlPal(DTYPE_CRYPT);
		}
		break;
	case DTYPE_CATACOMBS:
		CreateL2Dungeon(glSeedTbl[currlevel], lvldir);
		InitL2Triggers();
		Freeupstairs();
		LoadRndLvlPal(DTYPE_CATACOMBS);
		break;
	case DTYPE_CAVES:
		CreateL3Dungeon(glSeedTbl[currlevel], lvldir);
		InitL3Triggers();
		Freeupstairs();
		if (currlevel < 17) {
			LoadRndLvlPal(DTYPE_CAVES);
		} else {
			LoadRndLvlPal(DTYPE_NEST);
		}
		break;
	case DTYPE_HELL:
		CreateL4Dungeon(glSeedTbl[currlevel], lvldir);
		InitL4Triggers();
		Freeupstairs();
		LoadRndLvlPal(DTYPE_HELL);
		break;
	default:
		app_fatal("CreateLevel");
	}
}

void UpdateMonsterLights()
{
	for (int i = 0; i < ActiveMonsterCount; i++) {
		MonsterStruct *mon = &Monsters[ActiveMonsters[i]];
		if (mon->mlid != NO_LIGHT) {
			if (mon->mlid == Players[MyPlayerId]._plid) { // Fix old saves where some monsters had 0 instead of NO_LIGHT
				mon->mlid = NO_LIGHT;
				continue;
			}

			LightStruct *lid = &Lights[mon->mlid];
			if (mon->position.tile != lid->position.tile) {
				ChangeLightXY(mon->mlid, mon->position.tile);
			}
		}
	}
}

void GameLogic()
{
	if (!ProcessInput()) {
		return;
	}
	if (gbProcessPlayers) {
		gGameLogicStep = GameLogicStep::ProcessPlayers;
		ProcessPlayers();
	}
	if (leveltype != DTYPE_TOWN) {
		gGameLogicStep = GameLogicStep::ProcessMonsters;
		ProcessMonsters();
		gGameLogicStep = GameLogicStep::ProcessObjects;
		ProcessObjects();
		gGameLogicStep = GameLogicStep::ProcessMissiles;
		ProcessMissiles();
		gGameLogicStep = GameLogicStep::ProcessItems;
		ProcessItems();
		ProcessLightList();
		ProcessVisionList();
	} else {
		gGameLogicStep = GameLogicStep::ProcessTowners;
		ProcessTowners();
		gGameLogicStep = GameLogicStep::ProcessItemsTown;
		ProcessItems();
		gGameLogicStep = GameLogicStep::ProcessMissilesTown;
		ProcessMissiles();
	}
	gGameLogicStep = GameLogicStep::None;

#ifdef _DEBUG
	if (debug_mode_key_inverted_v && GetAsyncKeyState(DVL_VK_SHIFT)) {
		ScrollView();
	}
#endif

	sound_update();
	ClearPlrMsg();
	CheckTriggers();
	CheckQuests();
	force_redraw |= 1;
	pfile_update(false);

	plrctrls_after_game_logic();
}

void TimeoutCursor(bool bTimeout)
{
	if (bTimeout) {
		if (sgnTimeoutCurs == CURSOR_NONE && sgbMouseDown == CLICK_NONE) {
			sgnTimeoutCurs = pcurs;
			multi_net_ping();
			ClearPanel();
			AddPanelString(_("-- Network timeout --"));
			AddPanelString(_("-- Waiting for players --"));
			NewCursor(CURSOR_HOURGLASS);
			force_redraw = 255;
		}
		scrollrt_draw_game_screen();
	} else if (sgnTimeoutCurs != CURSOR_NONE) {
		NewCursor(sgnTimeoutCurs);
		sgnTimeoutCurs = CURSOR_NONE;
		ClearPanel();
		force_redraw = 255;
	}
}

void HelpKeyPressed()
{
	if (HelpFlag) {
		HelpFlag = false;
	} else if (stextflag != STORE_NONE) {
		ClearPanel();
		AddPanelString(_("No help available")); /// BUGFIX: message isn't displayed
		AddPanelString(_("while in stores"));
		track_repeat_walk(false);
	} else {
		invflag = false;
		chrflag = false;
		sbookflag = false;
		spselflag = false;
		if (qtextflag && leveltype == DTYPE_TOWN) {
			qtextflag = false;
			stream_stop();
		}
		QuestLogIsOpen = false;
		AutomapActive = false;
		msgdelay = 0;
		gamemenu_off();
		DisplayHelp();
		doom_close();
	}
}

#ifdef _DEBUG
void ItemInfoKeyPressed()
{
	if (pcursitem != -1) {
		sprintf(
		    tempstr,
		    "IDX = %i  :  Seed = %i  :  CF = %i",
		    Items[pcursitem].IDidx,
		    Items[pcursitem]._iSeed,
		    Items[pcursitem]._iCreateInfo);
		NetSendCmdString(1 << MyPlayerId, tempstr);
	}
	sprintf(tempstr, "Numitems : %i", ActiveItemCount);
	NetSendCmdString(1 << MyPlayerId, tempstr);
}
#endif

void InventoryKeyPressed()
{
	if (stextflag != STORE_NONE)
		return;
	invflag = !invflag;
	if (!chrflag && !QuestLogIsOpen && CanPanelsCoverView()) {
		if (!invflag) { // We closed the invetory
			if (MousePosition.x < 480 && MousePosition.y < PANEL_TOP) {
				SetCursorPos(MousePosition.x + 160, MousePosition.y);
			}
		} else if (!sbookflag) { // We opened the invetory
			if (MousePosition.x > 160 && MousePosition.y < PANEL_TOP) {
				SetCursorPos(MousePosition.x - 160, MousePosition.y);
			}
		}
	}
	sbookflag = false;
}

void CharacterSheetKeyPressed()
{
	if (stextflag != STORE_NONE)
		return;
	chrflag = !chrflag;
	if (!invflag && !sbookflag && CanPanelsCoverView()) {
		if (!chrflag) { // We closed the character sheet
			if (MousePosition.x > 160 && MousePosition.y < PANEL_TOP) {
				SetCursorPos(MousePosition.x - 160, MousePosition.y);
			}
		} else if (!QuestLogIsOpen) { // We opened the character sheet
			if (MousePosition.x < 480 && MousePosition.y < PANEL_TOP) {
				SetCursorPos(MousePosition.x + 160, MousePosition.y);
			}
		}
	}
	QuestLogIsOpen = false;
}

void QuestLogKeyPressed()
{
	if (stextflag != STORE_NONE)
		return;
	if (!QuestLogIsOpen) {
		StartQuestlog();
	} else {
		QuestLogIsOpen = false;
	}
	if (!invflag && !sbookflag && CanPanelsCoverView()) {
		if (!QuestLogIsOpen) { // We closed the quest log
			if (MousePosition.x > 160 && MousePosition.y < PANEL_TOP) {
				SetCursorPos(MousePosition.x - 160, MousePosition.y);
			}
		} else if (!chrflag) { // We opened the character quest log
			if (MousePosition.x < 480 && MousePosition.y < PANEL_TOP) {
				SetCursorPos(MousePosition.x + 160, MousePosition.y);
			}
		}
	}
	chrflag = false;
}

void DisplaySpellsKeyPressed()
{
	if (stextflag != STORE_NONE)
		return;
	chrflag = false;
	QuestLogIsOpen = false;
	invflag = false;
	sbookflag = false;
	if (!spselflag) {
		DoSpeedBook();
	} else {
		spselflag = false;
	}
	track_repeat_walk(false);
}

void SpellBookKeyPressed()
{
	if (stextflag != STORE_NONE)
		return;
	sbookflag = !sbookflag;
	if (!chrflag && !QuestLogIsOpen && CanPanelsCoverView()) {
		if (!sbookflag) { // We closed the invetory
			if (MousePosition.x < 480 && MousePosition.y < PANEL_TOP) {
				SetCursorPos(MousePosition.x + 160, MousePosition.y);
			}
		} else if (!invflag) { // We opened the invetory
			if (MousePosition.x > 160 && MousePosition.y < PANEL_TOP) {
				SetCursorPos(MousePosition.x - 160, MousePosition.y);
			}
		}
	}
	invflag = false;
}

bool IsPlayerDead()
{
	return Players[MyPlayerId]._pmode == PM_DEATH || MyPlayerIsDead;
}

void InitKeymapActions()
{
	keymapper.AddAction({
	    "Help",
	    DVL_VK_F1,
	    HelpKeyPressed,
	    [&]() { return !IsPlayerDead(); },
	});
#ifdef _DEBUG
	keymapper.AddAction({
	    "ItemInfo",
	    DVL_VK_INVALID,
	    ItemInfoKeyPressed,
	    [&]() { return !IsPlayerDead(); },
	});
	keymapper.AddAction({
	    "QuestDebug",
	    DVL_VK_INVALID,
	    PrintDebugQuest,
	    [&]() { return !IsPlayerDead(); },
	});
#endif
	for (int i = 0; i < 4; ++i) {
		quickSpellActionIndexes[i] = keymapper.AddAction({
		    std::string("QuickSpell") + std::to_string(i + 1),
		    DVL_VK_F5 + i,
		    [i]() {
			    if (spselflag) {
				    SetSpeedSpell(i);
				    return;
			    }
			    ToggleSpell(i);
		    },
		    [&]() { return !IsPlayerDead(); },
		});
	}
	for (int i = 0; i < 4; ++i) {
		keymapper.AddAction({
		    QuickMessages[i].key,
		    DVL_VK_F9 + i,
		    [i]() { DiabloHotkeyMsg(i); },
		});
	}
	keymapper.AddAction({
	    "DecreaseGamma",
	    'G',
	    DecreaseGamma,
	    [&]() { return !IsPlayerDead(); },
	});
	keymapper.AddAction({
	    "IncreaseGamma",
	    'F',
	    IncreaseGamma,
	    [&]() { return !IsPlayerDead(); },
	});
	keymapper.AddAction({
	    "Inventory",
	    'I',
	    InventoryKeyPressed,
	    [&]() { return !IsPlayerDead(); },
	});
	keymapper.AddAction({
	    "Character",
	    'C',
	    CharacterSheetKeyPressed,
	    [&]() { return !IsPlayerDead(); },
	});
	keymapper.AddAction({
	    "QuestLog",
	    'Q',
	    QuestLogKeyPressed,
	    [&]() { return !IsPlayerDead(); },
	});
	keymapper.AddAction({
	    "Zoom",
	    'Z',
	    [] {
		    zoomflag = !zoomflag;
		    CalcViewportGeometry();
	    },
	    [&]() { return !IsPlayerDead(); },
	});
	keymapper.AddAction({
	    "DisplaySpells",
	    'S',
	    DisplaySpellsKeyPressed,
	    [&]() { return !IsPlayerDead(); },
	});
	keymapper.AddAction({
	    "SpellBook",
	    'B',
	    SpellBookKeyPressed,
	    [&]() { return !IsPlayerDead(); },
	});
	keymapper.AddAction({
	    "GameInfo",
	    'V',
	    [] {
		    char pszStr[120];
		    const char *difficulties[3] = {
			    _("Normal"),
			    _("Nightmare"),
			    _("Hell"),
		    };
		    strcpy(pszStr, fmt::format(_(/* TRANSLATORS: {:s} means: Character Name, Game Version, Game Difficulty. */
		                                   "{:s}, version = {:s}, mode = {:s}"),
		                       gszProductName, PROJECT_VERSION, difficulties[sgGameInitInfo.nDifficulty])
		                       .c_str());
		    NetSendCmdString(1 << MyPlayerId, pszStr);
	    },
	    [&]() { return !IsPlayerDead(); },
	});
	for (int i = 0; i < 8; ++i) {
		keymapper.AddAction({
		    std::string("BeltItem") + std::to_string(i + 1),
		    '1' + i,
		    [i] {
			    auto &myPlayer = Players[MyPlayerId];
			    if (!myPlayer.SpdList[i].isEmpty() && myPlayer.SpdList[i]._itype != ITYPE_GOLD) {
				    UseInvItem(MyPlayerId, INVITEM_BELT_FIRST + i);
			    }
		    },
		    [&]() { return !IsPlayerDead(); },
		});
	}
	keymapper.AddAction({
	    "QuickSave",
	    DVL_VK_F2,
	    [] { gamemenu_save_game(false); },
	    [&]() { return !gbIsMultiplayer && !IsPlayerDead(); },
	});
	keymapper.AddAction({
	    "QuickLoad",
	    DVL_VK_F3,
	    [] { gamemenu_load_game(false); },
	    [&]() { return !gbIsMultiplayer && gbValidSaveFile; },
	});
	keymapper.AddAction({
	    "QuitGame",
	    DVL_VK_INVALID,
	    [] { gamemenu_quit_game(false); },
	});
#ifdef _DEBUG
	keymapper.AddAction({
	    "CheatExperience",
	    DVL_VK_INVALID,
	    [] {
		    if (debug_mode_key_inverted_v || debug_mode_key_w) {
			    NetSendCmd(true, CMD_CHEAT_EXPERIENCE);
			    return;
		    }
	    },
	    [&]() { return !IsPlayerDead(); },
	});
#endif
	keymapper.AddAction({
	    "StopHero",
	    DVL_VK_INVALID,
	    [] { Players[MyPlayerId].Stop(); },
	    [&]() { return !IsPlayerDead(); },
	});
}

} // namespace

bool LeftMouseDown(int wParam)
{
	if (gmenu_left_mouse(true))
		return false;

	if (control_check_talk_btn())
		return false;

	if (sgnTimeoutCurs != CURSOR_NONE)
		return false;

	if (MyPlayerIsDead) {
		control_check_btn_press();
		return false;
	}

	if (PauseMode == 2) {
		return false;
	}
	if (DoomFlag) {
		doom_close();
		return false;
	}

	if (spselflag) {
		SetSpell();
		return false;
	}

	if (stextflag != STORE_NONE) {
		CheckStoreBtn();
		return false;
	}

	bool isShiftHeld = (wParam & DVL_MK_SHIFT) != 0;

	if (MousePosition.y < PANEL_TOP || MousePosition.x < PANEL_LEFT || MousePosition.x >= PANEL_LEFT + PANEL_WIDTH) {
		if (!gmenu_is_active() && !TryIconCurs()) {
			if (QuestLogIsOpen && MousePosition.x > 32 && MousePosition.x < 288 && MousePosition.y > 32 && MousePosition.y < 308) {
				QuestlogESC();
			} else if (qtextflag) {
				qtextflag = false;
				stream_stop();
			} else if (chrflag && MousePosition.x < SPANEL_WIDTH && MousePosition.y < SPANEL_HEIGHT) {
				CheckChrBtns();
			} else if (invflag && MousePosition.x > RIGHT_PANEL && MousePosition.y < SPANEL_HEIGHT) {
				if (!dropGoldFlag)
					CheckInvItem(isShiftHeld);
			} else if (sbookflag && MousePosition.x > RIGHT_PANEL && MousePosition.y < SPANEL_HEIGHT) {
				CheckSBook();
			} else if (pcurs >= CURSOR_FIRSTITEM) {
				if (TryInvPut()) {
					NetSendCmdPItem(true, CMD_PUTITEM, { cursmx, cursmy });
					NewCursor(CURSOR_HAND);
				}
			} else {
				if (Players[MyPlayerId]._pStatPts != 0 && !spselflag)
					CheckLvlBtn();
				if (!lvlbtndown)
					return LeftMouseCmd(isShiftHeld);
			}
		}
	} else {
		if (!talkflag && !dropGoldFlag && !gmenu_is_active())
			CheckInvScrn(isShiftHeld);
		DoPanBtn();
		if (pcurs > CURSOR_HAND && pcurs < CURSOR_FIRSTITEM)
			NewCursor(CURSOR_HAND);
	}

	return false;
}

void LeftMouseUp(int wParam)
{
	gmenu_left_mouse(false);
	control_release_talk_btn();
	bool isShiftHeld = (wParam & (DVL_MK_SHIFT | DVL_MK_LBUTTON)) != 0;
	if (panbtndown)
		CheckBtnUp();
	if (chrbtnactive)
		ReleaseChrBtns(isShiftHeld);
	if (lvlbtndown)
		ReleaseLvlBtn();
	if (stextflag != STORE_NONE)
		ReleaseStoreBtn();
}

void RightMouseDown()
{
	if (gmenu_is_active() || sgnTimeoutCurs != CURSOR_NONE || PauseMode == 2 || Players[MyPlayerId]._pInvincible) {
		return;
	}

	if (DoomFlag) {
		doom_close();
		return;
	}
	if (stextflag != STORE_NONE)
		return;
	if (spselflag) {
		SetSpell();
		return;
	}
	if (MousePosition.y >= SPANEL_HEIGHT
	    || ((!sbookflag || MousePosition.x <= RIGHT_PANEL)
	        && !TryIconCurs()
	        && (pcursinvitem == -1 || !UseInvItem(MyPlayerId, pcursinvitem)))) {
		if (pcurs == CURSOR_HAND) {
			if (pcursinvitem == -1 || !UseInvItem(MyPlayerId, pcursinvitem))
				CheckPlrSpell();
		} else if (pcurs > CURSOR_HAND && pcurs < CURSOR_FIRSTITEM) {
			NewCursor(CURSOR_HAND);
		}
	}
}

void FreeGameMem()
{
	music_stop();

	pDungeonCels = nullptr;
	pMegaTiles = nullptr;
	pLevelPieces = nullptr;
	pSpecialCels = std::nullopt;

	FreeMissiles();
	FreeMonsters();
	FreeObjectGFX();
	FreeMonsterSnd();
	FreeTownerGFX();
}

bool StartGame(bool bNewGame, bool bSinglePlayer)
{
	gbSelectProvider = true;

	do {
		gbLoadGame = false;

		if (!NetInit(bSinglePlayer)) {
			gbRunGameResult = true;
			break;
		}

		// Save 2.8 MiB of RAM by freeing all main menu resources
		// before starting the game.
		UiDestroy();

		gbSelectProvider = false;

		if (bNewGame || !gbValidSaveFile) {
			InitLevels();
			InitQuests();
			InitPortals();
			InitDungMsgs(Players[MyPlayerId]);
		}
		interface_mode uMsg = WM_DIABNEWGAME;
		if (gbValidSaveFile && gbLoadGame) {
			uMsg = WM_DIABLOADGAME;
		}
		RunGameLoop(uMsg);
		NetClose();

		// If the player left the game into the main menu,
		// initialize main menu resources.
		if (gbRunGameResult)
			UiInitialize();
	} while (gbRunGameResult);

	SNetDestroy();
	return gbRunGameResult;
}

void diablo_quit(int exitStatus)
{
	DiabloDeinit();
	exit(exitStatus);
}

int DiabloMain(int argc, char **argv)
{
#ifdef _DEBUG
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
#endif

	DiabloParseFlags(argc, argv);
	InitKeymapActions();
	LoadOptions();
	DiabloInit();
	DiabloSplash();
	mainmenu_loop();
	DiabloDeinit();

	return 0;
}

bool TryIconCurs()
{
	if (pcurs == CURSOR_RESURRECT) {
		NetSendCmdParam1(true, CMD_RESURRECT, pcursplr);
		return true;
	}

	if (pcurs == CURSOR_HEALOTHER) {
		NetSendCmdParam1(true, CMD_HEALOTHER, pcursplr);
		return true;
	}

	if (pcurs == CURSOR_TELEKINESIS) {
		DoTelekinesis();
		return true;
	}

	if (pcurs == CURSOR_IDENTIFY) {
		if (pcursinvitem != -1)
			CheckIdentify(MyPlayerId, pcursinvitem);
		else
			NewCursor(CURSOR_HAND);
		return true;
	}

	if (pcurs == CURSOR_REPAIR) {
		if (pcursinvitem != -1)
			DoRepair(MyPlayerId, pcursinvitem);
		else
			NewCursor(CURSOR_HAND);
		return true;
	}

	if (pcurs == CURSOR_RECHARGE) {
		if (pcursinvitem != -1)
			DoRecharge(MyPlayerId, pcursinvitem);
		else
			NewCursor(CURSOR_HAND);
		return true;
	}

	if (pcurs == CURSOR_OIL) {
		if (pcursinvitem != -1)
			DoOil(MyPlayerId, pcursinvitem);
		else
			NewCursor(CURSOR_HAND);
		return true;
	}

	if (pcurs == CURSOR_TELEPORT) {
		auto &myPlayer = Players[MyPlayerId];
		if (pcursmonst != -1)
			NetSendCmdParam3(true, CMD_TSPELLID, pcursmonst, myPlayer._pTSpell, GetSpellLevel(MyPlayerId, myPlayer._pTSpell));
		else if (pcursplr != -1)
			NetSendCmdParam3(true, CMD_TSPELLPID, pcursplr, myPlayer._pTSpell, GetSpellLevel(MyPlayerId, myPlayer._pTSpell));
		else
			NetSendCmdLocParam2(true, CMD_TSPELLXY, { cursmx, cursmy }, myPlayer._pTSpell, GetSpellLevel(MyPlayerId, myPlayer._pTSpell));
		NewCursor(CURSOR_HAND);
		return true;
	}

	if (pcurs == CURSOR_DISARM && pcursobj == -1) {
		NewCursor(CURSOR_HAND);
		return true;
	}

	return false;
}

void diablo_pause_game()
{
	if (!gbIsMultiplayer) {
		if (PauseMode != 0) {
			PauseMode = 0;
		} else {
			PauseMode = 2;
			sound_stop();
			track_repeat_walk(false);
		}
		force_redraw = 255;
	}
}

bool PressEscKey()
{
	bool rv = false;

	if (DoomFlag) {
		doom_close();
		rv = true;
	}

	if (HelpFlag) {
		HelpFlag = false;
		rv = true;
	}

	if (qtextflag) {
		qtextflag = false;
		stream_stop();
		rv = true;
	}

	if (stextflag != STORE_NONE) {
		StoreESC();
		rv = true;
	}

	if (msgflag != EMSG_NONE) {
		msgdelay = 0;
		rv = true;
	}

	if (talkflag) {
		control_reset_talk();
		rv = true;
	}

	if (dropGoldFlag) {
		control_drop_gold(DVL_VK_ESCAPE);
		rv = true;
	}

	if (spselflag) {
		spselflag = false;
		rv = true;
	}

	if (invflag || chrflag || sbookflag || QuestLogIsOpen) {
		ClosePanels();
		rv = true;
	}

	return rv;
}

void DisableInputWndProc(uint32_t uMsg, int32_t /*wParam*/, int32_t lParam)
{
	switch (uMsg) {
	case DVL_WM_KEYDOWN:
	case DVL_WM_KEYUP:
	case DVL_WM_CHAR:
	case DVL_WM_SYSKEYDOWN:
	case DVL_WM_SYSCOMMAND:
		return;
	case DVL_WM_MOUSEMOVE:
		GetMousePos(lParam);
		return;
	case DVL_WM_LBUTTONDOWN:
		if (sgbMouseDown != CLICK_NONE)
			return;
		sgbMouseDown = CLICK_LEFT;
		return;
	case DVL_WM_LBUTTONUP:
		if (sgbMouseDown != CLICK_LEFT)
			return;
		sgbMouseDown = CLICK_NONE;
		return;
	case DVL_WM_RBUTTONDOWN:
		if (sgbMouseDown != CLICK_NONE)
			return;
		sgbMouseDown = CLICK_RIGHT;
		return;
	case DVL_WM_RBUTTONUP:
		if (sgbMouseDown != CLICK_RIGHT)
			return;
		sgbMouseDown = CLICK_NONE;
		return;
	case DVL_WM_CAPTURECHANGED:
		sgbMouseDown = CLICK_NONE;
		return;
	}

	MainWndProc(uMsg);
}

void GM_Game(uint32_t uMsg, int32_t wParam, int32_t lParam)
{
	switch (uMsg) {
	case DVL_WM_KEYDOWN:
		PressKey(wParam);
		return;
	case DVL_WM_KEYUP:
		ReleaseKey(wParam);
		return;
	case DVL_WM_CHAR:
		PressChar(wParam);
		return;
	case DVL_WM_SYSKEYDOWN:
		if (PressSysKey(wParam))
			return;
		break;
	case DVL_WM_SYSCOMMAND:
		if (wParam == DVL_SC_CLOSE) {
			gbRunGame = false;
			gbRunGameResult = false;
			return;
		}
		break;
	case DVL_WM_MOUSEMOVE:
		GetMousePos(lParam);
		gmenu_on_mouse_move();
		return;
	case DVL_WM_LBUTTONDOWN:
		blockClicks = false;
		GetMousePos(lParam);
		if (sgbMouseDown == CLICK_NONE) {
			sgbMouseDown = CLICK_LEFT;
			track_repeat_walk(LeftMouseDown(wParam));
		}
		return;
	case DVL_WM_LBUTTONUP:
		GetMousePos(lParam);
		if (sgbMouseDown == CLICK_LEFT) {
			sgbMouseDown = CLICK_NONE;
			LeftMouseUp(wParam);
			track_repeat_walk(false);
		}
		return;
	case DVL_WM_RBUTTONDOWN:
		blockClicks = false;
		GetMousePos(lParam);
		if (sgbMouseDown == CLICK_NONE) {
			sgbMouseDown = CLICK_RIGHT;
			RightMouseDown();
		}
		return;
	case DVL_WM_RBUTTONUP:
		GetMousePos(lParam);
		if (sgbMouseDown == CLICK_RIGHT) {
			sgbMouseDown = CLICK_NONE;
		}
		return;
	case DVL_WM_CAPTURECHANGED:
		sgbMouseDown = CLICK_NONE;
		track_repeat_walk(false);
		break;
	case WM_DIABNEXTLVL:
	case WM_DIABPREVLVL:
	case WM_DIABRTNLVL:
	case WM_DIABSETLVL:
	case WM_DIABWARPLVL:
	case WM_DIABTOWNWARP:
	case WM_DIABTWARPUP:
	case WM_DIABRETOWN:
		if (gbIsMultiplayer)
			pfile_write_hero();
		nthread_ignore_mutex(true);
		PaletteFadeOut(8);
		sound_stop();
		music_stop();
		track_repeat_walk(false);
		sgbMouseDown = CLICK_NONE;
		ShowProgress((interface_mode)uMsg);
		force_redraw = 255;
		DrawAndBlit();
		LoadPWaterPalette();
		if (gbRunGame)
			PaletteFadeIn(8);
		nthread_ignore_mutex(false);
		gbGameLoopStartup = true;
		return;
	}

	MainWndProc(uMsg);
}

void LoadGameLevel(bool firstflag, lvl_entry lvldir)
{
	if (setseed != 0)
		glSeedTbl[currlevel] = setseed;

	music_stop();
	if (pcurs > CURSOR_HAND && pcurs < CURSOR_FIRSTITEM) {
		NewCursor(CURSOR_HAND);
	}
	SetRndSeed(glSeedTbl[currlevel]);
	IncProgress();
	MakeLightTable();
	LoadLvlGFX();
	IncProgress();

	if (firstflag) {
		InitInv();
		InitQuestText();
		InitStores();
		InitAutomapOnce();
		InitHelp();
		InitText();
	}

	SetRndSeed(glSeedTbl[currlevel]);

	if (leveltype == DTYPE_TOWN)
		SetupTownStores();

	IncProgress();
	InitAutomap();

	if (leveltype != DTYPE_TOWN && lvldir != ENTRY_LOAD) {
		InitLighting();
		InitVision();
	}

	InitLevelMonsters();
	IncProgress();

	auto &myPlayer = Players[MyPlayerId];

	if (!setlevel) {
		CreateLevel(lvldir);
		IncProgress();
		FillSolidBlockTbls();
		SetRndSeed(glSeedTbl[currlevel]);

		if (leveltype != DTYPE_TOWN) {
			GetLevelMTypes();
			InitThemes();
			LoadAllGFX();
		} else {
			IncProgress();
			IncProgress();
			InitMissileGFX();
			IncProgress();
			IncProgress();
		}

		IncProgress();

		if (lvldir == ENTRY_RTNLVL)
			GetReturnLvlPos();
		if (lvldir == ENTRY_WARPLVL)
			GetPortalLvlPos();

		IncProgress();

		for (int i = 0; i < MAX_PLRS; i++) {
			auto &player = Players[i];
			if (player.plractive && currlevel == player.plrlevel) {
				InitPlayerGFX(player);
				if (lvldir != ENTRY_LOAD)
					InitPlayer(i, firstflag);
			}
		}

		PlayDungMsgs();
		InitMultiView();
		IncProgress();

		bool visited = false;
		int players = gbIsMultiplayer ? MAX_PLRS : 1;
		for (int i = 0; i < players; i++) {
			auto &player = Players[i];
			if (player.plractive)
				visited = visited || player._pLvlVisited[currlevel];
		}

		SetRndSeed(glSeedTbl[currlevel]);

		if (leveltype != DTYPE_TOWN) {
			if (firstflag || lvldir == ENTRY_LOAD || !myPlayer._pLvlVisited[currlevel] || gbIsMultiplayer) {
				HoldThemeRooms();
				glMid1Seed[currlevel] = GetLCGEngineState();
				InitMonsters();
				glMid2Seed[currlevel] = GetLCGEngineState();
				IncProgress();
				InitObjects();
				InitItems();
				if (currlevel < 17)
					CreateThemeRooms();
				IncProgress();
				glMid3Seed[currlevel] = GetLCGEngineState();
				InitMissiles();
				InitDead();
				glEndSeed[currlevel] = GetLCGEngineState();

				if (gbIsMultiplayer)
					DeltaLoadLevel();

				IncProgress();
				SavePreLighting();
			} else {
				HoldThemeRooms();
				InitMonsters();
				InitMissiles();
				InitDead();
				IncProgress();
				LoadLevel();
				IncProgress();
			}
		} else {
			for (int i = 0; i < MAXDUNX; i++) { // NOLINT(modernize-loop-convert)
				for (int j = 0; j < MAXDUNY; j++) {
					dFlags[i][j] |= BFLAG_LIT;
				}
			}

			InitTowners();
			InitItems();
			InitMissiles();
			IncProgress();

			if (!firstflag && lvldir != ENTRY_LOAD && myPlayer._pLvlVisited[currlevel] && !gbIsMultiplayer)
				LoadLevel();
			if (gbIsMultiplayer)
				DeltaLoadLevel();

			IncProgress();
		}
		if (!gbIsMultiplayer)
			ResyncQuests();
		else
			ResyncMPQuests();
	} else {
		LoadSetMap();
		IncProgress();
		GetLevelMTypes();
		IncProgress();
		InitMonsters();
		IncProgress();
		InitMissileGFX();
		IncProgress();
		InitDead();
		IncProgress();
		FillSolidBlockTbls();
		IncProgress();

		if (lvldir == ENTRY_WARPLVL)
			GetPortalLvlPos();
		IncProgress();

		for (int i = 0; i < MAX_PLRS; i++) {
			auto &player = Players[i];
			if (player.plractive && currlevel == player.plrlevel) {
				InitPlayerGFX(player);
				if (lvldir != ENTRY_LOAD)
					InitPlayer(i, firstflag);
			}
		}
		IncProgress();

		InitMultiView();
		IncProgress();

		if (firstflag || lvldir == ENTRY_LOAD || !myPlayer._pSLvlVisited[setlvlnum]) {
			InitItems();
			SavePreLighting();
		} else {
			LoadLevel();
		}

		InitMissiles();
		IncProgress();
	}

	SyncPortals();

	for (int i = 0; i < MAX_PLRS; i++) {
		auto &player = Players[i];
		if (player.plractive && player.plrlevel == currlevel && (!player._pLvlChanging || i == MyPlayerId)) {
			if (player._pHitPoints > 0) {
				if (!gbIsMultiplayer)
					dPlayer[player.position.tile.x][player.position.tile.y] = i + 1;
				else
					SyncInitPlrPos(i);
			} else {
				dFlags[player.position.tile.x][player.position.tile.y] |= BFLAG_DEAD_PLAYER;
			}
		}
	}

	SetDungeonMicros();

	InitLightMax();
	IncProgress();
	IncProgress();

	if (firstflag) {
		InitControlPan();
	}
	IncProgress();
	UpdateMonsterLights();
	if (leveltype != DTYPE_TOWN) {
		ProcessLightList();
		ProcessVisionList();
	}

	if (currlevel >= 21) {
		if (currlevel == 21) {
			CornerstoneLoad(CornerStone.position);
		}
		if (Quests[Q_NAKRUL]._qactive == QUEST_DONE && currlevel == 24) {
			SyncNakrulRoom();
		}
	}

	if (currlevel >= 17)
		music_start(currlevel > 20 ? TMUSIC_L5 : TMUSIC_L6);
	else
		music_start(leveltype);

	while (!IncProgress())
		;

	if (!gbIsSpawn && setlevel && setlvlnum == SL_SKELKING && Quests[Q_SKELKING]._qactive == QUEST_ACTIVE)
		PlaySFX(USFX_SKING1);
}

/**
 * @param bStartup Process additional ticks before returning
 */
void game_loop(bool bStartup)
{
	uint16_t wait = bStartup ? sgGameInitInfo.nTickRate * 3 : 3;

	for (unsigned i = 0; i < wait; i++) {
		if (!multi_handle_delta()) {
			TimeoutCursor(true);
			break;
		}
		TimeoutCursor(false);
		GameLogic();

		if (!gbRunGame || !gbIsMultiplayer || !nthread_has_500ms_passed())
			break;
	}
}

void diablo_color_cyc_logic()
{
	if (!sgOptions.Graphics.bColorCycling)
		return;

	if (leveltype == DTYPE_HELL) {
		lighting_color_cycling();
	} else if (currlevel >= 21) {
		palette_update_crypt();
	} else if (currlevel >= 17) {
		palette_update_hive();
	} else if (setlevel && setlvlnum == Quests[Q_PWATER]._qslvl) {
		UpdatePWaterPalette();
	} else if (leveltype == DTYPE_CAVES) {
		palette_update_caves();
	}
}

} // namespace devilution
