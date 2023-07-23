#include "engine/demomode.h"

#include <cstdint>
#include <cstdio>
#include <deque>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#include "controls/plrctrls.h"
#include "engine/events.hpp"
#include "gmenu.h"
#include "menu.h"
#include "nthread.h"
#include "options.h"
#include "pfile.h"
#include "utils/console.h"
#include "utils/display.h"
#include "utils/endian_stream.hpp"
#include "utils/paths.h"
#include "utils/str_cat.hpp"

namespace devilution {

// #define LOG_DEMOMODE_MESSAGES
// #define LOG_DEMOMODE_MESSAGES_MOUSEMOTION
// #define LOG_DEMOMODE_MESSAGES_RENDERING
// #define LOG_DEMOMODE_MESSAGES_GAMETICK

namespace {

enum class LoadingStatus : uint8_t {
	Success,
	FileNotFound,
	UnsupportedVersion,
};

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
	uint32_t sym;
	uint16_t mod;
};

struct DemoMsg {
	DemoMsgType type;
	uint8_t progressToNextGameTick;
	uint32_t eventType;
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

// These options affect gameplay and are stored in the demo file.
struct {
	uint8_t tickRate = 20;
	bool runInTown = false;
	bool theoQuest = false;
	bool cowQuest = false;
	bool autoGoldPickup = false;
	bool autoElixirPickup = false;
	bool autoOilPickup = false;
	bool autoPickupInTown = false;
	bool adriaRefillsMana = false;
	bool autoEquipWeapons = false;
	bool autoEquipArmor = false;
	bool autoEquipHelms = false;
	bool autoEquipShields = false;
	bool autoEquipJewelry = false;
	bool randomizeQuests = false;
	bool showItemLabels = false;
	bool autoRefillBelt = false;
	bool disableCripplingShrines = false;
	uint8_t numHealPotionPickup = 0;
	uint8_t numFullHealPotionPickup = 0;
	uint8_t numManaPotionPickup = 0;
	uint8_t numFullManaPotionPickup = 0;
	uint8_t numRejuPotionPickup = 0;
	uint8_t numFullRejuPotionPickup = 0;
} DemoSettings;

FILE *DemoRecording;
std::deque<DemoMsg> Demo_Message_Queue;
uint32_t DemoModeLastTick = 0;

int LogicTick = 0;
int StartTime = 0;

uint16_t DemoGraphicsWidth = 640;
uint16_t DemoGraphicsHeight = 480;

void ReadSettings(FILE *in, uint8_t version)
{
	DemoGraphicsWidth = ReadLE16(in);
	DemoGraphicsHeight = ReadLE16(in);
	if (version > 0) {
		DemoSettings.runInTown = ReadByte(in) != 0;
		DemoSettings.theoQuest = ReadByte(in) != 0;
		DemoSettings.cowQuest = ReadByte(in) != 0;
		DemoSettings.autoGoldPickup = ReadByte(in) != 0;
		DemoSettings.autoElixirPickup = ReadByte(in) != 0;
		DemoSettings.autoOilPickup = ReadByte(in) != 0;
		DemoSettings.autoPickupInTown = ReadByte(in) != 0;
		DemoSettings.adriaRefillsMana = ReadByte(in) != 0;
		DemoSettings.autoEquipWeapons = ReadByte(in) != 0;
		DemoSettings.autoEquipArmor = ReadByte(in) != 0;
		DemoSettings.autoEquipHelms = ReadByte(in) != 0;
		DemoSettings.autoEquipShields = ReadByte(in) != 0;
		DemoSettings.autoEquipJewelry = ReadByte(in) != 0;
		DemoSettings.randomizeQuests = ReadByte(in) != 0;
		DemoSettings.showItemLabels = ReadByte(in) != 0;
		DemoSettings.autoRefillBelt = ReadByte(in) != 0;
		DemoSettings.disableCripplingShrines = ReadByte(in) != 0;
		DemoSettings.numHealPotionPickup = ReadByte(in);
		DemoSettings.numFullHealPotionPickup = ReadByte(in);
		DemoSettings.numManaPotionPickup = ReadByte(in);
		DemoSettings.numFullManaPotionPickup = ReadByte(in);
		DemoSettings.numRejuPotionPickup = ReadByte(in);
		DemoSettings.numFullRejuPotionPickup = ReadByte(in);
	} else {
		DemoSettings = {};
	}
}

void WriteSettings(FILE *out)
{
	WriteLE16(out, gnScreenWidth);
	WriteLE16(out, gnScreenHeight);
	WriteByte(out, *sgOptions.Gameplay.runInTown);
	WriteByte(out, *sgOptions.Gameplay.theoQuest);
	WriteByte(out, *sgOptions.Gameplay.cowQuest);
	WriteByte(out, *sgOptions.Gameplay.autoGoldPickup);
	WriteByte(out, *sgOptions.Gameplay.autoElixirPickup);
	WriteByte(out, *sgOptions.Gameplay.autoOilPickup);
	WriteByte(out, *sgOptions.Gameplay.autoPickupInTown);
	WriteByte(out, *sgOptions.Gameplay.adriaRefillsMana);
	WriteByte(out, *sgOptions.Gameplay.autoEquipWeapons);
	WriteByte(out, *sgOptions.Gameplay.autoEquipArmor);
	WriteByte(out, *sgOptions.Gameplay.autoEquipHelms);
	WriteByte(out, *sgOptions.Gameplay.autoEquipShields);
	WriteByte(out, *sgOptions.Gameplay.autoEquipJewelry);
	WriteByte(out, *sgOptions.Gameplay.randomizeQuests);
	WriteByte(out, *sgOptions.Gameplay.showItemLabels);
	WriteByte(out, *sgOptions.Gameplay.autoRefillBelt);
	WriteByte(out, *sgOptions.Gameplay.disableCripplingShrines);
	WriteByte(out, *sgOptions.Gameplay.numHealPotionPickup);
	WriteByte(out, *sgOptions.Gameplay.numFullHealPotionPickup);
	WriteByte(out, *sgOptions.Gameplay.numManaPotionPickup);
	WriteByte(out, *sgOptions.Gameplay.numFullManaPotionPickup);
	WriteByte(out, *sgOptions.Gameplay.numRejuPotionPickup);
	WriteByte(out, *sgOptions.Gameplay.numFullRejuPotionPickup);
}

#if SDL_VERSION_ATLEAST(2, 0, 0)
bool CreateSdlEvent(const DemoMsg &dmsg, SDL_Event &event, uint16_t &modState)
{
	event.type = dmsg.eventType;
	switch (static_cast<SDL_EventType>(dmsg.eventType)) {
	case SDL_MOUSEMOTION:
		event.motion.which = 0;
		event.motion.x = dmsg.motion.x;
		event.motion.y = dmsg.motion.y;
		return true;
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		event.button.which = 0;
		event.button.button = dmsg.button.button;
		event.button.state = dmsg.eventType == SDL_MOUSEBUTTONDOWN ? SDL_PRESSED : SDL_RELEASED;
		event.button.x = dmsg.button.x;
		event.button.y = dmsg.button.y;
		modState = dmsg.button.mod;
		return true;
	case SDL_MOUSEWHEEL:
		event.wheel.which = 0;
		event.wheel.x = dmsg.wheel.x;
		event.wheel.y = dmsg.wheel.y;
		modState = dmsg.wheel.mod;
		return true;
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		event.key.state = dmsg.eventType == SDL_KEYDOWN ? SDL_PRESSED : SDL_RELEASED;
		event.key.keysym.sym = dmsg.key.sym;
		event.key.keysym.mod = dmsg.key.mod;
		return true;
	default:
		if (dmsg.eventType >= SDL_USEREVENT) {
			event.type = CustomEventToSdlEvent(static_cast<interface_mode>(dmsg.eventType - SDL_USEREVENT));
			return true;
		}
		event.type = static_cast<SDL_EventType>(0);
		LogWarn("Unsupported demo event (type={:x})", dmsg.eventType);
		return false;
	}
}
#else
SDLKey Sdl2ToSdl1Key(uint32_t key)
{
	if ((key & (1 << 30)) != 0) {
		constexpr uint32_t Keys1Start = 57;
		constexpr SDLKey Keys1[] {
			SDLK_CAPSLOCK, SDLK_F1, SDLK_F2, SDLK_F3, SDLK_F4, SDLK_F5, SDLK_F6,
			SDLK_F7, SDLK_F8, SDLK_F9, SDLK_F10, SDLK_F11, SDLK_F12,
			SDLK_PRINTSCREEN, SDLK_SCROLLLOCK, SDLK_PAUSE, SDLK_INSERT, SDLK_HOME,
			SDLK_PAGEUP, SDLK_DELETE, SDLK_END, SDLK_PAGEDOWN, SDLK_RIGHT, SDLK_LEFT,
			SDLK_DOWN, SDLK_UP, SDLK_NUMLOCKCLEAR, SDLK_KP_DIVIDE, SDLK_KP_MULTIPLY,
			SDLK_KP_MINUS, SDLK_KP_PLUS, SDLK_KP_ENTER, SDLK_KP_1, SDLK_KP_2,
			SDLK_KP_3, SDLK_KP_4, SDLK_KP_5, SDLK_KP_6, SDLK_KP_7, SDLK_KP_8,
			SDLK_KP_9, SDLK_KP_0, SDLK_KP_PERIOD
		};
		constexpr uint32_t Keys2Start = 224;
		constexpr SDLKey Keys2[] {
			SDLK_LCTRL, SDLK_LSHIFT, SDLK_LALT, SDLK_LGUI, SDLK_RCTRL, SDLK_RSHIFT,
			SDLK_RALT, SDLK_RGUI, SDLK_MODE
		};
		const uint32_t scancode = key & ~(1 << 30);
		if (scancode >= Keys1Start) {
			if (scancode < Keys1Start + sizeof(Keys1) / sizeof(Keys1[0]))
				return Keys1[scancode - Keys1Start];
			if (scancode >= Keys2Start && scancode < Keys2Start + sizeof(Keys2) / sizeof(Keys2[0]))
				return Keys2[scancode - Keys2Start];
		}
		LogWarn("Demo: unknown key {:d}", key);
		return SDLK_UNKNOWN;
	}
	if (key <= 122) {
		return static_cast<SDLKey>(key);
	}
	LogWarn("Demo: unknown key {:d}", key);
	return SDLK_UNKNOWN;
}

uint8_t Sdl2ToSdl1MouseButton(uint8_t button)
{
	switch (button) {
	case 4:
		return SDL_BUTTON_X1;
	case 5:
		return SDL_BUTTON_X2;
	default:
		return button;
	}
}

bool CreateSdlEvent(const DemoMsg &dmsg, SDL_Event &event, uint16_t &modState)
{
	switch (dmsg.eventType) {
	case 0x400:
		event.type = SDL_MOUSEMOTION;
		event.motion.which = 0;
		event.motion.x = dmsg.motion.x;
		event.motion.y = dmsg.motion.y;
		return true;
	case 0x401:
	case 0x402:
		event.type = dmsg.eventType == 0x401 ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
		event.button.which = 0;
		event.button.button = Sdl2ToSdl1MouseButton(dmsg.button.button);
		event.button.state = dmsg.eventType == 0x401 ? SDL_PRESSED : SDL_RELEASED;
		event.button.x = dmsg.button.x;
		event.button.y = dmsg.button.y;
		modState = dmsg.button.mod;
		return true;
	case 0x403: // SDL_MOUSEWHEEL
		if (dmsg.wheel.y == 0) {
			LogWarn("Demo: unsupported event (mouse wheel y == 0)");
			return false;
		}
		event.type = SDL_MOUSEBUTTONDOWN;
		event.button.which = 0;
		event.button.button = dmsg.wheel.y > 0 ? SDL_BUTTON_WHEELUP : SDL_BUTTON_WHEELDOWN;
		modState = dmsg.wheel.mod;
		return true;
	case 0x300:
	case 0x301:
		event.type = dmsg.eventType == 0x300 ? SDL_KEYDOWN : SDL_KEYUP;
		event.key.which = 0;
		event.key.state = dmsg.eventType == 0x300 ? SDL_PRESSED : SDL_RELEASED;
		event.key.keysym.sym = Sdl2ToSdl1Key(dmsg.key.sym);
		event.key.keysym.mod = static_cast<SDL_Keymod>(dmsg.key.mod);
		return true;
	default:
		if (dmsg.eventType >= 0x8000) {
			event.type = CustomEventToSdlEvent(static_cast<interface_mode>(dmsg.eventType - 0x8000));
			return true;
		}
		event.type = static_cast<SDL_EventType>(0);
		LogWarn("Demo: unsupported event (type={:x})", dmsg.eventType);
		return false;
	}
}
#endif

void LogDemoMessage(const DemoMsg &msg)
{
#ifdef LOG_DEMOMODE_MESSAGES
	const uint8_t progressToNextGameTick = msg.progressToNextGameTick;
	switch (msg.type) {
	case DemoMsgType::Message: {
		const uint32_t eventType = msg.eventType;
		switch (eventType) {
		case 0x400: // SDL_MOUSEMOTION
#ifdef LOG_DEMOMODE_MESSAGES_MOUSEMOTION
			Log("🖱️  Message {:>3} MOUSEMOTION {} {}", progressToNextGameTick,
			    msg.motion.x, msg.motion.y);
#endif
			break;
		case 0x401: // SDL_MOUSEBUTTONDOWN
		case 0x402: // SDL_MOUSEBUTTONUP
			Log("🖱️  Message {:>3} {} {} {} {} 0x{:x}", progressToNextGameTick,
			    eventType == 0x401 ? "MOUSEBUTTONDOWN" : "MOUSEBUTTONUP",
			    msg.button.button, msg.button.x, msg.button.y, msg.button.mod);
			break;
		case 0x403: // SDL_MOUSEWHEEL
			Log("🖱️  Message {:>3} MOUSEWHEEL {} {} 0x{:x}", progressToNextGameTick,
			    msg.wheel.x, msg.wheel.y, msg.wheel.mod);
			break;
		case 0x300: // SDL_KEYDOWN
		case 0x301: // SDL_KEYUP
			Log("🔤 Message {:>3} {} 0x{:x} 0x{:x}", progressToNextGameTick,
			    eventType == 0x300 ? "KEYDOWN" : "KEYUP",
			    msg.key.sym, msg.key.mod);
			break;
		case 0x100: // SDL_QUIT
			Log("❎  Message {:>3} QUIT", progressToNextGameTick);
			break;
		default:
			Log("📨  Message {:>3} USEREVENT 0x{:x}", progressToNextGameTick, eventType);
			break;
		}
	} break;
	case DemoMsgType::GameTick:
#ifdef LOG_DEMOMODE_MESSAGES_GAMETICK
		Log("⏲️  GameTick {:>3}", progressToNextGameTick);
#endif
		break;
	case DemoMsgType::Rendering:
#ifdef LOG_DEMOMODE_MESSAGES_RENDERING
		Log("🖼️  Rendering {:>3}", progressToNextGameTick);
#endif
		break;
	default:
		LogError("INVALID DEMO MODE MESSAGE {} {:>3}", static_cast<uint32_t>(msg.type), progressToNextGameTick);
		break;
	}
#endif // LOG_DEMOMODE_MESSAGES
}

LoadingStatus LoadDemoMessages(int i)
{
	const std::string path = StrCat(paths::PrefPath(), "demo_", i, ".dmo");
	FILE *demofile = OpenFile(path.c_str(), "rb");
	if (demofile == nullptr) {
		return LoadingStatus::FileNotFound;
	}

	const uint8_t version = ReadByte(demofile);
	if (version != 0 && version != 1) {
		return LoadingStatus::UnsupportedVersion;
	}

	gSaveNumber = ReadLE32(demofile);
	ReadSettings(demofile, version);

	while (true) {
		const uint32_t typeNum = ReadLE32(demofile);
		if (std::feof(demofile))
			break;
		const auto type = static_cast<DemoMsgType>(typeNum);

		const uint8_t progressToNextGameTick = ReadByte(demofile);

		switch (type) {
		case DemoMsgType::Message: {
			const uint32_t eventType = ReadLE32(demofile);
			DemoMsg msg { type, progressToNextGameTick, eventType, {} };
			switch (eventType) {
			case 0x400: // SDL_MOUSEMOTION
				msg.motion.x = ReadLE16(demofile);
				msg.motion.y = ReadLE16(demofile);
				break;
			case 0x401: // SDL_MOUSEBUTTONDOWN
			case 0x402: // SDL_MOUSEBUTTONUP
				msg.button.button = ReadByte(demofile);
				msg.button.x = ReadLE16(demofile);
				msg.button.y = ReadLE16(demofile);
				msg.button.mod = ReadLE16(demofile);
				break;
			case 0x403: // SDL_MOUSEWHEEL
				msg.wheel.x = ReadLE32<int32_t>(demofile);
				msg.wheel.y = ReadLE32<int32_t>(demofile);
				msg.wheel.mod = ReadLE16(demofile);
				break;
			case 0x300: // SDL_KEYDOWN
			case 0x301: // SDL_KEYUP
				msg.key.sym = static_cast<SDL_Keycode>(ReadLE32(demofile));
				msg.key.mod = static_cast<SDL_Keymod>(ReadLE16(demofile));
				break;
			case 0x100: // SDL_QUIT
				break;
			default:
				if (eventType < 0x8000) { // SDL_USEREVENT
					app_fatal(StrCat("Unknown event ", eventType));
				}
				break;
			}
			Demo_Message_Queue.push_back(msg);
			break;
		}
		default:
			Demo_Message_Queue.push_back(DemoMsg { type, progressToNextGameTick, 0, {} });
			break;
		}
	}

	std::fclose(demofile);

	DemoModeLastTick = SDL_GetTicks();

	return LoadingStatus::Success;
}

void RecordEventHeader(const SDL_Event &event)
{
	WriteLE32(DemoRecording, static_cast<uint32_t>(DemoMsgType::Message));
	WriteByte(DemoRecording, ProgressToNextGameTick);
	WriteLE32(DemoRecording, event.type);
}

} // namespace

namespace demo {

void InitPlayBack(int demoNumber, bool timedemo)
{
	DemoNumber = demoNumber;
	Timedemo = timedemo;
	ControlMode = ControlTypes::KeyboardAndMouse;

	LoadingStatus status = LoadDemoMessages(demoNumber);
	switch (status) {
	case LoadingStatus::Success:
		return;
	case LoadingStatus::FileNotFound:
		printInConsole("Demo file not found");
		break;
	case LoadingStatus::UnsupportedVersion:
		printInConsole("Unsupported Demo version");
		break;
	}
	printNewlineInConsole();
	diablo_quit(1);
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
	forceResolution = Size(DemoGraphicsWidth, DemoGraphicsHeight);

	sgOptions.Gameplay.runInTown.SetValue(DemoSettings.runInTown);
	sgOptions.Gameplay.theoQuest.SetValue(DemoSettings.theoQuest);
	sgOptions.Gameplay.cowQuest.SetValue(DemoSettings.cowQuest);
	sgOptions.Gameplay.autoGoldPickup.SetValue(DemoSettings.autoGoldPickup);
	sgOptions.Gameplay.autoElixirPickup.SetValue(DemoSettings.autoElixirPickup);
	sgOptions.Gameplay.autoOilPickup.SetValue(DemoSettings.autoOilPickup);
	sgOptions.Gameplay.autoPickupInTown.SetValue(DemoSettings.autoPickupInTown);
	sgOptions.Gameplay.adriaRefillsMana.SetValue(DemoSettings.adriaRefillsMana);
	sgOptions.Gameplay.autoEquipWeapons.SetValue(DemoSettings.autoEquipWeapons);
	sgOptions.Gameplay.autoEquipArmor.SetValue(DemoSettings.autoEquipArmor);
	sgOptions.Gameplay.autoEquipHelms.SetValue(DemoSettings.autoEquipHelms);
	sgOptions.Gameplay.autoEquipShields.SetValue(DemoSettings.autoEquipShields);
	sgOptions.Gameplay.autoEquipJewelry.SetValue(DemoSettings.autoEquipJewelry);
	sgOptions.Gameplay.randomizeQuests.SetValue(DemoSettings.randomizeQuests);
	sgOptions.Gameplay.showItemLabels.SetValue(DemoSettings.showItemLabels);
	sgOptions.Gameplay.autoRefillBelt.SetValue(DemoSettings.autoRefillBelt);
	sgOptions.Gameplay.disableCripplingShrines.SetValue(DemoSettings.disableCripplingShrines);
	sgOptions.Gameplay.numHealPotionPickup.SetValue(DemoSettings.numHealPotionPickup);
	sgOptions.Gameplay.numFullHealPotionPickup.SetValue(DemoSettings.numFullHealPotionPickup);
	sgOptions.Gameplay.numManaPotionPickup.SetValue(DemoSettings.numManaPotionPickup);
	sgOptions.Gameplay.numFullManaPotionPickup.SetValue(DemoSettings.numFullManaPotionPickup);
	sgOptions.Gameplay.numRejuPotionPickup.SetValue(DemoSettings.numRejuPotionPickup);
	sgOptions.Gameplay.numFullRejuPotionPickup.SetValue(DemoSettings.numFullRejuPotionPickup);
}

bool IsRunning()
{
	return DemoNumber != -1;
}

bool IsRecording()
{
	return RecordNumber != -1;
}

bool GetRunGameLoop(bool &drawGame, bool &processInput)
{
	if (Demo_Message_Queue.empty())
		app_fatal("Demo queue empty");
	const DemoMsg dmsg = Demo_Message_Queue.front();
	LogDemoMessage(dmsg);
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
			int32_t fraction = ticksElapsed * AnimationInfo::baseValueFraction / gnTickDelay;
			fraction = clamp<int32_t>(fraction, 0, AnimationInfo::baseValueFraction);
			uint8_t progressToNextGameTick = static_cast<uint8_t>(fraction);
			if (dmsg.type == DemoMsgType::GameTick || dmsg.progressToNextGameTick > progressToNextGameTick) {
				// we are ahead of the replay => add a additional rendering for smoothness
				if (gbRunGame && PauseMode == 0 && (gbIsMultiplayer || !gmenu_is_active()) && gbProcessPlayers) // if game is not running or paused there is no next gametick in the near future
					ProgressToNextGameTick = progressToNextGameTick;
				processInput = false;
				drawGame = true;
				return false;
			}
		}
	}
	ProgressToNextGameTick = dmsg.progressToNextGameTick;
	Demo_Message_Queue.pop_front();
	if (dmsg.type == DemoMsgType::GameTick)
		LogicTick++;
	return dmsg.type == DemoMsgType::GameTick;
}

bool FetchMessage(SDL_Event *event, uint16_t *modState)
{
	if (CurrentEventHandler == DisableInputEventHandler)
		return false;

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
		LogDemoMessage(dmsg);
		if (dmsg.type == DemoMsgType::Message) {
			const bool hasEvent = CreateSdlEvent(dmsg, *event, *modState);
			ProgressToNextGameTick = dmsg.progressToNextGameTick;
			Demo_Message_Queue.pop_front();
			return hasEvent;
		}
	}

	return false;
}

void RecordGameLoopResult(bool runGameLoop)
{
	WriteLE32(DemoRecording, static_cast<uint32_t>(runGameLoop ? DemoMsgType::GameTick : DemoMsgType::Rendering));
	WriteByte(DemoRecording, ProgressToNextGameTick);
}

void RecordMessage(const SDL_Event &event, uint16_t modState)
{
	if (!gbRunGame || DemoRecording == nullptr)
		return;
	if (CurrentEventHandler == DisableInputEventHandler)
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
		const std::string path = StrCat(paths::PrefPath(), "demo_", RecordNumber, ".dmo");
		DemoRecording = OpenFile(path.c_str(), "wb");
		if (DemoRecording == nullptr) {
			RecordNumber = -1;
			LogError("Failed to open {} for writing", path);
			return;
		}
		constexpr uint8_t Version = 1;
		WriteByte(DemoRecording, Version);
		WriteLE32(DemoRecording, gSaveNumber);
		WriteSettings(DemoRecording);
	}

	if (IsRunning()) {
		StartTime = SDL_GetTicks();
		LogicTick = 0;
	}
}

void NotifyGameLoopEnd()
{
	if (IsRecording()) {
		std::fclose(DemoRecording);
		DemoRecording = nullptr;
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
			SDL_Log("Timedemo: No final comparison cause reference is not present.");
			break;
		case HeroCompareResult::Same:
			SDL_Log("Timedemo: Same outcome as initial run. :)");
			break;
		case HeroCompareResult::Difference:
			Log("Timedemo: Different outcome than initial run. ;(\n{}", compareResult.message);
			break;
		}
	}
}

} // namespace demo

} // namespace devilution
