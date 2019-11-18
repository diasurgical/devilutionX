#include <string>
#include <SDL.h>

#ifdef PLATFORM_CTR
#include <3ds.h>
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

int main(int argc, char **argv)
{
#ifdef PLATFORM_CTR
	bool isN3DS;

	APT_CheckNew3DS(&isN3DS);
	if(isN3DS)
		osSetSpeedupEnable(true);
	
	romfsInit();
	atexit(romfsExit());
	
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO ) != 0) {
		printf("Failed to start SDL!\n");
		return -1;
	}
	SDL_Surface *screen;
	screen = SDL_SetVideoMode(400, 240, 0, SDL_SWSURFACE | SDL_CONSOLEBOTTOM); //Init console
	printf("SDL Initialized!\n");
#endif
	
	auto cmdline = build_cmdline(argc, argv);
	return dvl::WinMain(NULL, NULL, (char *)cmdline.c_str(), 0);

}
