#include <deque>
#include <SDL.h>

#include "devilution.h"
#include "stubs.h"
#include <math.h>
#include "../../touch/touch.h"
#ifdef SWITCH
	#include <switch.h>
#endif

/** @file
 * *
 * Windows message handling and keyboard event conversion for SDL.
 */

namespace dvl {

bool conInv = false;
float leftStickX = 0;
float leftStickY = 0;
float rightStickX = 0;
float rightStickY = 0;
float leftTrigger = 0;
float rightTrigger = 0;
float rightDeadzone = 0.07;
float leftDeadzone = 0.07;
int doAttack 	= 0;
int doUse 		= 0;
int doChar 		= 0;

static int leftStickXUnscaled = 0; // raw axis values reported by SDL_JOYAXISMOTION
static int leftStickYUnscaled = 0;
static int rightStickXUnscaled = 0;
static int rightStickYUnscaled = 0;
static int hiresDX = 0; // keep track of X/Y sub-pixel per frame mouse motion
static int hiresDY = 0;
static int64_t currentTime = 0; // used to update joystick mouse once per frame
static int64_t lastTime = 0;
static void ScaleJoystickAxes(float *x, float *y, float deadzone);
static void HandleJoystickAxes();
#ifdef SWITCH
static BOOL currently_docked = -1; // keep track of docked or handheld mode
static void HandleDocking();
extern SDL_Window *window;
extern SDL_Renderer *renderer;
#endif

static std::deque<MSG> message_queue;

static int translate_sdl_key(SDL_Keysym key)
{
	// ref: https://wiki.libsdl.org/SDL_Keycode
	// ref: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	SDL_Keycode sym = key.sym;
	switch (sym) {
	case SDLK_BACKSPACE:
		return DVL_VK_BACK;
	case SDLK_TAB:
		return DVL_VK_TAB;
	case SDLK_RETURN:
		return DVL_VK_RETURN;
	case SDLK_ESCAPE:
		return DVL_VK_ESCAPE;
	case SDLK_SPACE:
		return DVL_VK_SPACE;
	case SDLK_QUOTE:
		return DVL_VK_OEM_7;
	case SDLK_COMMA:
		return DVL_VK_OEM_COMMA;
	case SDLK_MINUS:
		return DVL_VK_OEM_MINUS;
	case SDLK_PERIOD:
		return DVL_VK_OEM_PERIOD;
	case SDLK_SLASH:
		return DVL_VK_OEM_2;
	case SDLK_SEMICOLON:
		return DVL_VK_OEM_1;
	case SDLK_EQUALS:
		return DVL_VK_OEM_PLUS;
	case SDLK_LEFTBRACKET:
		return DVL_VK_OEM_4;
	case SDLK_BACKSLASH:
		return DVL_VK_OEM_5;
	case SDLK_RIGHTBRACKET:
		return DVL_VK_OEM_6;
	case SDLK_BACKQUOTE:
		return DVL_VK_OEM_3;
	case SDLK_DELETE:
		return DVL_VK_DELETE;
	case SDLK_CAPSLOCK:
		return DVL_VK_CAPITAL;
	case SDLK_F1:
		return DVL_VK_F1;
	case SDLK_F2:
		return DVL_VK_F2;
	case SDLK_F3:
		return DVL_VK_F3;
	case SDLK_F4:
		return DVL_VK_F4;
	case SDLK_F5:
		return DVL_VK_F5;
	case SDLK_F6:
		return DVL_VK_F6;
	case SDLK_F7:
		return DVL_VK_F7;
	case SDLK_F8:
		return DVL_VK_F8;
	case SDLK_F9:
		return DVL_VK_F9;
	case SDLK_F10:
		return DVL_VK_F10;
	case SDLK_F11:
		return DVL_VK_F11;
	case SDLK_F12:
		return DVL_VK_F12;
	case SDLK_PRINTSCREEN:
		return DVL_VK_SNAPSHOT;
	case SDLK_SCROLLLOCK:
		return DVL_VK_SCROLL;
	case SDLK_PAUSE:
		return DVL_VK_PAUSE;
	case SDLK_INSERT:
		return DVL_VK_INSERT;
	case SDLK_HOME:
		return DVL_VK_HOME;
	case SDLK_PAGEUP:
		return DVL_VK_PRIOR;
	case SDLK_END:
		return DVL_VK_END;
	case SDLK_PAGEDOWN:
		return DVL_VK_NEXT;
	case SDLK_RIGHT:
		return DVL_VK_RIGHT;
	case SDLK_LEFT:
		return DVL_VK_LEFT;
	case SDLK_DOWN:
		return DVL_VK_DOWN;
	case SDLK_UP:
		return DVL_VK_UP;
	case SDLK_NUMLOCKCLEAR:
		return DVL_VK_NUMLOCK;
	case SDLK_KP_DIVIDE:
		return DVL_VK_DIVIDE;
	case SDLK_KP_MULTIPLY:
		return DVL_VK_MULTIPLY;
	case SDLK_KP_MINUS:
		// Returning DVL_VK_OEM_MINUS to play nice with Devilution automap zoom.
		//
		// For a distinct keypad key-code, DVL_VK_SUBTRACT should be returned.
		return DVL_VK_OEM_MINUS;
	case SDLK_KP_PLUS:
		// Returning DVL_VK_OEM_PLUS to play nice with Devilution automap zoom.
		//
		// For a distinct keypad key-code, DVL_VK_ADD should be returned.
		return DVL_VK_OEM_PLUS;
	case SDLK_KP_ENTER:
		return DVL_VK_RETURN;
	case SDLK_KP_1:
		return DVL_VK_NUMPAD1;
	case SDLK_KP_2:
		return DVL_VK_NUMPAD2;
	case SDLK_KP_3:
		return DVL_VK_NUMPAD3;
	case SDLK_KP_4:
		return DVL_VK_NUMPAD4;
	case SDLK_KP_5:
		return DVL_VK_NUMPAD5;
	case SDLK_KP_6:
		return DVL_VK_NUMPAD6;
	case SDLK_KP_7:
		return DVL_VK_NUMPAD7;
	case SDLK_KP_8:
		return DVL_VK_NUMPAD8;
	case SDLK_KP_9:
		return DVL_VK_NUMPAD9;
	case SDLK_KP_0:
		return DVL_VK_NUMPAD0;
	case SDLK_KP_PERIOD:
		return DVL_VK_DECIMAL;
	case SDLK_MENU:
		return DVL_VK_MENU;
#ifndef USE_SDL1
	case SDLK_KP_COMMA:
		return DVL_VK_OEM_COMMA;
#endif
	case SDLK_LCTRL:
		return DVL_VK_LCONTROL;
	case SDLK_LSHIFT:
		return DVL_VK_LSHIFT;
	case SDLK_LALT:
		return DVL_VK_LMENU;
	case SDLK_LGUI:
		return DVL_VK_LWIN;
	case SDLK_RCTRL:
		return DVL_VK_RCONTROL;
	case SDLK_RSHIFT:
		return DVL_VK_RSHIFT;
	case SDLK_RALT:
		return DVL_VK_RMENU;
	case SDLK_RGUI:
		return DVL_VK_RWIN;
	default:
		if (sym >= SDLK_a && sym <= SDLK_z) {
			return 'A' + (sym - SDLK_a);
		} else if (sym >= SDLK_0 && sym <= SDLK_9) {
			return '0' + (sym - SDLK_0);
		} else if (sym >= SDLK_F1 && sym <= SDLK_F12) {
			return DVL_VK_F1 + (sym - SDLK_F1);
		}
		DUMMY_PRINT("unknown key: name=%s sym=0x%X scan=%d mod=0x%X", SDL_GetKeyName(sym), sym, key.scancode, key.mod);
		return -1;
	}
}

#ifndef USE_SDL1
static int translate_controller_button_to_key(uint8_t sdlControllerButton)
{
	switch (sdlControllerButton) {
	case SDL_CONTROLLER_BUTTON_B:
		return 'H';
	case SDL_CONTROLLER_BUTTON_A:
		return DVL_VK_SPACE;
	case SDL_CONTROLLER_BUTTON_Y:
		return 'X';
	case SDL_CONTROLLER_BUTTON_X:
		return DVL_VK_RETURN;
	case SDL_CONTROLLER_BUTTON_LEFTSTICK:
		return 'Q';
	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
		return 'C';
	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
		return 'I';
	case SDL_CONTROLLER_BUTTON_START:
		return DVL_VK_ESCAPE;
	case SDL_CONTROLLER_BUTTON_BACK:
		return DVL_VK_TAB;
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		return DVL_VK_LEFT;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		return DVL_VK_RIGHT;
	case SDL_CONTROLLER_BUTTON_DPAD_UP:
		return DVL_VK_UP;
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
		return DVL_VK_DOWN;
	default:
		return 0;
	}
}
#endif


static WPARAM keystate_for_mouse(WPARAM ret)
{
	ret |= (SDL_GetModState() & KMOD_SHIFT) ? DVL_MK_SHIFT : 0;
	// XXX: other DVL_MK_* codes not implemented
	return ret;
}

static WINBOOL false_avail()
{
	DUMMY_PRINT("return false although event available", 1);
	return false;
}

WINBOOL PeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
	// update joystick, touch mouse (and docking the Switch) at maximally 60 fps
	currentTime = SDL_GetTicks();
	if ((currentTime - lastTime) > 15) {
#ifndef USE_SDL1
		finish_simulated_mouse_clicks(MouseX, MouseY);
#endif
        HandleJoystickAxes();
#ifdef SWITCH
		HandleDocking();
#endif
		lastTime = currentTime;
	}

	if (wMsgFilterMin != 0)
		UNIMPLEMENTED();
	if (wMsgFilterMax != 0)
		UNIMPLEMENTED();
	if (hWnd != NULL)
		UNIMPLEMENTED();

	if (wRemoveMsg == DVL_PM_NOREMOVE) {
		// This does not actually fill out lpMsg, but this is ok
		// since the engine never uses it in this case
		return !message_queue.empty() || SDL_PollEvent(NULL);
	}
	if (wRemoveMsg != DVL_PM_REMOVE) {
		UNIMPLEMENTED();
	}

	if (!message_queue.empty()) {
		*lpMsg = message_queue.front();
		message_queue.pop_front();
		return true;
	}

	SDL_Event e;
	if (!SDL_PollEvent(&e)) {
		return false;
	}

	lpMsg->hwnd = hWnd;
	lpMsg->message = 0;
	lpMsg->lParam = 0;
	lpMsg->wParam = 0;
#ifndef USE_SDL1
	handle_touch(&e, MouseX, MouseY);
#endif
	if (movie_playing) {
		// allow plus button or mouse click to skip movie, no other input
		switch (e.type) {
#ifndef USE_SDL1
		case SDL_CONTROLLERBUTTONDOWN:
		case SDL_CONTROLLERBUTTONUP:
			switch(e.cbutton.button) {
			case SDL_CONTROLLER_BUTTON_START:
			case SDL_CONTROLLER_BUTTON_A: // B on Switch
				lpMsg->message = e.type == SDL_CONTROLLERBUTTONUP ? DVL_WM_LBUTTONUP : DVL_WM_LBUTTONDOWN;
				lpMsg->lParam = (MouseY << 16) | (MouseX & 0xFFFF);
				if (lpMsg->message == DVL_WM_LBUTTONUP)
					lpMsg->wParam = keystate_for_mouse(0);
				else
					lpMsg->wParam = keystate_for_mouse(DVL_MK_LBUTTON);
				break;
			}
			break;
#endif
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			if (e.button.button == SDL_BUTTON_LEFT) {
				lpMsg->message = e.type == SDL_MOUSEBUTTONUP ? DVL_WM_LBUTTONUP : DVL_WM_LBUTTONDOWN;
				lpMsg->lParam = (e.button.y << 16) | (e.button.x & 0xFFFF);
				if (lpMsg->message == DVL_WM_LBUTTONUP)
					lpMsg->wParam = keystate_for_mouse(0);
				else
					lpMsg->wParam = keystate_for_mouse(DVL_MK_LBUTTON);
			}
			break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			lpMsg->message = e.type == SDL_KEYUP ? DVL_WM_LBUTTONUP : DVL_WM_LBUTTONDOWN;
			lpMsg->lParam = (MouseY << 16) | (MouseX  & 0xFFFF);
			if (lpMsg->message == DVL_WM_LBUTTONUP)
				lpMsg->wParam = keystate_for_mouse(0);
			else
				lpMsg->wParam = keystate_for_mouse(DVL_MK_LBUTTON);
			break;
		}
		return true;
	}
	switch (e.type) {
#ifndef USE_SDL1
	case SDL_CONTROLLERAXISMOTION:
		switch (e.caxis.axis) {
		case SDL_CONTROLLER_AXIS_LEFTX:
			leftStickXUnscaled = e.caxis.value;
			break;
		case SDL_CONTROLLER_AXIS_LEFTY:
			leftStickYUnscaled = -e.caxis.value;
			break;
		case SDL_CONTROLLER_AXIS_RIGHTX:
			rightStickXUnscaled = e.caxis.value;
			break;
		case SDL_CONTROLLER_AXIS_RIGHTY:
			rightStickYUnscaled = -e.caxis.value;
			break;
		case SDL_CONTROLLER_AXIS_TRIGGERLEFT: // ZL on Switch
			if (e.caxis.value)
				useBeltPotion(false); // health potion
			break;
		case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: // ZR on Switch
			if (e.caxis.value)
				useBeltPotion(true); // mana potion
			break;
		}
		leftStickX = leftStickXUnscaled;
		leftStickY = leftStickYUnscaled;
		rightStickX = rightStickXUnscaled;
		rightStickY = rightStickYUnscaled;
		ScaleJoystickAxes(&leftStickX, &leftStickY, leftDeadzone);
		ScaleJoystickAxes(&rightStickX, &rightStickY, rightDeadzone);
		break;
	case SDL_CONTROLLERBUTTONDOWN:
	case SDL_CONTROLLERBUTTONUP:
		switch(e.cbutton.button) {
		case SDL_CONTROLLER_BUTTON_B:	// A on Switch
		case SDL_CONTROLLER_BUTTON_Y:	// X on Switch
		case SDL_CONTROLLER_BUTTON_LEFTSTICK:
		case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
		case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
		case SDL_CONTROLLER_BUTTON_START:
		case SDL_CONTROLLER_BUTTON_BACK:
			lpMsg->message = e.type == SDL_CONTROLLERBUTTONUP ? DVL_WM_KEYUP : DVL_WM_KEYDOWN;
			lpMsg->wParam = (DWORD)translate_controller_button_to_key(e.cbutton.button);
			break;
		case SDL_CONTROLLER_BUTTON_DPAD_UP:
		case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
			lpMsg->message = e.type == SDL_CONTROLLERBUTTONUP ? DVL_WM_KEYUP : DVL_WM_KEYDOWN;
			lpMsg->wParam = (DWORD)translate_controller_button_to_key(e.cbutton.button);
			if (lpMsg->message == DVL_WM_KEYDOWN) {
				if (!stextflag) // prevent walking while in dialog mode
					movements(lpMsg->wParam);
			}
			break;
		case SDL_CONTROLLER_BUTTON_A:	// B on Switch
			lpMsg->message = e.type == SDL_CONTROLLERBUTTONUP ? DVL_WM_KEYUP : DVL_WM_KEYDOWN;
			lpMsg->wParam = (DWORD)translate_controller_button_to_key(e.cbutton.button);
			if (lpMsg->message == DVL_WM_KEYDOWN) {
				if (stextflag)
					talkwait = GetTickCount(); // JAKE: Wait before we re-initiate talking
				keyboardExpansion(lpMsg->wParam);
			}
			break;
		case SDL_CONTROLLER_BUTTON_X:	// Y on Switch
			if (invflag) {
				lpMsg->message = e.type == SDL_CONTROLLERBUTTONUP ? DVL_WM_RBUTTONUP : DVL_WM_RBUTTONDOWN;
				lpMsg->lParam = (MouseY << 16) | (MouseX & 0xFFFF);
				if (lpMsg->message == DVL_WM_RBUTTONDOWN) {
					lpMsg->wParam = keystate_for_mouse(DVL_MK_RBUTTON);
				} else {
					lpMsg->wParam = keystate_for_mouse(0);
				}
			} else {
				lpMsg->message = e.type == SDL_CONTROLLERBUTTONUP ? DVL_WM_KEYUP : DVL_WM_KEYDOWN;
				lpMsg->wParam = (DWORD)translate_controller_button_to_key(e.cbutton.button);
				if (lpMsg->message == DVL_WM_KEYDOWN)
					keyboardExpansion(lpMsg->wParam);
			}
			break;
		case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
			lpMsg->message = e.type == SDL_CONTROLLERBUTTONUP ? DVL_WM_LBUTTONUP : DVL_WM_LBUTTONDOWN;
			lpMsg->lParam = (MouseY << 16) | (MouseX & 0xFFFF);
			if (lpMsg->message == DVL_WM_LBUTTONDOWN) {
				if (newCurHidden) { // show cursor first, before clicking
					SetCursor_(CURSOR_HAND);
					newCurHidden = false;
				}
				lpMsg->wParam = keystate_for_mouse(DVL_MK_LBUTTON);
			} else {
				lpMsg->wParam = keystate_for_mouse(0);
			}
			break;
		}
		break;
#endif
#ifdef SWITCH
	// additional digital buttons that only exist on Switch:
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
		lpMsg->message = e.type == SDL_JOYBUTTONUP ? DVL_WM_KEYUP : DVL_WM_KEYDOWN;
		switch(e.jbutton.button) {
		case 16:	// L_JSTICK
			lpMsg->wParam = DVL_VK_LEFT;
			break;
		case 17:	// U_JSTICK
			lpMsg->wParam = DVL_VK_UP;
			break;
		case 18:	// R_JSTICK
			lpMsg->wParam = DVL_VK_RIGHT;
			break;
		case 19:	// D_JSTICK
			lpMsg->wParam = DVL_VK_DOWN;
			break;
		default:
			lpMsg->message = 0;
		}
		break;
#endif
	case SDL_QUIT:
		lpMsg->message = DVL_WM_QUIT;
		break;
	case SDL_KEYDOWN:
	case SDL_KEYUP: {
		int key = translate_sdl_key(e.key.keysym);
		if (key == -1)
			return false_avail();
		lpMsg->message = e.type == SDL_KEYDOWN ? DVL_WM_KEYDOWN : DVL_WM_KEYUP;
		lpMsg->wParam = (DWORD)key;
		// HACK: Encode modifier in lParam for TranslateMessage later
		lpMsg->lParam = e.key.keysym.mod << 16;
	} break;
	case SDL_MOUSEMOTION:
		if (pcurs == CURSOR_NONE) {
			SetCursor_(CURSOR_HAND);
			newCurHidden = false;
		}
		lpMsg->message = DVL_WM_MOUSEMOVE;
		lpMsg->lParam = (e.motion.y << 16) | (e.motion.x & 0xFFFF);
		lpMsg->wParam = keystate_for_mouse(0);
		break;
	case SDL_MOUSEBUTTONDOWN: {
		int button = e.button.button;
		if (button == SDL_BUTTON_LEFT) {
			lpMsg->message = DVL_WM_LBUTTONDOWN;
			lpMsg->lParam = (e.button.y << 16) | (e.button.x & 0xFFFF);
			lpMsg->wParam = keystate_for_mouse(DVL_MK_LBUTTON);
		} else if (button == SDL_BUTTON_RIGHT) {
			lpMsg->message = DVL_WM_RBUTTONDOWN;
			lpMsg->lParam = (e.button.y << 16) | (e.button.x & 0xFFFF);
			lpMsg->wParam = keystate_for_mouse(DVL_MK_RBUTTON);
		} else {
			return false_avail();
		}
	} break;
	case SDL_MOUSEBUTTONUP: {
		int button = e.button.button;
		if (button == SDL_BUTTON_LEFT) {
			lpMsg->message = DVL_WM_LBUTTONUP;
			lpMsg->lParam = (e.button.y << 16) | (e.button.x & 0xFFFF);
			lpMsg->wParam = keystate_for_mouse(0);
		} else if (button == SDL_BUTTON_RIGHT) {
			lpMsg->message = DVL_WM_RBUTTONUP;
			lpMsg->lParam = (e.button.y << 16) | (e.button.x & 0xFFFF);
			lpMsg->wParam = keystate_for_mouse(0);
		} else {
			return false_avail();
		}
	} break;
#ifndef USE_SDL1
	case SDL_TEXTINPUT:
	case SDL_WINDOWEVENT:
		if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
			lpMsg->message = DVL_WM_QUERYENDSESSION;
		} else {
			return false_avail();
		}
		break;
#endif
	default:
		DUMMY_PRINT("unknown SDL message 0x%X", e.type);
		return false_avail();
	}
	return true;
}

WINBOOL TranslateMessage(const MSG *lpMsg)
{
	assert(lpMsg->hwnd == 0);
	if (lpMsg->message == DVL_WM_KEYDOWN) {
		int key = lpMsg->wParam;
		unsigned mod = (DWORD)lpMsg->lParam >> 16;

		bool shift = (mod & KMOD_SHIFT) != 0;
		bool upper = shift != (mod & KMOD_CAPS);

		bool is_alpha = (key >= 'A' && key <= 'Z');
		bool is_numeric = (key >= '0' && key <= '9');
		bool is_control = key == DVL_VK_SPACE || key == DVL_VK_BACK || key == DVL_VK_ESCAPE || key == DVL_VK_TAB || key == DVL_VK_RETURN;
		bool is_oem = (key >= DVL_VK_OEM_1 && key <= DVL_VK_OEM_7);

		if (is_control || is_alpha || is_numeric || is_oem) {
			if (!upper && is_alpha) {
				key = tolower(key);
			} else if (shift && is_numeric) {
				key = key == '0' ? ')' : key - 0x10;
			} else if (is_oem) {
				// XXX: This probably only supports US keyboard layout
				switch (key) {
				case DVL_VK_OEM_1:
					key = shift ? ':' : ';';
					break;
				case DVL_VK_OEM_2:
					key = shift ? '?' : '/';
					break;
				case DVL_VK_OEM_3:
					key = shift ? '~' : '`';
					break;
				case DVL_VK_OEM_4:
					key = shift ? '{' : '[';
					break;
				case DVL_VK_OEM_5:
					key = shift ? '|' : '\\';
					break;
				case DVL_VK_OEM_6:
					key = shift ? '}' : ']';
					break;
				case DVL_VK_OEM_7:
					key = shift ? '"' : '\'';
					break;

				case DVL_VK_OEM_MINUS:
					key = shift ? '_' : '-';
					break;
				case DVL_VK_OEM_PLUS:
					key = shift ? '+' : '=';
					break;
				case DVL_VK_OEM_PERIOD:
					key = shift ? '>' : '.';
					break;
				case DVL_VK_OEM_COMMA:
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
			PostMessageA(lpMsg->hwnd, DVL_WM_CHAR, key, 0);
		}
	}

	return true;
}

SHORT GetAsyncKeyState(int vKey)
{
	DUMMY_ONCE();
	// TODO: Not handled yet.
	return 0;
}

LRESULT DispatchMessageA(const MSG *lpMsg)
{
	DUMMY_ONCE();
	assert(lpMsg->hwnd == 0);
	assert(CurrentProc);
	// assert(CurrentProc == GM_Game);

	return CurrentProc(lpMsg->hwnd, lpMsg->message, lpMsg->wParam, lpMsg->lParam);
}

WINBOOL PostMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DUMMY();

	assert(hWnd == 0);

	MSG msg;
	msg.hwnd = hWnd;
	msg.message = Msg;
	msg.wParam = wParam;
	msg.lParam = lParam;

	message_queue.push_back(msg);

	return true;
}

void ScaleJoystickAxes(float *x, float *y, float deadzone)
{
	//radial and scaled dead_zone
	//http://www.third-helix.com/2013/04/12/doing-thumbstick-dead-zones-right.html
	//input values go from -32767.0...+32767.0, output values are from -1.0 to 1.0;

	if (deadzone == 0) {
		return;
	}
	if (deadzone >= 1.0) {
		*x = 0;
		*y = 0;
		return;
	}

	const float maximum = 32767.0f;
	float analog_x = *x;
	float analog_y = *y;
	float dead_zone = deadzone * maximum;

	float magnitude = sqrtf(analog_x * analog_x + analog_y * analog_y);
	if (magnitude >= dead_zone) {
		// find scaled axis values with magnitudes between zero and maximum
		float scalingFactor = 1.0 / magnitude * (magnitude - dead_zone) / (maximum - dead_zone);
		analog_x = (analog_x * scalingFactor);
		analog_y = (analog_y * scalingFactor);

		// clamp to ensure results will never exceed the max_axis value
		float clamping_factor = 1.0f;
		float abs_analog_x = fabs(analog_x);
		float abs_analog_y = fabs(analog_y);
		if (abs_analog_x > 1.0 || abs_analog_y > 1.0){
			if (abs_analog_x > abs_analog_y) {
				clamping_factor = 1 / abs_analog_x;
			} else {
				clamping_factor = 1 / abs_analog_y;
			}
		}
		*x = (clamping_factor * analog_x);
		*y = (clamping_factor * analog_y);
	} else {
		*x = 0;
		*y = 0;
	}
}

static void HandleJoystickAxes()
{
	// deadzone is handled in ScaleJoystickAxes() already
	if (rightStickX != 0 || rightStickY != 0) {
		// right joystick
		if (automapflag) { // move map
			if (rightStickY < -0.5)
				AutomapUp();
			else if (rightStickY > 0.5)
				AutomapDown();
			else if (rightStickX < -0.5)
				AutomapRight();
			else if (rightStickX > 0.5)
				AutomapLeft();
		} else { // move cursor
			if (pcurs == CURSOR_NONE) {
				SetCursor_(CURSOR_HAND);
				newCurHidden = false;
			}

			const int slowdown = 80; // increase/decrease this to decrease/increase mouse speed

			int x = MouseX;
			int y = MouseY;
			hiresDX += rightStickX * 256.0;
			hiresDY += rightStickY * 256.0;

			x += hiresDX / slowdown;
			y += -(hiresDY) / slowdown;

			hiresDX %= slowdown; // keep track of dx remainder for sub-pixel per frame mouse motion
			hiresDY %= slowdown; // keep track of dy remainder for sub-pixel per frame mouse motion

			if (x < 0)
				x = 0;
			if (y < 0)
				y = 0;

			SetCursorPos(x, y);
			MouseX = x;
			MouseY = y;
		}
	}
}

#ifdef SWITCH
// Do a manual window resize when docking/undocking the Switch
static void HandleDocking()
{
	int docked;
	switch (appletGetOperationMode()) {
		case AppletOperationMode_Handheld:
			docked = 0;
			break;
		case AppletOperationMode_Docked:
			docked = 1;
			break;
		default:
			docked = 0;
	}

	int display_width;
	int display_height;
	if ((currently_docked == -1) || (docked && !currently_docked) || (!docked && currently_docked)) {
		// docked mode has changed, update window size
		if (docked) {
			display_width = 1920;
			display_height = 1080;
			currently_docked = 1;
		} else {
			display_width = 1280;
			display_height = 720;
			currently_docked = 0;
		}
		// remove leftover-garbage on screen
		for (int i = 0; i < 3; i++) {
			SDL_RenderClear(renderer);
			SDL_RenderPresent(renderer);
		}
		SDL_SetWindowSize(window, display_width, display_height);
	}
}
#endif
}
