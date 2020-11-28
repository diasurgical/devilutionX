
#include "controls/devices/joystick.h"

#include <cstddef>

#include "controls/controller_motion.h"
#include "stubs.h"

namespace dvl {

std::vector<Joystick> *const Joystick::joysticks_ = new std::vector<Joystick>;

ControllerButton Joystick::ToControllerButton(const SDL_Event &event) const
{
	switch (event.type) {
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
		switch (event.jbutton.button) {
#ifdef JOY_BUTTON_A
		case JOY_BUTTON_A:
			return ControllerButton_BUTTON_A;
#endif
#ifdef JOY_BUTTON_B
		case JOY_BUTTON_B:
			return ControllerButton_BUTTON_B;
#endif
#ifdef JOY_BUTTON_X
		case JOY_BUTTON_X:
			return ControllerButton_BUTTON_X;
#endif
#ifdef JOY_BUTTON_Y
		case JOY_BUTTON_Y:
			return ControllerButton_BUTTON_Y;
#endif
#ifdef JOY_BUTTON_LEFTSTICK
		case JOY_BUTTON_LEFTSTICK:
			return ControllerButton_BUTTON_LEFTSTICK;
#endif
#ifdef JOY_BUTTON_RIGHTSTICK
		case JOY_BUTTON_RIGHTSTICK:
			return ControllerButton_BUTTON_RIGHTSTICK;
#endif
#ifdef JOY_BUTTON_LEFTSHOULDER
		case JOY_BUTTON_LEFTSHOULDER:
			return ControllerButton_BUTTON_LEFTSHOULDER;
#endif
#ifdef JOY_BUTTON_RIGHTSHOULDER
		case JOY_BUTTON_RIGHTSHOULDER:
			return ControllerButton_BUTTON_RIGHTSHOULDER;
#endif
#ifdef JOY_BUTTON_TRIGGERLEFT
		case JOY_BUTTON_TRIGGERLEFT:
			return ControllerButton_AXIS_TRIGGERLEFT;
#endif
#ifdef JOY_BUTTON_TRIGGERRIGHT
		case JOY_BUTTON_TRIGGERRIGHT:
			return ControllerButton_AXIS_TRIGGERRIGHT;
#endif
#ifdef JOY_BUTTON_START
		case JOY_BUTTON_START:
			return ControllerButton_BUTTON_START;
#endif
#ifdef JOY_BUTTON_BACK
		case JOY_BUTTON_BACK:
			return ControllerButton_BUTTON_BACK;
#endif
#ifdef JOY_BUTTON_DPAD_LEFT
		case JOY_BUTTON_DPAD_LEFT:
			return ControllerButton_BUTTON_DPAD_LEFT;
#endif
#ifdef JOY_BUTTON_DPAD_UP
		case JOY_BUTTON_DPAD_UP:
			return ControllerButton_BUTTON_DPAD_UP;
#endif
#ifdef JOY_BUTTON_DPAD_RIGHT
		case JOY_BUTTON_DPAD_RIGHT:
			return ControllerButton_BUTTON_DPAD_RIGHT;
#endif
#ifdef JOY_BUTTON_DPAD_DOWN
		case JOY_BUTTON_DPAD_DOWN:
			return ControllerButton_BUTTON_DPAD_DOWN;
#endif
		default:
			break;
		}
		break;
	case SDL_JOYHATMOTION:
#if defined(JOY_HAT_DPAD_UP_HAT) && defined(JOY_HAT_DPAD_UP)
		if (event.jhat.hat == JOY_HAT_DPAD_UP_HAT && (event.jhat.value & JOY_HAT_DPAD_UP) != 0)
			return ControllerButton_BUTTON_DPAD_UP;
#endif
#if defined(JOY_HAT_DPAD_DOWN_HAT) && defined(JOY_HAT_DPAD_DOWN)
		if (event.jhat.hat == JOY_HAT_DPAD_DOWN_HAT && (event.jhat.value & JOY_HAT_DPAD_DOWN) != 0)
			return ControllerButton_BUTTON_DPAD_DOWN;
#endif
#if defined(JOY_HAT_DPAD_LEFT_HAT) && defined(JOY_HAT_DPAD_LEFT)
		if (event.jhat.hat == JOY_HAT_DPAD_LEFT_HAT && (event.jhat.value & JOY_HAT_DPAD_LEFT) != 0)
			return ControllerButton_BUTTON_DPAD_LEFT;
#endif
#if defined(JOY_HAT_DPAD_RIGHT_HAT) && defined(JOY_HAT_DPAD_RIGHT)
		if (event.jhat.hat == JOY_HAT_DPAD_RIGHT_HAT && (event.jhat.value & JOY_HAT_DPAD_RIGHT) != 0)
			return ControllerButton_BUTTON_DPAD_RIGHT;
#endif
		return ControllerButton_IGNORE;
		break;
	default:
		break;
	}
	return ControllerButton_NONE;
}

int Joystick::ToSdlJoyButton(ControllerButton button) const
{
	if (button == ControllerButton_AXIS_TRIGGERLEFT || button == ControllerButton_AXIS_TRIGGERRIGHT)
		UNIMPLEMENTED();
	switch (button) {
#ifdef JOY_BUTTON_A
	case ControllerButton_BUTTON_A:
		return JOY_BUTTON_A;
#endif
#ifdef JOY_BUTTON_B
	case ControllerButton_BUTTON_B:
		return JOY_BUTTON_B;
#endif
#ifdef JOY_BUTTON_X
	case ControllerButton_BUTTON_X:
		return JOY_BUTTON_X;
#endif
#ifdef JOY_BUTTON_Y
	case ControllerButton_BUTTON_Y:
		return JOY_BUTTON_Y;
#endif
#ifdef JOY_BUTTON_BACK
	case ControllerButton_BUTTON_BACK:
		return JOY_BUTTON_BACK;
#endif
#ifdef JOY_BUTTON_START
	case ControllerButton_BUTTON_START:
		return JOY_BUTTON_START;
#endif
#ifdef JOY_BUTTON_LEFTSTICK
	case ControllerButton_BUTTON_LEFTSTICK:
		return JOY_BUTTON_LEFTSTICK;
#endif
#ifdef JOY_BUTTON_RIGHTSTICK
	case ControllerButton_BUTTON_RIGHTSTICK:
		return JOY_BUTTON_RIGHTSTICK;
#endif
#ifdef JOY_BUTTON_LEFTSHOULDER
	case ControllerButton_BUTTON_LEFTSHOULDER:
		return JOY_BUTTON_LEFTSHOULDER;
#endif
#ifdef JOY_BUTTON_RIGHTSHOULDER
	case ControllerButton_BUTTON_RIGHTSHOULDER:
		return JOY_BUTTON_RIGHTSHOULDER;
#endif
#ifdef JOY_BUTTON_DPAD_UP
	case ControllerButton_BUTTON_DPAD_UP:
		return JOY_BUTTON_DPAD_UP;
#endif
#ifdef JOY_BUTTON_DPAD_DOWN
	case ControllerButton_BUTTON_DPAD_DOWN:
		return JOY_BUTTON_DPAD_DOWN;
#endif
#ifdef JOY_BUTTON_DPAD_LEFT
	case ControllerButton_BUTTON_DPAD_LEFT:
		return JOY_BUTTON_DPAD_LEFT;
#endif
#ifdef JOY_BUTTON_DPAD_RIGHT
	case ControllerButton_BUTTON_DPAD_RIGHT:
		return JOY_BUTTON_DPAD_RIGHT;
#endif
	default:
		return -1;
	}
}

bool Joystick::IsHatButtonPressed(ControllerButton button) const
{
	switch (button) {
#if defined(JOY_HAT_DPAD_UP_HAT) && defined(JOY_HAT_DPAD_UP)
	case ControllerButton_BUTTON_DPAD_UP:
		return (SDL_JoystickGetHat(sdl_joystick_, JOY_HAT_DPAD_UP_HAT) & JOY_HAT_DPAD_UP) != 0;
#endif
#if defined(JOY_HAT_DPAD_DOWN_HAT) && defined(JOY_HAT_DPAD_DOWN)
	case ControllerButton_BUTTON_DPAD_DOWN:
		return (SDL_JoystickGetHat(sdl_joystick_, JOY_HAT_DPAD_DOWN_HAT) & JOY_HAT_DPAD_DOWN) != 0;
#endif
#if defined(JOY_HAT_DPAD_LEFT_HAT) && defined(JOY_HAT_DPAD_LEFT)
	case ControllerButton_BUTTON_DPAD_LEFT:
		return (SDL_JoystickGetHat(sdl_joystick_, JOY_HAT_DPAD_LEFT_HAT) & JOY_HAT_DPAD_LEFT) != 0;
#endif
#if defined(JOY_HAT_DPAD_RIGHT_HAT) && defined(JOY_HAT_DPAD_RIGHT)
	case ControllerButton_BUTTON_DPAD_RIGHT:
		return (SDL_JoystickGetHat(sdl_joystick_, JOY_HAT_DPAD_RIGHT_HAT) & JOY_HAT_DPAD_RIGHT) != 0;
#endif
	default:
		return false;
	}
}

bool Joystick::IsPressed(ControllerButton button) const
{
	if (sdl_joystick_ == NULL)
		return false;
	if (IsHatButtonPressed(button))
		return true;
	const int joy_button = ToSdlJoyButton(button);
	return joy_button != -1 && SDL_JoystickGetButton(sdl_joystick_, joy_button);
}

bool Joystick::ProcessAxisMotion(const SDL_Event &event)
{
	if (event.type != SDL_JOYAXISMOTION)
		return false;
	switch (event.jaxis.axis) {
#ifdef JOY_AXIS_LEFTX
	case JOY_AXIS_LEFTX:
		leftStickXUnscaled = event.jaxis.value;
		leftStickNeedsScaling = true;
		break;
#endif
#ifdef JOY_AXIS_LEFTY
	case JOY_AXIS_LEFTY:
		leftStickYUnscaled = -event.jaxis.value;
		leftStickNeedsScaling = true;
		break;
#endif
#ifdef JOY_AXIS_RIGHTX
	case JOY_AXIS_RIGHTX:
		rightStickXUnscaled = event.jaxis.value;
		rightStickNeedsScaling = true;
		break;
#endif
#ifdef JOY_AXIS_RIGHTY
	case JOY_AXIS_RIGHTY:
		rightStickYUnscaled = -event.jaxis.value;
		rightStickNeedsScaling = true;
		break;
#endif
	default:
		return false;
	}
	return true;
}

void Joystick::Add(int device_index)
{
	if (SDL_NumJoysticks() <= device_index)
		return;
	Joystick result;
	SDL_Log("Adding joystick %d: %s", device_index,
	    SDL_JoystickNameForIndex(device_index));
	result.sdl_joystick_ = SDL_JoystickOpen(device_index);
	if (result.sdl_joystick_ == NULL) {
		SDL_Log(SDL_GetError());
		SDL_ClearError();
		return;
	}
#ifndef USE_SDL1
	result.instance_id_ = SDL_JoystickInstanceID(result.sdl_joystick_);
#endif
	joysticks_->push_back(result);
	sgbControllerActive = true;
}

void Joystick::Remove(SDL_JoystickID instance_id)
{
#ifndef USE_SDL1
	SDL_Log("Removing joystick (instance id: %d)", instance_id);
	for (std::size_t i = 0; i < joysticks_->size(); ++i) {
		const Joystick &joystick = (*joysticks_)[i];
		if (joystick.instance_id_ != instance_id)
			continue;
		joysticks_->erase(joysticks_->begin() + i);
		sgbControllerActive = !joysticks_->empty();
		return;
	}
	SDL_Log("Joystick not found with instance id: %d", instance_id);
#endif
}

const std::vector<Joystick> &Joystick::All()
{
	return *joysticks_;
}

Joystick *Joystick::Get(SDL_JoystickID instance_id)
{
	for (std::size_t i = 0; i < joysticks_->size(); ++i) {
		Joystick &joystick = (*joysticks_)[i];
		if (joystick.instance_id_ == instance_id)
			return &joystick;
	}
	return NULL;
}

Joystick *Joystick::Get(const SDL_Event &event)
{
	switch (event.type) {
#ifndef USE_SDL1
		case SDL_JOYAXISMOTION:
			return Get(event.jaxis.which);
		case SDL_JOYBALLMOTION:
			return Get(event.jball.which);
		case SDL_JOYHATMOTION:
			return Get(event.jhat.which);
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			return Get(event.jbutton.which);
			return Get(event.jbutton.which);
		default:
			return NULL;
#else
		case SDL_JOYAXISMOTION:
		case SDL_JOYBALLMOTION:
		case SDL_JOYHATMOTION:
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			return joysticks_->empty() ? NULL : &(*joysticks_)[0];
		default:
			return NULL;
#endif
	}
}

bool Joystick::IsPressedOnAnyJoystick(ControllerButton button)
{
	for (std::size_t i = 0; i < joysticks_->size(); ++i)
		if ((*joysticks_)[i].IsPressed(button))
			return true;
	return false;
}

} // namespace dvl
