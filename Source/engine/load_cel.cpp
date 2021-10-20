#include "engine/load_cel.hpp"

#include "engine/load_file.hpp"

namespace devilution {

CelSprite LoadCel(const char *pszName, int width)
{
	return CelSprite(LoadFileInMem(pszName), width);
}

CelSprite LoadCel(const char *pszName, const int *widths)
{
	return CelSprite(LoadFileInMem(pszName), widths);
}

} // namespace devilution
