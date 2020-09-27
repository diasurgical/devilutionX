#include "controls/controller.h"

#include "controls/devices/kbcontroller.h"
#include "controls/devices/joystick.h"
#include "controls/devices/game_controller.h"

namespace dvl {

ControllerButtonEvent ToControllerButtonEvent(const SDL_Event &event)
{
	ControllerButtonEvent result{ ControllerButton_NONE, false };
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
	result.button = GameControllerToControllerButton(event);
	if (result.button != ControllerButton_NONE)
		return result;
#endif

	result.button = JoyButtonToControllerButton(event);

	return result;
}

bool IsControllerButtonPressed(ControllerButton button)
{
	bool result = false;
#ifndef USE_SDL1
	result = result || IsGameControllerButtonPressed(button);
#endif
#if HAS_KBCTRL == 1
	result = result || IsKbCtrlButtonPressed(button);
#endif
	result = result || IsJoystickButtonPressed(button);
	return result;
}

void InitController()
{
	InitJoystick();
#ifndef USE_SDL1
	InitGameController();
#endif
}

} // namespace dvl
