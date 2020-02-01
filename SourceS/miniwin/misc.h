#pragma once

namespace dvl {

constexpr auto DVL_MAX_PATH = 260;

typedef uint16_t SHORT;
typedef int32_t LONG;
typedef uint8_t BOOLEAN;

typedef unsigned char UCHAR;

typedef uint32_t DWORD;
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef DWORD *LPDWORD;
typedef void *LPVOID;
typedef void *PVOID;

typedef unsigned int UINT;

typedef uintptr_t DWORD_PTR, *PDWORD_PTR;

typedef uintptr_t WPARAM;
typedef uintptr_t LPARAM;
typedef uintptr_t LRESULT;

//
// Handles
//
typedef void *HANDLE;

typedef HANDLE HWND, HMODULE, HDC, HINSTANCE;

typedef LRESULT(*WNDPROC)(HWND, UINT, WPARAM, LPARAM);

typedef struct _FILETIME {
	DWORD dwLowDateTime;
	DWORD dwHighDateTime;
} FILETIME, *LPFILETIME;

typedef struct tagMSG {
	UINT message;
	WPARAM wParam;
	LPARAM lParam;
} MSG, *LPMSG;

//
// Everything else
//

HANDLE CreateEventA();
BOOL CloseEvent(HANDLE event);
void SetEvent(HANDLE hEvent);
void ResetEvent(HANDLE hEvent);
int WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);

void SetCursorPos(int X, int Y);
void FocusOnCharInfo();

SHORT GetAsyncKeyState(int vKey);

bool PeekMessageA(LPMSG lpMsg);

bool TranslateMessage(const MSG *lpMsg);
LRESULT DispatchMessageA(const MSG *lpMsg);
bool PostMessageA(UINT Msg, WPARAM wParam, LPARAM lParam);

uintptr_t DVL_beginthreadex(void *_Security, unsigned _StackSize, unsigned(*_StartAddress)(void *),
    void *_ArgList, unsigned _InitFlag, unsigned *_ThrdAddr);
DWORD GetCurrentThreadId();
bool SetThreadPriority(HANDLE hThread, int nPriority);

//
// MSCVRT emulation
//

constexpr auto DVL_FILE_CURRENT = 1;

constexpr auto DVL_WM_QUIT = 0x0012;
constexpr auto DVL_INFINITE = 0xFFFFFFFF;

//
// Events
//
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

constexpr auto DVL_WM_PAINT = 0x000F;
constexpr auto DVL_WM_CLOSE = 0x0010;
constexpr auto DVL_WM_QUERYENDSESSION = 0x0011;
constexpr auto DVL_WM_ERASEBKGND = 0x0014;
constexpr auto DVL_WM_QUERYNEWPALETTE = 0x030F;

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
