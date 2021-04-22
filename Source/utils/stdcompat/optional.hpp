#pragma once

#ifdef __has_include
#if defined(__cplusplus) && __cplusplus >= 201606L && __has_include(<optional>)
#include <optional> // IWYU pragma: export
#elif __has_include(<experimental/optional>)
#include <experimental/optional> // IWYU pragma: export
#define optional experimental::optional
#define nullopt experimental::nullopt
#else
#error "Missing support for <optional> or <experimental/optional>"
#endif
#else
#error "__has_include unavailable"
#endif
