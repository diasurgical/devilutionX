/**
 * @file itemlabels.h
 *
 * Adds item labels QoL feature
 */
#pragma once

#include "engine.h"

namespace devilution {

void ToggleItemLabelHighlight();
void HighlightKeyPressed(bool pressed);
bool IsItemLabelHighlighted();
void ResetItemlabelHighlighted();
bool IsHighlightingLabelsEnabled();
void AddItemToLabelQueue(int id, Point position);
void DrawItemNameLabels(const Surface &out);

} // namespace devilution
