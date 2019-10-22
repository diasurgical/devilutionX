#ifndef VITAAUXUTIL_CPP
#define VITAAUXUTIL_CPP

#include <vitasdk.h>
#include <stdio.h>
#include "vita_aux_util.h"

#include "common/debugScreen.h"
#include "danzeffSDL/danzeff.h"

#ifndef printf
#define printf psvDebugScreenPrintf
#endif

#ifndef clrscr
#define COLOR_BLACK 0xFF000000
#define clrscr psvDebugScreenClear
#endif

/*#define SDL_PushEvent                \
	VitaAux::debug("eventInserted"); \
	SDL_PushEvent*/

using namespace std;

Uint32 latestTime = 0;

int VitaAux::hdebug                  = 1;
int VitaAux::errores                 = 0;
bool VitaAux::debugScreenInitialized = false;
#ifdef USE_SDL1
SDLKey VitaAux::latestKeyMouse = SDLK_UNKNOWN;
#else
char VitaAux::latestKeyMouse = SDLK_UNKNOWN;
#endif
VITAButtons *VitaAux::latestKey         = new VITAButtons();
VITATOUCH *VitaAux::latestPosition      = new VITATOUCH();
VITATOUCH *VitaAux::latestPositionClick = new VITATOUCH();
VITATOUCH *VitaAux::mousePosition       = new VITATOUCH();
VITATOUCH *VitaAux::latestTouch         = new VITATOUCH();
bool VitaAux::tapTriggered              = false;
unsigned long VitaAux::allocatedMemory  = 0;

void VitaAux::testJoystick(SDL_Joystick *joy)
{
	VitaAux::checkAndInitpsvDebugScreenInit();
	VitaAux::hdebug = 0;
	if (joy != NULL) {
		// Get information about the joystick
		char debugExit[1000];
#ifndef USE_SDL1
		const char *name = SDL_JoystickName(joy);
#else
		char name[100];
		sprintf(name, "Vita Joystick");
#endif
		const int num_axes    = SDL_JoystickNumAxes(joy);
		const int num_buttons = SDL_JoystickNumButtons(joy);
		const int num_hats    = SDL_JoystickNumHats(joy);
		int positionx         = 1;
		int positiony         = 1;
		psvDebugScreenSetCoordsXY(&positionx, &positiony);
		sprintf(debugExit, "Now reading from joystick '%s' with:\n"
		                   "%d axes\n"
		                   "%d buttons\n"
		                   "%d hats\n\n",
		    name,
		    num_axes,
		    num_buttons,
		    num_hats);
		VitaAux::debug(debugExit);

		int quit = 0;

		// Keep reading the state of the joystick in a loop
		while (quit == 0) {
			positionx    = 1;
			positiony    = 30;
			debugExit[0] = '\0';
			psvDebugScreenSetCoordsXY(&positionx, &positiony);
			if (SDL_QuitRequested()) {
				quit = 1;
			}

			for (int i = 0; i < num_axes; i++) {
				sprintf(debugExit + strlen(debugExit), "Axis %d: %d                    \n", i, SDL_JoystickGetAxis(joy, i));
			}

			for (int i = 0; i < num_buttons; i++) {
				sprintf(debugExit + strlen(debugExit), "Button %d: %d\n", i, SDL_JoystickGetButton(joy, i));
			}

			for (int i = 0; i < num_hats; i++) {
				sprintf(debugExit + strlen(debugExit), "Hat %d: %d\n", i, SDL_JoystickGetHat(joy, i));
			}

			sprintf(debugExit + strlen(debugExit), "\n");
			VitaAux::debug(debugExit);

			SDL_Delay(50);
		}

		SDL_JoystickClose(joy);
	} else {
		VitaAux::debug("Couldn't open the joystick. Quitting now...\n");
	}
}

void VitaAux::testTouch()
{
	VitaAux::checkAndInitpsvDebugScreenInit();
	VitaAux::hdebug = 0;
	// Get information about the joystick
	VitaAux::debug("swipe to the bottom with 1 finger to stop\n");
	/* should use SCE_TOUCH_SAMPLING_STATE_START instead of 1 but old SDK have an invalid values */
	sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
	sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);
	sceTouchEnableTouchForce(SCE_TOUCH_PORT_FRONT);
	sceTouchEnableTouchForce(SCE_TOUCH_PORT_BACK);

	SceTouchData touch_old[SCE_TOUCH_PORT_MAX_NUM];
	SceTouchData touch[SCE_TOUCH_PORT_MAX_NUM];

	while (1) {
		memcpy(touch_old, touch, sizeof(touch_old));
		printf("\e[0;5H");
		int port, i;
		/* sample both back and front surfaces */
		for (port = 0; port < SCE_TOUCH_PORT_MAX_NUM; port++) {
			sceTouchPeek(port, &touch[port], 1);
			printf("%s", ((const char *[]){ "FRONT", "BACK " })[port]);
			/* print every touch coordinates on that surface */
			for (i = 0; i < SCE_TOUCH_MAX_REPORT; i++)
				printf("\e[9%im%4i:%-4i ", (i < touch[port].reportNum) ? 7 : 0,
				    touch[port].report[i].x, touch[port].report[i].y);
			printf("\n");
		}

		if ((touch[SCE_TOUCH_PORT_FRONT].reportNum == 1)
		    && (touch_old[SCE_TOUCH_PORT_FRONT].reportNum == 1)
		    && (touch[SCE_TOUCH_PORT_FRONT].report[0].y >= 1000)
		    && (touch_old[SCE_TOUCH_PORT_FRONT].report[0].y < 1000))
			break;
	}
}

void VitaAux::testControls()
{
	VitaAux::checkAndInitpsvDebugScreenInit();
	char debugExit[1000];
	VITAButtons *pulsedButtons = new VITAButtons();
	int positionx              = 1;
	int positiony              = 1;
	VitaAux::hdebug            = 0;
	VitaAux::debug("Buttons Keys tester\nPress Select + Start + L + R to stop\n ");
	SceCtrlData ctrl;
	do {
		positionx = 1;
		positiony = 60;
		psvDebugScreenSetCoordsXY(&positionx, &positiony);
		VitaAux::readKeys(pulsedButtons);
		debugExit[0] = '\0';
		sprintf(debugExit + strlen(debugExit), "cross : ");
		if (pulsedButtons->cross != latestKey->cross) {
			sprintf(debugExit + strlen(debugExit), " %s \n", latestKey->cross == 1 ? "KEYUP" : "KEYDW");
		} else {
			sprintf(debugExit + strlen(debugExit), " -            \n");
		}

		sprintf(debugExit + strlen(debugExit), "circle : ");
		if (pulsedButtons->circle != latestKey->circle) {
			sprintf(debugExit + strlen(debugExit), " %s \n", latestKey->circle == 1 ? "KEYUP" : "KEYDW");
		} else {
			sprintf(debugExit + strlen(debugExit), " -            \n");
		}

		sprintf(debugExit + strlen(debugExit), "triangle : ");
		if (pulsedButtons->triangle != latestKey->triangle) {
			sprintf(debugExit + strlen(debugExit), " %s \n", latestKey->triangle == 1 ? "KEYUP" : "KEYDW");
		} else {
			sprintf(debugExit + strlen(debugExit), " -            \n");
		}

		sprintf(debugExit + strlen(debugExit), "squeare : ");
		if (pulsedButtons->square != latestKey->square) {
			sprintf(debugExit + strlen(debugExit), " %s \n", latestKey->square == 1 ? "KEYUP" : "KEYDW");
		} else {
			sprintf(debugExit + strlen(debugExit), " -            \n");
		}

		sprintf(debugExit + strlen(debugExit), "l tigger : ");
		if (pulsedButtons->l != latestKey->l) {
			sprintf(debugExit + strlen(debugExit), " %s \n", latestKey->l == 1 ? "KEYUP" : "KEYDW");
		} else {
			sprintf(debugExit + strlen(debugExit), " -            \n");
		}

		sprintf(debugExit + strlen(debugExit), "r tigger : ");
		if (pulsedButtons->r != latestKey->r) {
			sprintf(debugExit + strlen(debugExit), " %s \n", latestKey->r == 1 ? "KEYUP" : "KEYDW");
		} else {
			sprintf(debugExit + strlen(debugExit), " -            \n");
		}

		sprintf(debugExit + strlen(debugExit), "up : ");
		if (pulsedButtons->up != latestKey->up) {
			sprintf(debugExit + strlen(debugExit), " %s \n", latestKey->up == 1 ? "KEYUP" : "KEYDW");
		} else {
			sprintf(debugExit + strlen(debugExit), " -            \n");
		}

		sprintf(debugExit + strlen(debugExit), "left : ");
		if (pulsedButtons->left != latestKey->left) {
			sprintf(debugExit + strlen(debugExit), " %s \n", latestKey->left == 1 ? "KEYUP" : "KEYDW");
		} else {
			sprintf(debugExit + strlen(debugExit), " -            \n");
		}

		sprintf(debugExit + strlen(debugExit), "down : ");
		if (pulsedButtons->down != latestKey->down) {
			sprintf(debugExit + strlen(debugExit), " %s \n", latestKey->down == 1 ? "KEYUP" : "KEYDW");
		} else {
			sprintf(debugExit + strlen(debugExit), " -            \n");
		}

		sprintf(debugExit + strlen(debugExit), "right : ");
		if (pulsedButtons->right != latestKey->right) {
			sprintf(debugExit + strlen(debugExit), " %s \n", latestKey->right == 1 ? "KEYUP" : "KEYDW");
		} else {
			sprintf(debugExit + strlen(debugExit), " -            \n");
		}

		sprintf(debugExit + strlen(debugExit), "select : ");
		if (pulsedButtons->select != latestKey->select) {
			sprintf(debugExit + strlen(debugExit), " %s \n", latestKey->select == 1 ? "KEYUP" : "KEYDW");
		} else {
			sprintf(debugExit + strlen(debugExit), " -            \n");
		}

		sprintf(debugExit + strlen(debugExit), "start : ");
		if (pulsedButtons->start != latestKey->start) {
			sprintf(debugExit + strlen(debugExit), " %s \n", latestKey->start == 1 ? "KEYUP" : "KEYDW");
		} else {
			sprintf(debugExit + strlen(debugExit), " -            \n");
		}

		VITAButtons *aux = latestKey;
		latestKey        = pulsedButtons;
		pulsedButtons    = aux;
		VitaAux::debug(debugExit);
		delaya(300);
	} while (pulsedButtons->l != 1 && pulsedButtons->r != 1 && pulsedButtons->select != 1 && pulsedButtons->start != 1);
}

void VitaAux::init()
{
	//VitaAux::checkAndInitpsvDebugScreenInit();
	//VitaAux::debug("DevilutionX port by @gokuhs\n Loading...");
	if (!VitaAux::checkAndCreateFolder()) {
		sceKernelExitProcess(3);
	}
	VitaAux::initVitaTouch();
	VitaAux::initVitaButtons();
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) || !(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)) {
		char sdl_image_error[200];
		sprintf(sdl_image_error, "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		VitaAux::dialog(sdl_image_error, "IMG_init error!", true, true, true);
	}
}

void VitaAux::ExitAPP(int exitCode)
{
	sceKernelExitProcess(exitCode);
}

void VitaAux::getch()
{
	VitaAux::VitaAux::delaya(7);
	SceCtrlData pad;
	while (1) {
		sceCtrlReadBufferPositive(0, &pad, 1);
		if (pad.buttons) {
			break;
		}
		VitaAux::delaya(7);
	}
	//return &pad;
}

bool VitaAux::checkAndCreateFolder()
{
	SceIoStat info;
	if (sceIoGetstat("ux0:/data/DVLX00001", &info) < 0) {
		if (sceIoMkdir("ux0:/data/DVLX00001", 0777) != 0) {
			VitaAux::error("Can't create data folder");
			return false;
		}
	}
	return true;
}

bool VitaAux::copy_file(char *file_in, char *file_out)
{
	int c;
	FILE *in, *out;
	in  = fopen(file_in, "r");
	out = fopen(file_out, "w");
	if (in == NULL || !in) {
		fprintf(stderr, "%s: No such file or directory\n", file_in);
		return false;
	} else if (out == NULL || !out) {
		fprintf(stderr, "%s: No such file or directory\n", file_out);
		return false;
	}
	while ((c = getc(in)) != EOF)
		putc(c, out);

	fclose(in);
	fclose(out);

	return true;
}

SDL_Surface *KBRenderingSurface = NULL;

void VitaAux::showIME(const char *title, char *dest, void (*PreRenderigFunction)(), void (*PostRenderigFunction)(), SDL_Surface **renderingSurface)
{
	if (!danzeff_load()) {
		VitaAux::error("Can't load Danzeff!!");
		sprintf(dest, "Error");
	} else {
		if (*renderingSurface == NULL) {
			*renderingSurface = KBRenderingSurface;
		} else {
			KBRenderingSurface = *renderingSurface;
		}
		SDL_FillRect(KBRenderingSurface, &KBRenderingSurface->clip_rect, SDL_MapRGBA(KBRenderingSurface->format, 0, 0, 0, 200));
		danzeff_set_screen(KBRenderingSurface);
		set_danzeff_debug_funtion(&VitaAux::debug);
		unsigned char name_pos;
		unsigned int key;
		int exit_osk;
		exit_osk = 0;
		SDL_Joystick *paddata;
		name_pos = 0;
		while (dest[name_pos])
			name_pos++;
		if (SDL_NumJoysticks() >= 1) {
			paddata = SDL_JoystickOpen(0);
			//VitaAux::testJoystick(paddata);
			if (paddata == NULL) {
				VitaAux::dialog("Joystick can't open", "The joystick is not open property", true, false, true);
			}
			//SDL_JoystickEventState(SDL_ENABLE);
			while (!exit_osk) {
				SDL_JoystickUpdate();
				key = danzeff_readInput(paddata);
				if (key != 0) {
					switch (key) {
					case DANZEFF_START:
						exit_osk = 1;
						break;
					case DANZEFF_SELECT:
						exit_osk = 2;
						break;
					case 8: //backspace
						if (name_pos > 0) {
							name_pos--;
						}
						dest[name_pos] = 0;
						break;
					default:
						if (key >= 32) {
							dest[name_pos] = key;
							if (name_pos < 16)
								name_pos++; //Posicion Maxima
							dest[name_pos] = 0;
						}
						break;
					}
				}
				danzeff_render((*PreRenderigFunction), (*PostRenderigFunction));
			}
			if (exit_osk == 1) {
				int pru;
				pru       = strlen(dest);
				dest[pru] = '\0';
			} else if (exit_osk == 2) {
				//TODO cancela, go back to old name
			}
			SDL_JoystickClose(paddata);
		} else {
			VitaAux::dialog("Joystick can't open", "Any Joystick found", true, false, true);
		}
		SDL_FillRect(KBRenderingSurface, &KBRenderingSurface->clip_rect, SDL_MapRGBA(KBRenderingSurface->format, 0, 0, 0, 0));
		*renderingSurface = NULL;
		danzeff_free();
	}

	VitaAux::debug("Ime result: ");
	VitaAux::debug((char *)dest);
}

void VitaAux::delay(int a)
{
	sceKernelDelayThread(a * 1000 * 1000);
}

void VitaAux::delaya(int a)
{
	sceKernelDelayThread(a * 1000);
}

int VitaAux::debug(unsigned int number)
{
	//psvDebugScreenPrintf("Error: \e[31m%s\n\e[m", texto);
	char *texto_new = new char[100];
	sprintf(texto_new, "%u\0", number);
	return debug(texto_new);
}

int VitaAux::debug(unsigned long number)
{
	//psvDebugScreenPrintf("Error: \e[31m%s\n\e[m", texto);
	char *texto_new = new char[100];
	sprintf(texto_new, "%lu\0", number);
	return debug(texto_new);
}

int VitaAux::debug(double number)
{
	//psvDebugScreenPrintf("Error: \e[31m%s\n\e[m", texto);
	char *texto_new = new char[100];
	sprintf(texto_new, "%f\0", number);
	return debug(texto_new);
}

int VitaAux::debug(int number)
{
	//psvDebugScreenPrintf("Error: \e[31m%s\n\e[m", texto);
	char *texto_new = new char[100];
	sprintf(texto_new, "%i\0", number);
	return debug(texto_new);
}

int VitaAux::debug(long number)
{
	//psvDebugScreenPrintf("Error: \e[31m%s\n\e[m", texto);
	char *texto_new = new char[100];
	sprintf(texto_new, "%i\0", number);
	return debug(texto_new);
}

int VitaAux::debug(const char *texto)
{
	//psvDebugScreenPrintf("Error: \e[31m%s\n\e[m", texto);
	char *texto_new = new char[100];
	int i           = 0;
	while (texto[i] != '\0' && i < 99) {
		texto_new[i] = texto[i];
		i++;
	}
	texto_new[i] = '\0';
	return debug(texto_new);
}

int VitaAux::debug(const char texto)
{
	//psvDebugScreenPrintf("Error: \e[31m%s\n\e[m", texto);
	char *texto_new = new char[2];
	texto_new[0]    = texto;
	texto_new[1]    = '\0';
	return debug(texto_new);
}

int VitaAux::debug(char *texto)
{
	VitaAux::checkAndInitpsvDebugScreenInit();
	//printf("\e[42m%s\n\e[m", texto);
	printf(texto);

	if (hdebug == 0)
		return 0;
	FILE *ar;
	int tam;
	tam = strlen(texto);
	if (hdebug == 1) {
		ar = fopen("ux0:/data/DVLX00001/log.txt", "wt");
		if (!ar) {
			printf("Error creating log file, disable logs :-(");
			hdebug = -1;
			return -1;
		}
		fwrite("App log file\n", 1, 48, ar);
		hdebug = 2;
		fclose(ar);
	}
	if (hdebug == 2) {
		ar = fopen("ux0:/data/DVLX00001/log.txt", "at+");
		fseek(ar, 0, SEEK_END);
		fwrite(texto, 1, tam, ar);
		fwrite("\n", 1, 1, ar);
		fclose(ar);
	}
	if (hdebug == 3)
		return -1;
	return 0;
}

int VitaAux::error(char *texto)
{
	VitaAux::checkAndInitpsvDebugScreenInit();
	VitaAux::debug(texto);
	printf("\e[41m%s\n\e[m", texto);
	FILE *ar;
	int tam;
	tam = strlen(texto);
	if (errores == 0) {
		ar = fopen("ux0:/data/DVLX00001/error.txt", "wt");
		if (!ar) {
			printf("Error creating log file, disable loguing error... :-(");
			//VitaAux::delay(4);
			hdebug = -1;
			return -1;
		}
		fwrite("Errors log file\n\n", 1, 48, ar);
		errores = 2;
		fclose(ar);
	}
	if (errores == 2) {
		ar = fopen("ux0:/data/DVLX00001/error.txt", "at+");
		fseek(ar, 0, SEEK_END);
		fwrite(texto, 1, tam, ar);
		fwrite("\n", 1, 1, ar);
		fclose(ar);
		//sceKernelExitGame();
	}
	return 0;
}

int VitaAux::error(const char *texto)
{
	// psvDebugScreenPrintf("Error: \e[31m%s\n\e[m", texto);
	char *texto_new = new char[100];
	int i           = 0;
	while (texto[i] != '\0' && i < 99) {
		texto_new[i] = texto[i];
		i++;
	}
	texto_new[i] = '\0';
	return error(texto_new);
}

void VitaAux::dialog(const char *text, const char *caption, bool error, bool fatal, bool keypress)
{
	char asciiArt[1000];
	char type[7] = "INFO \0";
	if (error) {
		sprintf(type, "ERROR");
	}
	if (fatal) {
		sprintf(type, "FATAL");
	}
	sprintf(asciiArt, "\n"
	                  " ||====================================================================|| \n"
	                  " ||--------------------------------------------------------------------|| \n"
	                  " ||                          %s                                     || \n"
	                  " ||--------------------------------------------------------------------|| \n"
	                  " ||====================================================================|| \n"
	                  " ||%*s|| \n"
	                  " ||====================================================================|| \n"
	                  " ||%*s|| \n"
	                  " ||====================================================================|| \n",
	    type, 68 / 2 + strlen(caption) / 2, caption,
	    68 / 2 + strlen(text) / 2, text, text);
	if (error) {
		VitaAux::error(asciiArt);
	} else {
		VitaAux::checkAndInitpsvDebugScreenInit();
		VitaAux::debug(asciiArt);
	}
	if (keypress) {
		delay(1);
		VitaAux::getch();
	}
	if (fatal) {
		VitaAux::ExitAPP(-1);
	}
}

void VitaAux::initVitaTouch()
{
	sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
	sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);
	sceTouchEnableTouchForce(SCE_TOUCH_PORT_FRONT);
	sceTouchEnableTouchForce(SCE_TOUCH_PORT_BACK);

	VitaAux::mousePosition->x_front = 0;
	VitaAux::mousePosition->y_front = 0;
	VitaAux::latestTouch->x_front   = 0;
	VitaAux::latestTouch->y_front   = 0;
	VitaAux::latestTouch->x_back    = 0;
	VitaAux::latestTouch->y_back    = 0;
}

VITATOUCH VitaAux::getVitaTouch(bool retournLatest)
{
	VITATOUCH returned;
	int port;
	SceTouchData touch[SCE_TOUCH_PORT_MAX_NUM];
	returned.x_front = 0;
	returned.y_front = 0;
	returned.x_back  = 0;
	returned.y_back  = 0;

	sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch[SCE_TOUCH_PORT_FRONT], 1);
	sceTouchPeek(SCE_TOUCH_PORT_BACK, &touch[SCE_TOUCH_PORT_BACK], 1);
	if (touch[SCE_TOUCH_PORT_FRONT].reportNum > 0) {
		returned.x_front = touch[SCE_TOUCH_PORT_FRONT].report[0].x;
		returned.y_front = touch[SCE_TOUCH_PORT_FRONT].report[0].y;
	}
	if (touch[SCE_TOUCH_PORT_BACK].reportNum > 0) {
		returned.x_back = touch[SCE_TOUCH_PORT_BACK].report[0].x;
		returned.y_back = touch[SCE_TOUCH_PORT_BACK].report[0].y;
	}

	//Adjust
	if (returned.x_front != 0 && returned.y_front != 0) {
		returned.x_front        = (int)(returned.x_front * 960 / 1919);
		returned.y_front        = (int)(returned.y_front * 544 / 1087);
		latestPosition->x_front = returned.x_front;
		latestPosition->y_front = returned.y_front;
	} else {
		if (retournLatest == true) {
			returned.x_front = latestPosition->x_front;
			returned.y_front = latestPosition->y_front;
		}
	}
	if (returned.x_back != 0 && returned.y_back != 0) {
		returned.x_back        = (int)(returned.x_back * 960 / 1919);
		returned.y_back        = (int)(returned.y_back * 544 / 1087);
		latestPosition->x_back = returned.x_back;
		latestPosition->y_back = returned.y_back;
	} else {
		if (retournLatest == true) {
			returned.x_back = latestPosition->x_back;
			returned.y_back = latestPosition->y_back;
		}
	}
	return returned;
}
void VitaAux::initVitaButtons()
{
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
}
void VitaAux::getPressedKeyAsSDL_Event(bool inMenu, VITAMOUSEMODE mouseMode)
{
	SDL_Event *event;
	SDL_Event eventMouse;
	VITAButtons *pulsedButtons = new VITAButtons();
	SceCtrlData ctrl;
	VitaAux::readKeys(pulsedButtons);
	if (pulsedButtons->cross != latestKey->cross) {
		event                 = new SDL_Event();
		event->type           = latestKey->cross == 1 ? SDL_KEYUP : SDL_KEYDOWN;
		event->key.keysym.sym = !inMenu ? SDLK_7 : SDLK_SPACE;
		SDL_PushEvent(event);
	}
	if (pulsedButtons->circle != latestKey->circle) {
		event                 = new SDL_Event();
		event->type           = latestKey->circle == 1 ? SDL_KEYUP : SDL_KEYDOWN;
		event->key.keysym.sym = !inMenu ? SDLK_8 : SDLK_ESCAPE;
		SDL_PushEvent(event);
	}
	if (pulsedButtons->triangle != latestKey->triangle) {
		event                 = new SDL_Event();
		event->type           = latestKey->triangle == 1 ? SDL_KEYUP : SDL_KEYDOWN;
		event->key.keysym.sym = !inMenu ? SDLK_6 : SDLK_PAGEUP;
		SDL_PushEvent(event);
	}
	if (pulsedButtons->square != latestKey->square) {
		event                 = new SDL_Event();
		event->type           = latestKey->square == 1 ? SDL_KEYUP : SDL_KEYDOWN;
		event->key.keysym.sym = !inMenu ? SDLK_5 : SDLK_PAGEDOWN;
		SDL_PushEvent(event);
	}
	if (pulsedButtons->up != latestKey->up) {
		event                 = new SDL_Event();
		event->type           = latestKey->up == 1 ? SDL_KEYUP : SDL_KEYDOWN;
		event->key.keysym.sym = !inMenu ? SDLK_2 : SDLK_UP;
		SDL_PushEvent(event);
	}
	if (pulsedButtons->left != latestKey->left) {
		event                 = new SDL_Event();
		event->type           = latestKey->left == 1 ? SDL_KEYUP : SDL_KEYDOWN;
		event->key.keysym.sym = !inMenu ? SDLK_1 : SDLK_LEFT;
		SDL_PushEvent(event);
	}
	if (pulsedButtons->down != latestKey->down) {
		event                 = new SDL_Event();
		event->type           = latestKey->down == 1 ? SDL_KEYUP : SDL_KEYDOWN;
		event->key.keysym.sym = !inMenu ? SDLK_3 : SDLK_DOWN;
		SDL_PushEvent(event);
	}
	if (pulsedButtons->right != latestKey->right) {
		event                 = new SDL_Event();
		event->type           = latestKey->right == 1 ? SDL_KEYUP : SDL_KEYDOWN;
		event->key.keysym.sym = !inMenu ? SDLK_4 : SDLK_RIGHT;
		SDL_PushEvent(event);
	}
	if (pulsedButtons->select != latestKey->select) {
		event                 = new SDL_Event();
		event->type           = latestKey->select == 1 ? SDL_KEYUP : SDL_KEYDOWN;
		event->key.keysym.sym = !inMenu ? SDLK_TAB : SDLK_DELETE;
		SDL_PushEvent(event);
	}
	if (pulsedButtons->start != latestKey->start) {
		event                 = new SDL_Event();
		event->type           = latestKey->start == 1 ? SDL_KEYUP : SDL_KEYDOWN;
		event->key.keysym.sym = !inMenu ? SDLK_ESCAPE : SDLK_RETURN;
		SDL_PushEvent(event);
	}

	VITAButtons *aux = latestKey;
	latestKey        = pulsedButtons;
	pulsedButtons    = aux;

	//Touch pads

	if (mouseMode == VITAMOUSEMODE_AS_MOUSE) {
		VITATOUCH touch = VitaAux::getVitaTouch();
		if (touch.x_front != latestPosition->x_front || touch.y_front != latestPosition->y_front) {
			event           = new SDL_Event();
			event->button.x = touch.x_front;
			event->button.y = touch.y_front;
			event->motion.x = touch.x_front;
			event->motion.y = touch.y_front;
			event->type     = SDL_MOUSEMOTION;
			SDL_PushEvent(event);
		}
		latestPosition->x_front = touch.x_front;
		latestPosition->y_front = touch.y_front;
		//"Mouse" Actions
		if (pulsedButtons->l != latestKey->l) {
			event                = new SDL_Event();
			event->button.x      = latestPosition->x_front;
			event->button.y      = latestPosition->y_front;
			event->button.button = SDL_BUTTON_LEFT;
			event->type          = latestKey->l == 1 ? SDL_MOUSEBUTTONUP : SDL_MOUSEBUTTONDOWN;
			SDL_PushEvent(event);
		}
		if (pulsedButtons->r != latestKey->r) {
			event                = new SDL_Event();
			event->button.x      = latestPosition->x_front;
			event->button.y      = latestPosition->y_front;
			event->button.button = SDL_BUTTON_RIGHT;
			event->type          = latestKey->l == 1 ? SDL_MOUSEBUTTONUP : SDL_MOUSEBUTTONDOWN;
			SDL_PushEvent(event);
		}
	} else { //VITAMOUSEMODE_AS_TOUCHPAD
		VITATOUCH touch = VitaAux::getVitaTouch(false);
		if ((touch.x_back != 0 || touch.y_back != 0) && (latestTouch->x_back != 0 || latestTouch->y_back != 0) && (latestTouch->x_back != touch.x_back || latestTouch->y_back != touch.x_back)) //Movement touch > 0 and latestTouch != 0
		{
			//Movement logic
			int appendX = latestTouch->x_back - touch.x_back;
			int appendY = latestTouch->y_back - touch.y_back;
			mousePosition->x_front -= (int)(appendX * 1.5);
			mousePosition->y_front -= (int)(appendY * 1.5);
			if (mousePosition->x_front < 0) {
				mousePosition->x_front = 0;
			}
			if (mousePosition->y_front < 0) {
				mousePosition->y_front = 0;
			}
			if (mousePosition->x_front > SCREEN_WIDTH) {
				mousePosition->x_front = SCREEN_WIDTH;
			}
			if (mousePosition->y_front > SCREEN_HEIGHT) {
				mousePosition->y_front = SCREEN_HEIGHT;
			}
			event           = new SDL_Event();
			event->button.x = mousePosition->x_front;
			event->button.y = mousePosition->y_front;
			event->motion.x = mousePosition->x_front;
			event->motion.y = mousePosition->y_front;
			event->type     = SDL_MOUSEMOTION;
			SDL_PushEvent(event);
		}
		latestTouch->x_back = touch.x_back;
		latestTouch->y_back = touch.y_back;

		bool clickTrigged = false;
		if (touch.x_front > 0 && latestPositionClick->x_front == 0) {
			event                = new SDL_Event();
			event->button.x      = mousePosition->x_front;
			event->button.y      = mousePosition->y_front;
			event->button.button = touch.x_front < 850 ? SDL_BUTTON_LEFT : SDL_BUTTON_RIGHT;
			event->type          = SDL_MOUSEBUTTONDOWN;
			SDL_PushEvent(event);
			latestPositionClick->x_front = touch.x_front;
			clickTrigged                 = true;
		} else if (touch.x_front == 0 && latestPositionClick->x_front != 0) {
			event                = new SDL_Event();
			event->button.x      = mousePosition->x_front;
			event->button.y      = mousePosition->y_front;
			event->button.button = latestPositionClick->x_front < 850 ? SDL_BUTTON_LEFT : SDL_BUTTON_RIGHT;
			event->type          = SDL_MOUSEBUTTONUP;
			SDL_PushEvent(event);
			latestPositionClick->x_front = 0;
			clickTrigged                 = true;
		}

		//Tap Action
		if (!clickTrigged) {
			if (touch.x_back != 0 || touch.y_back != 0) {
				if (latestTime == 0) {
					latestTime = SDL_GetTicks();
				}
			} else {
				if (latestTime != 0) {
					Uint32 time_now = SDL_GetTicks();
					if (time_now - latestTime < 100) {
						event                = new SDL_Event();
						event->button.x      = mousePosition->x_front;
						event->button.y      = mousePosition->y_front;
						event->button.button = SDL_BUTTON_LEFT;
						event->type          = SDL_MOUSEBUTTONDOWN;
						SDL_PushEvent(event);
						VitaAux::tapTriggered = true;
					}
					latestTime = 0;
				}
			}
			if (VitaAux::tapTriggered) {
				VitaAux::tapTriggered = false;
				event                 = new SDL_Event();
				event->button.x       = mousePosition->x_front;
				event->button.y       = mousePosition->y_front;
				event->button.button  = SDL_BUTTON_LEFT;
				event->type           = SDL_MOUSEBUTTONUP;
				SDL_PushEvent(event);
			}
		}
	}
}

void VitaAux::printMemInfo(unsigned int amount)
{
	SceKernelFreeMemorySizeInfo sizeMem;
	char memTest[300];
	if (sceKernelGetFreeMemorySize(&sizeMem) == 0) {
		sprintf(memTest, "Mem info: size: %i, size cdram: %i, size user: %i\n", sizeMem.size, sizeMem.size_cdram, sizeMem.size_user);
		if (amount > 0) {
			sprintf(memTest + strlen(memTest), "Need: %i\n", amount);
		}
	} else {
		sprintf(memTest, "I can't get size mem info\n");
		if (amount > 0) {
			sprintf(memTest + strlen(memTest), "But you trying to locate: %i bytes\n", amount);
		}
	}
	sprintf(memTest + strlen(memTest), "Allocated Memory: %lu\n-----", allocatedMemory);
	VitaAux::debug(memTest);
}

void VitaAux::updateAllocMem(unsigned int amount, bool minus)
{
	if (!minus) {
		allocatedMemory += amount;
	} else {
		allocatedMemory -= amount;
	}
}

void VitaAux::readKeys(VITAButtons *keys)
{

	SceCtrlData ctrl;
	sceCtrlPeekBufferPositive(0, &ctrl, 1);
	//Reset
	keys->select   = 0;
	keys->start    = 0;
	keys->up       = 0;
	keys->right    = 0;
	keys->down     = 0;
	keys->left     = 0;
	keys->l        = 0;
	keys->r        = 0;
	keys->triangle = 0;
	keys->circle   = 0;
	keys->cross    = 0;
	keys->square   = 0;
	/*
	const char *btn_label[] = { "SELECT ", "", "", "START ",
	"UP ", "RIGHT ", "DOWN ", "LEFT ", "L ", "R ", "", "",
	"TRIANGLE ", "CIRCLE ", "CROSS ", "SQUARE " };
	*/
	if (ctrl.buttons & SCE_CTRL_START) {
		keys->start = 1;
	}
	if (ctrl.buttons & SCE_CTRL_SELECT) {
		keys->select = 1;
	}
	if (ctrl.buttons & SCE_CTRL_UP) {
		keys->up = 1;
	}
	if (ctrl.buttons & SCE_CTRL_RIGHT) {
		keys->right = 1;
	}
	if (ctrl.buttons & SCE_CTRL_DOWN) {
		keys->down = 1;
	}
	if (ctrl.buttons & SCE_CTRL_LEFT) {
		keys->left = 1;
	}
	if (ctrl.buttons & SCE_CTRL_LTRIGGER) {
		keys->l = 1;
	}
	if (ctrl.buttons & SCE_CTRL_RTRIGGER) {
		keys->r = 1;
	}
	if (ctrl.buttons & SCE_CTRL_TRIANGLE) {
		keys->triangle = 1;
	}
	if (ctrl.buttons & SCE_CTRL_CIRCLE) {
		keys->circle = 1;
	}
	if (ctrl.buttons & SCE_CTRL_CROSS) {
		keys->cross = 1;
	}
	if (ctrl.buttons & SCE_CTRL_SQUARE) {
		keys->square = 1;
	}
}

void VitaAux::checkAndInitpsvDebugScreenInit()
{
	if (!VitaAux::debugScreenInitialized) {
		psvDebugScreenInit();
	}
}
#endif