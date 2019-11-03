#include "controls/devices/game_controller.h"

#ifndef USE_SDL1
#include "controls/controller_motion.h"
#include "controls/devices/joystick.h"
#include "stubs.h"

namespace dvl {

static SDL_GameController *current_game_controller = nullptr;
static bool sgbTriggerLeftDown = false;
static bool sgbTriggerRightDown = false;

ControllerButton GameControllerToControllerButton(const SDL_Event &event)
{
	switch (event.type) {
	case SDL_CONTROLLERAXISMOTION:
		switch (event.caxis.axis) {
		case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
			if (event.caxis.value < 8192) { // 25% pressed
				sgbTriggerLeftDown = false;
			}
			if (event.caxis.value > 16384 && !sgbTriggerLeftDown) { // 50% pressed
				sgbTriggerLeftDown = true;
				return ControllerButton::AXIS_TRIGGERLEFT;
			}
			return ControllerButton::NONE;
		case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
			if (event.caxis.value < 8192) { // 25% pressed
				sgbTriggerRightDown = false;
			}
			if (event.caxis.value > 16384 && !sgbTriggerRightDown) { // 50% pressed
				sgbTriggerRightDown = true;
				return ControllerButton::AXIS_TRIGGERRIGHT;
			}
			return ControllerButton::NONE;
		}
		break;
	case SDL_CONTROLLERBUTTONDOWN:
	case SDL_CONTROLLERBUTTONUP:
		switch (event.cbutton.button) {
		case SDL_CONTROLLER_BUTTON_A:
			return ControllerButton::BUTTON_A;
		case SDL_CONTROLLER_BUTTON_B:
			return ControllerButton::BUTTON_B;
		case SDL_CONTROLLER_BUTTON_X:
			return ControllerButton::BUTTON_X;
		case SDL_CONTROLLER_BUTTON_Y:
			return ControllerButton::BUTTON_Y;
		case SDL_CONTROLLER_BUTTON_LEFTSTICK:
			return ControllerButton::BUTTON_LEFTSTICK;
		case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
			return ControllerButton::BUTTON_RIGHTSTICK;
		case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
			return ControllerButton::BUTTON_LEFTSHOULDER;
		case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
			return ControllerButton::BUTTON_RIGHTSHOULDER;
		case SDL_CONTROLLER_BUTTON_START:
			return ControllerButton::BUTTON_START;
		case SDL_CONTROLLER_BUTTON_BACK:
			return ControllerButton::BUTTON_BACK;
		case SDL_CONTROLLER_BUTTON_DPAD_UP:
			return ControllerButton::BUTTON_DPAD_UP;
		case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
			return ControllerButton::BUTTON_DPAD_DOWN;
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
			return ControllerButton::BUTTON_DPAD_LEFT;
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
			return ControllerButton::BUTTON_DPAD_RIGHT;
		default:
			break;
		}
	default:
		break;
	}
	return ControllerButton::NONE;
}

namespace {

SDL_GameControllerButton ControllerButtonToGameControllerButton(ControllerButton button)
{
	if (button == ControllerButton::AXIS_TRIGGERLEFT || button == ControllerButton::AXIS_TRIGGERRIGHT)
		UNIMPLEMENTED();
	switch (button) {
	case ControllerButton::BUTTON_A:
		return SDL_CONTROLLER_BUTTON_A;
	case ControllerButton::BUTTON_B:
		return SDL_CONTROLLER_BUTTON_B;
	case ControllerButton::BUTTON_X:
		return SDL_CONTROLLER_BUTTON_X;
	case ControllerButton::BUTTON_Y:
		return SDL_CONTROLLER_BUTTON_Y;
	case ControllerButton::BUTTON_BACK:
		return SDL_CONTROLLER_BUTTON_BACK;
	case ControllerButton::BUTTON_START:
		return SDL_CONTROLLER_BUTTON_START;
	case ControllerButton::BUTTON_LEFTSTICK:
		return SDL_CONTROLLER_BUTTON_LEFTSTICK;
	case ControllerButton::BUTTON_RIGHTSTICK:
		return SDL_CONTROLLER_BUTTON_RIGHTSTICK;
	case ControllerButton::BUTTON_LEFTSHOULDER:
		return SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
	case ControllerButton::BUTTON_RIGHTSHOULDER:
		return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
	case ControllerButton::BUTTON_DPAD_UP:
		return SDL_CONTROLLER_BUTTON_DPAD_UP;
	case ControllerButton::BUTTON_DPAD_DOWN:
		return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
	case ControllerButton::BUTTON_DPAD_LEFT:
		return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
	case ControllerButton::BUTTON_DPAD_RIGHT:
		return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
	default:
		return SDL_CONTROLLER_BUTTON_INVALID;
	}
}

} // namespace

bool IsGameControllerButtonPressed(ControllerButton button)
{
	if (current_game_controller == nullptr)
		return false;
	const SDL_GameControllerButton gc_button = ControllerButtonToGameControllerButton(button);
	return gc_button != SDL_CONTROLLER_BUTTON_INVALID && SDL_GameControllerGetButton(current_game_controller, gc_button);
}

bool ProcessGameControllerAxisMotion(const SDL_Event &event)
{
	if (event.type != SDL_CONTROLLERAXISMOTION)
		return false;
	switch (event.caxis.axis) {
	case SDL_CONTROLLER_AXIS_LEFTX:
		leftStickXUnscaled = event.caxis.value;
		leftStickNeedsScaling = true;
		break;
	case SDL_CONTROLLER_AXIS_LEFTY:
		leftStickYUnscaled = -event.caxis.value;
		leftStickNeedsScaling = true;
		break;
	case SDL_CONTROLLER_AXIS_RIGHTX:
		rightStickXUnscaled = event.caxis.value;
		rightStickNeedsScaling = true;
		break;
	case SDL_CONTROLLER_AXIS_RIGHTY:
		rightStickYUnscaled = -event.caxis.value;
		rightStickNeedsScaling = true;
		break;
	default:
		return false;
	}
	return true;
}

SDL_GameController *CurrentGameController()
{
	return current_game_controller;
}

void InitGameController()
{
	if (CurrentJoystickIndex() == -1)
		return;
	const SDL_JoystickGUID guid = SDL_JoystickGetGUID(CurrentJoystick());
	SDL_Log("Opening gamepad %d: %s", CurrentJoystickIndex(), SDL_GameControllerMappingForGUID(guid));
	current_game_controller = SDL_GameControllerOpen(CurrentJoystickIndex());
	if (current_game_controller == nullptr)
		SDL_Log(SDL_GetError());
}

} // namespace dvl
#endif
