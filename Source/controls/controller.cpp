#include "controls/controller.h"

#include <cmath>

#include "controls/devices/game_controller.h"
#include "controls/devices/joystick.h"
#include "controls/devices/kbcontroller.h"

namespace devilution {

ControllerButtonEvent ToControllerButtonEvent(const SDL_Event &event)
{
	ControllerButtonEvent result { ControllerButton_NONE, false };
	switch (event.type) {
	case SDL_CONTROLLERBUTTONUP:
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
	GameController *const controller = GameController::Get(event);
	if (controller != nullptr) {
		result.button = controller->ToControllerButton(event);
		if (result.button != ControllerButton_NONE)
			return result;
	}

	const Joystick *joystick = Joystick::Get(event);
	if (joystick != nullptr)
		result.button = devilution::Joystick::ToControllerButton(event);

	return result;
}

bool IsControllerButtonPressed(ControllerButton button)
{
	if (GameController::IsPressedOnAnyController(button))
		return true;
#if HAS_KBCTRL == 1
	if (IsKbCtrlButtonPressed(button))
		return true;
#endif
	return Joystick::IsPressedOnAnyJoystick(button);
}

bool HandleControllerAddedOrRemovedEvent(const SDL_Event &event)
{
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
}

} // namespace devilution
