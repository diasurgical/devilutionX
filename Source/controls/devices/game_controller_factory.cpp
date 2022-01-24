#include "controls/devices/game_controller_factory.h"

#include "controls/devices/default_game_controller.h"
#include "controls/devices/dual_stick_four_shoulders_game_controller.h"

namespace devilution {

GameController *GameControllerFactory::Create()
{
// TODO add an OPTIONS_SET_GAME_CONTROLLER and get selected controller from options for alowing PC users to choose from a list of predefined controllers
#if defined(DUAL_STICK_4_SHOULDERS_GAME_CONTROLLER)
	return new DualStick4ShouldersGameController();
#elif defined(OPTIONS_SET_GAME_CONTROLLER)
	return nullptr;
#else
	return new DefaultGameController();
#endif
}

} // namespace devilution
