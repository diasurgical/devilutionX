/**
* @file common.h
*
* Common functions for QoL features
*/
#pragma once

#include <SDL.h>

namespace devilution {

struct CelOutputBuffer;

void FastDrawHorizLine(const CelOutputBuffer &out, int x, int y, int width, Uint8 col);
/**
 * @brief Prints integer into buffer, using ',' as thousands separator.
 * @param out Destination buffer
 * @param n Number to print
 * @return Address of first character after printed number
*/
char *PrintWithSeparator(char *out, long long n);

} // namespace devilution
