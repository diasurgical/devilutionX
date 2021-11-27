/**
 * @file itemlabels.h
 *
 * Adds item labels QoL feature
 */
#pragma once

#include "engine.h"

namespace devilution {

void ToggleItemLabelHighlight();
void AltPressed(bool pressed);
bool IsItemLabelHighlighted();
bool IsHighlightingLabelsEnabled();
void AddItemToLabelQueue(int id, int x, int y);
void DrawItemNameLabels(const Surface &out);

} // namespace devilution
