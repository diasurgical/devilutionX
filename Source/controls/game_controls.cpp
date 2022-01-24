#include "controls/game_controls.h"

#include "controls/controller_motion.h"
#include "controls/devices/game_controller.h"
#include "controls/devices/joystick.h"
#include "controls/game_controls.h"
#include "controls/plrctrls.h"
#include "doom.h"
#include "gmenu.h"
#include "options.h"
#include "qol/itemlabels.h"
#include "stores.h"
#include "utils/language.h"

namespace devilution {

bool GetGameAction(const SDL_Event &event, ControllerButtonEvent ctrlEvent, GameAction *action)
{
	GameController *controller = GameController::Get(event);

	if (controller)
		return controller->GetGameAction(event, ctrlEvent, action);

	return false;
}

AxisDirection GetMoveDirection()
{
    GameController *controller = GameController::GetCurrentGameController();

    if (controller)
        return controller->GetMoveDirection();

    return AxisDirection { AxisDirectionX_NONE, AxisDirectionY_NONE };
}

bool GetStartModifierLeftCircleMenuHint(CircleMenuHint *hint)
{
    GameController *controller = GameController::GetCurrentGameController();

    if (controller)
	    return controller->GetStartModifierLeftCircleMenuHint(hint);

    return false;
}

bool GetStartModifierRightCircleMenuHint(CircleMenuHint *hint)
{
    GameController *controller = GameController::GetCurrentGameController();

    if (controller)
	    return controller->GetStartModifierRightCircleMenuHint(hint);

    return false;
}

bool GetSelectModifierLeftCircleMenuHint(CircleMenuHint *hint)
{
    GameController *controller = GameController::GetCurrentGameController();

    if (controller)
	    return controller->GetSelectModifierLeftCircleMenuHint(hint);

    return false;
}

bool GetSelectModifierRightCircleMenuHint(CircleMenuHint *hint)
{
    GameController *controller = GameController::GetCurrentGameController();

    if (controller)
	    return controller->GetSelectModifierRightCircleMenuHint(hint);

    return false;
}

MenuAction GetAButtonMenuAction(const SDL_Event &event)
{
	GameController *controller = GameController::Get(event);

	if (controller)
		return controller->GetAButtonMenuAction();

	return MenuAction_NONE;
}

MenuAction GetBButtonMenuAction(const SDL_Event &event)
{
	GameController *controller = GameController::Get(event);

	if (controller)
		return controller->GetBButtonMenuAction();

	return MenuAction_NONE;
}

} // namespace devilution
