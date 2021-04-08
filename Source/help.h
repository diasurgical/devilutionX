/**
 * @file help.h
 *
 * Interface of the in-game help text.
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

extern BOOL helpflag;

void InitHelp();
void DrawHelp(CelOutputBuffer out);
void DisplayHelp();
void HelpScrollUp();
void HelpScrollDown();

#ifdef __cplusplus
}
#endif

}
