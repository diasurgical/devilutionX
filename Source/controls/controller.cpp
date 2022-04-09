#include "controls/controller.h"

#include <cmath>

#ifndef USE_SDL1
#include "controls/devices/game_controller.h"
#endif
#include "controls/devices/joystick.h"
#include "controls/devices/kbcontroller.h"
#include "controls/game_controls.h"
#include "controls/plrctrls.h"
#include "player.h"

namespace devilution {

namespace {

bool skipTick = false;
bool heldControllerButtonEventsLocked = false;
ControllerButton actionButtons[2] = { ControllerButton_ATTACK, ControllerButton_CAST_SPELL };

bool CreateActionButtonEvent(SDL_Event *event, ControllerButton button)
{
	SDL_JoystickID which;

#ifndef USE_SDL1
	if (GameController::IsPressedOnAnyController(button, &which)) {
		event->type = SDL_CONTROLLERBUTTONDOWN;
		event->cbutton.button = GameController::ToSdlGameControllerButton(button);
		event->cbutton.state = ControllerButtonState_HELD;
		event->cbutton.which = which;

		return true;
	}
#endif
#if HAS_KBCTRL == 1
	if (IsKbCtrlButtonPressed(button)) {
		event->type = SDL_KEYDOWN;
		event->key.keysym.sym = ControllerButtonToKbCtrlKeyCode(button);
		event->key.state = ControllerButtonState_HELD;

		return true;
	}
#endif
	if (Joystick::IsPressedOnAnyJoystick(button)) {
		event->type = SDL_JOYBUTTONDOWN;
		event->jbutton.button = Joystick::ToSdlJoyButton(button);
		event->jbutton.state = ControllerButtonState_HELD;

		return true;
	}

	return false;
}

} // namespace

void UnlockControllerState(const SDL_Event &event)
{
#ifndef USE_SDL1
	GameController *const controller = GameController::Get(event);
	if (controller != nullptr) {
		controller->UnlockTriggerState();
	}
#endif
}

ControllerButtonEvent ToControllerButtonEvent(const SDL_Event &event)
{
	ControllerButtonEvent result { ControllerButton_NONE, false, ControllerButtonState_RELEASED };
	switch (event.type) {
#ifndef USE_SDL1
	case SDL_CONTROLLERBUTTONUP:
#endif
	case SDL_JOYBUTTONUP:
	case SDL_KEYUP:
		result.up = true;
		break;
	default:
		break;
	}
#if HAS_KBCTRL == 1
	result.button = KbCtrlToControllerButton(event);
	result.state = event.key.state;

	if (result.button != ControllerButton_NONE)
		return result;
#endif
#ifndef USE_SDL1
	GameController *const controller = GameController::Get(event);
	if (controller != nullptr) {
		result.button = controller->ToControllerButton(event);
		result.state = event.cbutton.state;

		if (result.button != ControllerButton_NONE)
			return result;
	}
#endif

	const Joystick *joystick = Joystick::Get(event);
	if (joystick != nullptr) {
		result.button = devilution::Joystick::ToControllerButton(event);
		result.state = ControllerButtonState_PRESSED;
		if (IsAnyOf(event.type, SDL_JOYBUTTONUP, SDL_JOYBUTTONDOWN))
			result.state = event.jbutton.state;
	}

	return result;
}

bool IsControllerButtonPressed(ControllerButton button)
{
#ifndef USE_SDL1
	if (GameController::IsPressedOnAnyController(button))
		return true;
#endif
#if HAS_KBCTRL == 1
	if (IsKbCtrlButtonPressed(button))
		return true;
#endif
	return Joystick::IsPressedOnAnyJoystick(button);
}

bool HandleControllerAddedOrRemovedEvent(const SDL_Event &event)
{
#ifndef USE_SDL1
	switch (event.type) {
	case SDL_CONTROLLERDEVICEADDED:
		GameController::Add(event.cdevice.which);
		break;
	case SDL_CONTROLLERDEVICEREMOVED:
		GameController::Remove(event.cdevice.which);
		break;
	case SDL_JOYDEVICEADDED:
		Joystick::Add(event.jdevice.which);
		break;
	case SDL_JOYDEVICEREMOVED:
		Joystick::Remove(event.jdevice.which);
		break;
	default:
		return false;
	}
	return true;
#else
	return false;
#endif
}

void UnlockHeldControllerButtonEvents(const SDL_Event &event)
{
	ControllerButtonEvent ctrlEvent = ToControllerButtonEvent(event);

	for (int i = 0; i < 2; ++i)
		if (actionButtons[i] == ctrlEvent.button) {
			heldControllerButtonEventsLocked = !CanControlHero();

			break;
		}
}

int PollActionButtonPressed(SDL_Event *event)
{
	if (heldControllerButtonEventsLocked)
		return 0;

	if (skipTick) {
		skipTick = false;

		return 0;
	}

	if (!Players[MyPlayerId].CanChangeAction())
		return 0;

	for (int i = 0; i < 2; ++i)
		if (CreateActionButtonEvent(event, actionButtons[i])) {
			skipTick = true; // 1 tick is skipped for allowing the player animation to change

			return 1;
		}

	return 0;
}

} // namespace devilution
