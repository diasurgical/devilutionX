#include "controls/game_controls.h"

#include <cstdint>

#include "controls/controller.h"
#include "controls/controller_motion.h"
#include "controls/devices/game_controller.h"
#include "controls/devices/joystick.h"
#include "controls/menu_controls.h"
#include "controls/modifier_hints.h"
#include "controls/plrctrls.h"
#include "qol/itemlabels.h"
#include "doom.h"
#include "gmenu.h"
#include "options.h"
#include "stores.h"

namespace devilution {

bool start_modifier_active = false;
bool select_modifier_active = false;

namespace {

uint32_t TranslateControllerButtonToKey(ControllerButton controllerButton)
{
	switch (controllerButton) {
	case ControllerButton_BUTTON_A:
		return (sgpCurrentMenu != nullptr || stextflag != STORE_NONE || QuestLogIsOpen) ? DVL_VK_RETURN : DVL_VK_SPACE;
	case ControllerButton_BUTTON_B:
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

bool HandleStartAndSelect(const ControllerButtonEvent &ctrlEvent, GameAction *action)
{
	return false;
}

} // namespace

bool GetGameAction(const SDL_Event &event, ControllerButtonEvent ctrlEvent, GameAction *action)
{
	const bool inGameMenu = InGameMenu();
	bool modifier_active = IsControllerButtonPressed(ControllerButton_AXIS_TRIGGERLEFT);
	select_modifier_active = modifier_active;
	start_modifier_active = modifier_active;

	// Stick clicks simulate the mouse both in menus and in-game.
	switch (ctrlEvent.button) {
	case ControllerButton_BUTTON_RIGHTSTICK:
		if (!IsAutomapActive()) {
			if (modifier_active)
				*action = GameActionSendMouseClick { GameActionSendMouseClick::RIGHT, ctrlEvent.up };
			else
				*action = GameActionSendMouseClick { GameActionSendMouseClick::LEFT, ctrlEvent.up };
		}
		return true;
	default:
		break;
	}

	if (!inGameMenu) {
		AltPressed(IsControllerButtonPressed(ControllerButton_AXIS_TRIGGERRIGHT));

		switch (ctrlEvent.button) {
		case ControllerButton_IGNORE:
			return true;
		default:
			break;
		}

		if (modifier_active) {
			switch (ctrlEvent.button) {
			case ControllerButton_BUTTON_DPAD_UP:
				if (!ctrlEvent.up)
					*action = GameAction(GameActionType_TOGGLE_QUEST_LOG);
				return true;
			case ControllerButton_BUTTON_DPAD_RIGHT:
				if (!ctrlEvent.up)
					*action = GameAction(GameActionType_TOGGLE_INVENTORY);
				return true;
			case ControllerButton_BUTTON_DPAD_DOWN:
				if (!ctrlEvent.up)
					*action = GameAction(GameActionType_TOGGLE_SPELL_BOOK);
				return true;
			case ControllerButton_BUTTON_DPAD_LEFT:
				if (!ctrlEvent.up)
					*action = GameAction(GameActionType_TOGGLE_CHARACTER_INFO);
				return true;
			}
		}

		// Closes menus or opens quick spell book if nothing is open.
		if (ctrlEvent.button == ControllerButton_BUTTON_B) {
			if (ctrlEvent.up)
				return true;
			if (modifier_active)
				*action = GameActionSendKey { DVL_VK_F8, ctrlEvent.up };
			else if (DoomFlag)
				*action = GameActionSendKey { DVL_VK_ESCAPE, ctrlEvent.up };
			else if (invflag)
				*action = GameAction(GameActionType_TOGGLE_INVENTORY);
			else if (sbookflag)
				*action = GameAction(GameActionType_TOGGLE_SPELL_BOOK);
			else if (QuestLogIsOpen)
				*action = GameAction(GameActionType_TOGGLE_QUEST_LOG);
			else if (chrflag)
				*action = GameAction(GameActionType_TOGGLE_CHARACTER_INFO);
			else
				*action = GameAction(GameActionType_TOGGLE_QUICK_SPELL_MENU);
			return true;
		}

		if (!QuestLogIsOpen && !sbookflag) {
			switch (ctrlEvent.button) {
			case ControllerButton_IGNORE:
				return true;
			case ControllerButton_BUTTON_A:
				if (!ctrlEvent.up) {
					if (modifier_active)
						*action = GameActionSendKey { DVL_VK_F7, ctrlEvent.up };
					else
						*action = GameAction(GameActionType_PRIMARY_ACTION);
				}
				return true;
			case ControllerButton_BUTTON_X:
				if (!ctrlEvent.up) {
					if (modifier_active)
						*action = GameActionSendKey { DVL_VK_F5, ctrlEvent.up };
					else
						*action = GameAction(GameActionType_SECONDARY_ACTION);
				}
				return true;
			case ControllerButton_BUTTON_Y:
				if (!ctrlEvent.up) {
					if (modifier_active)
						*action = GameActionSendKey { DVL_VK_F6, ctrlEvent.up };
					else
						*action = GameAction(GameActionType_CAST_SPELL);
				}
				return true;
			case ControllerButton_BUTTON_LEFTSHOULDER:
				if (stextflag == STORE_NONE && !ctrlEvent.up)
					*action = GameAction(GameActionType_USE_HEALTH_POTION);
				return true;
			case ControllerButton_BUTTON_RIGHTSHOULDER:
				if (stextflag == STORE_NONE && !ctrlEvent.up)
					*action = GameAction(GameActionType_USE_MANA_POTION);
				return true;
			case ControllerButton_BUTTON_DPAD_UP:
			case ControllerButton_BUTTON_DPAD_DOWN:
			case ControllerButton_BUTTON_DPAD_LEFT:
			case ControllerButton_BUTTON_DPAD_RIGHT:
				// The rest of D-Pad actions are handled in charMovement() on every game_logic() call.
				return true;
			default:
				break;
			}
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
		default:
			break;
		}
	}

	// By default, map to a keyboard key.
	if (ctrlEvent.button != ControllerButton_NONE) {
		*action = GameActionSendKey { TranslateControllerButtonToKey(ctrlEvent.button),
			ctrlEvent.up };
		return true;
	}

	// Ignore unhandled joystick events where a GameController is open for this joystick.
	// This is because SDL sends both game controller and joystick events in this case.
	const Joystick *const joystick = Joystick::Get(event);
	if (joystick != nullptr && GameController::Get(joystick->instance_id()) != nullptr) {
		return true;
	}
	if (event.type == SDL_CONTROLLERAXISMOTION) {
		return true; // Ignore releasing the trigger buttons
	}

	return false;
}

AxisDirection GetMoveDirection()
{
	return GetLeftStickOrDpadDirection(!select_modifier_active);
}

} // namespace devilution

