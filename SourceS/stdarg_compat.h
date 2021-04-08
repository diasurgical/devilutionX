#pragma once

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__)
#include <stdarg.h>
#else
// work around https://reviews.llvm.org/D51265
typedef __builtin_va_list va_list;
#define _VA_LIST_T
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap) __builtin_va_end(ap)
#endif
