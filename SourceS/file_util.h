#pragma once

#include <cstdint>

#if defined(_WIN64) || defined(_WIN32)
// Suppress definitions of `min` and `max` macros by <windows.h>:
#define NOMINMAX 1
#include <windows.h>
#endif

#if _POSIX_C_SOURCE >= 200112L || defined(_BSD_SOURCE) || defined(__APPLE__)
#include <unistd.h>
#else
#include <cstdio>
#endif

namespace dvl {

inline bool FileExists(const char *path)
{
#if _POSIX_C_SOURCE >= 200112L || defined(_BSD_SOURCE) || defined(__APPLE__)
	return ::access(path, F_OK) == 0;
#else
	auto *file = std::fopen(path, "rb");
	if (file == nullptr) return false;
	std::fclose(file);
	return true;
#endif
}

inline bool ResizeFile(const char *path, std::uint32_t size)
{
#if defined(_WIN64) || defined(_WIN32)
	LARGE_INTEGER lisize;
	lisize.QuadPart = static_cast<LONGLONG>(size);
	if (lisize.QuadPart < 0) {
		return false;
	}
	int path_utf16_size = MultiByteToWideChar(CP_UTF8, 0, path, -1, nullptr, 0);
	auto path_utf16 = new wchar_t[path_utf16_size];
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
	char name[DVL_MAX_PATH];
	TranslateFileName(name, sizeof(name), lpFileName);

	FILE *f = fopen(name, "r+");
	if (f) {
		fclose(f);
		remove(name);
		f = NULL;
		SDL_Log("Removed file: %s", name);
	} else {
		SDL_Log("Failed to remove file: %s", name);
	}
}

} // namespace dvl
