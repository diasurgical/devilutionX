#include <string>

#ifdef VITA
#ifdef USE_SDL1
#include <SDL/SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include "../vita/vita_aux_util.h"
#include <vitasdk.h>
#else
#include <SDL.h>
#endif

#include "devilution.h"

#if !defined(__APPLE__)
extern "C" const char *__asan_default_options()
{
	return "halt_on_error=0";
}
#endif

static std::string build_cmdline(int argc, char **argv)
{
	std::string str;
	for (int i = 1; i < argc; i++) {
		if (i != 1) {
			str += ' ';
		}
		str += argv[i];
	}
	return str;
}

#ifdef VITA
extern void IN_Init(void *windowData);

int devilution_main(unsigned int argc, void *argv)
#else
int main(int argc, char **argv)
#endif
{
	auto cmdline = build_cmdline(argc, (char **)argv);
	return dvl::WinMain(NULL, NULL, (char *)cmdline.c_str(), 0);
}
#ifdef VITA
int main(int argc, char **argv)
{

	// Starting input
	//IN_Init(NULL);
	VitaAux::init();
	// Setting maximum clocks
	scePowerSetArmClockFrequency(444);
	scePowerSetBusClockFrequency(222);
	scePowerSetGpuClockFrequency(222);
	scePowerSetGpuXbarClockFrequency(166);
#ifndef USE_SDL1
	VitaAux::dialog("For now, with SDL2 and fullscreen mode\nthe performace of the game is very poor\n\
	Please use USE_SDL1 flag on Makefile.list in ON to solve this.\n\n(or better fix my code ;)",
	    "SDL2 performace problem", false, false, true);
#endif
	// We need a bigger stack to run Diablo, so we create a new thread with a proper stack size
	SceUID main_thread
	    = sceKernelCreateThread("devilutionX", devilution_main, 0x40, 0x7000000, 0, 0, NULL);
	if (main_thread >= 0) {
		sceKernelStartThread(main_thread, 0, NULL);
		sceKernelWaitThreadEnd(main_thread, NULL, NULL);
	} else {
		VitaAux::dialog("Can't create main thread!", "Error creation and launch MainThread", true, true, true);
	}
	return 0;
}
#endif
