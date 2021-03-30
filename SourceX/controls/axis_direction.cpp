#include "axis_direction.h"

#include <SDL.h>

namespace dvl {

AxisDirection AxisDirectionRepeater::Get(AxisDirection axis_direction)
{
	const int now = SDL_GetTicks();
	switch (axis_direction.x) {
	case AxisDirectionX_LEFT:
		last_right_ = 0;
		if (now - last_left_ < min_interval_ms_) {
			axis_direction.x = AxisDirectionX_NONE;
		} else {
			last_left_ = now;
		}
		break;
	case AxisDirectionX_RIGHT:
		last_left_ = 0;
		if (now - last_right_ < min_interval_ms_) {
			axis_direction.x = AxisDirectionX_NONE;
		} else {
			last_right_ = now;
		}
		break;
	case AxisDirectionX_NONE:
		last_left_ = last_right_ = 0;
		break;
	}
	switch (axis_direction.y) {
	case AxisDirectionY_UP:
		last_down_ = 0;
		if (now - last_up_ < min_interval_ms_) {
			axis_direction.y = AxisDirectionY_NONE;
		} else {
			last_up_ = now;
		}
		break;
	case AxisDirectionY_DOWN:
		last_up_ = 0;
		if (now - last_down_ < min_interval_ms_) {
			axis_direction.y = AxisDirectionY_NONE;
		} else {
			last_down_ = now;
		}
		break;
	case AxisDirectionY_NONE:
		last_up_ = last_down_ = 0;
		break;
	}
	return axis_direction;
}

} // namespace dvl
