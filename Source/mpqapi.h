/**
 * @file mpqapi.h
 *
 * Interface of functions for creating and editing MPQ files.
 */
#pragma once

#include <cstdint>

#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

struct _FILEHEADER {
	uint32_t signature;
	int headersize;
	uint32_t filesize;
	uint16_t version;
	int16_t sectorsizeid;
	int hashoffset;
	int blockoffset;
	int hashcount;
	int blockcount;
	uint8_t pad[72];
};

struct _HASHENTRY {
	uint32_t hashcheck[2];
	uint32_t lcid;
	int32_t block;
};

struct _BLOCKENTRY {
	uint32_t offset;
	uint32_t sizealloc;
	uint32_t sizefile;
	uint32_t flags;
};

void mpqapi_remove_hash_entry(const char *pszName);
void mpqapi_remove_hash_entries(bool (*fnGetName)(uint8_t, char *));
bool mpqapi_write_file(const char *pszName, const byte *pbData, size_t dwLen);
void mpqapi_rename(char *pszOld, char *pszNew);
bool mpqapi_has_file(const char *pszName);
bool OpenMPQ(const char *pszArchive);
bool mpqapi_flush_and_close(bool bFree);

} // namespace devilution
