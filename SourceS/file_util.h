#pragma once

#include <algorithm>
#include <string>
#ifndef _XBOX
#include <cstdint>
#endif

#include <SDL.h>

#ifdef USE_SDL1
#include "sdl2_to_1_2_backports.h"
#endif

#if 0//defined(_WIN64) || defined(_WIN32)
// Suppress definitions of `min` and `max` macros by <windows.h>:
#define NOMINMAX 1
#include <windows.h>
#else
#include <xtl.h>
#endif

#if _POSIX_C_SOURCE >= 200112L || defined(_BSD_SOURCE) || defined(__APPLE__)
#include <unistd.h>
#include <sys/stat.h>
#else
#include <cstdio>
#endif

namespace dvl {

inline bool FileExists(const char *path)
{
#if _POSIX_C_SOURCE >= 200112L || defined(_BSD_SOURCE) || defined(__APPLE__)
	return ::access(path, F_OK) == 0;
#else
	FILE *file = std::fopen(path, "rb");
	if (file == NULL) return false;
	std::fclose(file);
	return true;
#endif
}

inline bool GetFileSize(const char *path, size_t *size)
{
#if 1//defined(_WIN64) || defined(_WIN32)
	WIN32_FILE_ATTRIBUTE_DATA attr;
	int path_utf16_size = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
	wchar_t* path_utf16 = new wchar_t[path_utf16_size];
	if (MultiByteToWideChar(CP_UTF8, 0, path, -1, path_utf16, path_utf16_size) != path_utf16_size) {
		delete[] path_utf16;
		return false;
	}
	std::wstring tmp1(path_utf16);
	std::string mystr(tmp1.begin(), tmp1.end());
	if (!GetFileAttributesEx(mystr.c_str(), GetFileExInfoStandard, &attr)) {
		delete[] path_utf16;
		return false;
	}
	delete[] path_utf16;
	*size = (attr.nFileSizeHigh) << (sizeof(attr.nFileSizeHigh) * 8) | attr.nFileSizeLow;
	return true;
#else
	struct ::stat stat_result;
	if (::stat(path, &stat_result) == -1)
		return false;
	*size = static_cast<uintmax_t>(stat_result.st_size);
	return true;
#endif
}

inline void RemoveFile(char *lpFileName)
{
	std::string name = lpFileName;
#ifndef _XBOX
	std::replace(name.begin(), name.end(), '\\', '/');
#endif
	FILE *f = fopen(name.c_str(), "r+");
	if (f) {
		fclose(f);
		remove(name.c_str());
		f = NULL;
		SDL_Log("Removed file: %s", name.c_str());
	} else {
		SDL_Log("Failed to remove file: %s", name.c_str());
	}
}

} // namespace dvl
