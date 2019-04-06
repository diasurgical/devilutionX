//HEADER_GOES_HERE
#ifndef __MPQAPI_H__
#define __MPQAPI_H__

extern char mpq_buf[4096];
extern BOOL save_archive_modified; // weak
extern BOOLEAN save_archive_open;     // weak

BOOL __fastcall mpqapi_set_hidden(const char *pszArchive, BOOL hidden);
void __fastcall mpqapi_store_creation_time(const char *pszArchive, int dwChar);
BOOL __fastcall mpqapi_reg_load_modification_time(char *dst, int size);
void __fastcall mpqapi_xor_buf(char *pbData);
void __fastcall mpqapi_update_multi_creation_time(DWORD dwChar);
BOOLEAN __fastcall mpqapi_reg_store_modification_time(char *pbData, DWORD dwLen);
_BLOCKENTRY *__fastcall j_mpqapi_remove_hash_entry(char *pszName);
void __fastcall mpqapi_remove_hash_entry(const char *pszName);
void __fastcall mpqapi_free_block(int block_offset, int block_size);
_BLOCKENTRY *__fastcall mpqapi_new_block(int *block_index);
int __fastcall mpqapi_get_hash_index_of_path(const char *pszName);
int __fastcall mpqapi_get_hash_index(short index, int hash_a, int hash_b, int locale);
void __fastcall mpqapi_remove_hash_entries(BOOL(__stdcall *fnGetName)(DWORD, char *));
BOOL __fastcall mpqapi_write_file(const char *pszName, const BYTE *pbData, DWORD dwLen);
_BLOCKENTRY *__fastcall mpqapi_add_file(const char *pszName, _BLOCKENTRY *pBlk, int block_index);
BOOL __fastcall mpqapi_write_file_contents(const char *pszName, const BYTE *pbData, int dwLen, _BLOCKENTRY *pBlk);
int __fastcall mpqapi_find_free_block(int size, int *block_size);
void __fastcall mpqapi_rename(char *pszOld, char *pszNew);
BOOL __fastcall mpqapi_has_file(const char *pszName);
BOOL __fastcall mpqapi_open_archive(const char *pszArchive, BOOL hidden, int dwChar);
BOOLEAN __fastcall mpqapi_parse_archive_header(_FILEHEADER *pHdr, int *pdwNextFileStart);
void __fastcall mpqapi_close_archive(const char *pszArchive, BOOL bFree, int dwChar);
void __fastcall mpqapi_store_modified_time(const char *pszArchive, int dwChar);
void __fastcall mpqapi_flush_and_close(const char *pszArchive, BOOL bFree, int dwChar);
BOOLEAN __cdecl mpqapi_write_header();
BOOLEAN __cdecl mpqapi_write_block_table();
BOOLEAN __cdecl mpqapi_write_hash_table();
BOOLEAN __cdecl mpqapi_can_seek();

/* rdata */

/* data */

#endif /* __MPQAPI_H__ */
