#pragma once

#include "controls/devices/game_controller.h"

namespace devilution {

class DualStick4ShouldersGameController : public GameController {
public:
	AxisDirection GetMoveDirection();
	bool GetStartModifierLeftCircleMenuHint(CircleMenuHint *hint);
	bool GetStartModifierRightCircleMenuHint(CircleMenuHint *hint);
	bool CanDrawSelectModifierLeftCircleMenuHint();
	bool CanDrawSelectModifierRightCircleMenuHint();

protected:
	bool HandleControllerButtonEvent(const SDL_Event &event, ControllerButtonEvent ctrlEvent, GameAction *action);
};

} // namespace devilution
