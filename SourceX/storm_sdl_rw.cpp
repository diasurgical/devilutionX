#include "storm_sdl_rw.h"

#include <cstdint>
#include <cstring>

#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"
#include "../Source/engine.h"

namespace dvl {

static HANDLE SFileRw_GetHandle(struct SDL_RWops *context)
{
	return (HANDLE)context->hidden.unknown.data1;
}

static void SFileRw_SetHandle(struct SDL_RWops *context, HANDLE handle)
{
	context->hidden.unknown.data1 = handle;
}

#ifndef USE_SDL1
static Sint64 SFileRw_size(struct SDL_RWops *context)
{
	return SFileGetFileSize(SFileRw_GetHandle(context), NULL);
}
#endif

#ifndef USE_SDL1
static Sint64 SFileRw_seek(struct SDL_RWops *context, Sint64 offset, int whence)
#else
static int SFileRw_seek(struct SDL_RWops *context, int offset, int whence)
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
	const std::uint64_t pos = SFileSetFilePointer(SFileRw_GetHandle(context), offset, swhence);
	if (pos == static_cast<std::uint64_t>(-1)) {
		SDL_Log("SFileRw_seek error: %ud", (unsigned int)SErrGetLastError());
	}
	return pos;
}

#ifndef USE_SDL1
static size_t SFileRw_read(struct SDL_RWops *context, void *ptr, size_t size, size_t maxnum)
#else
static int SFileRw_read(struct SDL_RWops *context, void *ptr, int size, int maxnum)
#endif
{
	DWORD num_read = 0;
	if (!SFileReadFile(SFileRw_GetHandle(context), ptr, maxnum * size, &num_read, NULL)) {
		const DWORD err_code = SErrGetLastError();
		if (err_code != DVL_ERROR_HANDLE_EOF) {
			SDL_Log("SFileRw_read error: %u %u ERROR CODE %u", (unsigned int)size, (unsigned int)maxnum, (unsigned int)err_code);
		}
	}
	return num_read / size;
}

static int SFileRw_close(struct SDL_RWops *context)
{
	mem_free_dbg(context);
	return 0;
}

SDL_RWops *SFileRw_FromStormHandle(HANDLE handle)
{
	SDL_RWops *result = (SDL_RWops *)DiabloAllocPtr(sizeof(SDL_RWops));
	std::memset(result, 0, sizeof(*result));

#ifndef USE_SDL1
	result->size = &SFileRw_size;
	result->type = SDL_RWOPS_UNKNOWN;
#else
	result->type = 0;
#endif

	result->seek = &SFileRw_seek;
	result->read = &SFileRw_read;
	result->write = NULL;
	result->close = &SFileRw_close;
	SFileRw_SetHandle(result, handle);
	return result;
}

} // namespace dvl
