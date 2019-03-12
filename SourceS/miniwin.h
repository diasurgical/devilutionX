#pragma once

#include <ctype.h>
#include <fcntl.h>
#include <math.h>
// work around https://reviews.llvm.org/D51265
#ifdef __APPLE__
#include "macos_stdarg.h"
#else
#include <stdarg.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
// For _rotr()
#include <x86intrin.h>

// Constants
#define CONST const
#define TRUE true
#define FALSE false

#define NO_ERROR 0

// Remove calling conventions (original calling conventions confuse address-sanitizer and aren't supported by all compilers)
#define __cdecl
#define __fastcall
#define __stdcall
#define CALLBACK
#define APIENTRY
#define WINAPI
#define WINAPIV

#define METHOD virtual __stdcall

#define ALIGNED(n) __attribute__((aligned(n)))

// Basic types
#define __int8 char
#define __int16 short
#define __int32 int
#define __int64 long long ALIGNED(8)

#define VOID void

typedef char CHAR;
typedef uint16_t SHORT;
typedef int32_t LONG;
typedef uint8_t BOOLEAN;

typedef LONG *PLONG;
typedef uint32_t ULONG;
typedef ULONG *PULONG;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;
typedef unsigned char UCHAR;
typedef UCHAR *PUCHAR;
typedef char *PSZ;

typedef uint32_t DWORD;
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
typedef void *PVOID;
typedef const void *LPCVOID;
typedef void *HBRUSH;
typedef void *HMENU;
typedef void *HICON;
typedef void *LPITEMIDLIST;
typedef LPITEMIDLIST PIDLIST_ABSOLUTE;
typedef LPITEMIDLIST PCIDLIST_ABSOLUTE;

typedef int INT;
typedef unsigned int UINT;
typedef unsigned int *PUINT;

// GCC qword alignment is 4, MSVC is 8, work around by introducing a more aligned type
typedef long long INT64 ALIGNED(8);
typedef unsigned long long UINT64 ALIGNED(8);

typedef intptr_t INT_PTR, *PINT_PTR;
typedef uintptr_t UINT_PTR, *PUINT_PTR;

typedef intptr_t LONG_PTR, *PLONG_PTR;
typedef uintptr_t ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR SIZE_T;

typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;

typedef CHAR *LPSTR;
typedef CHAR *LPTSTR;
typedef const CHAR *LPCSTR;

typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;

//
// Handles
//
typedef void *HANDLE;
#define INVALID_HANDLE_VALUE ((HANDLE)-1)
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

#define INFINITE 0xFFFFFFFF

#pragma pack(push, 1)
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
#pragma pack(pop)

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

#define MAKEFOURCC(x, y, z, w)             \
	(((uint32_t)((uint8_t)x))              \
	    | (((uint32_t)((uint8_t)y)) << 8)  \
	    | (((uint32_t)((uint8_t)z)) << 16) \
	    | (((uint32_t)((uint8_t)w)) << 24))

typedef uint32_t FOURCC;

typedef struct {
	FOURCC ckid;
	DWORD cksize;
	FOURCC fccType;
	DWORD dwDataOffset;
	DWORD dwFlags;
} MMCKINFO;

//
// System time is represented with the following structure:
//

typedef struct _SYSTEMTIME {
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

typedef struct tagWNDCLASSEXA {
	UINT cbSize;
	UINT style;
	WNDPROC lpfnWndProc;
	int cbClsExtra;
	int cbWndExtra;
	HINSTANCE hInstance;
	HICON hIcon;
	HCURSOR hCursor;
	HBRUSH hbrBackground;
	LPCSTR lpszMenuName;
	LPCSTR lpszClassName;
	HICON hIconSm;
} WNDCLASSEXA;

typedef unsigned long _fsize_t; /* Could be 64 bits for Win32 */

struct _finddata_t {
	unsigned attrib;
	time_t time_create; /* -1 for FAT file systems */
	time_t time_access; /* -1 for FAT file systems */
	time_t time_write;
	_fsize_t size;
	char name[260];
};

typedef WORD ATOM;
#define WINUSERAPI
#define WNDCLASSEX WNDCLASSEXA

#define FOURCC_RIFF MAKEFOURCC('R', 'I', 'F', 'F')

//
// COM
//
#define DECLARE_INTERFACE_(name, base) struct name : public base
#define THIS_
#define THIS
#define PURE = 0

#define CS_HREDRAW 0x0001
#define CS_VREDRAW 0x0002

#define IDC_ARROW (LPCSTR)0x1 // Dummy value

#define CSIDL_STARTMENU 0x000b

#define SW_HIDE 0
#define SW_SHOWNORMAL 1

#define BLACK_BRUSH 4

#define LR_DEFAULTCOLOR 0x0000

#define IMAGE_ICON 1

#define SM_CXSCREEN 0
#define SM_CYSCREEN 1

#define GW_HWNDNEXT 2

#define STDMETHOD(name) STDMETHOD_(HRESULT, name)
#define STDMETHOD_(type, name) virtual WINAPI type name

typedef void *REFIID;

struct IUnknown {
	// clang-format off
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

DWORD WINAPI GetTickCount(VOID);

DWORD WINAPI GetLastError(VOID);
VOID WINAPI SetLastError(DWORD dwErrCode);

WINBOOL WINAPI CloseHandle(HANDLE hObject);

HANDLE WINAPI CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, WINBOOL bManualReset, WINBOOL bInitialState,
    LPCSTR lpName);
#define CreateEvent CreateEventA
BOOL WINAPI SetEvent(HANDLE hEvent);
BOOL WINAPI ResetEvent(HANDLE hEvent);
DWORD WINAPI WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);

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
HWND GetDesktopWindow();
HRESULT SHGetSpecialFolderLocation(HWND hwnd, int csidl, PIDLIST_ABSOLUTE *ppidl);
HWND CreateWindowExA(
    DWORD dwExStyle,
    LPCSTR lpClassName,
    LPCSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam);
#define CreateWindowEx CreateWindowExA
HWND WINAPI FindWindowA(LPCSTR lpClassName, LPCSTR lpWindowName);
#define FindWindow FindWindowA
BOOL InvalidateRect(HWND hWnd, const RECT *lpRect, BOOL bErase);
BOOL UpdateWindow(HWND hWnd);
BOOL ShowWindow(HWND hWnd, int nCmdShow);
WINUSERAPI ATOM WINAPI RegisterClassExA(const WNDCLASSEX *lpwcx);
#define RegisterClassEx RegisterClassExA
int GetSystemMetrics(int nIndex);
HGDIOBJ GetStockObject(int i);
HCURSOR LoadCursorA(HINSTANCE hInstance, LPCSTR lpCursorName);
#define LoadCursor LoadCursorA
BOOL GetUserNameA(LPSTR lpBuffer, LPDWORD pcbBuffer);
#define GetUserName GetUserNameA
void GetLocalTime(LPSYSTEMTIME lpSystemTime);
long __cdecl _findfirst(const char *, struct _finddata_t *);
int __cdecl _findnext(long, struct _finddata_t *);

HICON LoadIconA(HINSTANCE hInstance, LPCSTR lpIconName);
#define LoadIcon LoadIconA
HANDLE LoadImageA(HINSTANCE hInst, LPCSTR name, UINT type, int cx, int cy, UINT fuLoad);
#define LoadImage LoadImageA
BOOL SHGetPathFromIDListA(PCIDLIST_ABSOLUTE pidl, LPSTR pszPath);
#define SHGetPathFromIDList SHGetPathFromIDListA
HINSTANCE ShellExecuteA(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd);
#define ShellExecute ShellExecuteA
int GetClassName(HWND hWnd, LPTSTR lpClassName, int nMaxCount);

typedef LONG(WINAPI *PTOP_LEVEL_EXCEPTION_FILTER)(
    struct _EXCEPTION_POINTERS *ExceptionInfo);
typedef PTOP_LEVEL_EXCEPTION_FILTER LPTOP_LEVEL_EXCEPTION_FILTER;
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI SetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);

HMODULE GetModuleHandleA(LPCSTR lpModuleName);
#define GetModuleHandle GetModuleHandleA

#define THREAD_BASE_PRIORITY_MAX 2
#define THREAD_PRIORITY_NORMAL 0
#define THREAD_PRIORITY_HIGHEST THREAD_BASE_PRIORITY_MAX
#define THREAD_PRIORITY_ABOVE_NORMAL (THREAD_PRIORITY_HIGHEST - 1)

uintptr_t __cdecl _beginthreadex(void *_Security, unsigned _StackSize, unsigned(__stdcall *_StartAddress)(void *),
    void *_ArgList, unsigned _InitFlag, unsigned *_ThrdAddr);
HANDLE WINAPI GetCurrentThread(VOID);
DWORD WINAPI GetCurrentThreadId(VOID);
WINBOOL WINAPI SetThreadPriority(HANDLE hThread, int nPriority);
VOID WINAPI Sleep(DWORD dwMilliseconds);

VOID WINAPI GetSystemInfo(LPSYSTEM_INFO lpSystemInfo);

HDC WINAPI GetDC(HWND hWnd);
int WINAPI ReleaseDC(HWND hWnd, HDC hDC);
WINBOOL WINAPI TextOutA(HDC hdc, int x, int y, LPCSTR lpString, int c);
#define TextOut TextOutA

#define HORZRES 8
#define VERTRES 10
#define NUMRESERVED 106
int WINAPI GetDeviceCaps(HDC hdc, int index);
BOOL GetWindowRect(HWND hDlg, tagRECT *Rect);
UINT WINAPI GetSystemPaletteEntries(HDC hdc, UINT iStart, UINT cEntries, LPPALETTEENTRY pPalEntries);

#define _snprintf snprintf
#define _vsnprintf vsnprintf
int WINAPIV wsprintfA(LPSTR, LPCSTR, ...);
#define wsprintf wsprintfA
int WINAPIV wvsprintfA(LPSTR dest, LPCSTR format, va_list arglist);
#define wvsprintf wvsprintfA
int __cdecl _strcmpi(const char *_Str1, const char *_Str2);
char *__cdecl _itoa(int _Value, char *_Dest, int _Radix);

char *__cdecl _strlwr(char *str);

//
// File I/O
//
#define FILE_BEGIN 0
#define FILE_CURRENT 1
#define FILE_END 2
#define FILE_FLAG_WRITE_THROUGH 0x80000000
#define CREATE_ALWAYS 2
#define GENERIC_READ 0x80000000L
#define GENERIC_WRITE 0x40000000L
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define ERROR_FILE_NOT_FOUND 2
#define FILE_ATTRIBUTE_NORMAL 128
#define FILE_ATTRIBUTE_HIDDEN 0x00000002
#define FILE_ATTRIBUTE_SYSTEM 0x00000004
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#define FILE_SHARE_READ 1

#define OFS_MAXPATHNAME 128
#define MAX_PATH 260

typedef struct _WIN32_FIND_DATAA {
	DWORD dwFileAttributes;
	FILETIME ftCreationTime;
	FILETIME ftLastAccessTime;
	FILETIME ftLastWriteTime;
	DWORD nFileSizeHigh;
	DWORD nFileSizeLow;
	DWORD dwReserved0;
	DWORD dwReserved1;
	CHAR cFileName[MAX_PATH];
	CHAR cAlternateFileName[14];
	DWORD dwFileType;
	DWORD dwCreatorType;
	WORD wFinderFlags;
} WIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;

typedef void *LPOVERLAPPED;

typedef struct _IMAGE_FILE_HEADER {
	WORD Machine;
	WORD NumberOfSections;
	DWORD TimeDateStamp;
	DWORD PointerToSymbolTable;
	DWORD NumberOfSymbols;
	WORD SizeOfOptionalHeader;
	WORD Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

//
// Calculate the byte offset of a field in a structure of type type.
//

#define FIELD_OFFSET(type, field) ((LONG)(INT_PTR) & (((type *)0)->field))
#define IMAGE_FIRST_SECTION(ntheader) ((PIMAGE_SECTION_HEADER)((UINT_PTR)ntheader + FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader) + ((PIMAGE_NT_HEADERS)(ntheader))->FileHeader.SizeOfOptionalHeader))

typedef BOOL(CALLBACK *DLGPROC)(HWND, UINT, WPARAM, LPARAM);

#define IMAGE_NT_SIGNATURE 0x00004550 // PE00

typedef struct _IMAGE_OPTIONAL_HEADER {
	WORD Magic;
	BYTE MajorLinkerVersion;
	BYTE MinorLinkerVersion;
	DWORD SizeOfCode;
	DWORD SizeOfInitializedData;
	DWORD SizeOfUninitializedData;
	DWORD AddressOfEntryPoint;
	DWORD BaseOfCode;
	DWORD BaseOfData;
	DWORD ImageBase;
	DWORD SectionAlignment;
	DWORD FileAlignment;
	WORD MajorOperatingSystemVersion;
	WORD MinorOperatingSystemVersion;
	WORD MajorImageVersion;
	WORD MinorImageVersion;
	WORD MajorSubsystemVersion;
	WORD MinorSubsystemVersion;
	DWORD Win32VersionValue;
	DWORD SizeOfImage;
	DWORD SizeOfHeaders;
	DWORD CheckSum;
	WORD Subsystem;
	WORD DllCharacteristics;
	DWORD SizeOfStackReserve;
	DWORD SizeOfStackCommit;
	DWORD SizeOfHeapReserve;
	DWORD SizeOfHeapCommit;
	DWORD LoaderFlags;
	DWORD NumberOfRvaAndSizes;
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;

typedef struct _IMAGE_NT_HEADERS {
	DWORD Signature;
	IMAGE_FILE_HEADER FileHeader;
	IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} IMAGE_NT_HEADERS, *PIMAGE_NT_HEADERS;

#define IMAGE_DOS_SIGNATURE 0x5A4D

typedef struct _IMAGE_DOS_HEADER {
	WORD e_magic;
	WORD e_cblp;
	WORD e_cp;
	WORD e_crlc;
	WORD e_cparhdr;
	WORD e_minalloc;
	WORD e_maxalloc;
	WORD e_ss;
	WORD e_sp;
	WORD e_csum;
	WORD e_ip;
	WORD e_cs;
	WORD e_lfarlc;
	WORD e_ovno;
	WORD e_res[4];
	WORD e_oemid;
	WORD e_oeminfo;
	WORD e_res2[10];
	LONG e_lfanew;
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

typedef struct _OFSTRUCT {
	BYTE cBytes;
	BYTE fFixedDisk;
	WORD nErrCode;
	WORD Reserved1;
	WORD Reserved2;
	CHAR szPathName[OFS_MAXPATHNAME];
} OFSTRUCT, *LPOFSTRUCT, *POFSTRUCT;

#define VER_PLATFORM_WIN32_NT 2

typedef struct _OSVERSIONINFOA {
	DWORD dwOSVersionInfoSize;
	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	DWORD dwBuildNumber;
	DWORD dwPlatformId;
	CHAR szCSDVersion[128];
} OSVERSIONINFO, *LPOSVERSIONINFOA;

typedef struct _IMAGE_SECTION_HEADER {
	union {
		DWORD PhysicalAddress;
		DWORD VirtualSize;
	} Misc;
	DWORD VirtualAddress;
	DWORD SizeOfRawData;
	DWORD PointerToRawData;
	DWORD PointerToRelocations;
	DWORD PointerToLinenumbers;
	WORD NumberOfRelocations;
	WORD NumberOfLinenumbers;
	DWORD Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

BOOL GetVersionExA(LPOSVERSIONINFOA lpVersionInformation);
#define GetVersionEx GetVersionExA

void lstrcpynA(LPSTR lpString1, LPCSTR lpString2, int iMaxLength);
#define lstrcpyn lstrcpynA

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
	(STANDARD_RIGHTS_REQUIRED | SECTION_QUERY | SECTION_MAP_WRITE | SECTION_MAP_READ | SECTION_MAP_EXECUTE | SECTION_EXTEND_SIZE)

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

LPVOID VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
BOOL VirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);

DWORD WINAPI WaitForInputIdle(HANDLE hProcess, DWORD dwMilliseconds);
HWND WINAPI GetForegroundWindow(VOID);
HWND WINAPI GetWindow(HWND hWnd, UINT uCmd);
DWORD WINAPI GetWindowThreadProcessId(HWND hWnd, LPDWORD lpdwProcessId);

DWORD WINAPI GetPrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString,
    DWORD nSize, LPCSTR lpFileName);
#define GetPrivateProfileString GetPrivateProfileStringA
int MessageBoxA(HWND hWnd, const char *Text, const char *Title, UINT Flags);
#define MessageBox MessageBoxA
typedef LONG LSTATUS, HKEY, REGSAM, PHKEY;
#define HKEY_CURRENT_USER 1
#define KEY_READ 0x20019
#define KEY_WRITE 0x20006
#define REG_SZ 1
LSTATUS RegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
#define RegOpenKeyEx RegOpenKeyExA
LSTATUS RegQueryValueExA(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, BYTE *lpData, LPDWORD lpcbData);
#define RegQueryValueEx RegQueryValueExA
LSTATUS RegSetValueExA(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE *lpData, DWORD cbData);
#define RegSetValueEx RegSetValueExA
LSTATUS RegCloseKeyA(HKEY hKey);
#define RegCloseKey RegCloseKeyA
void PostQuitMessage(int nExitCode);
LRESULT DefWindowProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
#define DefWindowProc DefWindowProcA
LONG GetWindowLongA(HWND hWnd, int nIndex);
#define GetWindowLong GetWindowLongA
LONG SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong);
#define SetWindowLong SetWindowLongA

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
BOOL FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);
#define FindNextFile FindNextFileA
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
DWORD GetCurrentDirectory(DWORD nBufferLength, LPTSTR lpBuffer);
DWORD GetLogicalDriveStringsA(DWORD nBufferLength, LPSTR lpBuffer);
#define GetLogicalDriveStrings GetLogicalDriveStringsA
UINT GetDriveTypeA(LPCSTR lpRootPathName);
#define GetDriveType GetDriveTypeA
WINBOOL WINAPI GetDiskFreeSpaceA(LPCSTR lpRootPathName, LPDWORD lpSectorsPerCluster, LPDWORD lpBytesPerSector,
    LPDWORD lpNumberOfFreeClusters, LPDWORD lpTotalNumberOfClusters);
#define GetDiskFreeSpace GetDiskFreeSpaceA
DWORD WINAPI GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize);
#define GetModuleFileName GetModuleFileNameA
WINBOOL WINAPI GetComputerNameA(LPSTR lpBuffer, LPDWORD nSize);
#define GetComputerName GetComputerNameA
DWORD GetFileVersionInfoSizeA(LPCSTR lptstrFilename, LPDWORD lpdwHandle);
#define GetFileVersionInfoSize GetFileVersionInfoSizeA
BOOL GetFileVersionInfoA(LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData);
#define GetFileVersionInfo GetFileVersionInfoA
BOOL VerQueryValueA(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen);
#define VerQueryValue VerQueryValueA
WINBOOL WINAPI DeleteFileA(LPCSTR lpFileName);
#define DeleteFile DeleteFileA
WINBOOL WINAPI CopyFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, WINBOOL bFailIfExists);
#define CopyFile CopyFileA
HFILE WINAPI OpenFile(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle);

#define GWL_STYLE (-16)

#define WS_POPUP 0x80000000L
#define WS_SYSMENU 0x00080000L

#define DRIVE_CDROM 5

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

#define WM_INITDIALOG 0x0110
#define WM_COMMAND 0x0111
#define WM_SYSCOMMAND 0x0112

#define WM_CHAR 0x0102
#define WM_CAPTURECHANGED 0x0215

#define WM_CREATE 0x0001
#define WM_DESTROY 0x0002
#define WM_PAINT 0x000F
#define WM_CLOSE 0x0010
#define WM_QUERYENDSESSION 0x0011
#define WM_ERASEBKGND 0x0014
#define WM_ACTIVATEAPP 0x001C
#define WM_QUERYNEWPALETTE 0x030F
#define WM_PALETTECHANGED 0x0311

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

#define MK_SHIFT 0x0004
#define MK_LBUTTON 0x0001
#define MK_RBUTTON 0x0002

#define MB_TASKMODAL 0x00002000L
#define MB_ICONHAND 0x00000010L
#define MB_ICONEXCLAMATION 0x00000030L

#define FORMAT_MESSAGE_IGNORE_INSERTS 0x00000200
#define FORMAT_MESSAGE_FROM_HMODULE 0x00000800
#define FORMAT_MESSAGE_FROM_SYSTEM 0x00001000

#define STATUS_BREAKPOINT ((DWORD)0x80000003L)
#define STATUS_GUARD_PAGE_VIOLATION ((DWORD)0x80000001L)
#define STATUS_FLOAT_DIVIDE_BY_ZERO ((DWORD)0xC000008EL)
#define STATUS_SINGLE_STEP ((DWORD)0x80000004L)
#define STATUS_ARRAY_BOUNDS_EXCEEDED ((DWORD)0xC000008CL)
#define STATUS_ACCESS_VIOLATION ((DWORD)0xC0000005L)
#define STATUS_IN_PAGE_ERROR ((DWORD)0xC0000006L)
#define STATUS_ILLEGAL_INSTRUCTION ((DWORD)0xC000001DL)
#define STATUS_DATATYPE_MISALIGNMENT ((DWORD)0x80000002L)
#define STATUS_FLOAT_DENORMAL_OPERAND ((DWORD)0xC000008DL)
#define STATUS_NONCONTINUABLE_EXCEPTION ((DWORD)0xC0000025L)
#define STATUS_INTEGER_DIVIDE_BY_ZERO ((DWORD)0xC0000094L)
#define STATUS_INVALID_HANDLE ((DWORD)0xC0000008L)
#define STATUS_FLOAT_OVERFLOW ((DWORD)0xC0000091L)
#define STATUS_ILLEGAL_INSTRUCTION ((DWORD)0xC000001DL)
#define STATUS_GUARD_PAGE_VIOLATION ((DWORD)0x80000001L)
#define STATUS_ILLEGAL_INSTRUCTION ((DWORD)0xC000001DL)
#define STATUS_INTEGER_OVERFLOW ((DWORD)0xC0000095L)
#define STATUS_PRIVILEGED_INSTRUCTION ((DWORD)0xC0000096L)
#define STATUS_FLOAT_UNDERFLOW ((DWORD)0xC0000093L)
#define STATUS_FLOAT_INEXACT_RESULT ((DWORD)0xC000008FL)
#define STATUS_FLOAT_INVALID_OPERATION ((DWORD)0xC0000090L)
#define STATUS_FLOAT_STACK_CHECK ((DWORD)0xC0000092L)
#define STATUS_INVALID_DISPOSITION ((DWORD)0xC0000026L)
#define STATUS_STACK_OVERFLOW ((DWORD)0xC00000FDL)

#define EXCEPTION_CONTINUE_SEARCH 0x0
#define EXCEPTION_ACCESS_VIOLATION STATUS_ACCESS_VIOLATION
#define EXCEPTION_BREAKPOINT STATUS_BREAKPOINT
#define EXCEPTION_GUARD_PAGE STATUS_GUARD_PAGE_VIOLATION
#define EXCEPTION_FLT_DIVIDE_BY_ZERO STATUS_FLOAT_DIVIDE_BY_ZERO
#define EXCEPTION_SINGLE_STEP STATUS_SINGLE_STEP
#define EXCEPTION_ARRAY_BOUNDS_EXCEEDED STATUS_ARRAY_BOUNDS_EXCEEDED
#define EXCEPTION_IN_PAGE_ERROR STATUS_IN_PAGE_ERROR
#define EXCEPTION_ILLEGAL_INSTRUCTION STATUS_ILLEGAL_INSTRUCTION
#define EXCEPTION_DATATYPE_MISALIGNMENT STATUS_DATATYPE_MISALIGNMENT
#define EXCEPTION_FLT_DENORMAL_OPERAND STATUS_FLOAT_DENORMAL_OPERAND
#define EXCEPTION_NONCONTINUABLE_EXCEPTION STATUS_NONCONTINUABLE_EXCEPTION
#define EXCEPTION_INT_DIVIDE_BY_ZERO STATUS_INTEGER_DIVIDE_BY_ZERO
#define EXCEPTION_INVALID_HANDLE STATUS_INVALID_HANDLE
#define EXCEPTION_FLT_OVERFLOW STATUS_FLOAT_OVERFLOW
#define EXCEPTION_ILLEGAL_INSTRUCTION STATUS_ILLEGAL_INSTRUCTION
#define EXCEPTION_GUARD_PAGE STATUS_GUARD_PAGE_VIOLATION
#define EXCEPTION_ILLEGAL_INSTRUCTION STATUS_ILLEGAL_INSTRUCTION
#define EXCEPTION_INT_OVERFLOW STATUS_INTEGER_OVERFLOW
#define EXCEPTION_PRIV_INSTRUCTION STATUS_PRIVILEGED_INSTRUCTION
#define EXCEPTION_FLT_UNDERFLOW STATUS_FLOAT_UNDERFLOW
#define EXCEPTION_FLT_INEXACT_RESULT STATUS_FLOAT_INEXACT_RESULT
#define EXCEPTION_FLT_INVALID_OPERATION STATUS_FLOAT_INVALID_OPERATION
#define EXCEPTION_FLT_STACK_CHECK STATUS_FLOAT_STACK_CHECK
#define EXCEPTION_INVALID_DISPOSITION STATUS_INVALID_DISPOSITION
#define EXCEPTION_STACK_OVERFLOW STATUS_STACK_OVERFLOW

#define HWND_NOTOPMOST (HWND) - 2
#define HWND_TOP (HWND)0

#define SWP_NOACTIVATE 0x0010
#define SWP_NOMOVE 0x0002
#define SWP_NOSIZE 0x0004
#define SWP_NOZORDER 0x0001

typedef struct _CONTEXT {

	//
	// The flags values within this flag control the contents of
	// a CONTEXT record.
	//
	// If the context record is used as an input parameter, then
	// for each portion of the context record controlled by a flag
	// whose value is set, it is assumed that that portion of the
	// context record contains valid context. If the context record
	// is being used to modify a threads context, then only that
	// portion of the threads context will be modified.
	//
	// If the context record is used as an IN OUT parameter to capture
	// the context of a thread, then only those portions of the thread's
	// context corresponding to set flags will be returned.
	//
	// The context record is never used as an OUT only parameter.
	//

	DWORD ContextFlags;

	//
	// This section is specified/returned if CONTEXT_DEBUG_REGISTERS is
	// set in ContextFlags.  Note that CONTEXT_DEBUG_REGISTERS is NOT
	// included in CONTEXT_FULL.
	//

	DWORD Dr0;
	DWORD Dr1;
	DWORD Dr2;
	DWORD Dr3;
	DWORD Dr6;
	DWORD Dr7;

	//
	// This section is specified/returned if the
	// ContextFlags word contians the flag CONTEXT_SEGMENTS.
	//

	DWORD SegGs;
	DWORD SegFs;
	DWORD SegEs;
	DWORD SegDs;

	//
	// This section is specified/returned if the
	// ContextFlags word contians the flag CONTEXT_INTEGER.
	//

	DWORD Edi;
	DWORD Esi;
	DWORD Ebx;
	DWORD Edx;
	DWORD Ecx;
	DWORD Eax;

	//
	// This section is specified/returned if the
	// ContextFlags word contians the flag CONTEXT_CONTROL.
	//

	DWORD Ebp;
	DWORD Eip;
	DWORD SegCs;  // MUST BE SANITIZED
	DWORD EFlags; // MUST BE SANITIZED
	DWORD Esp;
	DWORD SegSs;
} CONTEXT;

typedef CONTEXT *PCONTEXT;

//
// Exception record definition.
//

typedef struct _EXCEPTION_RECORD {
	DWORD ExceptionCode;
	DWORD ExceptionFlags;
	struct _EXCEPTION_RECORD *ExceptionRecord;
	PVOID ExceptionAddress;
	DWORD NumberParameters;
} EXCEPTION_RECORD;

typedef EXCEPTION_RECORD *PEXCEPTION_RECORD;

typedef struct _EXCEPTION_POINTERS {
	PEXCEPTION_RECORD ExceptionRecord;
	PCONTEXT ContextRecord;
} EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;

typedef struct _MEMORY_BASIC_INFORMATION {
	PVOID BaseAddress;
	PVOID AllocationBase;
	DWORD AllocationProtect;
	SIZE_T RegionSize;
	DWORD State;
	DWORD Protect;
	DWORD Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;

//
// Total fakes
//
typedef struct {
} SOCKADDR, GUID, *LPGUID;

typedef struct {
	DWORD cb;
} STARTUPINFOA;

BOOL IsBadReadPtr(const VOID *lp, UINT_PTR ucb);
BOOL IsBadWritePtr(LPVOID lp, UINT_PTR ucb);
SIZE_T VirtualQuery(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength);

//
// MSCVRT emulation
//

extern void LoadCharNames();
extern void LoadAndPlaySound(char *FilePath, int lVolume, int lPan);
extern void DrawArtWithMask(int SX, int SY, int SW, int SH, int nFrame, BYTE bMask, void *pBuffer);
extern BOOL __cdecl LoadArtWithPal(char *pszFile, void **pBuffer, int frames, DWORD *data);

#define OF_EXIST 1

#include "miniwin_ddraw.h"
#include "miniwin_dsound.h"
#include "miniwin_ui.h"
#include "miniwin_thread.h"
#include "miniwin_rand.h"
