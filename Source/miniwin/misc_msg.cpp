#include "miniwin/misc_msg.h"

#include <cstdint>
#include <deque>
#include <string>

#include <SDL.h>

#include "control.h"
#include "controls/controller.h"
#include "controls/controller_motion.h"
#include "controls/game_controls.h"
#include "controls/input.h"
#include "controls/plrctrls.h"
#include "controls/remap_keyboard.h"
#ifndef USE_SDL1
#include "controls/touch/event_handlers.h"
#endif
#include "cursor.h"
#include "engine/demomode.h"
#include "engine/rectangle.hpp"
#include "hwcursor.hpp"
#include "inv.h"
#include "menu.h"
#include "movie.h"
#include "panels/spell_list.hpp"
#include "qol/stash.h"
#include "utils/display.h"
#include "utils/log.hpp"
#include "utils/sdl_compat.h"
#include "utils/stubs.h"
#include "utils/utf8.hpp"

#ifdef __vita__
#include "platform/vita/touch.h"
#endif

#ifdef __SWITCH__
#include "platform/switch/docking.h"
#include <switch.h>
#endif

/** @file
 * *
 * Windows message handling and keyboard event conversion for SDL.
 */

namespace devilution {

static std::deque<tagMSG> message_queue;

void SetMouseButtonEvent(SDL_Event &event, uint32_t type, uint8_t button, Point position)
{
	event.type = type;
	event.button.button = button;
	if (type == SDL_MOUSEBUTTONDOWN) {
		event.button.state = SDL_PRESSED;
	} else {
		event.button.state = SDL_RELEASED;
	}
	event.button.x = position.x;
	event.button.y = position.y;
}

void SetCursorPos(Point position)
{
	if (ControlDevice != ControlTypes::KeyboardAndMouse) {
		MousePosition = position;
		return;
	}

	LogicalToOutput(&position.x, &position.y);
	if (!demo::IsRunning())
		SDL_WarpMouseInWindow(ghMainWnd, position.x, position.y);
}

// Moves the mouse to the first attribute "+" button.
void FocusOnCharInfo()
{
	Player &myPlayer = *MyPlayer;

	if (invflag || myPlayer._pStatPts <= 0)
		return;

	// Find the first incrementable stat.
	int stat = -1;
	for (auto attribute : enum_values<CharacterAttribute>()) {
		if (myPlayer.GetBaseAttributeValue(attribute) >= myPlayer.GetMaximumAttributeValue(attribute))
			continue;
		stat = static_cast<int>(attribute);
	}
	if (stat == -1)
		return;

	SetCursorPos(ChrBtnsRect[stat].Center());
}

namespace {

int32_t PositionForMouse(int16_t x, int16_t y)
{
	return (((uint16_t)(y & 0xFFFF)) << 16) | (uint16_t)(x & 0xFFFF);
}

bool FalseAvail(const char *name, int value)
{
	LogVerbose("Unhandled SDL event: {} {}", name, value);
	return true;
}

/**
 * @brief Try to clean the inventory related cursor states.
 * @return True if it is safe to close the inventory
 */
bool BlurInventory()
{
	if (!MyPlayer->HoldItem.isEmpty()) {
		if (!TryDropItem()) {
			MyPlayer->Say(HeroSpeech::WhereWouldIPutThis);
			return false;
		}
	}

	CloseInventory();
	if (pcurs > CURSOR_HAND)
		NewCursor(CURSOR_HAND);
	if (chrflag)
		FocusOnCharInfo();

	return true;
}

void ProcessGamepadEvents(GameAction &action)
{
	switch (action.type) {
	case GameActionType_NONE:
	case GameActionType_SEND_KEY:
		break;
	case GameActionType_USE_HEALTH_POTION:
		if (IsStashOpen)
			Stash.PreviousPage();
		else
			UseBeltItem(BLT_HEALING);
		break;
	case GameActionType_USE_MANA_POTION:
		if (IsStashOpen)
			Stash.NextPage();
		else
			UseBeltItem(BLT_MANA);
		break;
	case GameActionType_PRIMARY_ACTION:
		PerformPrimaryAction();
		break;
	case GameActionType_SECONDARY_ACTION:
		PerformSecondaryAction();
		break;
	case GameActionType_CAST_SPELL:
		PerformSpellAction();
		break;
	case GameActionType_TOGGLE_QUICK_SPELL_MENU:
		if (!invflag || BlurInventory()) {
			if (!spselflag)
				DoSpeedBook();
			else
				spselflag = false;
			chrflag = false;
			QuestLogIsOpen = false;
			sbookflag = false;
			CloseGoldWithdraw();
			IsStashOpen = false;
		}
		break;
	case GameActionType_TOGGLE_CHARACTER_INFO:
		chrflag = !chrflag;
		if (chrflag) {
			QuestLogIsOpen = false;
			CloseGoldWithdraw();
			IsStashOpen = false;
			spselflag = false;
			if (pcurs == CURSOR_DISARM)
				NewCursor(CURSOR_HAND);
			FocusOnCharInfo();
		}
		break;
	case GameActionType_TOGGLE_QUEST_LOG:
		if (!QuestLogIsOpen) {
			StartQuestlog();
			chrflag = false;
			CloseGoldWithdraw();
			IsStashOpen = false;
			spselflag = false;
		} else {
			QuestLogIsOpen = false;
		}
		break;
	case GameActionType_TOGGLE_INVENTORY:
		if (invflag) {
			BlurInventory();
		} else {
			sbookflag = false;
			spselflag = false;
			invflag = true;
			if (pcurs == CURSOR_DISARM)
				NewCursor(CURSOR_HAND);
			FocusOnInventory();
		}
		break;
	case GameActionType_TOGGLE_SPELL_BOOK:
		if (BlurInventory()) {
			CloseInventory();
			spselflag = false;
			sbookflag = !sbookflag;
		}
		break;
	}
}

} // namespace

bool FetchMessage_Real(tagMSG *lpMsg)
{
#ifdef __SWITCH__
	HandleDocking();
#endif

	if (!message_queue.empty()) {
		*lpMsg = message_queue.front();
		message_queue.pop_front();
		return true;
	}

	SDL_Event e;
	if (PollEvent(&e) == 0) {
		return false;
	}

	lpMsg->message = 0;
	lpMsg->lParam = 0;
	lpMsg->wParam = 0;

#ifdef __vita__
	HandleTouchEvent(&e, MousePosition);
#elif !defined(USE_SDL1)
	HandleTouchEvent(e);
#endif

	if (e.type == SDL_QUIT) {
		lpMsg->message = DVL_WM_QUIT;
		return true;
	}

	if (IsAnyOf(e.type, SDL_KEYUP, SDL_KEYDOWN) && e.key.keysym.sym == SDLK_UNKNOWN) {
		// Erroneous events generated by RG350 kernel.
		return true;
	}

#if !defined(USE_SDL1) && !defined(__vita__)
	if (!movie_playing) {
		// SDL generates mouse events from touch-based inputs to provide basic
		// touchscreeen support for apps that don't explicitly handle touch events
		if (IsAnyOf(e.type, SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP) && e.button.which == SDL_TOUCH_MOUSEID)
			return true;
		if (e.type == SDL_MOUSEMOTION && e.motion.which == SDL_TOUCH_MOUSEID)
			return true;
		if (e.type == SDL_MOUSEWHEEL && e.wheel.which == SDL_TOUCH_MOUSEID)
			return true;
	}
#endif

#ifdef USE_SDL1
	if (e.type == SDL_MOUSEMOTION) {
		OutputToLogical(&e.motion.x, &e.motion.y);
	} else if (IsAnyOf(e.type, SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP)) {
		OutputToLogical(&e.button.x, &e.button.y);
	}
#endif

	const ControllerButtonEvent ctrlEvent = ToControllerButtonEvent(e);
	bool isGamepadMotion = ProcessControllerMotion(e, ctrlEvent);

	DetectInputMethod(e, ctrlEvent);
	if (isGamepadMotion) {
		return true;
	}

	if (IsAnyOf(ctrlEvent.button, ControllerButtonPrimary, ControllerButtonSecondary, ControllerButtonTertiary) && IsAnyOf(ControllerButtonHeld, ctrlEvent.button, ControllerButton_NONE)) {
		ControllerButtonHeld = (ctrlEvent.up || IsControllerButtonPressed(ControllerButton_BUTTON_BACK)) ? ControllerButton_NONE : ctrlEvent.button;
		LastMouseButtonAction = MouseActionType::None;
	}

	GameAction action;
	if (GetGameAction(e, ctrlEvent, &action)) {
		if (movie_playing) {
			if (action.type != GameActionType_NONE) {
				lpMsg->message = DVL_WM_KEYDOWN;
				if (action.type == GameActionType_SEND_KEY)
					lpMsg->wParam = action.send_key.vk_code;
			}
		} else if (action.type == GameActionType_SEND_KEY) {
			if ((action.send_key.vk_code & KeymapperMouseButtonMask) != 0) {
				const unsigned button = action.send_key.vk_code & ~KeymapperMouseButtonMask;
				lpMsg->message = action.send_key.up
				    ? (button == SDL_BUTTON_LEFT ? DVL_WM_LBUTTONUP : DVL_WM_RBUTTONUP)
				    : (button == SDL_BUTTON_RIGHT ? DVL_WM_LBUTTONDOWN : DVL_WM_RBUTTONDOWN);
				lpMsg->wParam = 0;
				lpMsg->lParam = (static_cast<int16_t>(MousePosition.y) << 16) | static_cast<int16_t>(MousePosition.x);
			} else {
				lpMsg->message = action.send_key.up ? DVL_WM_KEYUP : DVL_WM_KEYDOWN;
				lpMsg->wParam = action.send_key.vk_code;
			}
		} else {
			ProcessGamepadEvents(action);
		}
		return true;
	}

	if (HandleControllerAddedOrRemovedEvent(e))
		return true;

	switch (e.type) {
	case SDL_QUIT:
		lpMsg->message = DVL_WM_QUIT;
		break;
	case SDL_KEYDOWN:
	case SDL_KEYUP: {
#ifdef USE_SDL1
		if (gbRunGame && (IsTalkActive() || dropGoldFlag)) {
			Uint16 unicode = e.key.keysym.unicode;
			if (unicode >= ' ') {
				std::string utf8;
				AppendUtf8(unicode, utf8);
				if (IsTalkActive())
					control_new_text(utf8);
				if (dropGoldFlag)
					GoldDropNewText(utf8);
			}
		}
#endif
		SDL_Keycode key = e.key.keysym.sym;
		remap_keyboard_key(&key);
		if (key == -1)
			return FalseAvail(e.type == SDL_KEYDOWN ? "SDL_KEYDOWN" : "SDL_KEYUP", e.key.keysym.sym);
		lpMsg->message = e.type == SDL_KEYDOWN ? DVL_WM_KEYDOWN : DVL_WM_KEYUP;
		lpMsg->wParam = static_cast<uint32_t>(key);
		lpMsg->lParam = EncodeKeyboardModState(e.key.keysym.mod);
	} break;
	case SDL_MOUSEMOTION:
		lpMsg->message = DVL_WM_MOUSEMOVE;
		lpMsg->lParam = PositionForMouse(e.motion.x, e.motion.y);
		lpMsg->wParam = EncodeMouseModState(SDL_GetModState());
		if (ControlMode == ControlTypes::KeyboardAndMouse && invflag)
			InvalidateInventorySlot();
		break;
	case SDL_MOUSEBUTTONDOWN: {
		lpMsg->lParam = PositionForMouse(e.button.x, e.button.y);
		lpMsg->wParam = EncodeMouseModState(SDL_GetModState());
		const int button = e.button.button;
		switch (button) {
		case SDL_BUTTON_LEFT:
			lpMsg->message = DVL_WM_LBUTTONDOWN;
			break;
		case SDL_BUTTON_RIGHT:
			lpMsg->message = DVL_WM_RBUTTONDOWN;
			break;
		case SDL_BUTTON_MIDDLE:
			lpMsg->message = DVL_WM_MBUTTONDOWN;
			break;
		case SDL_BUTTON_X1:
			lpMsg->message = DVL_WM_X1BUTTONDOWN;
			break;
		case SDL_BUTTON_X2:
			lpMsg->message = DVL_WM_X2BUTTONDOWN;
			break;
		}
	} break;
	case SDL_MOUSEBUTTONUP: {
		lpMsg->lParam = PositionForMouse(e.button.x, e.button.y);
		lpMsg->wParam = EncodeMouseModState(SDL_GetModState());
		const int button = e.button.button;
		switch (button) {
		case SDL_BUTTON_LEFT:
			lpMsg->message = DVL_WM_LBUTTONUP;
			break;
		case SDL_BUTTON_RIGHT:
			lpMsg->message = DVL_WM_RBUTTONUP;
			break;
		case SDL_BUTTON_MIDDLE:
			lpMsg->message = DVL_WM_MBUTTONUP;
			break;
		case SDL_BUTTON_X1:
			lpMsg->message = DVL_WM_X1BUTTONUP;
			break;
		case SDL_BUTTON_X2:
			lpMsg->message = DVL_WM_X2BUTTONUP;
			break;
		}
	} break;
#ifndef USE_SDL1
	case SDL_MOUSEWHEEL:
		lpMsg->message = DVL_WM_KEYDOWN;
		if (e.wheel.y > 0) {
			lpMsg->wParam = (SDL_GetModState() & KMOD_CTRL) != 0 ? SDLK_KP_PLUS : SDLK_UP;
		} else if (e.wheel.y < 0) {
			lpMsg->wParam = (SDL_GetModState() & KMOD_CTRL) != 0 ? SDLK_KP_MINUS : SDLK_DOWN;
		} else if (e.wheel.x > 0) {
			lpMsg->wParam = SDLK_LEFT;
		} else if (e.wheel.x < 0) {
			lpMsg->wParam = SDLK_RIGHT;
		}
		break;
#if SDL_VERSION_ATLEAST(2, 0, 4)
	case SDL_AUDIODEVICEADDED:
		return FalseAvail("SDL_AUDIODEVICEADDED", e.adevice.which);
	case SDL_AUDIODEVICEREMOVED:
		return FalseAvail("SDL_AUDIODEVICEREMOVED", e.adevice.which);
	case SDL_KEYMAPCHANGED:
		return FalseAvail("SDL_KEYMAPCHANGED", 0);
#endif
	case SDL_TEXTEDITING:
		if (gbRunGame)
			break;
		return FalseAvail("SDL_TEXTEDITING", e.edit.length);
	case SDL_TEXTINPUT:
		if (gbRunGame && IsTalkActive()) {
			control_new_text(e.text.text);
			break;
		}
		if (gbRunGame && dropGoldFlag) {
			GoldDropNewText(e.text.text);
			break;
		}
		if (gbRunGame && IsWithdrawGoldOpen) {
			GoldWithdrawNewText(e.text.text);
			break;
		}
		return FalseAvail("SDL_TEXTINPUT", e.text.windowID);
	case SDL_WINDOWEVENT:
		switch (e.window.event) {
		case SDL_WINDOWEVENT_SHOWN:
			gbActive = true;
			lpMsg->message = DVL_WM_PAINT;
			break;
		case SDL_WINDOWEVENT_HIDDEN:
			gbActive = false;
			break;
		case SDL_WINDOWEVENT_EXPOSED:
			lpMsg->message = DVL_WM_PAINT;
			break;
		case SDL_WINDOWEVENT_LEAVE:
			lpMsg->message = DVL_WM_CAPTURECHANGED;
			break;
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			ReinitializeHardwareCursor();
			break;
		case SDL_WINDOWEVENT_MOVED:
		case SDL_WINDOWEVENT_RESIZED:
		case SDL_WINDOWEVENT_MINIMIZED:
		case SDL_WINDOWEVENT_MAXIMIZED:
		case SDL_WINDOWEVENT_RESTORED:
#if SDL_VERSION_ATLEAST(2, 0, 5)
		case SDL_WINDOWEVENT_TAKE_FOCUS:
#endif
			break;
		case SDL_WINDOWEVENT_CLOSE:
			lpMsg->message = DVL_WM_QUERYENDSESSION;
			break;

		case SDL_WINDOWEVENT_FOCUS_LOST:
			diablo_focus_pause();
			break;
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			diablo_focus_unpause();
			break;

		default:
			return FalseAvail("SDL_WINDOWEVENT", e.window.event);
		}

		break;
#else
	case SDL_ACTIVEEVENT:
		if ((e.active.state & SDL_APPINPUTFOCUS) != 0) {
			if (e.active.gain == 0)
				diablo_focus_pause();
			else
				diablo_focus_unpause();
		}
		break;
#endif
	default:
		return FalseAvail("unknown", e.type);
	}
	return true;
}

bool FetchMessage(tagMSG *lpMsg)
{
	bool available = demo::IsRunning() ? demo::FetchMessage(lpMsg) : FetchMessage_Real(lpMsg);

	if (available && demo::IsRecording())
		demo::RecordMessage(lpMsg);

	return available;
}

void TranslateMessage(const tagMSG *lpMsg)
{
	if (lpMsg->message == DVL_WM_KEYDOWN) {
		const auto key = static_cast<SDL_Keycode>(lpMsg->wParam);
		const uint16_t mod = DecodeKeyboardModState(lpMsg->lParam >> 16);

		const bool isShift = (mod & KMOD_SHIFT) != 0;
		const bool isCapsLock = (mod & KMOD_CAPS) != 0;
		const bool isUpper = isShift != isCapsLock;

		char chr;
		if (key >= SDLK_a && key <= SDLK_z) {
			chr = static_cast<char>(key);
			if (isUpper)
				chr = static_cast<char>(chr - ('a' - 'A'));
		} else if (key <= 0x7F) {
			chr = static_cast<char>(key);
		} else if (key >= SDLK_KP_1 && key <= SDLK_KP_9) {
			chr = static_cast<char>(SDLK_1 + (key - SDLK_KP_1));
		} else if (key == SDLK_KP_0) {
			chr = static_cast<char>(SDLK_0);
		} else if (key == SDLK_KP_PLUS) {
			chr = static_cast<char>(SDLK_PLUS);
		} else if (key == SDLK_KP_MINUS) {
			chr = static_cast<char>(SDLK_MINUS);
		} else if (key == SDLK_KP_DIVIDE) {
			chr = static_cast<char>(SDLK_SLASH);
		} else if (key == SDLK_KP_MULTIPLY) {
			chr = static_cast<char>(SDLK_ASTERISK);
		} else if (key == SDLK_KP_COMMA) {
			chr = static_cast<char>(SDLK_COMMA);
		} else if (key == SDLK_KP_PERIOD) {
			chr = static_cast<char>(SDLK_PERIOD);
		} else if (key == SDLK_KP_ENTER) {
			chr = static_cast<char>(SDLK_RETURN);
		} else if (key == SDLK_KP_EQUALS) {
			chr = static_cast<char>(SDLK_EQUALS);
		} else {
			return;
		}

		if (isShift) {
			switch (chr) {
			case '1':
				chr = '!';
				break;
			case '2':
				chr = '@';
				break;
			case '3':
				chr = '#';
				break;
			case '4':
				chr = '$';
				break;
			case '5':
				chr = '%';
				break;
			case '6':
				chr = '^';
				break;
			case '7':
				chr = '&';
				break;
			case '8':
				chr = '*';
				break;
			case '9':
				chr = '(';
				break;
			case '0':
				chr = ')';
				break;
			}
		}

		// XXX: This does not add extended info to lParam
		PostMessage(DVL_WM_CHAR, key, 0);
	}
}

void PushMessage(const tagMSG *lpMsg)
{
	assert(CurrentEventHandler != nullptr);

	CurrentEventHandler(lpMsg->message, lpMsg->wParam, lpMsg->lParam);
}

void PostMessage(uint32_t type, uint32_t wParam, uint32_t lParam)
{
	message_queue.push_back({ type, wParam, lParam });
}

void ClearMessageQueue()
{
	message_queue.clear();
}

} // namespace devilution
