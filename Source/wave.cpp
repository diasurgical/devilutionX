#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

BOOL WCloseFile(HANDLE file)
{
	return SFileCloseFile(file);
}

LONG WGetFileSize(HANDLE hsFile, DWORD *lpFileSizeHigh)
{
	DWORD retry = 0;
	LONG ret;

	while ((ret = SFileGetFileSize(hsFile, lpFileSizeHigh)) == 0)
		WGetFileArchive(hsFile, &retry, NULL);

	return ret;
}

void WGetFileArchive(HANDLE hsFile, DWORD *retries, const char *FileName)
{
	HANDLE archive;

	if (*retries >= 5)
		FileErrDlg(FileName);

	if (hsFile && SFileGetFileArchive(hsFile, &archive) && archive != diabdat_mpq) {
		Sleep(20);
		(*retries)++;
	} else if (!InsertCDDlg())
		FileErrDlg(FileName);
}

BOOL WOpenFile(const char *FileName, HANDLE *phsFile, BOOL mayNotExist)
{
	DWORD retry = 0;

	while (1) {
		if (SFileOpenFile(FileName, phsFile))
			return TRUE;
		if (mayNotExist && SErrGetLastError() == ERROR_FILE_NOT_FOUND)
			break;
		WGetFileArchive(NULL, &retry, FileName);
	}
	return FALSE;
}

void WReadFile(HANDLE hsFile, LPVOID buf, DWORD to_read)
{
	DWORD retry = 0;
	DWORD readed;
	int initial_pos = WSetFilePointer(hsFile, 0, NULL, FILE_CURRENT);

	while (!SFileReadFile(hsFile, buf, to_read, &readed, NULL)) {
		WGetFileArchive(hsFile, &retry, NULL);
		WSetFilePointer(hsFile, initial_pos, NULL, FILE_BEGIN);
	}
}

int WSetFilePointer(HANDLE file1, int offset, HANDLE file2, int whence)
{
	DWORD retry = 0;
	int result;

	while (1) {
		result = SFileSetFilePointer(file1, offset, file2, whence);
		if (result != -1)
			break;
		WGetFileArchive(file1, &retry, NULL);
	}
	return result;
}


DEVILUTION_END_NAMESPACE
