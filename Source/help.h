/**
 * @file help.h
 *
 * Interface of the in-game help text.
 */
#pragma once

#include "engine.h"

namespace devilution {

extern bool HelpFlag;

void InitHelp();
void DrawHelp(const Surface &out);
void DisplayHelp();
void HelpScrollUp();
void HelpScrollDown();

} // namespace devilution
