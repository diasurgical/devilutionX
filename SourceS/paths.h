#pragma once

#include <string>

namespace dvl {

const std::string &GetBasePath();
const std::string &GetPrefPath();
const std::string &GetConfigPath();
const std::string &GetTtfPath();
const std::string &GetTtfName();

void SetBasePath(const char *path);
void SetPrefPath(const char *path);
void SetConfigPath(const char *path);
void SetTtfPath(const char *path);
void SetTtfName(const char *path);

} // namespace dvl
