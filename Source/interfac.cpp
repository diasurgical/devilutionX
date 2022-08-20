/**
 * @file interfac.cpp
 *
 * Implementation of load screens.
 */

#include <cstdint>

#include <SDL.h>

#include "control.h"
#include "engine.h"
#include "engine/clx_sprite.hpp"
#include "engine/dx.h"
#include "engine/load_cel.hpp"
#include "engine/load_clx.hpp"
#include "engine/palette.h"
#include "engine/render/clx_render.hpp"
#include "hwcursor.hpp"
#include "init.h"
#include "loadsave.h"
#include "miniwin/misc_msg.h"
#include "pfile.h"
#include "plrmsg.h"
#include "utils/sdl_geometry.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

namespace {

constexpr uint32_t MaxProgress = 534;

OptionalOwnedClxSpriteList sgpBackCel;

bool IsProgress;
uint32_t sgdwProgress;
int progress_id;

/** The color used for the progress bar as an index into the palette. */
const uint8_t BarColor[3] = { 138, 43, 254 };
/** The screen position of the top left corner of the progress bar. */
const int BarPos[3][2] = { { 53, 37 }, { 53, 421 }, { 53, 37 } };

OptionalOwnedClxSpriteList ArtCutsceneWidescreen;

uint32_t CustomEventsBegin = SDL_USEREVENT;
constexpr uint32_t NumCustomEvents = WM_LAST - WM_FIRST + 1;

Cutscenes PickCutscene(interface_mode uMsg)
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
		int lvl = MyPlayer->plrlevel;
		if (lvl == 1 && uMsg == WM_DIABNEXTLVL)
			return CutTown;
		if (lvl == 16 && uMsg == WM_DIABNEXTLVL)
			return CutGate;

		switch (GetLevelType(lvl)) {
		case DTYPE_TOWN:
			return CutTown;
		case DTYPE_CATHEDRAL:
			return CutLevel1;
		case DTYPE_CATACOMBS:
			return CutLevel2;
		case DTYPE_CAVES:
			return CutLevel3;
		case DTYPE_HELL:
			return CutLevel4;
		case DTYPE_NEST:
			return CutLevel6;
		case DTYPE_CRYPT:
			return CutLevel5;
		default:
			return CutLevel1;
		}
	}
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

void LoadCutsceneBackground(interface_mode uMsg)
{
	const char *celPath;
	const char *palPath;

	switch (PickCutscene(uMsg)) {
	case CutStart:
		ArtCutsceneWidescreen = LoadOptionalClx("gendata\\cutstartw.pcx");
		celPath = "gendata\\cutstart.cel";
		palPath = "gendata\\cutstart.pal";
		progress_id = 1;
		break;
	case CutTown:
		celPath = "gendata\\cuttt.cel";
		palPath = "gendata\\cuttt.pal";
		progress_id = 1;
		break;
	case CutLevel1:
		celPath = "gendata\\cutl1d.cel";
		palPath = "gendata\\cutl1d.pal";
		progress_id = 0;
		break;
	case CutLevel2:
		celPath = "gendata\\cut2.cel";
		palPath = "gendata\\cut2.pal";
		progress_id = 2;
		break;
	case CutLevel3:
		celPath = "gendata\\cut3.cel";
		palPath = "gendata\\cut3.pal";
		progress_id = 1;
		break;
	case CutLevel4:
		celPath = "gendata\\cut4.cel";
		palPath = "gendata\\cut4.pal";
		progress_id = 1;
		break;
	case CutLevel5:
		celPath = "nlevels\\cutl5.cel";
		palPath = "nlevels\\cutl5.pal";
		progress_id = 1;
		break;
	case CutLevel6:
		celPath = "nlevels\\cutl6.cel";
		palPath = "nlevels\\cutl6.pal";
		progress_id = 1;
		break;
	case CutPortal:
		ArtCutsceneWidescreen = LoadOptionalClx("gendata\\cutportlw.pcx");
		celPath = "gendata\\cutportl.cel";
		palPath = "gendata\\cutportl.pal";
		progress_id = 1;
		break;
	case CutPortalRed:
		ArtCutsceneWidescreen = LoadOptionalClx("gendata\\cutportrw.pcx");
		celPath = "gendata\\cutportr.cel";
		palPath = "gendata\\cutportr.pal";
		progress_id = 1;
		break;
	case CutGate:
		celPath = "gendata\\cutgate.cel";
		palPath = "gendata\\cutgate.pal";
		progress_id = 1;
		break;
	}

	assert(!sgpBackCel);
	sgpBackCel = LoadCel(celPath, 640);
	LoadPalette(palPath);

	sgdwProgress = 0;
}

void FreeCutsceneBackground()
{
	sgpBackCel = std::nullopt;
	ArtCutsceneWidescreen = std::nullopt;
}

void DrawCutsceneBackground()
{
	const Rectangle &uiRectangle = GetUIRectangle();
	const Surface &out = GlobalBackBuffer();
	if (ArtCutsceneWidescreen) {
		const ClxSprite sprite = (*ArtCutsceneWidescreen)[0];
		RenderClxSprite(out, sprite, { uiRectangle.position.x - (sprite.width() - uiRectangle.size.width) / 2, uiRectangle.position.y });
	}
	ClxDraw(out, { uiRectangle.position.x, 480 - 1 + uiRectangle.position.y }, (*sgpBackCel)[0]);
}

void DrawCutsceneForeground()
{
	const Rectangle &uiRectangle = GetUIRectangle();
	const Surface &out = GlobalBackBuffer();
	constexpr int ProgressHeight = 22;
	SDL_Rect rect = MakeSdlRect(
	    out.region.x + BarPos[progress_id][0] + uiRectangle.position.x,
	    out.region.y + BarPos[progress_id][1] + uiRectangle.position.y,
	    sgdwProgress,
	    ProgressHeight);
	SDL_FillRect(out.surface, &rect, BarColor[progress_id]);

	if (DiabloUiSurface() == PalSurface)
		BltFast(&rect, &rect);
	RenderPresent();
}

} // namespace

void RegisterCustomEvents()
{
#ifndef USE_SDL1
	CustomEventsBegin = SDL_RegisterEvents(NumCustomEvents);
#endif
}

bool IsCustomEvent(uint32_t eventType)
{
	return eventType >= CustomEventsBegin && eventType < CustomEventsBegin + NumCustomEvents;
}

interface_mode GetCustomEvent(uint32_t eventType)
{
	return static_cast<interface_mode>(eventType - CustomEventsBegin);
}

uint32_t CustomEventToSdlEvent(interface_mode eventType)
{
	return CustomEventsBegin + eventType;
}

void interface_msg_pump()
{
	SDL_Event event;
	uint16_t modState;
	while (FetchMessage(&event, &modState)) {
		if (event.type != SDL_QUIT) {
			HandleMessage(event, modState);
		}
	}
}

void IncProgress()
{
	if (HeadlessMode)
		return;
	interface_msg_pump();
	if (!IsProgress)
		return;
	sgdwProgress += 23;
	if (sgdwProgress > MaxProgress)
		sgdwProgress = MaxProgress;
	DrawCutsceneForeground();
}

void CompleteProgress()
{
	if (HeadlessMode)
		return;
	if (!IsProgress)
		return;
	while (sgdwProgress < MaxProgress)
		IncProgress();
}

void ShowProgress(interface_mode uMsg)
{
	IsProgress = true;

	gbSomebodyWonGameKludge = false;
	plrmsg_delay(true);

	EventHandler previousHandler = SetEventHandler(DisableInputEventHandler);

	if (!HeadlessMode) {
		assert(ghMainWnd);

		interface_msg_pump();
		ClearScreenBuffer();
		scrollrt_draw_game_screen();
		BlackPalette();

		// Blit the background once and then free it.
		LoadCutsceneBackground(uMsg);
		DrawCutsceneBackground();
		if (RenderDirectlyToOutputSurface && IsDoubleBuffered()) {
			// Blit twice for triple buffering.
			for (unsigned i = 0; i < 2; ++i) {
				if (DiabloUiSurface() == PalSurface)
					BltFast(nullptr, nullptr);
				RenderPresent();
				DrawCutsceneBackground();
			}
		}
		FreeCutsceneBackground();

		if (IsHardwareCursor())
			SetHardwareCursorVisible(false);

		PaletteFadeIn(8);
		IncProgress();
		sound_init();
		IncProgress();
	}

	Player &myPlayer = *MyPlayer;

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
			pfile_save_level();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		FreeGameMem();
		setlevel = false;
		currlevel = myPlayer.plrlevel;
		leveltype = GetLevelType(currlevel);
		IncProgress();
		LoadGameLevel(false, ENTRY_MAIN);
		IncProgress();
		break;
	case WM_DIABPREVLVL:
		IncProgress();
		if (!gbIsMultiplayer) {
			pfile_save_level();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		FreeGameMem();
		currlevel--;
		leveltype = GetLevelType(currlevel);
		assert(myPlayer.isOnActiveLevel());
		IncProgress();
		LoadGameLevel(false, ENTRY_PREV);
		IncProgress();
		break;
	case WM_DIABSETLVL:
		SetReturnLvlPos();
		IncProgress();
		if (!gbIsMultiplayer) {
			pfile_save_level();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		setlevel = true;
		leveltype = setlvltype;
		currlevel = static_cast<uint8_t>(setlvlnum);
		FreeGameMem();
		IncProgress();
		LoadGameLevel(false, ENTRY_SETLVL);
		IncProgress();
		break;
	case WM_DIABRTNLVL:
		IncProgress();
		if (!gbIsMultiplayer) {
			pfile_save_level();
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
			pfile_save_level();
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
			pfile_save_level();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		FreeGameMem();
		setlevel = false;
		currlevel = myPlayer.plrlevel;
		leveltype = GetLevelType(currlevel);
		IncProgress();
		LoadGameLevel(false, ENTRY_TWARPDN);
		IncProgress();
		break;
	case WM_DIABTWARPUP:
		IncProgress();
		if (!gbIsMultiplayer) {
			pfile_save_level();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		FreeGameMem();
		currlevel = myPlayer.plrlevel;
		leveltype = GetLevelType(currlevel);
		IncProgress();
		LoadGameLevel(false, ENTRY_TWARPUP);
		IncProgress();
		break;
	case WM_DIABRETOWN:
		IncProgress();
		if (!gbIsMultiplayer) {
			pfile_save_level();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		FreeGameMem();
		setlevel = false;
		currlevel = myPlayer.plrlevel;
		leveltype = GetLevelType(currlevel);
		IncProgress();
		LoadGameLevel(false, ENTRY_MAIN);
		IncProgress();
		break;
	}

	if (!HeadlessMode) {
		assert(ghMainWnd);

		PaletteFadeOut(8);
	}

	previousHandler = SetEventHandler(previousHandler);
	assert(previousHandler == DisableInputEventHandler);
	IsProgress = false;

	NetSendCmdLocParam2(true, CMD_PLAYER_JOINLEVEL, myPlayer.position.tile, myPlayer.plrlevel, myPlayer.plrIsOnSetLevel ? 1 : 0);
	plrmsg_delay(false);

	if (gbSomebodyWonGameKludge && myPlayer.isOnLevel(16)) {
		PrepDoEnding();
	}

	gbSomebodyWonGameKludge = false;
}

} // namespace devilution
