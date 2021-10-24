#pragma once

#include <string>

namespace devilution {

namespace paths {

const std::string &AppPath();
const std::string &BasePath();
const std::string &PrefPath();
const std::string &ConfigPath();

void SetBasePath(const std::string &path);
void SetPrefPath(const std::string &path);
void SetConfigPath(const std::string &path);

} // namespace paths

} // namespace devilution
