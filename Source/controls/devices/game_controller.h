
#pragma once

#include <vector>

#include <SDL.h>

#include "controls/controller_buttons.h"

#ifndef USE_SDL1
namespace devilution {

class GameController {
	static std::vector<GameController> *const controllers_;

public:
	static void Add(int joystickIndex);
	static void Remove(SDL_JoystickID instanceId);
	static GameController *Get(SDL_JoystickID instanceId);
	static GameController *Get(const SDL_Event &event);
	static const std::vector<GameController> &All();
	static bool IsPressedOnAnyController(ControllerButton button);

	// NOTE: Not idempotent.
	// Must be called exactly once for each SDL input event.
	ControllerButton ToControllerButton(const SDL_Event &event);

	bool IsPressed(ControllerButton button) const;
	static bool ProcessAxisMotion(const SDL_Event &event);

private:
	static SDL_GameControllerButton ToSdlGameControllerButton(ControllerButton button);

	SDL_GameController *sdl_game_controller_ = NULL;
	SDL_JoystickID instance_id_ = -1;

	bool trigger_left_is_down_ = false;
	bool trigger_right_is_down_ = false;
};

} // namespace devilution
#endif
