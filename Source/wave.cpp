#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

void WCloseFile(HANDLE file)
{
	SFileCloseFile(file);
}

LONG WGetFileSize(HANDLE hsFile, DWORD *lpFileSizeHigh, const char *FileName)
{
	LONG ret;

	if ((ret = SFileGetFileSize(hsFile, lpFileSizeHigh)) == 0)
		FileErrDlg(FileName);

	return ret;
}

BOOL WOpenFile(const char *FileName, HANDLE *phsFile, BOOL mayNotExist)
{
	if (!SFileOpenFile(FileName, phsFile))
		FileErrDlg(FileName);

	return TRUE;
}

void WReadFile(HANDLE hsFile, LPVOID buf, DWORD to_read, const char *FileName)
{
	if (SFileSetFilePointer(hsFile, 0, NULL, DVL_FILE_CURRENT) == -1)
		FileErrDlg(FileName);

	if (!SFileReadFile(hsFile, buf, to_read, NULL, NULL))
		FileErrDlg(FileName);
}

DEVILUTION_END_NAMESPACE
