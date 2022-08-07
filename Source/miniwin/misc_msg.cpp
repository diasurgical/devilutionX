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

bool FetchMessage_Real(SDL_Event *event, uint16_t *modState)
{
#ifdef __SWITCH__
	HandleDocking();
#endif

	SDL_Event e;
	if (PollEvent(&e) == 0) {
		return false;
	}

	event->type = static_cast<SDL_EventType>(0);
	*modState = SDL_GetModState();

#ifdef __vita__
	HandleTouchEvent(&e, MousePosition);
#elif !defined(USE_SDL1)
	HandleTouchEvent(e);
#endif

	if (e.type == SDL_QUIT || IsCustomEvent(e.type)) {
		*event = e;
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
				event->type = SDL_KEYDOWN;
				if (action.type == GameActionType_SEND_KEY)
					event->key.keysym.sym = static_cast<SDL_Keycode>(action.send_key.vk_code);
			}
		} else if (action.type == GameActionType_SEND_KEY) {
			if ((action.send_key.vk_code & KeymapperMouseButtonMask) != 0) {
				const unsigned button = action.send_key.vk_code & ~KeymapperMouseButtonMask;
				SetMouseButtonEvent(*event, action.send_key.up ? SDL_MOUSEBUTTONUP : SDL_MOUSEBUTTONDOWN, static_cast<uint8_t>(button), MousePosition);
			} else {
				event->type = action.send_key.up ? SDL_KEYUP : SDL_KEYDOWN;
				event->key.state = action.send_key.up ? SDL_PRESSED : SDL_RELEASED;
				event->key.keysym.sym = static_cast<SDL_Keycode>(action.send_key.vk_code);
			}
		} else {
			ProcessGamepadEvents(action);
		}
		return true;
	}

	if (HandleControllerAddedOrRemovedEvent(e))
		return true;

	switch (e.type) {
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
		event->type = e.type;
		event->key.state = e.key.state;
		event->key.keysym.sym = key;
		event->key.keysym.mod = e.key.keysym.mod;
	} break;
	case SDL_MOUSEMOTION:
		*event = e;
		if (ControlMode == ControlTypes::KeyboardAndMouse && invflag)
			InvalidateInventorySlot();
		break;
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		*event = e;
		break;
#ifndef USE_SDL1
	case SDL_MOUSEWHEEL:
		event->type = SDL_KEYDOWN;
		if (e.wheel.y > 0) {
			event->key.keysym.sym = (SDL_GetModState() & KMOD_CTRL) != 0 ? SDLK_KP_PLUS : SDLK_UP;
		} else if (e.wheel.y < 0) {
			event->key.keysym.sym = (SDL_GetModState() & KMOD_CTRL) != 0 ? SDLK_KP_MINUS : SDLK_DOWN;
		} else if (e.wheel.x > 0) {
			event->key.keysym.sym = SDLK_LEFT;
		} else if (e.wheel.x < 0) {
			event->key.keysym.sym = SDLK_RIGHT;
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
		*event = e;
		break;
#else
	case SDL_ACTIVEEVENT:
		*event = e;
		break;
#endif
	default:
		return FalseAvail("unknown", e.type);
	}
	return true;
}

bool FetchMessage(SDL_Event *event, uint16_t *modState)
{
	const bool available = demo::IsRunning() ? demo::FetchMessage(event, modState) : FetchMessage_Real(event, modState);

	if (available && demo::IsRecording())
		demo::RecordMessage(*event, *modState);

	return available;
}

void HandleMessage(const SDL_Event &event, uint16_t modState)
{
	assert(CurrentEventHandler != nullptr);

	CurrentEventHandler(event, modState);
}

} // namespace devilution
