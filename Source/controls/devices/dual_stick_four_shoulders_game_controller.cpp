#include "controls/devices/dual_stick_four_shoulders_game_controller.h"

#include "controls/controller_motion.h"
#include "controls/plrctrls.h"
#include "doom.h"
#include "options.h"
#include "qol/itemlabels.h"
#include "stores.h"
#include "utils/language.h"

namespace devilution {

bool DualStick4ShouldersGameController::HandleControllerButtonEvent(const SDL_Event &event, ControllerButtonEvent ctrlEvent, GameAction *action)
{
	bool modifier_active = IsControllerButtonPressed(ControllerButton_AXIS_TRIGGERLEFT);
	select_modifier_active = modifier_active;
	start_modifier_active = modifier_active;

	// Stick clicks simulate the mouse both in menus and in-game.
	if (ctrlEvent.button == ControllerButton_BUTTON_RIGHTSTICK) {
		if (!IsAutomapActive()) {
			if (modifier_active)
				*action = GameActionSendMouseClick { GameActionSendMouseClick::RIGHT, ctrlEvent.up };
			else
				*action = GameActionSendMouseClick { GameActionSendMouseClick::LEFT, ctrlEvent.up };
		}

		return true;
	}

	if (!InGameMenu()) {
		AltPressed(IsControllerButtonPressed(ControllerButton_AXIS_TRIGGERRIGHT));

		if (ctrlEvent.button == ControllerButton_IGNORE)
			return true;

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
		if (ctrlEvent.button == ControllerButton_CANCEL) {
			if (ctrlEvent.up)
				return true;

			if (modifier_active)
#ifdef SWAP_CONFIRM_CANCEL_BUTTONS
				*action = GameActionSendKey { DVL_VK_F7, ctrlEvent.up };
#else
				*action = GameActionSendKey { DVL_VK_F8, ctrlEvent.up };
#endif
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
			case ControllerButton_CONFIRM:
				if (!ctrlEvent.up) {
					if (modifier_active)
#ifdef SWAP_CONFIRM_CANCEL_BUTTONS
						*action = GameActionSendKey { DVL_VK_F8, ctrlEvent.up };
#else
						*action = GameActionSendKey { DVL_VK_F7, ctrlEvent.up };
#endif
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
			}
		}
	}

	return false;
}

AxisDirection DualStick4ShouldersGameController::GetMoveDirection()
{
	return GetLeftStickOrDpadDirection(!select_modifier_active);
}

bool DualStick4ShouldersGameController::GetStartModifierLeftCircleMenuHint(CircleMenuHint *hint)
{
	if (start_modifier_active)
		*hint = CircleMenuHint(/*isDpad=*/true, /*top=*/_("Quests"), /*right=*/_("Inv"), /*bottom=*/_("Spells"), /*left=*/_("Char"));

	return start_modifier_active;
}

bool DualStick4ShouldersGameController::GetStartModifierRightCircleMenuHint(CircleMenuHint *hint)
{
	return false;
}

bool DualStick4ShouldersGameController::GetSelectModifierLeftCircleMenuHint(CircleMenuHint *hint)
{
	return false;
}

bool DualStick4ShouldersGameController::GetSelectModifierRightCircleMenuHint(CircleMenuHint *hint)
{
	if (select_modifier_active)
		*hint = CircleMenuHint(/*isDpad=*/false, /*top=*/"Y", /*right=*/"B", /*bottom=*/"A", /*left=*/"X");

	return select_modifier_active;
}

} // namespace devilution
