#include "storm/storm_sdl_rw.h"

#include <cstdint>
#include <cstring>

#include "engine.h"
#include "storm/storm.h"
#include "utils/log.hpp"

namespace devilution {

static HANDLE SFileRwGetHandle(struct SDL_RWops *context)
{
	return (HANDLE)context->hidden.unknown.data1;
}

static void SFileRwSetHandle(struct SDL_RWops *context, HANDLE handle)
{
	context->hidden.unknown.data1 = handle;
}

static Sint64 SFileRwSize(struct SDL_RWops *context)
{
	return SFileGetFileSize(SFileRwGetHandle(context));
}

static Sint64 SFileRwSeek(struct SDL_RWops *context, Sint64 offset, int whence)
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

static size_t SFileRwRead(struct SDL_RWops *context, void *ptr, size_t size, size_t maxnum)
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

	result->size = &SFileRwSize;
	result->type = SDL_RWOPS_UNKNOWN;
	result->seek = &SFileRwSeek;
	result->read = &SFileRwRead;
	result->write = nullptr;
	result->close = &SFileRwClose;
	SFileRwSetHandle(result, handle);
	return result;
}

SDL_RWops *SFileOpenRw(const char *filename)
{
	HANDLE handle;
	if (SFileOpenFile(filename, &handle))
		return SFileRw_FromStormHandle(handle);

	return nullptr;
}

} // namespace devilution
