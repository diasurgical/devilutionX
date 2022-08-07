#include "engine/load_cl2.hpp"

#include <memory>
#include <utility>

#include "engine/load_file.hpp"
#include "utils/cl2_to_clx.hpp"

namespace devilution {

OwnedClxSpriteListOrSheet LoadCl2ListOrSheet(const char *pszName, PointerOrValue<uint16_t> widthOrWidths)
{
	size_t size;
	std::unique_ptr<uint8_t[]> data = LoadFileInMem<uint8_t>(pszName, &size);
	return Cl2ToClx(std::move(data), size, widthOrWidths);
}

} // namespace devilution
