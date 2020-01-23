//HEADER_GOES_HERE
#ifndef __MPQAPI_H__
#define __MPQAPI_H__

extern char mpq_buf[4096];
extern BOOL save_archive_modified;
extern BOOLEAN save_archive_open;

void mpqapi_store_creation_time(const char *pszArchive, DWORD dwChar);
BOOL mpqapi_reg_load_modification_time(char *dst, int size);
void mpqapi_xor_buf(char *pbData);
void mpqapi_store_default_time(DWORD dwChar);
BOOLEAN mpqapi_reg_store_modification_time(char *pbData, DWORD dwLen);
void mpqapi_remove_hash_entry(const char *pszName);
void mpqapi_alloc_block(int block_offset, int block_size);
_BLOCKENTRY *mpqapi_new_block(int *block_index);
int FetchHandle(const char *pszName);
int mpqapi_get_hash_index(short index, int hash_a, int hash_b, int locale);
void mpqapi_remove_hash_entries(BOOL(*fnGetName)(DWORD, char *));
BOOL mpqapi_write_file(const char *pszName, const BYTE *pbData, DWORD dwLen);
_BLOCKENTRY *mpqapi_add_file(const char *pszName, _BLOCKENTRY *pBlk, int block_index);
BOOL mpqapi_write_file_contents(const char *pszName, const BYTE *pbData, DWORD dwLen, _BLOCKENTRY *pBlk);
int mpqapi_find_free_block(int size, int *block_size);
void mpqapi_rename(char *pszOld, char *pszNew);
BOOL mpqapi_has_file(const char *pszName);
BOOL OpenMPQ(const char *pszArchive, DWORD dwChar);
BOOL ParseMPQHeader(_FILEHEADER *pHdr, DWORD *pdwNextFileStart);
void CloseMPQ(const char *pszArchive, BOOL bFree, DWORD dwChar);
void mpqapi_store_modified_time(const char *pszArchive, DWORD dwChar);
BOOL mpqapi_flush_and_close(const char *pszArchive, BOOL bFree, DWORD dwChar);
BOOL WriteMPQHeader();
BOOL mpqapi_write_block_table();
BOOL mpqapi_write_hash_table();
BOOL mpqapi_can_seek();

/* rdata */

/* data */

#endif /* __MPQAPI_H__ */
