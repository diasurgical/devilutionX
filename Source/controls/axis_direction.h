#pragma once

#include <stdint.h>
#include <string_view>
namespace devilution {

enum AxisDirectionX : uint8_t {
	AxisDirectionX_NONE,
	AxisDirectionX_LEFT,
	AxisDirectionX_RIGHT,
};

[[maybe_unused]] constexpr std::string_view toString(AxisDirectionX value)
{
	switch(value) {
	case AxisDirectionX_NONE:
		return "None";
	case AxisDirectionX_LEFT:
		return "Left";
	case AxisDirectionX_RIGHT:
		return "Right";
	}
}

enum AxisDirectionY : uint8_t {
	AxisDirectionY_NONE,
	AxisDirectionY_UP,
	AxisDirectionY_DOWN,
};

[[maybe_unused]] constexpr std::string_view toString(AxisDirectionY value)
{
	switch(value) {
	case AxisDirectionY_NONE:
		return "None";
	case AxisDirectionY_UP:
		return "Up";
	case AxisDirectionY_DOWN:
		return "Down";
	}
}

/**
 * @brief 8-way direction of a D-Pad or a thumb stick.
 */
struct AxisDirection {
	AxisDirectionX x;
	AxisDirectionY y;
};

/**
 * @brief Returns a non-empty AxisDirection at most once per the given time interval.
 */
class AxisDirectionRepeater {
public:
	AxisDirectionRepeater(int min_interval_ms = 200)
	    : last_left_(0)
	    , last_right_(0)
	    , last_up_(0)
	    , last_down_(0)
	    , min_interval_ms_(min_interval_ms)
	{
	}

	AxisDirection Get(AxisDirection axis_direction);

private:
	int last_left_;
	int last_right_;
	int last_up_;
	int last_down_;
	int min_interval_ms_;
};

} // namespace devilution
