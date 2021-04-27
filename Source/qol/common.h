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
 * @brief Quickly draw a horizontal line (via memset). Does not clip to output buffer.
 * @param out Destination buffer
 * @param x Start X coordinate (left end)
 * @param y Vertical location of line
 * @param width Number of pixels to fill
 * @param col Color index to use for filling
*/
void FastDrawHorizLine(const CelOutputBuffer &out, int x, int y, int width, Uint8 col);

/**
 * @brief Quickly draw a vertical line. Does not clip to output buffer.
 * @param out Destination buffer
 * @param x Horizontal location of line
 * @param y Start Y coordinate (top end)
 * @param height Number of pixels to fill
 * @param col Color index to use for filling
*/
void FastDrawVertLine(const CelOutputBuffer &out, int x, int y, int height, Uint8 col);

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
