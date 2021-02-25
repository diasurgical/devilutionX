#pragma once
#include "all.h"

namespace dvl {

extern bool stash;
extern BYTE *pInvCels;
extern void InvDrawSlotBack(int X, int Y, int W, int H);
extern int SwapItem(ItemStruct *a, ItemStruct *b);
void DrawStash();
void CheckStash();

} // namespace dvl
