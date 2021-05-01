#pragma once

#ifdef __has_include
#if defined(__cplusplus) && __cplusplus >= 201703L && __has_include(<string_view>)
#include <string_view> // IWYU pragma: export
#elif __has_include(<experimental/string_view>)
#include <experimental/string_view> // IWYU pragma: export
#define string_view experimental::string_view
#else
#error "Missing support for <string_view> or <experimental/string_view>"
#endif
#else
#error "__has_include unavailable"
#endif
