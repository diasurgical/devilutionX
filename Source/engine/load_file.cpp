#include "load_file.hpp"

#include "diablo.h"
#include "storm/storm.h"

namespace devilution {

size_t GetFileSize(const char *pszName)
{
	HANDLE file;
	if (!SFileOpenFile(pszName, &file)) {
		if (!gbQuietMode)
			app_fatal("GetFileSize - SFileOpenFile failed for file:\n%s", pszName);
		return 0;
	}
	const size_t fileLen = SFileGetFileSize(file);
	SFileCloseFileThreadSafe(file);

	return fileLen;
}

void LoadFileData(const char *pszName, byte *buffer, size_t fileLen)
{
	HANDLE file;
	if (!SFileOpenFile(pszName, &file)) {
		if (!gbQuietMode)
			app_fatal("LoadFileData - SFileOpenFile failed for file:\n%s", pszName);
		return;
	}

	if (fileLen == 0)
		app_fatal("Zero length SFILE:\n%s", pszName);

	SFileReadFileThreadSafe(file, buffer, fileLen);
	SFileCloseFileThreadSafe(file);
}

} // namespace devilution
