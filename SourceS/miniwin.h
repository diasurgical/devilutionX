#pragma once

#include <ctype.h>
#include <math.h>
// work around https://reviews.llvm.org/D51265
#if defined(__APPLE__) || defined(__FreeBSD__)
#include "macos_stdarg.h"
#else
#include <stdarg.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// For _rotr()
// _rotl and _rotr are predeclared in Clang.
#if !defined(_MSC_VER) && !defined(__clang__) && defined(DEVILUTION_ENGINE)
#if defined(__x86_64__) || defined(__i386__)
#include <x86intrin.h>
#else
unsigned int _rotl(unsigned int value, int shift);
unsigned int _rotr(unsigned int value, int shift);
#endif
#endif

#ifndef _WIN32
#define __int8 char
#define __int16 short
#define __int32 int
#define __int64 long long __attribute__((aligned(8)))
#endif

#include "miniwin/misc.h"
#include "miniwin/com.h"
#include "miniwin/ui.h"
#include "miniwin/thread.h"
#include "miniwin/rand.h"
#include "storm_full.h"

#define MAX_PATH DVL_MAX_PATH
#include "miniwin/misc_macro.h"
#include "miniwin/com_macro.h"
