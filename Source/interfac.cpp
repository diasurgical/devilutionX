//HEADER_GOES_HERE

#include "../types.h"

DEVILUTION_BEGIN_NAMESPACE

void *sgpBackCel;
int sgdwProgress;
int progress_id; // idb

const unsigned char progress_bar_colours[3] = { 138u, 43u, 254u };
const int progress_bar_screen_pos[3][2] = { { 53, 37 }, { 53, 421 }, { 53, 37 } };

void __cdecl interface_msg_pump()
{
	MSG Msg; // [esp+8h] [ebp-1Ch]

	while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
		if (Msg.message != WM_QUIT) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
}

BOOL __cdecl IncProgress()
{
	interface_msg_pump();
	sgdwProgress += 15;
	if ((unsigned int)sgdwProgress > 0x216)
		sgdwProgress = 534;
	if (sgpBackCel)
		DrawCutscene();
	return (unsigned int)sgdwProgress >= 0x216;
}

void __cdecl DrawCutscene()
{
	unsigned int v0; // esi

	j_lock_buf_priv(1);
	CelDecodeOnly(64, 639, (BYTE *)sgpBackCel, 1, 640);
	v0 = 0;
	if (sgdwProgress) {
		do
			DrawProgress(
			    progress_bar_screen_pos[progress_id][0] + v0++ + 64,
			    progress_bar_screen_pos[progress_id][1] + 160,
			    progress_id);
		while (v0 < sgdwProgress);
	}
	j_unlock_buf_priv(1);
	drawpanflag = 255;
	scrollrt_draw_game_screen(0);
}
// 52571C: using guessed type int drawpanflag;

void __fastcall DrawProgress(int screen_x, int screen_y, int progress_id)
{
	_BYTE *v3;     // eax
	signed int v4; // ecx

	v3 = (unsigned char *)gpBuffer + screen_y_times_768[screen_y] + screen_x;
	v4 = 22;
	do {
		*v3 = progress_bar_colours[progress_id];
		v3 += 768;
		--v4;
	} while (v4);
}

void __fastcall ShowProgress(unsigned int uMsg)
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
// 5CF31C: using guessed type char setlvltype;
// 5CF31D: using guessed type char setlevel;
// 6761B8: using guessed type char gbSomebodyWonGameKludge;
// 679660: using guessed type char gbMaxPlayers;

void __cdecl FreeInterface()
{
	void *ptr;

	ptr = sgpBackCel;
	sgpBackCel = NULL;
	mem_free_dbg(ptr);
}

void __fastcall InitCutscene(unsigned int uMsg)
{
	int v1;            // eax
	int v2;            // eax
	int v3;            // eax
	int v4;            // eax
	unsigned char *v5; // eax
	char *v6;          // ecx
	int *v7;           // eax
	int v8;            // eax
	int v9;            // eax
	int v10;           // eax
	int v11;           // eax
	int v12;           // eax
	int v13;           // eax
	int v14;           // eax

	switch (uMsg) {
	case WM_DIABNEXTLVL:
		v1 = gnLevelTypeTbl[currlevel];
		if (!v1)
			goto LABEL_31;
		v2 = v1 - 1;
		if (!v2)
			goto LABEL_10;
		v3 = v2 - 1;
		if (!v3)
			goto LABEL_9;
		v4 = v3 - 1;
		if (!v4)
			goto LABEL_29;
		if (v4 != 1)
			goto LABEL_10;
		if (currlevel < 0xFu)
			goto LABEL_28;
		v5 = LoadFileInMem("Gendata\\Cutgate.CEL", 0);
		v6 = "Gendata\\Cutgate.pal";
		goto LABEL_30;
	case WM_DIABPREVLVL:
		v7 = &gnLevelTypeTbl[currlevel];
		if (!*(v7 - 1))
			goto LABEL_31;
		v8 = *v7;
		if (!v8)
			goto LABEL_31;
		v9 = v8 - 1;
		if (!v9)
			goto LABEL_10;
		v10 = v9 - 1;
		if (!v10) {
		LABEL_9:
			sgpBackCel = LoadFileInMem("Gendata\\Cut2.CEL", 0);
			LoadPalette("Gendata\\Cut2.pal");
			progress_id = 2;
			goto LABEL_33;
		}
		v11 = v10 - 1;
		if (!v11)
			goto LABEL_29;
		if (v11 == 1)
			goto LABEL_28;
	LABEL_10:
		sgpBackCel = LoadFileInMem("Gendata\\Cutl1d.CEL", 0);
		LoadPalette("Gendata\\Cutl1d.pal");
		progress_id = 0;
		goto LABEL_33;
	case WM_DIABRTNLVL:
	case WM_DIABSETLVL:
		if (setlvlnum == SL_BONECHAMB)
			goto LABEL_21;
		if (setlvlnum != SL_VILEBETRAYER)
			goto LABEL_10;
		v5 = LoadFileInMem("Gendata\\Cutportr.CEL", 0);
		v6 = "Gendata\\Cutportr.pal";
		goto LABEL_30;
	case WM_DIABWARPLVL:
		v5 = LoadFileInMem("Gendata\\Cutportl.CEL", 0);
		v6 = "Gendata\\Cutportl.pal";
		goto LABEL_30;
	case WM_DIABTOWNWARP:
	case WM_DIABTWARPUP:
		v12 = gnLevelTypeTbl[plr[myplr].plrlevel];
		if (!v12)
			goto LABEL_31;
		v13 = v12 - 2;
		if (!v13) {
		LABEL_21:
			sgpBackCel = LoadFileInMem("Gendata\\Cut2.CEL", 0);
			LoadPalette("Gendata\\Cut2.pal");
			progress_id = SL_BONECHAMB;
			goto LABEL_33;
		}
		v14 = v13 - 1;
		if (v14) {
			if (v14 != 1)
				goto LABEL_33;
		LABEL_28:
			v5 = LoadFileInMem("Gendata\\Cut4.CEL", 0);
			v6 = "Gendata\\Cut4.pal";
		} else {
		LABEL_29:
			v5 = LoadFileInMem("Gendata\\Cut3.CEL", 0);
			v6 = "Gendata\\Cut3.pal";
		}
	LABEL_30:
		sgpBackCel = v5;
		LoadPalette(v6);
		progress_id = 1;
	LABEL_33:
		sgdwProgress = 0;
		return;
	case WM_DIABRETOWN:
	LABEL_31:
		v5 = LoadFileInMem("Gendata\\Cuttt.CEL", 0);
		v6 = "Gendata\\Cuttt.pal";
		goto LABEL_30;
	case WM_DIABNEWGAME:
	case WM_DIABLOADGAME:
		v5 = LoadFileInMem("Gendata\\Cutstart.CEL", 0);
		v6 = "Gendata\\Cutstart.pal";
		goto LABEL_30;
	default:
		app_fatal("Unknown progress mode");
		goto LABEL_33;
	}
}

DEVILUTION_END_NAMESPACE
