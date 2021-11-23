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
template <typename T>
int Sign(T t)
{
	return (t > T(0)) - (t < T(0));
}

/**
 * @brief Linearly interpolate from a towards b using mixing value t
 * @tparam V Any arithmetic type, used for interpolants and return value
 * @tparam T Any arithmetic type, used for interpolator
 * @param a Low interpolation value (returned when t == 0)
 * @param b High interpolation value (returned when t == 1)
 * @param t Interpolator, commonly in range [0..1], values outside this range will extrapolate
 * @return a + (b - a) * t
 */
template <typename V, typename T>
V Lerp(V a, V b, T t)
{
	return a + (b - a) * t;
}

/**
 * @brief Inverse lerp, given two key values a and b, and a free value v, determine mixing factor t so that v = Lerp(a, b, t)
 * @tparam T Any arithmetic type
 * @param a Low key value (function returns 0 if v == a)
 * @param b High key value (function returns 1 if v == b)
 * @param v Mixing factor, commonly in range [a..b] to get a return [0..1]
 * @return Value t so that v = Lerp(a, b, t); or 0 if b == a
 */
template <typename T>
T InvLerp(T a, T b, T v)
{
	if (b == a)
		return T(0);

	return (v - a) / (b - a);
}

/**
 * @brief Remaps value v from range [inMin, inMax] to [outMin, outMax]
 * @tparam T Any arithmetic type
 * @param inMin First bound of input range
 * @param inMax Second bound of input range
 * @param outMin First bound of output range
 * @param outMax Second bound of output range
 * @param v Value to remap
 * @return Transformed value so that InvLerp(inMin, inMax, v) == InvLerp(outMin, outMax, return)
 */
template <typename T>
T Remap(T inMin, T inMax, T outMin, T outMax, T v)
{
	auto t = InvLerp(inMin, inMax, v);
	return Lerp(outMin, outMax, t);
}

} // namespace math
} // namespace devilution
