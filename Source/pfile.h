/**
 * @file pfile.h
 *
 * Interface of the save game encoding functionality.
 */
#pragma once

#include "DiabloUI/diabloui.h"
#include "mpq/mpq_writer.hpp"
#include "player.h"

namespace devilution {

#define MAX_CHARACTERS 99

extern bool gbValidSaveFile;

class PFileScopedArchiveWriter {
public:
	// Opens the player save file for writing
	PFileScopedArchiveWriter(bool clearTables = !gbIsMultiplayer);

	// Finishes writing and closes the player save file.
	~PFileScopedArchiveWriter();

private:
	int save_num_;
	bool clear_tables_;
};

/**
 * @brief Comparsion result of pfile_compare_hero_demo
 */
enum class HeroCompareResult {
	ReferenceNotFound,
	Same,
	Difference,
};

MpqWriter &CurrentSaveArchive();
MpqWriter &StashArchive();
std::optional<MpqArchive> OpenSaveArchive(uint32_t saveNum);
std::optional<MpqArchive> OpenStashArchive();
const char *pfile_get_password();
std::unique_ptr<byte[]> ReadArchive(MpqArchive &archive, const char *pszName, size_t *pdwLen = nullptr);
void pfile_write_hero(bool writeGameData = false, bool clearTables = !gbIsMultiplayer);
/**
 * @brief Save a reference game-state (save game) for the demo recording
 * @param demo that is recorded
 */
void pfile_write_hero_demo(int demo);
/**
 * @brief Compares the actual game-state (savegame) with a reference game-state (save game from demo recording)
 * @param demo for the comparsion
 * @return The comparsion result.
 */
HeroCompareResult pfile_compare_hero_demo(int demo);
void sfile_write_stash();
bool pfile_ui_set_hero_infos(bool (*uiAddHeroInfo)(_uiheroinfo *));
void pfile_ui_set_class_stats(unsigned int playerClass, _uidefaultstats *classStats);
uint32_t pfile_ui_get_first_unused_save_num();
bool pfile_ui_save_create(_uiheroinfo *heroinfo);
bool pfile_delete_save(_uiheroinfo *heroInfo);
void pfile_read_player_from_save(uint32_t saveNum, Player &player);
bool LevelFileExists();
void GetTempLevelNames(char *szTemp);
void GetPermLevelNames(char *szPerm);
void pfile_remove_temp_files();
std::unique_ptr<byte[]> pfile_read(const char *pszName, size_t *pdwLen);
void pfile_update(bool forceSave);

} // namespace devilution
