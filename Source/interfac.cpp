#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

void *sgpBackCel;
int sgdwProgress;
int progress_id;

const BYTE progress_bar_colours[3] = { 138, 43, 254 };
const int progress_bar_screen_pos[3][2] = { { 53, 37 }, { 53, 421 }, { 53, 37 } };

void interface_msg_pump()
{
	MSG Msg;

	while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
		if (Msg.message != WM_QUIT) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
}

BOOL IncProgress()
{
	interface_msg_pump();
	sgdwProgress += 15;
	if ((DWORD)sgdwProgress > 534)
		sgdwProgress = 534;
	if (sgpBackCel)
		DrawCutscene();
	return (DWORD)sgdwProgress >= 534;
}

void DrawCutscene()
{
	DWORD i;

	lock_buf(1);
	CelDecodeOnly(SCREEN_X, 480 + SCREEN_Y - 1, (BYTE *)sgpBackCel, 1, 640);

	for (i = 0; i < sgdwProgress; i++) {
		DrawProgress(
		    progress_bar_screen_pos[progress_id][0] + i + SCREEN_X,
		    progress_bar_screen_pos[progress_id][1] + SCREEN_Y,
		    progress_id);
	}

	unlock_buf(1);
	drawpanflag = 255;
	scrollrt_draw_game_screen(0);
}

void DrawProgress(int screen_x, int screen_y, int progress_id)
{
	BYTE *dst;
	int i;

	dst = &gpBuffer[screen_x + PitchTbl[screen_y]];
	for (i = 0; i < 22; i++) {
		*dst = progress_bar_colours[progress_id];
		dst += BUFFER_WIDTH;
	}
}

void ShowProgress(unsigned int uMsg)
{
	WNDPROC saveProc;

	gbSomebodyWonGameKludge = FALSE;
	plrmsg_delay(TRUE);

	/// ASSERT: assert(ghMainWnd);
	saveProc = SetWindowProc(DisableInputWndProc);

	interface_msg_pump();
	ClearScreenBuffer();
	scrollrt_draw_game_screen(TRUE);
	InitCutscene(uMsg);
	BlackPalette();
	DrawCutscene();
	PaletteFadeIn(8);
	IncProgress();
	stream_update();
	IncProgress();

	switch (uMsg) {
	case WM_DIABLOADGAME:
		IncProgress();
		LoadGame(TRUE);
		IncProgress();
		break;
	case WM_DIABNEWGAME:
		IncProgress();
		FreeGameMem();
		IncProgress();
		pfile_remove_temp_files();
		LoadGameLevel(TRUE, 0);
		IncProgress();
		break;
	case WM_DIABNEXTLVL:
		IncProgress();
		if (gbMaxPlayers == 1) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		FreeGameMem();
		currlevel++;
		leveltype = gnLevelTypeTbl[currlevel];
		/// ASSERT: assert(plr[myplr].plrlevel == currlevel);
		IncProgress();
		LoadGameLevel(FALSE, 0);
		IncProgress();
		break;
	case WM_DIABPREVLVL:
		IncProgress();
		if (gbMaxPlayers == 1) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		FreeGameMem();
		currlevel--;
		leveltype = gnLevelTypeTbl[currlevel];
		/// ASSERT: assert(plr[myplr].plrlevel == currlevel);
		IncProgress();
		LoadGameLevel(FALSE, 1);
		IncProgress();
		break;
	case WM_DIABSETLVL:
		SetReturnLvlPos();
		if (gbMaxPlayers == 1) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		setlevel = TRUE;
		leveltype = setlvltype;
		FreeGameMem();
		IncProgress();
		LoadGameLevel(FALSE, 2);
		IncProgress();
		break;
	case WM_DIABRTNLVL:
		if (gbMaxPlayers == 1) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		setlevel = FALSE;
		FreeGameMem();
		IncProgress();
		GetReturnLvlPos();
		LoadGameLevel(FALSE, 3);
		IncProgress();
		break;
	case WM_DIABWARPLVL:
		IncProgress();
		if (gbMaxPlayers == 1) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		FreeGameMem();
		GetPortalLevel();
		IncProgress();
		LoadGameLevel(FALSE, 5);
		IncProgress();
		break;
	case WM_DIABTOWNWARP:
		IncProgress();
		if (gbMaxPlayers == 1) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		FreeGameMem();
		currlevel = plr[myplr].plrlevel;
		leveltype = gnLevelTypeTbl[currlevel];
		/// ASSERT: assert(plr[myplr].plrlevel == currlevel);
		IncProgress();
		LoadGameLevel(FALSE, 6);
		IncProgress();
		break;
	case WM_DIABTWARPUP:
		IncProgress();
		if (gbMaxPlayers == 1) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		FreeGameMem();
		currlevel = plr[myplr].plrlevel;
		leveltype = gnLevelTypeTbl[currlevel];
		/// ASSERT: assert(plr[myplr].plrlevel == currlevel);
		IncProgress();
		LoadGameLevel(FALSE, 7);
		IncProgress();
		break;
	case WM_DIABRETOWN:
		IncProgress();
		if (gbMaxPlayers == 1) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		FreeGameMem();
		currlevel = plr[myplr].plrlevel;
		leveltype = gnLevelTypeTbl[currlevel];
		/// ASSERT: assert(plr[myplr].plrlevel == currlevel);
		IncProgress();
		LoadGameLevel(FALSE, 0);
		IncProgress();
		break;
	}

	/// ASSERT: assert(ghMainWnd);

	PaletteFadeOut(8);
	FreeInterface();

	saveProc = SetWindowProc(saveProc);
	/// ASSERT: assert(saveProc == DisableInputWndProc);

	NetSendCmdLocParam1(TRUE, CMD_PLAYER_JOINLEVEL, plr[myplr].WorldX, plr[myplr].WorldY, plr[myplr].plrlevel);
	plrmsg_delay(FALSE);
	ResetPal();

	if (gbSomebodyWonGameKludge && plr[myplr].plrlevel == 16) {
		PrepDoEnding();
	}

	gbSomebodyWonGameKludge = FALSE;
}

void FreeInterface()
{
	MemFreeDbg(sgpBackCel);
}

void InitCutscene(unsigned int uMsg)
{
	/// ASSERT: assert(! sgpBackCel);

	switch (uMsg) {
	case WM_DIABNEXTLVL:
		switch (gnLevelTypeTbl[currlevel]) {
		case 0:
			sgpBackCel = LoadFileInMem("Gendata\\Cuttt.CEL", NULL);
			LoadPalette("Gendata\\Cuttt.pal");
			progress_id = 1;
			break;
		case 1:
			sgpBackCel = LoadFileInMem("Gendata\\Cutl1d.CEL", NULL);
			LoadPalette("Gendata\\Cutl1d.pal");
			progress_id = 0;
			break;
		case 2:
			sgpBackCel = LoadFileInMem("Gendata\\Cut2.CEL", NULL);
			LoadPalette("Gendata\\Cut2.pal");
			progress_id = 2;
			break;
		case 3:
			sgpBackCel = LoadFileInMem("Gendata\\Cut3.CEL", NULL);
			LoadPalette("Gendata\\Cut3.pal");
			progress_id = 1;
			break;
		case 4:
			if (currlevel < 15) {
				sgpBackCel = LoadFileInMem("Gendata\\Cut4.CEL", NULL);
				LoadPalette("Gendata\\Cut4.pal");
				progress_id = 1;
			} else {
				sgpBackCel = LoadFileInMem("Gendata\\Cutgate.CEL", NULL);
				LoadPalette("Gendata\\Cutgate.pal");
				progress_id = 1;
			}
			break;
		default:
			sgpBackCel = LoadFileInMem("Gendata\\Cutl1d.CEL", NULL);
			LoadPalette("Gendata\\Cutl1d.pal");
			progress_id = 0;
			break;
		}
		break;
	case WM_DIABPREVLVL:
		if (gnLevelTypeTbl[currlevel - 1] == 0) {
			sgpBackCel = LoadFileInMem("Gendata\\Cuttt.CEL", NULL);
			LoadPalette("Gendata\\Cuttt.pal");
			progress_id = 1;
		} else {
			switch (gnLevelTypeTbl[currlevel]) {
			case 0:
				sgpBackCel = LoadFileInMem("Gendata\\Cuttt.CEL", NULL);
				LoadPalette("Gendata\\Cuttt.pal");
				progress_id = 1;
				break;
			case 1:
				sgpBackCel = LoadFileInMem("Gendata\\Cutl1d.CEL", NULL);
				LoadPalette("Gendata\\Cutl1d.pal");
				progress_id = 0;
				break;
			case 2:
				sgpBackCel = LoadFileInMem("Gendata\\Cut2.CEL", NULL);
				LoadPalette("Gendata\\Cut2.pal");
				progress_id = 2;
				break;
			case 3:
				sgpBackCel = LoadFileInMem("Gendata\\Cut3.CEL", NULL);
				LoadPalette("Gendata\\Cut3.pal");
				progress_id = 1;
				break;
			case 4:
				sgpBackCel = LoadFileInMem("Gendata\\Cut4.CEL", NULL);
				LoadPalette("Gendata\\Cut4.pal");
				progress_id = 1;
				break;
			default:
				sgpBackCel = LoadFileInMem("Gendata\\Cutl1d.CEL", NULL);
				LoadPalette("Gendata\\Cutl1d.pal");
				progress_id = 0;
				break;
			}
		}
		break;
	case WM_DIABSETLVL:
		if (setlvlnum == SL_BONECHAMB) {
			sgpBackCel = LoadFileInMem("Gendata\\Cut2.CEL", NULL);
			LoadPalette("Gendata\\Cut2.pal");
			progress_id = 2;
		} else if (setlvlnum == SL_VILEBETRAYER) {
			sgpBackCel = LoadFileInMem("Gendata\\Cutportr.CEL", NULL);
			LoadPalette("Gendata\\Cutportr.pal");
			progress_id = 1;
		} else {
			sgpBackCel = LoadFileInMem("Gendata\\Cutl1d.CEL", NULL);
			LoadPalette("Gendata\\Cutl1d.pal");
			progress_id = 0;
		}
		break;
	case WM_DIABRTNLVL:
		if (setlvlnum == SL_BONECHAMB) {
			sgpBackCel = LoadFileInMem("Gendata\\Cut2.CEL", NULL);
			LoadPalette("Gendata\\Cut2.pal");
			progress_id = 2;
		} else if (setlvlnum == SL_VILEBETRAYER) {
			sgpBackCel = LoadFileInMem("Gendata\\Cutportr.CEL", NULL);
			LoadPalette("Gendata\\Cutportr.pal");
			progress_id = 1;
		} else {
			sgpBackCel = LoadFileInMem("Gendata\\Cutl1d.CEL", NULL);
			LoadPalette("Gendata\\Cutl1d.pal");
			progress_id = 0;
		}
		break;
	case WM_DIABWARPLVL:
		sgpBackCel = LoadFileInMem("Gendata\\Cutportl.CEL", NULL);
		LoadPalette("Gendata\\Cutportl.pal");
		progress_id = 1;
		break;
	case WM_DIABLOADGAME:
		sgpBackCel = LoadFileInMem("Gendata\\Cutstart.CEL", NULL);
		LoadPalette("Gendata\\Cutstart.pal");
		progress_id = 1;
		break;
	case WM_DIABNEWGAME:
		sgpBackCel = LoadFileInMem("Gendata\\Cutstart.CEL", NULL);
		LoadPalette("Gendata\\Cutstart.pal");
		progress_id = 1;
		break;
	case WM_DIABTOWNWARP:
	case WM_DIABTWARPUP:
		switch (gnLevelTypeTbl[plr[myplr].plrlevel]) {
		case 0:
			sgpBackCel = LoadFileInMem("Gendata\\Cuttt.CEL", NULL);
			LoadPalette("Gendata\\Cuttt.pal");
			progress_id = 1;
			break;
		case 2:
			sgpBackCel = LoadFileInMem("Gendata\\Cut2.CEL", NULL);
			LoadPalette("Gendata\\Cut2.pal");
			progress_id = 2;
			break;
		case 3:
			sgpBackCel = LoadFileInMem("Gendata\\Cut3.CEL", NULL);
			LoadPalette("Gendata\\Cut3.pal");
			progress_id = 1;
			break;
		case 4:
			sgpBackCel = LoadFileInMem("Gendata\\Cut4.CEL", NULL);
			LoadPalette("Gendata\\Cut4.pal");
			progress_id = 1;
			break;
		}
		break;
	case WM_DIABRETOWN:
		sgpBackCel = LoadFileInMem("Gendata\\Cuttt.CEL", NULL);
		LoadPalette("Gendata\\Cuttt.pal");
		progress_id = 1;
		break;
	default:
		app_fatal("Unknown progress mode");
		break;
	}

	sgdwProgress = 0;
}

DEVILUTION_END_NAMESPACE
