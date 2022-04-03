#include "engine/load_cel.hpp"

#include "engine/load_file.hpp"

namespace devilution {

OwnedCelSprite LoadCel(const char *pszName, uint16_t width)
{
	return OwnedCelSprite(LoadFileInMem(pszName), width);
}

OwnedCelSprite LoadCel(const char *pszName, const uint16_t *widths)
{
	return OwnedCelSprite(LoadFileInMem(pszName), widths);
}

} // namespace devilution
