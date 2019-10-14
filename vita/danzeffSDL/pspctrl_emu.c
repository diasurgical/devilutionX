#include "pspctrl_emu.h"

#ifdef PSP
SceCtrlData getCtrlFromJoystick(SDL_Joystick* joystick)
{
	SceCtrlData newData;
	newData.Buttons = 0;
	newData.Lx = 0;
	newData.Ly = 0;
	
	SDL_JoystickUpdate();

	//Buttons
	if (SDL_JoystickGetButton(joystick, 0)) newData.Buttons |= PSP_CTRL_TRIANGLE;
	if (SDL_JoystickGetButton(joystick, 1)) newData.Buttons |= PSP_CTRL_CIRCLE;
	if (SDL_JoystickGetButton(joystick, 2)) newData.Buttons |= PSP_CTRL_CROSS;
	if (SDL_JoystickGetButton(joystick, 3)) newData.Buttons |= PSP_CTRL_SQUARE;

	//Digital
	if (SDL_JoystickGetButton(joystick, 6)) newData.Buttons |= PSP_CTRL_DOWN;
	if (SDL_JoystickGetButton(joystick, 7)) newData.Buttons |= PSP_CTRL_LEFT;
	if (SDL_JoystickGetButton(joystick, 8)) newData.Buttons |= PSP_CTRL_UP;
	if (SDL_JoystickGetButton(joystick, 9)) newData.Buttons |= PSP_CTRL_RIGHT;

	//L R
	if (SDL_JoystickGetButton(joystick, 4)) newData.Buttons |= PSP_CTRL_LTRIGGER;
	if (SDL_JoystickGetButton(joystick, 5)) newData.Buttons |= PSP_CTRL_RTRIGGER;

	//Start Select
	if (SDL_JoystickGetButton(joystick, 11)) newData.Buttons |= PSP_CTRL_START;
	if (SDL_JoystickGetButton(joystick, 10)) newData.Buttons |= PSP_CTRL_SELECT;
	
	//Analog
	newData.Lx = (SDL_JoystickGetAxis(joystick, 0) / 256) + 128;
	newData.Ly = (SDL_JoystickGetAxis(joystick, 1) / 256) + 128;
	
	return newData;
}
#else //not psp

//This has been configured according to my smartjoy adaptor in linux, YMMV.
//You could set this up to work from a keyboard if you wanted!
SceCtrlData getCtrlFromJoystick(SDL_Joystick* joystick)
{
	SceCtrlData newData;
	newData.Buttons = 0;
	newData.Lx = 0;
	newData.Ly = 0;
	
	SDL_JoystickUpdate();
	
	//Buttons
	if (SDL_JoystickGetButton(joystick, 0)) newData.Buttons |= PSP_CTRL_TRIANGLE;
	if (SDL_JoystickGetButton(joystick, 1)) newData.Buttons |= PSP_CTRL_CIRCLE;
	if (SDL_JoystickGetButton(joystick, 2)) newData.Buttons |= PSP_CTRL_CROSS;
	if (SDL_JoystickGetButton(joystick, 3)) newData.Buttons |= PSP_CTRL_SQUARE;
	
	//Start/Select
	if (SDL_JoystickGetButton(joystick, 8)) newData.Buttons |= PSP_CTRL_START;
	if (SDL_JoystickGetButton(joystick, 9)) newData.Buttons |= PSP_CTRL_SELECT;
	
	
	//L/R (MERGE THEM)
	if (SDL_JoystickGetButton(joystick, 4)) newData.Buttons |= PSP_CTRL_LTRIGGER;
	if (SDL_JoystickGetButton(joystick, 6)) newData.Buttons |= PSP_CTRL_LTRIGGER;
	if (SDL_JoystickGetButton(joystick, 5)) newData.Buttons |= PSP_CTRL_RTRIGGER;
	if (SDL_JoystickGetButton(joystick, 7)) newData.Buttons |= PSP_CTRL_RTRIGGER;

	//Left digi
	int L_DIGI_X = SDL_JoystickGetAxis(joystick, 4);
	if (L_DIGI_X > 0) newData.Buttons |= PSP_CTRL_RIGHT; else if (L_DIGI_X < 0) newData.Buttons |= PSP_CTRL_LEFT;
	
	int L_DIGI_Y = SDL_JoystickGetAxis(joystick, 5);
	if (L_DIGI_Y > 0) newData.Buttons |= PSP_CTRL_DOWN; else if (L_DIGI_Y < 0) newData.Buttons |= PSP_CTRL_UP;
	
	//ANALOG
	newData.Lx = (SDL_JoystickGetAxis(joystick, 0) / 256) + 128;
	newData.Ly = (SDL_JoystickGetAxis(joystick, 1) / 256) + 128;

	return newData;
}
#endif
