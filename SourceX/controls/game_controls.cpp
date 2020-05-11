#include "controls/game_controls.h"

#ifndef _XBOX
#include <cstdint>
#endif

#include "controls/controller.h"
#include "controls/controller_motion.h"
#ifndef _XBOX
#include "controls/devices/game_controller.h"
#endif
#include "controls/devices/joystick.h"
#include "controls/menu_controls.h"
#include "controls/modifier_hints.h"
#include "controls/plrctrls.h"

namespace dvl {

namespace {

DWORD translate_controller_button_to_key(ControllerButtonNS::ControllerButton controller_button)
{
	switch (controller_button) {
	case ControllerButtonNS::BUTTON_A: // Bottom button
		return questlog ? DVL_VK_SPACE : DVL_VK_ESCAPE;
	case ControllerButtonNS::BUTTON_B: // Right button
		return sgpCurrentMenu || stextflag || questlog ? DVL_VK_RETURN : DVL_VK_SPACE;
	case ControllerButtonNS::BUTTON_Y: // Top button
		return DVL_VK_RETURN;
	case ControllerButtonNS::BUTTON_LEFTSTICK:
		return DVL_VK_TAB; // Map
	case ControllerButtonNS::BUTTON_START:
		return DVL_VK_ESCAPE;
	case ControllerButtonNS::BUTTON_DPAD_LEFT:
		return DVL_VK_LEFT;
	case ControllerButtonNS::BUTTON_DPAD_RIGHT:
		return DVL_VK_RIGHT;
	case ControllerButtonNS::BUTTON_DPAD_UP:
		return DVL_VK_UP;
	case ControllerButtonNS::BUTTON_DPAD_DOWN:
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

	start_modifier_active = !in_game_menu && IsControllerButtonPressed(ControllerButtonNS::BUTTON_START);
	select_modifier_active = !in_game_menu && IsControllerButtonPressed(ControllerButtonNS::BUTTON_BACK);

	// SELECT + D-Pad simulating mouse movement.
	if (IsControllerButtonPressed(ControllerButtonNS::BUTTON_BACK) && IsDPadButton(ctrl_event.button)) {
		return true;
	}

	// START + SELECT
	if (!ctrl_event.up
	    && ((ctrl_event.button == ControllerButtonNS::BUTTON_BACK && IsControllerButtonPressed(ControllerButtonNS::BUTTON_START))
	           || (ctrl_event.button == ControllerButtonNS::BUTTON_START && IsControllerButtonPressed(ControllerButtonNS::BUTTON_BACK)))) {
		select_modifier_active = start_modifier_active = false;
		*action = GameActionSendKey( DVL_VK_ESCAPE, ctrl_event.up );
		return true;
	}

	if (!in_game_menu) {
		switch (ctrl_event.button) {
		case ControllerButtonNS::BUTTON_LEFTSHOULDER:
			if (IsControllerButtonPressed(ControllerButtonNS::BUTTON_BACK)) {
				if (!IsAutomapActive())
					*action = GameActionSendMouseClick( GameActionSendMouseClick::LEFT, ctrl_event.up );
				return true;
			}
			break;
		case ControllerButtonNS::BUTTON_LEFTSTICK:
			if (IsControllerButtonPressed(ControllerButtonNS::BUTTON_BACK)) {
				if (!IsAutomapActive())
					*action = GameActionSendMouseClick( GameActionSendMouseClick::RIGHT, ctrl_event.up );
				return true;
			}
			break;
		case ControllerButtonNS::AXIS_TRIGGERLEFT: // ZL (aka L2)
			if (!ctrl_event.up) {
				if (IsControllerButtonPressed(ControllerButtonNS::BUTTON_BACK))
					*action = GameAction(GameActionTypeNS::TOGGLE_QUEST_LOG);
				else
					*action = GameAction(GameActionTypeNS::TOGGLE_CHARACTER_INFO);
			}
			return true;
		case ControllerButtonNS::AXIS_TRIGGERRIGHT: // ZR (aka R2)
			if (!ctrl_event.up) {
				if (IsControllerButtonPressed(ControllerButtonNS::BUTTON_BACK))
					*action = GameAction(GameActionTypeNS::TOGGLE_SPELL_BOOK);
				else
					*action = GameAction(GameActionTypeNS::TOGGLE_INVENTORY);
			}
			return true;
//		case ControllerButtonNS::BUTTON_LEFTSTICK:
//			if (IsControllerButtonPressed(ControllerButtonNS::BUTTON_BACK)) {
//				if (!IsAutomapActive())
//					*action = GameActionSendMouseClick( GameActionSendMouseClick::LEFT, ctrl_event.up );
//				return true;
//			}
//			break;
		case ControllerButtonNS::BUTTON_START:
			if (IsControllerButtonPressed(ControllerButtonNS::BUTTON_BACK)) {
				*action = GameActionSendKey( DVL_VK_ESCAPE, ctrl_event.up );
			}
			return true;
			break;
		default:
			break;
		}
		if (IsControllerButtonPressed(ControllerButtonNS::BUTTON_START)) {
			switch (ctrl_event.button) {
			case ControllerButtonNS::CBIGNORE:
			case ControllerButtonNS::BUTTON_START:
				return true;
			case ControllerButtonNS::BUTTON_DPAD_UP:
				*action = GameActionSendKey( DVL_VK_ESCAPE, ctrl_event.up );
				return true;
			case ControllerButtonNS::BUTTON_DPAD_RIGHT:
				if (!ctrl_event.up)
					*action = GameAction(GameActionTypeNS::TOGGLE_INVENTORY);
				return true;
			case ControllerButtonNS::BUTTON_DPAD_DOWN:
				*action = GameActionSendKey( DVL_VK_TAB, ctrl_event.up );
				return true;
			case ControllerButtonNS::BUTTON_DPAD_LEFT:
				if (!ctrl_event.up)
					*action = GameAction(GameActionTypeNS::TOGGLE_CHARACTER_INFO);
				return true;
			case ControllerButtonNS::BUTTON_Y: // Top button
				// Not mapped. Reserved for future use.
				return true;
			case ControllerButtonNS::BUTTON_B: // Right button
				// Not mapped. TODO: map to attack in place.
				return true;
			case ControllerButtonNS::BUTTON_A: // Bottom button
				if (!ctrl_event.up)
					*action = GameAction(GameActionTypeNS::TOGGLE_SPELL_BOOK);
				return true;
			case ControllerButtonNS::BUTTON_X: // Left button
				if (!ctrl_event.up)
					*action = GameAction(GameActionTypeNS::TOGGLE_QUEST_LOG);
				return true;
			case ControllerButtonNS::BUTTON_LEFTSHOULDER:
				if (!ctrl_event.up)
					*action = GameAction(GameActionTypeNS::TOGGLE_CHARACTER_INFO);
				return true;
			case ControllerButtonNS::BUTTON_RIGHTSHOULDER:
				if (!ctrl_event.up)
					*action = GameAction(GameActionTypeNS::TOGGLE_INVENTORY);
				return true;
			default:
				return true;
			}
		}

		// Bottom button: Closes menus or opens quick spell book if nothing is open.
		if (ctrl_event.button == ControllerButtonNS::BUTTON_A) { // Bottom button
			if (ctrl_event.up) return true;
			if (IsControllerButtonPressed(ControllerButtonNS::BUTTON_BACK))
				*action = GameActionSendKey( DVL_VK_F7, ctrl_event.up );
			else if (invflag)
				*action = GameAction(GameActionTypeNS::TOGGLE_INVENTORY);
			else if (sbookflag)
				*action = GameAction(GameActionTypeNS::TOGGLE_SPELL_BOOK);
			else if (questlog)
				*action = GameAction(GameActionTypeNS::TOGGLE_QUEST_LOG);
			else if (chrflag)
				*action = GameAction(GameActionTypeNS::TOGGLE_CHARACTER_INFO);
			else
				*action = GameAction(GameActionTypeNS::TOGGLE_QUICK_SPELL_MENU);
			return true;
		}

		if (!questlog && !sbookflag) {
			switch (ctrl_event.button) {
			case ControllerButtonNS::CBIGNORE:
				return true;
			case ControllerButtonNS::BUTTON_B: // Right button
				if (!ctrl_event.up) {
					if (IsControllerButtonPressed(ControllerButtonNS::BUTTON_BACK))
						*action = GameActionSendKey( DVL_VK_F8, ctrl_event.up );
					else
						*action = GameAction(GameActionTypeNS::PRIMARY_ACTION);
				}
				return true;
			case ControllerButtonNS::BUTTON_Y: // Top button
				if (!ctrl_event.up) {
					if (IsControllerButtonPressed(ControllerButtonNS::BUTTON_BACK))
						*action = GameActionSendKey( DVL_VK_F6, ctrl_event.up );
					else
						*action = GameAction(GameActionTypeNS::SECONDARY_ACTION);
				}
				return true;
			case ControllerButtonNS::BUTTON_X: // Left button
				if (!ctrl_event.up) {
					if (IsControllerButtonPressed(ControllerButtonNS::BUTTON_BACK))
						*action = GameActionSendKey( DVL_VK_F5, ctrl_event.up );
					else
						*action = GameAction(GameActionTypeNS::CAST_SPELL);
				}
				return true;
			case ControllerButtonNS::BUTTON_LEFTSHOULDER:
				if (!stextflag && !ctrl_event.up)
					*action = GameAction(GameActionTypeNS::USE_HEALTH_POTION);
				return true;
			case ControllerButtonNS::BUTTON_RIGHTSHOULDER:
				if (!stextflag && !ctrl_event.up)
					*action = GameAction(GameActionTypeNS::USE_MANA_POTION);
				return true;
			case ControllerButtonNS::BUTTON_DPAD_UP:
			case ControllerButtonNS::BUTTON_DPAD_DOWN:
			case ControllerButtonNS::BUTTON_DPAD_LEFT:
			case ControllerButtonNS::BUTTON_DPAD_RIGHT:
				// The rest of D-Pad actions are handled in charMovement() on every game_logic() call.
				return true;
			case ControllerButtonNS::BUTTON_RIGHTSTICK:
				if (!IsAutomapActive()) {
					if (IsControllerButtonPressed(ControllerButtonNS::BUTTON_BACK))
						*action = GameActionSendMouseClick( GameActionSendMouseClick::RIGHT, ctrl_event.up );
					else
						*action = GameActionSendMouseClick( GameActionSendMouseClick::LEFT, ctrl_event.up );
				}
				return true;
			default:
				break;
			}
		}
	}

	if (ctrl_event.button == ControllerButtonNS::BUTTON_BACK) {
		return true; // Ignore mod button
	}

	// By default, map to a keyboard key.
	if (ctrl_event.button != ControllerButtonNS::NONE) {
		*action = GameActionSendKey( translate_controller_button_to_key(ctrl_event.button),
			ctrl_event.up );
		return true;
	}

#ifndef USE_SDL1
	// Ignore unhandled joystick events if gamepad is active.
	// We receive the same events as gamepad events.
	if (CurrentGameController() != NULL && event.type >= SDL_JOYAXISMOTION && event.type <= SDL_JOYBUTTONUP) {
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
	MoveDirection result(MoveDirectionXNS::NONE, MoveDirectionYNS::NONE);

	if (stickY >= 0.5 || IsControllerButtonPressed(ControllerButtonNS::BUTTON_DPAD_UP)) {
		result.y = MoveDirectionYNS::UP;
	} else if (stickY <= -0.5 || IsControllerButtonPressed(ControllerButtonNS::BUTTON_DPAD_DOWN)) {
		result.y = MoveDirectionYNS::DOWN;
	}

	if (stickX <= -0.5 || IsControllerButtonPressed(ControllerButtonNS::BUTTON_DPAD_LEFT)) {
		result.x = MoveDirectionXNS::LEFT;
	} else if (stickX >= 0.5 || IsControllerButtonPressed(ControllerButtonNS::BUTTON_DPAD_RIGHT)) {
		result.x = MoveDirectionXNS::RIGHT;
	}

	return result;
}

} // namespace dvl
