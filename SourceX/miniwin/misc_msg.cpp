#include <cstdint>
#include <deque>
#include <SDL.h>


#include "all.h"
#include "display.h"
#include "stubs.h"
#include "controls/controller_motion.h"
#include "controls/game_controls.h"
#include "controls/plrctrls.h"
#include "controls/remap_keyboard.h"
#include "controls/touch.h"
#include "display.h"
#include "controls/controller.h"

#ifdef __SWITCH__
#include "platform/switch/docking.h"
#include <switch.h>
#endif

#ifdef ANDROID
//#include "misc_micro.h"
#include "miniwin/androidinput.h"

#include <time.h>
time_t start_t = 0;
time_t end_t = 0;
double diff_t;



time_t Qstart_t = 0;
time_t Qend_t = 0;
double Qdiff_t;

time_t Sstart_t = 0;
time_t Send_t = 0;
double Sdiff_t;


time_t Fstart_t = 0;
time_t Fend_t = 0;
double Fdiff_t;


//JNI CALL BACK CODE
#include <string.h>
#include <inttypes.h>
#include <pthread.h>
#include <jni.h>
#include <android/log.h>
#include <assert.h>


typedef struct tick_context {
    JavaVM  *javaVM;
	jclass   jniHelperClz;
	jobject  jniHelperObj;
	jclass   mainActivityClz;
	jobject  mainActivityObj;
	pthread_mutex_t  lock;
	int      done;
} TickContext;
TickContext g_ctx;
//org/diasurgical/devilutionx/DevilutionXSDLActivity
jstring
org_diasurgical_devilutionx_DevilutionXSDLActivity_stringFromJNI( JNIEnv* env, jobject thiz ){

 return env->NewStringUTF("Hello from JNI !  Compiled with ABI");


}







//onNativeTouch

// void*  UpdateTicks(void* context) {

//     SDL_Log("DEBUG -- UPDATE TICKS IS CALLED");
//     TickContext *pctx = (TickContext*) context;
//     JavaVM *javaVM = pctx->javaVM;
//     JNIEnv *env;

//     jint res = javaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
//     if (res != JNI_OK) {
//         res = javaVM->AttachCurrentThread(&env, NULL);
//         if (JNI_OK != res) {
//             //LOGE("Failed to AttachCurrentThread, ErrorCode = %d", res);

//             SDL_Log("FAILED TO ATTACH THREAD");
//             return NULL;
//         }
//     }

//     jmethodID statusId = env->GetMethodID(pctx->jniHelperClz, "updateStatus", "(Ljava/lang/String;)V");
//     //sendJavaMsg(env, pctx->jniHelperObj, statusId, "TickerThread status: initializing...");

//     // get mainActivity updateTimer function
//     jmethodID timerId = env->GetMethodID(pctx->mainActivityClz, "updateTimer", "()V");

//     struct timeval beginTime, curTime, usedTime, leftTime;
//     const struct timeval kOneSecond = {
//             (__kernel_time_t)1,
//             (__kernel_suseconds_t) 0
//     };

//     //sendJavaMsg(env, pctx->jniHelperObj, statusId, "TickerThread status: start ticking ...");
//     while(1) {
//         gettimeofday(&beginTime, NULL);
//         pthread_mutex_lock(&pctx->lock);
//         int done = pctx->done;
//         if (pctx->done) {
//             pctx->done = 0;
//         }
//         pthread_mutex_unlock(&pctx->lock);
//         if (done) {
//             break;
//         }
//         env->CallVoidMethod(pctx->mainActivityObj, timerId);

//         gettimeofday(&curTime, NULL);
//         timersub(&curTime, &beginTime, &usedTime);
//         timersub(&kOneSecond, &usedTime, &leftTime);
//         struct timespec sleepTime;
//         sleepTime.tv_sec = leftTime.tv_sec;
//         sleepTime.tv_nsec = leftTime.tv_usec * 1000;

//         if (sleepTime.tv_sec <= 1) {
//             nanosleep(&sleepTime, NULL);
//         } else {
//             //sendJavaMsg(env, pctx->jniHelperObj, statusId, "TickerThread error: processing too long!");
//         }
//     }

//     //sendJavaMsg(env, pctx->jniHelperObj, statusId, "TickerThread status: ticking stopped");
//     javaVM->DetachCurrentThread();
//     return context;
// }





// extern "C" JNIEXPORT
// void JNICALL InvokeFunction(JNIEnv *env, jobject instance) {
//     SDL_Log("JNI CALL START TICKET");
//     pthread_t       threadInfo_;
//     pthread_attr_t  threadAttr_;

//     pthread_attr_init(&threadAttr_);
//     pthread_attr_setdetachstate(&threadAttr_, PTHREAD_CREATE_DETACHED);

//     pthread_mutex_init(&g_ctx.lock, NULL);

//     jclass clz = env->GetObjectClass(instance);
//     g_ctx.mainActivityClz = (jclass)env->NewGlobalRef(clz); // hack
//     g_ctx.mainActivityObj = env->NewGlobalRef(instance);

//     int result  = pthread_create( &threadInfo_, &threadAttr_, UpdateTicks, &g_ctx);
//     assert(result == 0);

//     pthread_attr_destroy(&threadAttr_);

//     (void)result;
// }


// void queryRuntimeInfo(JNIEnv *env, jobject instance) {
//     SDL_Log("DEBUG QUERYRUNTIME INTO");
//     // Find out which OS we are running on. It does not matter for this app
//     // just to demo how to call static functions.
//     // Our java JniHelper class id and instance are initialized when this
//     // shared lib got loaded, we just directly use them
//     //    static function does not need instance, so we just need to feed
//     //    class and method id to JNI
//     jmethodID versionFunc = env->GetStaticMethodID(g_ctx.jniHelperClz, "getBuildVersion", "()Ljava/lang/String;"); // This returns null.
//     if (!versionFunc) {
//         SDL_Log("DEBUG versionFunc INTO");
//         //LOGE("Failed to retrieve getBuildVersion() methodID @ line %d",__LINE__);
//         return;
//     }
//     SDL_Log("DEBUG BUILD VERSION INTO");
//     jstring buildVersion = (jstring)env->CallStaticObjectMethod(g_ctx.jniHelperClz, versionFunc); // hack
//     const char *version = env->GetStringUTFChars(buildVersion, NULL);

//     if (!version) {
//         SDL_Log("DEBUG buildVersion INTO");
//         //LOGE("Unable to get version string @ line %d", __LINE__);
//         return;
//     }
//     //LOGI("Android Version - %s", version);
//     SDL_Log("DEBUG ANDROID VERSION %s", version);
//     env->ReleaseStringUTFChars(buildVersion, version);

//     // we are called from JNI_OnLoad, so got to release LocalRef to avoid leaking
//     env->DeleteLocalRef(buildVersion);

//     // Query available memory size from a non-static public function
//     // we need use an instance of JniHelper class to call JNI
//     jmethodID memFunc = env->GetMethodID(g_ctx.jniHelperClz, "getRuntimeMemorySize", "()J");
//     if (!memFunc) {
//         //LOGE("Failed to retrieve getRuntimeMemorySize() methodID @ line %d",__LINE__);
//         SDL_Log("DEBUG Failed to retrieve memory size");
//         return;
//     }
//     jlong result = env->CallLongMethod(instance, memFunc);
//     //LOGI("Runtime free memory size: %" PRId64, result);
//     SDL_Log("DEBUG RUN TIME FREE MEMORY SIZE");
//     (void)result;  // silence the compiler warning
// }




JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
	JNIEnv* env;
	memset(&g_ctx, 0, sizeof(g_ctx));

	SDL_Log(" DEBUG  -- JNI INIT!!!");
	g_ctx.javaVM = vm;
	if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
		return JNI_ERR; // JNI version not supported.
	}


	jclass  clz = env->FindClass("org/diasurgical/devilutionx/DevilutionXSDLActivity");
	g_ctx.jniHelperClz = (jclass)env->NewGlobalRef(clz); // hack

	jmethodID  jniHelperCtor = env->GetMethodID(g_ctx.jniHelperClz,"<init>", "()V");
	jobject    handler = env->NewObject(g_ctx.jniHelperClz, jniHelperCtor);
	g_ctx.jniHelperObj = env->NewGlobalRef(handler);
	//queryRuntimeInfo(env, g_ctx.jniHelperObj);

	g_ctx.done = 0;
	g_ctx.mainActivityObj = NULL;
	return  JNI_VERSION_1_6;
}








//END JNI CODE
#endif





/** @file
 * *
 * Windows message handling and keyboard event conversion for SDL.
 */

namespace dvl {



static std::deque<MSG> message_queue;

bool mouseWarping = false;
int mouseWarpingX;
int mouseWarpingY;

void SetCursorPos(int X, int Y)
{
	mouseWarpingX = X;
	mouseWarpingY = Y;
	mouseWarping = true;
	LogicalToOutput(&X, &Y);
	SDL_WarpMouseInWindow(ghMainWnd, X, Y);
}

// Moves the mouse to the first attribute "+" button.
void FocusOnCharInfo()
{
	if (invflag || plr[myplr]._pStatPts <= 0)
		return;

	// Find the first incrementable stat.
	int pc = plr[myplr]._pClass;
	int stat = -1;
	for (int i = 4; i >= 0; --i) {
		switch (i) {
		case ATTRIB_STR:
			if (plr[myplr]._pBaseStr >= MaxStats[pc][ATTRIB_STR])
				continue;
			break;
		case ATTRIB_MAG:
			if (plr[myplr]._pBaseMag >= MaxStats[pc][ATTRIB_MAG])
				continue;
			break;
		case ATTRIB_DEX:
			if (plr[myplr]._pBaseDex >= MaxStats[pc][ATTRIB_DEX])
				continue;
			break;
		case ATTRIB_VIT:
			if (plr[myplr]._pBaseVit >= MaxStats[pc][ATTRIB_VIT])
				continue;
			break;
		default:
			continue;
		}
		stat = i;
	}
	if (stat == -1)
		return;
	const RECT32 &rect = ChrBtnsRect[stat];
	SetCursorPos(rect.x + (rect.w / 2), rect.y + (rect.h / 2));
}

static int translate_sdl_key(SDL_Keysym key)
{
	// ref: https://wiki.libsdl.org/SDL_Keycode
	// ref: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	SDL_Keycode sym = key.sym;
	remap_keyboard_key(&sym);
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
		SDL_Log("unknown key: name=%s sym=0x%X scan=%d mod=0x%X", SDL_GetKeyName(sym), sym, key.scancode, key.mod);
		return -1;
	}
}

namespace {

LPARAM position_for_mouse(short x, short y)
{
	return (((uint16_t)(y & 0xFFFF)) << 16) | (uint16_t)(x & 0xFFFF);
}

WPARAM keystate_for_mouse(WPARAM ret)
{
	ret |= (SDL_GetModState() & KMOD_SHIFT) ? DVL_MK_SHIFT : 0;
	// XXX: other DVL_MK_* codes not implemented
	return ret;
}

bool false_avail(const char *name, int value)
{
	SDL_Log("Unhandled SDL event: %s %d", name, value);
	return true;
}

void StoreSpellCoords()
{
	const int START_X = 20;
	const int END_X = 636;
	const int END_Y = 495;
	const int BOX_SIZE = 56;
	speedspellcount = 0;
	int xo = END_X, yo = END_Y;
	for (int i = 0; i < 4; i++) {
		std::uint64_t spells;
		switch (i) {
		case RSPLTYPE_SKILL:
			spells = plr[myplr]._pAblSpells;
			break;
		case RSPLTYPE_SPELL:
			spells = plr[myplr]._pMemSpells;
			break;
		case RSPLTYPE_SCROLL:
			spells = plr[myplr]._pScrlSpells;
			break;
		case RSPLTYPE_CHARGES:
			spells = plr[myplr]._pISpells;
			break;
		default:
			continue;
		}
		std::uint64_t spell = 1;
		for (int j = 1; j < MAX_SPELLS; j++) {
			if ((spell & spells)) {
				speedspellscoords[speedspellcount] = { xo - 36, yo - 188 };
				++speedspellcount;
				xo -= BOX_SIZE;
				if (xo == START_X) {
					xo = END_X;
					yo -= BOX_SIZE;
				}
			}
			spell <<= 1;
		}
		if (spells && xo != END_X)
			xo -= BOX_SIZE;
		if (xo == START_X) {
			xo = END_X;
			yo -= BOX_SIZE;
		}
	}
}

} // namespace

/**
 * @brief Try to clean the inventory related cursor states.
 * @return True if it is safe to close the inventory
 */
bool BlurInventory()
{
	if (pcurs >= CURSOR_FIRSTITEM) {
		if (!TryDropItem()) {
			if (plr[myplr]._pClass == PC_WARRIOR) {
				PlaySFX(PS_WARR16); // "Where would I put this?"
#ifndef SPAWN
			} else if (plr[myplr]._pClass == PC_ROGUE) {
				PlaySFX(PS_ROGUE16);
			} else if (plr[myplr]._pClass == PC_SORCERER) {
				PlaySFX(PS_MAGE16);
#endif
			}
			return false;
		}
	}

	invflag = false;
	if (pcurs > CURSOR_HAND)
		SetCursor_(CURSOR_HAND);
	if (chrflag)
		FocusOnCharInfo();

	return true;
}






bool PeekMessage(LPMSG lpMsg)
{
#ifdef __SWITCH__
	HandleDocking();
#endif

	if (!message_queue.empty()) {
		*lpMsg = message_queue.front();
		message_queue.pop_front();
		return true;
	}

	SDL_Event e;

if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) && (!invflag && !spselflag && !chrflag && !stextflag && !questlog && !helpflag && !talkflag /* && !qtextflag*/))
{


		// if (e.type == SDL_FINGERDOWN) {
		// int Xclick = e.tfinger.x * SCREEN_WIDTH;
		// int Yclick = e.tfinger.y * SCREEN_HEIGHT;
		// 	SDL_GetMouseState(&MouseX, &MouseY);
		// 	convert_touch_xy_to_game_xy(e.tfinger.x, e.tfinger.y, &MouseX, &MouseY);
		// 	if(!qtextflag)
		// 		PerformDPADMovement((int)gAndroidTouchX, (int)gAndroidTouchY);
		// }


	}

#ifdef ANDROID
	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) && (!invflag && !spselflag && !chrflag && !stextflag && !questlog && !helpflag && !talkflag && !sbookflag))
	{
		SDL_GetMouseState(&MouseX, &MouseY);

			int x ;
			int y ;
			SDL_GetMouseState(&x, &y);
			dvl::OutputToLogical(&x, &y);
			PerformDPADMovement(x, y);
}

#endif

	if (!SDL_PollEvent(&e)) {
		return false;
	}

	lpMsg->message = 0;
	lpMsg->lParam = 0;
	lpMsg->wParam = 0;

	if (e.type == SDL_QUIT) {
		lpMsg->message = DVL_WM_QUIT;
		return true;
	}
#ifndef ANDROID
#ifndef USE_SDL1
	handle_touch(&e, MouseX, MouseY);
#endif

#ifdef USE_SDL1
	if (e.type == SDL_MOUSEMOTION) {
		OutputToLogical(&e.motion.x, &e.motion.y);
	} else if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
		OutputToLogical(&e.button.x, &e.button.y);
	}
#endif
#endif

	if ((e.type == SDL_KEYUP || e.type == SDL_KEYDOWN) && e.key.keysym.sym == SDLK_UNKNOWN) {
		// Erroneous events generated by RG350 kernel.
		return true;
	}



	if (ProcessControllerMotion(e)) {
		ScaleJoysticks();
		return true;
	}



	GameAction action;
	if (GetGameAction(e, &action)) {
		if (action.type != GameActionType_NONE) {
			sgbControllerActive = true;

			if (movie_playing) {
				lpMsg->message = DVL_WM_KEYDOWN;
				if (action.type == GameActionType_SEND_KEY)
					lpMsg->wParam = action.send_key.vk_code;
				return true;
			}
		}

		switch (action.type) {
		case GameActionType_NONE:
			break;
		case GameActionType_USE_HEALTH_POTION:
			UseBeltItem(BLT_HEALING);
			break;
		case GameActionType_USE_MANA_POTION:
			UseBeltItem(BLT_MANA);
			break;
		case GameActionType_PRIMARY_ACTION:
			PerformPrimaryAction();
			break;
		case GameActionType_SECONDARY_ACTION:
			PerformSecondaryAction();
			break;
		case GameActionType_CAST_SPELL:
			PerformSpellAction();
			break;
		case GameActionType_TOGGLE_QUICK_SPELL_MENU:
			if (!invflag || BlurInventory()) {
				if (!spselflag)
					DoSpeedBook();
				else
					spselflag = false;
				chrflag = false;
				questlog = false;
				sbookflag = false;
				StoreSpellCoords();
			}
			break;
		case GameActionType_TOGGLE_CHARACTER_INFO:
			chrflag = !chrflag;
			if (chrflag) {
				questlog = false;
				spselflag = false;
				if (pcurs == CURSOR_DISARM)
					SetCursor_(CURSOR_HAND);
				FocusOnCharInfo();
			}
			break;
		case GameActionType_TOGGLE_QUEST_LOG:
			if (!questlog) {
				StartQuestlog();
				chrflag = false;
				spselflag = false;
			} else {
				questlog = false;
			}
			break;
		case GameActionType_TOGGLE_INVENTORY:
			if (invflag) {
				BlurInventory();
			} else {
				sbookflag = false;
				spselflag = false;
				invflag = true;
				if (pcurs == CURSOR_DISARM)
					SetCursor_(CURSOR_HAND);
				FocusOnInventory();
			}
			break;
		case GameActionType_TOGGLE_SPELL_BOOK:
			if (BlurInventory()) {
				invflag = false;
				spselflag = false;
				sbookflag = !sbookflag;
			}
			break;
		case GameActionType_SEND_KEY:
			lpMsg->message = action.send_key.up ? DVL_WM_KEYUP : DVL_WM_KEYDOWN;
			lpMsg->wParam = action.send_key.vk_code;
			return true;
		case GameActionType_SEND_MOUSE_CLICK:
			sgbControllerActive = false;
			switch (action.send_mouse_click.button) {
			case GameActionSendMouseClick::LEFT:
				lpMsg->message = action.send_mouse_click.up ? DVL_WM_LBUTTONUP : DVL_WM_LBUTTONDOWN;
				break;
			case GameActionSendMouseClick::RIGHT:
				lpMsg->message = action.send_mouse_click.up ? DVL_WM_RBUTTONUP : DVL_WM_RBUTTONDOWN;
				break;
			}
			lpMsg->lParam = position_for_mouse(MouseX, MouseY);
			break;
		}
		return true;
	} else if (e.type < SDL_JOYAXISMOTION || e.type >= 0x700) {
		if (!mouseWarping || e.type != SDL_MOUSEMOTION)
			sgbControllerActive = false;
		if (mouseWarping && e.type == SDL_MOUSEMOTION)
			mouseWarping = false;
	}

	switch (e.type) {
#ifndef USE_SDL1
	case SDL_CONTROLLERDEVICEADDED:
	case SDL_CONTROLLERDEVICEREMOVED:
		break;
	case SDL_JOYDEVICEADDED:
	case SDL_JOYDEVICEREMOVED:
		InitController();
		break;
#endif
	case SDL_QUIT:
		lpMsg->message = DVL_WM_QUIT;
		break;
	case SDL_KEYDOWN:
	case SDL_KEYUP: {
		int key = translate_sdl_key(e.key.keysym);
		if (key == -1)
			return false_avail(e.type == SDL_KEYDOWN ? "SDL_KEYDOWN" : "SDL_KEYUP", e.key.keysym.sym);
		lpMsg->message = e.type == SDL_KEYDOWN ? DVL_WM_KEYDOWN : DVL_WM_KEYUP;
		lpMsg->wParam = (DWORD)key;
		// HACK: Encode modifier in lParam for TranslateMessage later
		lpMsg->lParam = e.key.keysym.mod << 16;
	} break;
	case SDL_MOUSEMOTION:
		lpMsg->message = DVL_WM_MOUSEMOVE;
		lpMsg->lParam = position_for_mouse(e.motion.x, e.motion.y);
		lpMsg->wParam = keystate_for_mouse(0);
		break;

	case SDL_FINGERMOTION: {

		int Xclick = e.tfinger.x * SCREEN_WIDTH;
		int Yclick = e.tfinger.y * SCREEN_HEIGHT;
			SDL_GetMouseState(&MouseX, &MouseY);
			convert_touch_xy_to_game_xy(e.tfinger.x, e.tfinger.y, &MouseX, &MouseY);
			if(!qtextflag)
				PerformDPADMovement((int)gAndroidTouchX, (int)gAndroidTouchY);
}

	case SDL_FINGERDOWN: {
		if(invflag){

		//SDL_Log("DEBUG FINGERDOW!! ");
		time(&Fend_t);
		Fdiff_t = difftime(Fend_t, Fstart_t);
	    if(Fdiff_t > 0.8){ // This can be better .

		   //SDL_Log("DEBUG DELAY MET!!! ");
		   //NetSendCmdLocParam1(true, pcurs == CURSOR_DISARM ? CMD_DISARMXY : CMD_OPOBJXY,  object[closest]._ox, object[closest]._oy, pcursobj);
		    //if (invflag) {
				UseInvItem(myplr, pcursinvitem);
			//}
			}
			time(&Fstart_t);
		}


		int Xclick = e.tfinger.x * SCREEN_WIDTH;
		int Yclick = e.tfinger.y * SCREEN_HEIGHT;
			SDL_GetMouseState(&MouseX, &MouseY);
			convert_touch_xy_to_game_xy(e.tfinger.x, e.tfinger.y, &MouseX, &MouseY);
			if(!qtextflag)
				PerformDPADMovement((int)gAndroidTouchX, (int)gAndroidTouchY);

		FingerDownAction(e, lpMsg);
		if(!qtextflag)
				PerformDPADMovement((int)gAndroidTouchX, (int)gAndroidTouchY);


	} break;



case SDL_FINGERUP: {
	gAndroidTouchX = 0;
	gAndroidTouchY = 0;
	AttackButtonPressed = false;
	CastButtonPressed = false;
	//SDL_Log("DEBUG Finger UP!\n");
	int Xclick = e.tfinger.x * SCREEN_WIDTH;
	int Yclick = e.tfinger.y * SCREEN_HEIGHT;
	lpMsg->message = DVL_WM_LBUTTONUP;
	lpMsg->lParam = (Yclick << 16) | (Xclick & 0xFFFF);
	lpMsg->wParam = keystate_for_mouse(0);
	lpMsg->message = DVL_WM_RBUTTONUP;
	lpMsg->lParam = (Yclick << 16) | (Xclick & 0xFFFF);
	lpMsg->wParam = keystate_for_mouse(DVL_MK_RBUTTON);
	 SDL_PumpEvents();
	SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
} break;

/*
	case SDL_MOUSEBUTTONDOWN: {
		int button = e.button.button;
		if (button == SDL_BUTTON_LEFT) {
			lpMsg->message = DVL_WM_LBUTTONDOWN;
			lpMsg->lParam = position_for_mouse(e.button.x, e.button.y);
			lpMsg->wParam = keystate_for_mouse(DVL_MK_LBUTTON);
		} else if (button == SDL_BUTTON_RIGHT) {
			lpMsg->message = DVL_WM_RBUTTONDOWN;
			lpMsg->lParam = position_for_mouse(e.button.x, e.button.y);
			lpMsg->wParam = keystate_for_mouse(DVL_MK_RBUTTON);
		}
	} break;

	*/
	case SDL_MOUSEBUTTONUP: {
		int button = e.button.button;
		if (button == SDL_BUTTON_LEFT) {
			lpMsg->message = DVL_WM_LBUTTONUP;
			lpMsg->lParam = position_for_mouse(e.button.x, e.button.y);
			lpMsg->wParam = keystate_for_mouse(0);
		} else if (button == SDL_BUTTON_RIGHT) {
			lpMsg->message = DVL_WM_RBUTTONUP;
			lpMsg->lParam = position_for_mouse(e.button.x, e.button.y);
			lpMsg->wParam = keystate_for_mouse(0);
		}
	} break;
#ifndef USE_SDL1
	case SDL_MOUSEWHEEL:
		lpMsg->message = DVL_WM_KEYDOWN;
		if (e.wheel.y > 0) {
			lpMsg->wParam = GetAsyncKeyState(DVL_VK_CONTROL) ? DVL_VK_OEM_PLUS : DVL_VK_UP;
		} else if (e.wheel.y < 0) {
			lpMsg->wParam = GetAsyncKeyState(DVL_VK_CONTROL) ? DVL_VK_OEM_MINUS : DVL_VK_DOWN;
		} else if (e.wheel.x > 0) {
			lpMsg->wParam = DVL_VK_LEFT;
		} else if (e.wheel.x < 0) {
			lpMsg->wParam = DVL_VK_RIGHT;
		}
		break;
#if SDL_VERSION_ATLEAST(2, 0, 4)
	case SDL_AUDIODEVICEADDED:
		return false_avail("SDL_AUDIODEVICEADDED", e.adevice.which);
	case SDL_AUDIODEVICEREMOVED:
		return false_avail("SDL_AUDIODEVICEREMOVED", e.adevice.which);
	case SDL_KEYMAPCHANGED:
		return false_avail("SDL_KEYMAPCHANGED", 0);
#endif
	case SDL_TEXTEDITING:
		return false_avail("SDL_TEXTEDITING", e.edit.length);
	case SDL_TEXTINPUT:
		return false_avail("SDL_TEXTINPUT", e.text.windowID);
	case SDL_WINDOWEVENT:
		switch (e.window.event) {
		case SDL_WINDOWEVENT_SHOWN:
			gbActive = true;
			lpMsg->message = DVL_WM_PAINT;
			break;
		case SDL_WINDOWEVENT_HIDDEN:
			gbActive = false;
			break;
		case SDL_WINDOWEVENT_EXPOSED:
			lpMsg->message = DVL_WM_PAINT;
			break;
		case SDL_WINDOWEVENT_MOVED:
		case SDL_WINDOWEVENT_RESIZED:
		case SDL_WINDOWEVENT_SIZE_CHANGED:
		case SDL_WINDOWEVENT_MINIMIZED:
		case SDL_WINDOWEVENT_MAXIMIZED:
		case SDL_WINDOWEVENT_RESTORED:
		case SDL_WINDOWEVENT_LEAVE:
		case SDL_WINDOWEVENT_FOCUS_GAINED:
		case SDL_WINDOWEVENT_FOCUS_LOST:
#if SDL_VERSION_ATLEAST(2, 0, 5)
		case SDL_WINDOWEVENT_TAKE_FOCUS:
#endif
			break;
		case SDL_WINDOWEVENT_ENTER:
			// Bug in SDL, SDL_WarpMouseInWindow doesn't emit SDL_MOUSEMOTION
			// and SDL_GetMouseState gives previous location if mouse was
			// outside window (observed on Ubuntu 19.04)
			if (mouseWarping) {
				MouseX = mouseWarpingX;
				MouseY = mouseWarpingY;
				mouseWarping = false;
			}
			break;
		case SDL_WINDOWEVENT_CLOSE:
			lpMsg->message = DVL_WM_QUERYENDSESSION;
			break;
		default:
			return false_avail("SDL_WINDOWEVENT", e.window.event);
		}

		break;
#endif
	default:
		return false_avail("unknown", e.type);
	}
	return true;
}

bool TranslateMessage(const MSG *lpMsg)
{
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
				switch (key) {
				case '1':
					key = '!';
					break;
				case '2':
					key = '@';
					break;
				case '3':
					key = '#';
					break;
				case '4':
					key = '$';
					break;
				case '5':
					key = '%';
					break;
				case '6':
					key = '^';
					break;
				case '7':
					key = '&';
					break;
				case '8':
					key = '*';
					break;
				case '9':
					key = '(';
					break;
				case '0':
					key = ')';
					break;
				}
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

#ifdef _DEBUG
			if (key >= 32) {
				SDL_Log("char: %c", key);
			}
#endif

			// XXX: This does not add extended info to lParam
			PostMessage(DVL_WM_CHAR, key, 0);
		}
	}

	return true;
}

SHORT GetAsyncKeyState(int vKey)
{
	if (vKey == DVL_MK_LBUTTON)
		return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT);
	if (vKey == DVL_MK_RBUTTON)
		return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT);
	const Uint8 *state = SDLC_GetKeyState();
	switch (vKey) {
	case DVL_VK_CONTROL:
		return state[SDLC_KEYSTATE_LEFTCTRL] || state[SDLC_KEYSTATE_RIGHTCTRL] ? 0x8000 : 0;
	case DVL_VK_SHIFT:
		return state[SDLC_KEYSTATE_LEFTSHIFT] || state[SDLC_KEYSTATE_RIGHTSHIFT] ? 0x8000 : 0;
	case DVL_VK_MENU:
		return state[SDLC_KEYSTATE_LALT] || state[SDLC_KEYSTATE_RALT] ? 0x8000 : 0;
	case DVL_VK_LEFT:
		return state[SDLC_KEYSTATE_LEFT] ? 0x8000 : 0;
	case DVL_VK_UP:
		return state[SDLC_KEYSTATE_UP] ? 0x8000 : 0;
	case DVL_VK_RIGHT:
		return state[SDLC_KEYSTATE_RIGHT] ? 0x8000 : 0;
	case DVL_VK_DOWN:
		return state[SDLC_KEYSTATE_DOWN] ? 0x8000 : 0;
	default:
		return 0;
	}
}

LRESULT DispatchMessage(const MSG *lpMsg)
{
	assert(CurrentProc);

	return CurrentProc(NULL, lpMsg->message, lpMsg->wParam, lpMsg->lParam);
}

bool PostMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	MSG msg;
	msg.message = Msg;
	msg.wParam = wParam;
	msg.lParam = lParam;

	message_queue.push_back(msg);

	return true;
}

} // namespace dvl
