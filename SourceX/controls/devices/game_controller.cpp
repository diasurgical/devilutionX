#include "controls/devices/game_controller.h"

#ifndef USE_SDL1

#include <cstddef>

#include "controls/controller_motion.h"
#include "controls/devices/joystick.h"
#include "stubs.h"

// Defined in SourceX/controls/plctrls.cpp
extern "C" bool sgbControllerActive;

namespace dvl {

std::vector<GameController> *const GameController::controllers_ = new std::vector<GameController>;

ControllerButton GameController::ToControllerButton(const SDL_Event &event)
{
	switch (event.type) {
	case SDL_CONTROLLERAXISMOTION:
		switch (event.caxis.axis) {
		case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
			if (event.caxis.value < 8192) { // 25% pressed
				trigger_left_is_down_ = false;
			}
			if (event.caxis.value > 16384 && !trigger_left_is_down_) { // 50% pressed
				trigger_left_is_down_ = true;
				return ControllerButton_AXIS_TRIGGERLEFT;
			}
			return ControllerButton_NONE;
		case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
			if (event.caxis.value < 8192) { // 25% pressed
				trigger_right_is_down_ = false;
			}
			if (event.caxis.value > 16384 && !trigger_right_is_down_) { // 50% pressed
				trigger_right_is_down_ = true;
				return ControllerButton_AXIS_TRIGGERRIGHT;
			}
			return ControllerButton_NONE;
		}
		break;
	case SDL_CONTROLLERBUTTONDOWN:
	case SDL_CONTROLLERBUTTONUP:
		switch (event.cbutton.button) {
		case SDL_CONTROLLER_BUTTON_A:
			return ControllerButton_BUTTON_A;
		case SDL_CONTROLLER_BUTTON_B:
			return ControllerButton_BUTTON_B;
		case SDL_CONTROLLER_BUTTON_X:
			return ControllerButton_BUTTON_X;
		case SDL_CONTROLLER_BUTTON_Y:
			return ControllerButton_BUTTON_Y;
		case SDL_CONTROLLER_BUTTON_LEFTSTICK:
			return ControllerButton_BUTTON_LEFTSTICK;
		case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
			return ControllerButton_BUTTON_RIGHTSTICK;
		case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
			return ControllerButton_BUTTON_LEFTSHOULDER;
		case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
			return ControllerButton_BUTTON_RIGHTSHOULDER;
		case SDL_CONTROLLER_BUTTON_START:
			return ControllerButton_BUTTON_START;
		case SDL_CONTROLLER_BUTTON_BACK:
			return ControllerButton_BUTTON_BACK;
		case SDL_CONTROLLER_BUTTON_DPAD_UP:
			return ControllerButton_BUTTON_DPAD_UP;
		case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
			return ControllerButton_BUTTON_DPAD_DOWN;
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
			return ControllerButton_BUTTON_DPAD_LEFT;
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
			return ControllerButton_BUTTON_DPAD_RIGHT;
		default:
			break;
		}
	default:
		break;
	}
	return ControllerButton_NONE;
}

SDL_GameControllerButton GameController::ToSdlGameControllerButton(ControllerButton button) const
{
	if (button == ControllerButton_AXIS_TRIGGERLEFT || button == ControllerButton_AXIS_TRIGGERRIGHT)
		UNIMPLEMENTED();
	switch (button) {
	case ControllerButton_BUTTON_A:
		return SDL_CONTROLLER_BUTTON_A;
	case ControllerButton_BUTTON_B:
		return SDL_CONTROLLER_BUTTON_B;
	case ControllerButton_BUTTON_X:
		return SDL_CONTROLLER_BUTTON_X;
	case ControllerButton_BUTTON_Y:
		return SDL_CONTROLLER_BUTTON_Y;
	case ControllerButton_BUTTON_BACK:
		return SDL_CONTROLLER_BUTTON_BACK;
	case ControllerButton_BUTTON_START:
		return SDL_CONTROLLER_BUTTON_START;
	case ControllerButton_BUTTON_LEFTSTICK:
		return SDL_CONTROLLER_BUTTON_LEFTSTICK;
	case ControllerButton_BUTTON_RIGHTSTICK:
		return SDL_CONTROLLER_BUTTON_RIGHTSTICK;
	case ControllerButton_BUTTON_LEFTSHOULDER:
		return SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
	case ControllerButton_BUTTON_RIGHTSHOULDER:
		return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
	case ControllerButton_BUTTON_DPAD_UP:
		return SDL_CONTROLLER_BUTTON_DPAD_UP;
	case ControllerButton_BUTTON_DPAD_DOWN:
		return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
	case ControllerButton_BUTTON_DPAD_LEFT:
		return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
	case ControllerButton_BUTTON_DPAD_RIGHT:
		return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
	default:
		return SDL_CONTROLLER_BUTTON_INVALID;
	}
}

bool GameController::IsPressed(ControllerButton button) const
{
	const SDL_GameControllerButton gc_button = ToSdlGameControllerButton(button);
	return gc_button != SDL_CONTROLLER_BUTTON_INVALID && SDL_GameControllerGetButton(sdl_game_controller_, gc_button);
}

bool GameController::ProcessAxisMotion(const SDL_Event &event)
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

void GameController::Add(int joystick_index)
{
	SDL_Log("Opening game controller for joystick at index %d", joystick_index);
	GameController result;
	result.sdl_game_controller_ = SDL_GameControllerOpen(joystick_index);
	if (result.sdl_game_controller_ == NULL) {
		SDL_Log(SDL_GetError());
		SDL_ClearError();
		return;
	}
	SDL_Joystick *const sdl_joystick = SDL_GameControllerGetJoystick(result.sdl_game_controller_);
	result.instance_id_ = SDL_JoystickInstanceID(sdl_joystick);
	controllers_->push_back(result);

	const SDL_JoystickGUID guid = SDL_JoystickGetGUID(sdl_joystick);
	char *mapping = SDL_GameControllerMappingForGUID(guid);
	SDL_Log("Opened game controller with mapping:\n%s", mapping);
	SDL_free(mapping);
}

void GameController::Remove(SDL_JoystickID instance_id)
{
	SDL_Log("Removing game controller with instance id %d", instance_id);
	for (std::size_t i = 0; i < controllers_->size(); ++i) {
		const GameController &controller = (*controllers_)[i];
		if (controller.instance_id_ != instance_id)
			continue;
		controllers_->erase(controllers_->begin() + i);
		sgbControllerActive = !controllers_->empty();
		return;
	}
	SDL_Log("Game controller not found with instance id: %d", instance_id);
}

GameController *GameController::Get(SDL_JoystickID instance_id)
{
	for (std::size_t i = 0; i < controllers_->size(); ++i) {
		GameController &controller = (*controllers_)[i];
		if (controller.instance_id_ == instance_id)
			return &controller;
	}
	return NULL;
}

GameController *GameController::Get(const SDL_Event &event)
{
	switch (event.type) {
	case SDL_CONTROLLERAXISMOTION:
		return Get(event.caxis.which);
	case SDL_CONTROLLERBUTTONDOWN:
	case SDL_CONTROLLERBUTTONUP:
		return Get(event.jball.which);
	default:
		return NULL;
	}
}

const std::vector<GameController> &GameController::All()
{
	return *controllers_;
}

bool GameController::IsPressedOnAnyController(ControllerButton button)
{
	for (std::size_t i = 0; i < controllers_->size(); ++i)
		if ((*controllers_)[i].IsPressed(button))
			return true;
	return false;
}

} // namespace dvl
#endif
