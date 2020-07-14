#pragma once

namespace dvl {

constexpr auto DVL_OFS_MAXPATHNAME = 128;
constexpr auto DVL_MAX_PATH = 260;

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
typedef void *HMENU;
typedef void *LPITEMIDLIST;
typedef LPITEMIDLIST PIDLIST_ABSOLUTE;
typedef LPITEMIDLIST PCIDLIST_ABSOLUTE;

typedef int INT;
typedef unsigned int UINT;
typedef unsigned int *PUINT;

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

typedef HANDLE HWND, HGDIOBJ, HMODULE, HDC, HRGN, HINSTANCE, HPALETTE, HCURSOR;

typedef LONG LCID;

typedef DWORD COLORREF;

typedef LRESULT(CALLBACK *WNDPROC)(HWND, UINT, WPARAM, LPARAM);

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

typedef struct tagMSG {
	HWND hwnd;
	UINT message;
	WPARAM wParam;
	LPARAM lParam;
	DWORD time;
	POINT pt;
} MSG, *LPMSG;

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

typedef unsigned long _fsize_t; /* Could be 64 bits for Win32 */

struct DVL_finddata_t {
	unsigned attrib;
	time_t time_create; /* -1 for FAT file systems */
	time_t time_access; /* -1 for FAT file systems */
	time_t time_write;
	_fsize_t size;
	char name[DVL_MAX_PATH];
};

typedef WORD ATOM;

//
// Everything else
//
typedef struct tagPALETTEENTRY {
	BYTE peRed;
	BYTE peGreen;
	BYTE peBlue;
	BYTE peFlags;
} PALETTEENTRY, *PPALETTEENTRY, *LPPALETTEENTRY;

typedef void *LPSECURITY_ATTRIBUTES;

typedef struct _LIST_ENTRY {
	struct _LIST_ENTRY *Flink;
	struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY;

DWORD WINAPI GetTickCount();

DWORD WINAPI GetLastError();
void WINAPI SetLastError(DWORD dwErrCode);

WINBOOL WINAPI CloseHandle(HANDLE hObject);

HANDLE WINAPI CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, WINBOOL bManualReset, WINBOOL bInitialState,
    LPCSTR lpName);
BOOL CloseEvent(HANDLE event);
void SetEvent(HANDLE hEvent);
void ResetEvent(HANDLE hEvent);
int WINAPI WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);

#ifdef __ANDROID__
WINBOOL SetCursorPos(int X, int Y);
#endif
#ifndef __ANDROID__
void SetCursorPos(int X, int Y);
#endif
void FocusOnCharInfo();

SHORT WINAPI GetAsyncKeyState(int vKey);

WINBOOL WINAPI PeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);

WINBOOL WINAPI TranslateMessage(const MSG *lpMsg);
LRESULT WINAPI DispatchMessageA(const MSG *lpMsg);
WINBOOL WINAPI PostMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

bool SpawnWindow(LPCSTR lpWindowName, int nWidth, int nHeight);

typedef LONG(WINAPI *PTOP_LEVEL_EXCEPTION_FILTER)(
    struct _EXCEPTION_POINTERS *ExceptionInfo);

uintptr_t __cdecl DVL_beginthreadex(void *_Security, unsigned _StackSize, unsigned(__stdcall *_StartAddress)(void *),
    void *_ArgList, unsigned _InitFlag, unsigned *_ThrdAddr);
HANDLE WINAPI GetCurrentThread();
DWORD WINAPI GetCurrentThreadId();
WINBOOL WINAPI SetThreadPriority(HANDLE hThread, int nPriority);
void WINAPI Sleep(DWORD dwMilliseconds);

int WINAPIV wsprintfA(LPSTR, LPCSTR, ...);
int WINAPIV wvsprintfA(LPSTR dest, LPCSTR format, va_list arglist);
int __cdecl _strcmpi(const char *_Str1, const char *_Str2);
int __cdecl _strnicmp(const char *_Str1, const char *_Str2, size_t n);
char *__cdecl _itoa(int _Value, char *_Dest, int _Radix);

//
// File I/O
//

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

typedef LONG LSTATUS, HKEY, REGSAM, PHKEY;

WINBOOL WINAPI WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped);
DWORD WINAPI SetFilePointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod);
WINBOOL WINAPI SetEndOfFile(HANDLE hFile);
DWORD WINAPI GetFileAttributesA(LPCSTR lpFileName);
WINBOOL WINAPI SetFileAttributesA(LPCSTR lpFileName, DWORD dwFileAttributes);
HANDLE WINAPI CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
WINBOOL WINAPI ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped);
DWORD WINAPI GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);
WINBOOL WINAPI GetComputerNameA(LPSTR lpBuffer, LPDWORD nSize);
WINBOOL WINAPI DeleteFileA(LPCSTR lpFileName);

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
	// ContextFlags word contains the flag CONTEXT_SEGMENTS.
	//

	DWORD SegGs;
	DWORD SegFs;
	DWORD SegEs;
	DWORD SegDs;

	//
	// This section is specified/returned if the
	// ContextFlags word contains the flag CONTEXT_INTEGER.
	//

	DWORD Edi;
	DWORD Esi;
	DWORD Ebx;
	DWORD Edx;
	DWORD Ecx;
	DWORD Eax;

	//
	// This section is specified/returned if the
	// ContextFlags word contains the flag CONTEXT_CONTROL.
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
} SOCKADDR;

//
// MSCVRT emulation
//

extern void LoadCharNames();
extern void LoadAndPlaySound(char *FilePath, int lVolume, int lPan);
extern void DrawArtWithMask(int SX, int SY, int SW, int SH, int nFrame, BYTE bMask, void *pBuffer);
extern BOOL __cdecl LoadArtWithPal(char *pszFile, void **pBuffer, int frames, DWORD *data);

constexpr auto DVL_WM_SYSKEYUP = 0x0105;
constexpr auto DVL_VER_PLATFORM_WIN32_NT = 2;

constexpr auto DVL_CREATE_ALWAYS = 2;
constexpr auto DVL_GENERIC_READ = 0x80000000L;
constexpr auto DVL_GENERIC_WRITE = 0x40000000L;
constexpr auto DVL_OPEN_EXISTING = 3;
constexpr auto DVL_FILE_BEGIN = 0;
constexpr auto DVL_FILE_CURRENT = 1;
constexpr auto DVL_ERROR_FILE_NOT_FOUND = 2;

constexpr auto DVL_PM_NOREMOVE = 0x0000;
constexpr auto DVL_PM_REMOVE = 0x0001;
constexpr auto DVL_WM_QUIT = 0x0012;
constexpr auto DVL_INFINITE = 0xFFFFFFFF;

//
// Events
//
constexpr auto DVL_WM_MOUSEFIRST = 0x0200;
constexpr auto DVL_WM_MOUSEMOVE = 0x0200;
constexpr auto DVL_WM_LBUTTONDOWN = 0x0201;
constexpr auto DVL_WM_LBUTTONUP = 0x0202;
constexpr auto DVL_WM_RBUTTONDOWN = 0x0204;
constexpr auto DVL_WM_RBUTTONUP = 0x0205;

constexpr auto DVL_WM_KEYDOWN = 0x0100;
constexpr auto DVL_WM_KEYUP = 0x0101;
constexpr auto DVL_WM_SYSKEYDOWN = 0x0104;

constexpr auto DVL_WM_SYSCOMMAND = 0x0112;

constexpr auto DVL_WM_CHAR = 0x0102;
constexpr auto DVL_WM_CAPTURECHANGED = 0x0215;

constexpr auto DVL_WM_CREATE = 0x0001;
constexpr auto DVL_WM_PAINT = 0x000F;
constexpr auto DVL_WM_CLOSE = 0x0010;
constexpr auto DVL_WM_QUERYENDSESSION = 0x0011;
constexpr auto DVL_WM_ERASEBKGND = 0x0014;
constexpr auto DVL_WM_MOUSEHOVER = 0x02A1;
constexpr auto DVL_WM_QUERYNEWPALETTE = 0x030F;
constexpr auto DVL_WM_PALETTECHANGED = 0x0311;

constexpr auto DVL_SC_CLOSE = 0xF060;

// Virtual key codes.
//
// ref: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
constexpr auto DVL_VK_BACK = 0x08;     // BACKSPACE key
constexpr auto DVL_VK_TAB = 0x09;      // TAB key
constexpr auto DVL_VK_RETURN = 0x0D;   // ENTER key
constexpr auto DVL_VK_SHIFT = 0x10;    // SHIFT key
constexpr auto DVL_VK_CONTROL = 0x11;  // CONTROL key
constexpr auto DVL_VK_MENU = 0x12;     // ALT key
constexpr auto DVL_VK_PAUSE = 0x13;    // PAUSE key
constexpr auto DVL_VK_CAPITAL = 0x14;  // CAPS LOCK key
constexpr auto DVL_VK_ESCAPE = 0x1B;   // ESC key
constexpr auto DVL_VK_SPACE = 0x20;    // SPACEBAR
constexpr auto DVL_VK_PRIOR = 0x21;    // PAGE UP key
constexpr auto DVL_VK_NEXT = 0x22;     // PAGE DOWN key
constexpr auto DVL_VK_END = 0x23;      // END key
constexpr auto DVL_VK_HOME = 0x24;     // HOME key
constexpr auto DVL_VK_LEFT = 0x25;     // LEFT ARROW key
constexpr auto DVL_VK_UP = 0x26;       // UP ARROW key
constexpr auto DVL_VK_RIGHT = 0x27;    // RIGHT ARROW key
constexpr auto DVL_VK_DOWN = 0x28;     // DOWN ARROW key
constexpr auto DVL_VK_SNAPSHOT = 0x2C; // PRINT SCREEN key
constexpr auto DVL_VK_INSERT = 0x2D;   // INS key
constexpr auto DVL_VK_DELETE = 0x2E;   // DEL key
// DVL_VK_0 through DVL_VK_9 correspond to '0' - '9'
// DVL_VK_A through DVL_VK_Z correspond to 'A' - 'Z'
constexpr auto DVL_VK_LWIN = 0x5B;       // Left Windows key (Natural keyboard)
constexpr auto DVL_VK_RWIN = 0x5C;       // Right Windows key (Natural keyboard)
constexpr auto DVL_VK_NUMPAD0 = 0x60;    // Numeric keypad 0 key
constexpr auto DVL_VK_NUMPAD1 = 0x61;    // Numeric keypad 1 key
constexpr auto DVL_VK_NUMPAD2 = 0x62;    // Numeric keypad 2 key
constexpr auto DVL_VK_NUMPAD3 = 0x63;    // Numeric keypad 3 key
constexpr auto DVL_VK_NUMPAD4 = 0x64;    // Numeric keypad 4 key
constexpr auto DVL_VK_NUMPAD5 = 0x65;    // Numeric keypad 5 key
constexpr auto DVL_VK_NUMPAD6 = 0x66;    // Numeric keypad 6 key
constexpr auto DVL_VK_NUMPAD7 = 0x67;    // Numeric keypad 7 key
constexpr auto DVL_VK_NUMPAD8 = 0x68;    // Numeric keypad 8 key
constexpr auto DVL_VK_NUMPAD9 = 0x69;    // Numeric keypad 9 key
constexpr auto DVL_VK_MULTIPLY = 0x6A;   // Multiply key
constexpr auto DVL_VK_ADD = 0x6B;        // Add key
constexpr auto DVL_VK_SUBTRACT = 0x6D;   // Subtract key
constexpr auto DVL_VK_DECIMAL = 0x6E;    // Decimal key
constexpr auto DVL_VK_DIVIDE = 0x6F;     // Divide key
constexpr auto DVL_VK_F1 = 0x70;         // F1 key
constexpr auto DVL_VK_F2 = 0x71;         // F2 key
constexpr auto DVL_VK_F3 = 0x72;         // F3 key
constexpr auto DVL_VK_F4 = 0x73;         // F4 key
constexpr auto DVL_VK_F5 = 0x74;         // F5 key
constexpr auto DVL_VK_F6 = 0x75;         // F6 key
constexpr auto DVL_VK_F7 = 0x76;         // F7 key
constexpr auto DVL_VK_F8 = 0x77;         // F8 key
constexpr auto DVL_VK_F9 = 0x78;         // F9 key
constexpr auto DVL_VK_F10 = 0x79;        // F10 key
constexpr auto DVL_VK_F11 = 0x7A;        // F11 key
constexpr auto DVL_VK_F12 = 0x7B;        // F12 key
constexpr auto DVL_VK_NUMLOCK = 0x90;    // NUM LOCK key
constexpr auto DVL_VK_SCROLL = 0x91;     // SCROLL LOCK key
constexpr auto DVL_VK_LSHIFT = 0xA0;     // Left SHIFT key
constexpr auto DVL_VK_RSHIFT = 0xA1;     // Right SHIFT key
constexpr auto DVL_VK_LCONTROL = 0xA2;   // Left CONTROL key
constexpr auto DVL_VK_RCONTROL = 0xA3;   // Right CONTROL key
constexpr auto DVL_VK_LMENU = 0xA4;      // Left MENU key
constexpr auto DVL_VK_RMENU = 0xA5;      // Right MENU key
constexpr auto DVL_VK_OEM_1 = 0xBA;      // For the US standard keyboard, the ';:' key
constexpr auto DVL_VK_OEM_PLUS = 0xBB;   // For any country/region, the '+' key
constexpr auto DVL_VK_OEM_COMMA = 0xBC;  // For any country/region, the ',' key
constexpr auto DVL_VK_OEM_MINUS = 0xBD;  // For any country/region, the '-' key
constexpr auto DVL_VK_OEM_PERIOD = 0xBE; // For any country/region, the '.' key
constexpr auto DVL_VK_OEM_2 = 0xBF;      // For the US standard keyboard, the '/?' key
constexpr auto DVL_VK_OEM_3 = 0xC0;      // For the US standard keyboard, the '`~' key
constexpr auto DVL_VK_OEM_4 = 0xDB;      // For the US standard keyboard, the '[{' key
constexpr auto DVL_VK_OEM_5 = 0xDC;      // For the US standard keyboard, the '\|' key
constexpr auto DVL_VK_OEM_6 = 0xDD;      // For the US standard keyboard, the ']}' key
constexpr auto DVL_VK_OEM_7 = 0xDE;      // For the US standard keyboard, the 'single-quote/double-quote' key

constexpr auto DVL_MK_SHIFT = 0x0004;
constexpr auto DVL_MK_LBUTTON = 0x0001;
constexpr auto DVL_MK_RBUTTON = 0x0002;

} // namespace dvl
