#ifndef INCLUDED_LIB_PSPCTRL_EMU_H 
#define INCLUDED_LIB_PSPCTRL_EMU_H

#include <SDL/SDL.h>

//This is a system to provide a crossplatform implementation of the controller
//so that psp programs can be coded on the pc using a ps2 convertor and then
//ran on the psp without doing any changes.

//#define PSP if you are building for PSP
//If you are building for PC then check pspctrl_emu.c and prepare a function for use on PC.

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enumeration for the digital controller buttons.
 *
 * @note PSP_CTRL_NOTE can only be read in kernel mode
 */
enum PspCtrlButtons
{
	/** Select button. */
	PSP_CTRL_SELECT     = 0x000001,
	/** Start button. */
	PSP_CTRL_START      = 0x000008,
	/** Up D-Pad button. */
	PSP_CTRL_UP         = 0x000010,
	/** Right D-Pad button. */
	PSP_CTRL_RIGHT      = 0x000020,
	/** Down D-Pad button. */
	PSP_CTRL_DOWN      	= 0x000040,
	/** Left D-Pad button. */
	PSP_CTRL_LEFT      	= 0x000080,
	/** Left trigger. */
	PSP_CTRL_LTRIGGER   = 0x000100,
	/** Right trigger. */
	PSP_CTRL_RTRIGGER   = 0x000200,
	/** Triangle button. */
	PSP_CTRL_TRIANGLE   = 0x001000,
	/** Circle button. */
	PSP_CTRL_CIRCLE     = 0x002000,
	/** Cross button. */
	PSP_CTRL_CROSS      = 0x004000,
	/** Square button. */
	PSP_CTRL_SQUARE     = 0x008000,
	/** Home button. */
	PSP_CTRL_HOME       = 0x010000,
	/** Hold button. */
	PSP_CTRL_HOLD       = 0x020000,
	/** Music Note button. */
	PSP_CTRL_NOTE       = 0x800000,
};


typedef struct SceCtrlData {
	/** The current read frame. */
	unsigned int 	TimeStamp;	//UNUSED
	/** Bit mask containing zero or more of ::PspCtrlButtons. */
	unsigned int 	Buttons;
	/** Analogue stick, X axis. */
	unsigned char 	Lx;
	/** Analogue stick, Y axis. */
	unsigned char 	Ly;
	/** Reserved. */
	unsigned char 	Rsrv[6]; //UNUSED
} SceCtrlData;

SceCtrlData getCtrlFromJoystick(SDL_Joystick* joystick);

#ifdef __cplusplus
}
#endif

#endif //INCLUDED_LIB_PSPCTRL_EMU_H
