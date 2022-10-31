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

enum class DemoMsgType : uint8_t {
	GameTick = 0,
	Rendering = 1,
	Message = 2,
};

struct MouseMotionEventData {
	uint16_t x;
	uint16_t y;
};

struct MouseButtonEventData {
	uint8_t button;
	uint16_t x;
	uint16_t y;
	uint16_t mod;
};

struct MouseWheelEventData {
	int32_t x;
	int32_t y;
	uint16_t mod;
};

struct KeyEventData {
	SDL_Keycode sym;
	SDL_Keymod mod;
};

struct DemoMsg {
	DemoMsgType type;
	float progressToNextGameTick;
	SDL_EventType eventType;
	union {
		MouseMotionEventData motion;
		MouseButtonEventData button;
		MouseWheelEventData wheel;
		KeyEventData key;
	};
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
			const auto eventType = static_cast<SDL_EventType>(ReadLE32(demofile));
			DemoMsg msg { type, progressToNextGameTick, eventType, {} };
			switch (eventType) {
			case SDL_MOUSEMOTION:
				msg.motion.x = ReadLE16(demofile);
				msg.motion.y = ReadLE16(demofile);
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				msg.button.button = ReadByte(demofile);
				msg.button.x = ReadLE16(demofile);
				msg.button.y = ReadLE16(demofile);
				msg.button.mod = ReadLE16(demofile);
				break;
#ifndef USE_SDL1
			case SDL_MOUSEWHEEL:
				msg.wheel.x = ReadLE32<int32_t>(demofile);
				msg.wheel.y = ReadLE32<int32_t>(demofile);
				msg.wheel.mod = ReadLE16(demofile);
				break;
#endif
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				msg.key.sym = static_cast<SDL_Keycode>(ReadLE32(demofile));
				msg.key.mod = static_cast<SDL_Keymod>(ReadLE16(demofile));
				break;
			case SDL_QUIT:
				break;
			default:
				if (eventType < SDL_USEREVENT) {
					app_fatal(StrCat("Unknown event ", static_cast<uint32_t>(eventType)));
				}
				break;
			}
			Demo_Message_Queue.push_back(msg);
			break;
		}
		default:
			Demo_Message_Queue.push_back(DemoMsg { type, progressToNextGameTick, static_cast<SDL_EventType>(0), {} });
			break;
		}
	}

	demofile.close();

	DemoModeLastTick = SDL_GetTicks();

	return true;
}

void RecordEventHeader(const SDL_Event &event)
{
	WriteLE32(DemoRecording, static_cast<uint32_t>(DemoMsgType::Message));
	WriteLEFloat(DemoRecording, gfProgressToNextGameTick);
	WriteLE32(DemoRecording, event.type);
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

bool FetchMessage(SDL_Event *event, uint16_t *modState)
{
	SDL_Event e;
	if (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT) {
			*event = e;
			return true;
		}
		if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
			Demo_Message_Queue.clear();
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
			event->type = dmsg.eventType;
			switch (dmsg.eventType) {
			case SDL_MOUSEMOTION:
				event->motion.x = dmsg.motion.x;
				event->motion.y = dmsg.motion.y;
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				event->button.button = dmsg.button.button;
				event->button.state = dmsg.eventType == SDL_MOUSEBUTTONDOWN ? SDL_PRESSED : SDL_RELEASED;
				event->button.x = dmsg.button.x;
				event->button.y = dmsg.button.y;
				*modState = dmsg.button.mod;
				break;
#ifndef USE_SDL1
			case SDL_MOUSEWHEEL:
				event->wheel.x = dmsg.wheel.x;
				event->wheel.y = dmsg.wheel.y;
				*modState = dmsg.wheel.mod;
				break;
#endif
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				event->key.state = dmsg.eventType == SDL_KEYDOWN ? SDL_PRESSED : SDL_RELEASED;
				event->key.keysym.sym = dmsg.key.sym;
				event->key.keysym.mod = dmsg.key.mod;
				break;
			default:
				if (dmsg.eventType >= SDL_USEREVENT) {
					event->type = CustomEventToSdlEvent(static_cast<interface_mode>(dmsg.eventType - SDL_USEREVENT));
				}
				break;
			}
			gfProgressToNextGameTick = dmsg.progressToNextGameTick;
			Demo_Message_Queue.pop_front();
			return true;
		}
	}

	return false;
}

void RecordGameLoopResult(bool runGameLoop)
{
	WriteLE32(DemoRecording, static_cast<uint32_t>(runGameLoop ? DemoMsgType::GameTick : DemoMsgType::Rendering));
	WriteLEFloat(DemoRecording, gfProgressToNextGameTick);
}

void RecordMessage(const SDL_Event &event, uint16_t modState)
{
	if (!gbRunGame || !DemoRecording.is_open())
		return;
	switch (event.type) {
	case SDL_MOUSEMOTION:
		RecordEventHeader(event);
		WriteLE16(DemoRecording, event.motion.x);
		WriteLE16(DemoRecording, event.motion.y);
		break;
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		RecordEventHeader(event);
		WriteByte(DemoRecording, event.button.button);
		WriteLE16(DemoRecording, event.button.x);
		WriteLE16(DemoRecording, event.button.y);
		WriteLE16(DemoRecording, modState);
		break;
#ifndef USE_SDL1
	case SDL_MOUSEWHEEL:
		RecordEventHeader(event);
		WriteLE32(DemoRecording, event.wheel.x);
		WriteLE32(DemoRecording, event.wheel.y);
		WriteLE16(DemoRecording, modState);
		break;
#endif
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		RecordEventHeader(event);
		WriteLE32(DemoRecording, static_cast<uint32_t>(event.key.keysym.sym));
		WriteLE16(DemoRecording, static_cast<uint16_t>(event.key.keysym.mod));
		break;
#ifndef USE_SDL1
	case SDL_WINDOWEVENT:
		if (event.window.type == SDL_WINDOWEVENT_CLOSE) {
			SDL_Event quitEvent;
			quitEvent.type = SDL_QUIT;
			RecordEventHeader(quitEvent);
		}
		break;
#endif
	case SDL_QUIT:
		RecordEventHeader(event);
		break;
	default:
		if (IsCustomEvent(event.type)) {
			SDL_Event stableCustomEvent;
			stableCustomEvent.type = SDL_USEREVENT + static_cast<uint32_t>(GetCustomEvent(event.type));
			RecordEventHeader(stableCustomEvent);
		}
		break;
	}
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

		HeroCompareResult compareResult = pfile_compare_hero_demo(DemoNumber, false);
		switch (compareResult.status) {
		case HeroCompareResult::ReferenceNotFound:
			SDL_Log("Timedemo: No final comparision cause reference is not present.");
			break;
		case HeroCompareResult::Same:
			SDL_Log("Timedemo: Same outcome as inital run. :)");
			break;
		case HeroCompareResult::Difference:
			Log("Timedemo: Different outcome then inital run. ;(\n{}", compareResult.message);
			break;
		}
	}
}

} // namespace demo

} // namespace devilution
