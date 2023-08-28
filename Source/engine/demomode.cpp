#include "engine/demomode.h"

#include <cstdint>
#include <cstdio>
#include <deque>
#include <limits>
#include <optional>

#include <fmt/format.h>

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

struct MouseMotionEventData {
	uint16_t x;
	uint16_t y;
};

struct MouseButtonEventData {
	uint16_t x;
	uint16_t y;
	uint16_t mod;
	uint8_t button;
};

struct MouseWheelEventData {
	int16_t x;
	int16_t y;
	uint16_t mod;
};

struct KeyEventData {
	uint32_t sym;
	uint16_t mod;
};

union DemoMsgEventData {
	MouseMotionEventData motion;
	MouseButtonEventData button;
	MouseWheelEventData wheel;
	KeyEventData key;
};

struct DemoMsg {
	enum EventType : uint8_t {
		GameTick = 0,
		Rendering = 1,

		// Inputs:
		MinEvent = 8,

		QuitEvent = 8,
		MouseMotionEvent = 9,
		MouseButtonDownEvent = 10,
		MouseButtonUpEvent = 11,
		MouseWheelEvent = 12,
		KeyDownEvent = 13,
		KeyUpEvent = 14,

		MinCustomEvent = 64,
	};

	EventType type;
	uint8_t progressToNextGameTick;

	[[nodiscard]] bool isEvent() const
	{
		return type >= MinEvent;
	}
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
uint32_t DemoModeLastTick = 0;

int LogicTick = 0;
uint32_t StartTime = 0;

uint16_t DemoGraphicsWidth = 640;
uint16_t DemoGraphicsHeight = 480;

std::deque<DemoMsg> DemoMessageQueue;
std::deque<MouseMotionEventData> MouseMotionEventDataQueue;
std::deque<MouseButtonEventData> MouseButtonEventDataQueue;
std::deque<MouseWheelEventData> MouseWheelEventDataQueue;
std::deque<KeyEventData> KeyEventDataQueue;

struct DemoMessageAndData {
	DemoMsg message;
	DemoMsgEventData data;
};
DemoMessageAndData PopDemoMessage()
{
	DemoMessageAndData result;
	result.message = DemoMessageQueue.front();
	DemoMessageQueue.pop_front();
	switch (result.message.type) {
	case DemoMsg::MouseMotionEvent:
		result.data.motion = MouseMotionEventDataQueue.front();
		MouseMotionEventDataQueue.pop_front();
		break;
	case DemoMsg::MouseButtonDownEvent:
	case DemoMsg::MouseButtonUpEvent:
		result.data.button = MouseButtonEventDataQueue.front();
		MouseButtonEventDataQueue.pop_front();
		break;
	case DemoMsg::MouseWheelEvent:
		result.data.wheel = MouseWheelEventDataQueue.front();
		MouseWheelEventDataQueue.pop_front();
		break;
	case DemoMsg::KeyDownEvent:
	case DemoMsg::KeyUpEvent:
		result.data.key = KeyEventDataQueue.front();
		KeyEventDataQueue.pop_front();
		break;
	default:
		break;
	}
	return result;
}

void ReadSettings(FILE *in, uint8_t version) // NOLINT(readability-identifier-length)
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
	WriteByte(out, static_cast<uint8_t>(*sgOptions.Gameplay.runInTown));
	WriteByte(out, static_cast<uint8_t>(*sgOptions.Gameplay.theoQuest));
	WriteByte(out, static_cast<uint8_t>(*sgOptions.Gameplay.cowQuest));
	WriteByte(out, static_cast<uint8_t>(*sgOptions.Gameplay.autoGoldPickup));
	WriteByte(out, static_cast<uint8_t>(*sgOptions.Gameplay.autoElixirPickup));
	WriteByte(out, static_cast<uint8_t>(*sgOptions.Gameplay.autoOilPickup));
	WriteByte(out, static_cast<uint8_t>(*sgOptions.Gameplay.autoPickupInTown));
	WriteByte(out, static_cast<uint8_t>(*sgOptions.Gameplay.adriaRefillsMana));
	WriteByte(out, static_cast<uint8_t>(*sgOptions.Gameplay.autoEquipWeapons));
	WriteByte(out, static_cast<uint8_t>(*sgOptions.Gameplay.autoEquipArmor));
	WriteByte(out, static_cast<uint8_t>(*sgOptions.Gameplay.autoEquipHelms));
	WriteByte(out, static_cast<uint8_t>(*sgOptions.Gameplay.autoEquipShields));
	WriteByte(out, static_cast<uint8_t>(*sgOptions.Gameplay.autoEquipJewelry));
	WriteByte(out, static_cast<uint8_t>(*sgOptions.Gameplay.randomizeQuests));
	WriteByte(out, static_cast<uint8_t>(*sgOptions.Gameplay.showItemLabels));
	WriteByte(out, static_cast<uint8_t>(*sgOptions.Gameplay.autoRefillBelt));
	WriteByte(out, static_cast<uint8_t>(*sgOptions.Gameplay.disableCripplingShrines));
	WriteByte(out, *sgOptions.Gameplay.numHealPotionPickup);
	WriteByte(out, *sgOptions.Gameplay.numFullHealPotionPickup);
	WriteByte(out, *sgOptions.Gameplay.numManaPotionPickup);
	WriteByte(out, *sgOptions.Gameplay.numFullManaPotionPickup);
	WriteByte(out, *sgOptions.Gameplay.numRejuPotionPickup);
	WriteByte(out, *sgOptions.Gameplay.numFullRejuPotionPickup);
}

#if SDL_VERSION_ATLEAST(2, 0, 0)
bool CreateSdlEvent(const DemoMsg &dmsg, const DemoMsgEventData &data, SDL_Event &event, uint16_t &modState)
{
	const uint8_t type = dmsg.type;
	switch (type) {
	case DemoMsg::MouseMotionEvent:
		event.type = SDL_MOUSEMOTION;
		event.motion.which = 0;
		event.motion.x = data.motion.x;
		event.motion.y = data.motion.y;
		return true;
	case DemoMsg::MouseButtonDownEvent:
	case DemoMsg::MouseButtonUpEvent:
		event.type = type == DemoMsg::MouseButtonDownEvent ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
		event.button.which = 0;
		event.button.button = data.button.button;
		event.button.state = type == DemoMsg::MouseButtonDownEvent ? SDL_PRESSED : SDL_RELEASED;
		event.button.x = data.button.x;
		event.button.y = data.button.y;
		modState = data.button.mod;
		return true;
	case DemoMsg::MouseWheelEvent:
		event.type = SDL_MOUSEWHEEL;
		event.wheel.which = 0;
		event.wheel.x = data.wheel.x;
		event.wheel.y = data.wheel.y;
		modState = data.wheel.mod;
		return true;
	case DemoMsg::KeyDownEvent:
	case DemoMsg::KeyUpEvent:
		event.type = type == DemoMsg::KeyDownEvent ? SDL_KEYDOWN : SDL_KEYUP;
		event.key.state = type == DemoMsg::KeyDownEvent ? SDL_PRESSED : SDL_RELEASED;
		event.key.keysym.sym = data.key.sym;
		event.key.keysym.mod = data.key.mod;
		return true;
	default:
		if (type >= DemoMsg::MinCustomEvent) {
			event.type = CustomEventToSdlEvent(static_cast<interface_mode>(type - DemoMsg::MinCustomEvent));
			return true;
		}
		event.type = static_cast<SDL_EventType>(0);
		LogWarn("Unsupported demo event (type={})", type);
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

bool CreateSdlEvent(const DemoMsg &dmsg, const DemoMsgEventData &data, SDL_Event &event, uint16_t &modState)
{
	const uint8_t type = dmsg.type;
	switch (type) {
	case DemoMsg::MouseMotionEvent:
		event.type = SDL_MOUSEMOTION;
		event.motion.which = 0;
		event.motion.x = data.motion.x;
		event.motion.y = data.motion.y;
		return true;
	case DemoMsg::MouseButtonDownEvent:
	case DemoMsg::MouseButtonUpEvent:
		event.type = type == DemoMsg::MouseButtonDownEvent ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
		event.button.which = 0;
		event.button.button = Sdl2ToSdl1MouseButton(data.button.button);
		event.button.state = type == DemoMsg::MouseButtonDownEvent ? SDL_PRESSED : SDL_RELEASED;
		event.button.x = data.button.x;
		event.button.y = data.button.y;
		modState = data.button.mod;
		return true;
	case DemoMsg::MouseWheelEvent:
		if (data.wheel.y == 0) {
			LogWarn("Demo: unsupported event (mouse wheel y == 0)");
			return false;
		}
		event.type = SDL_MOUSEBUTTONDOWN;
		event.button.which = 0;
		event.button.button = data.wheel.y > 0 ? SDL_BUTTON_WHEELUP : SDL_BUTTON_WHEELDOWN;
		modState = data.wheel.mod;
		return true;
	case DemoMsg::KeyDownEvent:
	case DemoMsg::KeyUpEvent:
		event.type = type == DemoMsg::KeyDownEvent ? SDL_KEYDOWN : SDL_KEYUP;
		event.key.which = 0;
		event.key.state = type == DemoMsg::KeyDownEvent ? SDL_PRESSED : SDL_RELEASED;
		event.key.keysym.sym = Sdl2ToSdl1Key(data.key.sym);
		event.key.keysym.mod = static_cast<SDL_Keymod>(data.key.mod);
		return true;
	default:
		if (type >= DemoMsg::MinCustomEvent) {
			event.type = CustomEventToSdlEvent(static_cast<interface_mode>(type - DemoMsg::MinCustomEvent));
			return true;
		}
		event.type = static_cast<SDL_EventType>(0);
		LogWarn("Demo: unsupported event (type={:x})", type);
		return false;
	}
}
#endif

uint8_t MapPreV2DemoMsgEventType(uint16_t type)
{
	switch (type) {
	case 0x100:
		return DemoMsg::QuitEvent;
	case 0x300:
		return DemoMsg::KeyDownEvent;
	case 0x301:
		return DemoMsg::KeyUpEvent;
	case 0x400:
		return DemoMsg::MouseMotionEvent;
	case 0x401:
		return DemoMsg::MouseButtonDownEvent;
	case 0x402:
		return DemoMsg::MouseButtonUpEvent;
	case 0x403:
		return DemoMsg::MouseWheelEvent;

	default:
		if (type < 0x8000) { // SDL_USEREVENT
			app_fatal(StrCat("Unknown event ", type));
		}
		return DemoMsg::MinCustomEvent + (type - 0x8000);
	}
}

void LogDemoMessage(const DemoMsg &msg, const DemoMsgEventData &data = DemoMsgEventData {})
{
#ifdef LOG_DEMOMODE_MESSAGES
	const uint8_t progressToNextGameTick = msg.progressToNextGameTick;
	switch (msg.type) {
	case DemoMsg::GameTick:
#ifdef LOG_DEMOMODE_MESSAGES_GAMETICK
		Log("⏲️  GameTick {:>3}", progressToNextGameTick);
#endif
		break;
	case DemoMsg::Rendering:
#ifdef LOG_DEMOMODE_MESSAGES_RENDERING
		Log("🖼️  Rendering {:>3}", progressToNextGameTick);
#endif
		break;
	case DemoMsg::MouseMotionEvent:
#ifdef LOG_DEMOMODE_MESSAGES_MOUSEMOTION
		Log("🖱️  Message {:>3} MOUSEMOTION {} {}", progressToNextGameTick,
		    data.motion.x, data.motion.y);
#endif
		break;
	case DemoMsg::MouseButtonDownEvent:
	case DemoMsg::MouseButtonUpEvent:
		Log("🖱️  Message {:>3} {} {} {} {} 0x{:x}", progressToNextGameTick,
		    msg.type == DemoMsg::MouseButtonDownEvent ? "MOUSEBUTTONDOWN" : "MOUSEBUTTONUP",
		    data.button.button, data.button.x, data.button.y, data.button.mod);
		break;
	case DemoMsg::MouseWheelEvent:
		Log("🖱️  Message {:>3} MOUSEWHEEL {} {} 0x{:x}", progressToNextGameTick,
		    data.wheel.x, data.wheel.y, data.wheel.mod);
		break;
	case DemoMsg::KeyDownEvent:
	case DemoMsg::KeyUpEvent:
		Log("🔤 Message {:>3} {} 0x{:x} 0x{:x}", progressToNextGameTick,
		    msg.type == DemoMsg::KeyDownEvent ? "KEYDOWN" : "KEYUP",
		    data.key.sym, data.key.mod);
		break;
	case DemoMsg::QuitEvent:
		Log("❎  Message {:>3} QUIT", progressToNextGameTick);
		break;
	default:
		Log("📨  Message {:>3} USEREVENT {}", progressToNextGameTick, static_cast<uint8_t>(msg.type));
		break;
	}
#endif // LOG_DEMOMODE_MESSAGES
}

LoadingStatus LoadDemoMessages(int demoNumber)
{
	const std::string path = StrCat(paths::PrefPath(), "demo_", demoNumber, ".dmo");
	FILE *demofile = OpenFile(path.c_str(), "rb");
	if (demofile == nullptr) {
		return LoadingStatus::FileNotFound;
	}

	const uint8_t version = ReadByte(demofile);
	if (version > 2) {
		return LoadingStatus::UnsupportedVersion;
	}

	gSaveNumber = ReadLE32(demofile);
	ReadSettings(demofile, version);

	while (true) {
		const uint8_t typeNum = version >= 2 ? ReadByte(demofile) : ReadLE32(demofile);
		if (std::feof(demofile) != 0)
			break;

		const uint8_t progressToNextGameTick = ReadByte(demofile);

		switch (typeNum) {
		case DemoMsg::GameTick:
		case DemoMsg::Rendering:
			DemoMessageQueue.push_back(DemoMsg { static_cast<DemoMsg::EventType>(typeNum), progressToNextGameTick });
			break;
		default: {
			const uint8_t eventType = version >= 2 ? typeNum : MapPreV2DemoMsgEventType(static_cast<uint16_t>(ReadLE32(demofile)));
			DemoMessageQueue.push_back(DemoMsg { static_cast<DemoMsg::EventType>(eventType), progressToNextGameTick });
			switch (eventType) {
			case DemoMsg::MouseMotionEvent: {
				MouseMotionEventData motion;
				motion.x = ReadLE16(demofile);
				motion.y = ReadLE16(demofile);
				MouseMotionEventDataQueue.push_back(motion);
			} break;
			case DemoMsg::MouseButtonDownEvent:
			case DemoMsg::MouseButtonUpEvent: {
				MouseButtonEventData button;
				button.button = ReadByte(demofile);
				button.x = ReadLE16(demofile);
				button.y = ReadLE16(demofile);
				button.mod = ReadLE16(demofile);
				MouseButtonEventDataQueue.push_back(button);
			} break;
			case DemoMsg::MouseWheelEvent: {
				MouseWheelEventData wheel;
				wheel.x = version >= 2 ? ReadLE16<int16_t>(demofile) : static_cast<int16_t>(ReadLE32<int32_t>(demofile));
				wheel.y = version >= 2 ? ReadLE16<int16_t>(demofile) : static_cast<int16_t>(ReadLE32<int32_t>(demofile));
				wheel.mod = ReadLE16(demofile);
				MouseWheelEventDataQueue.push_back(wheel);
			} break;
			case DemoMsg::KeyDownEvent:
			case DemoMsg::KeyUpEvent: {
				KeyEventData key;
				key.sym = static_cast<SDL_Keycode>(ReadLE32(demofile));
				key.mod = static_cast<SDL_Keymod>(ReadLE16(demofile));
				KeyEventDataQueue.push_back(key);
			} break;
			case DemoMsg::QuitEvent: // SDL_QUIT
				break;
			default:
				if (eventType < DemoMsg::MinCustomEvent) {
					app_fatal(StrCat("Unknown event ", eventType));
				}
				break;
			}
		} break;
		}
	}

	std::fclose(demofile);

	DemoModeLastTick = SDL_GetTicks();

	return LoadingStatus::Success;
}

void WriteDemoMsgHeader(DemoMsg::EventType type)
{
	WriteByte(DemoRecording, type);
	WriteByte(DemoRecording, ProgressToNextGameTick);
}

} // namespace

namespace demo {

void InitPlayBack(int demoNumber, bool timedemo)
{
	DemoNumber = demoNumber;
	Timedemo = timedemo;
	ControlMode = ControlTypes::KeyboardAndMouse;

	const LoadingStatus status = LoadDemoMessages(demoNumber);
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
	if (DemoMessageQueue.empty())
		app_fatal("Demo queue empty");
	const DemoMsg &dmsg = DemoMessageQueue.front();
	if (dmsg.isEvent())
		app_fatal("Unexpected event demo message in GetRunGameLoop");
	LogDemoMessage(dmsg);
	if (Timedemo) {
		// disable additonal rendering to speedup replay
		drawGame = dmsg.type == DemoMsg::GameTick && !HeadlessMode;
	} else {
		int currentTickCount = SDL_GetTicks();
		int ticksElapsed = currentTickCount - DemoModeLastTick;
		bool tickDue = ticksElapsed >= gnTickDelay;
		drawGame = false;
		if (tickDue) {
			if (dmsg.type == DemoMsg::GameTick) {
				DemoModeLastTick = currentTickCount;
			}
		} else {
			int32_t fraction = ticksElapsed * AnimationInfo::baseValueFraction / gnTickDelay;
			fraction = std::clamp<int32_t>(fraction, 0, AnimationInfo::baseValueFraction);
			uint8_t progressToNextGameTick = static_cast<uint8_t>(fraction);
			if (dmsg.type == DemoMsg::GameTick || dmsg.progressToNextGameTick > progressToNextGameTick) {
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
	const bool isGameTick = dmsg.type == DemoMsg::GameTick;
	DemoMessageQueue.pop_front();
	if (isGameTick)
		LogicTick++;
	return isGameTick;
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
			DemoMessageQueue.clear();
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

	if (!DemoMessageQueue.empty()) {
		if (DemoMessageQueue.front().isEvent()) {
			const DemoMessageAndData dmsg = PopDemoMessage();
			LogDemoMessage(dmsg.message, dmsg.data);
			const bool hasEvent = CreateSdlEvent(dmsg.message, dmsg.data, *event, *modState);
			ProgressToNextGameTick = dmsg.message.progressToNextGameTick;
			return hasEvent;
		} else {
			LogDemoMessage(DemoMessageQueue.front());
		}
	}

	return false;
}

void RecordGameLoopResult(bool runGameLoop)
{
	WriteDemoMsgHeader(runGameLoop ? DemoMsg::GameTick : DemoMsg::Rendering);

	if (runGameLoop && !IsRunning())
		LogicTick++;
}

void RecordMessage(const SDL_Event &event, uint16_t modState)
{
	if (!gbRunGame || DemoRecording == nullptr)
		return;
	if (CurrentEventHandler == DisableInputEventHandler)
		return;
	switch (event.type) {
	case SDL_MOUSEMOTION:
		WriteDemoMsgHeader(DemoMsg::MouseMotionEvent);
		WriteLE16(DemoRecording, event.motion.x);
		WriteLE16(DemoRecording, event.motion.y);
		break;
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
#ifdef USE_SDL1
		if (event.button.button == SDL_BUTTON_WHEELUP || event.button.button == SDL_BUTTON_WHEELDOWN) {
			WriteDemoMsgHeader(DemoMsg::MouseWheelEvent);
			WriteLE16(DemoRecording, 0);
			WriteLE16(DemoRecording, event.button.button == SDL_BUTTON_WHEELUP ? 1 : -1);
			WriteLE16(DemoRecording, modState);
		} else {
#endif
			WriteDemoMsgHeader(event.type == SDL_MOUSEBUTTONDOWN ? DemoMsg::MouseButtonDownEvent : DemoMsg::MouseButtonUpEvent);
			WriteByte(DemoRecording, event.button.button);
			WriteLE16(DemoRecording, event.button.x);
			WriteLE16(DemoRecording, event.button.y);
			WriteLE16(DemoRecording, modState);
#ifdef USE_SDL1
		}
#endif
		break;
#ifndef USE_SDL1
	case SDL_MOUSEWHEEL:
		WriteDemoMsgHeader(DemoMsg::MouseWheelEvent);
		if (event.wheel.x < std::numeric_limits<int16_t>::min()
		    || event.wheel.x > std::numeric_limits<int16_t>::max()
		    || event.wheel.y < std::numeric_limits<int16_t>::min()
		    || event.wheel.y > std::numeric_limits<int16_t>::max()) {
			app_fatal(fmt::format("Mouse wheel event x/y out of int16_t range. x={} y={}",
			    event.wheel.x, event.wheel.y));
		}
		WriteLE16(DemoRecording, event.wheel.x);
		WriteLE16(DemoRecording, event.wheel.y);
		WriteLE16(DemoRecording, modState);
		break;
#endif
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		WriteDemoMsgHeader(event.type == SDL_KEYDOWN ? DemoMsg::KeyDownEvent : DemoMsg::KeyUpEvent);
		WriteLE32(DemoRecording, static_cast<uint32_t>(event.key.keysym.sym));
		WriteLE16(DemoRecording, static_cast<uint16_t>(event.key.keysym.mod));
		break;
#ifndef USE_SDL1
	case SDL_WINDOWEVENT:
		if (event.window.type == SDL_WINDOWEVENT_CLOSE) {
			WriteDemoMsgHeader(DemoMsg::QuitEvent);
		}
		break;
#endif
	case SDL_QUIT:
		WriteDemoMsgHeader(DemoMsg::QuitEvent);
		break;
	default:
		if (IsCustomEvent(event.type)) {
			WriteDemoMsgHeader(static_cast<DemoMsg::EventType>(
			    DemoMsg::MinCustomEvent + static_cast<uint8_t>(GetCustomEvent(event.type))));
		}
		break;
	}
}

void NotifyGameLoopStart()
{
	LogicTick = 0;

	if (IsRunning()) {
		StartTime = SDL_GetTicks();
	}

	if (IsRecording()) {
		const std::string path = StrCat(paths::PrefPath(), "demo_", RecordNumber, ".dmo");
		DemoRecording = OpenFile(path.c_str(), "wb");
		if (DemoRecording == nullptr) {
			RecordNumber = -1;
			LogError("Failed to open {} for writing", path);
			return;
		}
		constexpr uint8_t Version = 2;
		WriteByte(DemoRecording, Version);
		WriteLE32(DemoRecording, gSaveNumber);
		WriteSettings(DemoRecording);
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
		const float seconds = (SDL_GetTicks() - StartTime) / 1000.0F;
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

uint32_t SimulateMillisecondsSinceStartup()
{
	return LogicTick * 50;
}

} // namespace demo

} // namespace devilution
