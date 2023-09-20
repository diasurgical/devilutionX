#include "utils/file_util.h"

#include <cerrno>
#include <cstdint>
#include <cstring>

#include <algorithm>
#include <string>
#include <vector>

#include <SDL.h>

#include "utils/log.hpp"
#include "utils/stdcompat/filesystem.hpp"
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

#ifndef NXDK
#include <shlwapi.h>
#endif
#endif

#if (_POSIX_C_SOURCE >= 200112L || defined(_BSD_SOURCE) || defined(__APPLE__)) && !defined(NXDK)
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifdef __APPLE__
#include <copyfile.h>
#endif

// We include sys/stat.h for mkdir where available.
#if !defined(DVL_HAS_FILESYSTEM) && defined(__has_include) && !defined(_WIN32)
#if __has_include(<sys/stat.h>)
#include <sys/stat.h>
#endif
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

string_view Dirname(string_view path)
{
	while (path.size() > 1 && path.back() == DirectorySeparator)
		path.remove_suffix(1);
	if (path.size() == 1 && path.back() == DirectorySeparator)
		return DIRECTORY_SEPARATOR_STR;
	const size_t sep = path.find_last_of(DIRECTORY_SEPARATOR_STR);
	if (sep == string_view::npos)
		return ".";
	return string_view { path.data(), sep };
}

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
			LogError("GetFileAttributesA({}): error code {}", path, ::GetLastError());
#else
			LogError("PathFileExistsW({}): error code {}", path, ::GetLastError());
#endif
		}
		return false;
	}
	return true;
#elif (_POSIX_C_SOURCE >= 200112L || defined(_BSD_SOURCE) || defined(__APPLE__)) && !defined(__ANDROID__)
	return ::access(path, F_OK) == 0;
#elif defined(DVL_HAS_FILESYSTEM)
	std::error_code ec;
	return std::filesystem::exists(std::filesystem::u8path(path), ec);
#else
	SDL_RWops *file = SDL_RWFromFile(path, "r+b");
	if (file == nullptr)
		return false;
	SDL_RWclose(file);
	return true;
#endif
}

#if defined(_WIN64) || defined(_WIN32)
namespace {
DWORD WindowsGetFileAttributes(const char *path)
{
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
	if (attr == INVALID_FILE_ATTRIBUTES) {
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
	return attr;
}
} // namespace

#endif

bool DirectoryExists(const char *path)
{
#if defined(_WIN64) || defined(_WIN32)
	const DWORD attr = WindowsGetFileAttributes(path);
	return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
#elif defined(DVL_HAS_FILESYSTEM)
	std::error_code error;
	return std::filesystem::is_directory(std::filesystem::u8path(path), error);
#elif (_POSIX_C_SOURCE >= 200112L || defined(_BSD_SOURCE) || defined(__APPLE__)) && !defined(__ANDROID__)
	struct ::stat statResult;
	return ::stat(path, &statResult) == 0 && S_ISDIR(statResult.st_mode);
#endif
}

bool FileExistsAndIsWriteable(const char *path)
{
#if defined(_WIN64) || defined(_WIN32)
	const DWORD attr = WindowsGetFileAttributes(path);
	return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_READONLY) == 0;
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

bool CreateDir(const char *path)
{
#ifdef DVL_HAS_FILESYSTEM
	std::error_code error;
	std::filesystem::create_directory(std::filesystem::u8path(path), error);
	if (error) {
		LogError("failed to create directory {}: {}", path, error.message());
		return false;
	}
	return true;
#elif defined(_WIN64) || defined(_WIN32)
#ifdef NXDK
	if (::CreateDirectoryA(path, /*lpSecurityAttributes=*/nullptr) != 0)
		return true;
	if (::GetLastError() == ERROR_ALREADY_EXISTS)
		return true;
	LogError("failed to create directory {}", path);
	return false;
#else
	const auto pathUtf16 = ToWideChar(path);
	if (pathUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return false;
	}
	if (::CreateDirectoryW(&pathUtf16[0], /*lpSecurityAttributes=*/nullptr) != 0)
		return true;
	if (::GetLastError() == ERROR_ALREADY_EXISTS)
		return true;
	LogError("failed to create directory {}", path);
	return false;
#endif
#else
	const int result = ::mkdir(path, 0777);
	if (result != 0 && result != EEXIST) {
		LogError("failed to create directory {}", path);
		return false;
	}
	return true;
#endif
}

void RecursivelyCreateDir(const char *path)
{
#ifdef DVL_HAS_FILESYSTEM
	std::error_code error;
	std::filesystem::create_directories(std::filesystem::u8path(path), error);
	if (error) {
		LogError("failed to create directory {}: {}", path, error.message());
	}
#else
	if (DirectoryExists(path))
		return;
	std::vector<std::string> paths;
	std::string cur { path };
	do {
		paths.push_back(cur);
		string_view parent = Dirname(cur);
		if (parent == cur)
			break;
		cur.assign(parent.data(), parent.size());
	} while (!DirectoryExists(cur.c_str()));
	for (auto it = std::rbegin(paths); it != std::rend(paths); ++it) {
		if (!CreateDir(it->c_str()))
			return;
	}
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

void RenameFile(const char *from, const char *to)
{
#if defined(NXDK)
	::MoveFile(from, to);
#elif defined(_WIN64) || defined(_WIN32)
	const auto fromUtf16 = ToWideChar(from);
	const auto toUtf16 = ToWideChar(to);
	if (fromUtf16 == nullptr || toUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return;
	}
	::MoveFileW(&fromUtf16[0], &toUtf16[0]);
#elif defined(DVL_HAS_FILESYSTEM)
	std::error_code ec;
	std::filesystem::rename(std::filesystem::u8path(from), std::filesystem::u8path(to), ec);
#else
	::rename(from, to);
#endif
}

void CopyFileOverwrite(const char *from, const char *to)
{
#if defined(NXDK)
	if (!::CopyFile(from, to, /*bFailIfExists=*/false)) {
		LogError("Failed to copy {} to {}", from, to);
	}
#elif defined(_WIN64) || defined(_WIN32)
	const auto fromUtf16 = ToWideChar(from);
	const auto toUtf16 = ToWideChar(to);
	if (fromUtf16 == nullptr || toUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return;
	}
	if (!::CopyFileW(&fromUtf16[0], &toUtf16[0], /*bFailIfExists=*/false)) {
		LogError("Failed to copy {} to {}", from, to);
	}
#elif defined(__APPLE__)
	::copyfile(from, to, nullptr, COPYFILE_ALL);
#elif defined(DVL_HAS_FILESYSTEM)
	std::error_code error;
	std::filesystem::copy_file(std::filesystem::u8path(from), std::filesystem::u8path(to), std::filesystem::copy_options::overwrite_existing, error);
	if (error) {
		LogError("Failed to copy {} to {}: {}", from, to, error.message());
	}
#else
	FILE *infile = OpenFile(from, "rb");
	if (infile == nullptr) {
		LogError("Failed to open {} for reading: {}", from, std::strerror(errno));
		return;
	}
	FILE *outfile = OpenFile(to, "wb");
	if (outfile == nullptr) {
		LogError("Failed to open {} for writing: {}", to, std::strerror(errno));
		std::fclose(infile);
		return;
	}
	char buffer[4096];
	size_t numRead;
	while ((numRead = std::fread(buffer, sizeof(char), sizeof(buffer), infile)) > 0) {
		if (std::fwrite(buffer, sizeof(char), numRead, outfile) != numRead) {
			LogError("Write failed {}: {}", to, std::strerror(errno));
			break;
		}
	}
	std::fclose(infile);
	std::fclose(outfile);
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
		LogVerbose("Removed file: {}", name);
	} else {
		LogVerbose("Failed to remove file: {}", name);
	}
#endif
}

FILE *OpenFile(const char *path, const char *mode)
{
#if (defined(_WIN64) || defined(_WIN32)) && !defined(NXDK)
	std::unique_ptr<wchar_t[]> pathUtf16;
	std::unique_ptr<wchar_t[]> modeUtf16;
	if ((pathUtf16 = ToWideChar(path)) == nullptr
	    || (modeUtf16 = ToWideChar(mode)) == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return {};
	}
	return _wfopen(pathUtf16.get(), modeUtf16.get());
#else
	return std::fopen(path, mode);
#endif
}

} // namespace devilution
