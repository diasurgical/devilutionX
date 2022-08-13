#pragma once

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <memory>
#include <string>

#include "utils/stdcompat/optional.hpp"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

bool FileExists(const char *path);
bool FileExistsAndIsWriteable(const char *path);
bool GetFileSize(const char *path, std::uintmax_t *size);
bool ResizeFile(const char *path, std::uintmax_t size);
void RemoveFile(const char *path);
std::optional<std::fstream> CreateFileStream(const char *path, std::ios::openmode mode);

#if (defined(_WIN64) || defined(_WIN32)) && !defined(NXDK)
std::unique_ptr<wchar_t[]> ToWideChar(string_view path);
#endif

} // namespace devilution
