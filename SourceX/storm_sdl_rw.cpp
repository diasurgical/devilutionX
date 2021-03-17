#include "storm_sdl_rw.h"

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
	DWORD result;
	if (!SFileGetFileSize(SFileRw_GetHandle(context), &result))
		return -1;
	return result;
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
	return SFileSetFilePointer(SFileRw_GetHandle(context), offset, NULL, swhence);
}

#ifndef USE_SDL1
static size_t SFileRw_read(struct SDL_RWops *context, void *ptr, size_t size, size_t maxnum)
#else
static int SFileRw_read(struct SDL_RWops *context, void *ptr, int size, int maxnum)
#endif
{
	DWORD num_read = 0;
	SFileReadFile(SFileRw_GetHandle(context), ptr, maxnum * size, &num_read, NULL);
	return num_read;
}

static int SFileRw_close(struct SDL_RWops *context)
{
	mem_free_dbg(context);
	return 0;
}

SDL_RWops *SFileRw_FromStormHandle(HANDLE handle)
{
	SDL_RWops *result = (SDL_RWops *)DiabloAllocPtr(sizeof(SDL_RWops));

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
