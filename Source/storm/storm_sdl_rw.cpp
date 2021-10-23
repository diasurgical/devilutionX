#include "storm/storm_sdl_rw.h"

#include <algorithm>
#include <cstdint>
#include <cstring>

#include "engine.h"
#include "storm/storm.h"
#include "utils/file_util.h"
#include "utils/log.hpp"
#include "utils/paths.h"

namespace devilution {

namespace {

static HANDLE SFileRwGetHandle(struct SDL_RWops *context)
{
	return (HANDLE)context->hidden.unknown.data1;
}

static void SFileRwSetHandle(struct SDL_RWops *context, HANDLE handle)
{
	context->hidden.unknown.data1 = handle;
}

#ifndef USE_SDL1
static Sint64 SFileRwSize(struct SDL_RWops *context)
{
	return SFileGetFileSize(SFileRwGetHandle(context));
}
#endif

#ifndef USE_SDL1
static Sint64 SFileRwSeek(struct SDL_RWops *context, Sint64 offset, int whence)
#else
static int SFileRwSeek(struct SDL_RWops *context, int offset, int whence)
#endif
{
	DWORD swhence;
	switch (whence) {
	case RW_SEEK_SET:
		swhence = DVL_FILE_BEGIN;
		break;
	case RW_SEEK_CUR:
		swhence = DVL_FILE_CURRENT;
		break;
	case RW_SEEK_END:
		swhence = DVL_FILE_END;
		break;
	default:
		return -1;
	}
	const std::uint64_t pos = SFileSetFilePointer(SFileRwGetHandle(context), offset, swhence);
	if (pos == static_cast<std::uint64_t>(-1)) {
		Log("SFileRwSeek error: {}", SErrGetLastError());
	}
	return pos;
}

#ifndef USE_SDL1
static size_t SFileRwRead(struct SDL_RWops *context, void *ptr, size_t size, size_t maxnum)
#else
static int SFileRwRead(struct SDL_RWops *context, void *ptr, int size, int maxnum)
#endif
{
	size_t numRead = 0;
	if (!SFileReadFileThreadSafe(SFileRwGetHandle(context), ptr, maxnum * size, &numRead)) {
		const auto errCode = SErrGetLastError();
		if (errCode != STORM_ERROR_HANDLE_EOF) {
			Log("SFileRwRead error: {} {} ERROR CODE {}", size, maxnum, errCode);
		}
	}
	return numRead / size;
}

static int SFileRwClose(struct SDL_RWops *context)
{
	SFileCloseFileThreadSafe(SFileRwGetHandle(context));
	delete context;
	return 0;
}

SDL_RWops *SFileRw_FromStormHandle(HANDLE handle)
{
	auto *result = new SDL_RWops();
	std::memset(result, 0, sizeof(*result));

#ifndef USE_SDL1
	result->size = &SFileRwSize;
	result->type = SDL_RWOPS_UNKNOWN;
#else
	result->type = 0;
#endif

	result->seek = &SFileRwSeek;
	result->read = &SFileRwRead;
	result->write = nullptr;
	result->close = &SFileRwClose;
	SFileRwSetHandle(result, handle);
	return result;
}

#ifdef _WIN32
constexpr char PathSeparator = '\\';
#else
constexpr char PathSeparator = '/';
#endif

// Converts ASCII characters to lowercase
// Converts slash / backslash to system file-separator
const char LowerCaseAsciiPathTable[256] {
	'\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F',
	'\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F',
	'\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2A', '\x2B', '\x2C', '\x2D', '\x2E', PathSeparator,
	'\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x3A', '\x3B', '\x3C', '\x3D', '\x3E', '\x3F',
	'\x40', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6A', '\x6B', '\x6C', '\x6D', '\x6E', '\x6F',
	'\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7A', '\x5B', PathSeparator, '\x5D', '\x5E', '\x5F',
	'\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6A', '\x6B', '\x6C', '\x6D', '\x6E', '\x6F',
	'\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7A', '\x7B', '\x7C', '\x7D', '\x7E', '\x7F',
	'\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', '\x88', '\x89', '\x8A', '\x8B', '\x8C', '\x8D', '\x8E', '\x8F',
	'\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', '\x98', '\x99', '\x9A', '\x9B', '\x9C', '\x9D', '\x9E', '\x9F',
	'\xA0', '\xA1', '\xA2', '\xA3', '\xA4', '\xA5', '\xA6', '\xA7', '\xA8', '\xA9', '\xAA', '\xAB', '\xAC', '\xAD', '\xAE', '\xAF',
	'\xB0', '\xB1', '\xB2', '\xB3', '\xB4', '\xB5', '\xB6', '\xB7', '\xB8', '\xB9', '\xBA', '\xBB', '\xBC', '\xBD', '\xBE', '\xBF',
	'\xC0', '\xC1', '\xC2', '\xC3', '\xC4', '\xC5', '\xC6', '\xC7', '\xC8', '\xC9', '\xCA', '\xCB', '\xCC', '\xCD', '\xCE', '\xCF',
	'\xD0', '\xD1', '\xD2', '\xD3', '\xD4', '\xD5', '\xD6', '\xD7', '\xD8', '\xD9', '\xDA', '\xDB', '\xDC', '\xDD', '\xDE', '\xDF',
	'\xE0', '\xE1', '\xE2', '\xE3', '\xE4', '\xE5', '\xE6', '\xE7', '\xE8', '\xE9', '\xEA', '\xEB', '\xEC', '\xED', '\xEE', '\xEF',
	'\xF0', '\xF1', '\xF2', '\xF3', '\xF4', '\xF5', '\xF6', '\xF7', '\xF8', '\xF9', '\xFA', '\xFB', '\xFC', '\xFD', '\xFE', '\xFF'
};

} // namespace

SDL_RWops *SFileOpenRw(const char *filename)
{
	std::string relativePath = filename;
	for (char &c : relativePath)
		c = LowerCaseAsciiPathTable[static_cast<unsigned char>(c)];

	if (relativePath[0] == '/')
		return SDL_RWFromFile(relativePath.c_str(), "rb");

	// Files next to the MPQ archives override MPQ contents.
	SDL_RWops *rwops;
	if (paths::MpqDir()) {
		const std::string path = *paths::MpqDir() + relativePath;
		// Avoid spamming DEBUG messages if the file does not exist.
		if ((FileExists(path.c_str())) && (rwops = SDL_RWFromFile(path.c_str(), "rb")) != nullptr) {
			LogVerbose("Loaded MPQ file override: {}", path);
			return rwops;
		}
	}

	// Load from all the MPQ archives.
	HANDLE handle;
	if (SFileOpenFile(filename, &handle))
		return SFileRw_FromStormHandle(handle);

	// Load from the `/assets` directory next to the devilutionx binary.
	const std::string path = paths::AssetsPath() + relativePath;
	if ((rwops = SDL_RWFromFile(path.c_str(), "rb")) != nullptr)
		return rwops;

#ifdef __ANDROID__
	// On Android, fall back to the APK's assets.
	// This is handled by SDL when we pass a relative path.
	if (!paths::AssetsPath().empty() && (rwops = SDL_RWFromFile(relativePath.c_str(), "rb")))
		return rwops;
#endif

	return nullptr;
}

} // namespace devilution
