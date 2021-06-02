#pragma once

#include <algorithm>

namespace devilution {

template <typename T>
constexpr T abs(const T &a)
{
#if defined(__GNUC__) || defined(__GNUG__) || defined(_MSC_VER)
	return std::abs(a);
#else
	return (a < 0) ? -a : a;
#endif
}

}
