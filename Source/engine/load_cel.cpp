#include "engine/load_cel.hpp"

#include "engine/load_file.hpp"

namespace devilution {

OwnedCelSprite LoadCel(const char *pszName, int width)
{
	return OwnedCelSprite(LoadFileInMem(pszName), width);
}

OwnedCelSprite LoadCel(const char *pszName, const int *widths)
{
	return OwnedCelSprite(LoadFileInMem(pszName), widths);
}

} // namespace devilution
