#pragma once

#include <string>

#include "utils/stdcompat/optional.hpp"

namespace devilution {

namespace paths {

const std::string &BasePath();
const std::string &PrefPath();
const std::string &ConfigPath();
const std::string &AssetsPath();
const std::optional<std::string> &MpqDir();

void SetBasePath(const std::string &path);
void SetPrefPath(const std::string &path);
void SetConfigPath(const std::string &path);
void SetAssetsPath(const std::string &path);
void SetMpqDir(const std::string &path);

} // namespace paths

} // namespace devilution
