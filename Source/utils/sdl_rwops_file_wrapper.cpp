#include "utils/sdl_rwops_file_wrapper.hpp"

#ifdef DEVILUTIONX_SDL_RWOPS_FILE_WRAPPER_AVAILABLE

#include "utils/log.hpp"

namespace devilution {
extern "C" {

#ifdef DEVILUTIONX_SDL_RWOPS_FILE_WRAPPER_IMPL_FOPENCOOKIE

ssize_t SDL_RWops_CookieRead(void *cookie, char *buf, size_t nbytes)
{
	size_t numRead = SDL_RWread(static_cast<SDL_RWops *>(cookie), buf, nbytes, 1);
	if (numRead == 0) {
		Log("SDL_RWread error: {} ERROR CODE {}", SDL_GetError(), numRead);
	}
	return numRead * nbytes;
}

int SDL_RWops_CookieSeek(void *cookie, off64_t *pos, int whence)
{
	int swhence;
	switch (whence) {
	case SEEK_SET:
		swhence = RW_SEEK_SET;
		break;
	case SEEK_CUR:
		swhence = RW_SEEK_CUR;
		break;
	case SEEK_END:
		swhence = RW_SEEK_END;
		break;
	default:
		return -1;
	}
	const Sint64 spos = SDL_RWseek(static_cast<SDL_RWops *>(cookie), *pos, swhence);
	if (spos < 0) {
		Log("SDL_RWops_RwSeek error: {}", SDL_GetError());
		return -1;
	}
	*pos = static_cast<off64_t>(spos);
	return 0;
}

int SDL_RWops_CookieClose(void *cookie)
{
	return SDL_RWclose(static_cast<SDL_RWops *>(cookie));
}

} // extern "C"
#endif

FILE *FILE_FromSDL_RWops(SDL_RWops *handle)
{
#ifdef DEVILUTIONX_SDL_RWOPS_FILE_WRAPPER_IMPL_FOPENCOOKIE
	cookie_io_functions_t ioFns;
	std::memset(&ioFns, 0, sizeof(ioFns));
	ioFns.read = &SDL_RWops_CookieRead;
	ioFns.seek = &SDL_RWops_CookieSeek;
	ioFns.close = &SDL_RWops_CookieClose;
	return fopencookie(handle, "rb", ioFns);
#else
#error "unimplemented"
#endif
}

} // namespace devilution

#endif
