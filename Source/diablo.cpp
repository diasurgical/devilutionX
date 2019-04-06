//HEADER_GOES_HERE

#include "../types.h"

DEVILUTION_BEGIN_NAMESPACE

HWND ghMainWnd;
int glMid1Seed[NUMLEVELS];
int glMid2Seed[NUMLEVELS];
int gnLevelTypeTbl[NUMLEVELS];
int MouseY;             // idb
int MouseX;             // idb
BOOL gbGameLoopStartup; // idb
int glSeedTbl[NUMLEVELS];
BOOL gbRunGame;
int glMid3Seed[NUMLEVELS];
BOOL gbRunGameResult;
int zoomflag; // weak
BOOL gbProcessPlayers;
int glEndSeed[NUMLEVELS];
BOOL gbLoadGame;
HINSTANCE ghInst; // idb
int DebugMonsters[10];
char cineflag;   // weak
int drawpanflag; // weak
int visiondebug; // weak
int scrollflag;  /* unused */
BOOL light4flag;
int leveldebug;      // weak
int monstdebug;      // weak
int trigdebug;       /* unused */
int setseed;         // weak
int debugmonsttypes; // weak
int PauseMode;       // weak
int sgnTimeoutCurs;
char sgbMouseDown;     // weak
int color_cycle_timer; // weak

/* rdata */

BOOL fullscreen = TRUE;
#ifdef _DEBUG
int showintrodebug = 1;
int questdebug = -1;
int debug_mode_key_s;
int debug_mode_key_w;
int debug_mode_key_inverted_v;
int debug_mode_dollar_sign;
int debug_mode_key_d;
int debug_mode_key_i;
int dbgplr;
int dbgqst;
int dbgmon;
int arrowdebug;
int frameflag;
int frameend;
int framerate;
int framestart;
#endif
BOOL FriendlyMode = TRUE;
char *spszMsgTbl[4] = {
	"I need help! Come Here!",
	"Follow me.",
	"Here's something for you.",
	"Now you DIE!"
};                                                      // weak
char *spszMsgKeyTbl[4] = { "F9", "F10", "F11", "F12" }; // weak

void __cdecl FreeGameMem()
{
	music_stop();

	MemFreeDbg(pDungeonCels);
	MemFreeDbg(pMegaTiles);
	MemFreeDbg(pLevelPieces);
	MemFreeDbg(level_special_cel);
	MemFreeDbg(pSpeedCels);

	FreeMissiles();
	FreeMonsters();
	FreeObjectGFX();
	FreeEffects();
	FreeTownerGFX();
}

BOOL __fastcall StartGame(BOOL bNewGame, BOOL bSinglePlayer)
{
	BOOL fExitProgram;
	unsigned int uMsg;

	byte_678640 = 1;

	do {
		fExitProgram = FALSE;
		gbLoadGame = FALSE;

		if (!NetInit(bSinglePlayer, &fExitProgram)) {
			gbRunGameResult = !fExitProgram;
			break;
		}

		byte_678640 = 0;

		if (bNewGame || !gbValidSaveFile) {
			InitLevels();
			InitQuests();
			InitPortals();
			InitDungMsgs(myplr);
		}
		if (!gbValidSaveFile || !gbLoadGame)
			uMsg = WM_DIABNEWGAME;
		else
			uMsg = WM_DIABLOADGAME;

		run_game_loop(uMsg);
		NetClose();
		pfile_create_player_description(0, 0);
	} while (gbRunGameResult);

	SNetDestroy();
	return gbRunGameResult;
}
// 678640: using guessed type char byte_678640;

void __fastcall run_game_loop(unsigned int uMsg)
{
	//int v3; // eax
	BOOLEAN v5; // zf
	//int v6; // eax
	signed int v7;    // [esp+8h] [ebp-24h]
	WNDPROC saveProc; // [esp+Ch] [ebp-20h]
	MSG msg;          // [esp+10h] [ebp-1Ch]

	nthread_ignore_mutex(1);
	start_game(uMsg);
	saveProc = SetWindowProc(GM_Game);
	control_update_life_mana();
	msg_process_net_packets();
	gbRunGame = TRUE;
	gbProcessPlayers = TRUE;
	gbRunGameResult = TRUE;
	drawpanflag = 255;
	DrawAndBlit();
	PaletteFadeIn(8);
	drawpanflag = 255;
	gbGameLoopStartup = 1;
	nthread_ignore_mutex(0);
	while (gbRunGame) {
		diablo_color_cyc_logic();
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				if (msg.message == WM_QUIT) {
					gbRunGameResult = FALSE;
					gbRunGame = FALSE;
					break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (!gbRunGame || (v7 = 1, !nthread_has_500ms_passed(0)))
				v7 = 0;
			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
			v5 = v7 == 0;
		} else {
			//_LOBYTE(v6) = nthread_has_500ms_passed();
			v5 = nthread_has_500ms_passed(0) == 0;
		}
		if (!v5) {
			multi_process_network_packets();
			game_loop(gbGameLoopStartup);
			msgcmd_send_chat();
			gbGameLoopStartup = 0;
			DrawAndBlit();
		}
#ifdef SLEEP
		Sleep(1);
#endif
	}
	if ((unsigned char)gbMaxPlayers > 1u)
		pfile_write_hero();
	pfile_flush_W();
	PaletteFadeOut(8);
	SetCursor_(CURSOR_NONE);
	ClearScreenBuffer();
	drawpanflag = 255;
	scrollrt_draw_game_screen(1);
	SetWindowProc(saveProc);
	free_game();
	if (cineflag) {
		cineflag = FALSE;
		DoEnding();
	}
}
// 525718: using guessed type char cineflag;
// 52571C: using guessed type int drawpanflag;
// 679660: using guessed type char gbMaxPlayers;

void __fastcall start_game(unsigned int uMsg)
{
	cineflag = FALSE;
	zoomflag = 1;
	InitCursor();
	InitLightTable();
	LoadDebugGFX();
	music_stop();
	ShowProgress(uMsg);
	gmenu_init_menu();
	InitLevelCursor();
	sgnTimeoutCurs = 0;
	sgbMouseDown = 0;
	track_repeat_walk(0);
}
// 52569C: using guessed type int zoomflag;
// 525718: using guessed type char cineflag;
// 525748: using guessed type char sgbMouseDown;

void __cdecl free_game()
{
	int i; // esi

	FreeControlPan();
	FreeInvGFX();
	FreeGMenu();
	FreeQuestText();
	FreeStoreMem();

	for (i = 0; i < MAX_PLRS; i++)
		FreePlayerGFX(i);

	FreeItemGFX();
	FreeCursor();
	FreeLightTable();
	FreeDebugGFX();
	FreeGameMem();
}

BOOL __cdecl diablo_get_not_running()
{
	SetLastError(0);
	CreateEvent(NULL, FALSE, FALSE, "DiabloEvent");
	return GetLastError() != ERROR_ALREADY_EXISTS;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HINSTANCE hInst;
	int nData;
	char szFileName[MAX_PATH];
	BOOL bNoEvent;

	hInst = hInstance;
#ifndef DEBUGGER
	diablo_reload_process(hInstance);
#endif
	ghInst = hInst;

	if (RestrictedTest())
		ErrOkDlg(IDD_DIALOG10, 0, "C:\\Src\\Diablo\\Source\\DIABLO.CPP", 877);
	if (ReadOnlyTest()) {
		if (!GetModuleFileName(ghInst, szFileName, sizeof(szFileName)))
			szFileName[0] = '\0';
		DirErrorDlg(szFileName);
	}

	ShowCursor(FALSE);
	srand(GetTickCount());
	InitHash();
	fault_get_filter();

	bNoEvent = diablo_get_not_running();
	if (!diablo_find_window("DIABLO") && bNoEvent) {
#ifdef _DEBUG
		SFileEnableDirectAccess(TRUE);
#endif
		diablo_init_screen();
		diablo_parse_flags(lpCmdLine);
		init_create_window(nCmdShow);
		sound_init();
		UiInitialize();

#ifdef _DEBUG
		if (showintrodebug)
#endif
			play_movie("gendata\\logo.smk", TRUE);

		{
			char szValueName[] = "Intro";
			if (!SRegLoadValue("Diablo", szValueName, 0, &nData))
				nData = 1;
			if (nData)
				play_movie("gendata\\diablo1.smk", TRUE);
			SRegSaveValue("Diablo", szValueName, 0, 0);
		}

#ifdef _DEBUG
		if (showintrodebug) {
#endif
			UiTitleDialog(7);
			BlackPalette();
#ifdef _DEBUG
		}
#endif

		mainmenu_loop();
		UiDestroy();
		SaveGamma();

		if (ghMainWnd) {
			Sleep(300);
			DestroyWindow(ghMainWnd);
		}
	}

	return FALSE;
}

void __fastcall diablo_parse_flags(char *args)
{
#ifdef _DEBUG
	int n;   // edi
	int v15; // eax
#endif
	while (*args) {
		for (; isspace(*args); ++args)
			;
		if (!_strcmpi("dd_emulate", args)) {
			gbEmulate = 1;
			args += strlen("dd_emulate");
		} else if (!_strcmpi("dd_backbuf", args)) {
			gbBackBuf = 1;
			args += strlen("dd_backbuf");
		} else if (!_strcmpi("ds_noduplicates", args)) {
			gbDupSounds = FALSE;
			args += strlen("ds_noduplicates");
		} else {
#ifdef _DEBUG
			switch (tolower(*args++)) {
			case '^': // god mod with all spells as skills
				debug_mode_key_inverted_v = 1;
				break;
			case '$': // demi-god
				debug_mode_dollar_sign = 1;
				break;
			/*case 'b': // enable drop log
					debug_mode_key_b = 1;
					break;*/
			case 'd': // no startup video+???
				showintrodebug = 0;
				debug_mode_key_d = 1;
				break;
			case 'f': // draw fps
				EnableFrameCount();
				break;
			case 'i': // disable network timeout
				debug_mode_key_i = 1;
				break;
			/*case 'j': // <level>: init trigger at level
					for ( ; isspace(*args); ++args )
						;
					for ( n = 0; isdigit(*args); n = v15 + 10 * n - 48 )
						v15 = *args++;
					debug_mode_key_J_trigger = n;
					break;*/
			case 'l': // <dtype> <level>: start in level as type
				setlevel = 0;
				for (leveldebug = 1; isspace(*args); ++args)
					;
				for (n = 0; isdigit(*args); n = v15 + 10 * n - 48)
					v15 = *args++;
				for (leveltype = n; isspace(*args); ++args)
					;
				for (n = 0; isdigit(*args); n = v15 + 10 * n - 48)
					v15 = *args++;
				currlevel = n;
				plr[0].plrlevel = n;
				break;
			case 'm': // <mtype>: add debug monster, up to 10 allowed
				for (monstdebug = 1; isspace(*args); ++args)
					;
				for (n = 0; isdigit(*args); n = v15 + 10 * n - 48)
					v15 = *args++;
				DebugMonsters[debugmonsttypes++] = n;
				break;
			case 'n': // disable startup video
				showintrodebug = 0;
				break;
			case 'q': // <qnum>: force a certain quest
				for (; isspace(*args); ++args)
					;
				for (n = 0; isdigit(*args); n = v15 + 10 * n - 48)
					v15 = *args++;
				questdebug = n;
				break;
			case 'r': // <seed>: set map seed to
				for (; isspace(*args); ++args)
					;
				for (n = 0; isdigit(*args); n = v15 + 10 * n - 48)
					v15 = *args++;
				setseed = n;
				break;
			case 's': // unused
				debug_mode_key_s = 1;
				break;
			case 't': // <qlvl>: sets current quest level
				leveldebug = 1;
				for (setlevel = 1; isspace(*args); ++args)
					;
				for (n = 0; isdigit(*args); n = v15 + 10 * n - 48)
					v15 = *args++;
				setlvlnum = n;
				break;
			case 'v': // draw yellow debug tiles
				visiondebug = 1;
				break;
			case 'w': // rest of the cheats, some only in town
				debug_mode_key_w = 1;
				break;
			case 'x':
				fullscreen = FALSE;
				break;
			}
#else
			tolower(*args++);
#endif
		}
	}
}
// 52A548: using guessed type char gbBackBuf;
// 52A549: using guessed type char gbEmulate;

void __cdecl diablo_init_screen()
{
	int i;

	MouseX = 320;
	MouseY = 240;
	ScrollInfo._sdx = 0;
	ScrollInfo._sdy = 0;
	ScrollInfo._sxoff = 0;
	ScrollInfo._syoff = 0;
	ScrollInfo._sdir = SDIR_NONE;

	for (i = 0; i < 1024; i++)
		screen_y_times_768[i] = i * 768;

	ClrDiabloMsg();
}

BOOL __fastcall diablo_find_window(LPCSTR lpClassName)
{
	HWND result; // eax
	HWND v2;     // esi
	HWND v3;     // eax
	HWND v4;     // edi

	result = FindWindow(lpClassName, 0);
	v2 = result;
	if (!result)
		return 0;

	v3 = GetLastActivePopup(result);
	if (v3)
		v2 = v3;
	v4 = GetTopWindow(v2);
	if (!v4)
		v4 = v2;
	SetForegroundWindow(v2);
	SetFocus(v4);
	return 1;
}

void __fastcall diablo_reload_process(HMODULE hModule)
{
	char *i;                // eax
	DWORD dwSize;           // esi
	BOOL v3;                // edi
	_DWORD *v4;             // eax
	_DWORD *v5;             // esi
	HWND v6;                // eax
	char Name[276];         // [esp+Ch] [ebp-29Ch]
	char Filename[260];     // [esp+120h] [ebp-188h]
	STARTUPINFOA si;        // [esp+224h] [ebp-84h]
	SYSTEM_INFO sinf;       // [esp+268h] [ebp-40h]
	PROCESS_INFORMATION pi; // [esp+28Ch] [ebp-1Ch]
	DWORD dwProcessId;      // [esp+29Ch] [ebp-Ch]
	HANDLE hMap;            // [esp+2A0h] [ebp-8h]
	HWND hWnd;              // [esp+2A4h] [ebp-4h]

	//*Filename = empty_string;
	memset(Filename, 0, sizeof(Filename));
	//	*(_WORD *)&Filename[257] = 0;
	//	Filename[259] = 0;
	GetModuleFileName(hModule, Filename, 0x104u);
	wsprintf(Name, "Reload-%s", Filename);
	for (i = Name; *i; ++i) {
		if (*i == '\\')
			*i = '/';
	}
	GetSystemInfo(&sinf);
	dwSize = sinf.dwPageSize;
	if (sinf.dwPageSize < 4096)
		dwSize = 4096;
	hMap = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, SEC_COMMIT | PAGE_READWRITE, 0, dwSize, Name);
	v3 = GetLastError() != ERROR_ALREADY_EXISTS;
	if (hMap) {
		v4 = (unsigned int *)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, dwSize);
		v5 = v4;
		if (v4) {
			if (v3) {
				*v4 = -1;
				v4[1] = 0;
				memset(&si, 0, sizeof(si));
				si.cb = sizeof(si);
				CreateProcess(Filename, NULL, NULL, NULL, FALSE, CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi);
				WaitForInputIdle(pi.hProcess, 0xFFFFFFFF);
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
				while (*v5 < 0)
					Sleep(1000);
				UnmapViewOfFile(v5);
				CloseHandle(hMap);
				ExitProcess(0);
			}
			if (InterlockedIncrement((LPLONG)v4)) {
				v6 = GetForegroundWindow();
				do {
					hWnd = v6;
					v6 = GetWindow(v6, 3u);
				} while (v6);
				while (1) {
					GetWindowThreadProcessId(hWnd, &dwProcessId);
					if (dwProcessId == v5[1])
						break;
					hWnd = GetWindow(hWnd, 2u);
					if (!hWnd)
						goto LABEL_23;
				}
				SetForegroundWindow(hWnd);
			LABEL_23:
				UnmapViewOfFile(v5);
				CloseHandle(hMap);
				ExitProcess(0);
			}
			v5[1] = GetCurrentProcessId();
		}
	}
}

BOOL __cdecl PressEscKey()
{
	BOOL rv = FALSE;

	if (doomflag) {
		doom_close();
		rv = TRUE;
	}
	if (helpflag) {
		helpflag = 0;
		rv = TRUE;
	}

	if (qtextflag) {
		qtextflag = 0;
		sfx_stop();
		rv = TRUE;
	} else if (stextflag) {
		STextESC();
		rv = TRUE;
	}

	if (msgflag) {
		msgdelay = 0;
		rv = TRUE;
	}
	if (talkflag) {
		control_reset_talk();
		rv = TRUE;
	}
	if (dropGoldFlag) {
		control_drop_gold(VK_ESCAPE);
		rv = TRUE;
	}
	if (spselflag) {
		spselflag = 0;
		rv = TRUE;
	}

	return rv;
}
// 4B84DC: using guessed type int dropGoldFlag;
// 4B8960: using guessed type int talkflag;
// 4B8C98: using guessed type int spselflag;
// 52575C: using guessed type int doomflag;
// 52B9F0: using guessed type char msgdelay;
// 52B9F1: using guessed type char msgflag;
// 646D00: using guessed type char qtextflag;
// 6AA705: using guessed type char stextflag;

LRESULT CALLBACK DisableInputWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	case WM_SYSKEYDOWN:
	case WM_SYSCOMMAND:
	case WM_MOUSEMOVE:
		return 0;
	case WM_LBUTTONDOWN:
		if (sgbMouseDown == 0) {
			sgbMouseDown = 1;
			SetCapture(hWnd);
		}
		return 0;
	case WM_LBUTTONUP:
		if (sgbMouseDown == 1) {
			sgbMouseDown = 0;
			ReleaseCapture();
		}
		return 0;
	case WM_RBUTTONDOWN:
		if (sgbMouseDown == 0) {
			sgbMouseDown = 2;
			SetCapture(hWnd);
		}
		return 0;
	case WM_RBUTTONUP:
		if (sgbMouseDown == 2) {
			sgbMouseDown = 0;
			ReleaseCapture();
		}
		return 0;
	case WM_CAPTURECHANGED:
		if (hWnd != (HWND)lParam)
			sgbMouseDown = 0;
		return 0;
	}

	return MainWndProc(hWnd, uMsg, wParam, lParam);
}
// 525748: using guessed type char sgbMouseDown;

LRESULT CALLBACK GM_Game(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_KEYDOWN:
		PressKey(wParam);
		return 0;
	case WM_KEYUP:
		ReleaseKey(wParam);
		return 0;
	case WM_CHAR:
		PressChar(wParam);
		return 0;
	case WM_SYSKEYDOWN:
		if (PressSysKey(wParam))
			return 0;
		break;
	case WM_SYSCOMMAND:
		if (wParam == SC_CLOSE) {
			gbRunGame = FALSE;
			gbRunGameResult = FALSE;
			return 0;
		}
		break;
	case WM_MOUSEMOVE:
		MouseX = LOWORD(lParam);
		MouseY = HIWORD(lParam);
		gmenu_on_mouse_move(LOWORD(lParam));
		return 0;
	case WM_LBUTTONDOWN:
		MouseX = LOWORD(lParam);
		MouseY = HIWORD(lParam);
		if (sgbMouseDown == 0) {
			sgbMouseDown = 1;
			SetCapture(hWnd);
			track_repeat_walk(LeftMouseDown(wParam));
		}
		return 0;
	case WM_LBUTTONUP:
		MouseX = LOWORD(lParam);
		MouseY = HIWORD(lParam);
		if (sgbMouseDown == 1) {
			sgbMouseDown = 0;
			LeftMouseUp();
			track_repeat_walk(FALSE);
			ReleaseCapture();
		}
		return 0;
	case WM_RBUTTONDOWN:
		MouseX = LOWORD(lParam);
		MouseY = HIWORD(lParam);
		if (sgbMouseDown == 0) {
			sgbMouseDown = 2;
			SetCapture(hWnd);
			RightMouseDown();
		}
		return 0;
	case WM_RBUTTONUP:
		MouseX = LOWORD(lParam);
		MouseY = HIWORD(lParam);
		if (sgbMouseDown == 2) {
			sgbMouseDown = 0;
			ReleaseCapture();
		}
		return 0;
	case WM_CAPTURECHANGED:
		if (hWnd != (HWND)lParam) {
			sgbMouseDown = 0;
			track_repeat_walk(FALSE);
		}
		break;
	case WM_DIABNEXTLVL:
	case WM_DIABPREVLVL:
	case WM_DIABRTNLVL:
	case WM_DIABSETLVL:
	case WM_DIABWARPLVL:
	case WM_DIABTOWNWARP:
	case WM_DIABTWARPUP:
	case WM_DIABRETOWN:
		if (gbMaxPlayers > 1)
			pfile_write_hero();
		nthread_ignore_mutex(TRUE);
		PaletteFadeOut(8);
		FreeMonsterSnd();
		music_stop();
		track_repeat_walk(FALSE);
		sgbMouseDown = 0;
		ReleaseCapture();
		ShowProgress(uMsg);
		drawpanflag = 255;
		DrawAndBlit();
		if (gbRunGame)
			PaletteFadeIn(8);
		nthread_ignore_mutex(FALSE);
		gbGameLoopStartup = TRUE;
		return 0;
	}

	return MainWndProc(hWnd, uMsg, wParam, lParam);
}
// 52571C: using guessed type int drawpanflag;
// 525748: using guessed type char sgbMouseDown;
// 679660: using guessed type char gbMaxPlayers;

BOOL __fastcall LeftMouseDown(int wParam)
{
	if (gmenu_left_mouse(1) || control_check_talk_btn() || sgnTimeoutCurs)
		return 0;
	if (deathflag) {
		control_check_btn_press();
		return 0;
	}
	if (PauseMode == 2)
		return 0;
	if (doomflag) {
		doom_close();
		return 0;
	}
	if (spselflag) {
		SetSpell();
		return 0;
	}
	if (stextflag) {
		CheckStoreBtn();
		return 0;
	}
	if (MouseY >= 352) {
		if (!talkflag && !dropGoldFlag) {
			if (!gmenu_exception())
				CheckInvScrn();
		}
		DoPanBtn();
		if (pcurs <= 1 || pcurs >= 12)
			return 0;
		goto LABEL_48;
	}
	if (gmenu_exception() || TryIconCurs())
		return 0;
	if (questlog && MouseX > 32 && MouseX < 288 && MouseY > 32 && MouseY < 308) {
		QuestlogESC();
		return 0;
	}
	if (qtextflag) {
		qtextflag = FALSE;
		sfx_stop();
		return 0;
	}
	if (chrflag && MouseX < 320) {
		CheckChrBtns();
		return 0;
	}
	if (invflag && MouseX > 320) {
		if (!dropGoldFlag)
			CheckInvItem();
		return 0;
	}
	if (sbookflag && MouseX > 320) {
		CheckSBook();
		return 0;
	}
	if (pcurs >= CURSOR_FIRSTITEM) {
		if (!TryInvPut())
			return 0;
		NetSendCmdPItem(TRUE, CMD_PUTITEM, cursmx, cursmy);
	LABEL_48:
		SetCursor_(CURSOR_HAND);
		return 0;
	}
	if (plr[myplr]._pStatPts && !spselflag)
		CheckLvlBtn();
	if (!lvlbtndown)
		return LeftMouseCmd(wParam == MK_SHIFT + MK_LBUTTON);
	return 0;
}
// 484368: using guessed type int FriendlyMode;
// 4B84DC: using guessed type int dropGoldFlag;
// 4B851C: using guessed type int lvlbtndown;
// 4B8960: using guessed type int talkflag;
// 4B8968: using guessed type int sbookflag;
// 4B8C98: using guessed type int spselflag;
// 4B8CC0: using guessed type char pcursitem;
// 4B8CC1: using guessed type char pcursobj;
// 4B8CC2: using guessed type char pcursplr;
// 525740: using guessed type int PauseMode;
// 52575C: using guessed type int doomflag;
// 646D00: using guessed type char qtextflag;
// 69BD04: using guessed type int questlog;
// 6AA705: using guessed type char stextflag;

BOOL __fastcall LeftMouseCmd(BOOL bShift)
{
	BOOL bNear;

	if (!leveltype) {
		if (pcursitem != -1 && pcurs == CURSOR_HAND)
			NetSendCmdLocParam1(TRUE, invflag ? CMD_GOTOGETITEM : CMD_GOTOAGETITEM, cursmx, cursmy, pcursitem);
		if (pcursmonst != -1)
			NetSendCmdLocParam1(TRUE, CMD_TALKXY, cursmx, cursmy, pcursmonst);
		if (pcursitem == -1 && pcursmonst == -1 && pcursplr == -1)
			return TRUE;
	} else {
		bNear = abs(plr[myplr].WorldX - cursmx) < 2 && abs(plr[myplr].WorldY - cursmy) < 2;
		if (pcursitem != -1 && pcurs == CURSOR_HAND && !bShift) {
			NetSendCmdLocParam1(pcurs, invflag ? CMD_GOTOGETITEM : CMD_GOTOAGETITEM, cursmx, cursmy, pcursitem);
		} else if (pcursobj != -1 && (!bShift || bNear && object[pcursobj]._oBreak == 1)) {
			NetSendCmdLocParam1(TRUE, pcurs == CURSOR_DISARM ? CMD_DISARMXY : CMD_OPOBJXY, cursmx, cursmy, pcursobj);
		} else if (plr[myplr]._pwtype == WT_RANGED) {
			if (bShift) {
				NetSendCmdLoc(TRUE, CMD_RATTACKXY, cursmx, cursmy);
			} else if (pcursmonst != -1) {
				if (CanTalkToMonst(pcursmonst)) {
					NetSendCmdParam1(TRUE, CMD_ATTACKID, pcursmonst);
				} else {
					NetSendCmdParam1(TRUE, CMD_RATTACKID, pcursmonst);
				}
			} else if (pcursplr != -1 && !FriendlyMode) {
				NetSendCmdParam1(TRUE, CMD_RATTACKPID, pcursplr);
			}
		} else {
			if (bShift) {
				if (pcursmonst != -1) {
					if (CanTalkToMonst(pcursmonst)) {
						NetSendCmdParam1(TRUE, CMD_ATTACKID, pcursmonst);
					} else {
						NetSendCmdLoc(TRUE, CMD_SATTACKXY, cursmx, cursmy);
					}
				} else {
					NetSendCmdLoc(TRUE, CMD_SATTACKXY, cursmx, cursmy);
				}
			} else if (pcursmonst != -1) {
				NetSendCmdParam1(TRUE, CMD_ATTACKID, pcursmonst);
			} else if (pcursplr != -1 && !FriendlyMode) {
				NetSendCmdParam1(TRUE, CMD_ATTACKPID, pcursplr);
			}
		}
		if (!bShift && pcursitem == -1 && pcursobj == -1 && pcursmonst == -1 && pcursplr == -1)
			return TRUE;
	}

	return FALSE;
}

BOOLEAN __cdecl TryIconCurs()
{
	unsigned char v0; // dl
	int v1;           // edx
	int v2;           // eax
	int v3;           // eax
	int v4;           // ST0C_4
	int v5;           // eax

	switch (pcurs) {
	case CURSOR_RESURRECT:
		v0 = CMD_RESURRECT;
	LABEL_3:
		NetSendCmdParam1(TRUE, v0, pcursplr);
		return 1;
	case CURSOR_HEALOTHER:
		v0 = CMD_HEALOTHER;
		goto LABEL_3;
	case CURSOR_TELEKINESIS:
		DoTelekinesis();
		return 1;
	case CURSOR_IDENTIFY:
		if (pcursinvitem != -1) {
			CheckIdentify(myplr, pcursinvitem);
			return 1;
		}
	LABEL_26:
		SetCursor_(CURSOR_HAND);
		return 1;
	case CURSOR_REPAIR:
		if (pcursinvitem != -1) {
			DoRepair(myplr, pcursinvitem);
			return 1;
		}
		goto LABEL_26;
	case CURSOR_RECHARGE:
		if (pcursinvitem != -1) {
			DoRecharge(myplr, pcursinvitem);
			return 1;
		}
		goto LABEL_26;
	case CURSOR_TELEPORT:
		v1 = plr[myplr]._pTSpell;
		if (pcursmonst == -1) {
			if (pcursplr == -1) {
				v4 = GetSpellLevel(myplr, v1);
				v5 = 21720 * myplr;
				_LOWORD(v5) = plr[myplr]._pTSpell;
				NetSendCmdLocParam2(TRUE, CMD_TSPELLXY, cursmx, cursmy, v5, v4);
			} else {
				v3 = GetSpellLevel(myplr, v1);
				NetSendCmdParam3(TRUE, CMD_TSPELLPID, pcursplr, plr[myplr]._pTSpell, v3);
			}
		} else {
			v2 = GetSpellLevel(myplr, v1);
			NetSendCmdParam3(TRUE, CMD_TSPELLID, pcursmonst, plr[myplr]._pTSpell, v2);
		}
		goto LABEL_26;
	}
	if (pcurs == CURSOR_DISARM && pcursobj == -1)
		goto LABEL_26;
	return 0;
}
// 4B8CB8: using guessed type char pcursinvitem;
// 4B8CC1: using guessed type char pcursobj;
// 4B8CC2: using guessed type char pcursplr;

void __cdecl LeftMouseUp()
{
	gmenu_left_mouse(0);
	control_release_talk_btn();
	if (panbtndown)
		CheckBtnUp();
	if (chrbtnactive)
		ReleaseChrBtns();
	if (lvlbtndown)
		ReleaseLvlBtn();
	if (stextflag)
		ReleaseStoreBtn();
}
// 4B851C: using guessed type int lvlbtndown;
// 4B8C90: using guessed type int panbtndown;
// 6AA705: using guessed type char stextflag;

void __cdecl RightMouseDown()
{
	if (!gmenu_exception() && sgnTimeoutCurs == CURSOR_NONE && PauseMode != 2 && !plr[myplr]._pInvincible) {
		if (doomflag) {
			doom_close();
		} else if (!stextflag) {
			if (spselflag) {
				SetSpell();
			} else if (MouseY >= 352
			    || (!sbookflag || MouseX <= 320)
			        && !TryIconCurs()
			        && (pcursinvitem == -1 || !UseInvItem(myplr, pcursinvitem))) {
				if (pcurs == 1) {
					if (pcursinvitem == -1 || !UseInvItem(myplr, pcursinvitem))
						CheckPlrSpell();
				} else if (pcurs > 1 && pcurs < 12) {
					SetCursor_(CURSOR_HAND);
				}
			}
		}
	}
}
// 4B8968: using guessed type int sbookflag;
// 4B8C98: using guessed type int spselflag;
// 4B8CB8: using guessed type char pcursinvitem;
// 525740: using guessed type int PauseMode;
// 52575C: using guessed type int doomflag;
// 6AA705: using guessed type char stextflag;

BOOL __fastcall PressSysKey(int wParam)
{
	if (gmenu_exception() || wParam != VK_F10)
		return FALSE;
	diablo_hotkey_msg(1);
	return TRUE;
}

void __fastcall diablo_hotkey_msg(int dwMsg)
{
	int v1;                  // esi
	char *v2;                // eax
	char Filename[260];      // [esp+4h] [ebp-154h]
	char ReturnedString[80]; // [esp+108h] [ebp-50h]

	v1 = dwMsg;
	if (gbMaxPlayers != 1) {
		if (!GetModuleFileName(ghInst, Filename, 0x104u))
			app_fatal("Can't get program name");
		v2 = strrchr(Filename, '\\');
		if (v2)
			*v2 = 0;
		strcat(Filename, "\\Diablo.ini");
		GetPrivateProfileString("NetMsg", spszMsgKeyTbl[v1], spszMsgTbl[v1], ReturnedString, 0x50u, Filename);
		NetSendCmdString(-1, ReturnedString);
	}
}
// 48436C: using guessed type char *spszMsgTbl[4];
// 48437C: using guessed type char *spszMsgKeyTbl[4];
// 679660: using guessed type char gbMaxPlayers;

void __fastcall ReleaseKey(int vkey)
{
	if (vkey == VK_SNAPSHOT)
		CaptureScreen();
}

void __fastcall PressKey(int vkey)
{
	int v1;        // esi
	int v2;        // ecx
	int v3;        // ecx
	signed int v4; // eax

	v1 = vkey;
	if (!gmenu_presskeys(vkey) && !control_presskeys(v1)) {
		if (!deathflag)
			goto LABEL_113;
		if (sgnTimeoutCurs == CURSOR_NONE) {
			if (v1 == VK_F9)
				diablo_hotkey_msg(0);
			if (v1 == VK_F10)
				diablo_hotkey_msg(1);
			if (v1 == VK_F11)
				diablo_hotkey_msg(2);
			if (v1 == VK_F12)
				diablo_hotkey_msg(3);
			if (v1 == VK_RETURN)
				control_type_message();
			if (v1 == VK_ESCAPE) {
			LABEL_113:
				if (v1 == VK_ESCAPE) {
					if (!PressEscKey()) {
						track_repeat_walk(0);
						gamemenu_previous();
					}
					return;
				}
				if (sgnTimeoutCurs == CURSOR_NONE && !dropGoldFlag) {
					if (v1 == VK_PAUSE) {
						diablo_pause_game();
						return;
					}
					if (PauseMode != 2) {
						switch (v1) {
						case VK_RETURN:
							if (stextflag) {
								STextEnter();
							} else if (questlog) {
								QuestlogEnter();
							} else {
								control_type_message();
							}
							return;
						case VK_F1:
							if (helpflag) {
								helpflag = 0;
								return;
							}
							if (stextflag) {
								ClearPanel();
								AddPanelString("No help available", 1);
								AddPanelString("while in stores", 1);
								track_repeat_walk(0);
								return;
							}
							invflag = 0;
							chrflag = 0;
							sbookflag = 0;
							spselflag = 0;
							if (qtextflag && leveltype == DTYPE_TOWN) {
								qtextflag = FALSE;
								sfx_stop();
							}
							questlog = 0;
							automapflag = 0;
							msgdelay = 0;
							gamemenu_off();
							DisplayHelp();
						LABEL_110:
							doom_close();
							return;
#ifdef _DEBUG
						case VK_F3:
							if (pcursitem != -1) {
								sprintf(tempstr, "IDX = %i  :  Seed = %i  :  CF = %i", item[pcursitem].IDidx, item[pcursitem]._iSeed, item[pcursitem]._iCreateInfo);
								NetSendCmdString(1 << myplr, tempstr);
							}
							sprintf(tempstr, "Numitems : %i", numitems);
							NetSendCmdString(1 << myplr, tempstr);
							return;
						case VK_F4:
							PrintDebugQuest();
							return;
#endif
						case VK_F5:
							v2 = 0;
							goto LABEL_48;
						case VK_F6:
							v2 = 1;
							goto LABEL_48;
						case VK_F7:
							v2 = 2;
							goto LABEL_48;
						case VK_F8:
							v2 = 3;
						LABEL_48:
							if (spselflag)
								SetSpeedSpell(v2);
							else
								ToggleSpell(v2);
							return;
						case VK_F9:
							v3 = 0;
						LABEL_59:
							diablo_hotkey_msg(v3);
							return;
						case VK_F10:
							v3 = 1;
							goto LABEL_59;
						case VK_F11:
							v3 = 2;
							goto LABEL_59;
						case VK_F12:
							v3 = 3;
							goto LABEL_59;
						case VK_UP:
							if (stextflag) {
								STextUp();
							} else if (questlog) {
								QuestlogUp();
							} else if (helpflag) {
								HelpScrollUp();
							} else if (automapflag) {
								AutomapUp();
							}
							return;
						case VK_DOWN:
							if (stextflag) {
								STextDown();
							} else if (questlog) {
								QuestlogDown();
							} else if (helpflag) {
								HelpScrollDown();
							} else if (automapflag) {
								AutomapDown();
							}
							return;
						case VK_PRIOR:
							if (stextflag)
								STextPrior();
							return;
						case VK_NEXT:
							if (stextflag)
								STextNext();
							return;
						case VK_LEFT:
							if (automapflag && !talkflag)
								AutomapLeft();
							return;
						case VK_RIGHT:
							if (automapflag && !talkflag)
								AutomapRight();
							return;
						case VK_TAB:
							DoAutoMap();
							return;
						case VK_SPACE:
							if (!chrflag) {
								if (!invflag) {
								LABEL_106:
									helpflag = 0;
									invflag = 0;
									chrflag = 0;
									sbookflag = 0;
									spselflag = 0;
									if (qtextflag && leveltype == DTYPE_TOWN) {
										qtextflag = FALSE;
										sfx_stop();
									}
									questlog = 0;
									automapflag = 0;
									msgdelay = 0;
									gamemenu_off();
									goto LABEL_110;
								}
								v4 = MouseX;
								if (MouseX >= 480 || MouseY >= 352) {
								LABEL_101:
									if (!invflag && chrflag && v4 > 160 && MouseY < 352)
										SetCursorPos(v4 - 160, MouseY);
									goto LABEL_106;
								}
								SetCursorPos(MouseX + 160, MouseY);
							}
							v4 = MouseX;
							goto LABEL_101;
						}
					}
				}
			}
		}
	}
}
// 4B84DC: using guessed type int dropGoldFlag;
// 4B8960: using guessed type int talkflag;
// 4B8968: using guessed type int sbookflag;
// 4B8C98: using guessed type int spselflag;
// 525740: using guessed type int PauseMode;
// 52B9F0: using guessed type char msgdelay;
// 646D00: using guessed type char qtextflag;
// 69BD04: using guessed type int questlog;
// 6AA705: using guessed type char stextflag;

void __cdecl diablo_pause_game()
{
	if ((unsigned char)gbMaxPlayers <= 1u) {
		if (PauseMode) {
			PauseMode = 0;
		} else {
			PauseMode = 2;
			FreeMonsterSnd();
			track_repeat_walk(0);
		}
		drawpanflag = 255;
	}
}
// 52571C: using guessed type int drawpanflag;
// 525740: using guessed type int PauseMode;
// 679660: using guessed type char gbMaxPlayers;

void __fastcall PressChar(int vkey)
{
	int v1;  // ebx
	BOOL v4; // ecx
	int v5;  // ecx
	int v6;  // eax
	BOOL v7; // ecx
	int v9;  // ecx
	int v10; // eax
	int v11; // eax
	int v12; // eax
	int v13; // eax
	int v14; // eax
	int v15; // eax
	int v16; // eax
	int v18; // [esp-4h] [ebp-8h]

	v1 = vkey;
	if (!gmenu_exception() && !control_talk_last_key(v1) && sgnTimeoutCurs == CURSOR_NONE && !deathflag) {
		if ((_BYTE)v1 == 'p' || (_BYTE)v1 == 'P') {
			diablo_pause_game();
		} else if (PauseMode != 2) {
			if (doomflag) {
				doom_close();
				return;
			}
			if (dropGoldFlag) {
				control_drop_gold(v1);
				return;
			}
			switch (v1) {
			case '!':
			case '1':
				v9 = myplr;
				v10 = plr[myplr].SpdList[0]._itype;
				if (v10 != -1 && v10 != 11) {
					v18 = 47;
					goto LABEL_72;
				}
				return;
			case '#':
			case '3':
				v9 = myplr;
				v12 = plr[myplr].SpdList[2]._itype;
				if (v12 != -1 && v12 != 11) {
					v18 = 49;
					goto LABEL_72;
				}
				return;
			case '$':
			case '4':
				v9 = myplr;
				v13 = plr[myplr].SpdList[3]._itype;
				if (v13 != -1 && v13 != 11) {
					v18 = 50;
					goto LABEL_72;
				}
				return;
			case '%':
			case '5':
				v9 = myplr;
				v14 = plr[myplr].SpdList[4]._itype;
				if (v14 != -1 && v14 != 11) {
					v18 = 51;
					goto LABEL_72;
				}
				return;
			case '&':
			case '7':
				v9 = myplr;
				v16 = plr[myplr].SpdList[6]._itype;
				if (v16 != -1 && v16 != 11) {
					v18 = 53;
					goto LABEL_72;
				}
				return;
			case '*':
			case '8':
#ifdef _DEBUG
				if (debug_mode_key_inverted_v || debug_mode_key_w) {
					NetSendCmd(TRUE, CMD_CHEAT_EXPERIENCE);
					return;
				}
#endif
				v9 = myplr;
				if (plr[myplr].SpdList[7]._itype != -1
				    && plr[myplr].SpdList[7]._itype != 11) {
					v18 = 54;
					goto LABEL_72;
				}
				return;
			case '+':
			case '=':
				if (automapflag)
					AutomapZoomIn();
				return;
			case '-':
			case '_':
				if (automapflag)
					AutomapZoomOut();
				return;
			case '2':
			case '@':
				v9 = myplr;
				v11 = plr[myplr].SpdList[1]._itype;
				if (v11 != -1 && v11 != 11) {
					v18 = 48;
					goto LABEL_72;
				}
				return;
			case '6':
			case '^':
				v9 = myplr;
				v15 = plr[myplr].SpdList[5]._itype;
				if (v15 != -1 && v15 != 11) {
					v18 = 52;
				LABEL_72:
					UseInvItem(v9, v18);
				}
				return;
			case 'B':
			case 'b':
				if (!stextflag) {
					invflag = 0;
					sbookflag = sbookflag == 0;
				}
				return;
			case 'C':
			case 'c':
				if (!stextflag) {
					questlog = 0;
					v7 = chrflag == 0;
					chrflag = chrflag == 0;
					if (!v7 || invflag)
						goto LABEL_18;
					goto LABEL_24;
				}
				return;
			case 'F':
			case 'f':
				IncreaseGamma();
				return;
			case 'G':
			case 'g':
				DecreaseGamma();
				return;
			case 'I':
			case 'i':
				if (stextflag)
					return;
				sbookflag = 0;
				v4 = invflag == 0;
				invflag = invflag == 0;
				if (!v4 || chrflag) {
				LABEL_24:
					if (MouseX < 480) {
						v5 = MouseY;
						if (MouseY < 352) {
							v6 = MouseX + 160;
							goto LABEL_27;
						}
					}
				} else {
				LABEL_18:
					if (MouseX > 160) {
						v5 = MouseY;
						if (MouseY < 352) {
							v6 = MouseX - 160;
						LABEL_27:
							SetCursorPos(v6, v5);
							return;
						}
					}
				}
				break;
			case 'Q':
			case 'q':
				if (!stextflag) {
					chrflag = 0;
					if (questlog)
						questlog = 0;
					else
						StartQuestlog();
				}
				return;
			case 'S':
			case 's':
				if (!stextflag) {
					invflag = 0;
					if (spselflag)
						spselflag = 0;
					else
						DoSpeedBook();
					track_repeat_walk(0);
				}
				return;
			case 'V':
				NetSendCmdString(1 << myplr, gszVersionNumber);
				return;
			case 'v':
				NetSendCmdString(1 << myplr, gszProductName);
				return;
			case 'Z':
			case 'z':
				zoomflag = zoomflag == 0;
				return;
#ifdef _DEBUG
			case ')':
			case '0':
				if (debug_mode_key_inverted_v) {
					if (arrowdebug > 2)
						arrowdebug = 0;
					if (!arrowdebug) {
						plr[myplr]._pIFlags &= ~ISPL_FIRE_ARROWS;
						plr[myplr]._pIFlags &= ~ISPL_LIGHT_ARROWS;
					}
					if (arrowdebug == 1)
						plr[myplr]._pIFlags |= ISPL_FIRE_ARROWS;
					if (arrowdebug == 2)
						plr[myplr]._pIFlags |= ISPL_LIGHT_ARROWS;
					arrowdebug++;
				}
				return;
			case ':':
				if (!currlevel && debug_mode_key_w)
					SetAllSpellsCheat();
				return;
			case '[':
				if (!currlevel && debug_mode_key_w)
					TakeGoldCheat();
				return;
			case ']':
				if (!currlevel && debug_mode_key_w)
					MaxSpellsCheat();
				return;
			case 'a':
				if (debug_mode_key_inverted_v) {
					spelldata[SPL_TELEPORT].sTownSpell = TRUE;
					plr[myplr]._pSplLvl[plr[myplr]._pSpell]++;
				}
				return;
			case 'D':
				PrintDebugPlayer(1);
				return;
			case 'd':
				PrintDebugPlayer(0);
				return;
			case 'e':
				if (debug_mode_key_d) {
					sprintf(tempstr, "EFlag = %i", plr[myplr]._peflag);
					NetSendCmdString(1 << myplr, tempstr);
				}
				return;
			case 'L':
			case 'l':
				if (debug_mode_key_inverted_v)
					ToggleLighting();
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
				NetSendCmdString(1 << myplr, tempstr);
				sprintf(tempstr, "Mid1 = %i : Mid2 = %i : Mid3 = %i", glMid1Seed[currlevel], glMid2Seed[currlevel], glMid3Seed[currlevel]);
				NetSendCmdString(1 << myplr, tempstr);
				sprintf(tempstr, "End = %i", glEndSeed[currlevel]);
				NetSendCmdString(1 << myplr, tempstr);
				return;
			case 'T':
			case 't':
				if (debug_mode_key_inverted_v) {
					sprintf(tempstr, "PX = %i  PY = %i", plr[myplr].WorldX, plr[myplr].WorldY);
					NetSendCmdString(1 << myplr, tempstr);
					sprintf(tempstr, "CX = %i  CY = %i  DP = %i", cursmx, cursmy, dungeon[cursmx][cursmy]);
					NetSendCmdString(1 << myplr, tempstr);
				}
				return;
			case '|':
				if (!currlevel && debug_mode_key_w)
					GiveGoldCheat();
				return;
			case '~':
				if (!currlevel && debug_mode_key_w)
					StoresCheat();
				return;
#endif
			}
		}
	}
}
// 4B84DC: using guessed type int dropGoldFlag;
// 4B8968: using guessed type int sbookflag;
// 4B8C98: using guessed type int spselflag;
// 52569C: using guessed type int zoomflag;
// 525740: using guessed type int PauseMode;
// 52575C: using guessed type int doomflag;
// 69BD04: using guessed type int questlog;
// 6AA705: using guessed type char stextflag;

void __cdecl LoadLvlGFX()
{
	switch (leveltype) {
	case DTYPE_TOWN:
		pDungeonCels = LoadFileInMem("Levels\\TownData\\Town.CEL", 0);
		pMegaTiles = LoadFileInMem("Levels\\TownData\\Town.TIL", 0);
		pLevelPieces = LoadFileInMem("Levels\\TownData\\Town.MIN", 0);
		level_special_cel = LoadFileInMem("Levels\\TownData\\TownS.CEL", 0);
		break;
	case DTYPE_CATHEDRAL:
		pDungeonCels = LoadFileInMem("Levels\\L1Data\\L1.CEL", 0);
		pMegaTiles = LoadFileInMem("Levels\\L1Data\\L1.TIL", 0);
		pLevelPieces = LoadFileInMem("Levels\\L1Data\\L1.MIN", 0);
		level_special_cel = LoadFileInMem("Levels\\L1Data\\L1S.CEL", 0);
		break;
	case DTYPE_CATACOMBS:
		pDungeonCels = LoadFileInMem("Levels\\L2Data\\L2.CEL", 0);
		pMegaTiles = LoadFileInMem("Levels\\L2Data\\L2.TIL", 0);
		pLevelPieces = LoadFileInMem("Levels\\L2Data\\L2.MIN", 0);
		level_special_cel = LoadFileInMem("Levels\\L2Data\\L2S.CEL", 0);
		break;
	case DTYPE_CAVES:
		pDungeonCels = LoadFileInMem("Levels\\L3Data\\L3.CEL", 0);
		pMegaTiles = LoadFileInMem("Levels\\L3Data\\L3.TIL", 0);
		pLevelPieces = LoadFileInMem("Levels\\L3Data\\L3.MIN", 0);
		level_special_cel = LoadFileInMem("Levels\\L1Data\\L1S.CEL", 0);
		break;
	case DTYPE_HELL:
		pDungeonCels = LoadFileInMem("Levels\\L4Data\\L4.CEL", 0);
		pMegaTiles = LoadFileInMem("Levels\\L4Data\\L4.TIL", 0);
		pLevelPieces = LoadFileInMem("Levels\\L4Data\\L4.MIN", 0);
		level_special_cel = LoadFileInMem("Levels\\L2Data\\L2S.CEL", 0);
		break;
	default:
		app_fatal("LoadLvlGFX");
		return;
	}
}

void __cdecl LoadAllGFX()
{
	pSpeedCels = DiabloAllocPtr(0x100000);
	IncProgress();
	IncProgress();
	InitObjectGFX();
	IncProgress();
	InitMissileGFX();
	IncProgress();
}

void __fastcall CreateLevel(int lvldir)
{
	int hnd; // cl

	switch (leveltype) {
	case DTYPE_TOWN:
		CreateTown(lvldir);
		InitTownTriggers();
		hnd = 0;
		break;
	case DTYPE_CATHEDRAL:
		CreateL5Dungeon(glSeedTbl[currlevel], lvldir);
		InitL1Triggers();
		Freeupstairs();
		hnd = 1;
		break;
	case DTYPE_CATACOMBS:
		CreateL2Dungeon(glSeedTbl[currlevel], lvldir);
		InitL2Triggers();
		Freeupstairs();
		hnd = 2;
		break;
	case DTYPE_CAVES:
		CreateL3Dungeon(glSeedTbl[currlevel], lvldir);
		InitL3Triggers();
		Freeupstairs();
		hnd = 3;
		break;
	case DTYPE_HELL:
		CreateL4Dungeon(glSeedTbl[currlevel], lvldir);
		InitL4Triggers();
		Freeupstairs();
		hnd = 4;
		break;
	default:
		app_fatal("CreateLevel");
		return;
	}

	LoadRndLvlPal(hnd);
}

void __fastcall LoadGameLevel(BOOL firstflag, int lvldir)
{
	int i, j;
	BOOL visited;

	if (setseed)
		glSeedTbl[currlevel] = setseed;

	music_stop();
	SetCursor_(CURSOR_HAND);
	SetRndSeed(glSeedTbl[currlevel]);
	IncProgress();
	MakeLightTable();
	LoadLvlGFX();
	IncProgress();

	if (firstflag) {
		InitInv();
		InitItemGFX();
		InitQuestText();

		for (i = 0; i < gbMaxPlayers; i++)
			InitPlrGFXMem(i);

		InitStores();
		InitAutomapOnce();
		InitHelp();
	}

	SetRndSeed(glSeedTbl[currlevel]);

	if (leveltype == DTYPE_TOWN)
		SetupTownStores();

	IncProgress();
	InitAutomap();

	if (leveltype != DTYPE_TOWN && lvldir != 4) {
		InitLighting();
		InitVision();
	}

	InitLevelMonsters();
	IncProgress();

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
			InitMissileGFX();
		}

		IncProgress();

		if (lvldir == 3)
			GetReturnLvlPos();
		if (lvldir == 5)
			GetPortalLvlPos();

		IncProgress();

		for (i = 0; i < MAX_PLRS; i++) {
			if (plr[i].plractive && currlevel == plr[i].plrlevel) {
				InitPlayerGFX(i);
				if (lvldir != 4)
					InitPlayer(i, firstflag);
			}
		}

		PlayDungMsgs();
		InitMultiView();
		IncProgress();

		visited = FALSE;
		for (i = 0; i < gbMaxPlayers; i++) {
			if (plr[i].plractive)
				visited = visited || plr[i]._pLvlVisited[currlevel];
		}

		SetRndSeed(glSeedTbl[currlevel]);

		if (leveltype != DTYPE_TOWN) {
			if (firstflag || lvldir == 4 || !plr[myplr]._pLvlVisited[currlevel] || gbMaxPlayers != 1) {
				HoldThemeRooms();
				glMid1Seed[currlevel] = GetRndSeed();
				InitMonsters();
				glMid2Seed[currlevel] = GetRndSeed();
				InitObjects();
				InitItems();
				CreateThemeRooms();
				glMid3Seed[currlevel] = GetRndSeed();
				InitMissiles();
				InitDead();
				glEndSeed[currlevel] = GetRndSeed();

				if (gbMaxPlayers != 1)
					DeltaLoadLevel();

				IncProgress();
				SavePreLighting();
			} else {
				InitMonsters();
				InitMissiles();
				InitDead();
				IncProgress();
				LoadLevel();
				IncProgress();
			}
		} else {
			for (i = 0; i < MAXDUNX; i++) {
				for (j = 0; j < MAXDUNY; j++)
					dFlags[i][j] |= DFLAG_LIT;
			}

			InitTowners();
			InitItems();
			InitMissiles();
			IncProgress();

			if (!firstflag && lvldir != 4 && plr[myplr]._pLvlVisited[currlevel] && gbMaxPlayers == 1)
				LoadLevel();
			if (gbMaxPlayers != 1)
				DeltaLoadLevel();

			IncProgress();
		}
		if (gbMaxPlayers == 1)
			ResyncQuests();
		else
			ResyncMPQuests();
	} else {
		/// ASSERT: assert(! pSpeedCels);
		pSpeedCels = DiabloAllocPtr(0x100000);
		LoadSetMap();
		IncProgress();
		GetLevelMTypes();
		InitMonsters();
		InitMissileGFX();
		InitDead();
		FillSolidBlockTbls();
		IncProgress();

		if (lvldir == 5)
			GetPortalLvlPos();

		for (i = 0; i < MAX_PLRS; i++) {
			if (plr[i].plractive && currlevel == plr[i].plrlevel) {
				InitPlayerGFX(i);
				if (lvldir != 4)
					InitPlayer(i, firstflag);
			}
		}

		InitMultiView();
		IncProgress();

		if (firstflag || lvldir == 4 || !plr[myplr]._pSLvlVisited[setlvlnum]) {
			InitItems();
			SavePreLighting();
		} else {
			LoadLevel();
		}

		InitMissiles();
		IncProgress();
	}

	SyncPortals();

	for (i = 0; i < MAX_PLRS; i++) {
		if (plr[i].plractive && plr[i].plrlevel == currlevel && (!plr[i]._pLvlChanging || i == myplr)) {
			if (plr[i]._pHitPoints > 0) {
				if (gbMaxPlayers == 1)
					dPlayer[plr[i].WorldX][plr[i].WorldY] = i + 1;
				else
					SyncInitPlrPos(i);
			} else {
				dFlags[plr[i].WorldX][plr[i].WorldY] |= DFLAG_DEAD_PLAYER;
			}
		}
	}

	if (leveltype != DTYPE_TOWN)
		SetDungeonMicros();

	InitLightMax();
	IncProgress();
	IncProgress();

	if (firstflag) {
		InitControlPan();
		IncProgress();
	}
	if (leveltype != DTYPE_TOWN) {
		ProcessLightList();
		ProcessVisionList();
	}

	music_start(leveltype);

	while (!IncProgress())
		;

	if (setlevel && setlvlnum == SL_SKELKING && quests[QTYPE_KING]._qactive == 2)
		PlaySFX(USFX_SKING1);
}
// 525738: using guessed type int setseed;
// 5CF31D: using guessed type char setlevel;
// 679660: using guessed type char gbMaxPlayers;

void __fastcall game_loop(BOOL bStartup)
{
	int i;

	i = bStartup ? 60 : 3;

	while (i--) {
		if (!multi_handle_delta()) {
			timeout_cursor(TRUE);
			break;
		} else {
			timeout_cursor(FALSE);
			game_logic();
		}
		if (!gbRunGame || gbMaxPlayers == 1 || !nthread_has_500ms_passed(TRUE))
			break;
	}
}
// 679660: using guessed type char gbMaxPlayers;

void __cdecl game_logic()
{
	if (PauseMode != 2) {
		if (PauseMode == 1)
			PauseMode = 2;
		if (gbMaxPlayers == 1 && gmenu_exception()) {
			drawpanflag |= 1u;
		} else {
			if (!gmenu_exception() && sgnTimeoutCurs == CURSOR_NONE) {
				CheckCursMove();
				track_process();
			}
			if (gbProcessPlayers)
				ProcessPlayers();
			if (leveltype != DTYPE_TOWN) {
				ProcessMonsters();
				ProcessObjects();
				ProcessMissiles();
				ProcessItems();
				ProcessLightList();
				ProcessVisionList();
			} else {
				ProcessTowners();
				ProcessItems();
				ProcessMissiles();
			}
#ifdef _DEBUG
			if (debug_mode_key_inverted_v) {
				if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
					ScrollView();
			}
#endif
			sound_update();
			ClearPlrMsg();
			CheckTriggers();
			CheckQuests();
			drawpanflag |= 1u;
			pfile_update(0);
		}
	}
}
// 525718: using guessed type char cineflag;
// 52571C: using guessed type int drawpanflag;
// 525740: using guessed type int PauseMode;
// 679660: using guessed type char gbMaxPlayers;

void __fastcall timeout_cursor(BOOL bTimeout)
{
	if (bTimeout) {
		if (sgnTimeoutCurs == CURSOR_NONE && !sgbMouseDown) {
			sgnTimeoutCurs = pcurs;
			multi_net_ping();
			ClearPanel();
			AddPanelString("-- Network timeout --", 1);
			AddPanelString("-- Waiting for players --", 1);
			SetCursor_(CURSOR_HOURGLASS);
			drawpanflag = 255;
		}
		scrollrt_draw_game_screen(1);
	} else if (sgnTimeoutCurs) {
		SetCursor_(sgnTimeoutCurs);
		sgnTimeoutCurs = 0;
		ClearPanel();
		drawpanflag = 255;
	}
}
// 52571C: using guessed type int drawpanflag;
// 525748: using guessed type char sgbMouseDown;

void __cdecl diablo_color_cyc_logic()
{
	DWORD v0; // eax

	v0 = GetTickCount();
	if (v0 - color_cycle_timer >= 0x32) {
		color_cycle_timer = v0;
		if (palette_get_colour_cycling()) {
			if (leveltype == DTYPE_HELL) {
				lighting_color_cycling();
			} else if (leveltype == DTYPE_CAVES) {
				if (fullscreen)
					palette_update_caves();
			}
		}
	}
}
// 52574C: using guessed type int color_cycle_timer;

DEVILUTION_END_NAMESPACE
