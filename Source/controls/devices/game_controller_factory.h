#pragma once

#include "controls/devices/game_controller.h"

namespace devilution {

class GameControllerFactory {
public:
	static GameController *Create();
};

} // namespace devilution
