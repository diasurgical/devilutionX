/** @file
 * *
 * Windows message handling and keyboard event conversion for SDL.
 */
#include <deque>

#include "miniwin_sdl.h"

#include "../types.h"
#include "stubs.h"

static std::deque<MSG> message_queue;

static int translate_sdl_key(SDL_Keysym key)
{
	int sym = key.sym;
	switch (sym) {
	case SDLK_ESCAPE:
		return VK_ESCAPE;
	case SDLK_RETURN:
		return VK_RETURN;
	case SDLK_TAB:
		return VK_TAB;
	case SDLK_BACKSPACE:
		return VK_BACK;

	case SDLK_DOWN:
		return VK_DOWN;
	case SDLK_LEFT:
		return VK_LEFT;
	case SDLK_RIGHT:
		return VK_RIGHT;
	case SDLK_UP:
		return VK_UP;

	case SDLK_PAGEUP:
		return VK_PRIOR;
	case SDLK_PAGEDOWN:
		return VK_NEXT;

	case SDLK_PAUSE:
		return VK_PAUSE;

	case SDLK_SEMICOLON:
		return VK_OEM_1;
	case SDLK_QUESTION:
		return VK_OEM_2;
	case SDLK_BACKQUOTE:
		return VK_OEM_3;
	case SDLK_LEFTBRACKET:
		return VK_OEM_4;
	case SDLK_BACKSLASH:
		return VK_OEM_5;
	case SDLK_RIGHTBRACKET:
		return VK_OEM_6;
	case SDLK_QUOTE:
		return VK_OEM_7;
	case SDLK_MINUS:
		return VK_OEM_MINUS;
	case SDLK_PLUS:
		return VK_OEM_PLUS;
	case SDLK_PERIOD:
		return VK_OEM_PERIOD;
	case SDLK_COMMA:
		return VK_OEM_COMMA;

	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		// Not handled yet
		return -1;

	default:
		if (sym >= SDLK_a && sym <= SDLK_z) {
			return 'A' + (sym - SDLK_a);
		} else if (sym >= SDLK_0 && sym <= SDLK_9) {
			return '0' + (sym - SDLK_0);
		} else if (sym >= SDLK_F1 && sym <= SDLK_F12) {
			return VK_F1 + (sym - SDLK_F1);
		}

		DUMMY_PRINT("unknown key: name=%s sym=0x%X scan=%d mod=0x%X", SDL_GetKeyName(sym), sym, key.scancode, key.mod);
		return -1;
	}
}

WINBOOL WINAPI PeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
	// DUMMY_PRINT("hwnd: %d", hWnd);
	assert(wMsgFilterMin == 0 && wMsgFilterMax == 0);
	if (wRemoveMsg == PM_NOREMOVE) {
		// XXX: This does not actually fill out lpMsg properly
		lpMsg->hwnd = (HWND)-1;
		lpMsg->message = 0;

		return !message_queue.empty() || SDL_PollEvent(NULL);
	} else if (wRemoveMsg == PM_REMOVE) {
		if (!message_queue.empty()) {
			*lpMsg = message_queue.front();
			message_queue.pop_front();
			return TRUE;
		}

		SDL_Event e;
		int pending = SDL_PollEvent(&e);
		if (!pending) {
			return FALSE;
		}

		lpMsg->hwnd = hWnd;
		lpMsg->lParam = 0;
		lpMsg->wParam = 0;

		switch (e.type) {
		case SDL_QUIT: {
			lpMsg->message = WM_QUIT;
			break;
		}

		case SDL_KEYDOWN:
		case SDL_KEYUP: {
			int key = translate_sdl_key(e.key.keysym);
			if (key == -1) {
				return FALSE;
			}

			lpMsg->message = e.type == SDL_KEYDOWN ? WM_KEYFIRST : WM_KEYUP;
			lpMsg->wParam = (DWORD)key;
			// Hack: Encode modifier in lParam for TranslateMessage later
			lpMsg->lParam = e.key.keysym.mod << 16;
			break;
		}

		case SDL_MOUSEMOTION: {
			lpMsg->message = WM_MOUSEMOVE;
			lpMsg->lParam = (e.motion.y << 16) | (e.motion.x & 0xFFFF);
			break;
		}

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP: {
			int button = e.button.button;
			if (button != SDL_BUTTON_LEFT && button != SDL_BUTTON_RIGHT) {
				return FALSE;
			}
			bool left = button == SDL_BUTTON_LEFT;
			lpMsg->message = e.type == SDL_MOUSEBUTTONDOWN ? (left ? WM_LBUTTONDOWN : WM_RBUTTONDOWN)
			                                               : (left ? WM_LBUTTONUP : WM_RBUTTONUP);
			lpMsg->lParam = (e.button.y << 16) | (e.button.x & 0xFFFF);
			break;
		}

		case SDL_TEXTINPUT:
		case SDL_WINDOWEVENT: {
			return FALSE;
		}

		default: {
			DUMMY_PRINT("unknown SDL message 0x%X", e.type);
			return FALSE;
		}
		}

		return TRUE;
	}
	UNIMPLEMENTED();
}

WINBOOL WINAPI TranslateMessage(CONST MSG *lpMsg)
{
	DUMMY_ONCE();
	assert(lpMsg->hwnd == 0);

	if (lpMsg->message == WM_KEYDOWN) {
		int key = lpMsg->wParam;
		unsigned mod = (DWORD)lpMsg->lParam >> 16;

		bool shift = (mod & KMOD_SHIFT) != 0;
		bool upper = shift != (mod & KMOD_CAPS);

		bool is_alpha = (key >= 'A' && key <= 'Z');
		bool is_numeric = (key >= '0' && key <= '9');
		bool is_control = key == VK_SPACE || key == VK_BACK || key == VK_ESCAPE || key == VK_TAB || key == VK_RETURN;
		bool is_oem = (key >= VK_OEM_1 && key <= VK_OEM_7);

		if (is_control || is_alpha || is_numeric || is_oem) {
			if (!upper && is_alpha) {
				key = tolower(key);
			} else if (shift && is_numeric) {
				key = key == '0' ? ')' : key - 0x10;
			} else if (is_oem) {
				// XXX: This probably only supports US keyboard layout
				switch (key) {
				case VK_OEM_1:
					key = shift ? ':' : ';';
					break;
				case VK_OEM_2:
					key = shift ? '?' : '/';
					break;
				case VK_OEM_3:
					key = shift ? '~' : '`';
					break;
				case VK_OEM_4:
					key = shift ? '{' : '[';
					break;
				case VK_OEM_5:
					key = shift ? '|' : '\\';
					break;
				case VK_OEM_6:
					key = shift ? '}' : ']';
					break;
				case VK_OEM_7:
					key = shift ? '"' : '\'';
					break;

				case VK_OEM_MINUS:
					key = shift ? '_' : '-';
					break;
				case VK_OEM_PLUS:
					key = shift ? '+' : '=';
					break;
				case VK_OEM_PERIOD:
					key = shift ? '>' : '.';
					break;
				case VK_OEM_COMMA:
					key = shift ? '<' : ',';
					break;

				default:
					UNIMPLEMENTED();
				}
			}

			if (key >= 32) {
				DUMMY_PRINT("char: %c", key);
			}

			// XXX: This does not add extended info to lParam
			PostMessageA(lpMsg->hwnd, WM_CHAR, key, 0);
		}
	}

	return TRUE;
}

SHORT WINAPI GetAsyncKeyState(int vKey)
{
	DUMMY_ONCE();
	// TODO: Not handled yet.
	return 0;
}

LRESULT WINAPI DispatchMessageA(CONST MSG *lpMsg)
{
	DUMMY_ONCE();
	assert(lpMsg->hwnd == 0);
	assert(CurrentProc);
	// assert(CurrentProc == GM_Game);

	return CurrentProc(lpMsg->hwnd, lpMsg->message, lpMsg->wParam, lpMsg->lParam);
}

WINBOOL WINAPI PostMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DUMMY();

	assert(hWnd == 0);

	MSG msg;
	msg.hwnd = hWnd;
	msg.message = Msg;
	msg.wParam = wParam;
	msg.lParam = lParam;

	message_queue.push_back(msg);

	return TRUE;
}
