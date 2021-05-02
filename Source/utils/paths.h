#pragma once

#include <string>

namespace devilution {

namespace paths {

const std::string &BasePath();
const std::string &PrefPath();
const std::string &ConfigPath();
const std::string &LangPath();
const std::string &TtfPath();
const std::string &TtfName();

void SetBasePath(const std::string &path);
void SetPrefPath(const std::string &path);
void SetConfigPath(const std::string &path);
void SetLangPath(const std::string &path);
void SetTtfPath(const std::string &path);
void SetTtfName(const std::string &name);

} // namespace paths

} // namespace devilution
