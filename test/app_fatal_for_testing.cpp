#include <iostream>
#include <string_view>

namespace devilution {

[[noreturn]] void app_fatal(std::string_view str)
{
	std::cerr << "app_fatal: " << str << std::endl;
	std::abort();
}

} // namespace devilution
