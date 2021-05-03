/**
* @file common.h
*
* Common functions for QoL features
*/

#include <SDL.h>

#include "common.h"
#include "control.h"
#include "engine.h"
#include "qol/monhealthbar.h"
#include "qol/xpbar.h"

namespace devilution {

int GetTextWidth(const char *s)
{
	int l = 0;
	while (*s) {
		l += fontkern[fontframe[gbFontTransTbl[static_cast<BYTE>(*s++)]]] + 1;
	}
	return l;
}

void FastDrawHorizLine(const CelOutputBuffer &out, int x, int y, int width, Uint8 col)
{
	UnsafeDrawHorizontalLine(out, {x, y}, width, col);
}

void FastDrawVertLine(const CelOutputBuffer &out, int x, int y, int height, Uint8 col)
{
	UnsafeDrawVerticalLine(out, {x, y}, height, col);
}

char *PrintWithSeparator(char *out, long long n)
{
	if (n < 1000) {
		return out + sprintf(out, "%lld", n);
	}

	char *append = PrintWithSeparator(out, n / 1000);
	return append + sprintf(append, ",%03lld", n % 1000);
}

void FreeQol()
{
	FreeMonsterHealthBar();
	FreeXPBar();
}

void InitQol()
{
	InitMonsterHealthBar();
	InitXPBar();
}

} // namespace devilution
