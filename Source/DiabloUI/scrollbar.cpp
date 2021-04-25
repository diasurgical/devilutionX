#include "scrollbar.h"

namespace devilution {

Art ArtScrollBarBackground;
Art ArtScrollBarThumb;
Art ArtScrollBarArrow;

void LoadScrollBar()
{
	LoadArt(_("ui_art\\sb_bg.pcx"), &ArtScrollBarBackground);
	LoadArt(_("ui_art\\sb_thumb.pcx"), &ArtScrollBarThumb);
	LoadArt(_("ui_art\\sb_arrow.pcx"), &ArtScrollBarArrow, 4);
}

void UnloadScrollBar()
{
	ArtScrollBarArrow.Unload();
	ArtScrollBarThumb.Unload();
	ArtScrollBarBackground.Unload();
}

} // namespace devilution
