
#pragma once

#include <vector>

#include <SDL.h>

#include "controls/controller_buttons.h"

#ifndef USE_SDL1
namespace dvl {

class GameController {
	static std::vector<GameController> *const controllers_;

public:
	static void Add(int joystick_index);
	static void Remove(SDL_JoystickID instance_id);
	static GameController *Get(SDL_JoystickID instance_id);
	static GameController *Get(const SDL_Event &event);
	static const std::vector<GameController> &All();
	static bool IsPressedOnAnyController(ControllerButton button);

	// NOTE: Not idempotent.
	// Must be called exactly once for each SDL input event.
	ControllerButton ToControllerButton(const SDL_Event &event);

	bool IsPressed(ControllerButton button) const;
	bool ProcessAxisMotion(const SDL_Event &event);

private:
	SDL_GameControllerButton ToSdlGameControllerButton(ControllerButton button) const;

	SDL_GameController *sdl_game_controller_ = NULL;
	SDL_JoystickID instance_id_ = -1;

	bool trigger_left_is_down_ = false;
	bool trigger_right_is_down_ = false;
};

} // namespace dvl
#endif
