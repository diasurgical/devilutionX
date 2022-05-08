#include "scrollbar.h"

#include "engine/load_pcx.hpp"

namespace devilution {

std::optional<OwnedPcxSprite> ArtScrollBarBackground;
std::optional<OwnedPcxSprite> ArtScrollBarThumb;
std::optional<OwnedPcxSpriteSheet> ArtScrollBarArrow;

void LoadScrollBar()
{
	ArtScrollBarBackground = LoadPcxAsset("ui_art\\sb_bg.pcx");
	ArtScrollBarThumb = LoadPcxAsset("ui_art\\sb_thumb.pcx");
	ArtScrollBarArrow = LoadPcxSpriteSheetAsset("ui_art\\sb_arrow.pcx", 4);
}

void UnloadScrollBar()
{
	ArtScrollBarArrow = std::nullopt;
	ArtScrollBarThumb = std::nullopt;
	ArtScrollBarBackground = std::nullopt;
}

} // namespace devilution
