/**
 * @file loadsave.h
 *
 * Interface of save game functionality.
 */
#pragma once

#include "player.h"

namespace devilution {

extern bool gbIsHellfireSaveGame;
extern int giNumberOfLevels;

void RemoveInvalidItem(ItemStruct *pItem);
int RemapItemIdxFromDiablo(int i);
int RemapItemIdxToDiablo(int i);
bool IsHeaderValid(Uint32 magicNumber);
void LoadHotkeys();
void LoadHeroItems(PlayerStruct *pPlayer);
/**
 * @brief Remove invalid inventory items from the inventory grid
 * @param pnum The id of the player
 */
void RemoveEmptyInventory(int pnum);
void LoadGame(bool firstflag);
void SaveHotkeys();
void SaveHeroItems(PlayerStruct *pPlayer);
void SaveGameData();
void SaveGame();
void SaveLevel();
void LoadLevel();

}
