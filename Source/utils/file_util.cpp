#include "utils/file_util.h"

#include <algorithm>
#include <string>

#include <SDL.h>

#include "utils/log.hpp"

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#if defined(_WIN64) || defined(_WIN32)
#include <memory>

// Suppress definitions of `min` and `max` macros by <windows.h>:
#define NOMINMAX 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifndef NXDK
#include <shlwapi.h>
#endif
#endif

#if (_POSIX_C_SOURCE >= 200112L || defined(_BSD_SOURCE) || defined(__APPLE__)) && !defined(NXDK)
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace devilution {

#if (defined(_WIN64) || defined(_WIN32)) && !defined(NXDK)
std::unique_ptr<wchar_t[]> ToWideChar(string_view path)
{
	constexpr std::uint32_t flags = MB_ERR_INVALID_CHARS;
	const int utf16Size = ::MultiByteToWideChar(CP_UTF8, flags, path.data(), path.size(), nullptr, 0);
	if (utf16Size == 0)
		return nullptr;
	std::unique_ptr<wchar_t[]> utf16 { new wchar_t[utf16Size + 1] };
	if (::MultiByteToWideChar(CP_UTF8, flags, path.data(), path.size(), &utf16[0], utf16Size) != utf16Size)
		return nullptr;
	utf16[utf16Size] = L'\0';
	return utf16;
}
#endif

bool FileExists(const char *path)
{
#if defined(_WIN64) || defined(_WIN32)
#if defined(NXDK)
	const bool exists = ::GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES;
#else
	const auto pathUtf16 = ToWideChar(path);
	if (pathUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return false;
	}
	const bool exists = ::PathFileExistsW(&pathUtf16[0]);
#endif
	if (!exists) {
		if (::GetLastError() == ERROR_FILE_NOT_FOUND || ::GetLastError() == ERROR_PATH_NOT_FOUND) {
			::SetLastError(ERROR_SUCCESS);
		} else {
#if defined(NXDK)
			LogError("GetFileAttributesA: error code {}", ::GetLastError());
#else
			LogError("PathFileExistsW: error code {}", ::GetLastError());
#endif
		}
		return false;
	}
	return true;
#elif (_POSIX_C_SOURCE >= 200112L || defined(_BSD_SOURCE) || defined(__APPLE__)) && !defined(__ANDROID__)
	return ::access(path, F_OK) == 0;
#else
	SDL_RWops *file = SDL_RWFromFile(path, "r+b");
	if (file == nullptr)
		return false;
	SDL_RWclose(file);
	return true;
#endif
}

bool FileExistsAndIsWriteable(const char *path)
{
#if defined(_WIN64) || defined(_WIN32)
#if defined(NXDK)
	const DWORD attr = ::GetFileAttributesA(path);
#else
	const auto pathUtf16 = ToWideChar(path);
	if (pathUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return false;
	}
	const DWORD attr = ::GetFileAttributesW(&pathUtf16[0]);
#endif
	const bool exists = attr != INVALID_FILE_ATTRIBUTES;
	if (!exists) {
		if (::GetLastError() == ERROR_FILE_NOT_FOUND || ::GetLastError() == ERROR_PATH_NOT_FOUND) {
			::SetLastError(ERROR_SUCCESS);
		} else {
#if defined(NXDK)
			LogError("GetFileAttributesA: error code {}", ::GetLastError());
#else
			LogError("GetFileAttributesW: error code {}", ::GetLastError());
#endif
		}
	}
	return exists && (attr & FILE_ATTRIBUTE_READONLY) == 0;
#elif (_POSIX_C_SOURCE >= 200112L || defined(_BSD_SOURCE) || defined(__APPLE__)) && !defined(__ANDROID__)
	return ::access(path, W_OK) == 0;
#else
	if (!FileExists(path))
		return false;
	SDL_RWops *file = SDL_RWFromFile(path, "a+b");
	if (file == nullptr)
		return false;
	SDL_RWclose(file);
	return true;
#endif
}

bool GetFileSize(const char *path, std::uintmax_t *size)
{
#if defined(_WIN64) || defined(_WIN32)
	WIN32_FILE_ATTRIBUTE_DATA attr;
#if defined(NXDK)
	if (!GetFileAttributesExA(path, GetFileExInfoStandard, &attr)) {
		return false;
	}
#else
	const auto pathUtf16 = ToWideChar(path);
	if (pathUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return false;
	}
	if (!GetFileAttributesExW(&pathUtf16[0], GetFileExInfoStandard, &attr)) {
		return false;
	}
#endif
	// C4293 in msvc when shifting a 32 bit type by 32 bits.
	*size = static_cast<std::uintmax_t>(attr.nFileSizeHigh) << (sizeof(attr.nFileSizeHigh) * 8) | attr.nFileSizeLow;
	return true;
#else
	struct ::stat statResult;
	if (::stat(path, &statResult) == -1)
		return false;
	*size = static_cast<uintmax_t>(statResult.st_size);
	return true;
#endif
}

bool ResizeFile(const char *path, std::uintmax_t size)
{
#if defined(_WIN64) || defined(_WIN32)
	LARGE_INTEGER lisize;
	lisize.QuadPart = static_cast<LONGLONG>(size);
	if (lisize.QuadPart < 0) {
		return false;
	}
#ifdef NXDK
	HANDLE file = ::CreateFileA(path, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
#else
	const auto pathUtf16 = ToWideChar(path);
	if (pathUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return false;
	}
	HANDLE file = ::CreateFileW(&pathUtf16[0], GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
#endif
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

void RemoveFile(const char *path)
{
#if defined(NXDK)
	::DeleteFileA(path);
#elif defined(_WIN64) || defined(_WIN32)
	const auto pathUtf16 = ToWideChar(path);
	if (pathUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return;
	}
	::DeleteFileW(&pathUtf16[0]);
#else
	std::string name { path };
	std::replace(name.begin(), name.end(), '\\', '/');
	FILE *f = fopen(name.c_str(), "r+");
	if (f != nullptr) {
		fclose(f);
		remove(name.c_str());
		f = nullptr;
		Log("Removed file: {}", name);
	} else {
		Log("Failed to remove file: {}", name);
	}
#endif
}

std::optional<std::fstream> CreateFileStream(const char *path, std::ios::openmode mode)
{
#if (defined(_WIN64) || defined(_WIN32)) && !defined(NXDK)
	const auto pathUtf16 = ToWideChar(path);
	if (pathUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return {};
	}
	return { std::fstream(pathUtf16.get(), mode) };
#else
	return { std::fstream(path, mode) };
#endif
}

} // namespace devilution
