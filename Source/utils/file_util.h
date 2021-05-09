#pragma once

#include <cstdint>
#include <fstream>
#include <memory>

#include "miniwin/miniwin.h"

namespace devilution {

bool FileExists(const char *path);
bool GetFileSize(const char *path, std::uintmax_t *size);
bool ResizeFile(const char *path, std::uintmax_t size);
void RemoveFile(const char *lpFileName);
std::unique_ptr<std::fstream> CreateFileStream(const char *path, std::ios::openmode mode);
bool SFileOpenArchiveDiablo(const char *szMpqName, DWORD dwPriority, DWORD dwFlags, HANDLE *phMpq);

} // namespace devilution
