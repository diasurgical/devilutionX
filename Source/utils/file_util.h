#pragma once

#include <cstdint>
namespace devilution {

bool FileExists(const char *path);
bool GetFileSize(const char *path, std::uintmax_t *size);
bool ResizeFile(const char *path, std::uintmax_t size);
void RemoveFile(const char *lpFileName);

} // namespace devilution
