#include "DiabloUI/art_draw.h"
#include "DiabloUI/fonts.h"
#include "DiabloUI/text.h"
#include "DiabloUI/ui_item.h"

namespace dvl {

namespace {

int AlignedTextOffsetX(const char *text, std::size_t rect_w, TXT_JUST align, _artFontTables size)
{
	switch (align) {
	case JustCentre:
		return (rect_w - GetStrWidth(text, size)) / 2;
	case JustRight:
		return rect_w - GetStrWidth(text, size);
	default:
		return 0;
	}
}

} // namespace

void DrawArtStr(const char *text, const SDL_Rect &rect, int flags, bool drawTextCursor = false)
{
	_artFontTables size = AFT_SMALL;
	_artFontColors color = flags & UIS_GOLD ? AFC_GOLD : AFC_SILVER;
	TXT_JUST align = JustLeft;

	if (flags & UIS_MED)
		size = AFT_MED;
	else if (flags & UIS_BIG)
		size = AFT_BIG;
	else if (flags & UIS_HUGE)
		size = AFT_HUGE;

	if (flags & UIS_CENTER)
		align = JustCentre;
	else if (flags & UIS_RIGHT)
		align = JustRight;

	int x = rect.x + AlignedTextOffsetX(text, rect.w, align, size);

	int sx = x;
	int sy = rect.y;
	if (flags & UIS_VCENTER)
		sy += (rect.h - ArtFonts[size][color].h()) / 2;

	for (size_t i = 0, n = strlen(text); i < n; i++) {
		if (text[i] == '\n') {
			sx = x;
			sy += ArtFonts[size][color].h();
			continue;
		}
		BYTE w = FontTables[size][*(BYTE *)&text[i] + 2] ? FontTables[size][*(BYTE *)&text[i] + 2] : FontTables[size][0];
		DrawArt(sx, sy, &ArtFonts[size][color], *(BYTE *)&text[i], w);
		sx += w;
	}
	if (drawTextCursor && GetAnimationFrame(2, 500)) {
		DrawArt(sx, sy, &ArtFonts[size][color], '|');
	}
}

} // namespace dvl
