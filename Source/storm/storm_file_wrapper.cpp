#include "storm_file_wrapper.h"

#ifdef DEVILUTIONX_STORM_FILE_WRAPPER_AVAILABLE

#include "storm/storm.h"
#include "utils/log.hpp"

namespace devilution {
extern "C" {

#ifdef DEVILUTIONX_STORM_FILE_WRAPPER_IMPL_FOPENCOOKIE

ssize_t SFileCookieRead(void *cookie, char *buf, size_t nbytes)
{
	DWORD numRead = 0;
	if (!SFileReadFileThreadSafe(static_cast<HANDLE>(cookie), buf, nbytes, &numRead)) {
		const DWORD errCode = SErrGetLastError();
		if (errCode != STORM_ERROR_HANDLE_EOF) {
			Log("SFileRwRead error: {} ERROR CODE {}", (unsigned int)nbytes, (unsigned int)errCode);
		}
	}
	return numRead;
}

int SFileCookieSeek(void *cookie, off64_t *pos, int whence)
{
	int swhence;
	switch (whence) {
	case SEEK_SET:
		swhence = DVL_FILE_BEGIN;
		break;
	case SEEK_CUR:
		swhence = DVL_FILE_CURRENT;
		break;
	case SEEK_END:
		swhence = DVL_FILE_END;
		break;
	default:
		return -1;
	}
	const std::uint64_t spos = SFileSetFilePointer(static_cast<HANDLE>(cookie), *pos, swhence);
	if (spos == static_cast<std::uint64_t>(-1)) {
		Log("SFileRwSeek error: {}", (unsigned int)SErrGetLastError());
		return -1;
	}
	*pos = static_cast<off64_t>(spos);
	return 0;
}

int SFileCookieClose(void *cookie)
{
	return SFileCloseFileThreadSafe(static_cast<HANDLE>(cookie)) ? 0 : -1;
}

} // extern "C"
#endif

FILE *FILE_FromStormHandle(HANDLE handle)
{
#ifdef DEVILUTIONX_STORM_FILE_WRAPPER_IMPL_FOPENCOOKIE
	cookie_io_functions_t ioFns;
	std::memset(&ioFns, 0, sizeof(ioFns));
	ioFns.read = &SFileCookieRead;
	ioFns.seek = &SFileCookieSeek;
	ioFns.close = &SFileCookieClose;
	return fopencookie(handle, "rb", ioFns);
#else
#error "unimplemented"
#endif
}

} // namespace devilution

#endif
