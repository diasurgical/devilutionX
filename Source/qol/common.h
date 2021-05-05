/**
* @file common.h
*
* Common functions for QoL features
*/
#pragma once

#include <SDL.h>

namespace devilution {

struct CelOutputBuffer;

/**
 * @brief Return width (in pixels) of the passed in string, when printed with smaltext.cel. Does not consider line breaks.
 * @param s String for which to compute width
 * @return Pixel width of the string
*/
int GetTextWidth(const char *s);

/**
 * @brief Prints integer into buffer, using ',' as thousands separator.
 * @param out Destination buffer
 * @param n Number to print
 * @return Address of first character after printed number
*/
char *PrintWithSeparator(char *out, long long n);

void FreeQol();
void InitQol();

} // namespace devilution
