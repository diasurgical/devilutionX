//HEADER_GOES_HERE
#ifndef __PFILE_H__
#define __PFILE_H__

extern BOOL gbValidSaveFile; // idb
extern char hero_names[MAX_CHARACTERS][PLR_NAME_LEN];






void __cdecl pfile_cpp_init();
void __cdecl pfile_init_save_directory();
void __fastcall pfile_check_available_space(char *pszDir);
void __cdecl pfile_write_hero();
unsigned int __fastcall pfile_get_save_num_from_name(const char *name);
void __fastcall pfile_encode_hero(const PkPlayerStruct *pPack);
BOOL __fastcall pfile_open_archive(BOOL a1, unsigned int save_num);
void __fastcall pfile_get_save_path(char *pszBuf, DWORD dwBufSize, unsigned int save_num);
void __fastcall pfile_flush(BOOL is_single_player, unsigned int save_num);
bool __fastcall pfile_create_player_description(char *dst, int len);
int __fastcall pfile_create_save_file(char *name_1, char *name_2);
void __cdecl pfile_flush_W();
void __fastcall game_2_ui_player(const PlayerStruct *p, _uiheroinfo *heroinfo, BOOL bHasSaveFile);
unsigned char __fastcall game_2_ui_class(const PlayerStruct *p);
BOOL __stdcall pfile_ui_set_hero_infos(BOOL (__stdcall *ui_add_hero_info)(_uiheroinfo *));
char *__fastcall GetSaveDirectory(char *dst, int dst_size, unsigned int save_num);
BOOL __fastcall pfile_read_hero(void *archive, PkPlayerStruct *pPack);
HANDLE __fastcall pfile_open_save_archive(int *unused, unsigned int save_num);
void __fastcall pfile_SFileCloseArchive(HANDLE hsArchive);
BOOL __fastcall pfile_archive_contains_game(HANDLE hsArchive);
BOOL __stdcall pfile_ui_set_class_stats(int player_class_nr, _uidefaultstats *class_stats);
char __fastcall pfile_get_player_class(unsigned int player_class_nr);
BOOL __stdcall pfile_ui_save_create(_uiheroinfo *heroinfo);
BOOL __stdcall pfile_get_file_name(DWORD lvl, char *dst);
BOOL __stdcall pfile_delete_save(_uiheroinfo *hero_info);
void __cdecl pfile_read_player_from_save();
void __fastcall GetTempLevelNames(char *szTemp);
void __fastcall GetPermLevelNames(char *szPerm);
void __fastcall pfile_get_game_name(char *dst);
void __cdecl pfile_remove_temp_files();
BOOL __stdcall GetTempSaveNames(DWORD dwIndex, char *szTemp);
void __cdecl pfile_rename_temp_to_perm();
BOOL __stdcall GetPermSaveNames(DWORD dwIndex, char *szPerm);
void __fastcall pfile_write_save_file(char *pszName, void *pbData, int dwLen, int qwLen);
void __fastcall pfile_strcpy(char *dst, const char *src);
char *__fastcall pfile_read(char *pszName, int *pdwLen);
void __fastcall pfile_update(BOOL force_save);

/* rdata */

extern const int pfile_inf; // weak

#endif /* __PFILE_H__ */
