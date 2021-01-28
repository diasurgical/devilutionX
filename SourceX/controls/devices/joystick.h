#pragma once
// Joystick mappings for SDL1 and additional buttons on SDL2.

#include <vector>

#include <SDL.h>

#ifdef USE_SDL1
#include "sdl2_to_1_2_backports.h"
#endif

#include "controls/controller_buttons.h"

namespace dvl {

class Joystick {
	static std::vector<Joystick> *const joysticks_;

public:
	static void Add(int device_index);
	static void Remove(SDL_JoystickID instance_id);
	static Joystick *Get(SDL_JoystickID instance_id);
	static Joystick *Get(const SDL_Event &event);
	static const std::vector<Joystick> &All();
	static bool IsPressedOnAnyJoystick(ControllerButton button);

	ControllerButton ToControllerButton(const SDL_Event &event) const;
	bool IsPressed(ControllerButton button) const;
	bool ProcessAxisMotion(const SDL_Event &event);

	SDL_JoystickID instance_id() const
	{
		return instance_id_;
	}

private:
	int ToSdlJoyButton(ControllerButton button) const;
	bool IsHatButtonPressed(ControllerButton button) const;

	SDL_Joystick *sdl_joystick_ = NULL;
	SDL_JoystickID instance_id_ = -1;
};

} // namespace dvl
