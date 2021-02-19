#pragma once

#include <string>

namespace dvl {

const std::string &GetBasePath();
const std::string &GetPrefPath();
const std::string &GetConfigPath();

void SetBasePath(const char *path);
void SetPrefPath(const char *path);
void SetConfigPath(const char *path);

} // namespace dvl
