#pragma once

#include <type_traits>

namespace devilution {

#if defined(__cplusplus) && __cplusplus >= 201703L
using std::invoke_result_t;
#else
template <typename F, typename... Args>
using invoke_result_t = typename std::result_of<F(Args...)>::type;
#endif

} // namespace devilution
