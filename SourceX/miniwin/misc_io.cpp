#include <cstdio>
#include <set>
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

static std::set<memfile*> files;

HANDLE CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                   LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                   DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	char name[DVL_MAX_PATH];
	TranslateFileName(name, sizeof(name), lpFileName);
	DUMMY_PRINT("file: %s (%s)", lpFileName, name);
	UNIMPLEMENTED_UNLESS(!(dwDesiredAccess & ~(DVL_GENERIC_READ | DVL_GENERIC_WRITE)));
	memfile* file = new memfile;
	file->path = name;
	if (dwCreationDisposition == DVL_OPEN_EXISTING) {
		// read contents of existing file into buffer
		std::ifstream filestream(file->path, std::ios::binary);
		if(!filestream.fail()) {
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
	memfile* file = static_cast<memfile*>(hFile);
	UNIMPLEMENTED_UNLESS(!lpOverlapped);
	size_t len = std::min<size_t>(file->buf.size() - file->pos, nNumberOfBytesToRead);
	std::copy(file->buf.begin() + file->pos, file->buf.begin() + file->pos + len, static_cast<char*>(lpBuffer));
	file->pos += len;
	*lpNumberOfBytesRead = len;
	return true;
}

DWORD GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh)
{
	memfile* file = static_cast<memfile*>(hFile);
	return file->buf.size();
}

WINBOOL WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite,
                  LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
	memfile* file = static_cast<memfile*>(hFile);
	UNIMPLEMENTED_UNLESS(!lpOverlapped);
	if(!nNumberOfBytesToWrite)
		return true;
	if(file->buf.size() < file->pos + nNumberOfBytesToWrite)
		file->buf.resize(file->pos + nNumberOfBytesToWrite);
	std::copy(static_cast<const char*>(lpBuffer),
	          static_cast<const char*>(lpBuffer) + nNumberOfBytesToWrite,
	          file->buf.begin() + file->pos);
	file->pos += nNumberOfBytesToWrite;
	*lpNumberOfBytesWritten = nNumberOfBytesToWrite;
	return true;
}

DWORD SetFilePointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod)
{
	memfile* file = static_cast<memfile*>(hFile);
	UNIMPLEMENTED_UNLESS(!lpDistanceToMoveHigh);
	if (dwMoveMethod == DVL_FILE_BEGIN) {
		file->pos = lDistanceToMove;
	} else if (dwMoveMethod == DVL_FILE_CURRENT) {
		file->pos += lDistanceToMove;
	} else {
		UNIMPLEMENTED();
	}
	if(file->buf.size() < file->pos + 1)
		file->buf.resize(file->pos + 1);
	return file->pos;
}

WINBOOL SetEndOfFile(HANDLE hFile)
{
	memfile* file = static_cast<memfile*>(hFile);
	file->buf.erase(file->buf.begin() + file->pos, file->buf.end());
	return true;
}

DWORD GetFileAttributesA(LPCSTR lpFileName)
{
	char name[DVL_MAX_PATH];
	TranslateFileName(name, sizeof(name), lpFileName);
	std::ifstream filestream(name, std::ios::binary);
	if (filestream.fail()) {
		SetLastError(DVL_ERROR_FILE_NOT_FOUND);
		return (DWORD)-1;
	}
	return 0x80;
}

WINBOOL SetFileAttributesA(LPCSTR lpFileName, DWORD dwFileAttributes)
{
	return true;
}

WINBOOL CloseHandle(HANDLE hObject)
{
	//Todo(Amiga): Fix the error handling
	memfile* file = static_cast<memfile*>(hObject);
	if (files.find(file) == files.end())
		return true;
	std::unique_ptr<memfile> ufile(file);  // ensure that delete file is
	                                       // called on returning
	files.erase(file);
#ifndef __AMIGA__
	try {
#endif
		std::ofstream filestream(file->path + ".tmp", std::ios::binary | std::ios::trunc);
		if (filestream.fail())
		{
			#ifndef __AMIGA__
			throw std::runtime_error("ofstream");
			#endif
		}
		filestream.write(file->buf.data(), file->buf.size());
		if (filestream.fail())
		{
			#ifndef __AMIGA__
			throw std::runtime_error("ofstream::write");
			#endif
		}
		filestream.close();
		std::remove(file->path.c_str());
		if (std::rename((file->path + ".tmp").c_str(), file->path.c_str()))
		{
			#ifndef __AMIGA__
			throw std::runtime_error("rename");
			#endif
		}
		return true;
#ifndef __AMIGA__
	} catch (std::runtime_error e) {
		// log
		DialogBoxParam(ghInst, DVL_MAKEINTRESOURCE(IDD_DIALOG7), ghMainWnd, (DLGPROC)FuncDlg, (LPARAM)file->path.c_str());
		return false;
	}
#endif
}

}  // namespace dvl
