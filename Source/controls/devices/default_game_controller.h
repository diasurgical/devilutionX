#pragma once

#include "controls/devices/game_controller.h"

namespace devilution {

class DefaultGameController : public GameController {
public:
	AxisDirection GetMoveDirection();
	bool GetStartModifierLeftCircleMenuHint(CircleMenuHint *hint);
	bool GetStartModifierRightCircleMenuHint(CircleMenuHint *hint);
	bool CanDrawSelectModifierLeftCircleMenuHint();
	bool CanDrawSelectModifierRightCircleMenuHint();

protected:
	bool HandleControllerButtonEvent(const SDL_Event &event, ControllerButtonEvent ctrlEvent, GameAction *action);

private:
	bool HandleStartAndSelect(const ControllerButtonEvent &ctrlEvent, GameAction *action);
};

} // namespace devilution
