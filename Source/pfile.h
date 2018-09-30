//HEADER_GOES_HERE
#ifndef __PFILE_H__
#define __PFILE_H__

extern int pfile_cpp_init_value;
extern char hero_names[MAX_CHARACTERS][PLR_NAME_LEN];
extern bool gbValidSaveFile; // idb
extern int save_prev_tc; // weak

void __cdecl pfile_cpp_init();
void __cdecl pfile_init_save_directory();
void __fastcall pfile_check_available_space(char *pszDir);
void __cdecl pfile_write_hero();
int __fastcall pfile_get_save_num_from_name(char *name);
void __fastcall pfile_encode_hero(PkPlayerStruct *pPack);
bool __fastcall pfile_open_archive(bool a1, int save_num);
void __fastcall pfile_get_save_path(char *pszBuf, int dwBufSize, int save_num);
void __fastcall pfile_flush(bool is_single_player, int save_num);
bool __fastcall pfile_create_player_description(char *dst, int len);
int __fastcall pfile_create_save_file(char *name_1, char *name_2);
void __cdecl pfile_flush_W();
void __fastcall game_2_ui_player(PlayerStruct *p, _uiheroinfo *heroinfo, bool bHasSaveFile);
char __fastcall game_2_ui_class(PlayerStruct *p);
bool __stdcall pfile_ui_set_hero_infos(void (__stdcall *ui_add_hero_info)(_uiheroinfo *));
char *__fastcall GetSaveDirectory(char *dst, int dst_size, int save_num);
bool __fastcall pfile_read_hero(void *archive, PkPlayerStruct *pPack);
void *__fastcall pfile_open_save_archive(int *unused, int save_num);
void __fastcall pfile_SFileCloseArchive(void *hsArchive);
bool __fastcall pfile_archive_contains_game(void *hsArchive);
bool __stdcall pfile_ui_set_class_stats(int player_class_nr, _uidefaultstats *class_stats);
int __fastcall pfile_get_player_class(int player_class_nr);
bool __stdcall pfile_ui_save_create(_uiheroinfo *heroinfo);
bool __stdcall pfile_get_file_name(int lvl, char *dst);
bool __stdcall pfile_delete_save(_uiheroinfo *hero_info);
void __cdecl pfile_read_player_from_save();
void __fastcall GetTempLevelNames(char *szTemp);
void __fastcall GetPermLevelNames(char *szPerm);
void __fastcall pfile_get_game_name(char *dst);
void __cdecl pfile_remove_temp_files();
bool __stdcall GetTempSaveNames(int dwIndex, char *szTemp);
void __cdecl pfile_rename_temp_to_perm();
bool __stdcall GetPermSaveNames(int dwIndex, char *szPerm);
void __fastcall pfile_write_save_file(char *pszName, void *pbData, int dwLen, int qwLen);
void __fastcall pfile_strcpy(char *dst, char *src);
char *__fastcall pfile_read(char *pszName, int *pdwLen);
void __fastcall pfile_update(bool force_save);

/* rdata */

extern const int pfile_inf; // weak

#endif /* __PFILE_H__ */
