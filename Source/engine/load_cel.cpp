#include "engine/load_cel.hpp"

#ifdef DEBUG_CEL_TO_CL2_SIZE
#include <iostream>
#endif

#include "engine/load_file.hpp"
#include "utils/cel_to_clx.hpp"

namespace devilution {

OwnedClxSpriteListOrSheet LoadCelListOrSheet(const char *pszName, PointerOrValue<uint16_t> widthOrWidths)
{
	size_t size;
	std::unique_ptr<uint8_t[]> data = LoadFileInMem<uint8_t>(pszName, &size);
#ifdef DEBUG_CEL_TO_CL2_SIZE
	std::cout << pszName;
#endif
	return CelToClx(data.get(), size, widthOrWidths);
}

} // namespace devilution
