#include "engine/load_cl2.hpp"

#include <cstdint>
#include <memory>
#include <utility>

#include "mpq/mpq_common.hpp"
#include "utils/str_cat.hpp"

#ifdef UNPACKED_MPQS
#include "engine/load_clx.hpp"
#else
#include "engine/load_file.hpp"
#include "utils/cl2_to_clx.hpp"
#endif

namespace devilution {

OwnedClxSpriteListOrSheet LoadCl2ListOrSheet(const char *pszName, PointerOrValue<uint16_t> widthOrWidths)
{
	char path[MaxMpqPathSize];
	*BufCopy(path, pszName, DEVILUTIONX_CL2_EXT) = '\0';
#ifdef UNPACKED_MPQS
	return LoadClxListOrSheet(path);
#else
	size_t size;
	std::unique_ptr<uint8_t[]> data = LoadFileInMem<uint8_t>(path, &size);
	return Cl2ToClx(std::move(data), size, widthOrWidths);
#endif
}

} // namespace devilution
