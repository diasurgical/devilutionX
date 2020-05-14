#pragma once

#include <algorithm>
#include <string>
#include <cstdint>

#include <SDL.h>

#ifdef USE_SDL1
#include "sdl2_to_1_2_backports.h"
#endif

#if defined(_WIN64) || defined(_WIN32)
// Suppress definitions of `min` and `max` macros by <windows.h>:
#define NOMINMAX 1
#include <windows.h>
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

inline bool GetFileSize(const char *path, std::uintmax_t *size)
{
#if defined(_WIN64) || defined(_WIN32)
	WIN32_FILE_ATTRIBUTE_DATA attr;
	int path_utf16_size = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
	wchar_t* path_utf16 = new wchar_t[path_utf16_size];
	if (MultiByteToWideChar(CP_UTF8, 0, path, -1, path_utf16, path_utf16_size) != path_utf16_size) {
		delete[] path_utf16;
		return false;
	}
	if (!GetFileAttributesExW(path_utf16, GetFileExInfoStandard, &attr)) {
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

inline bool ResizeFile(const char *path, std::uintmax_t size)
{
#if defined(_WIN64) || defined(_WIN32)
	LARGE_INTEGER lisize;
	lisize.QuadPart = static_cast<LONGLONG>(size);
	if (lisize.QuadPart < 0) {
		return false;
	}
	int path_utf16_size = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
	wchar_t *path_utf16 = new wchar_t[path_utf16_size];
	if (MultiByteToWideChar(CP_UTF8, 0, path, -1, path_utf16, path_utf16_size) != path_utf16_size) {
		delete[] path_utf16;
		return false;
	}
	HANDLE file = ::CreateFileW(path_utf16, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	delete[] path_utf16;
	if (file == INVALID_HANDLE_VALUE) {
		return false;
	} else if (::SetFilePointerEx(file, lisize, NULL, FILE_BEGIN) == 0 || ::SetEndOfFile(file) == 0) {
		::CloseHandle(file);
		return false;
	}
	::CloseHandle(file);
	return true;
#elif _POSIX_C_SOURCE >= 200112L || defined(_BSD_SOURCE) || defined(__APPLE__)
	return ::truncate(path, static_cast<off_t>(size)) == 0;
#else
	static_assert(false, "truncate not implemented for the current platform");
#endif
}

inline void RemoveFile(char *lpFileName)
{
	std::string name = lpFileName;
	std::replace(name.begin(), name.end(), '\\', '/');
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
