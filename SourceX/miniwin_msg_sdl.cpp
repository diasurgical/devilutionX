#include "pch.h"
/** @file
 * *
 * Windows message handling and keyboard event conversion for SDL.
 */

static std::deque<MSG> message_queue;

static int translate_sdl_key(SDL_Keysym key)
{
	int sym = key.sym;
	switch (sym) {
	case SDLK_ESCAPE:
		return VK_ESCAPE;
	case SDLK_RETURN:
	case SDLK_KP_ENTER:
		return VK_RETURN;
	case SDLK_TAB:
		return VK_TAB;
	case SDLK_SPACE:
		return VK_SPACE;
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
		return VK_SHIFT;
	case SDLK_PRINTSCREEN:
		return VK_SNAPSHOT;

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

static WPARAM keystate_for_mouse(WPARAM ret)
{
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	ret |= keystate[SDL_SCANCODE_LSHIFT] ? MK_SHIFT : 0;
	ret |= keystate[SDL_SCANCODE_RSHIFT] ? MK_SHIFT : 0;
	// XXX: other MK_* codes not implemented
	return ret;
}

static WINBOOL false_avail()
{
	DUMMY_PRINT("return FALSE although event avaliable");
	return FALSE;
}

WINBOOL WINAPI PeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
	if (wMsgFilterMin != 0)
		UNIMPLEMENTED();
	if (wMsgFilterMax != 0)
		UNIMPLEMENTED();
	if (hWnd != NULL)
		UNIMPLEMENTED();

	if (wRemoveMsg == PM_NOREMOVE) {
		// This does not actually fill out lpMsg, but this is ok
		// since the engine never uses it in this case
		return !message_queue.empty() || SDL_PollEvent(NULL);
	}
	if (wRemoveMsg != PM_REMOVE) {
		UNIMPLEMENTED();
	}

	if (!message_queue.empty()) {
		*lpMsg = message_queue.front();
		message_queue.pop_front();
		return TRUE;
	}

	SDL_Event e;
	if (!SDL_PollEvent(&e)) {
		return FALSE;
	}

	lpMsg->hwnd = hWnd;
	lpMsg->lParam = 0;
	lpMsg->wParam = 0;

	switch (e.type) {
	case SDL_QUIT:
		lpMsg->message = WM_QUIT;
		break;
	case SDL_KEYDOWN:
	case SDL_KEYUP: {
		int key = translate_sdl_key(e.key.keysym);
		if (key == -1)
			return false_avail();
		lpMsg->message = e.type == SDL_KEYDOWN ? WM_KEYDOWN : WM_KEYUP;
		lpMsg->wParam = (DWORD)key;
		// HACK: Encode modifier in lParam for TranslateMessage later
		lpMsg->lParam = e.key.keysym.mod << 16;
	} break;
	case SDL_MOUSEMOTION:
		lpMsg->message = WM_MOUSEMOVE;
		lpMsg->lParam = (e.motion.y << 16) | (e.motion.x & 0xFFFF);
		lpMsg->wParam = keystate_for_mouse(0);
		break;
	case SDL_MOUSEBUTTONDOWN: {
		int button = e.button.button;
		if (button == SDL_BUTTON_LEFT) {
			lpMsg->message = WM_LBUTTONDOWN;
			lpMsg->lParam = (e.button.y << 16) | (e.button.x & 0xFFFF);
			lpMsg->wParam = keystate_for_mouse(MK_LBUTTON);
		} else if (button == SDL_BUTTON_RIGHT) {
			lpMsg->message = WM_RBUTTONDOWN;
			lpMsg->lParam = (e.button.y << 16) | (e.button.x & 0xFFFF);
			lpMsg->wParam = keystate_for_mouse(MK_RBUTTON);
		} else {
			return false_avail();
		}
	} break;
	case SDL_MOUSEBUTTONUP: {
		int button = e.button.button;
		if (button == SDL_BUTTON_LEFT) {
			lpMsg->message = WM_LBUTTONUP;
			lpMsg->lParam = (e.button.y << 16) | (e.button.x & 0xFFFF);
			lpMsg->wParam = keystate_for_mouse(0);
		} else if (button == SDL_BUTTON_RIGHT) {
			lpMsg->message = WM_RBUTTONUP;
			lpMsg->lParam = (e.button.y << 16) | (e.button.x & 0xFFFF);
			lpMsg->wParam = keystate_for_mouse(0);
		} else {
			return false_avail();
		}
	} break;
	case SDL_TEXTINPUT:
	case SDL_WINDOWEVENT:
		if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
			lpMsg->message = WM_QUERYENDSESSION;
		} else {
			return false_avail();
		}
		break;
	default:
		DUMMY_PRINT("unknown SDL message 0x%X", e.type);
		return false_avail();
	}
	return TRUE;
}

WINBOOL WINAPI TranslateMessage(CONST MSG *lpMsg)
{
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
