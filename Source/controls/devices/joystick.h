#pragma once
// Joystick mappings for SDL1 and additional buttons on SDL2.

#include <vector>

#include <SDL.h>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#include "controls/controller.h"
#include "controls/controller_buttons.h"

namespace devilution {

class Joystick {
	static std::vector<Joystick> joysticks_;

public:
	static void Add(int deviceIndex);
	static void Remove(SDL_JoystickID instanceId);
	static Joystick *Get(SDL_JoystickID instanceId);
	static Joystick *Get(const SDL_Event &event);
	static const std::vector<Joystick> &All();
	static bool IsPressedOnAnyJoystick(ControllerButton button);

	static ControllerButton ToControllerButton(const SDL_Event &event);
	bool IsPressed(ControllerButton button) const;
	static bool ProcessAxisMotion(const SDL_Event &event);

	SDL_JoystickID instance_id() const
	{
		return instance_id_;
	}

private:
	static int ToSdlJoyButton(ControllerButton button);

	bool IsHatButtonPressed(ControllerButton button) const;

	SDL_Joystick *sdl_joystick_ = NULL;
	SDL_JoystickID instance_id_ = -1;
};

} // namespace devilution
