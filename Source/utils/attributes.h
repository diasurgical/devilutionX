/**
 * @file attributes.h
 *
 * Macros for attributes on functions, variables, etc.
 */
#pragma once

#ifdef __has_attribute
#define DVL_HAVE_ATTRIBUTE(x) __has_attribute(x)
#else
#define DVL_HAVE_ATTRIBUTE(x) 0
#endif

#ifdef __has_builtin
#define DVL_HAVE_BUILTIN(x) __has_builtin(x)
#else
#define DVL_HAVE_BUILTIN(x) 0
#endif

#if DVL_HAVE_ATTRIBUTE(format) || (defined(__GNUC__) && !defined(__clang__))
#define DVL_PRINTF_ATTRIBUTE(fmtargnum, firstarg) \
	__attribute__((__format__(__printf__, fmtargnum, firstarg)))
#else
#define DVL_PRINTF_ATTRIBUTE(fmtargnum, firstarg)
#endif

#if DVL_HAVE_ATTRIBUTE(always_inline)
#define DVL_ALWAYS_INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#define DVL_ALWAYS_INLINE __forceinline
#else
#define DVL_ALWAYS_INLINE inline
#endif

#if DVL_HAVE_ATTRIBUTE(hot)
#define DVL_ATTRIBUTE_HOT __attribute__((hot))
#else
#define DVL_ATTRIBUTE_HOT
#endif

#if DVL_HAVE_BUILTIN(__builtin_unreachable)
#define DVL_UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#define DVL_UNREACHABLE __assume(0)
#else
#define DVL_UNREACHABLE
#endif

// Any global data used by tests must be marked with `DVL_API_FOR_TEST`.
#if defined(_MSC_VER) && defined(BUILD_TESTING)
#ifdef _DVL_EXPORTING
#define DVL_API_FOR_TEST __declspec(dllexport)
#else
#define DVL_API_FOR_TEST __declspec(dllimport)
#endif
#else
#define DVL_API_FOR_TEST
#endif
