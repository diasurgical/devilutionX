#ifdef _DEBUG
#include "lua/autocomplete.hpp"

#include <algorithm>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include <sol/sol.hpp>

#include "utils/algorithm/container.hpp"

namespace devilution {

namespace {

std::string_view GetLastToken(std::string_view text)
{
	if (text.empty())
		return {};
	size_t i = text.size();
	while (i > 0 && text[i - 1] != ' ')
		--i;
	return text.substr(i);
}

bool IsCallable(const sol::object &value)
{
	if (value.get_type() == sol::type::function)
		return true;
	if (!value.is<sol::table>())
		return false;
	const auto table = value.as<sol::table>();
	const auto metatable = table.get<std::optional<sol::object>>(sol::metatable_key);
	if (!metatable || !metatable->is<sol::table>())
		return false;
	const auto callFn = metatable->as<sol::table>().get<std::optional<sol::object>>(sol::meta_function::call);
	return callFn.has_value();
}

void SuggestionsFromTable(const sol::table &table, std::string_view prefix,
    size_t maxSuggestions, std::unordered_set<LuaAutocompleteSuggestion> &out)
{
	for (const auto &[key, value] : table) {
		if (key.get_type() == sol::type::string) {
			std::string keyStr = key.as<std::string>();
			if (!keyStr.starts_with(prefix) || keyStr.size() == prefix.size())
				continue;
			if (keyStr.starts_with("__") && !prefix.starts_with("__"))
				continue;
			// sol-internal keys -- we don't have fonts for these so skip them.
			if (keyStr.find("♻") != std::string::npos
			    || keyStr.find("☢") != std::string::npos
			    || keyStr.find("🔩") != std::string::npos)
				continue;
			std::string completionText = keyStr.substr(prefix.size());
			LuaAutocompleteSuggestion suggestion { std::move(keyStr), std::move(completionText) };
			if (IsCallable(value)) {
				suggestion.completionText.append("()");
				suggestion.cursorAdjust = -1;
			}
			out.insert(std::move(suggestion));
			if (out.size() == maxSuggestions)
				break;
		}
	}
	const auto fallback = table.get<std::optional<sol::object>>(sol::metatable_key);
	if (fallback.has_value() && fallback->get_type() == sol::type::table) {
		SuggestionsFromTable(fallback->as<sol::table>(), prefix, maxSuggestions, out);
	}
}

} // namespace

void GetLuaAutocompleteSuggestions(std::string_view text, const sol::environment &lua,
    size_t maxSuggestions, std::vector<LuaAutocompleteSuggestion> &out)
{
	out.clear();
	if (text.empty())
		return;
	std::string_view token = GetLastToken(text);
	const size_t dotPos = token.rfind('.');
	const std::string_view prefix = token.substr(dotPos + 1);
	token.remove_suffix(token.size() - (dotPos == std::string_view::npos ? 0 : dotPos));

	std::unordered_set<LuaAutocompleteSuggestion> suggestions;
	const auto addSuggestions = [&](const sol::table &table) {
		SuggestionsFromTable(table, prefix, maxSuggestions, suggestions);
	};

	if (token.empty()) {
		addSuggestions(lua);
		const auto fallback = lua.get<std::optional<sol::object>>("_G");
		if (fallback.has_value() && fallback->get_type() == sol::type::table) {
			addSuggestions(fallback->as<sol::table>());
		}
	} else {
		const auto obj = lua.traverse_get<std::optional<sol::object>>(token);
		if (!obj.has_value())
			return;
		if (obj->get_type() == sol::type::table) {
			addSuggestions(obj->as<sol::table>());
		}
	}

	out.insert(out.end(), suggestions.begin(), suggestions.end());
	c_sort(out);
}

} // namespace devilution
#endif // _DEBUG
