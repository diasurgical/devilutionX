#include "controls/devices/game_controller.h"

#include "controls/controller_motion.h"
#include "controls/devices/game_controller_factory.h"
#include "controls/devices/joystick.h"
#include "controls/plrctrls.h"
#include "gmenu.h"
#include "stores.h"
#include "utils/log.hpp"
#include "utils/stubs.h"

namespace devilution {

std::vector<GameController *> GameController::controllers_;
GameController *GameController::currentGameController_ = nullptr;

void GameController::UnlockTriggerState()
{
	trigger_left_state_ = ControllerButton_NONE;
	trigger_right_state_ = ControllerButton_NONE;
}

uint32_t GameController::TranslateControllerButtonToKey(ControllerButton controllerButton)
{
	switch (controllerButton) {
	case ControllerButton_CONFIRM:
		return (sgpCurrentMenu != nullptr || stextflag != STORE_NONE || QuestLogIsOpen) ? DVL_VK_RETURN : DVL_VK_SPACE;
	case ControllerButton_CANCEL:
		return QuestLogIsOpen ? DVL_VK_SPACE : DVL_VK_ESCAPE;
	case ControllerButton_BUTTON_BACK:
		return DVL_VK_TAB;
	case ControllerButton_BUTTON_START:
		return DVL_VK_ESCAPE;
	case ControllerButton_BUTTON_DPAD_LEFT:
		return DVL_VK_LEFT;
	case ControllerButton_BUTTON_DPAD_RIGHT:
		return DVL_VK_RIGHT;
	case ControllerButton_BUTTON_DPAD_UP:
		return DVL_VK_UP;
	case ControllerButton_BUTTON_DPAD_DOWN:
		return DVL_VK_DOWN;
	default:
		return 0;
	}
}

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
				trigger_left_state_ = ControllerButton_AXIS_TRIGGERLEFT;
			}
			return trigger_left_state_;
		case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
			if (event.caxis.value < 8192) { // 25% pressed
				trigger_right_is_down_ = false;
			}
			if (event.caxis.value > 16384 && !trigger_right_is_down_) { // 50% pressed
				trigger_right_is_down_ = true;
				trigger_right_state_ = ControllerButton_AXIS_TRIGGERRIGHT;
			}
			return trigger_right_state_;
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

SDL_GameControllerButton GameController::ToSdlGameControllerButton(ControllerButton button)
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
	switch (button) {
	case ControllerButton_AXIS_TRIGGERLEFT:
		return trigger_left_is_down_;
	case ControllerButton_AXIS_TRIGGERRIGHT:
		return trigger_right_is_down_;
	}

	const SDL_GameControllerButton gcButton = ToSdlGameControllerButton(button);

	if (gcButton != SDL_CONTROLLER_BUTTON_INVALID)
		return SDL_GameControllerHasButton(sdl_game_controller_, gcButton) && SDL_GameControllerGetButton(sdl_game_controller_, gcButton) != 0;

	return false;
}

bool GameController::ProcessAxisMotion(const SDL_Event &event)
{
	if (event.type != SDL_CONTROLLERAXISMOTION)
		return false;
	switch (event.caxis.axis) {
	case SDL_CONTROLLER_AXIS_LEFTX:
		leftStickXUnscaled = static_cast<float>(event.caxis.value);
		leftStickNeedsScaling = true;
		break;
	case SDL_CONTROLLER_AXIS_LEFTY:
		leftStickYUnscaled = static_cast<float>(-event.caxis.value);
		leftStickNeedsScaling = true;
		break;
	case SDL_CONTROLLER_AXIS_RIGHTX:
		rightStickXUnscaled = static_cast<float>(event.caxis.value);
		rightStickNeedsScaling = true;
		break;
	case SDL_CONTROLLER_AXIS_RIGHTY:
		rightStickYUnscaled = static_cast<float>(-event.caxis.value);
		rightStickNeedsScaling = true;
		break;
	default:
		return false;
	}
	return true;
}

bool GameController::GetGameAction(const SDL_Event &event, ControllerButtonEvent ctrlEvent, GameAction *action)
{
    if (HandleControllerButtonEvent(event, ctrlEvent, action))
        return true;

    if (!InGameMenu() && !QuestLogIsOpen && !sbookflag) {
		switch (ctrlEvent.button) {
        case ControllerButton_BUTTON_DPAD_UP:
        case ControllerButton_BUTTON_DPAD_DOWN:
        case ControllerButton_BUTTON_DPAD_LEFT:
        case ControllerButton_BUTTON_DPAD_RIGHT:
            // The rest of D-Pad actions are handled in charMovement() on every game_logic() call.
            return true;
        }
    }

	// DPad navigation is handled separately for these.
	if (gmenu_is_active() || QuestLogIsOpen || stextflag != STORE_NONE) {
		switch (ctrlEvent.button) {
		case ControllerButton_BUTTON_DPAD_UP:
		case ControllerButton_BUTTON_DPAD_DOWN:
		case ControllerButton_BUTTON_DPAD_LEFT:
		case ControllerButton_BUTTON_DPAD_RIGHT:
			return true;
		}
	}

	// By default, map to a keyboard key.
	if (ctrlEvent.button != ControllerButton_NONE) {
		*action = GameActionSendKey { TranslateControllerButtonToKey(ctrlEvent.button), ctrlEvent.up };
		return true;
	}

	// Ignore unhandled joystick events where a GameController is open for this joystick.
	// This is because SDL sends both game controller and joystick events in this case.
	const Joystick *const joystick = Joystick::Get(event);

	if (joystick)
		if (joystick->instance_id() == instance_id_)
			return true;

	if (event.type == SDL_CONTROLLERAXISMOTION)
		return true; // Ignore releasing the trigger buttons

	return false;
}

MenuAction GameController::GetAButtonMenuAction()
{
#ifdef SWAP_CONFIRM_CANCEL_BUTTONS
	return MenuAction_BACK;
#else
	return MenuAction_SELECT;
#endif
}

MenuAction GameController::GetBButtonMenuAction()
{
#ifdef SWAP_CONFIRM_CANCEL_BUTTONS
	return MenuAction_SELECT;
#else
	return MenuAction_BACK;
#endif
}

void GameController::Add(int joystickIndex)
{
	Log("Opening game controller for joystick at index {}", joystickIndex);
	GameController *controller = GameControllerFactory::Create();
	controller->sdl_game_controller_ = SDL_GameControllerOpen(joystickIndex);

	if (controller->sdl_game_controller_ == nullptr) {
		Log("{}", SDL_GetError());
		SDL_ClearError();
		delete controller;

		return;
	}

	SDL_Joystick *const sdlJoystick = SDL_GameControllerGetJoystick(controller->sdl_game_controller_);
	controller->instance_id_ = SDL_JoystickInstanceID(sdlJoystick);
	controllers_.push_back(controller);
	currentGameController_ = controller;

	const SDL_JoystickGUID guid = SDL_JoystickGetGUID(sdlJoystick);
	SDLUniquePtr<char> mapping { SDL_GameControllerMappingForGUID(guid) };
	if (mapping) {
		Log("Opened game controller with mapping:\n{}", mapping.get());
	}
}

void GameController::Remove(SDL_JoystickID instanceId)
{
	Log("Removing game controller with instance id {}", instanceId);
	for (std::size_t i = 0; i < controllers_.size(); ++i) {
		GameController *controller = controllers_[i];

		if (controller->instance_id_ != instanceId)
			continue;

		controllers_.erase(controllers_.begin() + i);

        if (currentGameController_->instance_id_ == controller->instance_id_)
            currentGameController_ = nullptr;

        delete controller;

		return;
	}
	Log("Game controller not found with instance id: {}", instanceId);
}

GameController *GameController::Get(SDL_JoystickID instanceId)
{
	for (auto *controller : controllers_) {
		if (controller->instance_id_ == instanceId) {
            currentGameController_ = controller;

			return controller;
        }
	}

	return nullptr;
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
		return nullptr;
	}
}

const std::vector<GameController *> &GameController::All()
{
	return controllers_;
}

bool GameController::IsPressedOnAnyController(ControllerButton button)
{
	for (auto *controller : controllers_)
		if (controller->IsPressed(button))
			return true;
	return false;
}

GameController *GameController::GetCurrentGameController()
{
    return currentGameController_;
}

} // namespace devilution
