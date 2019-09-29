#pragma once

#include <string>

namespace dvl {
extern "C" {

extern DWORD nLastError;

void TranslateFileName(std::string *filename);
}
} // namespace dvl
