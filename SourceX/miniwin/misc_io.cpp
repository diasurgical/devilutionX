#include <cstdio>
#include <cstring>
#include <unordered_set>
#include <string>
#include <iterator>
#include <vector>
#include <algorithm>
#include <fstream>
#include <memory>
#include <stdexcept>

#include "devilution.h"
#include "stubs.h"

namespace dvl {

struct memfile {
	std::string path;
	std::vector<char> buf;
	std::size_t pos = 0;
};

static std::unordered_set<memfile *> files;

HANDLE CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	char name[DVL_MAX_PATH];
	TranslateFileName(name, sizeof(name), lpFileName);
	SDL_Log("file: %s (%s)", lpFileName, name);
	UNIMPLEMENTED_UNLESS(!(dwDesiredAccess & ~(DVL_GENERIC_READ | DVL_GENERIC_WRITE)));
	memfile *file = new memfile;
	file->path = name;
	if (dwCreationDisposition == DVL_OPEN_EXISTING) {
		// read contents of existing file into buffer
		std::ifstream filestream(file->path, std::ios::binary);
		if (!filestream.fail()) {
			file->buf.insert(file->buf.begin(),
			    std::istreambuf_iterator<char>(filestream),
			    std::istreambuf_iterator<char>());
		}
	} else if (dwCreationDisposition == DVL_CREATE_ALWAYS) {
		// start with empty file
	} else {
		UNIMPLEMENTED();
	}
	files.insert(file);
	return file;
}

WINBOOL ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped)
{
	memfile *file = static_cast<memfile *>(hFile);
	UNIMPLEMENTED_UNLESS(!lpOverlapped);
	size_t len = std::min<size_t>(file->buf.size() - file->pos, nNumberOfBytesToRead);
	std::memcpy(lpBuffer, file->buf.data() + file->pos, len);
	file->pos += len;
	*lpNumberOfBytesRead = len;
	return true;
}

DWORD GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh)
{
	memfile *file = static_cast<memfile *>(hFile);
	return file->buf.size();
}

WINBOOL WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
	memfile *file = static_cast<memfile *>(hFile);
	UNIMPLEMENTED_UNLESS(!lpOverlapped);
	if (!nNumberOfBytesToWrite)
		return true;
	if (file->buf.size() < file->pos + nNumberOfBytesToWrite)
		file->buf.resize(file->pos + nNumberOfBytesToWrite);
	std::memcpy(file->buf.data() + file->pos, lpBuffer, nNumberOfBytesToWrite);
	file->pos += nNumberOfBytesToWrite;
	*lpNumberOfBytesWritten = nNumberOfBytesToWrite;
	return true;
}

DWORD SetFilePointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod)
{
	memfile *file = static_cast<memfile *>(hFile);
	UNIMPLEMENTED_UNLESS(!lpDistanceToMoveHigh);
	if (dwMoveMethod == DVL_FILE_BEGIN) {
		file->pos = lDistanceToMove;
	} else if (dwMoveMethod == DVL_FILE_CURRENT) {
		file->pos += lDistanceToMove;
	} else {
		UNIMPLEMENTED();

	}
	if (file->buf.size() < file->pos + 1)
		file->buf.resize(file->pos + 1);
	return file->pos;
}

WINBOOL SetEndOfFile(HANDLE hFile)
{
	memfile *file = static_cast<memfile *>(hFile);
	file->buf.erase(file->buf.begin() + file->pos, file->buf.end());
	return true;
}

void ShowOutOfDiskError()
{
	constexpr char text[] = "Failed to save, please free some disk space and try again.";
	UiErrorOkDialog("Out of Disk Space", text);
}

WINBOOL CloseHandle(HANDLE hObject)
{
	memfile *file = static_cast<memfile *>(hObject);
	const auto file_it = files.find(file);
	if (file_it == files.end())
		return CloseEvent(hObject);
	std::unique_ptr<memfile> ufile(file); // ensure that delete file is
	                                      // called on returning
	files.erase(file_it);
	std::ofstream filestream(file->path + ".tmp", std::ios::binary | std::ios::trunc);
	if (filestream.fail()) {
		ShowOutOfDiskError();
		return false;
	}
	filestream.write(file->buf.data(), file->buf.size());
	if (filestream.fail()) {
		ShowOutOfDiskError();
		return false;
	}
	filestream.close();
	std::remove(file->path.c_str());
	if (std::rename((file->path + ".tmp").c_str(), file->path.c_str())) {
		ShowOutOfDiskError();
		return false;
	}
	return true;
}

} // namespace dvl
