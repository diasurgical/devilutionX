#if defined(_DEBUG) && defined(DEVILUTIONX_RESOURCE_TRACKING_ENABLED)
#include "lua/modules/dev/resources.hpp"

#include <string>
#include <string_view>
#include <variant>

#include <sol/sol.hpp>

#include "engine/clx_sprite.hpp"
#include "engine/resource_store.hpp"
#include "lua/metadoc.hpp"
#include "utils/algorithm/container.hpp"
#include "utils/format_int.hpp"
#include "utils/str_cat.hpp"

namespace devilution {
namespace {

struct ResourceInfo {
	[[nodiscard]] std::string operator()(const ClxSpriteList &list) const
	{
		if (list.numSprites() == 1) return "sprite";
		return StrCat("sprite list with ", list.numSprites(), " sprites");
	}
	[[nodiscard]] std::string operator()(const ClxSpriteSheet &sheet) const
	{
		return StrCat("sprite sheet with ", sheet.numLists(), " lists");
	}
	[[nodiscard]] std::string operator()(const OwnedClxSpriteListOrSheet *ptr) const
	{
		return ptr->isSheet() ? (*this)(ptr->sheet()) : (*this)(ptr->list());
	}
	[[nodiscard]] std::string operator()(const OwnedClxSpriteList *ptr) const
	{
		return (*this)(ClxSpriteList { *ptr });
	}
	[[nodiscard]] std::string operator()(const OwnedClxSpriteSheet *ptr) const
	{
		return (*this)(ClxSpriteSheet { *ptr });
	}
};

std::string DebugCmdListLoadedResources()
{
	size_t count = 0;
	size_t totalSize = 0;
	const auto &all = GetResourceStore().getAll();
	std::vector<std::string> entries;
	entries.reserve(all.size());
	for (const auto &[name, sprites] : all) {
		std::string &entry = entries.emplace_back();
		StrAppend(entry, name);
		for (const ResourceStoreEntry &sprite : sprites) {
			if (!sprite.variant.empty()) StrAppend(entry, " ", sprite.variant);
			StrAppend(entry, " (", std::visit(ResourceInfo {}, sprite.ptr), ")");
			const size_t size = sprite.dataSize();
			StrAppend(entry, " ", FormatInteger(static_cast<int>(size)));
			totalSize += sprite.dataSize();
			++count;
		}
	}
	c_sort(entries);
	std::string result;

	size_t i = 0;
	for (const std::string &entry : entries) {
		StrAppend(result, ++i, ". ", entry);
		result += '\n';
	}
	StrAppend(result, FormatInteger(static_cast<int>(count)), " resources, total size: ", FormatInteger(static_cast<int>(totalSize)), " bytes");
	return result;
}

} // namespace

sol::table LuaDevResourcesModule(sol::state_view &lua)
{
	sol::table table = lua.create_table();
	SetDocumented(table, "listLoaded", "()", "Information about the loaded resources.", &DebugCmdListLoadedResources);
	return table;
}

} // namespace devilution
#endif // defined(_DEBUG) && defined(DEVILUTIONX_RESOURCE_TRACKING_ENABLED)
