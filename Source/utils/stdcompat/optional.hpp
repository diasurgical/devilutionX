#pragma once

#ifdef __has_include
#if defined(__cplusplus) && (__cplusplus >= 201606L || _MSC_VER >= 1930) && __has_include(<optional>)
#include <optional> // IWYU pragma: export
#elif __has_include(<experimental/optional>)
#include <experimental/optional> // IWYU pragma: export
#define optional experimental::optional
#define nullopt experimental::nullopt
#define nullopt_t experimental::nullopt_t
#else
#error "Missing support for <optional> or <experimental/optional>"
#endif
#else
#error "__has_include unavailable"
#endif
