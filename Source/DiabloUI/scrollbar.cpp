#include "scrollbar.h"

#include "engine/load_pcx.hpp"

namespace devilution {

std::optional<OwnedCelSpriteWithFrameHeight> ArtScrollBarBackground;
std::optional<OwnedCelSpriteWithFrameHeight> ArtScrollBarThumb;
std::optional<OwnedCelSpriteSheetWithFrameHeight> ArtScrollBarArrow;

void LoadScrollBar()
{
	ArtScrollBarBackground = LoadPcxAsCl2("ui_art\\sb_bg.pcx");
	ArtScrollBarThumb = LoadPcxAsCl2("ui_art\\sb_thumb.pcx");
	ArtScrollBarArrow = LoadPcxSpriteSheetAsCl2("ui_art\\sb_arrow.pcx", 4);
}

void UnloadScrollBar()
{
	ArtScrollBarArrow = std::nullopt;
	ArtScrollBarThumb = std::nullopt;
	ArtScrollBarBackground = std::nullopt;
}

} // namespace devilution
