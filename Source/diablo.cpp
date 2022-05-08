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
#endif
#include "DiabloUI/diabloui.h"
#include "controls/plrctrls.h"
#include "controls/touch/gamepad.h"
#include "controls/touch/renderers.h"
#include "diablo.h"
#include "discord/discord.h"
#include "doom.h"
#include "drlg_l1.h"
#include "drlg_l2.h"
#include "drlg_l3.h"
#include "drlg_l4.h"
#include "dx.h"
#include "encrypt.h"
#include "engine/cel_sprite.hpp"
#include "engine/demomode.h"
#include "engine/load_cel.hpp"
#include "engine/load_file.hpp"
#include "engine/random.hpp"
#include "error.h"
#include "gamemenu.h"
#include "gmenu.h"
#include "help.h"
#include "hwcursor.hpp"
#include "init.h"
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
#include "setmaps.h"
#include "sound.h"
#include "stores.h"
#include "storm/storm_net.hpp"
#include "storm/storm_svid.h"
#include "themes.h"
#include "town.h"
#include "towners.h"
#include "track.h"
#include "trigs.h"
#include "utils/console.h"
#include "utils/display.h"
#include "utils/language.h"
#include "utils/paths.h"
#include "utils/stdcompat/string_view.hpp"
#include "utils/utf8.hpp"

#ifdef __vita__
#include "platform/vita/touch.h"
#endif

#ifdef GPERF_HEAP_FIRST_GAME_ITERATION
#include <gperftools/heap-profiler.h>
#endif

namespace devilution {

uint32_t glSeedTbl[NUMLEVELS];
dungeon_type gnLevelTypeTbl[NUMLEVELS];
Point MousePosition;
bool gbRunGame;
bool gbRunGameResult;
bool ReturnToMainMenu;
bool zoomflag;
/** Enable updating of player character, set to false once Diablo dies */
bool gbProcessPlayers;
bool gbLoadGame;
bool cineflag;
int force_redraw;
int PauseMode;
bool gbBard;
bool gbBarbarian;
bool gbQuietMode = false;
clicktype sgbMouseDown;
uint16_t gnTickDelay = 50;
char gszProductName[64] = "DevilutionX vUnknown";

#ifdef _DEBUG
bool DebugDisableNetworkTimeout = false;
std::vector<std::string> DebugCmdsFromCommandLine;
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
	zoomflag = true;
	CalcViewportGeometry();
	cineflag = false;
	InitCursor();
#ifdef _DEBUG
	LoadDebugGFX();
#endif
	assert(ghMainWnd);
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

	for (auto &player : Players)
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

	assert(MousePosition.y < GetMainPanel().position.y || MousePosition.x < GetMainPanel().position.x || MousePosition.x >= GetMainPanel().position.x + PANEL_WIDTH);

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

	auto &myPlayer = Players[MyPlayerId];
	bNear = myPlayer.position.tile.WalkingDistance(cursPosition) < 2;
	if (pcursitem != -1 && pcurs == CURSOR_HAND && !bShift) {
		NetSendCmdLocParam1(true, invflag ? CMD_GOTOGETITEM : CMD_GOTOAGETITEM, cursPosition, pcursitem);
	} else if (pcursobj != -1 && !Objects[pcursobj].IsDisabled() && (!bShift || (bNear && Objects[pcursobj]._oBreak == 1))) {
		LastMouseButtonAction = MouseActionType::OperateObject;
		NetSendCmdLocParam1(true, pcurs == CURSOR_DISARM ? CMD_DISARMXY : CMD_OPOBJXY, cursPosition, pcursobj);
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
		} else if (pcursplr != -1 && !gbFriendlyMode) {
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
		} else if (pcursplr != -1 && !gbFriendlyMode) {
			LastMouseButtonAction = MouseActionType::AttackPlayerTarget;
			NetSendCmdParam1(true, CMD_ATTACKPID, pcursplr);
		}
	}
	if (!bShift && pcursitem == -1 && pcursobj == -1 && pcursmonst == -1 && pcursplr == -1) {
		LastMouseButtonAction = MouseActionType::Walk;
		NetSendCmdLoc(MyPlayerId, true, CMD_WALKXY, cursPosition);
	}
}

void LeftMouseDown(int wParam)
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

	bool isShiftHeld = (wParam & DVL_MK_SHIFT) != 0;
	bool isCtrlHeld = (wParam & DVL_MK_CTRL) != 0;

	if (!GetMainPanel().Contains(MousePosition)) {
		if (!gmenu_is_active() && !TryIconCurs()) {
			if (QuestLogIsOpen && GetLeftPanel().Contains(MousePosition)) {
				QuestlogESC();
			} else if (qtextflag) {
				qtextflag = false;
				stream_stop();
			} else if (chrflag && GetLeftPanel().Contains(MousePosition)) {
				CheckChrBtns();
			} else if (invflag && GetRightPanel().Contains(MousePosition)) {
				if (!dropGoldFlag)
					CheckInvItem(isShiftHeld, isCtrlHeld);
			} else if (IsStashOpen && GetLeftPanel().Contains(MousePosition)) {
				if (!IsWithdrawGoldOpen)
					CheckStashItem(MousePosition, isShiftHeld, isCtrlHeld);
				CheckStashButtonPress(MousePosition);
			} else if (sbookflag && GetRightPanel().Contains(MousePosition)) {
				CheckSBook();
			} else if (!MyPlayer->HoldItem.isEmpty()) {
				Point currentPosition = MyPlayer->position.tile;
				if (CanPut(currentPosition, GetDirection(currentPosition, cursPosition))) {
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

void LeftMouseUp(int wParam)
{
	gmenu_left_mouse(false);
	control_release_talk_btn();
	bool isShiftHeld = (wParam & (DVL_MK_SHIFT | DVL_MK_LBUTTON)) != 0;
	if (panbtndown)
		CheckBtnUp();
	CheckStashButtonRelease(MousePosition);
	if (chrbtnactive)
		ReleaseChrBtns(isShiftHeld);
	if (lvlbtndown)
		ReleaseLvlBtn();
	if (stextflag != STORE_NONE)
		ReleaseStoreBtn();
}

void RightMouseDown(bool isShiftHeld)
{
	LastMouseButtonAction = MouseActionType::None;

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
	if (sbookflag && GetRightPanel().Contains(MousePosition))
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

bool PressSysKey(int wParam)
{
	if (gmenu_is_active() || wParam != DVL_VK_F10)
		return false;
	DiabloHotkeyMsg(1);
	return true;
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
		if (!chrflag && !QuestLogIsOpen && !IsStashOpen && (invflag || sbookflag) && MousePosition.x < 480 && MousePosition.y < PANEL_TOP) {
			SetCursorPos(MousePosition + Displacement { 160, 0 });
		} else if (!invflag && !sbookflag && (chrflag || QuestLogIsOpen) && MousePosition.x > 160 && MousePosition.y < PANEL_TOP) {
			SetCursorPos(MousePosition - Displacement { 160, 0 });
		}
	}
	CloseInventory();
	chrflag = false;
	sbookflag = false;
	QuestLogIsOpen = false;
}

void PressKey(int vkey)
{
	if (vkey == DVL_VK_PAUSE) {
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
		if (vkey == DVL_VK_RETURN) {
			if (GetAsyncKeyState(DVL_VK_MENU)) {
				sgOptions.Graphics.fullscreen.SetValue(!IsFullScreen());
				SaveOptions();
			} else {
				control_type_message();
			}
		}
		if (vkey != DVL_VK_ESCAPE) {
			return;
		}
	}
	if (vkey == DVL_VK_ESCAPE) {
		if (!PressEscKey()) {
			LastMouseButtonAction = MouseActionType::None;
			gamemenu_on();
		}
		return;
	}

	if (sgnTimeoutCurs != CURSOR_NONE || dropGoldFlag || IsWithdrawGoldOpen) {
		return;
	}

	sgOptions.Keymapper.KeyPressed(vkey);

	if (PauseMode == 2) {
		if (vkey == DVL_VK_RETURN && GetAsyncKeyState(DVL_VK_MENU)) {
			sgOptions.Graphics.fullscreen.SetValue(!IsFullScreen());
			SaveOptions();
		}
		return;
	}

	if (vkey == DVL_VK_RETURN) {
		if (GetAsyncKeyState(DVL_VK_MENU)) {
			sgOptions.Graphics.fullscreen.SetValue(!IsFullScreen());
			SaveOptions();
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
		} else if (ChatLogFlag) {
			ChatLogScrollUp();
		} else if (AutomapActive) {
			AutomapUp();
		} else if (IsStashOpen) {
			Stash.PreviousPage();
		}
	} else if (vkey == DVL_VK_DOWN) {
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
	} else if (vkey == DVL_VK_PRIOR) {
		if (stextflag != STORE_NONE) {
			StorePrior();
		} else if (ChatLogFlag) {
			ChatLogScrollTop();
		}
	} else if (vkey == DVL_VK_NEXT) {
		if (stextflag != STORE_NONE) {
			StoreNext();
		} else if (ChatLogFlag) {
			ChatLogScrollBottom();
		}
	} else if (vkey == DVL_VK_LEFT) {
		if (AutomapActive && !talkflag) {
			AutomapLeft();
		}
	} else if (vkey == DVL_VK_RIGHT) {
		if (AutomapActive && !talkflag) {
			AutomapRight();
		}
	}
}

/**
 * @internal `return` must be used instead of `break` to be bin exact as C++
 */
void PressChar(char vkey)
{
	if (gmenu_is_active() || IsTalkActive() || sgnTimeoutCurs != CURSOR_NONE || MyPlayerIsDead) {
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
	if (IsWithdrawGoldOpen) {
		WithdrawGoldKeyPress(vkey);
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
	case 'M':
		NextDebugMonster();
		return;
	case 'm':
		GetDebugMonster();
		return;
#endif
	}
}

void GetMousePos(int32_t lParam)
{
	MousePosition = { (std::int16_t)(lParam & 0xffff), (std::int16_t)((lParam >> 16) & 0xffff) };
}

void GameEventHandler(uint32_t uMsg, int32_t wParam, int32_t lParam)
{
	switch (uMsg) {
	case DVL_WM_KEYDOWN:
		PressKey(wParam);
		return;
	case DVL_WM_KEYUP:
		ReleaseKey(wParam);
		return;
	case DVL_WM_CHAR:
		PressChar((char)wParam);
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
		GetMousePos(lParam);
		if (sgbMouseDown == CLICK_NONE) {
			sgbMouseDown = CLICK_LEFT;
			LeftMouseDown(wParam);
		}
		return;
	case DVL_WM_LBUTTONUP:
		GetMousePos(lParam);
		if (sgbMouseDown == CLICK_LEFT) {
			LastMouseButtonAction = MouseActionType::None;
			sgbMouseDown = CLICK_NONE;
			LeftMouseUp(wParam);
		}
		return;
	case DVL_WM_RBUTTONDOWN:
		GetMousePos(lParam);
		if (sgbMouseDown == CLICK_NONE) {
			sgbMouseDown = CLICK_RIGHT;
			RightMouseDown((wParam & DVL_MK_SHIFT) != 0);
		}
		return;
	case DVL_WM_RBUTTONUP:
		GetMousePos(lParam);
		if (sgbMouseDown == CLICK_RIGHT) {
			LastMouseButtonAction = MouseActionType::None;
			sgbMouseDown = CLICK_NONE;
		}
		return;
	case DVL_WM_MBUTTONDOWN:
		sgOptions.Keymapper.KeyPressed(DVL_VK_MBUTTON);
		return;
	case DVL_WM_MBUTTONUP:
		sgOptions.Keymapper.KeyReleased(DVL_VK_MBUTTON);
		return;
	case DVL_WM_X1BUTTONDOWN:
		sgOptions.Keymapper.KeyPressed(DVL_VK_X1BUTTON);
		return;
	case DVL_WM_X1BUTTONUP:
		sgOptions.Keymapper.KeyReleased(DVL_VK_X1BUTTON);
		return;
	case DVL_WM_X2BUTTONDOWN:
		sgOptions.Keymapper.KeyPressed(DVL_VK_X2BUTTON);
		return;
	case DVL_WM_X2BUTTONUP:
		sgOptions.Keymapper.KeyReleased(DVL_VK_X2BUTTON);
		return;
	case DVL_WM_CAPTURECHANGED:
		sgbMouseDown = CLICK_NONE;
		LastMouseButtonAction = MouseActionType::None;
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
		LastMouseButtonAction = MouseActionType::None;
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

void RunGameLoop(interface_mode uMsg)
{
	demo::NotifyGameLoopStart();

	WNDPROC saveProc;
	tagMSG msg;

	nthread_ignore_mutex(true);
	StartGame(uMsg);
	assert(ghMainWnd);
	saveProc = SetWindowProc(GameEventHandler);
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
		pfile_write_hero(/*writeGameData=*/false, /*clearTables=*/true);
		sfile_write_stash();
	}

	PaletteFadeOut(8);
	NewCursor(CURSOR_NONE);
	ClearScreenBuffer();
	force_redraw = 255;
	scrollrt_draw_game_screen();
	saveProc = SetWindowProc(saveProc);
	assert(saveProc == GameEventHandler);
	FreeGame();

	if (cineflag) {
		cineflag = false;
		DoEnding();
	}
}

[[noreturn]] void PrintHelpAndExit()
{
	printInConsole("%s", _(/* TRANSLATORS: Commandline Option */ "Options:\n").c_str());
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "-h, --help", _("Print this message and exit").c_str());
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--version", _("Print the version and exit").c_str());
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--data-dir", _("Specify the folder of diabdat.mpq").c_str());
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--save-dir", _("Specify the folder of save files").c_str());
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--config-dir", _("Specify the location of diablo.ini").c_str());
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "-n", _("Skip startup videos").c_str());
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "-f", _("Display frames per second").c_str());
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--verbose", _("Enable verbose logging").c_str());
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--record <#>", _("Record a demo file").c_str());
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--demo <#>", _("Play a demo file").c_str());
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--timedemo", _("Disable all frame limiting during demo playback").c_str());
	printInConsole("%s", _(/* TRANSLATORS: Commandline Option */ "\nGame selection:\n").c_str());
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--spawn", _("Force Shareware mode").c_str());
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--diablo", _("Force Diablo mode").c_str());
	printInConsole("    %-20s %-30s\n", /* TRANSLATORS: Commandline Option */ "--hellfire", _("Force Hellfire mode").c_str());
	printInConsole("%s", _(/* TRANSLATORS: Commandline Option */ "\nHellfire options:\n").c_str());
#ifdef _DEBUG
	printInConsole("\nDebug options:\n");
	printInConsole("    %-20s %-30s\n", "-i", "Ignore network timeout");
	printInConsole("    %-20s %-30s\n", "+<internal command>", "Pass commands to the engine");
#endif
	printInConsole("%s", _("\nReport bugs at https://github.com/diasurgical/devilutionX/\n").c_str());
	diablo_quit(0);
}

void DiabloParseFlags(int argc, char **argv)
{
#ifdef _DEBUG
	int argumentIndexOfLastCommandPart = -1;
	std::string currentCommand;
#endif
	bool timedemo = false;
	int demoNumber = -1;
	int recordNumber = -1;
	for (int i = 1; i < argc; i++) {
		const string_view arg = argv[i];
		if (arg == "-h" || arg == "--help") {
			PrintHelpAndExit();
		} else if (arg == "--version") {
			printInConsole("%s v%s\n", PROJECT_NAME, PROJECT_VERSION);
			diablo_quit(0);
		} else if (arg == "--data-dir") {
			if (i + 1 == argc) {
				printInConsole("%s requires an argument\n", "--data-dir");
				diablo_quit(0);
			}
			paths::SetBasePath(argv[++i]);
		} else if (arg == "--save-dir") {
			if (i + 1 == argc) {
				printInConsole("%s requires an argument\n", "--save-dir");
				diablo_quit(0);
			}
			paths::SetPrefPath(argv[++i]);
		} else if (arg == "--config-dir") {
			if (i + 1 == argc) {
				printInConsole("%s requires an argument\n", "--config-dir");
				diablo_quit(0);
			}
			paths::SetConfigPath(argv[++i]);
		} else if (arg == "--demo") {
			if (i + 1 == argc) {
				printInConsole("%s requires an argument\n", "--demo");
				diablo_quit(0);
			}
			demoNumber = SDL_atoi(argv[++i]);
			gbShowIntro = false;
		} else if (arg == "--timedemo") {
			timedemo = true;
		} else if (arg == "--record") {
			if (i + 1 == argc) {
				printInConsole("%s requires an argument\n", "--record");
				diablo_quit(0);
			}
			recordNumber = SDL_atoi(argv[++i]);
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
			printInConsole("unrecognized option '%s'\n", argv[i]);
			PrintHelpAndExit();
		}
	}

#ifdef _DEBUG
	if (!currentCommand.empty())
		DebugCmdsFromCommandLine.push_back(currentCommand);
#endif

	if (demoNumber != -1)
		demo::InitPlayBack(demoNumber, timedemo);
	if (recordNumber != -1)
		demo::InitRecording(recordNumber);
}

void DiabloInitScreen()
{
	MousePosition = { gnScreenWidth / 2, gnScreenHeight / 2 };
	if (ControlMode == ControlTypes::KeyboardAndMouse)
		SetCursorPos(MousePosition);
	ScrollInfo.tile = { 0, 0 };
	ScrollInfo.offset = { 0, 0 };
	ScrollInfo._sdir = ScrollDirection::None;

	ClrDiabloMsg();
}

void SetApplicationVersions()
{
	snprintf(gszProductName, sizeof(gszProductName) / sizeof(char), "%s v%s", PROJECT_NAME, PROJECT_VERSION);
	CopyUtf8(gszVersionNumber, fmt::format(_("version {:s}"), PROJECT_VERSION), sizeof(gszVersionNumber) / sizeof(char));
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

void UnstuckChargers()
{
	if (gbIsMultiplayer) {
		for (auto &player : Players) {
			if (!player.plractive)
				continue;
			if (player._pLvlChanging)
				continue;
			if (player.plrlevel != MyPlayer->plrlevel)
				continue;
			if (&player == MyPlayer)
				continue;
			return;
		}
	}
	for (int i = 0; i < ActiveMonsterCount; i++) {
		auto &monster = Monsters[ActiveMonsters[i]];
		if (monster._mmode == MonsterMode::Charge)
			monster._mmode = MonsterMode::Stand;
	}
}

void UpdateMonsterLights()
{
	for (int i = 0; i < ActiveMonsterCount; i++) {
		auto &monster = Monsters[ActiveMonsters[i]];
		if (monster.mlid != NO_LIGHT) {
			if (monster.mlid == Players[MyPlayerId]._plid) { // Fix old saves where some monsters had 0 instead of NO_LIGHT
				monster.mlid = NO_LIGHT;
				continue;
			}

			Light &light = Lights[monster.mlid];
			if (monster.position.tile != light.position.tile) {
				ChangeLightXY(monster.mlid, monster.position.tile);
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
	if (DebugScrollViewEnabled && GetAsyncKeyState(DVL_VK_SHIFT)) {
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
	if (!chrflag && !QuestLogIsOpen && !IsStashOpen && CanPanelsCoverView()) {
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
	if (!invflag && !sbookflag && CanPanelsCoverView()) {
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
	if (!invflag && !sbookflag && CanPanelsCoverView()) {
		if (!QuestLogIsOpen) { // We closed the quest log
			if (MousePosition.x > 160 && MousePosition.y < PANEL_TOP) {
				SetCursorPos(MousePosition - Displacement { 160, 0 });
			}
		} else if (!chrflag) { // We opened the character quest log
			if (MousePosition.x < 480 && MousePosition.y < PANEL_TOP) {
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
	if (!chrflag && !QuestLogIsOpen && CanPanelsCoverView()) {
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
	return Players[MyPlayerId]._pmode == PM_DEATH || MyPlayerIsDead;
}

bool IsGameRunning()
{
	return PauseMode != 2;
}

bool CanPlayerTakeAction()
{
	return !IsPlayerDead() && IsGameRunning();
}

void InitKeymapActions()
{
	for (int i = 0; i < 8; ++i) {
		sgOptions.Keymapper.AddAction(
		    "BeltItem{}",
		    N_("Belt item {}"),
		    N_("Use Belt item."),
		    '1' + i,
		    [i] {
			    auto &myPlayer = Players[MyPlayerId];
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
		    i < 4 ? DVL_VK_F5 + i : DVL_VK_INVALID,
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
	    DVL_VK_F2,
	    [] { gamemenu_save_game(false); },
	    nullptr,
	    [&]() { return !gbIsMultiplayer && CanPlayerTakeAction(); });
	sgOptions.Keymapper.AddAction(
	    "QuickLoad",
	    N_("Quick load"),
	    N_("Loads the game."),
	    DVL_VK_F3,
	    [] { gamemenu_load_game(false); },
	    nullptr,
	    [&]() { return !gbIsMultiplayer && gbValidSaveFile && stextflag == STORE_NONE && IsGameRunning(); });
#ifndef NOEXIT
	sgOptions.Keymapper.AddAction(
	    "QuitGame",
	    N_("Quit game"),
	    N_("Closes the game."),
	    DVL_VK_INVALID,
	    [] { gamemenu_quit_game(false); });
#endif
	sgOptions.Keymapper.AddAction(
	    "StopHero",
	    N_("Stop hero"),
	    N_("Stops walking and cancel pending actions."),
	    DVL_VK_INVALID,
	    [] { Players[MyPlayerId].Stop(); },
	    nullptr,
	    CanPlayerTakeAction);
	sgOptions.Keymapper.AddAction(
	    "Item Highlighting",
	    N_("Item highlighting"),
	    N_("Show/hide items on ground."),
	    DVL_VK_LMENU,
	    [] { AltPressed(true); },
	    [] { AltPressed(false); });
	sgOptions.Keymapper.AddAction(
	    "Toggle Item Highlighting",
	    N_("Toggle item highlighting"),
	    N_("Permanent show/hide items on ground."),
	    DVL_VK_RCONTROL,
	    nullptr,
	    [] { ToggleItemLabelHighlight(); });
	sgOptions.Keymapper.AddAction(
	    "Toggle Automap",
	    N_("Toggle automap"),
	    N_("Toggles if automap is displayed."),
	    DVL_VK_TAB,
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
		    DVL_VK_F9 + i,
		    [i]() { DiabloHotkeyMsg(i); },
		    nullptr,
		    nullptr,
		    i + 1);
	}
	sgOptions.Keymapper.AddAction(
	    "Hide Info Screens",
	    N_("Hide Info Screens"),
	    N_("Hide all info screens."),
	    DVL_VK_SPACE,
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
		    zoomflag = !zoomflag;
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
	    DVL_VK_F1,
	    HelpKeyPressed,
	    nullptr,
	    CanPlayerTakeAction);
	sgOptions.Keymapper.AddAction(
	    "Screenshot",
	    N_("Screenshot"),
	    N_("Takes a screenshot."),
	    DVL_VK_SNAPSHOT,
	    nullptr,
	    CaptureScreen);
	sgOptions.Keymapper.AddAction(
	    "GameInfo",
	    N_("Game info"),
	    N_("Displays game infos."),
	    'V',
	    [] {
		    EventPlrMsg(fmt::format(
		                    _(/* TRANSLATORS: {:s} means: Character Name, Game Version, Game Difficulty. */ "{:s} {:s}"),
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
} // namespace

void FreeGameMem()
{
	pDungeonCels = nullptr;
	pMegaTiles = nullptr;
	pLevelPieces = nullptr;
	pSpecialCels = std::nullopt;

	FreeMonsters();
	FreeMissileGFX();
	FreeObjectGFX();
	FreeMonsterSnd();
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
			InitDungMsgs(Players[MyPlayerId]);
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

	auto &myPlayer = Players[MyPlayerId];

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
			NetSendCmdParam3(true, CMD_TSPELLID, pcursmonst, myPlayer._pTSpell, GetSpellLevel(MyPlayerId, myPlayer._pTSpell));
		else if (pcursplr != -1)
			NetSendCmdParam3(true, CMD_TSPELLPID, pcursplr, myPlayer._pTSpell, GetSpellLevel(MyPlayerId, myPlayer._pTSpell));
		else
			NetSendCmdLocParam2(true, CMD_TSPELLXY, cursPosition, myPlayer._pTSpell, GetSpellLevel(MyPlayerId, myPlayer._pTSpell));
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
		control_drop_gold(DVL_VK_ESCAPE);
		rv = true;
	}

	if (IsWithdrawGoldOpen) {
		WithdrawGoldKeyPress(DVL_VK_ESCAPE);
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

void LoadGameLevel(bool firstflag, lvl_entry lvldir)
{
	_music_id neededTrack;
	if (currlevel >= 17)
		neededTrack = currlevel > 20 ? TMUSIC_L5 : TMUSIC_L6;
	else
		neededTrack = static_cast<_music_id>(leveltype);

	if (neededTrack != sgnMusicTrack)
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
		InitStash();
		InitQuestText();
		InitInfoBoxGfx();
		InitStores();
		InitAutomapOnce();
		InitHelp();
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

		for (auto &player : Players) {
			if (player.plractive && currlevel == player.plrlevel) {
				InitPlayerGFX(player);
				if (lvldir != ENTRY_LOAD)
					InitPlayer(player, firstflag);
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
				[[maybe_unused]] uint32_t mid1Seed = GetLCGEngineState();
				InitGolems();
				InitObjects();
				[[maybe_unused]] uint32_t mid2Seed = GetLCGEngineState();
				IncProgress();
				InitMonsters();
				InitItems();
				if (currlevel < 17)
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
#if !defined(USE_SDL1) && !defined(__vita__)
		InitVirtualGamepadGFX(renderer);
#endif
		InitMissileGFX(gbIsHellfire);
		IncProgress();
		InitCorpses();
		IncProgress();
		FillSolidBlockTbls();
		IncProgress();

		if (lvldir == ENTRY_WARPLVL)
			GetPortalLvlPos();
		IncProgress();

		for (auto &player : Players) {
			if (player.plractive && currlevel == player.plrlevel) {
				InitPlayerGFX(player);
				if (lvldir != ENTRY_LOAD)
					InitPlayer(player, firstflag);
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
				dFlags[player.position.tile.x][player.position.tile.y] |= DungeonFlag::DeadPlayer;
			}
		}
	}

	SetDungeonMicros();

	IncProgress();
	IncProgress();

	if (firstflag) {
		InitControlPan();
	}
	IncProgress();
	UpdateMonsterLights();
	UnstuckChargers();
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

#ifndef USE_SDL1
	ActivateVirtualGamepad();
#endif

	if (sgnMusicTrack != neededTrack)
		music_start(neededTrack);

	if (MinimizePaused) {
		music_mute();
	}

	while (!IncProgress())
		;

	if (!gbIsSpawn && setlevel && setlvlnum == SL_SKELKING && Quests[Q_SKELKING]._qactive == QUEST_ACTIVE)
		PlaySFX(USFX_SKING1);

	// Reset mouse selection of entities
	pcursmonst = -1;
	pcursobj = -1;
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
		ClearLastSendPlayerCmd();

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
