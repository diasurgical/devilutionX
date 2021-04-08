/**
 * @file minitext.h
 *
 * Interface of scrolling dialog text.
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

extern bool qtextflag;

void FreeQuestText();
void InitQuestText();
void InitQTextMsg(int m);

/**
 * @brief Draw the quest dialog window decoration and background.
 */
void DrawQTextBack(CelOutputBuffer out);

/**
 * @brief Draw the quest dialog window text.
 */
void DrawQText(CelOutputBuffer out);

#ifdef __cplusplus
}
#endif

}
