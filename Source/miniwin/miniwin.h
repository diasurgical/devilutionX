#pragma once

#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "engine/point.hpp"

namespace devilution {

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

typedef uint32_t DWORD;
typedef unsigned char BYTE;

//
// Handles
//
typedef void *HANDLE;

typedef void (*WNDPROC)(uint32_t, int32_t, int32_t);

struct tagMSG {
	uint32_t message;
	int32_t wParam;
	int32_t lParam;
};

//
// Everything else
//

void SetCursorPos(Point position);
void FocusOnCharInfo();

bool GetAsyncKeyState(int vKey);

bool FetchMessage(tagMSG *lpMsg);

bool TranslateMessage(const tagMSG *lpMsg);
void PushMessage(const tagMSG *lpMsg);
bool PostMessage(uint32_t type, int32_t wParam, int32_t lParam);
void ClearMessageQueue();

#ifdef _MSC_VER
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

//
// MSCVRT emulation
//

#define DVL_FILE_BEGIN 0
#define DVL_FILE_CURRENT 1
#define DVL_FILE_END 2

#define DVL_WM_QUIT 0x0012

//
// Events
//
#define DVL_WM_MOUSEMOVE 0x0200
#define DVL_WM_LBUTTONDOWN 0x0201
#define DVL_WM_LBUTTONUP 0x0202
#define DVL_WM_RBUTTONDOWN 0x0204
#define DVL_WM_RBUTTONUP 0x0205

#define DVL_WM_KEYDOWN 0x0100
#define DVL_WM_KEYUP 0x0101
#define DVL_WM_SYSKEYDOWN 0x0104

#define DVL_WM_SYSCOMMAND 0x0112

#define DVL_WM_CHAR 0x0102
#define DVL_WM_CAPTURECHANGED 0x0215

#define DVL_WM_PAINT 0x000F
#define DVL_WM_QUERYENDSESSION 0x0011

#define DVL_SC_CLOSE 0xF060

// Virtual key codes.
//
// ref: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
#define DVL_VK_INVALID 0     // Invalid key
#define DVL_VK_BACK 0x08     // BACKSPACE key
#define DVL_VK_TAB 0x09      // TAB key
#define DVL_VK_RETURN 0x0D   // ENTER key
#define DVL_VK_SHIFT 0x10    // SHIFT key
#define DVL_VK_CONTROL 0x11  // CONTROL key
#define DVL_VK_MENU 0x12     // ALT key
#define DVL_VK_PAUSE 0x13    // PAUSE key
#define DVL_VK_CAPITAL 0x14  // CAPS LOCK key
#define DVL_VK_ESCAPE 0x1B   // ESC key
#define DVL_VK_SPACE 0x20    // SPACEBAR
#define DVL_VK_PRIOR 0x21    // PAGE UP key
#define DVL_VK_NEXT 0x22     // PAGE DOWN key
#define DVL_VK_END 0x23      // END key
#define DVL_VK_HOME 0x24     // HOME key
#define DVL_VK_LEFT 0x25     // LEFT ARROW key
#define DVL_VK_UP 0x26       // UP ARROW key
#define DVL_VK_RIGHT 0x27    // RIGHT ARROW key
#define DVL_VK_DOWN 0x28     // DOWN ARROW key
#define DVL_VK_SNAPSHOT 0x2C // PRINT SCREEN key
#define DVL_VK_INSERT 0x2D   // INS key
#define DVL_VK_DELETE 0x2E   // DEL key
// DVL_VK_0 through DVL_VK_9 correspond to '0' - '9'
// DVL_VK_A through DVL_VK_Z correspond to 'A' - 'Z'
#define DVL_VK_LWIN 0x5B       // Left Windows key (Natural keyboard)
#define DVL_VK_RWIN 0x5C       // Right Windows key (Natural keyboard)
#define DVL_VK_NUMPAD0 '0'     // Numeric keypad 0 key
#define DVL_VK_NUMPAD1 '1'     // Numeric keypad 1 key
#define DVL_VK_NUMPAD2 '2'     // Numeric keypad 2 key
#define DVL_VK_NUMPAD3 '3'     // Numeric keypad 3 key
#define DVL_VK_NUMPAD4 '4'     // Numeric keypad 4 key
#define DVL_VK_NUMPAD5 '5'     // Numeric keypad 5 key
#define DVL_VK_NUMPAD6 '6'     // Numeric keypad 6 key
#define DVL_VK_NUMPAD7 '7'     // Numeric keypad 7 key
#define DVL_VK_NUMPAD8 '8'     // Numeric keypad 8 key
#define DVL_VK_NUMPAD9 '9'     // Numeric keypad 9 key
#define DVL_VK_MULTIPLY 0x6A   // Multiply key
#define DVL_VK_ADD 0x6B        // Add key
#define DVL_VK_SUBTRACT 0x6D   // Subtract key
#define DVL_VK_DECIMAL 0x6E    // Decimal key
#define DVL_VK_DIVIDE 0x6F     // Divide key
#define DVL_VK_F1 0x70         // F1 key
#define DVL_VK_F2 0x71         // F2 key
#define DVL_VK_F3 0x72         // F3 key
#define DVL_VK_F4 0x73         // F4 key
#define DVL_VK_F5 0x74         // F5 key
#define DVL_VK_F6 0x75         // F6 key
#define DVL_VK_F7 0x76         // F7 key
#define DVL_VK_F8 0x77         // F8 key
#define DVL_VK_F9 0x78         // F9 key
#define DVL_VK_F10 0x79        // F10 key
#define DVL_VK_F11 0x7A        // F11 key
#define DVL_VK_F12 0x7B        // F12 key
#define DVL_VK_NUMLOCK 0x90    // NUM LOCK key
#define DVL_VK_SCROLL 0x91     // SCROLL LOCK key
#define DVL_VK_LSHIFT 0xA0     // Left SHIFT key
#define DVL_VK_RSHIFT 0xA1     // Right SHIFT key
#define DVL_VK_LCONTROL 0xA2   // Left CONTROL key
#define DVL_VK_RCONTROL 0xA3   // Right CONTROL key
#define DVL_VK_LMENU 0xA4      // Left MENU key
#define DVL_VK_RMENU 0xA5      // Right MENU key
#define DVL_VK_OEM_1 0xBA      // For the US standard keyboard, the ':' key
#define DVL_VK_OEM_PLUS 0xBB   // For any country/region, the '+' key
#define DVL_VK_OEM_COMMA 0xBC  // For any country/region, the ',' key
#define DVL_VK_OEM_MINUS 0xBD  // For any country/region, the '-' key
#define DVL_VK_OEM_PERIOD 0xBE // For any country/region, the '.' key
#define DVL_VK_OEM_2 0xBF      // For the US standard keyboard, the '/?' key
#define DVL_VK_OEM_3 0xC0      // For the US standard keyboard, the '`~' key
#define DVL_VK_OEM_4 0xDB      // For the US standard keyboard, the '[{' key
#define DVL_VK_OEM_5 0xDC      // For the US standard keyboard, the '\|' key
#define DVL_VK_OEM_6 0xDD      // For the US standard keyboard, the ']}' key
#define DVL_VK_OEM_7 0xDE      // For the US standard keyboard, the 'single-quote/double-quote' key

#define DVL_MK_CTRL 0x0008
#define DVL_MK_SHIFT 0x0004
#define DVL_MK_LBUTTON 0x0001
#define DVL_MK_RBUTTON 0x0002

} // namespace devilution
