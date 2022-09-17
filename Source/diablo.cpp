/**
 * @file diablo.cpp
 *
 * Implementation of the main game initialization functions.
 */
#include <array>

#include <fmt/format.h>

#include <config.h>

#include "DiabloUI/selstart.h"
#include "automap.h"
#include "capture.h"
#include "cursor.h"
#include "dead.h"
#ifdef _DEBUG
#include "debug.h"
#include "miniwin/misc_msg.h"
#endif
#include "DiabloUI/diabloui.h"
#include "controls/plrctrls.h"
#include "diablo.h"
#include "discord/discord.h"
#include "doom.h"
#include "encrypt.h"
#include "engine/clx_sprite.hpp"
#include "engine/demomode.h"
#include "engine/dx.h"
#include "engine/load_cel.hpp"
#include "engine/load_file.hpp"
#include "engine/random.hpp"
#include "engine/sound.h"
#include "error.h"
#include "gamemenu.h"
#include "gmenu.h"
#include "help.h"
#include "hwcursor.hpp"
#include "init.h"
#include "levels/drlg_l1.h"
#include "levels/drlg_l2.h"
#include "levels/drlg_l3.h"
#include "levels/drlg_l4.h"
#include "levels/gendung.h"
#include "levels/setmaps.h"
#include "levels/themes.h"
#include "levels/town.h"
#include "levels/trigs.h"
#include "lighting.h"
#include "loadsave.h"
#include "menu.h"
#include "minitext.h"
#include "missiles.h"
#include "movie.h"
#include "multi.h"
#include "nthread.h"
#include "objects.h"
#include "options.h"
#include "panels/info_box.hpp"
#include "panels/spell_book.hpp"
#include "panels/spell_list.hpp"
#include "pfile.h"
#include "plrmsg.h"
#include "qol/chatlog.h"
#include "qol/itemlabels.h"
#include "qol/monhealthbar.h"
#include "qol/stash.h"
#include "qol/xpbar.h"
#include "restrict.h"
#include "stores.h"
#include "storm/storm_net.hpp"
#include "storm/storm_svid.h"
#include "towners.h"
#include "track.h"
#include "utils/console.h"
#include "utils/display.h"
#include "utils/language.h"
#include "utils/paths.h"
#include "utils/stdcompat/string_view.hpp"
#include "utils/str_cat.hpp"
#include "utils/utf8.hpp"

#ifndef USE_SDL1
#include "controls/touch/gamepad.h"
#include "controls/touch/renderers.h"
#endif

#ifdef __vita__
#include "platform/vita/touch.h"
#endif

#ifdef GPERF_HEAP_FIRST_GAME_ITERATION
#include <gperftools/heap-profiler.h>
#endif

namespace devilution {

uint32_t glSeedTbl[NUMLEVELS];
Point MousePosition;
bool gbRunGame;
bool gbRunGameResult;
bool ReturnToMainMenu;
/** Enable updating of player character, set to false once Diablo dies */
bool gbProcessPlayers;
bool gbLoadGame;
bool cineflag;
int force_redraw;
int PauseMode;
bool gbBard;
bool gbBarbarian;
bool HeadlessMode = false;
clicktype sgbMouseDown;
uint16_t gnTickDelay = 50;
char gszProductName[64] = "DevilutionX vUnknown";

#ifdef _DEBUG
bool DebugDisableNetworkTimeout = false;
std::vector<std::string> DebugCmdsFromCommandLine;
#endif
GameLogicStep gGameLogicStep = GameLogicStep::None;
QuickMessage QuickMessages[QUICK_MESSAGE_OPTIONS] = {
	{ "QuickMessage1", N_("I need help! Come Here!") },
	{ "QuickMessage2", N_("Follow me.") },
	{ "QuickMessage3", N_("Here's something for you.") },
	{ "QuickMessage4", N_("Now you DIE!") }
};

/** This and the following mouse variables are for handling in-game click-and-hold actions */
MouseActionType LastMouseButtonAction = MouseActionType::None;

// Controller support: Actions to run after updating the cursor state.
// Defined in SourceX/controls/plctrls.cpp.
extern void plrctrls_after_check_curs_move();
extern void plrctrls_every_frame();
extern void plrctrls_after_game_logic();

namespace {

char gszVersionNumber[64] = "internal version unknown";

bool gbGameLoopStartup;
bool forceSpawn;
bool forceDiablo;
int sgnTimeoutCurs;
bool gbShowIntro = true;
/** To know if these things have been done when we get to the diablo_deinit() function */
bool was_archives_init = false;
/** To know if surfaces have been initialized or not */
bool was_window_init = false;
bool was_ui_init = false;

void StartGame(interface_mode uMsg)
{
	CalcViewportGeometry();
	cineflag = false;
	InitCursor();
#ifdef _DEBUG
	LoadDebugGFX();
#endif
	assert(HeadlessMode || ghMainWnd);
	music_stop();
	InitMonsterHealthBar();
	InitXPBar();
	ShowProgress(uMsg);
	gmenu_init_menu();
	InitLevelCursor();
	sgnTimeoutCurs = CURSOR_NONE;
	sgbMouseDown = CLICK_NONE;
	LastMouseButtonAction = MouseActionType::None;
}

void FreeGame()
{
	FreeMonsterHealthBar();
	FreeXPBar();
	FreeControlPan();
	FreeInvGFX();
	FreeStashGFX();
	FreeGMenu();
	FreeQuestText();
	FreeInfoBoxGfx();
	FreeStoreMem();

	for (Player &player : Players)
		ResetPlayerGFX(player);

	FreeCursor();
#ifdef _DEBUG
	FreeDebugGFX();
#endif
	FreeGameMem();
	music_stop();
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
#ifdef __vita__
		FinishSimulatedMouseClicks(MousePosition);
#endif
		CheckCursMove();
		plrctrls_after_check_curs_move();
		RepeatMouseAction();
	}

	return true;
}

void LeftMouseCmd(bool bShift)
{
	bool bNear;

	assert(!GetMainPanel().contains(MousePosition));

	if (leveltype == DTYPE_TOWN) {
		CloseGoldWithdraw();
		IsStashOpen = false;
		if (pcursitem != -1 && pcurs == CURSOR_HAND)
			NetSendCmdLocParam1(true, invflag ? CMD_GOTOGETITEM : CMD_GOTOAGETITEM, cursPosition, pcursitem);
		if (pcursmonst != -1)
			NetSendCmdLocParam1(true, CMD_TALKXY, cursPosition, pcursmonst);
		if (pcursitem == -1 && pcursmonst == -1 && pcursplr == -1) {
			LastMouseButtonAction = MouseActionType::Walk;
			NetSendCmdLoc(MyPlayerId, true, CMD_WALKXY, cursPosition);
		}
		return;
	}

	Player &myPlayer = *MyPlayer;
	bNear = myPlayer.position.tile.WalkingDistance(cursPosition) < 2;
	if (pcursitem != -1 && pcurs == CURSOR_HAND && !bShift) {
		NetSendCmdLocParam1(true, invflag ? CMD_GOTOGETITEM : CMD_GOTOAGETITEM, cursPosition, pcursitem);
	} else if (ObjectUnderCursor != nullptr && !ObjectUnderCursor->IsDisabled() && (!bShift || (bNear && ObjectUnderCursor->_oBreak == 1))) {
		LastMouseButtonAction = MouseActionType::OperateObject;
		NetSendCmdLoc(MyPlayerId, true, pcurs == CURSOR_DISARM ? CMD_DISARMXY : CMD_OPOBJXY, cursPosition);
	} else if (myPlayer.UsesRangedWeapon()) {
		if (bShift) {
			LastMouseButtonAction = MouseActionType::Attack;
			NetSendCmdLoc(MyPlayerId, true, CMD_RATTACKXY, cursPosition);
		} else if (pcursmonst != -1) {
			if (CanTalkToMonst(Monsters[pcursmonst])) {
				NetSendCmdParam1(true, CMD_ATTACKID, pcursmonst);
			} else {
				LastMouseButtonAction = MouseActionType::AttackMonsterTarget;
				NetSendCmdParam1(true, CMD_RATTACKID, pcursmonst);
			}
		} else if (pcursplr != -1 && !myPlayer.friendlyMode) {
			LastMouseButtonAction = MouseActionType::AttackPlayerTarget;
			NetSendCmdParam1(true, CMD_RATTACKPID, pcursplr);
		}
	} else {
		if (bShift) {
			if (pcursmonst != -1) {
				if (CanTalkToMonst(Monsters[pcursmonst])) {
					NetSendCmdParam1(true, CMD_ATTACKID, pcursmonst);
				} else {
					LastMouseButtonAction = MouseActionType::Attack;
					NetSendCmdLoc(MyPlayerId, true, CMD_SATTACKXY, cursPosition);
				}
			} else {
				LastMouseButtonAction = MouseActionType::Attack;
				NetSendCmdLoc(MyPlayerId, true, CMD_SATTACKXY, cursPosition);
			}
		} else if (pcursmonst != -1) {
			LastMouseButtonAction = MouseActionType::AttackMonsterTarget;
			NetSendCmdParam1(true, CMD_ATTACKID, pcursmonst);
		} else if (pcursplr != -1 && !myPlayer.friendlyMode) {
			LastMouseButtonAction = MouseActionType::AttackPlayerTarget;
			NetSendCmdParam1(true, CMD_ATTACKPID, pcursplr);
		}
	}
	if (!bShift && pcursitem == -1 && ObjectUnderCursor == nullptr && pcursmonst == -1 && pcursplr == -1) {
		LastMouseButtonAction = MouseActionType::Walk;
		NetSendCmdLoc(MyPlayerId, true, CMD_WALKXY, cursPosition);
	}
}

void LeftMouseDown(uint16_t modState)
{
	LastMouseButtonAction = MouseActionType::None;

	if (gmenu_left_mouse(true))
		return;

	if (control_check_talk_btn())
		return;

	if (sgnTimeoutCurs != CURSOR_NONE)
		return;

	if (MyPlayerIsDead) {
		control_check_btn_press();
		return;
	}

	if (PauseMode == 2) {
		return;
	}
	if (DoomFlag) {
		doom_close();
		return;
	}

	if (spselflag) {
		SetSpell();
		return;
	}

	if (stextflag != STORE_NONE) {
		CheckStoreBtn();
		return;
	}

	const bool isShiftHeld = (modState & KMOD_SHIFT) != 0;
	const bool isCtrlHeld = (modState & KMOD_CTRL) != 0;

	if (!GetMainPanel().contains(MousePosition)) {
		if (!gmenu_is_active() && !TryIconCurs()) {
			if (QuestLogIsOpen && GetLeftPanel().contains(MousePosition)) {
				QuestlogESC();
			} else if (qtextflag) {
				qtextflag = false;
				stream_stop();
			} else if (chrflag && GetLeftPanel().contains(MousePosition)) {
				CheckChrBtns();
			} else if (invflag && GetRightPanel().contains(MousePosition)) {
				if (!dropGoldFlag)
					CheckInvItem(isShiftHeld, isCtrlHeld);
			} else if (IsStashOpen && GetLeftPanel().contains(MousePosition)) {
				if (!IsWithdrawGoldOpen)
					CheckStashItem(MousePosition, isShiftHeld, isCtrlHeld);
				CheckStashButtonPress(MousePosition);
			} else if (sbookflag && GetRightPanel().contains(MousePosition)) {
				CheckSBook();
			} else if (!MyPlayer->HoldItem.isEmpty()) {
				Point currentPosition = MyPlayer->position.tile;
				if (FindAdjacentPositionForItem(currentPosition, GetDirection(currentPosition, cursPosition))) {
					NetSendCmdPItem(true, CMD_PUTITEM, cursPosition, MyPlayer->HoldItem);
					NewCursor(CURSOR_HAND);
				}
			} else {
				CheckLvlBtn();
				if (!lvlbtndown)
					LeftMouseCmd(isShiftHeld);
			}
		}
	} else {
		if (!talkflag && !dropGoldFlag && !IsWithdrawGoldOpen && !gmenu_is_active())
			CheckInvScrn(isShiftHeld, isCtrlHeld);
		DoPanBtn();
		CheckStashButtonPress(MousePosition);
		if (pcurs > CURSOR_HAND && pcurs < CURSOR_FIRSTITEM)
			NewCursor(CURSOR_HAND);
	}
}

void LeftMouseUp(uint16_t modState)
{
	gmenu_left_mouse(false);
	control_release_talk_btn();
	if (panbtndown)
		CheckBtnUp();
	CheckStashButtonRelease(MousePosition);
	if (chrbtnactive) {
		const bool isShiftHeld = (modState & KMOD_SHIFT) != 0;
		ReleaseChrBtns(isShiftHeld);
	}
	if (lvlbtndown)
		ReleaseLvlBtn();
	if (stextflag != STORE_NONE)
		ReleaseStoreBtn();
}

void RightMouseDown(bool isShiftHeld)
{
	LastMouseButtonAction = MouseActionType::None;

	if (gmenu_is_active() || sgnTimeoutCurs != CURSOR_NONE || PauseMode == 2 || MyPlayer->_pInvincible) {
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
	if (sbookflag && GetRightPanel().contains(MousePosition))
		return;
	if (TryIconCurs())
		return;
	if (pcursinvitem != -1 && UseInvItem(MyPlayerId, pcursinvitem))
		return;
	if (pcursstashitem != uint16_t(-1) && UseStashItem(pcursstashitem))
		return;
	if (pcurs == CURSOR_HAND) {
		CheckPlrSpell(isShiftHeld);
	} else if (pcurs > CURSOR_HAND && pcurs < CURSOR_FIRSTITEM) {
		NewCursor(CURSOR_HAND);
	}
}

void ReleaseKey(int vkey)
{
	if (sgnTimeoutCurs != CURSOR_NONE || dropGoldFlag)
		return;
	sgOptions.Keymapper.KeyReleased(vkey);
}

void ClosePanels()
{
	if (CanPanelsCoverView()) {
		if (!IsLeftPanelOpen() && IsRightPanelOpen() && MousePosition.x < 480 && MousePosition.y < GetMainPanel().position.y) {
			SetCursorPos(MousePosition + Displacement { 160, 0 });
		} else if (!IsRightPanelOpen() && IsLeftPanelOpen() && MousePosition.x > 160 && MousePosition.y < GetMainPanel().position.y) {
			SetCursorPos(MousePosition - Displacement { 160, 0 });
		}
	}
	CloseInventory();
	chrflag = false;
	sbookflag = false;
	QuestLogIsOpen = false;
}

void PressKey(SDL_Keycode vkey, uint16_t modState)
{
	if (vkey == SDLK_UNKNOWN)
		return;

	if (vkey == SDLK_PAUSE) {
		diablo_pause_game();
		return;
	}
	if (gmenu_presskeys(vkey) || control_presskeys(vkey)) {
		return;
	}

	if (MyPlayerIsDead) {
		if (sgnTimeoutCurs != CURSOR_NONE) {
			return;
		}
		sgOptions.Keymapper.KeyPressed(vkey);
		if (vkey == SDLK_RETURN || vkey == SDLK_KP_ENTER) {
			if ((modState & KMOD_ALT) != 0) {
				sgOptions.Graphics.fullscreen.SetValue(!IsFullScreen());
				SaveOptions();
			} else {
				control_type_message();
			}
		}
		if (vkey != SDLK_ESCAPE) {
			return;
		}
	}
	if (vkey == SDLK_ESCAPE) {
		if (!PressEscKey()) {
			LastMouseButtonAction = MouseActionType::None;
			gamemenu_on();
		}
		return;
	}

	if (dropGoldFlag) {
		control_drop_gold(vkey);
		return;
	}
	if (IsWithdrawGoldOpen) {
		WithdrawGoldKeyPress(vkey);
		return;
	}

	if (sgnTimeoutCurs != CURSOR_NONE) {
		return;
	}

	sgOptions.Keymapper.KeyPressed(vkey);

	if (PauseMode == 2) {
		if ((vkey == SDLK_RETURN || vkey == SDLK_KP_ENTER) && (modState & KMOD_ALT) != 0) {
			sgOptions.Graphics.fullscreen.SetValue(!IsFullScreen());
			SaveOptions();
		}
		return;
	}

	if (DoomFlag) {
		doom_close();
		return;
	}

	switch (vkey) {
	case SDLK_PLUS:
	case SDLK_KP_PLUS:
	case SDLK_EQUALS:
	case SDLK_KP_EQUALS:
		if (AutomapActive) {
			AutomapZoomIn();
		}
		return;
	case SDLK_MINUS:
	case SDLK_KP_MINUS:
	case SDLK_UNDERSCORE:
		if (AutomapActive) {
			AutomapZoomOut();
		}
		return;
#ifdef _DEBUG
	case SDLK_m:
		if ((modState & KMOD_SHIFT) != 0)
			NextDebugMonster();
		else
			GetDebugMonster();
		return;
#endif
	case SDLK_RETURN:
	case SDLK_KP_ENTER:
		if ((modState & KMOD_ALT) != 0) {
			sgOptions.Graphics.fullscreen.SetValue(!IsFullScreen());
			SaveOptions();
		} else if (stextflag != STORE_NONE) {
			StoreEnter();
		} else if (QuestLogIsOpen) {
			QuestlogEnter();
		} else {
			control_type_message();
		}
		return;
	case SDLK_UP:
		if (stextflag != STORE_NONE) {
			StoreUp();
		} else if (QuestLogIsOpen) {
			QuestlogUp();
		} else if (HelpFlag) {
			HelpScrollUp();
		} else if (ChatLogFlag) {
			ChatLogScrollUp();
		} else if (AutomapActive) {
			AutomapUp();
		} else if (IsStashOpen) {
			Stash.PreviousPage();
		}
		return;
	case SDLK_DOWN:
		if (stextflag != STORE_NONE) {
			StoreDown();
		} else if (QuestLogIsOpen) {
			QuestlogDown();
		} else if (HelpFlag) {
			HelpScrollDown();
		} else if (ChatLogFlag) {
			ChatLogScrollDown();
		} else if (AutomapActive) {
			AutomapDown();
		} else if (IsStashOpen) {
			Stash.NextPage();
		}
		return;
	case SDLK_PAGEUP:
		if (stextflag != STORE_NONE) {
			StorePrior();
		} else if (ChatLogFlag) {
			ChatLogScrollTop();
		}
		return;
	case SDLK_PAGEDOWN:
		if (stextflag != STORE_NONE) {
			StoreNext();
		} else if (ChatLogFlag) {
			ChatLogScrollBottom();
		}
		return;
	case SDLK_LEFT:
		if (AutomapActive && !talkflag)
			AutomapLeft();
		return;
	case SDLK_RIGHT:
		if (AutomapActive && !talkflag)
			AutomapRight();
		return;
	default:
		break;
	}
}

void GameEventHandler(const SDL_Event &event, uint16_t modState)
{
	switch (event.type) {
	case SDL_KEYDOWN:
		PressKey(event.key.keysym.sym, modState);
		return;
	case SDL_KEYUP:
		ReleaseKey(event.key.keysym.sym);
		return;
	case SDL_MOUSEMOTION:
		MousePosition = { event.motion.x, event.motion.y };
		gmenu_on_mouse_move();
		return;
	case SDL_MOUSEBUTTONDOWN:
		MousePosition = { event.button.x, event.button.y };
		if (sgbMouseDown == CLICK_NONE) {
			switch (event.button.button) {
			case SDL_BUTTON_LEFT:
				sgbMouseDown = CLICK_LEFT;
				LeftMouseDown(modState);
				break;
			case SDL_BUTTON_RIGHT:
				sgbMouseDown = CLICK_RIGHT;
				RightMouseDown((modState & KMOD_SHIFT) != 0);
				break;
			default:
				sgOptions.Keymapper.KeyPressed(event.button.button | KeymapperMouseButtonMask);
				break;
			}
		}
		return;
	case SDL_MOUSEBUTTONUP:
		MousePosition = { event.button.x, event.button.y };

		if (sgbMouseDown == CLICK_LEFT && event.button.button == SDL_BUTTON_LEFT) {
			LastMouseButtonAction = MouseActionType::None;
			sgbMouseDown = CLICK_NONE;
			LeftMouseUp(modState);
		} else if (sgbMouseDown == CLICK_RIGHT && event.button.button == SDL_BUTTON_RIGHT) {
			LastMouseButtonAction = MouseActionType::None;
			sgbMouseDown = CLICK_NONE;
		} else {
			sgOptions.Keymapper.KeyReleased(event.button.button | KeymapperMouseButtonMask);
		}
		return;
	default:
		if (IsCustomEvent(event.type)) {
			if (gbIsMultiplayer)
				pfile_write_hero();
			nthread_ignore_mutex(true);
			PaletteFadeOut(8);
			sound_stop();
			LastMouseButtonAction = MouseActionType::None;
			sgbMouseDown = CLICK_NONE;
			ShowProgress(GetCustomEvent(event.type));
			force_redraw = 255;
			DrawAndBlit();
			LoadPWaterPalette();
			if (gbRunGame)
				PaletteFadeIn(8);
			nthread_ignore_mutex(false);
			gbGameLoopStartup = true;
			return;
		}
		MainWndProc(event);
		break;
	}
}

void RunGameLoop(interface_mode uMsg)
{
	demo::NotifyGameLoopStart();

	nthread_ignore_mutex(true);
	StartGame(uMsg);
	assert(HeadlessMode || ghMainWnd);
	EventHandler previousHandler = SetEventHandler(GameEventHandler);
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

	discord_manager::StartGame();
#ifdef GPERF_HEAP_FIRST_GAME_ITERATION
	unsigned run_game_iteration = 0;
#endif

	while (gbRunGame) {

#ifdef _DEBUG
		if (!gbGameLoopStartup && !DebugCmdsFromCommandLine.empty()) {
			for (auto &cmd : DebugCmdsFromCommandLine) {
				CheckDebugTextCommand(cmd);
			}
			DebugCmdsFromCommandLine.clear();
		}
#endif

		SDL_Event event;
		uint16_t modState;
		while (FetchMessage(&event, &modState)) {
			if (event.type == SDL_QUIT) {
				gbRunGameResult = false;
				gbRunGame = false;
				break;
			}
			HandleMessage(event, modState);
		}
		if (!gbRunGame)
			break;

		bool drawGame = true;
		bool processInput = true;
		bool runGameLoop = demo::IsRunning() ? demo::GetRunGameLoop(drawGame, processInput) : nthread_has_500ms_passed();
		if (demo::IsRecording())
			demo::RecordGameLoopResult(runGameLoop);

		discord_manager::UpdateGame();

		if (!runGameLoop) {
			if (processInput)
				ProcessInput();
			if (!drawGame)
				continue;
			force_redraw |= 1;
			DrawAndBlit();
			continue;
		}

		diablo_color_cyc_logic();
		multi_process_network_packets();
		game_loop(gbGameLoopStartup);
		gbGameLoopStartup = false;
		if (drawGame)
			DrawAndBlit();
#ifdef GPERF_HEAP_FIRST_GAME_ITERATION
		if (run_game_iteration++ == 0)
			HeapProfilerDump("first_game_iteration");
#endif
	}

	demo::NotifyGameLoopEnd();

	if (gbIsMultiplayer) {
		pfile_write_hero(/*writeGameData=*/false);
		sfile_write_stash();
	}

	PaletteFadeOut(8);
	NewCursor(CURSOR_NONE);
	ClearScreenBuffer();
	force_redraw = 255;
	scrollrt_draw_game_screen();
	previousHandler = SetEventHandler(previousHandler);
	assert(HeadlessMode || previousHandler == GameEventHandler);
	FreeGame();

	if (cineflag) {
		cineflag = false;
		DoEnding();
	}
}

void PrintWithRightPadding(string_view str, size_t width)
{
	printInConsole(str);
	if (str.size() >= width)
		return;
	printInConsole(std::string(width - str.size(), ' '));
}

void PrintHelpOption(string_view flags, string_view description)
{
	printInConsole("    ");
	PrintWithRightPadding(flags, 20);
	printInConsole(" ");
	PrintWithRightPadding(description, 30);
	printNewlineInConsole();
}

[[noreturn]] void PrintHelpAndExit()
{
	printInConsole((/* TRANSLATORS: Commandline Option */ "Options:"));
	printNewlineInConsole();
	PrintHelpOption("-h, --help", _(/* TRANSLATORS: Commandline Option */ "Print this message and exit"));
	PrintHelpOption("--version", _(/* TRANSLATORS: Commandline Option */ "Print the version and exit"));
	PrintHelpOption("--data-dir", _(/* TRANSLATORS: Commandline Option */ "Specify the folder of diabdat.mpq"));
	PrintHelpOption("--save-dir", _(/* TRANSLATORS: Commandline Option */ "Specify the folder of save files"));
	PrintHelpOption("--config-dir", _(/* TRANSLATORS: Commandline Option */ "Specify the location of diablo.ini"));
	PrintHelpOption("-n", _(/* TRANSLATORS: Commandline Option */ "Skip startup videos"));
	PrintHelpOption("-f", _(/* TRANSLATORS: Commandline Option */ "Display frames per second"));
	PrintHelpOption("--verbose", _(/* TRANSLATORS: Commandline Option */ "Enable verbose logging"));
#ifndef DISABLE_DEMOMODE
	PrintHelpOption("--record <#>", _(/* TRANSLATORS: Commandline Option */ "Record a demo file"));
	PrintHelpOption("--demo <#>", _(/* TRANSLATORS: Commandline Option */ "Play a demo file"));
	PrintHelpOption("--timedemo", _(/* TRANSLATORS: Commandline Option */ "Disable all frame limiting during demo playback"));
#endif
	printNewlineInConsole();
	printInConsole(_(/* TRANSLATORS: Commandline Option */ "Game selection:"));
	printNewlineInConsole();
	PrintHelpOption("--spawn", _(/* TRANSLATORS: Commandline Option */ "Force Shareware mode"));
	PrintHelpOption("--diablo", _(/* TRANSLATORS: Commandline Option */ "Force Diablo mode"));
	PrintHelpOption("--hellfire", _(/* TRANSLATORS: Commandline Option */ "Force Hellfire mode"));
	printInConsole(_(/* TRANSLATORS: Commandline Option */ "Hellfire options:"));
	printNewlineInConsole();
#ifdef _DEBUG
	printNewlineInConsole();
	printInConsole("Debug options:");
	printNewlineInConsole();
	PrintHelpOption("-i", "Ignore network timeout");
	PrintHelpOption("+<internal command>", "Pass commands to the engine");
#endif
	printNewlineInConsole();
	printInConsole(_("Report bugs at https://github.com/diasurgical/devilutionX/"));
	printNewlineInConsole();
	diablo_quit(0);
}

void PrintFlagsRequiresArgument(string_view flag)
{
	printInConsole(flag);
	printInConsole("requires an argument");
	printNewlineInConsole();
}

void DiabloParseFlags(int argc, char **argv)
{
#ifdef _DEBUG
	int argumentIndexOfLastCommandPart = -1;
	std::string currentCommand;
#endif
#ifndef DISABLE_DEMOMODE
	bool timedemo = false;
	int demoNumber = -1;
	int recordNumber = -1;
	bool createDemoReference = false;
#endif
	for (int i = 1; i < argc; i++) {
		const string_view arg = argv[i];
		if (arg == "-h" || arg == "--help") {
			PrintHelpAndExit();
		} else if (arg == "--version") {
			printInConsole(PROJECT_NAME);
			printInConsole(" v");
			printInConsole(PROJECT_VERSION);
			printNewlineInConsole();
			diablo_quit(0);
		} else if (arg == "--data-dir") {
			if (i + 1 == argc) {
				PrintFlagsRequiresArgument("--data-dir");
				diablo_quit(0);
			}
			paths::SetBasePath(argv[++i]);
		} else if (arg == "--save-dir") {
			if (i + 1 == argc) {
				PrintFlagsRequiresArgument("--save-dir");
				diablo_quit(0);
			}
			paths::SetPrefPath(argv[++i]);
		} else if (arg == "--config-dir") {
			if (i + 1 == argc) {
				PrintFlagsRequiresArgument("--config-dir");
				diablo_quit(0);
			}
			paths::SetConfigPath(argv[++i]);
#ifndef DISABLE_DEMOMODE
		} else if (arg == "--demo") {
			if (i + 1 == argc) {
				PrintFlagsRequiresArgument("--demo");
				diablo_quit(0);
			}
			demoNumber = SDL_atoi(argv[++i]);
			gbShowIntro = false;
		} else if (arg == "--timedemo") {
			timedemo = true;
		} else if (arg == "--record") {
			if (i + 1 == argc) {
				PrintFlagsRequiresArgument("--record");
				diablo_quit(0);
			}
			recordNumber = SDL_atoi(argv[++i]);
		} else if (arg == "--create-reference") {
			createDemoReference = true;
#else
		} else if (arg == "--demo" || arg == "--timedemo" || arg == "--record" || arg == "--create-reference") {
			printInConsole("Binary compiled without demo mode support.");
			printNewlineInConsole();
			diablo_quit(1);
#endif
		} else if (arg == "-n") {
			gbShowIntro = false;
		} else if (arg == "-f") {
			EnableFrameCount();
		} else if (arg == "--spawn") {
			forceSpawn = true;
		} else if (arg == "--diablo") {
			forceDiablo = true;
		} else if (arg == "--hellfire") {
			forceHellfire = true;
		} else if (arg == "--vanilla") {
			gbVanilla = true;
		} else if (arg == "--verbose") {
			SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
#ifdef _DEBUG
		} else if (arg == "-i") {
			DebugDisableNetworkTimeout = true;
		} else if (arg[0] == '+') {
			if (!currentCommand.empty())
				DebugCmdsFromCommandLine.push_back(currentCommand);
			argumentIndexOfLastCommandPart = i;
			currentCommand = arg.substr(1);
		} else if (arg[0] != '-' && (argumentIndexOfLastCommandPart + 1) == i) {
			currentCommand.append(" ");
			currentCommand.append(arg);
			argumentIndexOfLastCommandPart = i;
#endif
		} else {
			printInConsole("unrecognized option '");
			printInConsole(argv[i]);
			printInConsole("'");
			printNewlineInConsole();
			PrintHelpAndExit();
		}
	}

#ifdef _DEBUG
	if (!currentCommand.empty())
		DebugCmdsFromCommandLine.push_back(currentCommand);
#endif

#ifndef DISABLE_DEMOMODE
	if (demoNumber != -1)
		demo::InitPlayBack(demoNumber, timedemo);
	if (recordNumber != -1)
		demo::InitRecording(recordNumber, createDemoReference);
#endif
}

void DiabloInitScreen()
{
	MousePosition = { gnScreenWidth / 2, gnScreenHeight / 2 };
	if (ControlMode == ControlTypes::KeyboardAndMouse)
		SetCursorPos(MousePosition);

	ClrDiabloMsg();
}

void SetApplicationVersions()
{
	*BufCopy(gszProductName, PROJECT_NAME, " v", PROJECT_VERSION) = '\0';
	*BufCopy(gszVersionNumber, "version ", PROJECT_VERSION) = '\0';
}

void CheckArchivesUpToDate()
{
	const bool devilutionxMpqOutOfDate = devilutionx_mpq && !devilutionx_mpq->HasFile("data\\charbg.clx");
	const bool fontsMpqOutOfDate = font_mpq && !font_mpq->HasFile("fonts\\12-4e.clx");

	if (devilutionxMpqOutOfDate && fontsMpqOutOfDate) {
		app_fatal(_("Please update devilutionx.mpq and fonts.mpq to the latest version"));
	} else if (devilutionxMpqOutOfDate) {
		app_fatal(_("Failed to load UI resources.\n"
		            "\n"
		            "Make sure devilutionx.mpq is in the game folder and that it is up to date."));
	} else if (fontsMpqOutOfDate) {
		app_fatal(_("Please update fonts.mpq to the latest version"));
	}
}

void DiabloInit()
{
	if (*sgOptions.Graphics.showFPS)
		EnableFrameCount();

	init_create_window();
	was_window_init = true;

	if (forceSpawn || *sgOptions.StartUp.shareware)
		gbIsSpawn = true;
	if (forceDiablo || *sgOptions.StartUp.gameMode == StartUpGameMode::Diablo)
		gbIsHellfire = false;
	if (forceHellfire)
		gbIsHellfire = true;

	gbIsHellfireSaveGame = gbIsHellfire;

	LanguageInitialize();

	SetApplicationVersions();

	for (size_t i = 0; i < QUICK_MESSAGE_OPTIONS; i++) {
		auto &messages = sgOptions.Chat.szHotKeyMsgs[i];
		if (messages.empty()) {
			messages.emplace_back(_(QuickMessages[i].message));
		}
	}

#ifndef USE_SDL1
	InitializeVirtualGamepad();
#endif

	UiInitialize();
	was_ui_init = true;

	ReadOnlyTest();

	if (gbIsHellfire && !forceHellfire && *sgOptions.StartUp.gameMode == StartUpGameMode::Ask) {
		UiSelStartUpGameOption();
		if (!gbIsHellfire) {
			// Reinitalize the UI Elements cause we changed the game
			UnloadUiGFX();
			UiInitialize();
			if (IsHardwareCursor())
				SetHardwareCursor(CursorInfo::UnknownCursor());
		}
	}

	DiabloInitScreen();

	snd_init();

	ui_sound_init();

	// Item graphics are loaded early, they already get touched during hero selection.
	InitItemGFX();

	// Always available.
	LoadSmallSelectionSpinner();

	CheckArchivesUpToDate();
}

void DiabloSplash()
{
	if (!gbShowIntro)
		return;

	if (*sgOptions.StartUp.splash == StartUpSplash::LogoAndTitleDialog)
		play_movie("gendata\\logo.smk", true);

	auto &intro = gbIsHellfire ? sgOptions.StartUp.hellfireIntro : sgOptions.StartUp.diabloIntro;

	if (*intro != StartUpIntro::Off) {
		if (gbIsHellfire)
			play_movie("gendata\\Hellfire.smk", true);
		else
			play_movie("gendata\\diablo1.smk", true);
		if (*intro == StartUpIntro::Once) {
			intro.SetValue(StartUpIntro::Off);
			SaveOptions();
		}
	}

	if (IsAnyOf(*sgOptions.StartUp.splash, StartUpSplash::TitleDialog, StartUpSplash::LogoAndTitleDialog))
		UiTitleDialog();
}

void DiabloDeinit()
{
	FreeItemGFX();

	if (gbSndInited)
		effects_cleanup_sfx();
	snd_deinit();
	if (was_ui_init)
		UiDestroy();
	if (was_archives_init)
		init_cleanup();
	if (was_window_init)
		dx_cleanup(); // Cleanup SDL surfaces stuff, so we have to do it before SDL_Quit().
	UnloadFonts();
	if (SDL_WasInit(SDL_INIT_EVERYTHING & ~SDL_INIT_HAPTIC) != 0)
		SDL_Quit();
}

void LoadLvlGFX()
{
	assert(pDungeonCels == nullptr);
	constexpr int SpecialCelWidth = 64;

	switch (leveltype) {
	case DTYPE_TOWN:
		if (gbIsHellfire) {
			pDungeonCels = LoadFileInMem("nlevels\\towndata\\town.cel");
			pMegaTiles = LoadFileInMem<MegaTile>("nlevels\\towndata\\town.til");
		} else {
			pDungeonCels = LoadFileInMem("levels\\towndata\\town.cel");
			pMegaTiles = LoadFileInMem<MegaTile>("levels\\towndata\\town.til");
		}
		pSpecialCels = LoadCel("levels\\towndata\\towns.cel", SpecialCelWidth);
		break;
	case DTYPE_CATHEDRAL:
		pDungeonCels = LoadFileInMem("levels\\l1data\\l1.cel");
		pMegaTiles = LoadFileInMem<MegaTile>("levels\\l1data\\l1.til");
		pSpecialCels = LoadCel("levels\\l1data\\l1s.cel", SpecialCelWidth);
		break;
	case DTYPE_CATACOMBS:
		pDungeonCels = LoadFileInMem("levels\\l2data\\l2.cel");
		pMegaTiles = LoadFileInMem<MegaTile>("levels\\l2data\\l2.til");
		pSpecialCels = LoadCel("levels\\l2data\\l2s.cel", SpecialCelWidth);
		break;
	case DTYPE_CAVES:
		pDungeonCels = LoadFileInMem("levels\\l3data\\l3.cel");
		pMegaTiles = LoadFileInMem<MegaTile>("levels\\l3data\\l3.til");
		pSpecialCels = LoadCel("levels\\l1data\\l1s.cel", SpecialCelWidth);
		break;
	case DTYPE_HELL:
		pDungeonCels = LoadFileInMem("levels\\l4data\\l4.cel");
		pMegaTiles = LoadFileInMem<MegaTile>("levels\\l4data\\l4.til");
		pSpecialCels = LoadCel("levels\\l2data\\l2s.cel", SpecialCelWidth);
		break;
	case DTYPE_NEST:
		pDungeonCels = LoadFileInMem("nlevels\\l6data\\l6.cel");
		pMegaTiles = LoadFileInMem<MegaTile>("nlevels\\l6data\\l6.til");
		pSpecialCels = LoadCel("levels\\l1data\\l1s.cel", SpecialCelWidth);
		break;
	case DTYPE_CRYPT:
		pDungeonCels = LoadFileInMem("nlevels\\l5data\\l5.cel");
		pMegaTiles = LoadFileInMem<MegaTile>("nlevels\\l5data\\l5.til");
		pSpecialCels = LoadCel("nlevels\\l5data\\l5s.cel", SpecialCelWidth);
		break;
	default:
		app_fatal("LoadLvlGFX");
	}
}

void LoadAllGFX()
{
	IncProgress();
#if !defined(USE_SDL1) && !defined(__vita__)
	InitVirtualGamepadGFX(renderer);
#endif
	IncProgress();
	InitObjectGFX();
	IncProgress();
	InitMissileGFX(gbIsHellfire);
	IncProgress();
}

/**
 * @param entry Where is the player entering from
 */
void CreateLevel(lvl_entry entry)
{
	CreateDungeon(glSeedTbl[currlevel], entry);

	switch (leveltype) {
	case DTYPE_TOWN:
		InitTownTriggers();
		break;
	case DTYPE_CATHEDRAL:
		InitL1Triggers();
		break;
	case DTYPE_CATACOMBS:
		InitL2Triggers();
		break;
	case DTYPE_CAVES:
		InitL3Triggers();
		break;
	case DTYPE_HELL:
		InitL4Triggers();
		break;
	case DTYPE_NEST:
		InitHiveTriggers();
		break;
	case DTYPE_CRYPT:
		InitCryptTriggers();
		break;
	default:
		app_fatal("CreateLevel");
	}

	if (leveltype != DTYPE_TOWN) {
		Freeupstairs();
	}
	LoadRndLvlPal(leveltype);
}

void UnstuckChargers()
{
	if (gbIsMultiplayer) {
		for (Player &player : Players) {
			if (!player.plractive)
				continue;
			if (player._pLvlChanging)
				continue;
			if (!player.isOnActiveLevel())
				continue;
			if (&player == MyPlayer)
				continue;
			return;
		}
	}
	for (size_t i = 0; i < ActiveMonsterCount; i++) {
		auto &monster = Monsters[ActiveMonsters[i]];
		if (monster.mode == MonsterMode::Charge)
			monster.mode = MonsterMode::Stand;
	}
}

void UpdateMonsterLights()
{
	for (size_t i = 0; i < ActiveMonsterCount; i++) {
		auto &monster = Monsters[ActiveMonsters[i]];

		if ((monster.flags & MFLAG_BERSERK) != 0) {
			int lightRadius = leveltype == DTYPE_NEST ? 9 : 3;
			monster.lightId = AddLight(monster.position.tile, lightRadius);
		}

		if (monster.lightId != NO_LIGHT) {
			if (monster.lightId == MyPlayer->_plid) { // Fix old saves where some monsters had 0 instead of NO_LIGHT
				monster.lightId = NO_LIGHT;
				continue;
			}

			Light &light = Lights[monster.lightId];
			if (monster.position.tile != light.position.tile) {
				ChangeLightXY(monster.lightId, monster.position.tile);
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
	if (DebugScrollViewEnabled && (SDL_GetModState() & KMOD_SHIFT) != 0) {
		ScrollView();
	}
#endif

	sound_update();
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
		LastMouseButtonAction = MouseActionType::None;
	} else {
		CloseInventory();
		chrflag = false;
		sbookflag = false;
		spselflag = false;
		if (qtextflag && leveltype == DTYPE_TOWN) {
			qtextflag = false;
			stream_stop();
		}
		QuestLogIsOpen = false;
		CancelCurrentDiabloMsg();
		gamemenu_off();
		DisplayHelp();
		doom_close();
	}
}

void InventoryKeyPressed()
{
	if (stextflag != STORE_NONE)
		return;
	invflag = !invflag;
	if (!IsLeftPanelOpen() && CanPanelsCoverView()) {
		if (!invflag) { // We closed the invetory
			if (MousePosition.x < 480 && MousePosition.y < GetMainPanel().position.y) {
				SetCursorPos(MousePosition + Displacement { 160, 0 });
			}
		} else if (!sbookflag) { // We opened the invetory
			if (MousePosition.x > 160 && MousePosition.y < GetMainPanel().position.y) {
				SetCursorPos(MousePosition - Displacement { 160, 0 });
			}
		}
	}
	sbookflag = false;
	CloseGoldWithdraw();
	IsStashOpen = false;
}

void CharacterSheetKeyPressed()
{
	if (stextflag != STORE_NONE)
		return;
	chrflag = !chrflag;
	if (!IsRightPanelOpen() && CanPanelsCoverView()) {
		if (!chrflag) { // We closed the character sheet
			if (MousePosition.x > 160 && MousePosition.y < GetMainPanel().position.y) {
				SetCursorPos(MousePosition - Displacement { 160, 0 });
			}
		} else if (!QuestLogIsOpen) { // We opened the character sheet
			if (MousePosition.x < 480 && MousePosition.y < GetMainPanel().position.y) {
				SetCursorPos(MousePosition + Displacement { 160, 0 });
			}
		}
	}
	QuestLogIsOpen = false;
	CloseGoldWithdraw();
	IsStashOpen = false;
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
	if (!IsRightPanelOpen() && CanPanelsCoverView()) {
		if (!QuestLogIsOpen) { // We closed the quest log
			if (MousePosition.x > 160 && MousePosition.y < GetMainPanel().position.y) {
				SetCursorPos(MousePosition - Displacement { 160, 0 });
			}
		} else if (!chrflag) { // We opened the character quest log
			if (MousePosition.x < 480 && MousePosition.y < GetMainPanel().position.y) {
				SetCursorPos(MousePosition + Displacement { 160, 0 });
			}
		}
	}
	chrflag = false;
	CloseGoldWithdraw();
	IsStashOpen = false;
}

void DisplaySpellsKeyPressed()
{
	if (stextflag != STORE_NONE)
		return;
	chrflag = false;
	QuestLogIsOpen = false;
	CloseInventory();
	sbookflag = false;
	if (!spselflag) {
		DoSpeedBook();
	} else {
		spselflag = false;
	}
	LastMouseButtonAction = MouseActionType::None;
}

void SpellBookKeyPressed()
{
	if (stextflag != STORE_NONE)
		return;
	sbookflag = !sbookflag;
	if (!IsLeftPanelOpen() && CanPanelsCoverView()) {
		if (!sbookflag) { // We closed the invetory
			if (MousePosition.x < 480 && MousePosition.y < GetMainPanel().position.y) {
				SetCursorPos(MousePosition + Displacement { 160, 0 });
			}
		} else if (!invflag) { // We opened the invetory
			if (MousePosition.x > 160 && MousePosition.y < GetMainPanel().position.y) {
				SetCursorPos(MousePosition - Displacement { 160, 0 });
			}
		}
	}
	CloseInventory();
}

bool IsPlayerDead()
{
	return MyPlayer->_pmode == PM_DEATH || MyPlayerIsDead;
}

bool IsGameRunning()
{
	return PauseMode != 2;
}

bool CanPlayerTakeAction()
{
	return !IsPlayerDead() && IsGameRunning();
}
} // namespace

void InitKeymapActions()
{
	for (int i = 0; i < 8; ++i) {
		sgOptions.Keymapper.AddAction(
		    "BeltItem{}",
		    N_("Belt item {}"),
		    N_("Use Belt item."),
		    '1' + i,
		    [i] {
			    Player &myPlayer = *MyPlayer;
			    if (!myPlayer.SpdList[i].isEmpty() && myPlayer.SpdList[i]._itype != ItemType::Gold) {
				    UseInvItem(MyPlayerId, INVITEM_BELT_FIRST + i);
			    }
		    },
		    nullptr,
		    CanPlayerTakeAction,
		    i + 1);
	}
	for (size_t i = 0; i < NumHotkeys; ++i) {
		sgOptions.Keymapper.AddAction(
		    "QuickSpell{}",
		    N_("Quick spell {}"),
		    N_("Hotkey for skill or spell."),
		    i < 4 ? static_cast<uint32_t>(SDLK_F5) + i : static_cast<uint32_t>(SDLK_UNKNOWN),
		    [i]() {
			    if (spselflag) {
				    SetSpeedSpell(i);
				    return;
			    }
			    if (!*sgOptions.Gameplay.quickCast)
				    ToggleSpell(i);
			    else
				    QuickCast(i);
		    },
		    nullptr,
		    CanPlayerTakeAction,
		    i + 1);
	}
	sgOptions.Keymapper.AddAction(
	    "DisplaySpells",
	    N_("Speedbook"),
	    N_("Open Speedbook."),
	    'S',
	    DisplaySpellsKeyPressed,
	    nullptr,
	    CanPlayerTakeAction);
	sgOptions.Keymapper.AddAction(
	    "QuickSave",
	    N_("Quick save"),
	    N_("Saves the game."),
	    SDLK_F2,
	    [] { gamemenu_save_game(false); },
	    nullptr,
	    [&]() { return !gbIsMultiplayer && CanPlayerTakeAction(); });
	sgOptions.Keymapper.AddAction(
	    "QuickLoad",
	    N_("Quick load"),
	    N_("Loads the game."),
	    SDLK_F3,
	    [] { gamemenu_load_game(false); },
	    nullptr,
	    [&]() { return !gbIsMultiplayer && gbValidSaveFile && stextflag == STORE_NONE && IsGameRunning(); });
#ifndef NOEXIT
	sgOptions.Keymapper.AddAction(
	    "QuitGame",
	    N_("Quit game"),
	    N_("Closes the game."),
	    SDLK_UNKNOWN,
	    [] { gamemenu_quit_game(false); });
#endif
	sgOptions.Keymapper.AddAction(
	    "StopHero",
	    N_("Stop hero"),
	    N_("Stops walking and cancel pending actions."),
	    SDLK_UNKNOWN,
	    [] { MyPlayer->Stop(); },
	    nullptr,
	    CanPlayerTakeAction);
	sgOptions.Keymapper.AddAction(
	    "Item Highlighting",
	    N_("Item highlighting"),
	    N_("Show/hide items on ground."),
	    SDLK_LALT,
	    [] { AltPressed(true); },
	    [] { AltPressed(false); });
	sgOptions.Keymapper.AddAction(
	    "Toggle Item Highlighting",
	    N_("Toggle item highlighting"),
	    N_("Permanent show/hide items on ground."),
	    SDLK_RCTRL,
	    nullptr,
	    [] { ToggleItemLabelHighlight(); });
	sgOptions.Keymapper.AddAction(
	    "Toggle Automap",
	    N_("Toggle automap"),
	    N_("Toggles if automap is displayed."),
	    SDLK_TAB,
	    DoAutoMap,
	    nullptr,
	    IsGameRunning);

	sgOptions.Keymapper.AddAction(
	    "Inventory",
	    N_("Inventory"),
	    N_("Open Inventory screen."),
	    'I',
	    InventoryKeyPressed,
	    nullptr,
	    CanPlayerTakeAction);
	sgOptions.Keymapper.AddAction(
	    "Character",
	    N_("Character"),
	    N_("Open Character screen."),
	    'C',
	    CharacterSheetKeyPressed,
	    nullptr,
	    CanPlayerTakeAction);
	sgOptions.Keymapper.AddAction(
	    "QuestLog",
	    N_("Quest log"),
	    N_("Open Quest log."),
	    'Q',
	    QuestLogKeyPressed,
	    nullptr,
	    CanPlayerTakeAction);
	sgOptions.Keymapper.AddAction(
	    "SpellBook",
	    N_("Spellbook"),
	    N_("Open Spellbook."),
	    'B',
	    SpellBookKeyPressed,
	    nullptr,
	    CanPlayerTakeAction);
	for (int i = 0; i < 4; ++i) {
		sgOptions.Keymapper.AddAction(
		    "QuickMessage{}",
		    N_("Quick Message {}"),
		    N_("Use Quick Message in chat."),
		    SDLK_F9 + i,
		    [i]() { DiabloHotkeyMsg(i); },
		    nullptr,
		    nullptr,
		    i + 1);
	}
	sgOptions.Keymapper.AddAction(
	    "Hide Info Screens",
	    N_("Hide Info Screens"),
	    N_("Hide all info screens."),
	    SDLK_SPACE,
	    [] {
		    ClosePanels();
		    HelpFlag = false;
		    ChatLogFlag = false;
		    spselflag = false;
		    if (qtextflag && leveltype == DTYPE_TOWN) {
			    qtextflag = false;
			    stream_stop();
		    }
		    AutomapActive = false;
		    CancelCurrentDiabloMsg();
		    gamemenu_off();
		    doom_close();
	    },
	    nullptr,
	    IsGameRunning);
	sgOptions.Keymapper.AddAction(
	    "Zoom",
	    N_("Zoom"),
	    N_("Zoom Game Screen."),
	    'Z',
	    [] {
		    sgOptions.Graphics.zoom.SetValue(!*sgOptions.Graphics.zoom);
		    CalcViewportGeometry();
	    },
	    nullptr,
	    CanPlayerTakeAction);
	sgOptions.Keymapper.AddAction(
	    "Pause Game",
	    N_("Pause Game"),
	    N_("Pauses the game."),
	    'P',
	    diablo_pause_game);
	sgOptions.Keymapper.AddAction(
	    "DecreaseGamma",
	    N_("Decrease Gamma"),
	    N_("Reduce screen brightness."),
	    'G',
	    DecreaseGamma,
	    nullptr,
	    CanPlayerTakeAction);
	sgOptions.Keymapper.AddAction(
	    "IncreaseGamma",
	    N_("Increase Gamma"),
	    N_("Increase screen brightness."),
	    'F',
	    IncreaseGamma,
	    nullptr,
	    CanPlayerTakeAction);
	sgOptions.Keymapper.AddAction(
	    "Help",
	    N_("Help"),
	    N_("Open Help Screen."),
	    SDLK_F1,
	    HelpKeyPressed,
	    nullptr,
	    CanPlayerTakeAction);
	sgOptions.Keymapper.AddAction(
	    "Screenshot",
	    N_("Screenshot"),
	    N_("Takes a screenshot."),
	    SDLK_PRINTSCREEN,
	    nullptr,
	    CaptureScreen);
	sgOptions.Keymapper.AddAction(
	    "GameInfo",
	    N_("Game info"),
	    N_("Displays game infos."),
	    'V',
	    [] {
		    EventPlrMsg(fmt::format(
		                    fmt::runtime(_(/* TRANSLATORS: {:s} means: Character Name, Game Version, Game Difficulty. */ "{:s} {:s}")),
		                    PROJECT_NAME,
		                    PROJECT_VERSION),
		        UiFlags::ColorWhite);
	    },
	    nullptr,
	    CanPlayerTakeAction);
	sgOptions.Keymapper.AddAction(
	    "ChatLog",
	    N_("Chat Log"),
	    N_("Displays chat log."),
	    'L',
	    [] {
		    ToggleChatLog();
	    });
#ifdef _DEBUG
	sgOptions.Keymapper.AddAction(
	    "DebugToggle",
	    "Debug toggle",
	    "Programming is like magic.",
	    'X',
	    [] {
		    DebugToggle = !DebugToggle;
	    });
#endif
}

void FreeGameMem()
{
	pDungeonCels = nullptr;
	pMegaTiles = nullptr;
	pSpecialCels = std::nullopt;

	FreeMonsters();
	FreeMissileGFX();
	FreeObjectGFX();
	FreeTownerGFX();
#ifndef USE_SDL1
	DeactivateVirtualGamepad();
	FreeVirtualGamepadGFX();
#endif
}

bool StartGame(bool bNewGame, bool bSinglePlayer)
{
	gbSelectProvider = true;
	ReturnToMainMenu = false;

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
			InitDungMsgs(*MyPlayer);
			DeltaSyncJunk();
		}
		giNumberOfLevels = gbIsHellfire ? 25 : 17;
		interface_mode uMsg = WM_DIABNEWGAME;
		if (gbValidSaveFile && gbLoadGame) {
			uMsg = WM_DIABLOADGAME;
		}
		RunGameLoop(uMsg);
		NetClose();
		UnloadFonts();

		// If the player left the game into the main menu,
		// initialize main menu resources.
		if (gbRunGameResult)
			UiInitialize();
		if (ReturnToMainMenu)
			return true;
	} while (gbRunGameResult);

	SNetDestroy();
	return gbRunGameResult;
}

void diablo_quit(int exitStatus)
{
	FreeGameMem();
	music_stop();
	DiabloDeinit();
	exit(exitStatus);
}

#ifdef __UWP__
void (*onInitialized)() = NULL;

void setOnInitialized(void (*callback)())
{
	onInitialized = callback;
}
#endif

int DiabloMain(int argc, char **argv)
{
#ifdef _DEBUG
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
#endif

	DiabloParseFlags(argc, argv);
	InitKeymapActions();

	// Need to ensure devilutionx.mpq (and fonts.mpq if available) are loaded before attempting to read translation settings
	LoadCoreArchives();
	was_archives_init = true;

	// Read settings including translation next. This will use the presence of fonts.mpq and look for assets in devilutionx.mpq
	LoadOptions();
	// Then look for a voice pack file based on the selected translation
	LoadLanguageArchive();

	// Finally load game data
	LoadGameArchives();

	DiabloInit();
#ifdef __UWP__
	onInitialized();
#endif
	SaveOptions();

	DiabloSplash();
	mainmenu_loop();
	DiabloDeinit();

	return 0;
}

bool TryIconCurs()
{
	if (pcurs == CURSOR_RESURRECT) {
		if (pcursplr != -1) {
			NetSendCmdParam1(true, CMD_RESURRECT, pcursplr);
			NewCursor(CURSOR_HAND);
			return true;
		}

		return false;
	}

	if (pcurs == CURSOR_HEALOTHER) {
		if (pcursplr != -1) {
			NetSendCmdParam1(true, CMD_HEALOTHER, pcursplr);
			NewCursor(CURSOR_HAND);
			return true;
		}

		return false;
	}

	if (pcurs == CURSOR_TELEKINESIS) {
		DoTelekinesis();
		return true;
	}

	Player &myPlayer = *MyPlayer;

	if (pcurs == CURSOR_IDENTIFY) {
		if (pcursinvitem != -1)
			CheckIdentify(myPlayer, pcursinvitem);
		else if (pcursstashitem != uint16_t(-1)) {
			Item &item = Stash.stashList[pcursstashitem];
			item._iIdentified = true;
		}
		NewCursor(CURSOR_HAND);
		return true;
	}

	if (pcurs == CURSOR_REPAIR) {
		if (pcursinvitem != -1)
			DoRepair(myPlayer, pcursinvitem);
		else if (pcursstashitem != uint16_t(-1)) {
			Item &item = Stash.stashList[pcursstashitem];
			RepairItem(item, myPlayer._pLevel);
		}
		NewCursor(CURSOR_HAND);
		return true;
	}

	if (pcurs == CURSOR_RECHARGE) {
		if (pcursinvitem != -1)
			DoRecharge(myPlayer, pcursinvitem);
		else if (pcursstashitem != uint16_t(-1)) {
			Item &item = Stash.stashList[pcursstashitem];
			RechargeItem(item, myPlayer);
		}
		NewCursor(CURSOR_HAND);
		return true;
	}

	if (pcurs == CURSOR_OIL) {
		bool changeCursor = true;
		if (pcursinvitem != -1)
			changeCursor = DoOil(myPlayer, pcursinvitem);
		else if (pcursstashitem != uint16_t(-1)) {
			Item &item = Stash.stashList[pcursstashitem];
			changeCursor = ApplyOilToItem(item, myPlayer);
		}
		if (changeCursor)
			NewCursor(CURSOR_HAND);
		return true;
	}

	if (pcurs == CURSOR_TELEPORT) {
		if (pcursmonst != -1)
			NetSendCmdParam4(true, CMD_TSPELLID, pcursmonst, myPlayer._pTSpell, myPlayer.GetSpellLevel(myPlayer._pTSpell), myPlayer.queuedSpell.spellFrom);
		else if (pcursplr != -1)
			NetSendCmdParam4(true, CMD_TSPELLPID, pcursplr, myPlayer._pTSpell, myPlayer.GetSpellLevel(myPlayer._pTSpell), myPlayer.queuedSpell.spellFrom);
		else
			NetSendCmdLocParam3(true, CMD_TSPELLXY, cursPosition, myPlayer._pTSpell, myPlayer.GetSpellLevel(myPlayer._pTSpell), myPlayer.queuedSpell.spellFrom);
		NewCursor(CURSOR_HAND);
		return true;
	}

	if (pcurs == CURSOR_DISARM && ObjectUnderCursor == nullptr) {
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
			qtextflag = false;
			LastMouseButtonAction = MouseActionType::None;
		}

		force_redraw = 255;
	}
}

bool GameWasAlreadyPaused = false;
bool MinimizePaused = false;

bool diablo_is_focused()
{
#ifndef USE_SDL1
	return SDL_GetKeyboardFocus() == ghMainWnd;
#else
	Uint8 appState = SDL_GetAppState();
	return (appState & SDL_APPINPUTFOCUS) != 0;
#endif
}

void diablo_focus_pause()
{
	if (!movie_playing && (gbIsMultiplayer || MinimizePaused)) {
		return;
	}

	GameWasAlreadyPaused = PauseMode != 0;

	if (!GameWasAlreadyPaused) {
		PauseMode = 2;
		sound_stop();
		LastMouseButtonAction = MouseActionType::None;
	}

	SVidMute();
	music_mute();

	MinimizePaused = true;
}

void diablo_focus_unpause()
{
	if (!GameWasAlreadyPaused) {
		PauseMode = 0;
	}

	SVidUnmute();
	music_unmute();

	MinimizePaused = false;
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

	if (ChatLogFlag) {
		ChatLogFlag = false;
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

	if (IsDiabloMsgAvailable()) {
		CancelCurrentDiabloMsg();
		rv = true;
	}

	if (talkflag) {
		control_reset_talk();
		rv = true;
	}

	if (dropGoldFlag) {
		control_drop_gold(SDLK_ESCAPE);
		rv = true;
	}

	if (IsWithdrawGoldOpen) {
		WithdrawGoldKeyPress(SDLK_ESCAPE);
		rv = true;
	}

	if (spselflag) {
		spselflag = false;
		rv = true;
	}

	if (IsLeftPanelOpen() || IsRightPanelOpen()) {
		ClosePanels();
		rv = true;
	}

	return rv;
}

void DisableInputEventHandler(const SDL_Event &event, uint16_t modState)
{
	switch (event.type) {
	case SDL_MOUSEMOTION:
		MousePosition = { event.motion.x, event.motion.y };
		return;
	case SDL_MOUSEBUTTONDOWN:
		if (sgbMouseDown != CLICK_NONE)
			return;
		switch (event.button.button) {
		case SDL_BUTTON_LEFT:
			sgbMouseDown = CLICK_LEFT;
			return;
		case SDL_BUTTON_RIGHT:
			sgbMouseDown = CLICK_RIGHT;
			return;
		default:
			return;
		}
	case SDL_MOUSEBUTTONUP:
		sgbMouseDown = CLICK_NONE;
		return;
	}

	MainWndProc(event);
}

void LoadGameLevel(bool firstflag, lvl_entry lvldir)
{
	_music_id neededTrack = GetLevelMusic(leveltype);

	if (neededTrack != sgnMusicTrack)
		music_stop();
	if (pcurs > CURSOR_HAND && pcurs < CURSOR_FIRSTITEM) {
		NewCursor(CURSOR_HAND);
	}
	SetRndSeed(glSeedTbl[currlevel]);
	IncProgress();
	MakeLightTable();
	SetDungeonMicros();
	LoadLvlGFX();
	IncProgress();

	if (firstflag) {
		CloseInventory();
		drawsbarflag = false;
		qtextflag = false;
		if (!HeadlessMode) {
			InitInv();
			InitStash();
			InitQuestText();
			InitInfoBoxGfx();
			InitHelp();
		}
		InitStores();
		InitAutomapOnce();
	}
	SetRndSeed(glSeedTbl[currlevel]);

	if (leveltype == DTYPE_TOWN) {
		SetupTownStores();
	} else {
		FreeStoreMem();
	}

	if (firstflag || lvldir == ENTRY_LOAD) {
		LoadStash();
	}

	IncProgress();
	InitAutomap();

	if (leveltype != DTYPE_TOWN && lvldir != ENTRY_LOAD) {
		InitLighting();
		InitVision();
	}

	InitLevelMonsters();
	IncProgress();

	Player &myPlayer = *MyPlayer;

	if (!setlevel) {
		CreateLevel(lvldir);
		IncProgress();
		LoadLevelSOLData();
		SetRndSeed(glSeedTbl[currlevel]);

		if (leveltype != DTYPE_TOWN) {
			GetLevelMTypes();
			InitThemes();
			if (!HeadlessMode)
				LoadAllGFX();
		} else if (!HeadlessMode) {
			IncProgress();
#if !defined(USE_SDL1) && !defined(__vita__)
			InitVirtualGamepadGFX(renderer);
#endif
			IncProgress();
			InitMissileGFX(gbIsHellfire);
			IncProgress();
			IncProgress();
		}

		IncProgress();

		if (lvldir == ENTRY_RTNLVL)
			GetReturnLvlPos();
		if (lvldir == ENTRY_WARPLVL)
			GetPortalLvlPos();

		IncProgress();

		for (Player &player : Players) {
			if (player.plractive && player.isOnActiveLevel()) {
				InitPlayerGFX(player);
				if (lvldir != ENTRY_LOAD)
					InitPlayer(player, firstflag);
			}
		}

		PlayDungMsgs();
		InitMultiView();
		IncProgress();

		bool visited = false;
		for (const Player &player : Players) {
			if (player.plractive)
				visited = visited || player._pLvlVisited[currlevel];
		}

		SetRndSeed(glSeedTbl[currlevel]);

		if (leveltype != DTYPE_TOWN) {
			if (firstflag || lvldir == ENTRY_LOAD || !myPlayer._pLvlVisited[currlevel] || gbIsMultiplayer) {
				HoldThemeRooms();
				[[maybe_unused]] uint32_t mid1Seed = GetLCGEngineState();
				InitGolems();
				InitObjects();
				[[maybe_unused]] uint32_t mid2Seed = GetLCGEngineState();
				IncProgress();
				InitMonsters();
				InitItems();
				CreateThemeRooms();
				IncProgress();
				[[maybe_unused]] uint32_t mid3Seed = GetLCGEngineState();
				InitMissiles();
				InitCorpses();
#ifdef _DEBUG
				SetDebugLevelSeedInfos(mid1Seed, mid2Seed, mid3Seed, GetLCGEngineState());
#endif

				if (gbIsMultiplayer)
					DeltaLoadLevel();

				IncProgress();
				SavePreLighting();
			} else {
				HoldThemeRooms();
				InitGolems();
				InitMonsters();
				InitMissiles();
				InitCorpses();
				IncProgress();
				LoadLevel();
				IncProgress();
			}
		} else {
			for (int i = 0; i < MAXDUNX; i++) { // NOLINT(modernize-loop-convert)
				for (int j = 0; j < MAXDUNY; j++) {
					dFlags[i][j] |= DungeonFlag::Lit;
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
			for (int x = 0; x < DMAXX; x++)
				for (int y = 0; y < DMAXY; y++)
					UpdateAutomapExplorer({ x, y }, MAP_EXP_SELF);
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
		InitGolems();
		InitMonsters();
		IncProgress();
		if (!HeadlessMode) {
#if !defined(USE_SDL1) && !defined(__vita__)
			InitVirtualGamepadGFX(renderer);
#endif
			InitMissileGFX(gbIsHellfire);
			IncProgress();
		}
		InitCorpses();
		IncProgress();
		LoadLevelSOLData();
		IncProgress();

		if (lvldir == ENTRY_WARPLVL)
			GetPortalLvlPos();
		IncProgress();

		for (Player &player : Players) {
			if (player.plractive && player.isOnActiveLevel()) {
				InitPlayerGFX(player);
				if (lvldir != ENTRY_LOAD)
					InitPlayer(player, firstflag);
			}
		}
		IncProgress();

		InitMultiView();
		IncProgress();

		if (firstflag || lvldir == ENTRY_LOAD || !myPlayer._pSLvlVisited[setlvlnum] || gbIsMultiplayer) {
			InitItems();
			SavePreLighting();
		} else {
			LoadLevel();
		}
		if (gbIsMultiplayer)
			DeltaLoadLevel();

		InitMissiles();
		IncProgress();
	}

	SyncPortals();

	for (Player &player : Players) {
		if (player.plractive && player.isOnActiveLevel() && (!player._pLvlChanging || &player == MyPlayer)) {
			if (player._pHitPoints > 0) {
				if (!gbIsMultiplayer)
					dPlayer[player.position.tile.x][player.position.tile.y] = player.getId() + 1;
				else
					SyncInitPlrPos(player);
			} else {
				dFlags[player.position.tile.x][player.position.tile.y] |= DungeonFlag::DeadPlayer;
			}
		}
	}

	IncProgress();
	IncProgress();

	if (firstflag && !HeadlessMode) {
		InitControlPan();
	}
	IncProgress();
	UpdateMonsterLights();
	UnstuckChargers();
	if (leveltype != DTYPE_TOWN) {
		ProcessLightList();
		ProcessVisionList();
	}

	if (leveltype == DTYPE_CRYPT) {
		if (currlevel == 21) {
			CornerstoneLoad(CornerStone.position);
		}
		if (Quests[Q_NAKRUL]._qactive == QUEST_DONE && currlevel == 24) {
			SyncNakrulRoom();
		}
	}

#ifndef USE_SDL1
	ActivateVirtualGamepad();
#endif

	if (sgnMusicTrack != neededTrack)
		music_start(neededTrack);

	if (MinimizePaused) {
		music_mute();
	}

	CompleteProgress();

	if (!gbIsSpawn && setlevel && setlvlnum == SL_SKELKING && Quests[Q_SKELKING]._qactive == QUEST_ACTIVE)
		PlaySFX(USFX_SKING1);

	// Reset mouse selection of entities
	pcursmonst = -1;
	ObjectUnderCursor = nullptr;
	pcursitem = -1;
	pcursinvitem = -1;
	pcursplr = -1;
}

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
		ClearLastSentPlayerCmd();

		if (!gbRunGame || !gbIsMultiplayer || demo::IsRunning() || demo::IsRecording() || !nthread_has_500ms_passed())
			break;
	}
}

void diablo_color_cyc_logic()
{
	if (!*sgOptions.Graphics.colorCycling)
		return;

	if (PauseMode != 0)
		return;

	if (leveltype == DTYPE_CAVES) {
		if (setlevel && setlvlnum == Quests[Q_PWATER]._qslvl) {
			UpdatePWaterPalette();
		} else {
			palette_update_caves();
		}
	} else if (leveltype == DTYPE_HELL) {
		lighting_color_cycling();
	} else if (leveltype == DTYPE_NEST) {
		palette_update_hive();
	} else if (leveltype == DTYPE_CRYPT) {
		palette_update_crypt();
	}
}

} // namespace devilution
