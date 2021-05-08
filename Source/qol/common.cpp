/**
* @file common.h
*
* Common functions for QoL features
*/

#include <SDL.h>

#include "common.h"
#include "engine.h"
#include "engine/render/text_render.hpp"
#include "qol/monhealthbar.h"
#include "qol/xpbar.h"

namespace devilution {

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
