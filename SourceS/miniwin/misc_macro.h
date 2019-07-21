#pragma once

#define TRUE true
#define FALSE false

#define INVALID_HANDLE_VALUE ((HANDLE)-1)
#define INVALID_HANDLE ((HANDLE)-1)
#define HFILE_ERROR DVL_HFILE_ERROR

//
// Intrinsics
//
#define LOBYTE(w) ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w) ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))
#define LOWORD(l) ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l) ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))

#ifdef _MSC_VER
#define InterlockedIncrement(x) (x)
#else
#define InterlockedIncrement(x) __sync_add_and_fetch(x, 1)
#endif

#define INFINITE DVL_INFINITE

#ifndef __BIG_ENDIAN__
#define MAKEFOURCC(x, y, z, w)             \
	(((uint32_t)((uint8_t)x))              \
	    | (((uint32_t)((uint8_t)y)) << 8)  \
	    | (((uint32_t)((uint8_t)z)) << 16) \
	    | (((uint32_t)((uint8_t)w)) << 24))
#else
#define MAKEFOURCC(w, z, y, x)             \
	(((uint32_t)((uint8_t)x))              \
	    | (((uint32_t)((uint8_t)y)) << 8)  \
	    | (((uint32_t)((uint8_t)z)) << 16) \
	    | (((uint32_t)((uint8_t)w)) << 24))
#endif

#define WINUSERAPI

#define FOURCC_RIFF MAKEFOURCC('R', 'I', 'F', 'F')

#define ERROR_ALREADY_EXISTS 183

#define CreateEvent CreateEventA

#define PM_NOREMOVE DVL_PM_NOREMOVE
#define PM_REMOVE DVL_PM_REMOVE
#define WM_QUIT DVL_WM_QUIT

#define PeekMessage PeekMessageA
#define DispatchMessage DispatchMessageA
#define PostMessage PostMessageA
#define CreateWindowEx CreateWindowExA
#define FindWindow FindWindowA
#define RegisterClassEx RegisterClassExA
#define LoadCursor LoadCursorA
#define GetUserName GetUserNameA
#define LoadIcon LoadIconA
#define LoadImage LoadImageA
#define SHGetPathFromIDList SHGetPathFromIDListA
#define ShellExecute ShellExecuteA
#define GetModuleHandle GetModuleHandleA

#define THREAD_BASE_PRIORITY_MAX 2
#define THREAD_PRIORITY_NORMAL 0
#define THREAD_PRIORITY_HIGHEST THREAD_BASE_PRIORITY_MAX
#define THREAD_PRIORITY_ABOVE_NORMAL (THREAD_PRIORITY_HIGHEST - 1)

#define TextOut TextOutA

#define HORZRES DVL_HORZRES
#define VERTRES DVL_VERTRES
#define NUMRESERVED 106

#define _snprintf snprintf
#define _vsnprintf vsnprintf
#define wsprintf wsprintfA
#define wvsprintf wvsprintfA

//
// File I/O
//

#define FILE_BEGIN DVL_FILE_BEGIN
#define FILE_CURRENT DVL_FILE_CURRENT
#define FILE_END 2
#define FILE_FLAG_WRITE_THROUGH 0x80000000
#define CREATE_ALWAYS DVL_CREATE_ALWAYS
#define GENERIC_READ DVL_GENERIC_READ
#define GENERIC_WRITE DVL_GENERIC_WRITE
#define OPEN_EXISTING DVL_OPEN_EXISTING
#define OPEN_ALWAYS 4
#define ERROR_FILE_NOT_FOUND DVL_ERROR_FILE_NOT_FOUND
#define FILE_ATTRIBUTE_NORMAL 128
#define FILE_ATTRIBUTE_HIDDEN 0x00000002
#define FILE_ATTRIBUTE_SYSTEM 0x00000004
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#define FILE_SHARE_READ 1

#define OFS_MAXPATHNAME DVL_OFS_MAXPATHNAME

//
// Calculate the byte offset of a field in a structure of type type.
//

#define FIELD_OFFSET(type, field) ((LONG)(INT_PTR) & (((type *)0)->field))
#define IMAGE_FIRST_SECTION(ntheader) ((PIMAGE_SECTION_HEADER)((UINT_PTR)ntheader + FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader) + ((PIMAGE_NT_HEADERS)(ntheader))->FileHeader.SizeOfOptionalHeader))

#define IMAGE_NT_SIGNATURE 0x00004550 // PE00
#define IMAGE_DOS_SIGNATURE 0x5A4D
#define VER_PLATFORM_WIN32_NT DVL_VER_PLATFORM_WIN32_NT

#define GetVersionEx GetVersionExA

#define lstrcpyn lstrcpynA

#define MEM_COMMIT 0x1000
#define MEM_RELEASE 0x8000

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

#define CreateProcess CreateProcessA
#define CreateFileMapping CreateFileMappingA
#define GetPrivateProfileString GetPrivateProfileStringA
#define MessageBox MessageBoxA

#define HKEY_CURRENT_USER 1
#define KEY_READ 0x20019
#define KEY_WRITE 0x20006
#define REG_SZ 1

#define RegOpenKeyEx RegOpenKeyExA
#define RegQueryValueEx RegQueryValueExA
#define RegSetValueEx RegSetValueExA
#define RegCloseKey RegCloseKeyA
#define DefWindowProc DefWindowProcA
#define GetWindowLong GetWindowLongA
#define SetWindowLong SetWindowLongA

#define GetFileAttributes GetFileAttributesA
#define SetFileAttributes SetFileAttributesA
#define FindFirstFile FindFirstFileA
#define FindNextFile FindNextFileA
#define CreateFile CreateFileA
#define GetWindowsDirectory GetWindowsDirectoryA
#define GetLogicalDriveStrings GetLogicalDriveStringsA
#define GetDriveType GetDriveTypeA
#define GetDiskFreeSpace GetDiskFreeSpaceA
#define GetModuleFileName GetModuleFileNameA
#define GetComputerName GetComputerNameA
#define GetFileVersionInfoSize GetFileVersionInfoSizeA
#define GetFileVersionInfo GetFileVersionInfoA
#define VerQueryValue VerQueryValueA
#define DeleteFile DeleteFileA
#define CopyFile CopyFileA

#define GWL_STYLE (-16)

#define WS_POPUP 0x80000000L
#define WS_SYSMENU 0x00080000L

#define DRIVE_CDROM DVL_DRIVE_CDROM

//
// Events
//

// were here

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

#define OF_EXIST 1

#define MAKEINTRESOURCE DVL_MAKEINTRESOURCE

#define _finddata_t DVL_finddata_t
#define _beginthreadex DVL_beginthreadex

//
// Events
//
#define WM_MOUSEFIRST DVL_WM_MOUSEFIRST
#define WM_MOUSEMOVE DVL_WM_MOUSEMOVE
#define WM_LBUTTONDOWN DVL_WM_LBUTTONDOWN
#define WM_LBUTTONUP DVL_WM_LBUTTONUP
#define WM_RBUTTONDOWN DVL_WM_RBUTTONDOWN
#define WM_RBUTTONUP DVL_WM_RBUTTONUP

#define WM_KEYFIRST DVL_WM_KEYFIRST
#define WM_KEYDOWN DVL_WM_KEYDOWN
#define WM_KEYUP DVL_WM_KEYUP
#define WM_SYSKEYDOWN DVL_WM_SYSKEYDOWN

#define WM_INITDIALOG DVL_WM_INITDIALOG
#define WM_COMMAND DVL_WM_COMMAND
#define WM_SYSCOMMAND DVL_WM_SYSCOMMAND

#define WM_CHAR DVL_WM_CHAR
#define WM_CAPTURECHANGED DVL_WM_CAPTURECHANGED

#define WM_CREATE DVL_WM_CREATE
#define WM_DESTROY DVL_WM_DESTROY
#define WM_PAINT DVL_WM_PAINT
#define WM_CLOSE DVL_WM_CLOSE
#define WM_QUERYENDSESSION DVL_WM_QUERYENDSESSION
#define WM_ERASEBKGND DVL_WM_ERASEBKGND
#define WM_ACTIVATEAPP DVL_WM_ACTIVATEAPP
#define WM_SYSKEYUP DVL_WM_SYSKEYUP
#define WM_QUERYNEWPALETTE DVL_WM_QUERYNEWPALETTE
#define WM_PALETTECHANGED DVL_WM_PALETTECHANGED

#define SC_CLOSE DVL_SC_CLOSE

// Virtual key codes.
//
// ref: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
#define VK_BACK DVL_VK_BACK             // BACKSPACE key
#define VK_TAB DVL_VK_TAB               // TAB key
#define VK_RETURN DVL_VK_RETURN         // ENTER key
#define VK_SHIFT DVL_VK_SHIFT           // SHIFT key
#define VK_CONTROL DVL_VK_CONTROL       // CONTROL key
#define VK_MENU DVL_VK_MENU             // ALT key
#define VK_PAUSE DVL_VK_PAUSE           // PAUSE key
#define VK_CAPITAL DVL_VK_CAPITAL       // CAPS LOCK key
#define VK_ESCAPE DVL_VK_ESCAPE         // ESC key
#define VK_SPACE DVL_VK_SPACE           // SPACEBAR
#define VK_PRIOR DVL_VK_PRIOR           // PAGE UP key
#define VK_NEXT DVL_VK_NEXT             // PAGE DOWN key
#define VK_END DVL_VK_END               // END key
#define VK_HOME DVL_VK_HOME             // HOME key
#define VK_LEFT DVL_VK_LEFT             // LEFT ARROW key
#define VK_UP DVL_VK_UP                 // UP ARROW key
#define VK_RIGHT DVL_VK_RIGHT           // RIGHT ARROW key
#define VK_DOWN DVL_VK_DOWN             // DOWN ARROW key
#define VK_SNAPSHOT DVL_VK_SNAPSHOT     // PRINT SCREEN key
#define VK_INSERT DVL_VK_INSERT         // INS key
#define VK_DELETE DVL_VK_DELETE         // DEL key
// VK_0 through VK_9 correspond to '0' - '9'
// VK_A through VK_Z correspond to 'A' - 'Z'
#define VK_LWIN DVL_VK_LWIN             // Left Windows key (Natural keyboard)
#define VK_RWIN DVL_VK_RWIN             // Right Windows key (Natural keyboard)
#define VK_NUMPAD0 DVL_VK_NUMPAD0       // Numeric keypad 0 key
#define VK_NUMPAD1 DVL_VK_NUMPAD1       // Numeric keypad 1 key
#define VK_NUMPAD2 DVL_VK_NUMPAD2       // Numeric keypad 2 key
#define VK_NUMPAD3 DVL_VK_NUMPAD3       // Numeric keypad 3 key
#define VK_NUMPAD4 DVL_VK_NUMPAD4       // Numeric keypad 4 key
#define VK_NUMPAD5 DVL_VK_NUMPAD5       // Numeric keypad 5 key
#define VK_NUMPAD6 DVL_VK_NUMPAD6       // Numeric keypad 6 key
#define VK_NUMPAD7 DVL_VK_NUMPAD7       // Numeric keypad 7 key
#define VK_NUMPAD8 DVL_VK_NUMPAD8       // Numeric keypad 8 key
#define VK_NUMPAD9 DVL_VK_NUMPAD9       // Numeric keypad 9 key
#define VK_MULTIPLY DVL_VK_MULTIPLY     // Multiply key
#define VK_ADD DVL_VK_ADD               // Add key
#define VK_SUBTRACT DVL_VK_SUBTRACT     // Subtract key
#define VK_DECIMAL DVL_VK_DECIMAL       // Decimal key
#define VK_DIVIDE DVL_VK_DIVIDE         // Divide key
#define VK_F1 DVL_VK_F1                 // F1 key
#define VK_F2 DVL_VK_F2                 // F2 key
#define VK_F3 DVL_VK_F3                 // F3 key
#define VK_F4 DVL_VK_F4                 // F4 key
#define VK_F5 DVL_VK_F5                 // F5 key
#define VK_F6 DVL_VK_F6                 // F6 key
#define VK_F7 DVL_VK_F7                 // F7 key
#define VK_F8 DVL_VK_F8                 // F8 key
#define VK_F9 DVL_VK_F9                 // F9 key
#define VK_F10 DVL_VK_F10               // F10 key
#define VK_F11 DVL_VK_F11               // F11 key
#define VK_F12 DVL_VK_F12               // F12 key
#define VK_NUMLOCK DVL_VK_NUMLOCK       // NUM LOCK key
#define VK_SCROLL DVL_VK_SCROLL         // SCROLL LOCK key
#define VK_LSHIFT DVL_VK_LSHIFT         // Left SHIFT key
#define VK_RSHIFT DVL_VK_RSHIFT         // Right SHIFT key
#define VK_LCONTROL DVL_VK_LCONTROL     // Left CONTROL key
#define VK_RCONTROL DVL_VK_RCONTROL     // Right CONTROL key
#define VK_LMENU DVL_VK_LMENU           // Left MENU key
#define VK_RMENU DVL_VK_RMENU           // Right MENU key
#define VK_OEM_1 DVL_VK_OEM_1           // For the US standard keyboard, the ';:' key
#define VK_OEM_PLUS DVL_VK_OEM_PLUS     // For any country/region, the '+' key
#define VK_OEM_COMMA DVL_VK_OEM_COMMA   // For any country/region, the ',' key
#define VK_OEM_MINUS DVL_VK_OEM_MINUS   // For any country/region, the '-' key
#define VK_OEM_PERIOD DVL_VK_OEM_PERIOD // For any country/region, the '.' key
#define VK_OEM_2 DVL_VK_OEM_2           // For the US standard keyboard, the '/?' key
#define VK_OEM_3 DVL_VK_OEM_3           // For the US standard keyboard, the '`~' key
#define VK_OEM_4 DVL_VK_OEM_4           // For the US standard keyboard, the '[{' key
#define VK_OEM_5 DVL_VK_OEM_5           // For the US standard keyboard, the '\|' key
#define VK_OEM_6 DVL_VK_OEM_6           // For the US standard keyboard, the ']}' key
#define VK_OEM_7 DVL_VK_OEM_7           // For the US standard keyboard, the 'single-quote/double-quote' key

#define MK_SHIFT DVL_MK_SHIFT
#define MK_LBUTTON DVL_MK_LBUTTON
#define MK_RBUTTON DVL_MK_RBUTTON

#define MB_TASKMODAL DVL_MB_TASKMODAL
#define MB_ICONHAND DVL_MB_ICONHAND
#define MB_ICONEXCLAMATION DVL_MB_ICONEXCLAMATION

/*
 * GetWindow() Constants
 */
#define GW_HWNDPREV         3
