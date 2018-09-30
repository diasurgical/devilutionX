//HEADER_GOES_HERE

#include "../types.h"

int log_cpp_init_value; // weak
static CRITICAL_SECTION sgMemCrit;
CHAR FileName[260]; // idb
char log_buffer[388];
LPCVOID lpAddress; // idb
DWORD nNumberOfBytesToWrite; // idb

const int log_inf = 0x7F800000; // weak

/* data */

int log_not_created = 1; // weak
HANDLE log_file = (HANDLE)0xFFFFFFFF; // idb

struct log_cpp_init_1
{
	log_cpp_init_1()
	{
		log_cpp_init_value = log_inf;
	}
} _log_cpp_init_1;
// 47F070: using guessed type int log_inf;
// 646A30: using guessed type int log_cpp_init_value;

struct log_cpp_init_2
{
	log_cpp_init_2()
	{
		log_init_mutex();
		j_log_cleanup_mutex();
	}
} _log_cpp_init_2;

void __cdecl log_init_mutex()
{
	InitializeCriticalSection(&sgMemCrit);
}

void __cdecl j_log_cleanup_mutex()
{
	atexit(log_cleanup_mutex);
}

void __cdecl log_cleanup_mutex()
{
	DeleteCriticalSection(&sgMemCrit);
}

void __cdecl log_flush(bool force_close)
{
	void *v1; // eax
	DWORD NumberOfBytesWritten; // [esp+8h] [ebp-4h]

	EnterCriticalSection(&sgMemCrit);
	if ( nNumberOfBytesToWrite )
	{
		if ( log_file == (HANDLE)-1 )
		{
			v1 = log_create();
			log_file = v1;
			if ( v1 == (void *)-1 )
			{
				nNumberOfBytesToWrite = 0;
				return;
			}
			SetFilePointer(v1, 0, NULL, FILE_END);
		}
		WriteFile(log_file, lpAddress, nNumberOfBytesToWrite, &NumberOfBytesWritten, 0);
		nNumberOfBytesToWrite = 0;
	}
	if ( force_close && log_file != (HANDLE)-1 )
	{
		CloseHandle(log_file);
		log_file = (HANDLE)-1;
	}
	LeaveCriticalSection(&sgMemCrit);
}

void *__cdecl log_create()
{
	char *v0; // eax
	void *v1; // ebx
	HANDLE v2; // eax
	char *v3; // edx
	char Filename[260]; // [esp+Ch] [ebp-15Ch]
	VS_FIXEDFILEINFO file_info; // [esp+110h] [ebp-58h]
	char Buffer[32]; // [esp+144h] [ebp-24h]
	DWORD pcbBuffer; // [esp+164h] [ebp-4h]

	if ( log_not_created )
	{
		if ( GetModuleFileName(0, Filename, 0x104u) && (v0 = strrchr(Filename, '\\')) != 0 )
			v0[1] = 0;
		else
			Filename[0] = 0;
		pcbBuffer = 32;
		if ( !GetUserName(Buffer, &pcbBuffer) )
			Buffer[0] = 0;
		log_get_version(&file_info);
		_snprintf(
			FileName,
			0x104u,
			"%s%s%02u%02u%02u.ERR",
			Filename,
			Buffer,
			_LOWORD(file_info.dwProductVersionMS),
			file_info.dwProductVersionLS >> 16,
			_LOWORD(file_info.dwProductVersionLS));
	}
	v1 = (void *)-1;
	for ( pcbBuffer = log_not_created == 0; (signed int)pcbBuffer < 2; ++pcbBuffer )
	{
		v2 = CreateFile(FileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		v1 = v2;
		if ( v2 != (HANDLE)-1 )
		{
			if ( GetFileSize(v2, 0) > 0x10000 )
				SetEndOfFile(v1);
			break;
		}
		v3 = strrchr(FileName, '\\');
		if ( !v3 )
			v3 = FileName;
		strcpy(Filename, "c:\\");
		memset(&Filename[4], 0, 0x100u);
		strcat(Filename, v3);
		strcpy(FileName, Filename);
	}
	log_not_created = 0;
	return v1;
}
// 4947D4: using guessed type int log_not_created;

void __fastcall log_get_version(VS_FIXEDFILEINFO *file_info)
{
	DWORD v1; // eax
	DWORD v2; // esi
	void *v3; // ebx
	unsigned int v4; // eax
	char Filename[260]; // [esp+8h] [ebp-114h]
	DWORD dwHandle; // [esp+10Ch] [ebp-10h]
	LPVOID lpBuffer; // [esp+110h] [ebp-Ch]
	unsigned int puLen; // [esp+114h] [ebp-8h]
	void *v9; // [esp+118h] [ebp-4h]

	v9 = file_info;
	memset(file_info, 0, 0x34u);
	if ( GetModuleFileName(0, Filename, 0x104u) )
	{
		v1 = GetFileVersionInfoSize(Filename, &dwHandle);
		v2 = v1;
		if ( v1 )
		{
			v3 = VirtualAlloc(0, v1, 0x1000u, 4u);
			if ( GetFileVersionInfo(Filename, 0, v2, v3) && VerQueryValue(v3, "\\", &lpBuffer, &puLen) )
			{
				v4 = puLen;
				if ( puLen >= 0x34 )
					v4 = 52;
				memcpy(v9, lpBuffer, v4);
			}
			VirtualFree(v3, 0, 0x8000u);
		}
	}
}

void log_printf(const char *pszFmt, ...)
{
	size_t v1; // edi
	char *v2; // eax
	char v3[512]; // [esp+Ch] [ebp-200h]
	va_list va; // [esp+218h] [ebp+Ch]

	va_start(va, pszFmt);
	EnterCriticalSection(&sgMemCrit);
	_vsnprintf(v3, 0x200u, pszFmt, va);
	v3[511] = 0;
	v1 = strlen(v3);
	if ( v1 + nNumberOfBytesToWrite > 0x1000 )
		log_flush(0);
	v2 = (char *)lpAddress;
	if ( lpAddress
	  || (v2 = (char *)VirtualAlloc((LPVOID)lpAddress, 0x1000u, 0x1000u, 4u),
		  nNumberOfBytesToWrite = 0,
		  (lpAddress = v2) != 0) )
	{
		memcpy(&v2[nNumberOfBytesToWrite], v3, v1);
		nNumberOfBytesToWrite += v1;
	}
	LeaveCriticalSection(&sgMemCrit);
}

void __cdecl log_dump_computer_info()
{
	char Buffer[64]; // [esp+0h] [ebp-88h]
	VS_FIXEDFILEINFO file_info; // [esp+40h] [ebp-48h]
	struct _SYSTEMTIME SystemTime; // [esp+74h] [ebp-14h]
	DWORD pcbBuffer; // [esp+84h] [ebp-4h]

	GetLocalTime(&SystemTime);
	pcbBuffer = 64;
	if ( !GetUserName(Buffer, &pcbBuffer) )
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
		_LOWORD(file_info.dwProductVersionMS),
		file_info.dwProductVersionLS >> 16,
		_LOWORD(file_info.dwProductVersionLS),
		Buffer,
		SystemTime.wMonth,
		SystemTime.wDay,
		SystemTime.wYear % 100,
		SystemTime.wHour,
		SystemTime.wMinute,
		SystemTime.wSecond,
		log_buffer);
}
