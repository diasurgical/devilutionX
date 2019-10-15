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

int VitaAux::hdebug  = 1;
int VitaAux::errores = 0;
#ifdef USE_SDL1
SDLKey VitaAux::latestKey = SDLK_UNKNOWN;
#else
char VitaAux::latestKey = SDLK_UNKNOWN;
#endif
unsigned long VitaAux::allocatedMemory = 0;

void VitaAux::testJoystick(SDL_Joystick *joy)
{
	psvDebugScreenInit();
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

void VitaAux::init()
{

	psvDebugScreenInit();
	VitaAux::debug("DevilutionX port by @gokuhs\n Loading...");
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
	int jhgf;
	for (jhgf = 0; jhgf < a; jhgf++) {
		sceDisplayWaitVblankStart();
	}
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
	printf("\e[42m%s\n\e[m", texto);

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
	psvDebugScreenInit();
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
		psvDebugScreenInit();
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
}
VITATOUCH VitaAux::getVitaTouch()
{
	VITATOUCH returned;
	SceTouchData touch[SCE_TOUCH_PORT_MAX_NUM];
	int port, i;
	for (port = 0; port < SCE_TOUCH_PORT_MAX_NUM; port++) {
		sceTouchPeek(port, &touch[port], 1);
		returned.x = touch[port].report[0].x;
		returned.y = touch[port].report[0].y;
	}
	return returned;
}

void VitaAux::initVitaButtons()
{
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
}
SDL_Event VitaAux::getPressedKeyAsSDL_Event()
{
	SDL_Event event;
	SceCtrlData ctrl;
	sceCtrlPeekBufferPositive(0, &ctrl, 1);
	switch (ctrl.buttons) {

		/*
		SCE_CTRL_SELECT      = 0x00000001,            //!< Select button.
	SCE_CTRL_L3          = 0x00000002,            //!< L3 button.
	SCE_CTRL_R3          = 0x00000004,            //!< R3 button.
	SCE_CTRL_L1          = 0x00000400,            //!< L1 button.
	SCE_CTRL_R1          = 0x00000800,            //!< R1 button.ç
	SCE_CTRL_INTERCEPTED = 0x00010000,            //!< Input not available because intercepted by another application
	SCE_CTRL_PSBUTTON    = SCE_CTRL_INTERCEPTED,  //!< Playstation (Home) button.
	SCE_CTRL_HEADPHONE   = 0x00080000,            //!< Headphone plugged in.
	SCE_CTRL_VOLUP       = 0x00100000,            //!< Volume up button.
	SCE_CTRL_VOLDOWN     = 0x00200000,            //!< Volume down button.
	SCE_CTRL_POWER       = 0x40000000             //!< Power button.
	*/
	case SCE_CTRL_UP: {
		event.type           = SDL_KEYDOWN;
		event.key.keysym.sym = SDLK_UP;
		if (VitaAux::latestKey == SDLK_UNKNOWN) {
			SDL_PushEvent(&event);
		}
		VitaAux::latestKey = SDLK_UP;
		break;
	}
	case SCE_CTRL_DOWN: {
		event.type           = SDL_KEYDOWN;
		event.key.keysym.sym = SDLK_DOWN;
		if (VitaAux::latestKey == SDLK_UNKNOWN) {
			SDL_PushEvent(&event);
		}
		VitaAux::latestKey = SDLK_DOWN;
		break;
	}
	case SCE_CTRL_LEFT: {
		event.type           = SDL_KEYDOWN;
		event.key.keysym.sym = SDLK_LEFT;
		if (VitaAux::latestKey == SDLK_UNKNOWN) {
			SDL_PushEvent(&event);
		}
		VitaAux::latestKey = SDLK_LEFT;
		break;
	}
	case SCE_CTRL_RIGHT: {
		event.type           = SDL_KEYDOWN;
		event.key.keysym.sym = SDLK_RIGHT;
		if (VitaAux::latestKey == SDLK_UNKNOWN) {
			SDL_PushEvent(&event);
		}
		VitaAux::latestKey = SDLK_RIGHT;
		break;
	}
	case SCE_CTRL_CIRCLE: {
		event.type           = SDL_KEYDOWN;
		event.key.keysym.sym = SDLK_ESCAPE;
		if (VitaAux::latestKey == SDLK_UNKNOWN) {
			SDL_PushEvent(&event);
		}
		VitaAux::latestKey = SDLK_ESCAPE;
		break;
	}
	case SCE_CTRL_CROSS: {
		event.type           = SDL_KEYDOWN;
		event.key.keysym.sym = SDLK_SPACE;
		if (VitaAux::latestKey == SDLK_UNKNOWN) {
			SDL_PushEvent(&event);
		}
		VitaAux::latestKey = SDLK_SPACE;
		break;
	}
	case SCE_CTRL_START: {
		event.type           = SDL_KEYDOWN;
		event.key.keysym.sym = SDLK_RETURN;
		if (VitaAux::latestKey == SDLK_UNKNOWN) {
			SDL_PushEvent(&event);
		}
		VitaAux::latestKey = SDLK_RETURN;
		break;
	}
	case SCE_CTRL_SELECT: {
		event.type           = SDL_KEYDOWN;
		event.key.keysym.sym = SDLK_DELETE;
		if (VitaAux::latestKey == SDLK_UNKNOWN) {
			SDL_PushEvent(&event);
		}
		VitaAux::latestKey = SDLK_DELETE;
		break;
	}
	case SCE_CTRL_SQUARE: {
		event.type           = SDL_KEYDOWN;
		event.key.keysym.sym = SDLK_PAGEDOWN;
		if (VitaAux::latestKey == SDLK_UNKNOWN) {
			SDL_PushEvent(&event);
		}
		VitaAux::latestKey = SDLK_PAGEDOWN;
		break;
	}
	case SCE_CTRL_TRIANGLE: {
		event.type           = SDL_KEYDOWN;
		event.key.keysym.sym = SDLK_PAGEUP;
		if (VitaAux::latestKey == SDLK_UNKNOWN) {
			SDL_PushEvent(&event);
		}
		VitaAux::latestKey = SDLK_PAGEUP;
		break;
	}

	case SCE_CTRL_LTRIGGER: {
		event.type          = SDL_MOUSEBUTTONDOWN;
		event.button.button = SDL_BUTTON_LEFT;
		/*if (VitaAux::latestKey == SDLK_UNKNOWN) {
			SDL_PushEvent(&event);
		}
		VitaAux::latestKey = SDL_BUTTON_LEFT;*/
		break;
	}

	case SCE_CTRL_RTRIGGER: {
		event.type          = SDL_MOUSEBUTTONDOWN;
		event.button.button = SDL_BUTTON_RIGHT;
		/*if (VitaAux::latestKey == SDLK_UNKNOWN) {
			SDL_PushEvent(&event);
		}
		VitaAux::latestKey = SDL_BUTTON_LEFT;*/
		break;
	}

	default:
		if (VitaAux::latestKey != SDLK_UNKNOWN) {
			event.type           = SDL_KEYUP;
			event.key.keysym.sym = VitaAux::latestKey;
			SDL_PushEvent(&event);
		}
		VitaAux::latestKey = SDLK_UNKNOWN;
		break;
	}
	return event;
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

#endif