#include "lua/modules/utils.hpp"

#include <ctime>

#include <sol/sol.hpp>

namespace devilution {

namespace {

std::string_view LuaDate(std::string_view format)
{
	time_t now = time(NULL);
	struct tm *timeinfo = localtime(&now);
	static char buf[75] {};
	if (strftime(buf, sizeof(buf), format.data(), timeinfo) > 0) {
		return std::string_view(buf);
	} else {
		return std::string_view("");
	}
}

} // namespace

sol::table LuaUtilsModule(sol::state_view &lua)
{
	return lua.create_table_with(
	    "date", LuaDate);
}

} // namespace devilution
