#include "engine/trn.hpp"

#include <cstdint>

#include <fmt/format.h>

#ifdef _DEBUG
#include "debug.h"
#endif
#include "engine/load_file.hpp"
#include "lighting.h"

namespace devilution {

uint8_t *GetInfravisionTRN()
{
	return InfravisionTable.data();
}

uint8_t *GetStoneTRN()
{
	return StoneTable.data();
}

uint8_t *GetPauseTRN()
{
	return PauseTable.data();
}

std::optional<std::array<uint8_t, 256>> GetClassTRN(Player &player)
{
	std::array<uint8_t, 256> trn;
	const char *path;

	switch (player._pClass) {
	case HeroClass::Warrior:
		path = "plrgfx\\warrior.trn";
		break;
	case HeroClass::Rogue:
		path = "plrgfx\\rogue.trn";
		break;
	case HeroClass::Sorcerer:
		path = "plrgfx\\sorcerer.trn";
		break;
	case HeroClass::Monk:
		path = "plrgfx\\monk.trn";
		break;
	case HeroClass::Bard:
		path = "plrgfx\\bard.trn";
		break;
	case HeroClass::Barbarian:
		path = "plrgfx\\barbarian.trn";
		break;
	}

#ifdef _DEBUG
	if (!debugTRN.empty()) {
		path = debugTRN.c_str();
	}
#endif
	if (LoadOptionalFileInMem(path, &trn[0], 256)) {
		return trn;
	}
	return std::nullopt;
}

std::optional<std::array<uint8_t, 256>> GetPlayerGraphicTRN(const char *pszName)
{
	char path[MaxMpqPathSize];
	*BufCopy(path, pszName, ".trn") = '\0';

	std::array<uint8_t, 256> trn;
	if (LoadOptionalFileInMem(path, &trn[0], 256)) {
		return trn;
	}
	return std::nullopt;
}

} // namespace devilution
