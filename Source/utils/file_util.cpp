#include "utils/file_util.h"

#include <algorithm>
#include <string>

#include <SDL.h>

#include "utils/log.hpp"

#include <memory>

// Suppress definitions of `min` and `max` macros by <windows.h>:
#define NOMINMAX 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>

#include "utils/log.hpp"

namespace devilution {

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

bool FileExists(const char *path)
{
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
}

bool FileExistsAndIsWriteable(const char *path)
{
	const auto pathUtf16 = ToWideChar(path);
	if (pathUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return false;
	}
	return ::GetFileAttributesW(&pathUtf16[0]) != INVALID_FILE_ATTRIBUTES && (::GetFileAttributesW(&pathUtf16[0]) & FILE_ATTRIBUTE_READONLY) == 0;
}

bool GetFileSize(const char *path, std::uintmax_t *size)
{
	const auto pathUtf16 = ToWideChar(path);
	if (pathUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return false;
	}
	WIN32_FILE_ATTRIBUTE_DATA attr;
	if (!GetFileAttributesExW(&pathUtf16[0], GetFileExInfoStandard, &attr)) {
		return false;
	}
	// C4293 in msvc when shifting a 32 bit type by 32 bits.
	*size = static_cast<std::uintmax_t>(attr.nFileSizeHigh) << (sizeof(attr.nFileSizeHigh) * 8) | attr.nFileSizeLow;
	return true;
}

bool ResizeFile(const char *path, std::uintmax_t size)
{
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
}

void RemoveFile(const char *lpFileName)
{
	const auto pathUtf16 = ToWideChar(lpFileName);
	if (pathUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return;
	}
	::DeleteFileW(&pathUtf16[0]);
}

std::optional<std::fstream> CreateFileStream(const char *path, std::ios::openmode mode)
{
	const auto pathUtf16 = ToWideChar(path);
	if (pathUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return {};
	}
	return { std::fstream(pathUtf16.get(), mode) };
}

FILE *FOpen(const char *path, const char *mode)
{
	const auto pathUtf16 = ToWideChar(path);
	if (pathUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return nullptr;
	}
	const auto modeUtf16 = ToWideChar(mode);
	if (modeUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return nullptr;
	}
	return ::_wfopen(&pathUtf16[0], &modeUtf16[0]);
}

} // namespace devilution
