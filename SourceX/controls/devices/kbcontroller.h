#pragma once

// Keyboard keys acting like gamepad buttons
#ifndef HAS_KBCTRL
#define HAS_KBCTRL 0
#endif

#if HAS_KBCTRL == 1
#include <SDL.h>
#include "controls/controller.h"
#include "controls/controller_buttons.h"

namespace dvl {

class KeyboardController : public Controller {
	static std::vector<KeyboardController> *const keyboardControllers_;

public:
	static void Add(int device_index);
	static KeyboardController *Get(const SDL_Event &event);

	ControllerButton ToControllerButton(const SDL_Event &event) const;

	bool ProcessAxisMotion(const SDL_Event &event);
};

bool IsKbCtrlButtonPressed(ControllerButton button);

} // namespace dvl
#endif
