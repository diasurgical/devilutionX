/**
* @file common.h
*
* Common functions for QoL features
*/

#include "common.h"
#include "inv.h"

namespace devilution {

bool IsMouseOverGameArea()
{
	if ((invflag || sbookflag) && MouseX > RIGHT_PANEL && MouseY <= SPANEL_HEIGHT)
		return false;
	if ((chrflag || questlog) && MouseX < SPANEL_WIDTH && MouseY <= SPANEL_HEIGHT)
		return false;
	if (MouseY >= PANEL_TOP && MouseX >= PANEL_LEFT && MouseX <= PANEL_LEFT + PANEL_WIDTH)
		return false;

	return true;
}

text_color GetItemTextColor(ItemStruct &item, bool reqCheck)
{
	if (reqCheck && !item._iStatFlag)
		return text_color::COL_RED;
	if (item._iMagical == ITEM_QUALITY_MAGIC)
		return text_color::COL_BLUE;
	if (item._iMagical == ITEM_QUALITY_UNIQUE)
		return text_color::COL_GOLD;
	return text_color::COL_WHITE;
}

int GetTextWidth(const char *s)
{
	int l = 0;
	while (*s) {
		l += fontkern[fontframe[gbFontTransTbl[(BYTE)*s++]]] + 1;
	}
	return l;
}

void FastDrawVertLine(const CelOutputBuffer &out, int x, int y, int height, Uint8 col)
{
	BYTE *p = out.at(x, y);
	for (int j = 0; j < height; j++) {
		*p = col;
		p += out.pitch();
	}
}

void FastDrawHorizLine(const CelOutputBuffer &out, int x, int y, int width, Uint8 col)
{
	memset(out.at(x, y), col, width);
}

void FillRect(const CelOutputBuffer &out, int x, int y, int width, int height, Uint8 col)
{
	for (int j = 0; j < height; j++) {
		FastDrawHorizLine(out, x, y + j, width, col);
	}
}

char *PrintWithSeparator(char *out, long long n)
{
	if (n < 1000) {
		return out + sprintf(out, "%lld", n);
	}

	char *append = PrintWithSeparator(out, n / 1000);
	return append + sprintf(append, ",%03lld", n % 1000);
}

} // namespace devilution
