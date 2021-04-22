#pragma once

#include "miniwin/miniwin.h"

namespace devilution {

void SVidPlayBegin(const char *filename, int flags, HANDLE *video);
bool SVidPlayContinue();
void SVidPlayEnd(HANDLE video);

} // namespace devilution
