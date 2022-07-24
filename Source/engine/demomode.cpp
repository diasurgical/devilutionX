#include "engine/demomode.h"

#include <deque>
#include <fstream>
#include <iostream>
#include <sstream>

#include "controls/plrctrls.h"
#include "menu.h"
#include "nthread.h"
#include "options.h"
#include "pfile.h"
#include "utils/display.h"
#include "utils/endian_stream.hpp"
#include "utils/paths.h"
#include "utils/str_cat.hpp"

namespace devilution {

namespace {

enum class DemoMsgType {
	GameTick = 0,
	Rendering = 1,
	Message = 2,
};

struct DemoMsg {
	DemoMsgType type;
	uint32_t message;
	uint32_t wParam;
	uint16_t lParam;
	float progressToNextGameTick;
};

int DemoNumber = -1;
bool Timedemo = false;
int RecordNumber = -1;
bool CreateDemoReference = false;

std::ofstream DemoRecording;
std::deque<DemoMsg> Demo_Message_Queue;
uint32_t DemoModeLastTick = 0;

int LogicTick = 0;
int StartTime = 0;

uint16_t DemoGraphicsWidth = 640;
uint16_t DemoGraphicsHeight = 480;

void PumpDemoMessage(DemoMsgType demoMsgType, uint32_t message, uint32_t wParam, uint16_t lParam, float progressToNextGameTick)
{
	Demo_Message_Queue.push_back(DemoMsg { demoMsgType, message, wParam, lParam, progressToNextGameTick });
}

bool LoadDemoMessages(int i)
{
	std::ifstream demofile;
	demofile.open(StrCat(paths::PrefPath(), "demo_", i, ".dmo"), std::fstream::binary);
	if (!demofile.is_open()) {
		return false;
	}

	const uint8_t version = ReadByte(demofile);
	if (version != 0) {
		return false;
	}

	gSaveNumber = ReadLE32(demofile);
	DemoGraphicsWidth = ReadLE16(demofile);
	DemoGraphicsHeight = ReadLE16(demofile);

	while (!demofile.eof()) {
		const uint32_t typeNum = ReadLE32(demofile);
		const auto type = static_cast<DemoMsgType>(typeNum);

		const float progressToNextGameTick = ReadLEFloat(demofile);

		switch (type) {
		case DemoMsgType::Message: {
			const uint32_t message = ReadLE32(demofile);
			const uint32_t wParam = ReadLE32(demofile);
			const uint16_t lParam = ReadLE16(demofile);
			PumpDemoMessage(type, message, wParam, lParam, progressToNextGameTick);
			break;
		}
		default:
			PumpDemoMessage(type, 0, 0, 0, progressToNextGameTick);
			break;
		}
	}

	demofile.close();

	DemoModeLastTick = SDL_GetTicks();

	return true;
}

} // namespace

namespace demo {

void InitPlayBack(int demoNumber, bool timedemo)
{
	DemoNumber = demoNumber;
	Timedemo = timedemo;
	ControlMode = ControlTypes::KeyboardAndMouse;

	if (!LoadDemoMessages(demoNumber)) {
		SDL_Log("Unable to load demo file");
		diablo_quit(1);
	}
}
void InitRecording(int recordNumber, bool createDemoReference)
{
	RecordNumber = recordNumber;
	CreateDemoReference = createDemoReference;
}
void OverrideOptions()
{
#ifndef USE_SDL1
	sgOptions.Graphics.fitToScreen.SetValue(false);
#endif
#if SDL_VERSION_ATLEAST(2, 0, 0)
	sgOptions.Graphics.hardwareCursor.SetValue(false);
#endif
	if (Timedemo) {
#ifndef USE_SDL1
		sgOptions.Graphics.vSync.SetValue(false);
#endif
		sgOptions.Graphics.limitFPS.SetValue(false);
	}
}

bool IsRunning()
{
	return DemoNumber != -1;
}

bool IsRecording()
{
	return RecordNumber != -1;
};

bool GetRunGameLoop(bool &drawGame, bool &processInput)
{
	if (Demo_Message_Queue.empty())
		app_fatal("Demo queue empty");
	const DemoMsg dmsg = Demo_Message_Queue.front();
	if (dmsg.type == DemoMsgType::Message)
		app_fatal("Unexpected Message");
	if (Timedemo) {
		// disable additonal rendering to speedup replay
		drawGame = dmsg.type == DemoMsgType::GameTick && !HeadlessMode;
	} else {
		int currentTickCount = SDL_GetTicks();
		int ticksElapsed = currentTickCount - DemoModeLastTick;
		bool tickDue = ticksElapsed >= gnTickDelay;
		drawGame = false;
		if (tickDue) {
			if (dmsg.type == DemoMsgType::GameTick) {
				DemoModeLastTick = currentTickCount;
			}
		} else {
			float progressToNextGameTick = clamp((float)ticksElapsed / (float)gnTickDelay, 0.F, 1.F);
			if (dmsg.type == DemoMsgType::GameTick || dmsg.progressToNextGameTick > progressToNextGameTick) {
				// we are ahead of the replay => add a additional rendering for smoothness
				gfProgressToNextGameTick = progressToNextGameTick;
				processInput = false;
				drawGame = true;
				return false;
			}
		}
	}
	gfProgressToNextGameTick = dmsg.progressToNextGameTick;
	Demo_Message_Queue.pop_front();
	if (dmsg.type == DemoMsgType::GameTick)
		LogicTick++;
	return dmsg.type == DemoMsgType::GameTick;
}

bool FetchMessage(tagMSG *lpMsg)
{
	SDL_Event e;
	if (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT) {
			lpMsg->message = DVL_WM_QUIT;
			lpMsg->wParam = 0;
			lpMsg->lParam = 0;
			return true;
		}
		if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
			Demo_Message_Queue.clear();
			ClearMessageQueue();
			DemoNumber = -1;
			Timedemo = false;
			last_tick = SDL_GetTicks();
		}
		if (e.type == SDL_KEYDOWN && IsAnyOf(e.key.keysym.sym, SDLK_KP_PLUS, SDLK_PLUS) && sgGameInitInfo.nTickRate < 255) {
			sgGameInitInfo.nTickRate++;
			sgOptions.Gameplay.tickRate.SetValue(sgGameInitInfo.nTickRate);
			gnTickDelay = 1000 / sgGameInitInfo.nTickRate;
		}
		if (e.type == SDL_KEYDOWN && IsAnyOf(e.key.keysym.sym, SDLK_KP_MINUS, SDLK_MINUS) && sgGameInitInfo.nTickRate > 1) {
			sgGameInitInfo.nTickRate--;
			sgOptions.Gameplay.tickRate.SetValue(sgGameInitInfo.nTickRate);
			gnTickDelay = 1000 / sgGameInitInfo.nTickRate;
		}
	}

	if (!Demo_Message_Queue.empty()) {
		const DemoMsg dmsg = Demo_Message_Queue.front();
		if (dmsg.type == DemoMsgType::Message) {
			lpMsg->message = dmsg.message;
			lpMsg->wParam = dmsg.wParam;
			lpMsg->lParam = dmsg.lParam;
			gfProgressToNextGameTick = dmsg.progressToNextGameTick;
			Demo_Message_Queue.pop_front();
			return true;
		}
	}

	lpMsg->message = 0;
	lpMsg->wParam = 0;
	lpMsg->lParam = 0;

	return false;
}

void RecordGameLoopResult(bool runGameLoop)
{
	WriteLE32(DemoRecording, static_cast<uint32_t>(runGameLoop ? DemoMsgType::GameTick : DemoMsgType::Rendering));
	WriteLEFloat(DemoRecording, gfProgressToNextGameTick);
}

void RecordMessage(tagMSG *lpMsg)
{
	if (!gbRunGame || !DemoRecording.is_open())
		return;
	WriteLE32(DemoRecording, static_cast<uint32_t>(DemoMsgType::Message));
	WriteLEFloat(DemoRecording, gfProgressToNextGameTick);
	WriteLE32(DemoRecording, lpMsg->message);
	WriteLE32(DemoRecording, lpMsg->wParam);
	WriteLE16(DemoRecording, lpMsg->lParam);
}

void NotifyGameLoopStart()
{
	if (IsRecording()) {
		DemoRecording.open(StrCat(paths::PrefPath(), "demo_", RecordNumber, ".dmo"), std::fstream::trunc | std::fstream::binary);
		constexpr uint8_t Version = 0;
		WriteByte(DemoRecording, Version);
		WriteLE32(DemoRecording, gSaveNumber);
		WriteLE16(DemoRecording, gnScreenWidth);
		WriteLE16(DemoRecording, gnScreenHeight);
	}

	if (IsRunning()) {
		StartTime = SDL_GetTicks();
		LogicTick = 0;
	}
}

void NotifyGameLoopEnd()
{
	if (IsRecording()) {
		DemoRecording.close();
		if (CreateDemoReference)
			pfile_write_hero_demo(RecordNumber);

		RecordNumber = -1;
		CreateDemoReference = false;
	}

	if (IsRunning() && !HeadlessMode) {
		float seconds = (SDL_GetTicks() - StartTime) / 1000.0f;
		SDL_Log("%d frames, %.2f seconds: %.1f fps", LogicTick, seconds, LogicTick / seconds);
		gbRunGameResult = false;
		gbRunGame = false;

		HeroCompareResult compareResult = pfile_compare_hero_demo(DemoNumber);
		switch (compareResult) {
		case HeroCompareResult::ReferenceNotFound:
			SDL_Log("Timedemo: No final comparision cause reference is not present.");
			break;
		case HeroCompareResult::Same:
			SDL_Log("Timedemo: Same outcome as inital run. :)");
			break;
		case HeroCompareResult::Difference:
			SDL_Log("Timedemo: Different outcome then inital run. ;(");
			break;
		}
	}
}

} // namespace demo

} // namespace devilution
