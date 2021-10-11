/**
 * @file mpqapi.h
 *
 * Interface of functions for creating and editing MPQ files.
 */
#pragma once

#include <cstdint>

#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

void mpqapi_remove_hash_entry(const char *pszName);
void mpqapi_remove_hash_entries(bool (*fnGetName)(uint8_t, char *));
bool mpqapi_write_file(const char *pszName, const byte *pbData, size_t dwLen);
void mpqapi_rename(char *pszOld, char *pszNew);
bool mpqapi_has_file(const char *pszName);
bool OpenMPQ(const char *pszArchive);
bool mpqapi_flush_and_close(bool bFree);

} // namespace devilution
