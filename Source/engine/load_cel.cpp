#include "engine/load_cel.hpp"

#ifdef DEBUG_CEL_TO_CL2_SIZE
#include <iostream>
#endif

#include "engine/load_file.hpp"
#include "utils/cel_to_cl2.hpp"
#include "utils/pointer_value_union.hpp"

namespace devilution {

OwnedCelSprite LoadCelAsCl2(const char *pszName, uint16_t width)
{
	size_t size;
	std::unique_ptr<uint8_t[]> data = LoadFileInMem<uint8_t>(pszName, &size);
#ifdef DEBUG_CEL_TO_CL2_SIZE
	std::cout << pszName;
#endif
	return CelToCl2(data.get(), size, PointerOrValue<uint16_t> { width });
}

OwnedCelSprite LoadCelAsCl2(const char *pszName, const uint16_t *widths)
{
	size_t size;
	std::unique_ptr<uint8_t[]> data = LoadFileInMem<uint8_t>(pszName, &size);
#ifdef DEBUG_CEL_TO_CL2_SIZE
	std::cout << pszName;
#endif
	return CelToCl2(data.get(), size, PointerOrValue<uint16_t> { widths });
}

} // namespace devilution
