#pragma once

#include "controls/devices/game_controller.h"

namespace devilution {

class DefaultGameController : public GameController {
public:
	AxisDirection GetMoveDirection();
	bool GetStartModifierLeftCircleMenuHint(CircleMenuHint *hint);
	bool GetStartModifierRightCircleMenuHint(CircleMenuHint *hint);
	bool GetSelectModifierLeftCircleMenuHint(CircleMenuHint *hint);
	bool GetSelectModifierRightCircleMenuHint(CircleMenuHint *hint);

protected:
	bool HandleControllerButtonEvent(const SDL_Event &event, ControllerButtonEvent ctrlEvent, GameAction *action);

private:
	bool HandleStartAndSelect(const ControllerButtonEvent &ctrlEvent, GameAction *action);
};

} // namespace devilution
