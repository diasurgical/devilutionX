/**
* @file common.h
*
* Common functions for QoL features
*/
#pragma once

#include "SDL_stdinc.h" // for Uint8

namespace devilution {

struct CelOutputBuffer;

void FastDrawHorizLine(const CelOutputBuffer &out, int x, int y, int width, Uint8 col);
void FastDrawVertLine(const CelOutputBuffer &out, int x, int y, int height, Uint8 col);
void FillRect(const CelOutputBuffer &out, int x, int y, int width, int height, Uint8 col);
int GetTextWidth(const char *s);
/**
 * @brief Prints integer into buffer, using ',' as thousands separator.
 * @param out Destination buffer
 * @param n Number to print
 * @return Address of first character after printed number
*/
char *PrintWithSeparator(char *out, long long n);

} // namespace devilution
