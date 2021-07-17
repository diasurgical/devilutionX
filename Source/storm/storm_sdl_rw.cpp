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

} // namespace devilution
