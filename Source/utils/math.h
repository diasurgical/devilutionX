/**
* @file math.h
*
* Math utility functions
*/
#pragma once

namespace devilution {
namespace math {

/**
 * @brief Compute sign of t
 * @tparam T Any arithmetic type
 * @param t Value to compute sign of
 * @return -1 if t < 0, 1 if t > 0, 0 if t == 0
*/
template<typename T>
int Sign(T t)
{
	return (t > T(0)) - (t < T(0));
}

} // namespace math
} // namespace devilution
