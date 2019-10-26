#pragma once
// Controller actions implementation

#include "devilution.h"

namespace dvl {

// Run on every game logic iteration.
void plrctrls_game_logic();

// Runs before the start of event loop drain, even if there are no events.
void plrctrls_event_loop();

// Moves the map if active, the cursor otherwise.
void HandleRightStickMotion();

// Whether we're in a dialog menu that the game handles natively with keyboard controls.
bool InGameMenu();

void useBeltPotion(bool mana);

// Talk to towners, click on inv items, attack, etc.
void performPrimaryAction();

// Open chests, doors, pickup items.
void performSecondaryAction();

typedef struct coords {
	int x;
	int y;
} coords;
extern coords speedspellscoords[50];
extern int speedspellcount;

} // namespace dvl
