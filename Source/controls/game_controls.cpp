#include "controls/game_controls.h"

#include <cstdint>

#include "controls/controller.h"
#include "controls/controller_motion.h"
#include "miniwin/misc_msg.h"
#ifndef USE_SDL1
#include "controls/devices/game_controller.h"
#endif
#include "controls/devices/joystick.h"
#include "controls/menu_controls.h"
#include "controls/modifier_hints.h"
#include "controls/plrctrls.h"
#include "controls/touch/gamepad.h"
#include "doom.h"
#include "gmenu.h"
#include "options.h"
#include "qol/stash.h"
#include "stores.h"

namespace devilution {

bool start_modifier_active = false;
bool select_modifier_active = false;
const ControllerButton ControllerButtonPrimary = ControllerButton_BUTTON_B;
const ControllerButton ControllerButtonSecondary = ControllerButton_BUTTON_Y;
const ControllerButton ControllerButtonTertiary = ControllerButton_BUTTON_X;

namespace {

SDL_Keycode TranslateControllerButtonToKey(ControllerButton controllerButton)
{
	switch (controllerButton) {
	case ControllerButton_BUTTON_A: // Bottom button
		return QuestLogIsOpen ? SDLK_SPACE : SDLK_ESCAPE;
	case ControllerButton_BUTTON_B: // Right button
		return (sgpCurrentMenu != nullptr || stextflag != STORE_NONE || QuestLogIsOpen) ? SDLK_RETURN : SDLK_SPACE;
	case ControllerButton_BUTTON_Y: // Top button
		return SDLK_RETURN;
	case ControllerButton_BUTTON_LEFTSTICK:
		return SDLK_TAB; // Map
	case ControllerButton_BUTTON_BACK:
	case ControllerButton_BUTTON_START:
		return SDLK_ESCAPE;
	case ControllerButton_BUTTON_DPAD_LEFT:
		return SDLK_LEFT;
	case ControllerButton_BUTTON_DPAD_RIGHT:
		return SDLK_RIGHT;
	case ControllerButton_BUTTON_DPAD_UP:
		return SDLK_UP;
	case ControllerButton_BUTTON_DPAD_DOWN:
		return SDLK_DOWN;
	default:
		return SDLK_UNKNOWN;
	}
}

bool HandleStartAndSelect(const ControllerButtonEvent &ctrlEvent, GameAction *action)
{
	const bool inGameMenu = InGameMenu();

	const bool startIsDown = IsControllerButtonPressed(ControllerButton_BUTTON_START);
	const bool selectIsDown = IsControllerButtonPressed(ControllerButton_BUTTON_BACK);
	start_modifier_active = !inGameMenu && startIsDown;
	select_modifier_active = !inGameMenu && selectIsDown && !start_modifier_active;

	// Tracks whether we've received both START and SELECT down events.
	//
	// Using `IsControllerButtonPressed()` for this would be incorrect.
	// If both buttons are pressed simultaneously, SDL sends 2 events for which both buttons are in the pressed state.
	// This allows us to avoid triggering START+SELECT action twice in this case.
	static bool startDownReceived = false;
	static bool selectDownReceived = false;
	switch (ctrlEvent.button) {
	case ControllerButton_BUTTON_BACK:
		selectDownReceived = !ctrlEvent.up;
		break;
	case ControllerButton_BUTTON_START:
		startDownReceived = !ctrlEvent.up;
		break;
	default:
		return false;
	}

	if (startDownReceived && selectDownReceived) {
		*action = GameActionSendKey { SDLK_ESCAPE, ctrlEvent.up };
		return true;
	}

	if (inGameMenu && (startIsDown || selectIsDown) && !ctrlEvent.up) {
		// If both are down, do nothing because `both_received` will trigger soon.
		if (startIsDown && selectIsDown)
			return true;
		*action = GameActionSendKey { SDLK_ESCAPE, ctrlEvent.up };
		return true;
	}

	return false;
}

} // namespace

bool GetGameAction(const SDL_Event &event, ControllerButtonEvent ctrlEvent, GameAction *action)
{
	const bool inGameMenu = InGameMenu();

#ifndef USE_SDL1
	if (ControlMode == ControlTypes::VirtualGamepad) {
		if (event.type == SDL_FINGERDOWN) {
			if (VirtualGamepadState.menuPanel.charButton.isHeld && VirtualGamepadState.menuPanel.charButton.didStateChange) {
				*action = GameAction(GameActionType_TOGGLE_CHARACTER_INFO);
				return true;
			}
			if (VirtualGamepadState.menuPanel.questsButton.isHeld && VirtualGamepadState.menuPanel.questsButton.didStateChange) {
				*action = GameAction(GameActionType_TOGGLE_QUEST_LOG);
				return true;
			}
			if (VirtualGamepadState.menuPanel.inventoryButton.isHeld && VirtualGamepadState.menuPanel.inventoryButton.didStateChange) {
				*action = GameAction(GameActionType_TOGGLE_INVENTORY);
				return true;
			}
			if (VirtualGamepadState.menuPanel.mapButton.isHeld && VirtualGamepadState.menuPanel.mapButton.didStateChange) {
				*action = GameActionSendKey { SDLK_TAB, false };
				return true;
			}
			if (VirtualGamepadState.primaryActionButton.isHeld && VirtualGamepadState.primaryActionButton.didStateChange) {
				if (!inGameMenu && !QuestLogIsOpen && !sbookflag) {
					*action = GameAction(GameActionType_PRIMARY_ACTION);
					if (ControllerButtonHeld == ControllerButton_NONE) {
						ControllerButtonHeld = ControllerButtonPrimary;
					}
				} else if (sgpCurrentMenu != nullptr || stextflag != STORE_NONE || QuestLogIsOpen) {
					*action = GameActionSendKey { SDLK_RETURN, false };
				} else {
					*action = GameActionSendKey { SDLK_SPACE, false };
				}
				return true;
			}
			if (VirtualGamepadState.secondaryActionButton.isHeld && VirtualGamepadState.secondaryActionButton.didStateChange) {
				if (!inGameMenu && !QuestLogIsOpen && !sbookflag) {
					*action = GameAction(GameActionType_SECONDARY_ACTION);
					if (ControllerButtonHeld == ControllerButton_NONE)
						ControllerButtonHeld = ControllerButtonSecondary;
				}
				return true;
			}
			if (VirtualGamepadState.spellActionButton.isHeld && VirtualGamepadState.spellActionButton.didStateChange) {
				if (!inGameMenu && !QuestLogIsOpen && !sbookflag) {
					*action = GameAction(GameActionType_CAST_SPELL);
					if (ControllerButtonHeld == ControllerButton_NONE)
						ControllerButtonHeld = ControllerButtonTertiary;
				}
				return true;
			}
			if (VirtualGamepadState.cancelButton.isHeld && VirtualGamepadState.cancelButton.didStateChange) {
				if (inGameMenu || DoomFlag || spselflag)
					*action = GameActionSendKey { SDLK_ESCAPE, false };
				else if (invflag)
					*action = GameAction(GameActionType_TOGGLE_INVENTORY);
				else if (sbookflag)
					*action = GameAction(GameActionType_TOGGLE_SPELL_BOOK);
				else if (QuestLogIsOpen)
					*action = GameAction(GameActionType_TOGGLE_QUEST_LOG);
				else if (chrflag)
					*action = GameAction(GameActionType_TOGGLE_CHARACTER_INFO);
				return true;
			}
			if (VirtualGamepadState.healthButton.isHeld && VirtualGamepadState.healthButton.didStateChange) {
				if (!QuestLogIsOpen && !sbookflag && stextflag == STORE_NONE)
					*action = GameAction(GameActionType_USE_HEALTH_POTION);
				return true;
			}
			if (VirtualGamepadState.manaButton.isHeld && VirtualGamepadState.manaButton.didStateChange) {
				if (!QuestLogIsOpen && !sbookflag && stextflag == STORE_NONE)
					*action = GameAction(GameActionType_USE_MANA_POTION);
				return true;
			}
		} else if (event.type == SDL_FINGERUP) {
			if ((!VirtualGamepadState.primaryActionButton.isHeld && ControllerButtonHeld == ControllerButtonPrimary)
			    || (!VirtualGamepadState.secondaryActionButton.isHeld && ControllerButtonHeld == ControllerButtonSecondary)
			    || (!VirtualGamepadState.spellActionButton.isHeld && ControllerButtonHeld == ControllerButtonTertiary)) {
				ControllerButtonHeld = ControllerButton_NONE;
				LastMouseButtonAction = MouseActionType::None;
			}
		}
	}
#endif
	if (IsStashOpen && ctrlEvent.button == ControllerButton_BUTTON_BACK) {
		StartGoldWithdraw();
		return false;
	}

	if (HandleStartAndSelect(ctrlEvent, action))
		return true;

	// Stick clicks simulate the mouse both in menus and in-game.
	switch (ctrlEvent.button) {
	case ControllerButton_BUTTON_LEFTSTICK:
		if (select_modifier_active) {
			if (!IsAutomapActive())
				*action = GameActionSendKey { SDL_BUTTON_LEFT | KeymapperMouseButtonMask, ctrlEvent.up };
			return true;
		}
		break;
	case ControllerButton_BUTTON_RIGHTSTICK:
		if (!IsAutomapActive()) {
			if (IsControllerButtonPressed(ControllerButton_BUTTON_BACK))
				*action = GameActionSendKey { SDL_BUTTON_RIGHT | KeymapperMouseButtonMask, ctrlEvent.up };
			else
				*action = GameActionSendKey { SDL_BUTTON_LEFT | KeymapperMouseButtonMask, ctrlEvent.up };
		}
		return true;
	default:
		break;
	}

	if (!inGameMenu) {
		switch (ctrlEvent.button) {
		case ControllerButton_BUTTON_LEFTSHOULDER:
			if ((select_modifier_active && !sgOptions.Controller.bSwapShoulderButtonMode) || (sgOptions.Controller.bSwapShoulderButtonMode && !select_modifier_active)) {
				if (!IsAutomapActive())
					*action = GameActionSendKey { SDL_BUTTON_LEFT | KeymapperMouseButtonMask, ctrlEvent.up };
				return true;
			}
			break;
		case ControllerButton_BUTTON_RIGHTSHOULDER:
			if ((select_modifier_active && !sgOptions.Controller.bSwapShoulderButtonMode) || (sgOptions.Controller.bSwapShoulderButtonMode && !select_modifier_active)) {
				if (!IsAutomapActive())
					*action = GameActionSendKey { SDL_BUTTON_RIGHT | KeymapperMouseButtonMask, ctrlEvent.up };
				return true;
			}
			break;
		case ControllerButton_AXIS_TRIGGERLEFT: // ZL (aka L2)
			if (!ctrlEvent.up) {
				if (select_modifier_active)
					*action = GameAction(GameActionType_TOGGLE_QUEST_LOG);
				else
					*action = GameAction(GameActionType_TOGGLE_CHARACTER_INFO);
			}
			return true;
		case ControllerButton_AXIS_TRIGGERRIGHT: // ZR (aka R2)
			if (!ctrlEvent.up) {
				if (select_modifier_active)
					*action = GameAction(GameActionType_TOGGLE_SPELL_BOOK);
				else
					*action = GameAction(GameActionType_TOGGLE_INVENTORY);
			}
			return true;
		case ControllerButton_IGNORE:
		case ControllerButton_BUTTON_START:
		case ControllerButton_BUTTON_BACK:
			return true;
		default:
			break;
		}
		if (sgOptions.Controller.bDpadHotkeys) {
			switch (ctrlEvent.button) {
			case ControllerButton_BUTTON_DPAD_UP:
				if (IsControllerButtonPressed(ControllerButton_BUTTON_BACK))
					*action = GameActionSendKey { sgOptions.Keymapper.KeyForAction("QuickSpell2"), ctrlEvent.up };
				else
					*action = GameActionSendKey { SDLK_ESCAPE, ctrlEvent.up };
				return true;
			case ControllerButton_BUTTON_DPAD_RIGHT:
				if (IsControllerButtonPressed(ControllerButton_BUTTON_BACK))
					*action = GameActionSendKey { sgOptions.Keymapper.KeyForAction("QuickSpell4"), ctrlEvent.up };
				else if (!ctrlEvent.up)
					*action = GameAction(GameActionType_TOGGLE_INVENTORY);
				return true;
			case ControllerButton_BUTTON_DPAD_DOWN:
				if (IsControllerButtonPressed(ControllerButton_BUTTON_BACK))
					*action = GameActionSendKey { sgOptions.Keymapper.KeyForAction("QuickSpell3"), ctrlEvent.up };
				else
					*action = GameActionSendKey { SDLK_TAB, ctrlEvent.up };
				return true;
			case ControllerButton_BUTTON_DPAD_LEFT:
				if (IsControllerButtonPressed(ControllerButton_BUTTON_BACK))
					*action = GameActionSendKey { sgOptions.Keymapper.KeyForAction("QuickSpell1"), ctrlEvent.up };
				else if (!ctrlEvent.up)
					*action = GameAction(GameActionType_TOGGLE_CHARACTER_INFO);
				return true;
			default:
				break;
			}
		}
		if (start_modifier_active) {
			switch (ctrlEvent.button) {
			case ControllerButton_BUTTON_DPAD_UP:
				*action = GameActionSendKey { SDLK_ESCAPE, ctrlEvent.up };
				return true;
			case ControllerButton_BUTTON_DPAD_RIGHT:
				if (!ctrlEvent.up)
					*action = GameAction(GameActionType_TOGGLE_INVENTORY);
				return true;
			case ControllerButton_BUTTON_DPAD_DOWN:
				*action = GameActionSendKey { SDLK_TAB, ctrlEvent.up };
				return true;
			case ControllerButton_BUTTON_DPAD_LEFT:
				if (!ctrlEvent.up)
					*action = GameAction(GameActionType_TOGGLE_CHARACTER_INFO);
				return true;
			case ControllerButton_BUTTON_Y: // Top button
#ifdef __3DS__
				if (!ctrlEvent.up) {
					sgOptions.Graphics.zoom.SetValue(!*sgOptions.Graphics.zoom);
					CalcViewportGeometry();
				}
#else
				/* Not mapped. Reserved for future use. */
#endif
				return true;
			case ControllerButton_BUTTON_B: // Right button
				// Not mapped. TODO: map to attack in place.
				return true;
			case ControllerButton_BUTTON_A: // Bottom button
				if (!ctrlEvent.up)
					*action = GameAction(GameActionType_TOGGLE_SPELL_BOOK);
				return true;
			case ControllerButton_BUTTON_X: // Left button
				if (!ctrlEvent.up)
					*action = GameAction(GameActionType_TOGGLE_QUEST_LOG);
				return true;
			case ControllerButton_BUTTON_LEFTSHOULDER:
				if (!ctrlEvent.up)
					*action = GameAction(GameActionType_TOGGLE_CHARACTER_INFO);
				return true;
			case ControllerButton_BUTTON_RIGHTSHOULDER:
				if (!ctrlEvent.up)
					*action = GameAction(GameActionType_TOGGLE_INVENTORY);
				return true;
			default:
				return true;
			}
		}

		// Bottom button: Closes menus or opens quick spell book if nothing is open.
		if (ctrlEvent.button == ControllerButton_BUTTON_A) { // Bottom button
			if (ctrlEvent.up)
				return true;
			if (IsControllerButtonPressed(ControllerButton_BUTTON_BACK))
				*action = GameActionSendKey { sgOptions.Keymapper.KeyForAction("QuickSpell3"), ctrlEvent.up };
			else if (DoomFlag)
				*action = GameActionSendKey { SDLK_ESCAPE, ctrlEvent.up };
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
			case ControllerButton_BUTTON_B: // Right button
				if (!ctrlEvent.up) {
					if (IsControllerButtonPressed(ControllerButton_BUTTON_BACK))
						*action = GameActionSendKey { sgOptions.Keymapper.KeyForAction("QuickSpell4"), ctrlEvent.up };
					else
						*action = GameAction(GameActionType_PRIMARY_ACTION);
				}
				return true;
			case ControllerButton_BUTTON_Y: // Top button
				if (!ctrlEvent.up) {
					if (IsControllerButtonPressed(ControllerButton_BUTTON_BACK))
						*action = GameActionSendKey { sgOptions.Keymapper.KeyForAction("QuickSpell2"), ctrlEvent.up };
					else
						*action = GameAction(GameActionType_SECONDARY_ACTION);
				}
				return true;
			case ControllerButton_BUTTON_X: // Left button
				if (!ctrlEvent.up) {
					if (IsControllerButtonPressed(ControllerButton_BUTTON_BACK))
						*action = GameActionSendKey { sgOptions.Keymapper.KeyForAction("QuickSpell1"), ctrlEvent.up };
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

		if (ctrlEvent.button == ControllerButton_BUTTON_BACK) {
			return true; // Ignore mod button
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
		*action = GameActionSendKey { static_cast<uint32_t>(TranslateControllerButtonToKey(ctrlEvent.button)),
			ctrlEvent.up };
		return true;
	}

#ifndef USE_SDL1
	// Ignore unhandled joystick events where a GameController is open for this joystick.
	// This is because SDL sends both game controller and joystick events in this case.
	const Joystick *const joystick = Joystick::Get(event);
	if (joystick != nullptr && GameController::Get(joystick->instance_id()) != nullptr) {
		return true;
	}
	if (event.type == SDL_CONTROLLERAXISMOTION) {
		return true; // Ignore releasing the trigger buttons
	}
#endif

	return false;
}

bool IsSimulatedMouseClickBinding(ControllerButtonEvent ctrlEvent)
{
	switch (ctrlEvent.button) {
	case ControllerButton_BUTTON_LEFTSTICK:
	case ControllerButton_BUTTON_LEFTSHOULDER:
	case ControllerButton_BUTTON_RIGHTSHOULDER:
		return select_modifier_active;
	case ControllerButton_BUTTON_RIGHTSTICK:
		return true;
	default:
		return false;
	}
}

AxisDirection GetMoveDirection()
{
	return GetLeftStickOrDpadDirection(/*allowDpad=*/!sgOptions.Controller.bDpadHotkeys);
}

} // namespace devilution
