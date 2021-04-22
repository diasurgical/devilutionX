/**
 * @file minitext.h
 *
 * Interface of scrolling dialog text.
 */
#pragma once

#include "engine.h"

namespace devilution {

extern bool qtextflag;

void FreeQuestText();
void InitQuestText();
void InitQTextMsg(int m);

/**
 * @brief Draw the quest dialog window decoration and background.
 */
void DrawQTextBack(const CelOutputBuffer &out);

/**
 * @brief Draw the quest dialog window text.
 */
void DrawQText(const CelOutputBuffer &out);

} // namespace devilution
