//HEADER_GOES_HERE
#ifndef __MPQAPI_H__
#define __MPQAPI_H__

extern int mpqapi_cpp_init_value; // weak
extern int sgdwMpqOffset; // idb
extern char mpq_buf[4096];
extern _HASHENTRY *sgpHashTbl;
extern bool save_archive_modified; // weak
extern _BLOCKENTRY *sgpBlockTbl;
extern bool save_archive_open; // weak

void __cdecl mpqapi_cpp_init();
bool __fastcall mpqapi_set_hidden(char *pszArchive, bool hidden);
void __fastcall mpqapi_store_creation_time(char *pszArchive, int dwChar);
bool __fastcall mpqapi_reg_load_modification_time(char *dst, int size);
void __fastcall mpqapi_xor_buf(char *pbData);
bool __fastcall mpqapi_reg_store_modification_time(char *pbData, int dwLen);
_BLOCKENTRY *__fastcall j_mpqapi_remove_hash_entry(char *pszName);
void __fastcall mpqapi_remove_hash_entry(char *pszName);
void __fastcall mpqapi_free_block(int block_offset, int block_size);
_BLOCKENTRY *__fastcall mpqapi_new_block(int *block_index);
int __fastcall mpqapi_get_hash_index_of_path(char *pszName);
int __fastcall mpqapi_get_hash_index(short index, int hash_a, int hash_b, int locale);
void __fastcall mpqapi_remove_hash_entries(bool (__stdcall *fnGetName)(int, char *));
bool __fastcall mpqapi_write_file(char *pszName, char *pbData, int dwLen);
_BLOCKENTRY *__fastcall mpqapi_add_file(char *pszName, _BLOCKENTRY *pBlk, int block_index);
bool __fastcall mpqapi_write_file_contents(char *pszName, char *pbData, int dwLen, _BLOCKENTRY *pBlk);
int __fastcall mpqapi_find_free_block(int size, int *block_size);
void __fastcall mpqapi_rename(char *pszOld, char *pszNew);
bool __fastcall mpqapi_has_file(char *pszName);
bool __fastcall mpqapi_open_archive(char *pszArchive, bool hidden, int dwChar);
bool __fastcall mpqapi_parse_archive_header(_FILEHEADER *pHdr, int *pdwNextFileStart);
void __fastcall mpqapi_close_archive(char *pszArchive, bool bFree, int dwChar);
void __fastcall mpqapi_store_modified_time(char *pszArchive, int dwChar);
void __fastcall mpqapi_flush_and_close(char *pszArchive, bool bFree, int dwChar);
bool __cdecl mpqapi_write_header();
bool __cdecl mpqapi_write_block_table();
bool __cdecl mpqapi_write_hash_table();
bool __cdecl mpqapi_can_seek();

/* rdata */

extern const int mpqapi_inf; // weak

/* data */

extern HANDLE sghArchive; // idb

#endif /* __MPQAPI_H__ */
