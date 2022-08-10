#include <fmt/format.h>
#include <unordered_map>

#ifdef _DEBUG
#include "debug.h"
#endif
#include "engine/load_file.hpp"
#include "engine/trn.hpp"
#include "lighting.h"

namespace devilution {

uint8_t *GetInfravisionTRN()
{
	return &LightTables[16 * 256];
}

uint8_t *GetStoneTRN()
{
	return &LightTables[17 * 256];
}

uint8_t *GetPauseTRN()
{
	return &LightTables[18 * 256];
}

std::optional<std::array<uint8_t, 256>> GetClassTRN(Player &player)
{
	std::array<uint8_t, 256> trn;
	const char *path;

	switch (player._pClass) {
	case HeroClass::Warrior:
		path = "PlrGFX\\warrior.TRN";
		break;
	case HeroClass::Rogue:
		path = "PlrGFX\\rogue.TRN";
		break;
	case HeroClass::Sorcerer:
		path = "PlrGFX\\sorcerer.TRN";
		break;
	case HeroClass::Monk:
		path = "PlrGFX\\monk.TRN";
		break;
	case HeroClass::Bard:
		path = "PlrGFX\\bard.TRN";
		break;
	case HeroClass::Barbarian:
		path = "PlrGFX\\barbarian.TRN";
		break;
	}

#ifdef _DEBUG
	if (!debugTRN.empty()) {
		path = debugTRN.c_str();
	}
#endif
	if (LoadOptionalFileInMem(path, &trn, 256)) {
		return trn;
	}
	return std::nullopt;
}

} // namespace devilution
