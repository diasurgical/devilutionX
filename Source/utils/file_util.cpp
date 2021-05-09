#include "utils/file_util.h"
#include "utils/log.hpp"

#include <algorithm>
#include <string>

#include <SDL.h>

#include "utils/stdcompat/string_view.hpp"

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#if defined(_WIN64) || defined(_WIN32)
#include <memory>

// Suppress definitions of `min` and `max` macros by <windows.h>:
#define NOMINMAX 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>

#include "utils/log.hpp"
#endif

#if _POSIX_C_SOURCE >= 200112L || defined(_BSD_SOURCE) || defined(__APPLE__)
#include <sys/stat.h>
#include <unistd.h>
#else
#include <cstdio>
#endif

#include "storm/storm.h"

namespace devilution {

namespace {

#if defined(_WIN64) || defined(_WIN32)
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

} // namespace

bool FileExists(const char *path)
{
#if defined(_WIN64) || defined(_WIN32)
	const auto pathUtf16 = ToWideChar(path);
	if (pathUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return false;
	}
	if (!::PathFileExistsW(&pathUtf16[0])) {
		if (::GetLastError() == ERROR_FILE_NOT_FOUND) {
			::SetLastError(ERROR_SUCCESS);
		} else {
			LogError("PathFileExistsW: error code {}", ::GetLastError());
		}
		return false;
	}
	return true;
#elif _POSIX_C_SOURCE >= 200112L || defined(_BSD_SOURCE) || defined(__APPLE__)
	return ::access(path, F_OK) == 0;
#else
	FILE *file = std::fopen(path, "rb");
	if (file == NULL)
		return false;
	std::fclose(file);
	return true;
#endif
}

bool GetFileSize(const char *path, std::uintmax_t *size)
{
#if defined(_WIN64) || defined(_WIN32)
	const auto pathUtf16 = ToWideChar(path);
	if (pathUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return false;
	}
	WIN32_FILE_ATTRIBUTE_DATA attr;
	if (!GetFileAttributesExW(&pathUtf16[0], GetFileExInfoStandard, &attr)) {
		return false;
	}
	*size = (attr.nFileSizeHigh) << (sizeof(attr.nFileSizeHigh) * 8) | attr.nFileSizeLow;
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
	const auto pathUtf16 = ToWideChar(path);
	if (pathUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return false;
	}
	HANDLE file = ::CreateFileW(&pathUtf16[0], GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
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

void RemoveFile(const char *lpFileName)
{
#if defined(_WIN64) || defined(_WIN32)
	const auto pathUtf16 = ToWideChar(lpFileName);
	if (pathUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return;
	}
	::DeleteFileW(&pathUtf16[0]);
#else
	std::string name = lpFileName;
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

std::unique_ptr<std::fstream> CreateFileStream(const char *path, std::ios::openmode mode)
{
#if defined(_WIN64) || defined(_WIN32)
	const auto pathUtf16 = ToWideChar(path);
	if (pathUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return nullptr;
	}
	return std::make_unique<std::fstream>(pathUtf16.get(), mode);
#else
	return std::make_unique<std::fstream>(path, mode);
#endif
}

bool SFileOpenArchiveDiablo(const char *szMpqName, DWORD dwPriority, DWORD dwFlags, HANDLE *phMpq)
{
#if defined(_WIN64) || defined(_WIN32)
	const auto szMpqNameUtf16 = ToWideChar(szMpqName);
	if (szMpqNameUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return false;
	}
	return SFileOpenArchive(szMpqNameUtf16.get(), dwPriority, dwFlags, phMpq);
#else
	return SFileOpenArchive(szMpqName, dwPriority, dwFlags, phMpq);
#endif
}

} // namespace devilution
