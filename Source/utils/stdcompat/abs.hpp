#pragma once

#include <cstdlib>

namespace devilution {

template <typename T>
constexpr T abs(const T &a)
{
#if defined(_MSC_VER)
	return std::abs(a);
#else
	return (a < 0) ? -a : a;
#endif
}

} // namespace devilution
