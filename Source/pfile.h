/**
 * @file pfile.h
 *
 * Interface of the save game encoding functionality.
 */
#pragma once

#include "player.h"
#include "DiabloUI/diabloui.h"

namespace devilution {

#define MAX_CHARACTERS 99

extern bool gbValidSaveFile;

class PFileScopedArchiveWriter {
public:
	// Opens the player save file for writing
	PFileScopedArchiveWriter(bool clear_tables = !gbIsMultiplayer);

	// Finishes writing and closes the player save file.
	~PFileScopedArchiveWriter();

private:
	int save_num_;
	bool clear_tables_;
};

const char *pfile_get_password();
void pfile_write_hero(bool write_game_data = false, bool clear_tables = !gbIsMultiplayer);
bool pfile_ui_set_hero_infos(bool (*ui_add_hero_info)(_uiheroinfo *));
void pfile_ui_set_class_stats(unsigned int player_class_nr, _uidefaultstats *class_stats);
bool pfile_ui_save_create(_uiheroinfo *heroinfo);
bool pfile_delete_save(_uiheroinfo *hero_info);
void pfile_read_player_from_save(char name[16], int playerId);
bool LevelFileExists();
void GetTempLevelNames(char *szTemp);
void GetPermLevelNames(char *szPerm);
void pfile_remove_temp_files();
std::unique_ptr<byte[]> pfile_read(const char *pszName, size_t *pdwLen);
void pfile_update(bool force_save);

} // namespace devilution
