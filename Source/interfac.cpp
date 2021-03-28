/**
 * @file interfac.cpp
 *
 * Implementation of load screens.
 */
#include "all.h"
#include "../SourceX/DiabloUI/art_draw.h"

DEVILUTION_BEGIN_NAMESPACE

BYTE *sgpBackCel;
Uint32 sgdwProgress;
int progress_id;

/** The color used for the progress bar as an index into the palette. */
const BYTE BarColor[3] = { 138, 43, 254 };
/** The screen position of the top left corner of the progress bar. */
const int BarPos[3][2] = { { 53, 37 }, { 53, 421 }, { 53, 37 } };

Art ArtCutsceneWidescreen;

static void FreeInterface()
{
	MemFreeDbg(sgpBackCel);
	ArtCutsceneWidescreen.Unload();
}

static Cutscenes PickCutscene(interface_mode uMsg)
{
	switch (uMsg) {
	case WM_DIABLOADGAME:
	case WM_DIABNEWGAME:
		return CutStart;
	case WM_DIABRETOWN:
		return CutTown;
	case WM_DIABNEXTLVL:
	case WM_DIABPREVLVL:
	case WM_DIABTOWNWARP:
	case WM_DIABTWARPUP: {
		int lvl = currlevel;
		if (uMsg == WM_DIABTWARPUP)
			lvl = plr[myplr].plrlevel;

		if (lvl == 1 && uMsg == WM_DIABPREVLVL)
			return CutTown;
		if (lvl == 16 && uMsg == WM_DIABNEXTLVL)
			return CutGate;

		switch (gnLevelTypeTbl[lvl]) {
		case DTYPE_TOWN:
			return CutTown;
		case DTYPE_CATHEDRAL:
			if (lvl > 16)
				return CutLevel5;
			return CutLevel1;
		case DTYPE_CATACOMBS:
			return CutLevel2;
		case DTYPE_CAVES:
			if (lvl > 16)
				return CutLevel6;
			return CutLevel3;
		case DTYPE_HELL:
			return CutLevel4;
		default:
			return CutLevel1;
		}
	};
	case WM_DIABWARPLVL:
		return CutPortal;
	case WM_DIABSETLVL:
	case WM_DIABRTNLVL:
		if (setlvlnum == SL_BONECHAMB)
			return CutLevel2;
		if (setlvlnum == SL_VILEBETRAYER)
			return CutPortalRed;
		return CutLevel1;
	default:
		app_fatal("Unknown progress mode");
	}
}

static void InitCutscene(interface_mode uMsg)
{
	const char *celPath;
	const char *palPath;

	switch (PickCutscene(uMsg)) {
	case CutStart:
		celPath = "Gendata\\Cutstart.cel";
		palPath = "Gendata\\Cutstart.pal";
		progress_id = 1;
		break;
	case CutTown:
		celPath = "Gendata\\Cuttt.cel";
		palPath = "Gendata\\Cuttt.pal";
		progress_id = 1;
		break;
	case CutLevel1:
		celPath = "Gendata\\Cutl1d.cel";
		palPath = "Gendata\\Cutl1d.pal";
		progress_id = 0;
		break;
	case CutLevel2:
		celPath = "Gendata\\Cut2.cel";
		palPath = "Gendata\\Cut2.pal";
		progress_id = 2;
		break;
	case CutLevel3:
		celPath = "Gendata\\Cut3.cel";
		palPath = "Gendata\\Cut3.pal";
		progress_id = 1;
		break;
	case CutLevel4:
		celPath = "Gendata\\Cut4.cel";
		palPath = "Gendata\\Cut4.pal";
		progress_id = 1;
		break;
	case CutLevel5:
		celPath = "Nlevels\\Cutl5.cel";
		palPath = "Nlevels\\Cutl5.pal";
		progress_id = 1;
		break;
	case CutLevel6:
		celPath = "Nlevels\\Cutl6.cel";
		palPath = "Nlevels\\Cutl6.pal";
		progress_id = 1;
		break;
	case CutPortal:
		LoadArt("Gendata\\Cutportlw.pcx", &ArtCutsceneWidescreen);
		celPath = "Gendata\\Cutportl.cel";
		palPath = "Gendata\\Cutportl.pal";
		progress_id = 1;
		break;
	case CutPortalRed:
		LoadArt("Gendata\\Cutportrw.pcx", &ArtCutsceneWidescreen);
		celPath = "Gendata\\Cutportr.cel";
		palPath = "Gendata\\Cutportr.pal";
		progress_id = 1;
		break;
	case CutGate:
		celPath = "Gendata\\Cutgate.cel";
		palPath = "Gendata\\Cutgate.pal";
		progress_id = 1;
		break;
	}

	assert(!sgpBackCel);
	sgpBackCel = LoadFileInMem(celPath, NULL);
	LoadPalette(palPath);

	sgdwProgress = 0;
}

static void DrawProgress(CelOutputBuffer out, int x, int y, int progress_id)
{
	BYTE *dst = out.at(x, y);
	for (int i = 0; i < 22; ++i, dst += out.pitch()) {
		*dst = BarColor[progress_id];
	}
}

static void DrawCutscene()
{
	lock_buf(1);
	CelOutputBuffer out = GlobalBackBuffer();
	DrawArt(out, PANEL_X - (ArtCutsceneWidescreen.w() - PANEL_WIDTH) / 2, UI_OFFSET_Y, &ArtCutsceneWidescreen);
	CelDrawTo(out, PANEL_X, 480 - 1 + UI_OFFSET_Y, sgpBackCel, 1, 640);

	for (Uint32 i = 0; i < sgdwProgress; i++) {
		DrawProgress(
		    out,
		    BarPos[progress_id][0] + i + PANEL_X,
		    BarPos[progress_id][1] + UI_OFFSET_Y,
		    progress_id);
	}

	unlock_buf(1);
	force_redraw = 255;
	scrollrt_draw_game_screen(FALSE);
}

void interface_msg_pump()
{
	MSG Msg;

	while (FetchMessage(&Msg)) {
		if (Msg.message != DVL_WM_QUIT) {
			TranslateMessage(&Msg);
			PushMessage(&Msg);
		}
	}
}

bool IncProgress()
{
	interface_msg_pump();
	sgdwProgress += 23;
	if (sgdwProgress > 534)
		sgdwProgress = 534;
	if (sgpBackCel)
		DrawCutscene();
	return sgdwProgress >= 534;
}

void ShowProgress(interface_mode uMsg)
{
	WNDPROC saveProc;

	gbSomebodyWonGameKludge = FALSE;
	plrmsg_delay(TRUE);

	assert(ghMainWnd);
	saveProc = SetWindowProc(DisableInputWndProc);

	interface_msg_pump();
	ClearScreenBuffer();
	scrollrt_draw_game_screen(TRUE);
	InitCutscene(uMsg);
	BlackPalette();
	DrawCutscene();
	PaletteFadeIn(8);
	IncProgress();
	sound_init();
	IncProgress();

	switch (uMsg) {
	case WM_DIABLOADGAME:
		IncProgress();
		IncProgress();
		LoadGame(TRUE);
		IncProgress();
		IncProgress();
		break;
	case WM_DIABNEWGAME:
		plr[myplr].pOriginalCathedral = !gbIsHellfire;
		IncProgress();
		FreeGameMem();
		IncProgress();
		pfile_remove_temp_files();
		IncProgress();
		LoadGameLevel(TRUE, ENTRY_MAIN);
		IncProgress();
		break;
	case WM_DIABNEXTLVL:
		IncProgress();
		if (!gbIsMultiplayer) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		FreeGameMem();
		currlevel++;
		leveltype = gnLevelTypeTbl[currlevel];
		assert(plr[myplr].plrlevel == currlevel);
		IncProgress();
		LoadGameLevel(FALSE, ENTRY_MAIN);
		IncProgress();
		break;
	case WM_DIABPREVLVL:
		IncProgress();
		if (!gbIsMultiplayer) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		FreeGameMem();
		currlevel--;
		leveltype = gnLevelTypeTbl[currlevel];
		assert(plr[myplr].plrlevel == currlevel);
		IncProgress();
		LoadGameLevel(FALSE, ENTRY_PREV);
		IncProgress();
		break;
	case WM_DIABSETLVL:
		SetReturnLvlPos();
		IncProgress();
		if (!gbIsMultiplayer) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		setlevel = TRUE;
		leveltype = setlvltype;
		FreeGameMem();
		IncProgress();
		LoadGameLevel(FALSE, ENTRY_SETLVL);
		IncProgress();
		break;
	case WM_DIABRTNLVL:
		IncProgress();
		if (!gbIsMultiplayer) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		setlevel = FALSE;
		FreeGameMem();
		IncProgress();
		GetReturnLvlPos();
		LoadGameLevel(FALSE, ENTRY_RTNLVL);
		IncProgress();
		break;
	case WM_DIABWARPLVL:
		IncProgress();
		if (!gbIsMultiplayer) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		FreeGameMem();
		GetPortalLevel();
		IncProgress();
		LoadGameLevel(FALSE, ENTRY_WARPLVL);
		IncProgress();
		break;
	case WM_DIABTOWNWARP:
		IncProgress();
		if (!gbIsMultiplayer) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		FreeGameMem();
		currlevel = plr[myplr].plrlevel;
		leveltype = gnLevelTypeTbl[currlevel];
		assert(plr[myplr].plrlevel == currlevel);
		IncProgress();
		LoadGameLevel(FALSE, ENTRY_TWARPDN);
		IncProgress();
		break;
	case WM_DIABTWARPUP:
		IncProgress();
		if (!gbIsMultiplayer) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		FreeGameMem();
		currlevel = plr[myplr].plrlevel;
		leveltype = gnLevelTypeTbl[currlevel];
		assert(plr[myplr].plrlevel == currlevel);
		IncProgress();
		LoadGameLevel(FALSE, ENTRY_TWARPUP);
		IncProgress();
		break;
	case WM_DIABRETOWN:
		IncProgress();
		if (!gbIsMultiplayer) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		FreeGameMem();
		currlevel = plr[myplr].plrlevel;
		leveltype = gnLevelTypeTbl[currlevel];
		assert(plr[myplr].plrlevel == currlevel);
		IncProgress();
		LoadGameLevel(FALSE, ENTRY_MAIN);
		IncProgress();
		break;
	}

	assert(ghMainWnd);

	PaletteFadeOut(8);
	FreeInterface();

	saveProc = SetWindowProc(saveProc);
	assert(saveProc == DisableInputWndProc);

	NetSendCmdLocParam1(TRUE, CMD_PLAYER_JOINLEVEL, plr[myplr]._px, plr[myplr]._py, plr[myplr].plrlevel);
	plrmsg_delay(FALSE);
	ResetPal();

	if (gbSomebodyWonGameKludge && plr[myplr].plrlevel == 16) {
		PrepDoEnding();
	}

	gbSomebodyWonGameKludge = FALSE;
}

DEVILUTION_END_NAMESPACE
