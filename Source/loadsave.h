/**
 * @file loadsave.h
 *
 * Interface of save game functionality.
 */
#pragma once

#include "mpq/mpq_writer.hpp"
#include "player.h"
#include "utils/attributes.h"

namespace devilution {

extern DVL_API_FOR_TEST bool gbIsHellfireSaveGame;
extern DVL_API_FOR_TEST uint8_t giNumberOfLevels;

void RemoveInvalidItem(Item &pItem);
_item_indexes RemapItemIdxFromDiablo(_item_indexes i);
_item_indexes RemapItemIdxToDiablo(_item_indexes i);
_item_indexes RemapItemIdxFromSpawn(_item_indexes i);
_item_indexes RemapItemIdxToSpawn(_item_indexes i);
bool IsHeaderValid(uint32_t magicNumber);
void LoadHotkeys();
void LoadHeroItems(Player &player);
/**
 * @brief Remove invalid inventory items from the inventory grid
 * @param player The player to remove invalid items from
 */
void RemoveEmptyInventory(Player &player);

/**
 * @brief Load game state
 * @param firstflag Can be set to false if we are simply reloading the current game
 */
void LoadGame(bool firstflag);
void SaveHotkeys(MpqWriter &saveWriter, const Player &player);
void SaveHeroItems(MpqWriter &saveWriter, Player &player);
void SaveGameData(MpqWriter &saveWriter);
void SaveGame();
void SaveLevel(MpqWriter &saveWriter);
void LoadLevel();
void ConvertLevels(MpqWriter &saveWriter);
void LoadStash();
void SaveStash(MpqWriter &stashWriter);

} // namespace devilution
