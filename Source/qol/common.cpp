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
#include "utils/language.h"

namespace devilution {

char *PrintWithSeparator(char *out, int n)
{
	if (n < 1000) {
		return out + sprintf(out, "%d", n);
	}

	char *append = PrintWithSeparator(out, n / 1000);
	return append + sprintf(append, _(/* TRANSLATORS: Decimal separator */ ",%03d"), n % 1000);
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
