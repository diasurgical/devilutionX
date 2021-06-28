/**
 * @file interfac.cpp
 *
 * Implementation of load screens.
 */

#include <cstdint>

#include "DiabloUI/art_draw.h"
#include "control.h"
#include "dx.h"
#include "engine.h"
#include "engine/cel_sprite.hpp"
#include "engine/load_cel.hpp"
#include "engine/render/cel_render.hpp"
#include "hwcursor.hpp"
#include "init.h"
#include "loadsave.h"
#include "palette.h"
#include "pfile.h"
#include "plrmsg.h"
#include "utils/sdl_geometry.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {
namespace {
std::optional<CelSprite> sgpBackCel;
} // namespace

uint32_t sgdwProgress;
int progress_id;

/** The color used for the progress bar as an index into the palette. */
const BYTE BarColor[3] = { 138, 43, 254 };
/** The screen position of the top left corner of the progress bar. */
const int BarPos[3][2] = { { 53, 37 }, { 53, 421 }, { 53, 37 } };

Art ArtCutsceneWidescreen;

static void FreeInterface()
{
	sgpBackCel = std::nullopt;
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
		int lvl = plr[myplr].plrlevel;
		if (lvl == 1 && uMsg == WM_DIABNEXTLVL)
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
	sgpBackCel = LoadCel(celPath, 640);
	LoadPalette(palPath);

	sgdwProgress = 0;
}

static void DrawCutscene()
{
	lock_buf(1);
	const CelOutputBuffer &out = GlobalBackBuffer();
	DrawArt(out, { PANEL_X - (ArtCutsceneWidescreen.w() - PANEL_WIDTH) / 2, UI_OFFSET_Y }, &ArtCutsceneWidescreen);
	CelDrawTo(out, { PANEL_X, 480 - 1 + UI_OFFSET_Y }, *sgpBackCel, 1);

	constexpr int ProgressHeight = 22;
	SDL_Rect rect = MakeSdlRect(
	    out.region.x + BarPos[progress_id][0] + PANEL_X,
	    out.region.y + BarPos[progress_id][1] + UI_OFFSET_Y,
	    sgdwProgress,
	    ProgressHeight);
	SDL_FillRect(out.surface, &rect, BarColor[progress_id]);

	unlock_buf(1);

	BltFast(&rect, &rect);
	RenderPresent();
}

void interface_msg_pump()
{
	tagMSG Msg;

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

	gbSomebodyWonGameKludge = false;
	plrmsg_delay(true);

	assert(ghMainWnd);
	saveProc = SetWindowProc(DisableInputWndProc);

	interface_msg_pump();
	ClearScreenBuffer();
	scrollrt_draw_game_screen();
	InitCutscene(uMsg);
	BlackPalette();
	DrawCutscene();

	if (IsHardwareCursor())
		SetHardwareCursorVisible(false);

	PaletteFadeIn(8);
	IncProgress();
	sound_init();
	IncProgress();

	auto &myPlayer = plr[myplr];

	switch (uMsg) {
	case WM_DIABLOADGAME:
		IncProgress();
		IncProgress();
		LoadGame(true);
		IncProgress();
		IncProgress();
		break;
	case WM_DIABNEWGAME:
		myPlayer.pOriginalCathedral = !gbIsHellfire;
		IncProgress();
		FreeGameMem();
		IncProgress();
		pfile_remove_temp_files();
		IncProgress();
		LoadGameLevel(true, ENTRY_MAIN);
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
		assert(myPlayer.plrlevel == currlevel);
		IncProgress();
		LoadGameLevel(false, ENTRY_MAIN);
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
		assert(myPlayer.plrlevel == currlevel);
		IncProgress();
		LoadGameLevel(false, ENTRY_PREV);
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
		setlevel = true;
		leveltype = setlvltype;
		FreeGameMem();
		IncProgress();
		LoadGameLevel(false, ENTRY_SETLVL);
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
		setlevel = false;
		FreeGameMem();
		IncProgress();
		GetReturnLvlPos();
		LoadGameLevel(false, ENTRY_RTNLVL);
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
		LoadGameLevel(false, ENTRY_WARPLVL);
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
		currlevel = myPlayer.plrlevel;
		leveltype = gnLevelTypeTbl[currlevel];
		IncProgress();
		LoadGameLevel(false, ENTRY_TWARPDN);
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
		currlevel = myPlayer.plrlevel;
		leveltype = gnLevelTypeTbl[currlevel];
		IncProgress();
		LoadGameLevel(false, ENTRY_TWARPUP);
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
		currlevel = myPlayer.plrlevel;
		leveltype = gnLevelTypeTbl[currlevel];
		IncProgress();
		LoadGameLevel(false, ENTRY_MAIN);
		IncProgress();
		break;
	}

	assert(ghMainWnd);

	PaletteFadeOut(8);
	FreeInterface();

	saveProc = SetWindowProc(saveProc);
	assert(saveProc == DisableInputWndProc);

	NetSendCmdLocParam1(true, CMD_PLAYER_JOINLEVEL, myPlayer.position.tile, myPlayer.plrlevel);
	plrmsg_delay(false);
	ResetPal();

	if (gbSomebodyWonGameKludge && myPlayer.plrlevel == 16) {
		PrepDoEnding();
	}

	gbSomebodyWonGameKludge = false;
}

} // namespace devilution
