#pragma once

namespace devilution {

bool SVidPlayBegin(const char *filename, int flags);
bool SVidPlayContinue();
void SVidPlayEnd();

} // namespace devilution
