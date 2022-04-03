#include <fmt/format.h>
#include <unordered_map>

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

} // namespace devilution
