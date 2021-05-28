#include <SDL.h>
#include <SDL_endian.h>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>

#define SI_SUPPORT_IOSTREAMS
#include <SimpleIni.h>

#include "storm/storm.h"
#include "DiabloUI/diabloui.h"
#include "options.h"
#include "utils/file_util.h"
#include "utils/log.hpp"
#include "utils/paths.h"
#include "utils/sdl_mutex.h"
#include "utils/stubs.h"

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

bool directFileAccess = false;
std::string *SBasePath = nullptr;

SdlMutex Mutex;

std::string getIniPath()
{
	auto path = paths::ConfigPath() + std::string("diablo.ini");
	return path;
}

CSimpleIni &getIni()
{
	static CSimpleIni ini;
	static bool isIniLoaded = false;
	if (!isIniLoaded) {
		auto path = getIniPath();
		auto stream = CreateFileStream(path.c_str(), std::fstream::in | std::fstream::binary);
		ini.SetSpaces(false);
		if (stream != nullptr)
			ini.LoadData(*stream);
		isIniLoaded = true;
	}
	return ini;
}

} // namespace

bool SFileReadFileThreadSafe(HANDLE hFile, void *buffer, DWORD nNumberOfBytesToRead, DWORD *read, int *lpDistanceToMoveHigh)
{
	const std::lock_guard<SdlMutex> lock(Mutex);
	return SFileReadFile(hFile, buffer, nNumberOfBytesToRead, read, lpDistanceToMoveHigh);
}

bool SFileCloseFileThreadSafe(HANDLE hFile)
{
	const std::lock_guard<SdlMutex> lock(Mutex);
	return SFileCloseFile(hFile);
}

// Converts ASCII characters to lowercase
// Converts slash (0x2F) / backslash (0x5C) to system file-separator
unsigned char AsciiToLowerTable_Path[256] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
#ifdef _WIN32
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x5C,
#else
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
#endif
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
#ifdef _WIN32
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
#else
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x5B, 0x2F, 0x5D, 0x5E, 0x5F,
#endif
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

bool SFileOpenFile(const char *filename, HANDLE *phFile)
{
	bool result = false;

	if (directFileAccess && SBasePath != nullptr) {
		std::string path = *SBasePath + filename;
		for (std::size_t i = SBasePath->size(); i < path.size(); ++i)
			path[i] = AsciiToLowerTable_Path[static_cast<unsigned char>(path[i])];
		result = SFileOpenFileEx((HANDLE) nullptr, path.c_str(), SFILE_OPEN_LOCAL_FILE, phFile);
	}

	if (!result && devilutionx_mpq != nullptr) {
		result = SFileOpenFileEx((HANDLE)devilutionx_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
	}
	if (gbIsHellfire) {
		if (!result && hfopt2_mpq != nullptr) {
			result = SFileOpenFileEx((HANDLE)hfopt2_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
		}
		if (!result && hfopt1_mpq != nullptr) {
			result = SFileOpenFileEx((HANDLE)hfopt1_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
		}
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
	if (!result && patch_rt_mpq != nullptr) {
		result = SFileOpenFileEx((HANDLE)patch_rt_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
	}
	if (!result && spawn_mpq != nullptr) {
		result = SFileOpenFileEx((HANDLE)spawn_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
	}
	if (!result && diabdat_mpq != nullptr) {
		result = SFileOpenFileEx((HANDLE)diabdat_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
	}

	if (!result || (*phFile == nullptr)) {
		const auto error = SErrGetLastError();
		if (error == STORM_ERROR_FILE_NOT_FOUND) {
			LogVerbose("{}(\"{}\") File not found", __FUNCTION__, filename);
		} else {
			LogError("{}(\"{}\") Failed with error code {}", __FUNCTION__, filename, error);
		}
	}
	return result;
}

bool getIniBool(const char *sectionName, const char *keyName, bool defaultValue)
{
	bool value = getIni().GetBoolValue(sectionName, keyName, defaultValue);
	return value;
}

float getIniFloat(const char *sectionName, const char *keyName, float defaultValue)
{
	const double value = getIni().GetDoubleValue(sectionName, keyName, defaultValue);
	return (float)value;
}

bool getIniValue(const char *sectionName, const char *keyName, char *string, int stringSize, const char *defaultString)
{
	const char *value = getIni().GetValue(sectionName, keyName);
	if (value == nullptr) {
		strncpy(string, defaultString, stringSize);
		return false;
	}
	strncpy(string, value, stringSize);
	return true;
}

void setIniValue(const char *sectionName, const char *keyName, const char *value, int len)
{
	auto &ini = getIni();
	std::string stringValue(value, len ? len : strlen(value));
	ini.SetValue(sectionName, keyName, stringValue.c_str());
}

void SaveIni()
{
	auto iniPath = getIniPath();
	auto stream = CreateFileStream(iniPath.c_str(), std::fstream::out | std::fstream::trunc | std::fstream::binary);
	getIni().Save(*stream, true);
}

int getIniInt(const char *keyname, const char *valuename, int defaultValue)
{
	long value = getIni().GetLongValue(keyname, valuename, defaultValue);
	return value;
}

void setIniInt(const char *keyname, const char *valuename, int value)
{
	getIni().SetLongValue(keyname, valuename, value);
}

void setIniFloat(const char *keyname, const char *valuename, float value)
{
	getIni().SetDoubleValue(keyname, valuename, value);
}

/**
 * @brief Find item in comma-sparated list
 */
bool IniFound(const char *list, const char *item)
{
	const char *look = strcasestr(list, item);
	if (!look)
		return false;
	look += strlen(item);
	while (*look == ' ')
		look++;
	return (bool) strchr(",;:", *look); // delimters including '\0'
}

/**
 * @brief Find needle pattern in haystack, ignoring case
 */
const char *strcasestr(const char *haystack, const char *needle)
{
	const char *look, *pp, *qq;

	for (look = haystack; *look; look++) {
		while (*look && (tolower(*look) != tolower(*needle)))
			look++;
		if (!*look)
			return NULL;

		for (pp = needle + 1, qq = look + 1; tolower(*pp) == tolower(*qq); pp++, qq++) {
			if (!*pp)	// match found!
				return (look);
		}
	}
	return NULL;
}

DWORD SErrGetLastError()
{
	return ::GetLastError();
}

void SErrSetLastError(DWORD dwErrCode)
{
	::SetLastError(dwErrCode);
}

bool SFileSetBasePath(const char *path)
{
	if (SBasePath == nullptr)
		SBasePath = new std::string;
	*SBasePath = path;
	return true;
}

bool SFileEnableDirectAccess(bool enable)
{
	directFileAccess = enable;
	return true;
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
