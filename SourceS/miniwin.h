#pragma once

#include <ctype.h>
#include <math.h>
// work around https://reviews.llvm.org/D51265
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__)
#include "macos_stdarg.h"
#else
#include <stdarg.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "miniwin/misc.h"
#include "storm_full.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif
