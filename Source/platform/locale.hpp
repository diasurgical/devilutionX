#pragma once

#include <string>
#include <vector>

namespace devilution {

/**
 * @brief Returns a list of preferred languages based on user/system configuration.
 * @return 0 or more POSIX locale codes (language code with optional region identifier)
 */
std::vector<std::string> GetLocales();

} // namespace devilution
