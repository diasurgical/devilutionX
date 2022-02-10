#include <fmt/format.h>
#include <unordered_map>

#include "engine/load_file.hpp"
#include "engine/trn.hpp"

namespace devilution {

namespace {

std::unordered_map<std::string, uint8_t *> TRNMap;

}

uint8_t *GetTRN(const char *path)
{
	if (TRNMap[path] == nullptr) {
		TRNMap[path] = LoadFileInMem<uint8_t>(path).release();
	}
	return TRNMap[path];
}

uint8_t *GetUniqueMonsterTRN(const char *path)
{
	return GetTRN(fmt::format(("Monsters\\Monsters\\{:s}.TRN"), path).c_str());
}

uint8_t *GetInfravisionTRN()
{
	return GetTRN("PlrGFX\\Infra.TRN");
}

uint8_t *GetStoneTRN()
{
	return GetTRN("PlrGFX\\Stone.TRN");
}

} // namespace devilution
