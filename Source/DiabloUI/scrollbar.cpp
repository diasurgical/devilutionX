#include "scrollbar.h"

#include "engine/load_pcx.hpp"

namespace devilution {

OptionalOwnedClxSpriteList ArtScrollBarBackground;
OptionalOwnedClxSpriteList ArtScrollBarThumb;
OptionalOwnedClxSpriteList ArtScrollBarArrow;

void LoadScrollBar()
{
	ArtScrollBarBackground = LoadPcx("ui_art\\sb_bg.pcx");
	ArtScrollBarThumb = LoadPcx("ui_art\\sb_thumb.pcx");
	ArtScrollBarArrow = LoadPcxSpriteList("ui_art\\sb_arrow.pcx", 4);
}

void UnloadScrollBar()
{
	ArtScrollBarArrow = std::nullopt;
	ArtScrollBarThumb = std::nullopt;
	ArtScrollBarBackground = std::nullopt;
}

} // namespace devilution
