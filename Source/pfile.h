//HEADER_GOES_HERE
#ifndef __PFILE_H__
#define __PFILE_H__

extern BOOL gbValidSaveFile;

void pfile_init_save_directory();
void pfile_check_available_space(char *pszDir);
void pfile_write_hero();
DWORD pfile_get_save_num_from_name(const char *name);
void pfile_encode_hero(const PkPlayerStruct *pPack);
BOOL pfile_open_archive(BOOL update, DWORD save_num);
void pfile_get_save_path(char *pszBuf, DWORD dwBufSize, DWORD save_num);
void pfile_flush(BOOL is_single_player, DWORD save_num);
BOOL pfile_create_player_description(char *dst, DWORD len);
BOOL pfile_rename_hero(const char *name_1, const char *name_2);
void pfile_flush_W();
void game_2_ui_player(const PlayerStruct *p, _uiheroinfo *heroinfo, BOOL bHasSaveFile);
BYTE game_2_ui_class(const PlayerStruct *p);
BOOL __stdcall pfile_ui_set_hero_infos(BOOL(__stdcall *ui_add_hero_info)(_uiheroinfo *));
char *GetSaveDirectory(char *dst, int dst_size, DWORD save_num);
BOOL pfile_read_hero(HANDLE archive, PkPlayerStruct *pPack);
HANDLE pfile_open_save_archive(BOOL *showFixedMsg, DWORD save_num);
void pfile_SFileCloseArchive(HANDLE hsArchive);
BOOL pfile_archive_contains_game(HANDLE hsArchive, DWORD save_num);
BOOL __stdcall pfile_ui_set_class_stats(unsigned int player_class_nr, _uidefaultstats *class_stats);
char pfile_get_player_class(unsigned int player_class_nr);
BOOL __stdcall pfile_ui_save_create(_uiheroinfo *heroinfo);
BOOL __stdcall pfile_get_file_name(DWORD lvl, char *dst);
BOOL __stdcall pfile_delete_save(_uiheroinfo *hero_info);
void pfile_read_player_from_save();
void GetTempLevelNames(char *szTemp);
void GetPermLevelNames(char *szPerm);
void pfile_get_game_name(char *dst);
void pfile_remove_temp_files();
BOOL __stdcall GetTempSaveNames(DWORD dwIndex, char *szTemp);
void pfile_rename_temp_to_perm();
BOOL __stdcall GetPermSaveNames(DWORD dwIndex, char *szPerm);
void pfile_write_save_file(const char *pszName, BYTE *pbData, DWORD dwLen, DWORD qwLen);
void pfile_strcpy(char *dst, const char *src);
BYTE *pfile_read(const char *pszName, DWORD *pdwLen);
void pfile_update(BOOL force_save);

/* rdata */

#endif /* __PFILE_H__ */
