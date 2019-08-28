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

#ifdef __ANDROID__
int SDL_main(int argc, char **argv)
{
	// I am placing this here becasue I don't want to stub another file.
	const int dir_err = system("mkdir -p /sdcard/devilutionx");
	if (-1 == dir_err) {
		printf("Error creating directory!n");
		exit(1);
	}

	//org.diasurgical.devilutionx
	const int dir_errx = system("mkdir -p /sdcard/Android/data/org.diasurgical.devilutionx");
	if (-1 == dir_errx) {
		printf("Error creating directory!n");
		exit(1);
	}

#else
int main(int argc, char **argv)
{
#endif
	auto cmdline = build_cmdline(argc, argv);
	return dvl::WinMain(NULL, NULL, (char *)cmdline.c_str(), 0);
}
