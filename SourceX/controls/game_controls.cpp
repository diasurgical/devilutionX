#include "controls/game_controls.h"

#include <cstdint>

#include "controls/controller.h"
#include "controls/controller_motion.h"
#include "controls/devices/game_controller.h"
#include "controls/devices/joystick.h"
#include "controls/menu_controls.h"
#include "controls/modifier_hints.h"
#include "controls/plrctrls.h"

namespace dvl {

bool start_modifier_active = false;
bool select_modifier_active = false;

namespace {

DWORD translate_controller_button_to_key(ControllerButton controller_button)
{
	switch (controller_button) {
	case ControllerButton_BUTTON_A: // Bottom button
		return questlog ? DVL_VK_SPACE : DVL_VK_ESCAPE;
	case ControllerButton_BUTTON_B: // Right button
		return sgpCurrentMenu || stextflag || questlog ? DVL_VK_RETURN : DVL_VK_SPACE;
	case ControllerButton_BUTTON_Y: // Top button
		return DVL_VK_RETURN;
	case ControllerButton_BUTTON_LEFTSTICK:
		return DVL_VK_TAB; // Map
	case ControllerButton_BUTTON_BACK:
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

bool HandleStartAndSelect(const ControllerButtonEvent &ctrl_event, GameAction *action)
{
	const bool in_game_menu = InGameMenu();

	const bool start_is_down = IsControllerButtonPressed(ControllerButton_BUTTON_START);
	const bool select_is_down = IsControllerButtonPressed(ControllerButton_BUTTON_BACK);
	start_modifier_active = !in_game_menu && start_is_down;
	select_modifier_active = !in_game_menu && select_is_down && !start_modifier_active;

	// Tracks whether we've received both START and SELECT down events.
	//
	// Using `IsControllerButtonPressed()` for this would be incorrect.
	// If both buttons are pressed simultaneously, SDL sends 2 events for which both buttons are in the pressed state.
	// This allows us to avoid triggering START+SELECT action twice in this case.
	static bool start_down_received = false;
	static bool select_down_received = false;
	switch (ctrl_event.button) {
	case ControllerButton_BUTTON_BACK:
		select_down_received = !ctrl_event.up;
		break;
	case ControllerButton_BUTTON_START:
		start_down_received = !ctrl_event.up;
		break;
	default:
		return false;
	}

	if (start_down_received && select_down_received) {
		*action = GameActionSendKey { DVL_VK_ESCAPE, ctrl_event.up };
		return true;
	}

	if (in_game_menu && (start_is_down || select_is_down) && !ctrl_event.up) {
		// If both are down, do nothing because `both_received` will trigger soon.
		if (start_is_down && select_is_down) return true;
		*action = GameActionSendKey { DVL_VK_ESCAPE, ctrl_event.up };
		return true;
	}

	return false;
}

} // namespace

bool GetGameAction(const SDL_Event &event, ControllerButtonEvent ctrl_event, GameAction *action)
{
	const bool in_game_menu = InGameMenu();

	if (HandleStartAndSelect(ctrl_event, action))
		return true;

	if (!in_game_menu) {
		switch (ctrl_event.button) {
		case ControllerButton_BUTTON_LEFTSHOULDER:
			if (select_modifier_active) {
				if (!IsAutomapActive())
					*action = GameActionSendMouseClick{ GameActionSendMouseClick::LEFT, ctrl_event.up };
				return true;
			}
			break;
		case ControllerButton_BUTTON_RIGHTSHOULDER:
			if (select_modifier_active) {
				if (!IsAutomapActive())
					*action = GameActionSendMouseClick{ GameActionSendMouseClick::RIGHT, ctrl_event.up };
				return true;
			}
			break;
		case ControllerButton_AXIS_TRIGGERLEFT: // ZL (aka L2)
			if (!ctrl_event.up) {
				if (select_modifier_active)
					*action = GameAction(GameActionType_TOGGLE_QUEST_LOG);
				else
					*action = GameAction(GameActionType_TOGGLE_CHARACTER_INFO);
			}
			return true;
		case ControllerButton_AXIS_TRIGGERRIGHT: // ZR (aka R2)
			if (!ctrl_event.up) {
				if (select_modifier_active)
					*action = GameAction(GameActionType_TOGGLE_SPELL_BOOK);
				else
					*action = GameAction(GameActionType_TOGGLE_INVENTORY);
			}
			return true;
		case ControllerButton_BUTTON_LEFTSTICK:
			if (select_modifier_active) {
				if (!IsAutomapActive())
					*action = GameActionSendMouseClick{ GameActionSendMouseClick::LEFT, ctrl_event.up };
				return true;
			}
			break;
		case ControllerButton_IGNORE:
		case ControllerButton_BUTTON_START:
		case ControllerButton_BUTTON_BACK:
			return true;
			break;
		default:
			break;
		}
		if (start_modifier_active) {
			switch (ctrl_event.button) {
			case ControllerButton_BUTTON_DPAD_UP:
				*action = GameActionSendKey{ DVL_VK_ESCAPE, ctrl_event.up };
				return true;
			case ControllerButton_BUTTON_DPAD_RIGHT:
				if (!ctrl_event.up)
					*action = GameAction(GameActionType_TOGGLE_INVENTORY);
				return true;
			case ControllerButton_BUTTON_DPAD_DOWN:
				*action = GameActionSendKey{ DVL_VK_TAB, ctrl_event.up };
				return true;
			case ControllerButton_BUTTON_DPAD_LEFT:
				if (!ctrl_event.up)
					*action = GameAction(GameActionType_TOGGLE_CHARACTER_INFO);
				return true;
			case ControllerButton_BUTTON_Y: // Top button
				// Not mapped. Reserved for future use.
				return true;
			case ControllerButton_BUTTON_B: // Right button
				// Not mapped. TODO: map to attack in place.
				return true;
			case ControllerButton_BUTTON_A: // Bottom button
				if (!ctrl_event.up)
					*action = GameAction(GameActionType_TOGGLE_SPELL_BOOK);
				return true;
			case ControllerButton_BUTTON_X: // Left button
				if (!ctrl_event.up)
					*action = GameAction(GameActionType_TOGGLE_QUEST_LOG);
				return true;
			case ControllerButton_BUTTON_LEFTSHOULDER:
				if (!ctrl_event.up)
					*action = GameAction(GameActionType_TOGGLE_CHARACTER_INFO);
				return true;
			case ControllerButton_BUTTON_RIGHTSHOULDER:
				if (!ctrl_event.up)
					*action = GameAction(GameActionType_TOGGLE_INVENTORY);
				return true;
			default:
				return true;
			}
		}

		// Bottom button: Closes menus or opens quick spell book if nothing is open.
		if (ctrl_event.button == ControllerButton_BUTTON_A) { // Bottom button
			if (ctrl_event.up) return true;
			if (IsControllerButtonPressed(ControllerButton_BUTTON_BACK))
				*action = GameActionSendKey { DVL_VK_F7, ctrl_event.up };
			else if (invflag)
				*action = GameAction(GameActionType_TOGGLE_INVENTORY);
			else if (sbookflag)
				*action = GameAction(GameActionType_TOGGLE_SPELL_BOOK);
			else if (questlog)
				*action = GameAction(GameActionType_TOGGLE_QUEST_LOG);
			else if (chrflag)
				*action = GameAction(GameActionType_TOGGLE_CHARACTER_INFO);
			else
				*action = GameAction(GameActionType_TOGGLE_QUICK_SPELL_MENU);
			return true;
		}

		if (!questlog && !sbookflag) {
			switch (ctrl_event.button) {
			case ControllerButton_IGNORE:
				return true;
			case ControllerButton_BUTTON_B: // Right button
				if (!ctrl_event.up) {
					if (IsControllerButtonPressed(ControllerButton_BUTTON_BACK))
						*action = GameActionSendKey{ DVL_VK_F8, ctrl_event.up };
					else
						*action = GameAction(GameActionType_PRIMARY_ACTION);
				}
				return true;
			case ControllerButton_BUTTON_Y: // Top button
				if (!ctrl_event.up) {
					if (IsControllerButtonPressed(ControllerButton_BUTTON_BACK))
						*action = GameActionSendKey{ DVL_VK_F6, ctrl_event.up };
					else
						*action = GameAction(GameActionType_SECONDARY_ACTION);
				}
				return true;
			case ControllerButton_BUTTON_X: // Left button
				if (!ctrl_event.up) {
					if (IsControllerButtonPressed(ControllerButton_BUTTON_BACK))
						*action = GameActionSendKey{ DVL_VK_F5, ctrl_event.up };
					else
						*action = GameAction(GameActionType_CAST_SPELL);
				}
				return true;
			case ControllerButton_BUTTON_LEFTSHOULDER:
				if (!stextflag && !ctrl_event.up)
					*action = GameAction(GameActionType_USE_HEALTH_POTION);
				return true;
			case ControllerButton_BUTTON_RIGHTSHOULDER:
				if (!stextflag && !ctrl_event.up)
					*action = GameAction(GameActionType_USE_MANA_POTION);
				return true;
			case ControllerButton_BUTTON_DPAD_UP:
			case ControllerButton_BUTTON_DPAD_DOWN:
			case ControllerButton_BUTTON_DPAD_LEFT:
			case ControllerButton_BUTTON_DPAD_RIGHT:
				// The rest of D-Pad actions are handled in charMovement() on every game_logic() call.
				return true;
			case ControllerButton_BUTTON_RIGHTSTICK:
				if (!IsAutomapActive()) {
					if (IsControllerButtonPressed(ControllerButton_BUTTON_BACK))
						*action = GameActionSendMouseClick{ GameActionSendMouseClick::RIGHT, ctrl_event.up };
					else
						*action = GameActionSendMouseClick{ GameActionSendMouseClick::LEFT, ctrl_event.up };
				}
				return true;
			default:
				break;
			}
		}

		if (ctrl_event.button == ControllerButton_BUTTON_BACK) {
			return true; // Ignore mod button
		}
	}


	// By default, map to a keyboard key.
	if (ctrl_event.button != ControllerButton_NONE) {
		*action = GameActionSendKey{ translate_controller_button_to_key(ctrl_event.button),
			ctrl_event.up };
		return true;
	}

#ifndef USE_SDL1
	// Ignore unhandled joystick events where a GameController is open for this joystick.
	// This is because SDL sends both game controller and joystick events in this case.
	const Joystick *const joystick = Joystick::Get(event);
	if (joystick != NULL && GameController::Get(joystick->instance_id()) != NULL) {
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
	MoveDirection result{ MoveDirectionX_NONE, MoveDirectionY_NONE };

	if (stickY >= 0.5 || IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_UP)) {
		result.y = MoveDirectionY_UP;
	} else if (stickY <= -0.5 || IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_DOWN)) {
		result.y = MoveDirectionY_DOWN;
	}

	if (stickX <= -0.5 || IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_LEFT)) {
		result.x = MoveDirectionX_LEFT;
	} else if (stickX >= 0.5 || IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_RIGHT)) {
		result.x = MoveDirectionX_RIGHT;
	}

	return result;
}

} // namespace dvl
