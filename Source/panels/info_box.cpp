#include "panels/info_box.hpp"

#include "engine/load_cel.hpp"

namespace devilution {

std::optional<CelSprite> pSTextBoxCels;
std::optional<CelSprite> pSTextSlidCels;

void InitInfoBoxGfx()
{
	pSTextSlidCels = LoadCel("Data\\TextSlid.CEL", 12);
	pSTextBoxCels = LoadCel("Data\\TextBox2.CEL", 271);
}

void FreeInfoBoxGfx()
{
	pSTextBoxCels = std::nullopt;
	pSTextSlidCels = std::nullopt;
}

} // namespace devilution
