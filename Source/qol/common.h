/**
* @file common.h
*
* Common functions for QoL features
*/
#pragma once

#include "SDL_stdinc.h" // for Uint8
#include "items.h"
#include "control.h"

namespace devilution {

bool IsMouseOverGameArea();
text_color GetItemTextColor(ItemStruct &item, bool reqCheck = true);
int GetTextWidth(const char *s);
void FastDrawVertLine(const CelOutputBuffer &out, int x, int y, int height, Uint8 col);
void FastDrawHorizLine(const CelOutputBuffer &out, int x, int y, int width, Uint8 col);
void FillRect(const CelOutputBuffer &out, int x, int y, int width, int height, Uint8 col);
/**
 * @brief Prints integer into buffer, using ',' as thousands separator.
 * @param out Destination buffer
 * @param n Number to print
 * @return Address of first character after printed number
*/
char *PrintWithSeparator(char *out, long long n);

} // namespace devilution
