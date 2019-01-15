#pragma once

#ifdef NO_GLOBALS
#define STATIC extern
#else
#define STATIC static
#endif

#include <vector>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// For _rotr()
#include <x86intrin.h>

// Constants
#define CONST const
#define TRUE true
#define FALSE false

#define NO_ERROR 0

// Calling conventions
#define __cdecl __attribute__((cdecl))
#define __fastcall __attribute__((fastcall))
#define __stdcall __attribute__((stdcall))
#define CALLBACK __stdcall
#define WINAPI __stdcall
#define WINAPIV __cdecl

#define ALIGNED(n) __attribute__((aligned(n)))

// Basic types
#define __int8 char
#define __int16 short
#define __int32 int
#define __int64 long long ALIGNED(8)

#define VOID void

extern void *prealpha_mpq;

typedef char CHAR;
typedef uint16_t SHORT;
typedef int32_t LONG;

typedef LONG *PLONG;
typedef unsigned long ULONG;
typedef ULONG *PULONG;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;
typedef unsigned char UCHAR;
typedef UCHAR *PUCHAR;
typedef char *PSZ;

typedef unsigned long DWORD;
typedef int BOOL, WINBOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef float FLOAT;
typedef FLOAT *PFLOAT;
typedef BOOL *LPBOOL;
typedef BYTE *LPBYTE;
typedef int *LPINT;
typedef WORD *LPWORD;
typedef long *LPLONG;
typedef DWORD *LPDWORD;
typedef void *LPVOID;
typedef CONST void *LPCVOID;

typedef int INT;
typedef unsigned int UINT;
typedef unsigned int *PUINT;

// GCC qword alignment is 4, MSVC is 8, work around by introducing a more aligned type
typedef long long INT64 ALIGNED(8);
typedef unsigned long long UINT64 ALIGNED(8);

typedef int INT_PTR, *PINT_PTR;
typedef unsigned int UINT_PTR, *PUINT_PTR;

typedef int32_t LONG_PTR, *PLONG_PTR;
typedef uint32_t ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR SIZE_T;

typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;

typedef CHAR *LPSTR;
typedef CONST CHAR *LPCSTR;

typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;

//
// Handles
//
typedef void *HANDLE;
#define INVALID_HANDLE ((HANDLE)-1)
#define HFILE_ERROR ((HFILE)-1)

typedef HANDLE HWND, HGDIOBJ, HMODULE, HDC, HRGN, HINSTANCE, HPALETTE, HFILE, HCURSOR;

typedef LONG LCID;

typedef DWORD COLORREF;

typedef LONG HRESULT;

typedef LRESULT(CALLBACK *WNDPROC)(HWND, UINT, WPARAM, LPARAM);

//
// Intrinsics
//
#define LOBYTE(w) ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w) ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))
#define LOWORD(l) ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l) ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))

#define InterlockedIncrement(x) __sync_add_and_fetch(x, 1)

typedef struct waveformat_tag {
	WORD wFormatTag;
	WORD nChannels;
	DWORD nSamplesPerSec;
	DWORD nAvgBytesPerSec;
	WORD nBlockAlign;
} WAVEFORMAT, *PWAVEFORMAT, *LPWAVEFORMAT;

typedef struct pcmwaveformat_tag {
	WAVEFORMAT wf;
	WORD wBitsPerSample;
} PCMWAVEFORMAT, *PPCMWAVEFORMAT, *LPPCMWAVEFORMAT;

typedef struct tWAVEFORMATEX {
	WORD wFormatTag;
	WORD nChannels;
	DWORD nSamplesPerSec;
	DWORD nAvgBytesPerSec;
	WORD nBlockAlign;
	WORD wBitsPerSample;
	WORD cbSize;
} WAVEFORMATEX, *LPWAVEFORMATEX, *LPCWAVEFORMATEX;

typedef struct _FILETIME {
	DWORD dwLowDateTime;
	DWORD dwHighDateTime;
} FILETIME, *LPFILETIME;

typedef struct tagRECT {
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;
} RECT;

typedef RECT *LPRECT;

typedef struct tagPOINT {
	LONG x;
	LONG y;
} POINT;

typedef struct tagSIZE {
	LONG cx;
	LONG cy;
} SIZE;

typedef struct tagVS_FIXEDFILEINFO {
	DWORD dwSignature;
	DWORD dwStrucVersion;
	DWORD dwFileVersionMS;
	DWORD dwFileVersionLS;
	DWORD dwProductVersionMS;
	DWORD dwProductVersionLS;
	DWORD dwFileFlagsMask;
	DWORD dwFileFlags;
	DWORD dwFileOS;
	DWORD dwFileType;
	DWORD dwFileSubtype;
	DWORD dwFileDateMS;
	DWORD dwFileDateLS;
} VS_FIXEDFILEINFO;

typedef struct tagMSG {
	HWND hwnd;
	UINT message;
	WPARAM wParam;
	LPARAM lParam;
	DWORD time;
	POINT pt;
} MSG, *LPMSG;

//
// COM
//
#define DECLARE_INTERFACE_(name, base) struct name : public base
#define THIS_
#define THIS
#define PURE = 0

#define STDMETHOD(name) STDMETHOD_(HRESULT, name)
#define STDMETHOD_(type, name) virtual WINAPI type name

typedef void *REFIID;

struct IUnknown {
	// clang-format off
	STDMETHOD(QueryInterface)(THIS_ REFIID, LPVOID *) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;
	// clang-format on
};

#define MAKE_HRESULT(sev, fac, code) ((HRESULT)(((uint32_t)(sev) << 31) | ((uint32_t)(fac) << 16) | ((uint32_t)(code))))
#define E_FAIL ((HRESULT)0x80004005L)
#define S_OK ((HRESULT)0)

//
// Everything else
//
typedef struct tagPALETTEENTRY {
	BYTE peRed;
	BYTE peGreen;
	BYTE peBlue;
	BYTE peFlags;
} PALETTEENTRY, *PPALETTEENTRY, *LPPALETTEENTRY;

typedef void *LPTOP_LEVEL_EXCEPTION_FILTER, *PEXCEPTION_POINTERS;

typedef struct _SYSTEM_INFO {
	union {
		DWORD dwOemId;
		struct {
			WORD wProcessorArchitecture;
			WORD wReserved;
		};
	};
	DWORD dwPageSize;
	LPVOID lpMinimumApplicationAddress;
	LPVOID lpMaximumApplicationAddress;
	DWORD_PTR dwActiveProcessorMask;
	DWORD dwNumberOfProcessors;
	DWORD dwProcessorType;
	DWORD dwAllocationGranularity;
	WORD wProcessorLevel;
	WORD wProcessorRevision;
} SYSTEM_INFO, *LPSYSTEM_INFO;

typedef void *LPSECURITY_ATTRIBUTES;

#define ERROR_ALREADY_EXISTS 183

typedef struct _LIST_ENTRY {
	struct _LIST_ENTRY *Flink;
	struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY;

typedef struct _RTL_CRITICAL_SECTION_DEBUG {
	WORD Type;
	WORD CreatorBackTraceIndex;
	struct _RTL_CRITICAL_SECTION *CriticalSection;
	LIST_ENTRY ProcessLocksList;
	DWORD EntryCount;
	DWORD ContentionCount;
	DWORD Flags;
	WORD CreatorBackTraceIndexHigh;
	WORD SpareWORD;
} RTL_CRITICAL_SECTION_DEBUG, *PRTL_CRITICAL_SECTION_DEBUG;

typedef struct {
	PRTL_CRITICAL_SECTION_DEBUG DebugInfo;
	LONG LockCount;
	LONG RecursionCount;
	HANDLE OwningThread;
	HANDLE LockSemaphore;
	ULONG_PTR SpinCount;
} CRITICAL_SECTION, *LPCRITICAL_SECTION;

VOID WINAPI InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
VOID WINAPI EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
VOID WINAPI LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
VOID WINAPI DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);

DWORD WINAPI WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);

DWORD WINAPI GetTickCount(VOID);

DWORD WINAPI GetLastError(VOID);
VOID WINAPI SetLastError(DWORD dwErrCode);

WINBOOL WINAPI CloseHandle(HANDLE hObject);

HANDLE WINAPI CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, WINBOOL bManualReset, WINBOOL bInitialState,
                           LPCSTR lpName);
#define CreateEvent CreateEventA

WINBOOL WINAPI SetCursorPos(int X, int Y);
int WINAPI ShowCursor(WINBOOL bShow);
HWND WINAPI SetCapture(HWND hWnd);
WINBOOL WINAPI ReleaseCapture(VOID);

SHORT WINAPI GetAsyncKeyState(int vKey);

#define PM_NOREMOVE 0x0000
#define PM_REMOVE 0x0001

#define WM_QUIT 0x0012

WINBOOL WINAPI PeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
#define PeekMessage PeekMessageA
WINBOOL WINAPI TranslateMessage(CONST MSG *lpMsg);
LRESULT WINAPI DispatchMessageA(CONST MSG *lpMsg);
#define DispatchMessage DispatchMessageA
WINBOOL WINAPI PostMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
#define PostMessage PostMessageA

WINBOOL WINAPI DestroyWindow(HWND hWnd);
HWND WINAPI GetLastActivePopup(HWND hWnd);
HWND WINAPI GetTopWindow(HWND hWnd);
WINBOOL WINAPI SetForegroundWindow(HWND hWnd);
HWND WINAPI SetFocus(HWND hWnd);
HWND WINAPI FindWindowA(LPCSTR lpClassName, LPCSTR lpWindowName);
#define FindWindow FindWindowA

#define THREAD_BASE_PRIORITY_MAX 2
#define THREAD_PRIORITY_NORMAL 0
#define THREAD_PRIORITY_HIGHEST THREAD_BASE_PRIORITY_MAX
#define THREAD_PRIORITY_ABOVE_NORMAL (THREAD_PRIORITY_HIGHEST - 1)

HANDLE WINAPI GetCurrentThread(VOID);
DWORD WINAPI GetCurrentThreadId(VOID);
WINBOOL WINAPI SetThreadPriority(HANDLE hThread, int nPriority);
VOID WINAPI Sleep(DWORD dwMilliseconds);

VOID WINAPI GetSystemInfo(LPSYSTEM_INFO lpSystemInfo);

HDC WINAPI GetDC(HWND hWnd);
int WINAPI ReleaseDC(HWND hWnd, HDC hDC);
WINBOOL WINAPI TextOutA(HDC hdc, int x, int y, LPCSTR lpString, int c);
#define TextOut TextOutA

#define NUMRESERVED 106
int WINAPI GetDeviceCaps(HDC hdc, int index);
UINT WINAPI GetSystemPaletteEntries(HDC hdc, UINT iStart, UINT cEntries, LPPALETTEENTRY pPalEntries);

uintptr_t __cdecl _beginthreadex(void *_Security, unsigned _StackSize, unsigned(__stdcall *_StartAddress)(void *),
                                 void *_ArgList, unsigned _InitFlag, unsigned *_ThrdAddr);
int WINAPIV wsprintfA(LPSTR, LPCSTR, ...);
#define wsprintf wsprintfA
int __cdecl _strcmpi(const char *_Str1, const char *_Str2);
char *__cdecl _itoa(int _Value, char *_Dest, int _Radix);

char *__cdecl _strlwr(char *str);

//
// File I/O
//
#define FILE_BEGIN 0
#define FILE_CURRENT 1
#define FILE_FLAG_WRITE_THROUGH 0x80000000
#define CREATE_ALWAYS 2
#define GENERIC_READ 0x80000000L
#define GENERIC_WRITE 0x40000000L
#define OPEN_EXISTING 3
#define ERROR_FILE_NOT_FOUND 2
#define FILE_ATTRIBUTE_HIDDEN 0x00000002
#define FILE_ATTRIBUTE_SYSTEM 0x00000004

struct _WIN32_FIND_DATAA {
	FILETIME ftCreationTime;
	FILETIME ftLastWriteTime;
};

typedef struct _WIN32_FIND_DATAA *LPWIN32_FIND_DATAA;

typedef void *LPOVERLAPPED;

#define OFS_MAXPATHNAME 128
#define MAX_PATH 260

typedef struct _OFSTRUCT {
	BYTE cBytes;
	BYTE fFixedDisk;
	WORD nErrCode;
	WORD Reserved1;
	WORD Reserved2;
	CHAR szPathName[OFS_MAXPATHNAME];
} OFSTRUCT, *LPOFSTRUCT, *POFSTRUCT;

#define SEC_COMMIT 0x8000000
#define PAGE_READWRITE 0x04

#define FILE_MAP_ALL_ACCESS SECTION_ALL_ACCESS
#define SECTION_QUERY 0x0001
#define SECTION_MAP_WRITE 0x0002
#define SECTION_MAP_READ 0x0004
#define SECTION_MAP_EXECUTE 0x0008
#define SECTION_EXTEND_SIZE 0x0010
#define SECTION_MAP_EXECUTE_EXPLICIT 0x0020
#define STANDARD_RIGHTS_REQUIRED 0x000F0000

#define SECTION_ALL_ACCESS \
	(STANDARD_RIGHTS_REQUIRED | SECTION_QUERY | SECTION_MAP_WRITE | SECTION_MAP_READ | SECTION_MAP_EXECUTE | \
	 SECTION_EXTEND_SIZE)

#define CREATE_NEW_PROCESS_GROUP 0x200

typedef struct _PROCESS_INFORMATION {
	HANDLE hProcess;
	HANDLE hThread;
	DWORD dwProcessId;
	DWORD dwThreadId;
} PROCESS_INFORMATION, *PPROCESS_INFORMATION, *LPPROCESS_INFORMATION;

typedef void *LPSTARTUPINFOA;
WINBOOL WINAPI CreateProcessA(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
                              LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles, DWORD dwCreationFlags,
                              LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo,
                              LPPROCESS_INFORMATION lpProcessInformation);
#define CreateProcess CreateProcessA
VOID WINAPI ExitProcess(UINT uExitCode);
DWORD WINAPI GetCurrentProcessId(VOID);

HANDLE WINAPI CreateFileMappingA(HANDLE hFile, LPSECURITY_ATTRIBUTES lpFileMappingAttributes, DWORD flProtect,
                                 DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName);
#define CreateFileMapping CreateFileMappingA
LPVOID WINAPI MapViewOfFile(HANDLE hFileMappingObject, DWORD dwDesiredAccess, DWORD dwFileOffsetHigh,
                            DWORD dwFileOffsetLow, SIZE_T dwNumberOfBytesToMap);
WINBOOL WINAPI UnmapViewOfFile(LPCVOID lpBaseAddress);

DWORD WINAPI WaitForInputIdle(HANDLE hProcess, DWORD dwMilliseconds);
HWND WINAPI GetForegroundWindow(VOID);
HWND WINAPI GetWindow(HWND hWnd, UINT uCmd);
DWORD WINAPI GetWindowThreadProcessId(HWND hWnd, LPDWORD lpdwProcessId);

DWORD WINAPI GetPrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString,
                                      DWORD nSize, LPCSTR lpFileName);
#define GetPrivateProfileString GetPrivateProfileStringA

WINBOOL WINAPI WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten,
                         LPOVERLAPPED lpOverlapped);
DWORD WINAPI SetFilePointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod);
WINBOOL WINAPI SetEndOfFile(HANDLE hFile);
DWORD WINAPI GetFileAttributesA(LPCSTR lpFileName);
#define GetFileAttributes GetFileAttributesA
WINBOOL WINAPI SetFileAttributesA(LPCSTR lpFileName, DWORD dwFileAttributes);
#define SetFileAttributes SetFileAttributesA
HANDLE WINAPI FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);
#define FindFirstFile FindFirstFileA
WINBOOL WINAPI FindClose(HANDLE hFindFile);
HANDLE WINAPI CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                          LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                          DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
#define CreateFile CreateFileA
WINBOOL WINAPI ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead,
                        LPOVERLAPPED lpOverlapped);
DWORD WINAPI GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);
UINT WINAPI GetWindowsDirectoryA(LPSTR lpBuffer, UINT uSize);
#define GetWindowsDirectory GetWindowsDirectoryA
WINBOOL WINAPI GetDiskFreeSpaceA(LPCSTR lpRootPathName, LPDWORD lpSectorsPerCluster, LPDWORD lpBytesPerSector,
                                 LPDWORD lpNumberOfFreeClusters, LPDWORD lpTotalNumberOfClusters);
#define GetDiskFreeSpace GetDiskFreeSpaceA
DWORD WINAPI GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize);
#define GetModuleFileName GetModuleFileNameA
WINBOOL WINAPI GetComputerNameA(LPSTR lpBuffer, LPDWORD nSize);
#define GetComputerName GetComputerNameA
WINBOOL WINAPI DeleteFileA(LPCSTR lpFileName);
#define DeleteFile DeleteFileA
WINBOOL WINAPI CopyFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, WINBOOL bFailIfExists);
#define CopyFile CopyFileA
HFILE WINAPI OpenFile(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle);

//
// Events
//
#define WM_MOUSEFIRST 0x0200
#define WM_MOUSEMOVE 0x0200
#define WM_LBUTTONDOWN 0x0201
#define WM_LBUTTONUP 0x0202
#define WM_RBUTTONDOWN 0x0204
#define WM_RBUTTONUP 0x0205

#define WM_KEYFIRST 0x0100
#define WM_KEYDOWN 0x0100
#define WM_KEYUP 0x0101
#define WM_SYSKEYDOWN 0x0104

#define WM_SYSCOMMAND 0x0112

#define WM_CHAR 0x0102
#define WM_CAPTURECHANGED 0x0215

#define SC_CLOSE 0xF060

#define VK_RETURN 0x0D
#define VK_BACK 0x08
#define VK_SHIFT 0x10
#define VK_ESCAPE 0x1B
#define VK_SPACE 0x20
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28

#define VK_F1 0x70
#define VK_F2 0x71
#define VK_F3 0x72
#define VK_F4 0x73
#define VK_F5 0x74
#define VK_F6 0x75
#define VK_F7 0x76
#define VK_F8 0x77
#define VK_F9 0x78
#define VK_F10 0x79
#define VK_F11 0x7A
#define VK_F12 0x7B

#define VK_TAB 0x09
#define VK_PAUSE 0x13
#define VK_PRIOR 0x21
#define VK_NEXT 0x22
#define VK_SNAPSHOT 0x2C

#define VK_OEM_1 0xBA
#define VK_OEM_PLUS 0xBB
#define VK_OEM_COMMA 0xBC
#define VK_OEM_MINUS 0xBD
#define VK_OEM_PERIOD 0xBE
#define VK_OEM_2 0xBF
#define VK_OEM_3 0xC0
#define VK_OEM_4 0xDB
#define VK_OEM_5 0xDC
#define VK_OEM_6 0xDD
#define VK_OEM_7 0xDE
//#define VK_OEM_8 0xDF
//#define VK_OEM_102 0xE2

//
// Total fakes
//
typedef struct {
} SOCKADDR, GUID, *LPGUID;

typedef struct {
	DWORD cb;
} STARTUPINFOA;

//
// MSCVRT emulation
//

int rand_miniwin(void);
void srand_miniwin(unsigned int seed);

extern void LoadCharNames();
extern void LoadAndPlaySound(char *FilePath, int lVolume, int lPan);
extern void DrawArtWithMask(int SX, int SY, int SW, int SH, int nFrame, BYTE bMask, void *pBuffer);
extern BOOL __cdecl LoadArtWithPal(char *pszFile, void **pBuffer, int frames, DWORD *data);


#include "miniwin_ddraw.h"
#include "miniwin_dsound.h"
