#ifndef VITAAUXUTIL_H
#define VITAAUXUTIL_H

#ifdef USE_SDL1
#include <SDL/SDL_events.h>
#else
#include <SDL2/SDL_events.h>
#endif

#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define RGB(r, g, b) (0xFF000000 | ((b) << 16) | ((g) << 8) | (r))

/*Mis Librerias pa PSP 4.3*/
/*Esto es una especie de pause pa PSP*/
typedef unsigned char BYTE;

typedef void *HANDLE;
typedef HANDLE HWND, HGDIOBJ, HMODULE, HDC, HRGN, HINSTANCE, HPALETTE, HCURSOR;
#ifndef NOTYPES
typedef int32_t LONG;
#endif
typedef LONG HRESULT;
#ifndef NOTYPES
typedef uint32_t DWORD;
#endif
typedef struct tagRECT {
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;
} RECT;
typedef RECT *LPRECT;
typedef struct tagPALETTEENTRY {
	BYTE peRed;
	BYTE peGreen;
	BYTE peBlue;
	BYTE peFlags;
} PALETTEENTRY, *PPALETTEENTRY, *LPPALETTEENTRY;

typedef struct tagVITATOUCH {
	int x_front;
	int y_front;
	int x_back;
	int y_back;
} VITATOUCH;

typedef struct VITAButtons {
	char cross;
	char circle;
	char triangle;
	char square;
	char l;
	char r;
	char up;
	char left;
	char down;
	char right;
	char select;
	char start;
};

typedef enum {
	VITAMOUSEMODE_AS_MOUSE    = 0,
	VITAMOUSEMODE_AS_TOUCHPAD = 1
} VITAMOUSEMODE;

typedef enum {
	SDL_JOYBUTTON_UP       = 8,
	SDL_JOYBUTTON_RIGHT    = 9,
	SDL_JOYBUTTON_DOWN     = 6,
	SDL_JOYBUTTON_LEFT     = 7,
	SDL_JOYBUTTON_L        = 4,
	SDL_JOYBUTTON_R        = 5,
	SDL_JOYBUTTON_X        = 2,
	SDL_JOYBUTTON_SQUARE   = 3,
	SDL_JOYBUTTON_TRIANGLE = 0,
	SDL_JOYBUTTON_CIRCLE   = 1,
	SDL_JOYBUTTON_SELECT   = 10,
	SDL_JOYBUTTON_START    = 11,
} SDL_BUTTON_VITA;

#ifdef USE_SDL1
typedef enum {
	/* Touch events */
	SDL_FINGERDOWN = 0x700,
	SDL_FINGERUP,
	SDL_FINGERMOTION
} SDL_EventTypeEx;

typedef int64_t SDL_FingerID;
typedef int64_t SDL_TouchID;

typedef struct SDL_TouchFingerEvent {
	SDL_TouchID port;
	Uint32 type;         /**< ::SDL_FINGERMOTION or ::SDL_FINGERDOWN or ::SDL_FINGERUP */
	Uint32 timestamp;    /**< In milliseconds, populated using SDL_GetTicks() */
	SDL_TouchID touchId; /**< The touch device id */
	SDL_FingerID fingerId;
	float x;        /**< Normalized in the range 0...1 */
	float y;        /**< Normalized in the range 0...1 */
	float dx;       /**< Normalized in the range -1...1 */
	float dy;       /**< Normalized in the range -1...1 */
	float pressure; /**< Normalized in the range 0...1 */
} SDL_TouchFingerEvent;

#endif

class VitaAux {
public:
	static void init();
	static void ExitAPP(int exitCode);
	static void getch();
	static bool checkAndCreateFolder();
	static bool copy_file(char *file_in, char *file_out);
	static void showIME(const char *title, char *dest, void (*PreRenderigFunction)(), void (*PostRenderigFunction)(), SDL_Surface **renderingSurface);

	//Modo de //debug, va creando un log.				|
	static int hdebug; //0 Desactivado, 1 Activado, 3 Error :-s
	                   //Variable de error;
	static int errores;
	static VITATOUCH *latestPosition;
	static VITATOUCH *latestPositionClick;
	static VITATOUCH *mousePosition;
	static VITATOUCH *latestTouch;
	static bool tapTriggered;
#ifdef USE_SDL1
	static SDLKey latestKeyMouse;
#else
	static char latestKeyMouse;
#endif
	static VITAButtons *latestKey;
	static unsigned long allocatedMemory;
	static bool debugScreenInitialized;

	static void delay(int);
	static void delaya(int);
	//El orgullo de la casa; escribe en un archivo el texto deseado a modo de debug, si hemos puesto que el valor de hdebug sea 1 se ejecutara si no saldrá-
	static int debug(const char *texto);
	static int debug(char *texto);
	static int debug(const char texto);
	static int debug(double number);
	static int debug(int number);
	static int debug(long number);
	static int debug(unsigned int number);
	static int debug(unsigned long number);
	//Funcion de Error

	static int error(char *texto);
	static int error(const char *texto);

	//Beutirfy dialogs
	static void dialog(const char *caption, const char *text, bool error, bool fatal, bool keypress);

	//Touch
	static void initVitaTouch();
	static VITATOUCH getVitaTouch(bool retournLatest = true);
#ifdef USE_SDL1
	static void processTouchEventToSDL(bool scaleTouchs = false);
#endif

	//Migrate to SDEvent
	static void
	initVitaButtons();
	static void getPressedKeyAsSDL_Event(bool inMenu, VITAMOUSEMODE mouseMode = VITAMOUSEMODE_AS_MOUSE);

	//Utils
	static void printMemInfo(unsigned int amount = 0);
	static void updateAllocMem(unsigned int amount, bool minus);
	static void testJoystick(SDL_Joystick *joy);
	static void testTouch();
	static void testControls();
	static void readKeys(VITAButtons *keys);
	static void checkAndInitpsvDebugScreenInit();
};
#endif