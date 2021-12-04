/**
 * @file animationinfo.h
 *
 * Contains most of the the demomode specific logic
 */

#include <deque>
#include <fstream>
#include <iostream>
#include <sstream>

#include "demomode.h"
#include "menu.h"
#include "nthread.h"
#include "options.h"
#include "pfile.h"
#include "utils/display.h"
#include "utils/paths.h"

namespace devilution {

namespace {

enum class DemoMsgType {
	GameTick = 0,
	Rendering = 1,
	Message = 2,
};

struct demoMsg {
	DemoMsgType type;
	uint32_t message;
	int32_t wParam;
	int32_t lParam;
	float progressToNextGameTick;
};

int DemoNumber = -1;
bool Timedemo = false;
int RecordNumber = -1;

std::ofstream DemoRecording;
std::deque<demoMsg> Demo_Message_Queue;
uint32_t DemoModeLastTick = 0;

int LogicTick = 0;
int StartTime = 0;

int DemoGraphicsWidth = 640;
int DemoGraphicsHeight = 480;

void PumpDemoMessage(DemoMsgType demoMsgType, uint32_t message, int32_t wParam, int32_t lParam, float progressToNextGameTick)
{
	demoMsg msg;
	msg.type = demoMsgType;
	msg.message = message;
	msg.wParam = wParam;
	msg.lParam = lParam;
	msg.progressToNextGameTick = progressToNextGameTick;

	Demo_Message_Queue.push_back(msg);
}

bool LoadDemoMessages(int i)
{
	std::ifstream demofile;
	char demoFilename[16];
	snprintf(demoFilename, 15, "demo_%d.dmo", i);
	demofile.open(paths::PrefPath() + demoFilename);
	if (!demofile.is_open()) {
		return false;
	}

	std::string line;
	std::getline(demofile, line);
	std::stringstream header(line);

	std::string number;
	std::getline(header, number, ','); // Demo version
	if (std::stoi(number) != 0) {
		return false;
	}

	std::getline(header, number, ',');
	gSaveNumber = std::stoi(number);

	std::getline(header, number, ',');
	DemoGraphicsWidth = std::stoi(number);

	std::getline(header, number, ',');
	DemoGraphicsHeight = std::stoi(number);

	while (std::getline(demofile, line)) {
		std::stringstream command(line);

		std::getline(command, number, ',');
		int typeNum = std::stoi(number);
		auto type = static_cast<DemoMsgType>(typeNum);

		std::getline(command, number, ',');
		float progressToNextGameTick = std::stof(number);

		switch (type) {
		case DemoMsgType::Message: {
			std::getline(command, number, ',');
			uint32_t message = std::stoi(number);
			std::getline(command, number, ',');
			int32_t wParam = std::stoi(number);
			std::getline(command, number, ',');
			int32_t lParam = std::stoi(number);
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

	if (!LoadDemoMessages(demoNumber)) {
		SDL_Log("Unable to load demo file");
		diablo_quit(1);
	}
}
void InitRecording(int recordNumber)
{
	RecordNumber = recordNumber;
}
void OverrideOptions()
{
	sgOptions.Graphics.nWidth = DemoGraphicsWidth;
	sgOptions.Graphics.nHeight = DemoGraphicsHeight;
#ifndef USE_SDL1
	sgOptions.Graphics.fitToScreen.SetValue(false);
#endif
#if SDL_VERSION_ATLEAST(2, 0, 0)
	sgOptions.Graphics.bHardwareCursor = false;
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
	demoMsg dmsg = Demo_Message_Queue.front();
	if (dmsg.type == DemoMsgType::Message)
		app_fatal("Unexpected Message");
	if (Timedemo) {
		// disable additonal rendering to speedup replay
		drawGame = dmsg.type == DemoMsgType::GameTick;
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
			lpMsg->lParam = 0;
			lpMsg->wParam = 0;
			return true;
		}
		if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
			Demo_Message_Queue.clear();
			ClearMessageQueue();
			DemoNumber = -1;
			Timedemo = false;
			last_tick = SDL_GetTicks();
		}
		if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_KP_PLUS && sgGameInitInfo.nTickRate < 255) {
			sgGameInitInfo.nTickRate++;
			sgOptions.Gameplay.nTickRate = sgGameInitInfo.nTickRate;
			gnTickDelay = 1000 / sgGameInitInfo.nTickRate;
		}
		if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_KP_MINUS && sgGameInitInfo.nTickRate > 1) {
			sgGameInitInfo.nTickRate--;
			sgOptions.Gameplay.nTickRate = sgGameInitInfo.nTickRate;
			gnTickDelay = 1000 / sgGameInitInfo.nTickRate;
		}
	}

	if (!Demo_Message_Queue.empty()) {
		demoMsg dmsg = Demo_Message_Queue.front();
		if (dmsg.type == DemoMsgType::Message) {
			lpMsg->message = dmsg.message;
			lpMsg->lParam = dmsg.lParam;
			lpMsg->wParam = dmsg.wParam;
			gfProgressToNextGameTick = dmsg.progressToNextGameTick;
			Demo_Message_Queue.pop_front();
			return true;
		}
	}

	lpMsg->message = 0;
	lpMsg->lParam = 0;
	lpMsg->wParam = 0;

	return false;
}

void RecordGameLoopResult(bool runGameLoop)
{
	DemoRecording << static_cast<uint32_t>(runGameLoop ? DemoMsgType::GameTick : DemoMsgType::Rendering) << "," << gfProgressToNextGameTick << "\n";
}

void RecordMessage(tagMSG *lpMsg)
{
	if (!gbRunGame || !DemoRecording.is_open())
		return;
	DemoRecording << static_cast<uint32_t>(DemoMsgType::Message) << "," << gfProgressToNextGameTick << "," << lpMsg->message << "," << lpMsg->wParam << "," << lpMsg->lParam << "\n";
}

void NotifyGameLoopStart()
{
	if (IsRecording()) {
		char demoFilename[16];
		snprintf(demoFilename, 15, "demo_%d.dmo", RecordNumber);
		DemoRecording.open(paths::PrefPath() + demoFilename, std::fstream::trunc);
		DemoRecording << "0," << gSaveNumber << "," << gnScreenWidth << "," << gnScreenHeight << "\n";
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

		RecordNumber = -1;
	}

	if (IsRunning()) {
		float secounds = (SDL_GetTicks() - StartTime) / 1000.0;
		SDL_Log("%d frames, %.2f seconds: %.1f fps", LogicTick, secounds, LogicTick / secounds);
		gbRunGameResult = false;
		gbRunGame = false;
	}
}

} // namespace demo

} // namespace devilution
