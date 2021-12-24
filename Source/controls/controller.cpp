#include "controls/controller.h"

#include <cmath>

#ifndef USE_SDL1
#include "controls/devices/game_controller.h"
#endif
#include "controls/devices/joystick.h"
#include "controls/devices/kbcontroller.h"

namespace devilution {

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
	ControllerButtonEvent result { ControllerButton_NONE, false };
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
	if (result.button != ControllerButton_NONE)
		return result;
#endif
#ifndef USE_SDL1
	GameController *const controller = GameController::Get(event);
	if (controller != nullptr) {
		result.button = controller->ToControllerButton(event);
		if (result.button != ControllerButton_NONE)
			return result;
	}
#endif

	const Joystick *joystick = Joystick::Get(event);
	if (joystick != nullptr)
		result.button = devilution::Joystick::ToControllerButton(event);

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

} // namespace devilution
