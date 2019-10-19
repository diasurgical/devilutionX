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
	int x;
	int y;
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
#ifdef USE_SDL1
	static SDLKey latestKeyMouse;
#else
	static char latestKeyMouse;
#endif
	static VITAButtons *latestKey;
	static unsigned long allocatedMemory;

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
	static VITATOUCH getVitaTouch();

	//Migrate to SDEvent
	static void initVitaButtons();
	static void getPressedKeyAsSDL_Event(bool inMenu);

	//Utils
	static void printMemInfo(unsigned int amount = 0);
	static void updateAllocMem(unsigned int amount, bool minus);
	static void testJoystick(SDL_Joystick *joy);
	static void testTouch();
	static void testControls();
	static void readKeys(VITAButtons *keys);
};
#endif