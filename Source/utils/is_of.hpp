#pragma once

#include "utils/attributes.h"

namespace devilution {

template <typename V, typename X, typename... Xs>
DVL_ALWAYS_INLINE constexpr bool IsAnyOf(const V &v, X x, Xs... xs)
{
	return v == x || ((v == xs) || ...);
}

template <typename V, typename X, typename... Xs>
DVL_ALWAYS_INLINE constexpr bool IsNoneOf(const V &v, X x, Xs... xs)
{
	return v != x && ((v != xs) && ...);
}

} // namespace devilution
