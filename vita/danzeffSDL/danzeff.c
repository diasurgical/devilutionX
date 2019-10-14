#include "danzeff.h"

#define false 0
#define true 1

struct _danzeff_state {
	//x,y are for an analogue stick
	int x;
	int y;
	unsigned int buttons;
};
typedef struct _danzeff_state danzeff_state;

//Specific key presses
#define BUT_UP 1    /* backspace */
#define BUT_RIGHT 2 /* move right */
#define BUT_DOWN 4  /* enter */
#define BUT_LEFT 8  /* move left */

//Toggle current input set
#define BUT_SWITCH 16 /* switch keyset */
#define BUT_SHIFT 32  /* hold for shift */

//These send keypresses
#define BUT_DUP 64     /* send top input */
#define BUT_DRIGHT 128 /* send right input */
#define BUT_DDOWN 256  /* send bottom input */
#define BUT_DLEFT 512  /* send left input */

//Special inputs, you do not need to define them, but if you want extra keys then do so. They are returned as DANZEFF_SELECT, DANZEFF_START
#define BUT_SELECT 1024
#define BUT_START 2048

/** To 'port' danzeff to a new input device, you need to create a new definition for the following function.
 *  the x,y variables define which square is selected (0-2).
 *  Take a look at the already implemented ones, you should be able to figure it out easily enough :)
 */
#ifdef DANZEFF_INPUT_VITA
danzeff_state getStateFromJoystick(SDL_Joystick *joystick)
{
	danzeff_state toReturn;
	toReturn.buttons = 0;

	if (SDL_JoystickGetButton(joystick, 0))
		toReturn.buttons |= BUT_DUP;
	if (SDL_JoystickGetButton(joystick, 1))
		toReturn.buttons |= BUT_DRIGHT;
	if (SDL_JoystickGetButton(joystick, 2))
		toReturn.buttons |= BUT_DDOWN;
	if (SDL_JoystickGetButton(joystick, 3))
		toReturn.buttons |= BUT_DLEFT;

	//Digital
	if (SDL_JoystickGetButton(joystick, 6))
		toReturn.buttons |= BUT_DOWN;
	if (SDL_JoystickGetButton(joystick, 7))
		toReturn.buttons |= BUT_LEFT;
	if (SDL_JoystickGetButton(joystick, 8))
		toReturn.buttons |= BUT_UP;
	if (SDL_JoystickGetButton(joystick, 9))
		toReturn.buttons |= BUT_RIGHT;

	//L R
	if (SDL_JoystickGetButton(joystick, 4))
		toReturn.buttons |= BUT_SWITCH;
	if (SDL_JoystickGetButton(joystick, 5))
		toReturn.buttons |= BUT_SHIFT;

	//Start Select
	if (SDL_JoystickGetButton(joystick, 11))
		toReturn.buttons |= BUT_START;
	if (SDL_JoystickGetButton(joystick, 10))
		toReturn.buttons |= BUT_SELECT;

	//Analog
	toReturn.x = SDL_JoystickGetAxis(joystick, 0);
	if (toReturn.x < -(32768 * 2 / 3))
		toReturn.x = 0;
	else if (toReturn.x > (32768 * 2 / 3))
		toReturn.x = 2;
	else
		toReturn.x = 1;

	toReturn.y = SDL_JoystickGetAxis(joystick, 1);
	if (toReturn.y < -(32768 * 2 / 3))
		toReturn.y = 0;
	else if (toReturn.y > (32768 * 2 / 3))
		toReturn.y = 2;
	else
		toReturn.y = 1;

	return toReturn;
}
#endif
#ifdef DANZEFF_INPUT_PSP
danzeff_state getStateFromJoystick(SDL_Joystick *joystick)
{
	danzeff_state toReturn;
	toReturn.buttons = 0;

	if (SDL_JoystickGetButton(joystick, 0))
		toReturn.buttons |= BUT_DUP;
	if (SDL_JoystickGetButton(joystick, 1))
		toReturn.buttons |= BUT_DRIGHT;
	if (SDL_JoystickGetButton(joystick, 2))
		toReturn.buttons |= BUT_DDOWN;
	if (SDL_JoystickGetButton(joystick, 3))
		toReturn.buttons |= BUT_DLEFT;

	//Digital
	if (SDL_JoystickGetButton(joystick, 6))
		toReturn.buttons |= BUT_DOWN;
	if (SDL_JoystickGetButton(joystick, 7))
		toReturn.buttons |= BUT_LEFT;
	if (SDL_JoystickGetButton(joystick, 8))
		toReturn.buttons |= BUT_UP;
	if (SDL_JoystickGetButton(joystick, 9))
		toReturn.buttons |= BUT_RIGHT;

	//L R
	if (SDL_JoystickGetButton(joystick, 4))
		toReturn.buttons |= BUT_SWITCH;
	if (SDL_JoystickGetButton(joystick, 5))
		toReturn.buttons |= BUT_SHIFT;

	//Start Select
	if (SDL_JoystickGetButton(joystick, 11))
		toReturn.buttons |= BUT_START;
	if (SDL_JoystickGetButton(joystick, 10))
		toReturn.buttons |= BUT_SELECT;

	//Analog
	toReturn.x = SDL_JoystickGetAxis(joystick, 0);
	if (toReturn.x < -(32768 * 2 / 3))
		toReturn.x = 0;
	else if (toReturn.x > (32768 * 2 / 3))
		toReturn.x = 2;
	else
		toReturn.x = 1;

	toReturn.y = SDL_JoystickGetAxis(joystick, 1);
	if (toReturn.y < -(32768 * 2 / 3))
		toReturn.y = 0;
	else if (toReturn.y > (32768 * 2 / 3))
		toReturn.y = 2;
	else
		toReturn.y = 1;

	return toReturn;
}
#endif

#ifdef DANZEFF_INPUT_SMARTJOY_PS2
danzeff_state getStateFromJoystick(SDL_Joystick *joystick)
{
	danzeff_state toReturn;
	toReturn.buttons = 0;

	//Buttons
	if (SDL_JoystickGetButton(joystick, 0))
		toReturn.buttons |= BUT_DUP;
	if (SDL_JoystickGetButton(joystick, 1))
		toReturn.buttons |= BUT_DRIGHT;
	if (SDL_JoystickGetButton(joystick, 2))
		toReturn.buttons |= BUT_DDOWN;
	if (SDL_JoystickGetButton(joystick, 3))
		toReturn.buttons |= BUT_DLEFT;

	//Start/Select
	if (SDL_JoystickGetButton(joystick, 8))
		toReturn.buttons |= BUT_START;
	if (SDL_JoystickGetButton(joystick, 9))
		toReturn.buttons |= BUT_SELECT;

	//L/R (MERGE THEM)
	if (SDL_JoystickGetButton(joystick, 4))
		toReturn.buttons |= BUT_SWITCH;
	if (SDL_JoystickGetButton(joystick, 6))
		toReturn.buttons |= BUT_SWITCH;
	if (SDL_JoystickGetButton(joystick, 5))
		toReturn.buttons |= BUT_SHIFT;
	if (SDL_JoystickGetButton(joystick, 7))
		toReturn.buttons |= BUT_SHIFT;

	//Left digi
	int L_DIGI_X = SDL_JoystickGetAxis(joystick, 4);
	if (L_DIGI_X > 0)
		toReturn.buttons |= BUT_RIGHT;
	else if (L_DIGI_X < 0)
		toReturn.buttons |= BUT_LEFT;

	int L_DIGI_Y = SDL_JoystickGetAxis(joystick, 5);
	if (L_DIGI_Y > 0)
		toReturn.buttons |= BUT_DOWN;
	else if (L_DIGI_Y < 0)
		toReturn.buttons |= BUT_UP;

	//ANALOG
	toReturn.x = SDL_JoystickGetAxis(joystick, 0);
	if (toReturn.x < -(32768 * 2 / 3))
		toReturn.x = 0;
	else if (toReturn.x > (32768 * 2 / 3))
		toReturn.x = 2;
	else
		toReturn.x = 1;

	toReturn.y = SDL_JoystickGetAxis(joystick, 1);
	if (toReturn.y < -(32768 * 2 / 3))
		toReturn.y = 0;
	else if (toReturn.y > (32768 * 2 / 3))
		toReturn.y = 2;
	else
		toReturn.y = 1;

	return toReturn;
}
#endif

/*bool*/ int holding     = false; //user is holding a button
/*bool*/ int dirty       = true;  //keyboard needs redrawing
/*bool*/ int shifted     = false; //user is holding shift
int mode                 = 0;     //charset selected. (0 - letters or 1 - numbers)
/*bool*/ int initialized = false; //keyboard is initialized

//Position on the 3-3 grid the user has selected (range 0-2)
int selected_x = 1;
int selected_y = 1;

// location that we are moved to
int moved_x = 0;
int moved_y = 0;

//Variable describing where each of the images is
#define guiStringsSize 12 /* size of guistrings array */
#define PICS_BASEDIR "app0:/danzeff/"
char *guiStrings[] = {
	PICS_BASEDIR "keys.png", PICS_BASEDIR "keys_t.png", PICS_BASEDIR "keys_s.png",
	PICS_BASEDIR "keys_c.png", PICS_BASEDIR "keys_c_t.png", PICS_BASEDIR "keys_s_c.png",
	PICS_BASEDIR "nums.png", PICS_BASEDIR "nums_t.png", PICS_BASEDIR "nums_s.png",
	PICS_BASEDIR "nums_c.png", PICS_BASEDIR "nums_c_t.png", PICS_BASEDIR "nums_s_c.png"
};

//amount of modes (non shifted), each of these should have a corresponding shifted mode.
#define MODE_COUNT 2
//this is the layout of the keyboard
char modeChar[MODE_COUNT * 2][3][3][5] = {
	{ //standard letters
	    { ",abc", ".def", "!ghi" },
	    { "-jkl", "\010m n", "?opq" },
	    { "(rst", ":uvw", ")xyz" } },

	{ //capital letters
	    { "^ABC", "@DEF", "*GHI" },
	    { "_JKL", "\010M N", "\"OPQ" },
	    { "=RST", ";UVW", "/XYZ" } },

	{ //numbers
	    { "\0\0\0001", "\0\0\0002", "\0\0\0003" },
	    { "\0\0\0004", "\010\0 5", "\0\0\0006" },
	    { "\0\0\0007", "\0\0\0008", "\0\00009" } },

	{ //special characters
	    { ",(.)", "\"<'>", "-[_]" },
	    { "!{?}", "\010\0 \0", "+\\=/" },
	    { ":@;#", "~$`%", "*^|&" } }
};

/*bool*/ int danzeff_isinitialized()
{
	return initialized;
}

/*bool*/ int danzeff_dirty()
{
	return dirty;
}

/** Attempts to read a character from the controller
* If no character is pressed then we return 0
* Other special values: 1 = move left, 2 = move right, 3 = select, 4 = start
* Every other value should be a standard ascii value.
* An unsigned int is returned so in the future we can support unicode input
*/
unsigned int danzeff_readInput(SDL_Joystick *joystick)
{
	danzeff_state state = getStateFromJoystick(joystick);

	//Work out where the analog stick is selecting

	if (selected_x != state.x || selected_y != state.y) //If they've moved, update dirty
	{
		dirty      = true;
		selected_x = state.x;
		selected_y = state.y;
	}

	//if they are changing shift then that makes it dirty too
	if ((!shifted && (state.buttons & BUT_SHIFT)) || (shifted && !(state.buttons & BUT_SHIFT)))
		dirty = true;

	unsigned int pressed = 0; //character they have entered, 0 as that means 'nothing'
	shifted              = (state.buttons & BUT_SHIFT) ? true : false;

	if (!holding) {
		if (state.buttons & (BUT_DDOWN | BUT_DRIGHT | BUT_DUP | BUT_DLEFT)) //pressing a char select button
		{
			int innerChoice = 0;
			if (state.buttons & BUT_DUP)
				innerChoice = 0;
			else if (state.buttons & BUT_DLEFT)
				innerChoice = 1;
			else if (state.buttons & BUT_DDOWN)
				innerChoice = 2;
			else //if (state.buttons & BUT_DRIGHT)
				innerChoice = 3;

			//Now grab the value out of the array
			pressed = modeChar[mode * 2 + shifted][state.y][state.x][innerChoice];
		} else if (state.buttons & BUT_SWITCH) //toggle mode
		{
			dirty = true;
			mode++;
			mode %= MODE_COUNT;
		} else if (state.buttons & BUT_DOWN) {
			pressed = '\n';
		} else if (state.buttons & BUT_UP) {
			pressed = 8; //backspace
		} else if (state.buttons & BUT_LEFT) {
			pressed = DANZEFF_LEFT; //LEFT
		} else if (state.buttons & BUT_RIGHT) {
			pressed = DANZEFF_RIGHT; //RIGHT
		} else if (state.buttons & BUT_SELECT) {
			pressed = DANZEFF_SELECT; //SELECT
		} else if (state.buttons & BUT_START) {
			pressed = DANZEFF_START; //START
		}
	}

	holding = state.buttons & ~BUT_SHIFT; //shift doesn't set holding

	return pressed;
}

SDL_Surface *keyBits[guiStringsSize];
int keyBitsSize = 0;

///variable needed for rendering in SDL, the screen surface to draw to, and a function to set it!
SDL_Surface *danzeff_screen;
SDL_Rect danzeff_screen_rect;
void danzeff_set_screen(SDL_Surface *screen)
{
	danzeff_screen        = screen;
	danzeff_screen_rect.x = 0;
	danzeff_screen_rect.y = 0;
	danzeff_screen_rect.h = screen->h;
	danzeff_screen_rect.w = screen->w;
}

///Internal function to draw a surface internally offset
//Render the given surface at the current screen position offset by screenX, screenY
//the surface will be internally offset by offsetX,offsetY. And the size of it to be drawn will be intWidth,intHeight
void surface_draw_offset(SDL_Surface *pixels, int screenX, int screenY, int offsetX, int offsetY, int intWidth, int intHeight)
{
	//move the draw position
	danzeff_screen_rect.x = moved_x + screenX;
	danzeff_screen_rect.y = moved_y + screenY;

	//Set up the rectangle
	SDL_Rect pixels_rect;
	pixels_rect.x = offsetX;
	pixels_rect.y = offsetY;
	pixels_rect.w = intWidth;
	pixels_rect.h = intHeight;

	SDL_BlitSurface(pixels, &pixels_rect, danzeff_screen, &danzeff_screen_rect);
}

///Draw a surface at the current moved_x, moved_y
void surface_draw_sdl(SDL_Surface *pixels)
{
	surface_draw_offset(pixels, 0, 0, 0, 0, pixels->w, pixels->h);
}

/* load all the guibits that make up the OSK */
int danzeff_load()
{
	if (initialized)
		return 1;

	int a;
	for (a = 0; a < guiStringsSize; a++) {
		keyBits[a] = IMG_Load(guiStrings[a]);
		if (keyBits[a] == NULL) {
			//ERROR! out of memory.
			printf("Error! out of memory loading keyboard.\n");
			//free all previously created surfaces and set initialized to false
			int b;
			for (b = 0; b < a; b++) {
				SDL_FreeSurface(keyBits[b]);
				keyBits[b] = NULL;
			}
			initialized = false;
			return initialized;
		}
	}
	initialized = true;
	return initialized;
}

void danzeff_load_lite()
{
	if (initialized)
		return;

	int a;
	for (a = 0; a < guiStringsSize; a++) {
		if (!((a - 1) % 3)) //skip loading the _t files
		{
			keyBits[a] = NULL;
			continue;
		}
		keyBits[a] = IMG_Load(guiStrings[a]);
		if (keyBits[a] == NULL) {
			//ERROR! out of memory.
			printf("Error! out of memory loading keyboard.\n");
			//free all previously created surfaces and set initialized to false
			int b;
			for (b = 0; b < a; b++) {
				SDL_FreeSurface(keyBits[b]);
				keyBits[b] = NULL;
			}
			initialized = false;
			return;
		}
	}
	initialized = true;
}

/* remove all the guibits from memory */
void danzeff_free()
{
	if (!initialized)
		return;

	int a;
	for (a = 0; a < guiStringsSize; a++) {
		SDL_FreeSurface(keyBits[a]);
		keyBits[a] = NULL;
	}
	initialized = false;
}

/* draw the keyboard at the current position */
void danzeff_render(void (*PreRenderigFunction)(), void (*PostRenderigFunction)())
{
	if ((*PreRenderigFunction) != NULL) {
		(*PreRenderigFunction)();
	}
	printf("Drawing Keyboard at (%i,%i)\n", selected_x, selected_y);
	dirty = false;

	///Draw the background for the selected keyboard either transparent or opaque
	///this is the whole background image, not including the special highlighted area
	//if centered or loaded lite then draw the whole thing opaque
	if ((selected_x == 1 && selected_y == 1) || keyBits[6 * mode + shifted * 3 + 1] == NULL)
		surface_draw_sdl(keyBits[6 * mode + shifted * 3]);
	else
		surface_draw_sdl(keyBits[6 * mode + shifted * 3 + 1]);

	///Draw the current Highlighted Selector (orange bit)
	surface_draw_offset(keyBits[6 * mode + shifted * 3 + 2],
	    //Offset from the current draw position to render at
	    selected_x * 43, selected_y * 43,
	    //internal offset of the image
	    selected_x * 64, selected_y * 64,
	    //size to render (always the same)
	    64, 64);
	//SDL_Flip(danzeff_screen);
	if ((*PostRenderigFunction) != NULL) {
		(*PostRenderigFunction)();
	}
}

/* move the position the keyboard is currently drawn at */
void danzeff_moveTo(const int newX, const int newY)
{
	moved_x = newX;
	moved_y = newY;
}
