#include <string>
#include <SDL.h>

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

void quit(){
	static bool completed = false;
	if(completed) return;
	completed = true;
	dvl::init_cleanup();
	dvl::SaveGamma();
	SDL_Quit();
}

int main(int argc, char **argv)
{
	atexit(quit);
	auto cmdline = build_cmdline(argc, argv);
	int result = dvl::WinMain(NULL, NULL, (char *)cmdline.c_str(), 0);
	quit();
	return result;
}
