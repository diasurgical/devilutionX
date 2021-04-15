/**
 * @file help.h
 *
 * Interface of the in-game help text.
 */
#pragma once

namespace devilution {

extern bool helpflag;

void InitHelp();
void DrawHelp(CelOutputBuffer out);
void DisplayHelp();
void HelpScrollUp();
void HelpScrollDown();

}
