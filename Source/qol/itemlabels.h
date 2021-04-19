/**
* @file itemlabels.h
*
* Adds Item Labels QoL feature
*/
#pragma once
#include "items.h"

namespace devilution {

struct CelOutputBuffer;

extern BYTE ItemAnimLs[];
extern BYTE *itemanims[ITEMTYPES];

void ToggleItemLabelHighlight();
void AltPressed(bool pressed);
bool IsGeneratingItemLabels();
bool IsItemLabelHighlighted();
void UpdateItemLabelOffsets(const CelOutputBuffer &out, BYTE *dst, int width);
void GenerateItemLabelOffsets(const CelOutputBuffer &out);
void AddItemToLabelQueue(int x, int y, int id);
void DrawItemNameLabels(const CelOutputBuffer &out);

} // namespace devilution
