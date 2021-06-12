/**
 * @file loadsave.h
 *
 * Interface of save game functionality.
 */
#pragma once

#include "player.h"

namespace devilution {

extern bool gbIsHellfireSaveGame;
extern uint8_t giNumberOfLevels;

void RemoveInvalidItem(ItemStruct *pItem);
_item_indexes RemapItemIdxFromDiablo(_item_indexes i);
_item_indexes RemapItemIdxToDiablo(_item_indexes i);
_item_indexes RemapItemIdxFromSpawn(_item_indexes i);
_item_indexes RemapItemIdxToSpawn(_item_indexes i);
bool IsHeaderValid(uint32_t magicNumber);
void LoadHotkeys();
void LoadHeroItems(PlayerStruct &pPlayer);
/**
 * @brief Remove invalid inventory items from the inventory grid
 * @param pnum The id of the player
 */
void RemoveEmptyInventory(int pnum);
void LoadGame(bool firstflag);
void SaveHotkeys();
void SaveHeroItems(PlayerStruct &pPlayer);
void SaveGameData();
void SaveGame();
void SaveLevel();
void LoadLevel();

} // namespace devilution
