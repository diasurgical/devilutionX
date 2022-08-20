#include "panels/info_box.hpp"

#include "engine/load_cel.hpp"

namespace devilution {

OptionalOwnedClxSpriteList pSTextBoxCels;
OptionalOwnedClxSpriteList pSTextSlidCels;

void InitInfoBoxGfx()
{
	pSTextSlidCels = LoadCel("data\\textslid.cel", 12);
	pSTextBoxCels = LoadCel("data\\textbox2.cel", 271);
}

void FreeInfoBoxGfx()
{
	pSTextBoxCels = std::nullopt;
	pSTextSlidCels = std::nullopt;
}

} // namespace devilution
