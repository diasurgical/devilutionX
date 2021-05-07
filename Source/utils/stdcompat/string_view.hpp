#pragma once

#ifdef __has_include
#if defined(__cplusplus) && __cplusplus >= 201703L && __has_include(<string_view>) // should be 201606L, but STL headers disagree
#include <string_view> // IWYU pragma: export
namespace devilution {
	using string_view = std::string_view;
}
#elif __has_include(<experimental/string_view>)
#include <experimental/string_view> // IWYU pragma: export
namespace devilution {
	using string_view = std::experimental::string_view;
}
#else
#error "Missing support for <string_view> or <experimental/string_view>"
#endif
#else
#error "__has_include unavailable"
#endif
