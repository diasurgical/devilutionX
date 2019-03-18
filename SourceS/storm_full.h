#pragma once

namespace dvl {
extern "C" {

extern DWORD nLastError;

void TranslateFileName(char *dst, int dstLen, const char *src);

}
} // namespace dvl
