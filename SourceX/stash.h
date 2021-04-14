
#pragma once

namespace devilution {

struct ItemStruct;

// Shows or hides the stash window.
void ShowStash(bool show);
bool IsStashVisible(void);

// Checks if the mouse coordinates are inside the visible stash window.
bool CheckStashHit(int mouseX, int mouseY);

void StashClick(int mouseX, int mouseY, bool shiftDown);
int CheckStashHighlight(int mouseX, int mouseY);
void DrawStash(CelOutputBuffer out);

// Returns the item in the given stash slot.
// Slots are transformed stash slot indices, i.e. start at -2 and count down.
ItemStruct* StashCheckItem(int pnum, int slot);

bool IsStashTakingKeyInput(void);
void StashHandleKeyInput(char vkey);

// Has to be called whenever spellbook stats have to be updated.
void StashUpdateSpellbookRequirements(int pnum);

void LoadStash(void);
void SaveStash(void);

} // namespace devilution
