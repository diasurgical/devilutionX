/**
 * @file minitext.h
 *
 * Interface of scrolling dialog text.
 */
#pragma once

#include "engine.h"
#include "textdat.h"

namespace devilution {

extern bool qtextflag;

void FreeQuestText();
void InitQuestText();
void InitQTextMsg(_speech_id m);

/**
 * @brief Draw the quest dialog window decoration and background.
 */
void DrawQTextBack(const Surface &out);

/**
 * @brief Draw the quest dialog window text.
 */
void DrawQText(const Surface &out);

} // namespace devilution
