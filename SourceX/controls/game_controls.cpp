#include "controls/game_controls.h"

#include <cstdint>

#include "controls/controller.h"
#include "controls/controller_motion.h"
#include "controls/devices/game_controller.h"
#include "controls/devices/joystick.h"
#include "controls/menu_controls.h"
#include "controls/plrctrls.h"

namespace dvl {

namespace {

DWORD translate_controller_button_to_key(ControllerButton controller_button)
{
	switch (controller_button) {
	case ControllerButton::BUTTON_A: // Bottom button
		return questlog ? DVL_VK_SPACE : DVL_VK_ESCAPE;
	case ControllerButton::BUTTON_B: // Right button
		return sgpCurrentMenu || stextflag || questlog ? DVL_VK_RETURN : DVL_VK_SPACE;
	case ControllerButton::BUTTON_Y: // Top button
		return DVL_VK_RETURN;
	case ControllerButton::BUTTON_LEFTSTICK:
		return DVL_VK_TAB; // Map
	case ControllerButton::BUTTON_START:
		return DVL_VK_ESCAPE;
	case ControllerButton::BUTTON_BACK:
		return 'Q'; // Quest log
	case ControllerButton::BUTTON_DPAD_LEFT:
		return DVL_VK_LEFT;
	case ControllerButton::BUTTON_DPAD_RIGHT:
		return DVL_VK_RIGHT;
	case ControllerButton::BUTTON_DPAD_UP:
		return DVL_VK_UP;
	case ControllerButton::BUTTON_DPAD_DOWN:
		return DVL_VK_DOWN;
	default:
		return 0;
	}
}

} // namespace

bool GetGameAction(const SDL_Event &event, GameAction *action)
{
	const ControllerButtonEvent ctrl_event = ToControllerButtonEvent(event);
	switch (ctrl_event.button) {
	case ControllerButton::IGNORE:
		return true;
	case ControllerButton::AXIS_TRIGGERLEFT: // ZL (aka L2)
		if (!ctrl_event.up)
			*action = GameAction(GameActionType::USE_HEALTH_POTION);
		return true;
	case ControllerButton::AXIS_TRIGGERRIGHT: // ZR (aka R2)
		if (!ctrl_event.up)
			*action = GameAction(GameActionType::USE_MANA_POTION);
		return true;
	case ControllerButton::BUTTON_B: // Right button
		if (InGameMenu())
			break; // Map to keyboard key
		if (!ctrl_event.up)
			*action = GameAction(GameActionType::PRIMARY_ACTION);
		return true;
	case ControllerButton::BUTTON_Y: // Top button
		if (InGameMenu())
			break; // Map to keyboard key
		if (!ctrl_event.up)
			*action = GameAction(GameActionType::SECONDARY_ACTION);
		return true;
	case ControllerButton::BUTTON_X: // Left button
		if (InGameMenu())
			break; // Map to keyboard key
		if (!ctrl_event.up)
			*action = GameAction(GameActionType::CAST_SPELL);
		return true;
	case ControllerButton::BUTTON_A: // Bottom button
		if (InGameMenu())
			break; // Map to keyboard key
		if (!ctrl_event.up)
			*action = GameAction(GameActionType::TOGGLE_QUICK_SPELL_MENU);
		return true;
	case ControllerButton::BUTTON_LEFTSHOULDER:
		if (!stextflag && !ctrl_event.up)
			*action = GameAction(GameActionType::TOGGLE_CHARACTER_INFO);
		return true;
	case ControllerButton::BUTTON_RIGHTSHOULDER:
		if (!stextflag && !ctrl_event.up)
			*action = GameAction(GameActionType::TOGGLE_INVENTORY);
		return true;
	case ControllerButton::BUTTON_DPAD_UP:
	case ControllerButton::BUTTON_DPAD_DOWN:
	case ControllerButton::BUTTON_DPAD_LEFT:
	case ControllerButton::BUTTON_DPAD_RIGHT:
		if (InGameMenu())
			break;
		// The rest is handled in charMovement() on every game_logic() call.
		return true;
	case ControllerButton::BUTTON_RIGHTSTICK:
		*action = GameActionSendMouseClick { GameActionSendMouseClick::LEFT, ctrl_event.up };
		return true;
	default:
		break;
	}

	// By default, map to a keyboard key.
	if (ctrl_event.button != ControllerButton::NONE) {
		*action = GameActionSendKey{ translate_controller_button_to_key(ctrl_event.button),
			ctrl_event.up };
		return true;
	}

#ifndef USE_SDL1
	// Ignore unhandled joystick events if gamepad is active.
	// We receive the same events as gamepad events.
	if (CurrentGameController() != nullptr && event.type >= SDL_JOYAXISMOTION && event.type <= SDL_JOYBUTTONUP) {
		return true;
	}
	if (event.type == SDL_CONTROLLERAXISMOTION) {
		return true; // Ignore releasing the trigger buttons
	}
#endif

	return false;
}

MoveDirection GetMoveDirection()
{
	const float stickX = leftStickX;
	const float stickY = leftStickY;
	MoveDirection result{ MoveDirectionX::NONE, MoveDirectionY::NONE };

	if (stickY >= 0.5 || IsControllerButtonPressed(ControllerButton::BUTTON_DPAD_UP)) {
		result.y = MoveDirectionY::UP;
	} else if (stickY <= -0.5 || IsControllerButtonPressed(ControllerButton::BUTTON_DPAD_DOWN)) {
		result.y = MoveDirectionY::DOWN;
	}

	if (stickX <= -0.5 || IsControllerButtonPressed(ControllerButton::BUTTON_DPAD_LEFT)) {
		result.x = MoveDirectionX::LEFT;
	} else if (stickX >= 0.5 || IsControllerButtonPressed(ControllerButton::BUTTON_DPAD_RIGHT)) {
		result.x = MoveDirectionX::RIGHT;
	}

	return result;
}

} // namespace dvl
