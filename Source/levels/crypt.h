/**
 * @file levels/crypt.h
 *
 * Interface of the cathedral level generation algorithms.
 */
#pragma once

#include "levels/gendung.h"

namespace devilution {

extern int UberRow;
extern int UberCol;
extern bool IsUberRoomOpened;
extern bool IsUberLeverActivated;
extern int UberDiabloMonsterIndex;

extern const Miniset L5STAIRSUP;

void InitCryptPieces();
void SetCryptRoom();
void SetCornerRoom();
void FixCryptDirtTiles();
bool PlaceCryptStairs(lvl_entry entry);
void CryptSubstitution();
void SetCryptSetPieceRoom();

} // namespace devilution
