#include <SDL.h>
#include <SDL_endian.h>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>

#include "DiabloUI/diabloui.h"
#include "options.h"
#include "storm/storm.h"
#include "utils/file_util.h"
#include "utils/log.hpp"
#include "utils/paths.h"
#include "utils/sdl_mutex.h"
#include "utils/stubs.h"
#include "utils/stdcompat/optional.hpp"

// Include Windows headers for Get/SetLastError.
#if defined(_WIN32)
// Suppress definitions of `min` and `max` macros by <windows.h>:
#define NOMINMAX 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else // !defined(_WIN32)
// On non-Windows, these are defined in 3rdParty/StormLib.
extern "C" void SetLastError(std::uint32_t dwErrCode);
extern "C" std::uint32_t GetLastError();
#endif

namespace devilution {
namespace {

SdlMutex Mutex;

} // namespace

bool SFileReadFileThreadSafe(HANDLE hFile, void *buffer, size_t nNumberOfBytesToRead, size_t *read, int *lpDistanceToMoveHigh)
{
	const std::lock_guard<SdlMutex> lock(Mutex);
	return SFileReadFile(hFile, buffer, nNumberOfBytesToRead, (unsigned int *)read, lpDistanceToMoveHigh);
}

bool SFileCloseFileThreadSafe(HANDLE hFile)
{
	const std::lock_guard<SdlMutex> lock(Mutex);
	return SFileCloseFile(hFile);
}

bool SFileOpenFile(const char *filename, HANDLE *phFile)
{
	bool result = false;

	if (!result && font_mpq != nullptr) {
		result = SFileOpenFileEx((HANDLE)font_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
	}
	if (!result && lang_mpq != nullptr) {
		result = SFileOpenFileEx((HANDLE)lang_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
	}
	if (!result && devilutionx_mpq != nullptr) {
		result = SFileOpenFileEx((HANDLE)devilutionx_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
	}
	if (gbIsHellfire) {
		if (!result && hfvoice_mpq != nullptr) {
			result = SFileOpenFileEx((HANDLE)hfvoice_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
		}
		if (!result && hfmusic_mpq != nullptr) {
			result = SFileOpenFileEx((HANDLE)hfmusic_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
		}
		if (!result && hfbarb_mpq != nullptr) {
			result = SFileOpenFileEx((HANDLE)hfbarb_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
		}
		if (!result && hfbard_mpq != nullptr) {
			result = SFileOpenFileEx((HANDLE)hfbard_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
		}
		if (!result && hfmonk_mpq != nullptr) {
			result = SFileOpenFileEx((HANDLE)hfmonk_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
		}
		if (!result) {
			result = SFileOpenFileEx((HANDLE)hellfire_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
		}
	}
	if (!result && spawn_mpq != nullptr) {
		result = SFileOpenFileEx((HANDLE)spawn_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
	}
	if (!result && diabdat_mpq != nullptr) {
		result = SFileOpenFileEx((HANDLE)diabdat_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
	}

	return result;
}

uint32_t SErrGetLastError()
{
	return ::GetLastError();
}

void SErrSetLastError(uint32_t dwErrCode)
{
	::SetLastError(dwErrCode);
}

#if defined(_WIN64) || defined(_WIN32)
bool SFileOpenArchive(const char *szMpqName, DWORD dwPriority, DWORD dwFlags, HANDLE *phMpq)
{
	const auto szMpqNameUtf16 = ToWideChar(szMpqName);
	if (szMpqNameUtf16 == nullptr) {
		LogError("UTF-8 -> UTF-16 conversion error code {}", ::GetLastError());
		return false;
	}
	return SFileOpenArchive(szMpqNameUtf16.get(), dwPriority, dwFlags, phMpq);
}
#endif

} // namespace devilution
