#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "miniwin.h"
#include "stubs.h"

void TranslateFileName(char *dst, int dstLen, const char *src)
{
	for (int i = 0; i < dstLen; i++) {
		char c = *src++;
		dst[i] = c == '\\' ? '/' : c;
		if (!c) {
			break;
		}
	}
}

HANDLE WINAPI CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                          LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                          DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	char name[260];
	TranslateFileName(name, sizeof(name), lpFileName);

	DUMMY_PRINT("file: %s (%s)", lpFileName, name);

	assert(dwDesiredAccess == GENERIC_READ | GENERIC_WRITE);

	int flags = O_RDWR;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	if (dwCreationDisposition == OPEN_EXISTING) {
		// Nothing
	} else if (dwCreationDisposition == CREATE_ALWAYS) {
		flags |= O_CREAT | O_TRUNC;
	} else {
		UNIMPLEMENTED();
	}
	int fd = open(name, flags, mode);

	return (HANDLE)fd;
}

WINBOOL WINAPI ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead,
                        LPOVERLAPPED lpOverlapped)
{
	DUMMY_ONCE();

	assert(!lpOverlapped);
	int len = read((int)hFile, lpBuffer, nNumberOfBytesToRead);
	assert(len != -1);
	*lpNumberOfBytesRead = len;
	return TRUE;
}

DWORD WINAPI GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh)
{
	DUMMY_ONCE();

	assert(!lpFileSizeHigh);
	struct stat s;
	int ret = fstat((int)hFile, &s);
	assert(ret == 0);
	return s.st_size;
}

WINBOOL WINAPI WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten,
                         LPOVERLAPPED lpOverlapped)
{
	DUMMY_ONCE();

	assert(!lpOverlapped);
	ssize_t len = write((int)hFile, lpBuffer, nNumberOfBytesToWrite);
	if (len == -1) {
		*lpNumberOfBytesWritten = 0;
		return FALSE;
	}
	*lpNumberOfBytesWritten = (DWORD)len;
	return TRUE;
}

DWORD WINAPI SetFilePointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod)
{
	DUMMY_ONCE();

	assert(!lpDistanceToMoveHigh);
	int whence;
	if (dwMoveMethod == FILE_BEGIN) {
		whence = SEEK_SET;
	} else if (dwMoveMethod == FILE_CURRENT) {
		whence = SEEK_CUR;
	} else {
		UNIMPLEMENTED();
	}
	off_t ret = lseek((int)hFile, lDistanceToMove, whence);
	return (DWORD)ret;
}

WINBOOL WINAPI SetEndOfFile(HANDLE hFile)
{
	DUMMY_ONCE();

	off_t cur = lseek((int)hFile, 0, SEEK_CUR);
	assert(cur != -1);
	int res = ftruncate((int)hFile, cur);
	assert(res == 0);
	return TRUE;
}

DWORD WINAPI GetFileAttributesA(LPCSTR lpFileName)
{
	char name[260];
	TranslateFileName(name, sizeof(name), lpFileName);

	DUMMY_PRINT("file: %s (%s)", lpFileName, name);

	struct stat s;
	int res = stat(name, &s);

	if (res == -1) {
		SetLastError(ERROR_FILE_NOT_FOUND);
		return (DWORD)-1;
	}

	return 0x80;
}

WINBOOL WINAPI SetFileAttributesA(LPCSTR lpFileName, DWORD dwFileAttributes)
{
	DUMMY_PRINT("file: %s", lpFileName);
	return TRUE;
}

WINBOOL WINAPI CloseHandle(HANDLE hObject)
{
	DUMMY();

	int ret = close((int)hObject);
	assert(ret == 0);
	return TRUE;
}
