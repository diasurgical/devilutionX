/**
* @file itemlabels.h
*
* Adds labels QoL feature
*/
#pragma once

namespace devilution {

void ToggleLabelHighlight();
void AltPressed(bool pressed);
bool IsLabelHighlighted();
bool IsHighlightingLabelsEnabled();
void AddItemToLabelQueue(int id, int x, int y);
void AddObjectToLabelQueue(int id, int x, int y);
void DrawLabels(const CelOutputBuffer &out);

} // namespace devilution
