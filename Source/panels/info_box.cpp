#include "panels/info_box.hpp"

#include "engine/load_cel.hpp"

namespace devilution {

OptionalOwnedCelSprite pSTextBoxCels;
OptionalOwnedCelSprite pSTextSlidCels;

void InitInfoBoxGfx()
{
	pSTextSlidCels = LoadCelAsCl2("Data\\TextSlid.CEL", 12);
	pSTextBoxCels = LoadCelAsCl2("Data\\TextBox2.CEL", 271);
}

void FreeInfoBoxGfx()
{
	pSTextBoxCels = std::nullopt;
	pSTextSlidCels = std::nullopt;
}

} // namespace devilution
