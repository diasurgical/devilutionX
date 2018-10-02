#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "miniwin.h"

#include "stubs.h"

DWORD last_error;

DWORD WINAPI GetLastError(VOID)
{
	return last_error;
}

VOID WINAPI SetLastError(DWORD dwErrCode)
{
	last_error = dwErrCode;
}

char __cdecl *_strlwr(char *str)
{
	for (char *p = str; *p; ++p) {
		*p = tolower(*p);
	}
	return str;
}

int WINAPIV wsprintfA(LPSTR dest, LPCSTR format, ...)
{
	va_list args;
	va_start(args, format);
	return vsprintf(dest, format, args);
}

int __cdecl _strcmpi(const char *_Str1, const char *_Str2)
{
	return strcasecmp(_Str1, _Str2);
}

char *__cdecl _itoa(int _Value, char *_Dest, int _Radix)
{
	UNIMPLEMENTED();
}

DWORD WINAPI GetTickCount(VOID)
{
	struct timespec now;
	if (clock_gettime(CLOCK_MONOTONIC, &now))
		return 0;
	return now.tv_sec * 1000.0 + now.tv_nsec / 1000000.0;
}

VOID WINAPI Sleep(DWORD dwMilliseconds)
{
	usleep(dwMilliseconds * 1000);
}

HANDLE WINAPI FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData)
{
	UNIMPLEMENTED();
}

WINBOOL WINAPI FindClose(HANDLE hFindFile)
{
	UNIMPLEMENTED();
}

UINT WINAPI GetWindowsDirectoryA(LPSTR lpBuffer, UINT uSize)
{
	UNIMPLEMENTED();
}

WINBOOL WINAPI GetDiskFreeSpaceA(LPCSTR lpRootPathName, LPDWORD lpSectorsPerCluster, LPDWORD lpBytesPerSector,
                                 LPDWORD lpNumberOfFreeClusters, LPDWORD lpTotalNumberOfClusters)
{
	UNIMPLEMENTED();
}

DWORD WINAPI GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize)
{
	DUMMY();
	assert(nSize >= 16);
	const char *name = ".\\diablo.exe";
	strncpy(lpFilename, name, nSize);
	return strlen(name);
}

WINBOOL WINAPI GetComputerNameA(LPSTR lpBuffer, LPDWORD nSize)
{
	UNIMPLEMENTED();
}

WINBOOL WINAPI DeleteFileA(LPCSTR lpFileName)
{
	UNIMPLEMENTED();
}

WINBOOL WINAPI CopyFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, WINBOOL bFailIfExists)
{
	UNIMPLEMENTED();
}

HFILE WINAPI OpenFile(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle)
{
	UNIMPLEMENTED();
}

VOID WINAPI InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	UNIMPLEMENTED();
}

VOID WINAPI EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	DUMMY_ONCE();
}

VOID WINAPI LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	DUMMY_ONCE();
}

VOID WINAPI DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	UNIMPLEMENTED();
}

DWORD WINAPI WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds)
{
	UNIMPLEMENTED();
}

HANDLE WINAPI CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, WINBOOL bManualReset, WINBOOL bInitialState,
                           LPCSTR lpName)
{
	DUMMY_PRINT("%s", nullstr(lpName));
	return NULL;
}

HWND WINAPI SetCapture(HWND hWnd)
{
	DUMMY_ONCE();
	return hWnd;
}

WINBOOL WINAPI ReleaseCapture(VOID)
{
	DUMMY_ONCE();
	return TRUE;
}

WINBOOL WINAPI DestroyWindow(HWND hWnd)
{
	UNIMPLEMENTED();
}

HWND WINAPI GetLastActivePopup(HWND hWnd)
{
	UNIMPLEMENTED();
}

HWND WINAPI GetTopWindow(HWND hWnd)
{
	UNIMPLEMENTED();
}

WINBOOL WINAPI SetForegroundWindow(HWND hWnd)
{
	UNIMPLEMENTED();
}

HWND WINAPI SetFocus(HWND hWnd)
{
	UNIMPLEMENTED();
}

HWND WINAPI FindWindowA(LPCSTR lpClassName, LPCSTR lpWindowName)
{
	DUMMY_PRINT("class: %s window: %s", nullstr(lpClassName), nullstr(lpWindowName));
	return NULL;
}

HANDLE WINAPI GetCurrentThread(VOID)
{
	DUMMY_ONCE();
	return NULL;
}

DWORD WINAPI GetCurrentThreadId(VOID)
{
	UNIMPLEMENTED();
}

WINBOOL WINAPI SetThreadPriority(HANDLE hThread, int nPriority)
{
	DUMMY_ONCE();
	return TRUE;
}

VOID WINAPI GetSystemInfo(LPSYSTEM_INFO lpSystemInfo)
{
	DUMMY();
	memset(lpSystemInfo, 0, sizeof(*lpSystemInfo));
	lpSystemInfo->dwPageSize = 4096;
}

HDC WINAPI GetDC(HWND hWnd)
{
	UNIMPLEMENTED();
}

int WINAPI ReleaseDC(HWND hWnd, HDC hDC)
{
	UNIMPLEMENTED();
}

int WINAPI GetDeviceCaps(HDC hdc, int index)
{
	UNIMPLEMENTED();
}

UINT WINAPI GetSystemPaletteEntries(HDC hdc, UINT iStart, UINT cEntries, LPPALETTEENTRY pPalEntries)
{
	UNIMPLEMENTED();
}

uintptr_t __cdecl _beginthreadex(void *_Security, unsigned _StackSize, unsigned(__stdcall *_StartAddress)(void *),
                                 void *_ArgList, unsigned _InitFlag, unsigned *_ThrdAddr)
{
	UNIMPLEMENTED();
}

WINBOOL WINAPI CreateProcessA(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
                              LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles, DWORD dwCreationFlags,
                              LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo,
                              LPPROCESS_INFORMATION lpProcessInformation)
{
	UNIMPLEMENTED();
}

VOID WINAPI ExitProcess(UINT uExitCode)
{
	UNIMPLEMENTED();
}

DWORD WINAPI GetCurrentProcessId(VOID)
{
	UNIMPLEMENTED();
}

HANDLE WINAPI CreateFileMappingA(HANDLE hFile, LPSECURITY_ATTRIBUTES lpFileMappingAttributes, DWORD flProtect,
                                 DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName)
{
	DUMMY();
	assert(hFile == (HANDLE)-1);
	return NULL;
}

LPVOID WINAPI MapViewOfFile(HANDLE hFileMappingObject, DWORD dwDesiredAccess, DWORD dwFileOffsetHigh,
                            DWORD dwFileOffsetLow, SIZE_T dwNumberOfBytesToMap)
{
	UNIMPLEMENTED();
}

WINBOOL WINAPI UnmapViewOfFile(LPCVOID lpBaseAddress)
{
	UNIMPLEMENTED();
}

DWORD WINAPI WaitForInputIdle(HANDLE hProcess, DWORD dwMilliseconds)
{
	UNIMPLEMENTED();
}

HWND WINAPI GetForegroundWindow(VOID)
{
	UNIMPLEMENTED();
}

HWND WINAPI GetWindow(HWND hWnd, UINT uCmd)
{
	UNIMPLEMENTED();
}

DWORD WINAPI GetWindowThreadProcessId(HWND hWnd, LPDWORD lpdwProcessId)
{
	UNIMPLEMENTED();
}

DWORD WINAPI GetPrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString,
                                      DWORD nSize, LPCSTR lpFileName)
{
	UNIMPLEMENTED();
}
