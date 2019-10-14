#ifndef DEBUG_SCREEN_C
#define DEBUG_SCREEN_C

/*
* debugScreen.c of Vita SDK
*
* - psvDebugScreenInit()
*    Initializes debug screen for output.
*
* - psvDebugScreenPuts()
*    Similar to the C library function puts() writes a string to the debug
*    screen up to but not including the NUL character.
*    Supports the most important CSI sequences of ECMA-48 / ISO/IEC 6429:1992.
*    Graphic Rendition Combination Mode (GRCM) supported is Cumulative.
*    Modifications:
*    - CSI SGR codes 30-37/38/39 & 40-47/48/49 set standard/fitting/default intensity, so instead of "\e[1;31m" use "\e31;1m"
*    - ANSI color #8 is made darker (40<>80), so that "dark" white is still lighter than "bright" dark
*    - support 16 save storages for CSI s and CSI u, e.g "\e[8s" and "\e[8u"
*    [1] https://en.wikipedia.org/wiki/ANSI_escape_code#CSI_sequences
*    [2] https://jonasjacek.github.io/colors/
*    [3] https://www.ecma-international.org/publications/standards/Ecma-048.htm
*    [4] https://invisible-island.net/xterm/ctlseqs/ctlseqs.html
*    [5] http://man7.org/linux/man-pages/man4/console_codes.4.html
*
*    (CSI = "\e[")
*     CSI [n] s   = Save Cursor Position to slot #n (0-15). Default 0.
*     CSI [n] u   = Restore Cursor Position from slot #n (0-15). Default 0.
*     CSI n A     = Cursor Up <n> times.
*     CSI n B     = Cursor Down <n> times.
*     CSI n C     = Cursor Forward <n> times.
*     CSI n D     = Cursor Back <n> times.
*     CSI n E     = Cursor Next Line <n> times and to Beginning of that Line.
*     CSI n F     = Cursor Previous Line <n> times and to Beginning of that Line.
*     CSI n G     = Cursor to Column <n>. The value is 1-based and defaults to 1 (first column) if omitted.
*     CSI n ; m H = Cursor to Row <n> and Column <m>. The values are 1-based and default to 1 (top left corner) if omitted.
*     CSI n ; m f = Cursor to Row <n> and Column <m>. The values are 1-based and default to 1 (top left corner) if omitted.
*     CSI [n] J   = Clears part of the screen. Cursor position does not change.
*                   0 (default) from cursor to end of screen.
*                   1 from cursor to beginning of the screen.
*                   2 entire screen
*     CSI [n] K   = Clears part of the line. Cursor position does not change.
*                   0 (default) from cursor to end of line.
*                   1 from cursor to beginning of line.
*                   2 clear entire line.
*     CSI [n] m = Sets the appearance of the following characters.
*               0       Reset all (colors and inversion) (default)
*               1       Increased intensity ("bright" color)
*               2       Decreased intensity ("faint"/"dark" color)
*               7       Enable inversion
*               22      Standard intensity ("normal" color)
*               27      Disable inversion
*               30–37   Set ANSI foreground color with standard intensity
*               38      Set foreground color. Arguments are 5;<n> or 2;<r>;<g>;<b>
*               39      Default foreground color
*               40–47   Set standard ANSI background color with standard intensity
*               48      Set background color. Arguments are 5;<n> or 2;<r>;<g>;<b>
*               49      Default background color
*               90–97   Set ANSI foreground color with increased intensity
*               100–107 Set ANSI background color with increased intensity
*
* - psvDebugScreenPrintf()
*    Similar to the C library function printf() formats a string and ouputs
*    it via psvDebugScreenPuts() to the debug screen.
*
* - psvDebugScreenGetColorStateCopy(ColorState *copy)
*    Get copy of current color state.
*
* - psvDebugScreenGetCoordsXY(int *x, int *y)
*    Get copy of current pixel coordinates.
*    Allows for multiple and custom position stores.
*    Allows correct positioning when using different font sizes.
*
* - psvDebugScreenSetCoordsXY(int *x, int *y)
*    Set pixel coordinates.
*    Allows for multiple and custom position stores.
*    Allows correct positioning when using different font sizes.
*
* - PsvDebugScreenFont *psvDebugScreenGetFont()
*    Get current font.
*
* - PsvDebugScreenFont *psvDebugScreenSetFont(PsvDebugScreenFont *font) {
*    Set font. Returns current font.
*
* - PsvDebugScreenFont *psvDebugScreenScaleFont2x(PsvDebugScreenFont *source_font) {
*    Scales a font by 2 (e.g. 8x8 to 16x16) and returns new scaled font.
*
* Also see the following samples:
* - debugscreen
* - debug_print
*
*/

#include <stdlib.h> // for malloc(), free()
#include <stdio.h> // for vsnprintf()
#include <string.h> // for memset(), memcpy()
#include <stdarg.h> // for va_list, va_start(), va_end()
#include <inttypes.h>

#include "debugScreen.h"

#include "debugScreenFont.c"

#define SCREEN_FB_WIDTH (960) // frame buffer aligned width for accessing vram
#define SCREEN_FB_SIZE  (2 * 1024 * 1024) // Must be 256KB aligned
#ifndef SCREEN_TAB_SIZE // this allows easy overriding
#define SCREEN_TAB_SIZE (8)
#endif
#define SCREEN_TAB_W    ((F)->size_w * (SCREEN_TAB_SIZE))
#define F psvDebugScreenFontCurrent

#define FROM_FULL_RGB(r,g,b ) ( ((b)<<16) | ((g)<<8) | (r) )
#define CONVERT_RGB_BGR(rgb) rgb = ( (((rgb)&0x0000FF)<<16) | ((rgb)&0x00FF00) | (((rgb)&0xFF0000)>>16) )

#define CLEARSCRNBLOCK(H,toH,W,toW,color) for (int h = (H); h < (toH); h++) for (int w = (W); w < (toW); w++) ((uint32_t*)base)[h*(SCREEN_FB_WIDTH) + w] = (color);
#define CLEARSCRNLINES(H,toH,color) { uint32_t *pixel = (uint32_t *)base + ((H) * (SCREEN_FB_WIDTH)); int i = (((toH) - (H)) * (SCREEN_FB_WIDTH)); for (; i > 0; i--) *pixel++ = (color); }

#define SAVE_STORAGES 16

static int mutex, coordX, coordY;
static int savedX[SAVE_STORAGES] = { 0 }, savedY[SAVE_STORAGES] = { 0 };
static ColorState colors = {
	0, 0, // truecolor flags
	0, 0, // truecolors
	0, 0, 0, 0, 0, // ANSI/VTERM/GREYSCALE colors
	7, 22, 0, 22, 0, // default colors (ANSI/VTERM/GREYSCALE)
};

static PsvDebugScreenFont *psvDebugScreenFontCurrent = &psvDebugScreenFont;

#ifdef __vita__
#include <psp2/display.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/kernel/threadmgr.h>
static void* base; // pointer to frame buffer
#else
#define NO_psvDebugScreenInit
#ifndef psvDebugScreenInitReplacement
#define psvDebugScreenInitReplacement(...)
#endif
#define sceKernelLockMutex(m,v,x) m=v
#define sceKernelUnlockMutex(m,v) m=v
static char base[(SCREEN_FB_WIDTH) * (SCREEN_HEIGHT) * 4];
#endif

static uint32_t DARK_COLORS_BGR[8] = {
	0x000000, 0x000040, 0x004000, 0x004040, 0x400000, 0x400040, 0x404000, 0x808080, // 0-7
};

// ANSI/VTERM/GREYSCALE palette: https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit
// modifications:
// - #8 is made darker (40<>80), so that "dark" white is still lighter than "bright" dark
static uint32_t ANSI_COLORS_BGR[256] = {
	0x000000, 0x000080, 0x008000, 0x008080, 0x800000, 0x800080, 0x808000, 0xc0c0c0, // 0-7
	0x404040, 0x0000ff, 0x00ff00, 0x00ffff, 0xff0000, 0xff00ff, 0xffff00, 0xffffff, // 8-15
	0x000000, 0x5f0000, 0x870000, 0xaf0000, 0xd70000, 0xff0000, 0x005f00, 0x5f5f00, // 16-23
	0x875f00, 0xaf5f00, 0xd75f00, 0xff5f00, 0x008700, 0x5f8700, 0x878700, 0xaf8700, // 24-31
	0xd78700, 0xff8700, 0x00af00, 0x5faf00, 0x87af00, 0xafaf00, 0xd7af00, 0xffaf00, // 32-39
	0x00d700, 0x5fd700, 0x87d700, 0xafd700, 0xd7d700, 0xffd700, 0x00ff00, 0x5fff00, // 40-47
	0x87ff00, 0xafff00, 0xd7ff00, 0xffff00, 0x00005f, 0x5f005f, 0x87005f, 0xaf005f, // 48-55
	0xd7005f, 0xff005f, 0x005f5f, 0x5f5f5f, 0x875f5f, 0xaf5f5f, 0xd75f5f, 0xff5f5f, // 56-63
	0x00875f, 0x5f875f, 0x87875f, 0xaf875f, 0xd7875f, 0xff875f, 0x00af5f, 0x5faf5f, // 64-71
	0x87af5f, 0xafaf5f, 0xd7af5f, 0xffaf5f, 0x00d75f, 0x5fd75f, 0x87d75f, 0xafd75f, // 72-79
	0xd7d75f, 0xffd75f, 0x00ff5f, 0x5fff5f, 0x87ff5f, 0xafff5f, 0xd7ff5f, 0xffff5f, // 80-87
	0x000087, 0x5f0087, 0x870087, 0xaf0087, 0xd70087, 0xff0087, 0x005f87, 0x5f5f87, // 88-95
	0x875f87, 0xaf5f87, 0xd75f87, 0xff5f87, 0x008787, 0x5f8787, 0x878787, 0xaf8787, // 96-103
	0xd78787, 0xff8787, 0x00af87, 0x5faf87, 0x87af87, 0xafaf87, 0xd7af87, 0xffaf87, // 104-111
	0x00d787, 0x5fd787, 0x87d787, 0xafd787, 0xd7d787, 0xffd787, 0x00ff87, 0x5fff87, // 112-119
	0x87ff87, 0xafff87, 0xd7ff87, 0xffff87, 0x0000af, 0x5f00af, 0x8700af, 0xaf00af, // 120-127
	0xd700af, 0xff00af, 0x005faf, 0x5f5faf, 0x875faf, 0xaf5faf, 0xd75faf, 0xff5faf, // 128-135
	0x0087af, 0x5f87af, 0x8787af, 0xaf87af, 0xd787af, 0xff87af, 0x00afaf, 0x5fafaf, // 136-143
	0x87afaf, 0xafafaf, 0xd7afaf, 0xffafaf, 0x00d7af, 0x5fd7af, 0x87d7af, 0xafd7af, // 144-151
	0xd7d7af, 0xffd7af, 0x00ffaf, 0x5fffaf, 0x87ffaf, 0xafffaf, 0xd7ffaf, 0xffffaf, // 152-159
	0x0000d7, 0x5f00d7, 0x8700d7, 0xaf00d7, 0xd700d7, 0xff00d7, 0x005fd7, 0x5f5fd7, // 160-167
	0x875fd7, 0xaf5fd7, 0xd75fd7, 0xff5fd7, 0x0087d7, 0x5f87d7, 0x8787d7, 0xaf87d7, // 168-175
	0xd787d7, 0xff87d7, 0x00afd7, 0x5fafd7, 0x87afd7, 0xafafd7, 0xd7afd7, 0xffafd7, // 176-183
	0x00d7d7, 0x5fd7d7, 0x87d7d7, 0xafd7d7, 0xd7d7d7, 0xffd7d7, 0x00ffd7, 0x5fffd7, // 184-191
	0x87ffd7, 0xafffd7, 0xd7ffd7, 0xffffd7, 0x0000ff, 0x5f00ff, 0x8700ff, 0xaf00ff, // 192-199
	0xd700ff, 0xff00ff, 0x005fff, 0x5f5fff, 0x875fff, 0xaf5fff, 0xd75fff, 0xff5fff, // 200-207
	0x0087ff, 0x5f87ff, 0x8787ff, 0xaf87ff, 0xd787ff, 0xff87ff, 0x00afff, 0x5fafff, // 208-215
	0x87afff, 0xafafff, 0xd7afff, 0xffafff, 0x00d7ff, 0x5fd7ff, 0x87d7ff, 0xafd7ff, // 216-223
	0xd7d7ff, 0xffd7ff, 0x00ffff, 0x5fffff, 0x87ffff, 0xafffff, 0xd7ffff, 0xffffff, // 224-231
	0x080808, 0x121212, 0x1c1c1c, 0x262626, 0x303030, 0x3a3a3a, 0x444444, 0x4e4e4e, // 232-239
	0x585858, 0x626262, 0x6c6c6c, 0x767676, 0x808080, 0x8a8a8a, 0x949494, 0x9e9e9e, // 240-247
	0xa8a8a8, 0xb2b2b2, 0xbcbcbc, 0xc6c6c6, 0xd0d0d0, 0xdadada, 0xe4e4e4, 0xeeeeee, // 248-255
};

/*
* Reset foreground color to default
*/
static void psvDebugScreenResetFgColor(void) {
	colors.fgTrueColorFlag = 0;
	colors.fgTrueColor = 0;
	colors.fgIndex = colors.fgIndexDefault;
	colors.fgIntensity = colors.fgIntensityDefault;
}

/*
* Reset background color to default
*/
static void psvDebugScreenResetBgColor(void) {
	colors.bgTrueColorFlag = 0;
	colors.bgTrueColor = 0;
	colors.bgIndex = colors.bgIndexDefault;
	colors.bgIntensity = colors.bgIntensityDefault;
}

/*
* Reset inversion state to default
*/
static void psvDebugScreenResetInversion(void) {
	colors.inversion = colors.inversionDefault;
}

/*
* Determine colors according to current color state
*/
static void psvDebugScreenSetColors(void) {
	uint32_t *color_fg, *color_bg;

	// special case: inversion
	if (!colors.inversion) {
		color_fg = &colors.color_fg;
		color_bg = &colors.color_bg;
	} else {
		color_fg = &colors.color_bg;
		color_bg = &colors.color_fg;
	}

	// foregound color
	if ((colors.fgIndex<=7) && (colors.fgIntensity==1)) { // ANSI palette with increased intensity
		colors.fgIndex |= 0x8;
	} else if ((colors.fgIndex<=15) && (colors.fgIntensity!=1)) { // ANSI palette with standard/decreased intensity
		colors.fgIndex &= 0x7;
	}
	if (colors.fgTrueColorFlag) {
		*color_fg = colors.fgTrueColor;
	} else {
		if ((colors.fgIndex<=7) && (colors.fgIntensity==2)) { // "ANSI" palette with decreased intensity
			*color_fg = DARK_COLORS_BGR[colors.fgIndex];
		} else { // ANSI/VTERM/GREYSCALE palette
			*color_fg = ANSI_COLORS_BGR[colors.fgIndex];
		}
	}
	*color_fg |= 0xFF000000; // opaque

	// backgound color
	if ((colors.bgIndex<=7) && (colors.bgIntensity==1)) { // ANSI palette with increased intensity
		colors.bgIndex |= 0x8;
	} else if ((colors.bgIndex<=15) && (colors.bgIntensity!=1)) { // ANSI palette with standard/decreased intensity
		colors.bgIndex &= 0x7;
	}
	if (colors.bgTrueColorFlag) {
		*color_bg = colors.bgTrueColor;
	} else {
		if ((colors.bgIndex<=7) && (colors.bgIntensity==2)) { // "ANSI" palette with decreased intensity
			*color_bg = DARK_COLORS_BGR[colors.bgIndex];
		} else { // ANSI/VTERM/GREYSCALE palette
			*color_bg = ANSI_COLORS_BGR[colors.bgIndex];
		}
	}
	*color_bg |= 0xFF000000; // opaque
}

/*
* Parse CSI sequences
*/
static size_t psvDebugScreenEscape(const unsigned char *str) {
	unsigned int i, argc, arg[32] = { 0 };
	unsigned int c;
	uint32_t unit, mode;
	int *colorTrueColorFlag;
	uint32_t *colorTrueColor;
	unsigned char *colorIndex, *colorIntensity;
	for (i = 0, argc = 0; (argc < (sizeof(arg)/sizeof(*arg))) && (str[i] != '\0'); i++) {
		switch (str[i]) {
			// numeric char
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				arg[argc] = (arg[argc] * 10) + (str[i] - '0');
				continue;
			// argument separator
			case ';': argc++; continue;
			// CSI commands
			// save/restore position
			case 's':
				if (arg[0]<SAVE_STORAGES) { savedX[arg[0]] = coordX; savedY[arg[0]] = coordY; }
				return i;
			case 'u':
				if (arg[0]<SAVE_STORAGES) { coordX = savedX[arg[0]]; coordY = savedY[arg[0]]; }
				return i;
			// cursor movement
			case 'A': coordY -= arg[0]    * (F)->size_h; return i;
			case 'B': coordY += arg[0]    * (F)->size_h; return i;
			case 'C': coordX += arg[0]    * (F)->size_w; return i;
			case 'D': coordX -= arg[0]    * (F)->size_w; return i;
			// cursor movement to beginning of next/previous line(s)
			case 'E': coordY += arg[0]    * (F)->size_h; coordX = 0; return i;
			case 'F': coordY -= arg[0]    * (F)->size_h; coordX = 0; return i;
			// cursor positioning
			case 'G': coordX = (arg[0]-1) * (F)->size_w; return i;
			case 'H':
			case 'f':
				coordY = (arg[0]-1) * (F)->size_h;
				coordX = (arg[1]-1) * (F)->size_w;
				return i;
			// clear part of "J"=screen or "K"=Line, so J code re-uses part of K
			case 'J':
			case 'K':
				if (arg[0]==0) { // from cursor to end of line/screen
					CLEARSCRNBLOCK(coordY, coordY + (F)->size_h, coordX, (SCREEN_WIDTH), colors.color_bg); // line
					if (str[i]=='J') CLEARSCRNLINES(coordY + (F)->size_h, (SCREEN_HEIGHT), colors.color_bg); // screen
				} else if (arg[0]==1) { // from beginning of line/screen to cursor
					CLEARSCRNBLOCK(coordY, coordY + (F)->size_h, 0, coordX, colors.color_bg); // line
					if (str[i]=='J') CLEARSCRNLINES(0, coordY, colors.color_bg); // screen
				} else if (arg[0]==2) { // whole line/screen
					if (str[i]=='K') CLEARSCRNLINES(coordY, coordY + (F)->size_h, colors.color_bg) // line
					else if (str[i]=='J') CLEARSCRNLINES(0, (SCREEN_HEIGHT), colors.color_bg); // screen
				}
				return i;
			// color
			case 'm':
				for (c = 0; c <= argc; c++) {
					switch (arg[c]) {
						// reset all
						case 0:
							psvDebugScreenResetFgColor();
							psvDebugScreenResetBgColor();
							psvDebugScreenResetInversion();
							continue;
							break;
						// intensity
						case 1: // increased = "bright" color
						case 2: // decreased = "dark" color
						case 22: // standard = "normal" color
							colors.fgIntensity = arg[c];
							continue;
							break;
						// inversion
						case 7: // enable
							colors.inversion = 1;
							continue;
							break;
						case 27: // disable
							colors.inversion = 0;
							continue;
							break;
						// set from color map or truecolor
						case 38: // foreground color
						case 48: // background color
							mode = arg[c] / 10;
							colorTrueColorFlag = mode&1 ? &colors.fgTrueColorFlag : &colors.bgTrueColorFlag;
							if (arg[c+1]==5) { // 8-bit: [0-15][16-231][232-255] color map
								*colorTrueColorFlag = 0;
								colorIndex = mode&1 ? &colors.fgIndex : &colors.bgIndex;
								*colorIndex = arg[c+2] & 0xFF;
								colorIntensity = mode&1 ? &colors.fgIntensity : &colors.bgIntensity;
								*colorIntensity = ((*colorIndex>=8) && (*colorIndex<=15)) ? 1 : 22;
								c+=2; // extra arguments
							} else if (arg[c+1]==2) { // 24-bit color space
								*colorTrueColorFlag = 1;
								colorTrueColor = mode&1 ? &colors.fgTrueColor : &colors.bgTrueColor;
								*colorTrueColor = FROM_FULL_RGB(arg[c+2], arg[c+3], arg[c+4]);
								c+=4; // extra arguments
							}
							continue;
							break;
						// default color
						case 39: // foreground color
							psvDebugScreenResetFgColor();
							continue;
							break;
						case 49: // background color
							psvDebugScreenResetBgColor();
							continue;
							break;
						// custom color reset
						default:
							// ANSI colors (30-37, 40-47, 90-97, 100-107)
							mode = arg[c] / 10;
							if ((mode!=3) && (mode!=4) && (mode!=9) && (mode!=10)) continue; // skip unsupported modes
							unit = arg[c] % 10;
							if (unit>7) continue; // skip unsupported modes
							colorTrueColorFlag = mode&1 ? &colors.fgTrueColorFlag : &colors.bgTrueColorFlag;
							*colorTrueColorFlag = 0;
							colorIndex = mode&1 ? &colors.fgIndex : &colors.bgIndex;
							*colorIndex = unit;
							colorIntensity = mode&1 ? &colors.fgIntensity : &colors.bgIntensity;
							*colorIntensity = mode&8 ? 1 : 22;
							break;
					}
				}
				psvDebugScreenSetColors();
				return i;
		}
	}
	return 0;
}

/*
* Initialize debug screen
*/
int psvDebugScreenInit() {
	psvDebugScreenResetFgColor();
	psvDebugScreenResetBgColor();
	psvDebugScreenResetInversion();
	psvDebugScreenSetColors();

#ifdef NO_psvDebugScreenInit
	psvDebugScreenInitReplacement();
	return 0; // avoid linking non-initializer (prx) with sceDisplay/sceMemory
#else
	mutex = sceKernelCreateMutex("log_mutex", 0, 0, NULL);
	SceUID displayblock = sceKernelAllocMemBlock("display", SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW, (SCREEN_FB_SIZE), NULL);
	sceKernelGetMemBlockBase(displayblock, (void**)&base);
	SceDisplayFrameBuf frame = { sizeof(frame), base, (SCREEN_FB_WIDTH), 0, (SCREEN_WIDTH), (SCREEN_HEIGHT) };
	return sceDisplaySetFrameBuf(&frame, SCE_DISPLAY_SETBUF_NEXTFRAME);
#endif
}

/*
* Draw text onto debug screen
*/
int psvDebugScreenPuts(const char * _text) {
	const unsigned char*text = (const unsigned char*)_text;
	int c;
	unsigned char t;
	unsigned char drawDummy;
	//
	uint32_t *vram;
	int bits_per_glyph = ((F)->width * (F)->height);
	int bitmap_offset;
	unsigned char *font;
	int row;
	int max_row;
	int col;
	unsigned char mask;
	uint32_t *pixel;

	sceKernelLockMutex(mutex, 1, NULL);
	for (c = 0; text[c] ; c++) {
		t = text[c];
		// handle CSI sequence
		if ((t == '\e') && (text[c+1] == '[')) {
			c += psvDebugScreenEscape(text + c + 2) + 2;
			if (coordX < 0) coordX = 0; // CSI position are 1-based,
			if (coordY < 0) coordY = 0; // prevent 0-based coordinate from producing a negative X/Y
			continue;
		}
		// handle non-printable characters #1 (line-dependent codes)
		if (t == '\n') {
			coordX = 0;
			coordY += (F)->size_h;
			continue;
		}
		if (t == '\r') {
			coordX = 0;
			continue;
		}
		// check if glyph fits in line
		if ((coordX + (F)->width) > (SCREEN_WIDTH)) {
			coordY += (F)->size_h;
			coordX = 0;
		}
		// check if glyph fits in screen
		if ((coordY + (F)->height) > (SCREEN_HEIGHT)) {
			coordX = coordY = 0;
		}
		// handle non-printable characters #2
		if (t == '\t') {
			coordX += (SCREEN_TAB_W) - (coordX % (SCREEN_TAB_W));
			continue;
		}

		// draw glyph or dummy glyph (dotted line in the middle)
		// works also with not byte-aligned glyphs
		vram = ((uint32_t*)base) + coordX + (coordY * (SCREEN_FB_WIDTH));
		row = 0;
		// check if glyph is available in font
		if ((t > (F)->last) || (t < (F)->first)) {
			drawDummy = 1;
		} else {
			drawDummy = 0;
			bitmap_offset = (t - (F)->first) * bits_per_glyph;
			font = &(F)->glyphs[ (bitmap_offset / 8) ];
			mask = 1 << 7;
			for (col = (bitmap_offset % 8); col > 0; col--, mask >>= 1);
		}
		// special case: dummy glyph, clear to middle height
		max_row = 0;
		if (drawDummy) {
			max_row = (F)->height / 2;
			for (; row < max_row; row++, vram += (SCREEN_FB_WIDTH)) {
				pixel = vram;
				col = 0;
				for (; col < (F)->size_w ; col++) {
					*pixel++ = colors.color_bg;
				}
			}
		}
		// draw font glyph or dummy glyph
		if (drawDummy) {
			max_row++;
			if (max_row > (F)->height) max_row = (F)->height;
		} else {
			max_row = (F)->height;
		}
		for (; row < max_row; row++, vram += (SCREEN_FB_WIDTH)) {
			pixel = vram;
			col = 0;
			for (; col < (F)->width ; col++, mask >>= 1) {
				if (drawDummy) {
					*pixel++ = (col&1) ? colors.color_fg : colors.color_bg;
				} else {
					if (!mask) { font++; mask = 1 << 7; } // no more bits: we exhausted this byte
					*pixel++ = (*font&mask) ? colors.color_fg : colors.color_bg;
				}
			}
			// right margin
			for (; col < (F)->size_w ; col++)
				*pixel++ = colors.color_bg;
		}
		// draw bottom margin
		max_row = (F)->size_h;
		for (; row < (F)->size_h; row++, vram += (SCREEN_FB_WIDTH))
			for (pixel = vram, col = 0; col < (F)->size_w ; col++)
				*pixel++ = colors.color_bg;
		// advance X position
		coordX += (F)->size_w;
	}
	sceKernelUnlockMutex(mutex, 1);
	return c;
}


/*
* Printf text onto debug screen
*/
__attribute__((__format__ (__printf__, 1, 2)))
int psvDebugScreenPrintf(const char *format, ...) {
	char buf[4096];

	va_list opt;
	va_start(opt, format);
	int ret = vsnprintf(buf, sizeof(buf), format, opt);
	psvDebugScreenPuts(buf);
	va_end(opt);

	return ret;
}

/*
* Return copy of color state
*/
void psvDebugScreenGetColorStateCopy(ColorState *copy) {
	if (copy) {
		memcpy(copy, &colors, sizeof(ColorState));
		CONVERT_RGB_BGR(copy->fgTrueColor);
		CONVERT_RGB_BGR(copy->bgTrueColor);
		CONVERT_RGB_BGR(copy->color_fg);
		CONVERT_RGB_BGR(copy->color_bg);
	}
}

/*
* Return copy of pixel coordinates
*/
void psvDebugScreenGetCoordsXY(int *x, int *y) {
	if (x) *x = coordX;
	if (y) *y = coordY;
}

/*
* Set pixel coordinates
*/
void psvDebugScreenSetCoordsXY(int *x, int *y) {
	if (x) {
		coordX = *x;
		if (coordX < 0) coordX = 0;
	}
	if (y) {
		coordY = *y;
		if (coordY < 0) coordY = 0;
	}
}

/*
* Return pointer to current font
*/
PsvDebugScreenFont *psvDebugScreenGetFont(void) {
	return F;
}

/*
* Set font
*/
PsvDebugScreenFont *psvDebugScreenSetFont(PsvDebugScreenFont *font) {
	if ((font) && (font->glyphs)) F = font;
	return F;
}

/*
* Return scaled-by-2 copy of font
*/
PsvDebugScreenFont *psvDebugScreenScaleFont2x(PsvDebugScreenFont *source_font) {
	// works also with not byte-aligned glyphs
	PsvDebugScreenFont *target_font;
	size_t size;
	size_t align;
	int glyph;
	int row;
	int col;
	int count;
	unsigned char *source_bitmap;
	unsigned char source_mask;
	unsigned char *target_bitmap, *target_bitmap2;
	unsigned char target_mask, target_mask2;
	int target_next_row_bytes, target_next_row_bits;
	unsigned char pixel;

	if (!source_font) return NULL;

	// allocate target structure and bitmap
	target_font = (PsvDebugScreenFont *)malloc(sizeof(PsvDebugScreenFont));
	memset(target_font, 0, sizeof(PsvDebugScreenFont));
	// copy and scale meta information
	target_font->width = 2 * source_font->width;
	target_font->height = 2 * source_font->height;
	target_font->first = source_font->first;
	target_font->last = source_font->last;
	target_font->size_w = 2 * source_font->size_w;
	target_font->size_h = 2 * source_font->size_h;

	// calculate size of target bitmap
	size = target_font->width * target_font->height * (target_font->last - target_font->first + 1);
	if (size <= 0) {
		free(target_font);
		return NULL;
	}
	align = size % 8;
	size /= 8;
	if (align) size++;

	// allocate and initialize target bitmap
	target_font->glyphs = (unsigned char *)malloc(size);
	memset(target_font->glyphs, 0, size);

	// scale source bitmap and store in target bitmap
	source_bitmap = source_font->glyphs;
	source_mask = 1 << 7;
	//
	target_bitmap = target_font->glyphs;
	target_mask = 1 << 7;
	target_next_row_bytes = target_font->width / 8;
	target_next_row_bits = target_font->width % 8;
	//
	for (glyph = source_font->first; glyph <= source_font->last; glyph++) {
		for (row = source_font->height; row > 0; row--) {
			// Find beginning of next target row
			target_bitmap2 = target_bitmap + target_next_row_bytes; // advance full bytes
			target_mask2 = target_mask; // advance remaining bits
			for (col = target_next_row_bits; col > 0; col--, target_mask2 >>= 1) {
				if (!target_mask2) { target_bitmap2++; target_mask2 = 1 << 7; } // no more bits: we advance to the next target byte
			}
			// Get pixel from source bitmap
			for (col = source_font->width; col > 0; col--, source_mask >>= 1) {
				if (!source_mask) { source_bitmap++; source_mask = 1 << 7; } // no more bits: we advance to the next source byte
				pixel = *source_bitmap & source_mask;
				// Put pixels into target bitmap
				for (count = 2; count > 0; count--) {
					// duplicate column in origial row
					if (!target_mask) { target_bitmap++; target_mask = 1 << 7; } // no more bits: we advance to the next target byte
					if (pixel) *target_bitmap |= target_mask;
					target_mask >>= 1;
					// duplicate column in duplicated row
					if (!target_mask2) { target_bitmap2++; target_mask2 = 1 << 7; } // no more bits: we advance to the next target byte
					if (pixel) *target_bitmap2 |= target_mask2;
					target_mask2 >>= 1;
				}
			}
			// Next target row is directly behind duplicated row
			target_bitmap = target_bitmap2;
			target_mask = target_mask2;
		}
	}

	return target_font;
}

#undef SCREEN_TAB_W
#undef F

#endif
