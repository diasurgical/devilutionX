/**
 * @file pfile.h
 *
 * Interface of the save game encoding functionality.
 */
#pragma once

#include "player.h"
#include "../DiabloUI/diabloui.h"

namespace devilution {

extern bool gbValidSaveFile;

class PFileScopedArchiveWriter {
public:
	// Opens the player save file for writing
	PFileScopedArchiveWriter();

	// Finishes writing and closes the player save file.
	~PFileScopedArchiveWriter();

private:
	DWORD save_num_;
};

const char *pfile_get_password();
void pfile_write_hero(bool write_game_data = false);
bool pfile_create_player_description();
void pfile_flush_W();
bool pfile_ui_set_hero_infos(bool (*ui_add_hero_info)(_uiheroinfo *));
bool pfile_archive_contains_game(HANDLE hsArchive, DWORD save_num);
void pfile_ui_set_class_stats(unsigned int player_class_nr, _uidefaultstats *class_stats);
bool pfile_ui_save_create(_uiheroinfo *heroinfo);
bool pfile_get_file_name(DWORD lvl, char *dst);
bool pfile_delete_save(_uiheroinfo *hero_info);
void pfile_read_player_from_save();
bool LevelFileExists();
void GetTempLevelNames(char *szTemp);
void GetPermLevelNames(char *szPerm);
void pfile_remove_temp_files();
void pfile_write_save_file(const char *pszName, BYTE *pbData, DWORD dwLen, DWORD qwLen);
BYTE *pfile_read(const char *pszName, DWORD *pdwLen);
void pfile_update(bool force_save);

} // namespace devilution
