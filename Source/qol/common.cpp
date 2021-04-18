/**
* @file common.h
*
* Common functions for QoL features
*/

#include "common.h"
#include "engine.h"

namespace devilution {

void FastDrawHorizLine(const CelOutputBuffer &out, int x, int y, int width, BYTE col)
{
	memset(out.at(x, y), col, width);
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
