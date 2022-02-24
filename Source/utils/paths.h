#pragma once

#include <string>
#include <vector>

#include "utils/stdcompat/optional.hpp"

namespace devilution {

namespace paths {

const std::string &BasePath();
const std::string &PrefPath();
const std::string &ConfigPath();
const std::string &AssetsPath();
const std::vector<std::string> &GetMPQSearchPaths();

void SetBasePath(const std::string &path);
void SetPrefPath(const std::string &path);
void SetConfigPath(const std::string &path);
void SetAssetsPath(const std::string &path);
void SetMPQSearchPaths(const std::vector<std::string> &paths);

} // namespace paths

} // namespace devilution
