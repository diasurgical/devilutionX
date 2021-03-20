#pragma once

namespace dvl {

enum AxisDirectionX {
	AxisDirectionX_NONE = 0,
	AxisDirectionX_LEFT,
	AxisDirectionX_RIGHT
};
enum AxisDirectionY {
	AxisDirectionY_NONE = 0,
	AxisDirectionY_UP,
	AxisDirectionY_DOWN
};

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

} // namespace dvl
