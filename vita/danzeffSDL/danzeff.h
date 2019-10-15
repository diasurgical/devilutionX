#ifndef INCLUDED_KEYBOARDS_DANZEFF_H
#define INCLUDED_KEYBOARDS_DANZEFF_H
#ifdef USE_SDL1
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

//Pure SDL version of danzeff.
//Can be compiled on PC, PSP, whatever (if you add an input definition for it).

//danzeff is BSD licensed, if you add a new input definition, please give it back :)
//to the original distribution.

///Define one of the following to build its input driver
//#define DANZEFF_INPUT_PSP
//#define DANZEFF_INPUT_SMARTJOY_PS2

#ifdef __cplusplus
extern "C" {
#endif

//Initialization and de-init of the keyboard, provided as the keyboard uses alot of images, so if you aren't going to use it for a while, I'd recommend unloading it.

//Loads the danzeff keyboard with transparent images.
int danzeff_load();
//Loads the danzeff keyboard without transparent images (_t images)
void danzeff_load_lite();
void danzeff_free();
//TODO: maybe we can have 3 init modes: normal, no transparent, SDL does transparency

//set the screen surface for rendering on.
void danzeff_set_screen(SDL_Surface *screen);

//returns true if the keyboard is initialized
/*bool*/ int danzeff_isinitialized();

/** Attempts to read a character from the controller
* If no character is pressed then we return 0
* Other special values: 1 = move left, 2 = move right, 3 = select, 4 = start
* Every other value should be a standard ascii value.
* An unsigned int is returned so in the future we can support unicode input
*/
unsigned int danzeff_readInput(SDL_Joystick *joystick);
#define DANZEFF_LEFT 1
#define DANZEFF_RIGHT 2
#define DANZEFF_SELECT 3
#define DANZEFF_START 4

//Move the area the keyboard is rendered at to here
void danzeff_moveTo(const int newX, const int newY);

//Returns true if the keyboard that would be rendered now is different to the last time
//that the keyboard was drawn, this is altered by readInput/render.
/*bool*/ int danzeff_dirty();

//draw the keyboard to the screen
void danzeff_render(void (*PreRenderigFunction)(), void (*PostRenderigFunction)());
void set_danzeff_debug_funtion(int (*customDebugFunction)(const char *));
void printDebug(const char *string);

#ifdef __cplusplus
}
#endif

#endif //INCLUDED_KEYBOARDS_DANZEFF_H
