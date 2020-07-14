#include "devilution.h"
#include "stubs.h"
#include <SDL.h>
#include <deque>



//Should be excluded...
// #include "controls/controller_motion.h"
// #include "controls/game_controls.h"
// #include "controls/plrctrls.h"
// #include "controls/touch.h"
#include "miniwin/ddraw.h"
//end

#ifdef ANDROID
#include "miniwin/ddraw.h"

#include <time.h>
time_t start_t = 0;
time_t end_t = 0;
double diff_t;


#endif

/** @file
 * *
 * Windows message handling and keyboard event conversion for SDL.
 */

namespace dvl {

static std::deque<MSG> message_queue;

static int translate_sdl_key(SDL_Keysym key)
{
	// ref: https://wiki.libsdl.org/SDL_Keycode
	// ref: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	int sym = key.sym;
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
	case SDLK_KP_COMMA:
		return DVL_VK_OEM_COMMA;
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

static WPARAM keystate_for_mouse(WPARAM ret)
{
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	ret |= keystate[SDL_SCANCODE_LSHIFT] ? DVL_MK_SHIFT : 0;
	ret |= keystate[SDL_SCANCODE_RSHIFT] ? DVL_MK_SHIFT : 0;
	// XXX: other DVL_MK_* codes not implemented
	return ret;
}

static WINBOOL false_avail()
{
//	DUMMY_PRINT("return false although event available", 1);
	return false;
}

#ifdef ANDROID


int num_fingers_down = 0;
int Xbase = 0;
int Ybase = 0;

   //  current_time = std::chrono::high_resolution_clock::now();

   //current_time = std::chrono::high_resolution_clock::now();

   //std::cout << "Program has been running for " << std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count() << " seconds" << std::endl;

#endif




WINBOOL PeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
	if (wMsgFilterMin != 0)
		UNIMPLEMENTED();
	if (wMsgFilterMax != 0)
		UNIMPLEMENTED();
	if (hWnd != NULL)
		UNIMPLEMENTED();
	SDL_Event e;

	

#ifdef ANDROID
	//SDL_Log("SDL EVENT TYPE %d", e.type);

	
	 int InitialTouchX, InitialTouchY = 0;
	 bool JoyStickInitalPressSet = 0;


	SDL_RenderSetLogicalSize(renderer, 640, 480);

	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) && (!invflag && !spselflag && !chrflag && !stextflag && !questlog && !helpflag && !talkflag)) 
	{


    if(JoyStickCTRL  == true){ //I am not sure if this is needed
		

		if (TouchX > JoyStickBox.x && TouchX < JoyStickBox.x + JoyStickBox.w && TouchY > JoyStickBox.y && TouchY < JoyStickBox.y + JoyStickBox.h){
			
			
			// Set initial touch position
			   InitialTouchX = TouchX;
               InitialTouchY = TouchY;

				



			
			   SDL_Log("DEBUG +++++ MX %d MY %d \n", InitialTouchX,  InitialTouchY);
			   
			   JoyStickInitalPressSet = true;
			   
			// Check for Motion 
			// From mothon we move directio

		}

	}
	 else{
		 //Simplify this all to one function.
		 //SDL_Log("DEBUG Pefroming Dpad movement \n");
		 PerformDPADMovement();
	}		
}
	else{
	//SDL_Log("---Nothing\n");
		
	}

#endif

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

	if (!SDL_PollEvent(&e)) {
		return false;
	}

	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) && (!invflag && !spselflag && !chrflag && !stextflag && !questlog && !helpflag && !talkflag /* && !qtextflag*/)) {


		if (e.type == SDL_FINGERDOWN) {

			int Xclick = e.tfinger.x * 640;
			int Yclick = e.tfinger.y * 480;
			SDL_GetMouseState(&MouseX, &MouseY);
			convert_touch_xy_to_game_xy(e.tfinger.x, e.tfinger.y, &MouseX, &MouseY);
				
			if (change_controlPressed){
				

			}
		//SDL_Log("DEBUG:---- TX %d, TY %d MX %d MYl %d \n", Xclick, Yclick, TouchX, TouchY);
		}

		
	}

	lpMsg->hwnd = hWnd;
	lpMsg->lParam = 0;
	lpMsg->wParam = 0;

	switch (e.type) {
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
		lpMsg->message = DVL_WM_MOUSEMOVE;
		lpMsg->lParam = (e.motion.y << 16) | (e.motion.x & 0xFFFF);
		lpMsg->wParam = keystate_for_mouse(0);
		break;
    #ifdef ANDROID
	case SDL_FINGERMOTION: {
		if(JoyStickCTRL == false){
			
		int Xclick = e.tfinger.x * 640;
			int Yclick = e.tfinger.y * 480;
			SDL_GetMouseState(&MouseX, &MouseY);
			convert_touch_xy_to_game_xy(e.tfinger.x, e.tfinger.y, &MouseX, &MouseY);
		SDL_Log("DEBUG X %d AND Y %d " , Xclick , Yclick);
		PerformDPADMovement();

	}else{
					 
	//	RenderJoyStick(MouseX , MouseY  );
		//SDL_RenderCopy(renderer, JoystickCircleT, NULL, &JoystickCircleRect);
		int touchLocationx = e.tfinger.x * 640;
        int touchLocationy = e.tfinger.y * 480;
		int DiffernceX = InitialTouchX - touchLocationx;
		int DiffernceY = InitialTouchY - touchLocationy;
		//SDL_Log(" TTX %d, TTY %d \n",  touchLocationx, touchLocationy);
/*

TTX 24, TTY 300 
initalx  39, initaly 312 

*/

		if (JoyStickInitalPressSet){
		
			InitialTouchX ;
            InitialTouchY ;
			int DeadZoneX = 500 ; 
			int DeadZoneY = 500 ;
			DiffernceX = DiffernceX+DeadZoneX;
			DiffernceY = DiffernceY+DeadZoneY;
			//SDL_Log(" DiffernceX  %d, DiffernceY %d \n",  DiffernceX, DiffernceY);
			//SDL_Log(" DiffernceX  %d, DiffernceY %d \n",  DiffernceX, DiffernceY);

				// I am curious about how I should handle tollerances....
				// DiffernceX  8, DiffernceY 66 


				//DiffernceX  506, DiffernceY 550
				if(  (DiffernceY > 525 )   && (DiffernceX > 495 && DiffernceX < 515 )    ){ 
					AutoPickGold(myplr);// North
					AutoPickItem(myplr);
					walkInDir(WALK_N);
				}
				if(  (DiffernceY < 533  && DiffernceY > 520 ) && (DiffernceX > 534 && DiffernceX < 487 )   ) { 
					AutoPickGold(myplr);// North East
					AutoPickItem(myplr);
					walkInDir(WALK_NE);
				}
				//  DiffernceX  545, DiffernceY 543 
				//  DiffernceX  547, DiffernceY 548
				if( (DiffernceY < 530  && DiffernceY > 510 ) && (DiffernceX > 534 && DiffernceX < 487 )  ) { 
					AutoPickGold(myplr);// North West
					AutoPickItem(myplr);
					walkInDir(WALK_NW);
				}
				if( (DiffernceY < 510  && DiffernceY > 485 ) && (DiffernceX > 440 && DiffernceX < 495 )  ) { 
					AutoPickGold(myplr);// East
					AutoPickItem(myplr);
					walkInDir(WALK_E);
				}
				if ( (DiffernceY < 510  && DiffernceY > 430 ) && ( DiffernceX > 495 && DiffernceX < 515 ) ){
					AutoPickGold(myplr);// South
					AutoPickItem(myplr);
					walkInDir(WALK_S);
				}
				if( (DiffernceY < 510  && DiffernceY > 485 ) && (DiffernceX > 525 && DiffernceX < 554 )  ){
					AutoPickGold(myplr); // West
					AutoPickItem(myplr);
					walkInDir(WALK_W);
				}

		 }


	}

}

	case SDL_FINGERDOWN: {


		int Xclick = e.tfinger.x * 640;
		int Yclick = e.tfinger.y * 480;
		SDL_Log("FINGER EVENTS\n");
		
		int fingernum = SDL_GetNumTouchFingers(e.tfinger.touchId);



		if (fingernum >= 2) {
			if (invflag) {
				UseInvItem(myplr, pcursinvitem);
			} else {
				//	SDL_Log("2 finger -- Right click --- X %d  Y %d \n",Xclick , Yclick );
				// Removed because of accidental spell casting.
				// lpMsg->message = DVL_WM_RBUTTONDOWN; // Not sure if this is needed...
				// lpMsg->lParam = (Yclick << 16) | (Xclick & 0xFFFF);
				// lpMsg->wParam = keystate_for_mouse(DVL_MK_RBUTTON);
				// checkMonstersNearby(true, true);
			}
		}

		if (Xclick > Crect.x && Xclick < Crect.x + Crect.w && Yclick > Crect.y && Yclick < Crect.y + Crect.h) {
		//	SDL_Log("Right click\n");
			lpMsg->message = DVL_WM_RBUTTONDOWN;
			lpMsg->lParam = (Yclick << 16) | (Xclick & 0xFFFF);
			lpMsg->wParam = keystate_for_mouse(DVL_MK_RBUTTON);
			checkMonstersNearby(true, true);
		} 
		if (Xclick > Shiftrect.x && Xclick < Shiftrect.x+Shiftrect.w && Yclick > Shiftrect.y && Yclick < Shiftrect.y+Shiftrect.h) {
			if (!ShiftButtonPressed) {
			//	SDL_Log("SHIFT PRESSED\n");
				ShiftButtonPressed = 1;

			} else {
			//	SDL_Log("SHIFT UNPRESSED\n");
				ShiftButtonPressed = 0;
			}
		}
		//If Change_controlsRect pressed
		if (Xclick > Change_controlsRect.x && Xclick < Change_controlsRect.x+Change_controlsRect.w && Yclick > Change_controlsRect.y && Yclick < Change_controlsRect.y+Change_controlsRect.h) {
		
			//	SDL_Log("change_controlPressed PRESSED\n");
				// change_controlPressed = 1;

				//JoyStickCTRL
				 if (change_controlPressed == 0){change_controlPressed = 1; }else{change_controlPressed = 0;}
				// if (JoyStickCTRL == 0){JoyStickCTRL = 1; }else{JoyStickCTRL = 0;}
				 //JoyStickCTRL
				//DemoModeEnabled = 1;
				//int DemoModeEnabled = 0 ? 1 : 0;
				if (DemoModeEnabled == 0){DemoModeEnabled = 1; }else{DemoModeEnabled = 0;}
				//Enable JoyStick

			







		}









		if (!sbookflag && !invflag && !stextflag && Xclick > Arect.x && Xclick < Arect.x + Arect.w && Yclick > Arect.y && Yclick < Arect.y + Arect.h) {
			AttackButtonPressed = true;

			if (leveltype != DTYPE_TOWN) {
				if (checkMonstersNearby(true, false) == false) { // Appears to works well.
					ActivateObject(true);
				}
			} else {
				checkTownersNearby(true);
			}
		}

		if (Xclick > DemonHealth.x && Xclick < DemonHealth.x + DemonHealth.w  && Yclick > DemonHealth.y && Yclick < DemonHealth.y+DemonHealth.h) {
		//	SDL_Log("Red Potion Used PRESSED\n");
			time(&end_t);
			diff_t = difftime(end_t, start_t);
			SDL_Log("Execution time = %f\n", diff_t);

			if(diff_t > 0.3){ // This can be better . 
				useBeltPotion(false);

			}

			
			time(&start_t);
			
		}
		if (Xclick > AngelMana.x && Xclick < AngelMana.x + AngelMana.w && Yclick > AngelMana.y && Yclick < AngelMana.y + AngelMana.h) {
		//	SDL_Log("Blue Potion Used PRESSED\n");
			time(&end_t);
			diff_t = difftime(end_t, start_t);
			SDL_Log("Execution time = %f\n", diff_t);

			if(diff_t > 0.3){ // This can be better . 
				useBeltPotion(true);

			}

			
			time(&start_t);
			
		}
		/*   */
		if (invflag || spselflag || chrflag || stextflag || questlog || helpflag || talkflag || deathflag || sgpCurrentMenu || sbookflag || ShiftButtonPressed || pcurs != CURSOR_HAND || // If flags
		    Xclick > LGameUIMenu.x   && Xclick < LGameUIMenu.x+LGameUIMenu.w     && Yclick > LGameUIMenu.y && Yclick < LGameUIMenu.y+LGameUIMenu.h  ||                                                                                                                           // OR if left panel click
		    Xclick > RGameUIMenu.x   && Xclick < RGameUIMenu.x+RGameUIMenu.w     && Yclick > RGameUIMenu.y && Yclick < RGameUIMenu.y+RGameUIMenu.h ||                                                                                                                           // OR if right panel click
		    Xclick > PotGameUIMenu.x && Xclick < PotGameUIMenu.x+PotGameUIMenu.w && Yclick > PotGameUIMenu.y && Yclick < PotGameUIMenu.y+PotGameUIMenu.h                                                                                                                        // OR if belt click

		) {
			if (ShiftButtonPressed) {
				LeftMouseCmd(true);
				if(invflag || chrflag || deathflag ){
					ShiftButtonPressed = false;
				}

			} else {

			//	SDL_Log("Left click\n");
				lpMsg->message = DVL_WM_LBUTTONDOWN;
				lpMsg->lParam = (Yclick << 16) | (Xclick & 0xFFFF);
				lpMsg->wParam = keystate_for_mouse(DVL_MK_LBUTTON);
			}
			if(questlog){
				questlog = false;	
			}





		}

	} break;
#endif

#ifndef ANDROID
		int button = e.button.button;
		if (button == SDL_BUTTON_LEFT) {
		//	SDL_Log("BUTTON DOWN X %d  Y %d \n", e.button.x, e.button.y);
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
	}
	break;

#endif
case SDL_FINGERUP: {
	TouchX = 0;
	TouchY = 0;
	AttackButtonPressed = false;
	CastButtonPressed = false;
//	SDL_Log("Finger UP!\n");
	int Xclick = e.tfinger.x * 640;
	int Yclick = e.tfinger.y * 480;
	lpMsg->message = DVL_WM_LBUTTONUP;
	lpMsg->lParam = (Yclick << 16) | (Xclick & 0xFFFF);
	lpMsg->wParam = keystate_for_mouse(0);
	lpMsg->message = DVL_WM_RBUTTONUP;
	lpMsg->lParam = (Yclick << 16) | (Xclick & 0xFFFF);
	lpMsg->wParam = keystate_for_mouse(DVL_MK_RBUTTON);
	 SDL_PumpEvents();
	SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
} break;
case SDL_MOUSEBUTTONUP: {
//	SDL_Log("MOUSE UP!\n");
	int button = e.button.button;
	if (button == SDL_BUTTON_LEFT) {
		lpMsg->message = DVL_WM_LBUTTONUP;
		lpMsg->lParam = (e.button.y << 16) | (e.button.x & 0xFFFF);
		lpMsg->wParam = keystate_for_mouse(0);
	} else if (button == SDL_BUTTON_RIGHT) {
		lpMsg->message = DVL_WM_RBUTTONUP;
		lpMsg->lParam = (e.button.y << 16) | (e.button.x & 0xFFFF);
		lpMsg->wParam = keystate_for_mouse(0);
		 SDL_PumpEvents();
	SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
	} else {
		return false_avail();
	}
} break;
case SDL_TEXTINPUT:
case SDL_WINDOWEVENT:
	if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
		lpMsg->message = DVL_WM_QUERYENDSESSION;
	} else {
		return false_avail();
	}
	break;
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
}