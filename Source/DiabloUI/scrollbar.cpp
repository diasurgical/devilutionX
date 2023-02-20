#include "scrollbar.h"

#include "engine/load_pcx.hpp"

namespace devilution {

OptionalOwnedClxSpriteList ArtScrollBarBackground;
OptionalOwnedClxSpriteList ArtScrollBarThumb;
OptionalOwnedClxSpriteList ArtScrollBarArrow;
Uint16 ScrollBarWidth;
uint16_t ScrollBarArrowFrame_UP_ACTIVE;
uint16_t ScrollBarArrowFrame_UP;
uint16_t ScrollBarArrowFrame_DOWN_ACTIVE;
uint16_t ScrollBarArrowFrame_DOWN;

void LoadScrollBar()
{
	ScrollBarWidth = 25;
	ScrollBarArrowFrame_UP_ACTIVE = 0;
	ScrollBarArrowFrame_UP = 1;
	ScrollBarArrowFrame_DOWN_ACTIVE = 2;
	ScrollBarArrowFrame_DOWN = 3;
	ArtScrollBarBackground = LoadPcx("ui_art\\sb_bg");
	ArtScrollBarThumb = LoadPcx("ui_art\\sb_thumb");
	ArtScrollBarArrow = LoadPcxSpriteList("ui_art\\sb_arrow", 4);
}

void UnloadScrollBar()
{
	ArtScrollBarArrow = std::nullopt;
	ArtScrollBarThumb = std::nullopt;
	ArtScrollBarBackground = std::nullopt;
}

} // namespace devilution
