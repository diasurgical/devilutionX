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

bool start_modifier_active = false;
bool select_modifier_active = false;

bool GetGameAction(const SDL_Event &event, GameAction *action)
{
	const ControllerButtonEvent ctrl_event = ToControllerButtonEvent(event);
	const bool in_game_menu = InGameMenu();

	start_modifier_active = !in_game_menu && IsControllerButtonPressed(ControllerButton::BUTTON_START);
	select_modifier_active = !in_game_menu && IsControllerButtonPressed(ControllerButton::BUTTON_BACK);

	// SELECT + D-Pad simulating mouse movement.
	if (IsControllerButtonPressed(ControllerButton::BUTTON_BACK) && IsDPadButton(ctrl_event.button)) {
		return true;
	}

	// START + SELECT
	if (!ctrl_event.up
	    && ((ctrl_event.button == ControllerButton::BUTTON_BACK && IsControllerButtonPressed(ControllerButton::BUTTON_START))
	           || (ctrl_event.button == ControllerButton::BUTTON_START && IsControllerButtonPressed(ControllerButton::BUTTON_BACK)))) {
		select_modifier_active = start_modifier_active = false;
		*action = GameActionSendKey{ DVL_VK_ESCAPE, ctrl_event.up };
		return true;
	}

	if (!in_game_menu) {
		switch (ctrl_event.button) {
		case ControllerButton::BUTTON_LEFTSHOULDER:
			if (IsControllerButtonPressed(ControllerButton::BUTTON_BACK)) {
				if (!IsAutomapActive())
					*action = GameActionSendMouseClick{ GameActionSendMouseClick::LEFT, ctrl_event.up };
				return true;
			}
			break;
		case ControllerButton::BUTTON_RIGHTSHOULDER:
			if (IsControllerButtonPressed(ControllerButton::BUTTON_BACK)) {
				if (!IsAutomapActive())
					*action = GameActionSendMouseClick{ GameActionSendMouseClick::RIGHT, ctrl_event.up };
				return true;
			}
			break;
		case ControllerButton::AXIS_TRIGGERLEFT: // ZL (aka L2)
			if (!ctrl_event.up) {
				if (IsControllerButtonPressed(ControllerButton::BUTTON_BACK))
					*action = GameAction(GameActionType::TOGGLE_QUEST_LOG);
				else
					*action = GameAction(GameActionType::TOGGLE_CHARACTER_INFO);
			}
			return true;
		case ControllerButton::AXIS_TRIGGERRIGHT: // ZR (aka R2)
			if (!ctrl_event.up) {
				if (IsControllerButtonPressed(ControllerButton::BUTTON_BACK))
					*action = GameAction(GameActionType::TOGGLE_SPELL_BOOK);
				else
					*action = GameAction(GameActionType::TOGGLE_INVENTORY);
			}
			return true;
		case ControllerButton::BUTTON_LEFTSTICK:
			if (IsControllerButtonPressed(ControllerButton::BUTTON_BACK)) {
				if (!IsAutomapActive())
					*action = GameActionSendMouseClick{ GameActionSendMouseClick::LEFT, ctrl_event.up };
				return true;
			}
			break;
		case ControllerButton::BUTTON_START:
			if (IsControllerButtonPressed(ControllerButton::BUTTON_BACK)) {
				*action = GameActionSendKey{ DVL_VK_ESCAPE, ctrl_event.up };
			}
			return true;
			break;
		default:
			break;
		}
		if (IsControllerButtonPressed(ControllerButton::BUTTON_START)) {
			switch (ctrl_event.button) {
			case ControllerButton::IGNORE:
			case ControllerButton::BUTTON_START:
				return true;
			case ControllerButton::BUTTON_DPAD_UP:
				*action = GameActionSendKey{ DVL_VK_ESCAPE, ctrl_event.up };
				return true;
			case ControllerButton::BUTTON_DPAD_RIGHT:
				if (!ctrl_event.up)
					*action = GameAction(GameActionType::TOGGLE_INVENTORY);
				return true;
			case ControllerButton::BUTTON_DPAD_DOWN:
				*action = GameActionSendKey{ DVL_VK_TAB, ctrl_event.up };
				return true;
			case ControllerButton::BUTTON_DPAD_LEFT:
				if (!ctrl_event.up)
					*action = GameAction(GameActionType::TOGGLE_CHARACTER_INFO);
				return true;
			case ControllerButton::BUTTON_Y: // Top button
				// Not mapped. Reserved for future use.
				return true;
			case ControllerButton::BUTTON_B: // Right button
				// Not mapped. TODO: map to attack in place.
				return true;
			case ControllerButton::BUTTON_A: // Bottom button
				if (!ctrl_event.up)
					*action = GameAction(GameActionType::TOGGLE_SPELL_BOOK);
				return true;
			case ControllerButton::BUTTON_X: // Left button
				if (!ctrl_event.up)
					*action = GameAction(GameActionType::TOGGLE_QUEST_LOG);
				return true;
			case ControllerButton::BUTTON_LEFTSHOULDER:
				if (!ctrl_event.up)
					*action = GameAction(GameActionType::TOGGLE_CHARACTER_INFO);
				return true;
			case ControllerButton::BUTTON_RIGHTSHOULDER:
				if (!ctrl_event.up)
					*action = GameAction(GameActionType::TOGGLE_INVENTORY);
				return true;
			default:
				return true;
			}
		}

		// Bottom button: Closes menus or opens quick spell book if nothing is open.
		if (ctrl_event.button == ControllerButton::BUTTON_A) { // Bottom button
			if (ctrl_event.up) return true;
			if (IsControllerButtonPressed(ControllerButton::BUTTON_BACK))
				*action = GameActionSendKey { DVL_VK_F7, ctrl_event.up };
			else if (invflag)
				*action = GameAction(GameActionType::TOGGLE_INVENTORY);
			else if (sbookflag)
				*action = GameAction(GameActionType::TOGGLE_SPELL_BOOK);
			else if (questlog)
				*action = GameAction(GameActionType::TOGGLE_QUEST_LOG);
			else if (chrflag)
				*action = GameAction(GameActionType::TOGGLE_CHARACTER_INFO);
			else
				*action = GameAction(GameActionType::TOGGLE_QUICK_SPELL_MENU);
			return true;
		}

		if (!questlog && !sbookflag) {
			switch (ctrl_event.button) {
			case ControllerButton::IGNORE:
				return true;
			case ControllerButton::BUTTON_B: // Right button
				if (!ctrl_event.up) {
					if (IsControllerButtonPressed(ControllerButton::BUTTON_BACK))
						*action = GameActionSendKey{ DVL_VK_F8, ctrl_event.up };
					else
						*action = GameAction(GameActionType::PRIMARY_ACTION);
				}
				return true;
			case ControllerButton::BUTTON_Y: // Top button
				if (!ctrl_event.up) {
					if (IsControllerButtonPressed(ControllerButton::BUTTON_BACK))
						*action = GameActionSendKey{ DVL_VK_F6, ctrl_event.up };
					else
						*action = GameAction(GameActionType::SECONDARY_ACTION);
				}
				return true;
			case ControllerButton::BUTTON_X: // Left button
				if (!ctrl_event.up) {
					if (IsControllerButtonPressed(ControllerButton::BUTTON_BACK))
						*action = GameActionSendKey{ DVL_VK_F5, ctrl_event.up };
					else
						*action = GameAction(GameActionType::CAST_SPELL);
				}
				return true;
			case ControllerButton::BUTTON_LEFTSHOULDER:
				if (!stextflag && !ctrl_event.up)
					*action = GameAction(GameActionType::USE_HEALTH_POTION);
				return true;
			case ControllerButton::BUTTON_RIGHTSHOULDER:
				if (!stextflag && !ctrl_event.up)
					*action = GameAction(GameActionType::USE_MANA_POTION);
				return true;
			case ControllerButton::BUTTON_DPAD_UP:
			case ControllerButton::BUTTON_DPAD_DOWN:
			case ControllerButton::BUTTON_DPAD_LEFT:
			case ControllerButton::BUTTON_DPAD_RIGHT:
				// The rest of D-Pad actions are handled in charMovement() on every game_logic() call.
				return true;
			case ControllerButton::BUTTON_RIGHTSTICK:
				if (!IsAutomapActive()) {
					if (IsControllerButtonPressed(ControllerButton::BUTTON_BACK))
						*action = GameActionSendMouseClick{ GameActionSendMouseClick::RIGHT, ctrl_event.up };
					else
						*action = GameActionSendMouseClick{ GameActionSendMouseClick::LEFT, ctrl_event.up };
				}
				return true;
			default:
				break;
			}
		}
	}

	if (ctrl_event.button == ControllerButton::BUTTON_BACK) {
		return true; // Ignore mod button
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
