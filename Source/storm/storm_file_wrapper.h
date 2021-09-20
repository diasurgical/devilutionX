/** A pointer to a Storm file as a `FILE *`. Only available on some platforms. */
#pragma once
#if defined(__linux__) || defined(__FreeBSD__) || defined(__DragonFly__) || defined(__HAIKU__)
#include <cstdio>

#include "miniwin/miniwin.h"

#define DEVILUTIONX_STORM_FILE_WRAPPER_AVAILABLE
#define DEVILUTIONX_STORM_FILE_WRAPPER_IMPL_FOPENCOOKIE

namespace devilution {

FILE *FILE_FromStormHandle(HANDLE handle);

} // namespace devilution

#endif
