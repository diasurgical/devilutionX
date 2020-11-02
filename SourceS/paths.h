#pragma once

#include <string>

namespace dvl {

const std::string &GetBasePath();
const std::string &GetPrefPath();

void SetBasePath(const char *path);
void SetPrefPath(const char *path);

} // namespace dvl
