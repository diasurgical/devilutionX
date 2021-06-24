#pragma once

#include <algorithm> // IWYU pragma: export

namespace devilution {
#if defined(__cplusplus) && __cplusplus >= 201703L
using std::clamp; // NOLINT(misc-unused-using-decls)
#else
template <typename T>
constexpr const T &clamp(const T &x, const T &lower, const T &upper)
{
	return std::min(std::max(x, lower), upper);
}
#endif
} // namespace devilution
