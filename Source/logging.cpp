#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
static CCritSect sgMemCrit;
#endif
CHAR FileName[MAX_PATH];
char log_buffer[388];
LPCVOID lpAddress;
DWORD nNumberOfBytesToWrite;

/* data */

BOOL log_not_created = 1;
HANDLE log_file = INVALID_HANDLE_VALUE;

void __cdecl log_flush(BOOL force_close)
{
	DWORD NumberOfBytesWritten;

#ifdef __cplusplus
	sgMemCrit.Enter();
#endif
	if (nNumberOfBytesToWrite) {
		if (log_file == INVALID_HANDLE_VALUE) {
			log_file = log_create();
			if (log_file == INVALID_HANDLE_VALUE) {
				nNumberOfBytesToWrite = 0;
				return;
			}
			SetFilePointer(log_file, 0, NULL, FILE_END);
		}
		WriteFile(log_file, lpAddress, nNumberOfBytesToWrite, &NumberOfBytesWritten, 0);
		nNumberOfBytesToWrite = 0;
	}
	if (force_close && log_file != INVALID_HANDLE_VALUE) {
		CloseHandle(log_file);
		log_file = INVALID_HANDLE_VALUE;
	}
#ifdef __cplusplus
	sgMemCrit.Leave();
#endif
}

HANDLE log_create()
{
	char *last_slash_pos;
	HANDLE fh;
	VS_FIXEDFILEINFO file_info;
	DWORD i;
	char buf[32];

	if (log_not_created) {
		char filename_tmp[MAX_PATH];
		if (GetModuleFileName(NULL, filename_tmp, sizeof filename_tmp) == 0)
			filename_tmp[0] = '\0';
		else {
			last_slash_pos = strrchr(filename_tmp, '\\');
			if (last_slash_pos == NULL)
				filename_tmp[0] = '\0';
			else
				*(last_slash_pos + 1) = '\0';
		}
		i = 32;
		if (!GetUserName(buf, &i))
			buf[0] = '\0';
		log_get_version(&file_info);
		_snprintf(
		    FileName,
		    sizeof(filename_tmp),
		    "%s%s%02u%02u%02u.ERR",
		    filename_tmp,
		    buf,
		    file_info.dwProductVersionMS & 0xFFFF,
		    file_info.dwProductVersionLS >> 16,
		    file_info.dwProductVersionLS & 0xFFFF);
	}
	fh = INVALID_HANDLE_VALUE;
	for (i = log_not_created ? 0 : 1; (int)i < 2; i++) {
		fh = CreateFile(FileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (fh != INVALID_HANDLE_VALUE) {
			if (GetFileSize(fh, NULL) > 0x10000)
				SetEndOfFile(fh);
			break;
		}
		last_slash_pos = strrchr(FileName, '\\');
		if (!last_slash_pos)
			last_slash_pos = FileName;
		char filename_tmp[MAX_PATH] = "c:\\";
		strcat(filename_tmp, last_slash_pos);
		strcpy(FileName, filename_tmp);
	}
	log_not_created = FALSE;
	return fh;
}

void log_get_version(VS_FIXEDFILEINFO *file_info)
{
	DWORD size, len, dwHandle;
	unsigned int puLen;
	void *version;
	char Filename[MAX_PATH];
	LPVOID lpBuffer;

	memset(file_info, 0, sizeof(*file_info));
	if (GetModuleFileName(0, Filename, sizeof(Filename))) {
		size = GetFileVersionInfoSize(Filename, &dwHandle);
		if (size) {
			version = VirtualAlloc(0, size, MEM_COMMIT, PAGE_READWRITE);
			if (GetFileVersionInfo(Filename, 0, size, version) && VerQueryValue(version, "\\", &lpBuffer, &puLen)) {
				len = puLen;
				if (puLen >= 52)
					len = 52;
				memcpy(file_info, lpBuffer, len);
			}
			VirtualFree(version, 0, MEM_RELEASE);
		}
	}
}

void __cdecl log_printf(const char *pszFmt, ...)
{
	size_t size;
	char *pBuffer;
	char msg[512];
	va_list va;

#ifdef __cplusplus
	sgMemCrit.Enter();
#endif
	va_start(va, pszFmt);
	_vsnprintf(msg, 0x200, pszFmt, va);
	va_end(va);
	msg[511] = 0;
	size = strlen(msg);
	if (size + nNumberOfBytesToWrite > 0x1000) {
		log_flush(0);
	}

	if (lpAddress == NULL) {
		lpAddress = (char *)VirtualAlloc((LPVOID)lpAddress, 0x1000, MEM_COMMIT, PAGE_READWRITE);
		pBuffer = (char *)lpAddress;
		nNumberOfBytesToWrite = 0;
	}
	if (lpAddress != NULL) {
		pBuffer = (char *)lpAddress;
		memcpy(&pBuffer[nNumberOfBytesToWrite], msg, size);
		nNumberOfBytesToWrite += size;
	}
#ifdef __cplusplus
	sgMemCrit.Leave();
#endif
}

void log_dump_computer_info()
{
	char Buffer[64];
	VS_FIXEDFILEINFO file_info;
	SYSTEMTIME SystemTime;
	DWORD pcbBuffer;

	GetLocalTime(&SystemTime);
	pcbBuffer = 64;
	if (!GetUserName(Buffer, &pcbBuffer))
		Buffer[0] = 0;
	log_get_version(&file_info);
	log_printf(
	    "\r\n"
	    "------------------------------------------------------\r\n"
	    "PROGRAM VERSION: %d.%d.%d.%d\r\n"
	    "COMPUTER NAME: %s\r\n"
	    "TIME: %02u/%02u/%02u %02u:%02u:%02u\r\n"
	    "INFO: %s\r\n"
	    "\r\n",
	    file_info.dwProductVersionMS >> 16,
	    file_info.dwProductVersionMS & 0xFFFF,
	    file_info.dwProductVersionLS >> 16,
	    file_info.dwProductVersionLS & 0xFFFF,
	    Buffer,
	    SystemTime.wMonth,
	    SystemTime.wDay,
	    SystemTime.wYear % 100,
	    SystemTime.wHour,
	    SystemTime.wMinute,
	    SystemTime.wSecond,
	    log_buffer);
}

DEVILUTION_END_NAMESPACE
