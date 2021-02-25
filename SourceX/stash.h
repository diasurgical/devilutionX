#pragma once
#include "all.h"
#include <string>

namespace dvl {

extern bool stash;
extern char pcursstashitem;
extern BYTE *pInvCels;
extern BYTE *tbuff;
extern void InvDrawSlotBack(int X, int Y, int W, int H);
extern int SwapItem(ItemStruct *a, ItemStruct *b);
extern const std::string &GetPrefPath();

extern void SaveItems(ItemStruct *pItem, const int n);
extern void LoadItems(const int n, ItemStruct *pItem);
extern void CopyInt(const void *src, void *dst);
extern void CopyBytes(const void *src, const int n, void *dst);

void InitStash();
void DrawStash();
void CheckStash();
bool LoadStash();
char CheckStashHLight();
int GetStashSlotFromMouse(int mx, int my);

} // namespace dvl
