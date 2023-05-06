#pragma once

#include <string>
#include <string_view> // IWYU pragma: export
namespace devilution {
using ::std::string_view;

inline void AppendStrView(std::string &out, string_view str)
{
	out.append(str);
}
} // namespace devilution
