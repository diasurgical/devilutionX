#include <string>
#include <SDL.h>

#include "devilution.h"

#ifdef __ANDROID__
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fstream>
#endif


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
#define HAVE_NEON_INTRINSICS 0
#ifdef HAVE_NEON_INTRINSICS //AUDIO FIX FOR SDL
#undef HAVE_NEON_INTRINSICS //un-define it
#define HAVE_NEON_INTRINSICS 0//redefine it with the new value
#endif






inline bool FileExists (const std::string& name) {
	struct stat buffer;
	return (stat (name.c_str(), &buffer) == 0);
}



int SDL_main(int argc, char* argv[]) {
// I am placing this here becasue I don't want to stub another file.




//This *CAN* be better. I understand.
if ( FileExists("/sdcard/devilutionx") == false) {
	const int dir_errz = system("mkdir -p /sdcard/devilutionx");
	if (-1 == dir_errz) {
//		printf("Error creating directory!\n");
		exit(1);
	}
}

if ( FileExists("/sdcard/devilutionx/diabdat.mpq") == false) {
    if ( FileExists("/sdcard/diabdat.mpq") == false){system("cp  spawn.mpq /sdcard/devilutionx/");  }
        const int dir_errbz = system("cp  /sdcard/diabdat.mpq /sdcard/devilutionx/");
	if (-1 == dir_errbz) {
//		printf("Error copying diabdat.mpq \n");
		exit(1);
	}
}


//org.diasurgical.devilutionx
if ( FileExists("/sdcard/Android/data/org.diasurgical.devilutionx") == false) {
	const int dir_errn = system("mkdir -p /sdcard/Android/data/org.diasurgical.devilutionx");
	if (-1 == dir_errn) {
//		printf("Error creating directory!\n");
		exit(1);
	}
}


	return dvl::WinMain(NULL, NULL, (char *)"", 0);
}
#else
int main(int argc, char **argv)
{
	auto cmdline = build_cmdline(argc, argv);
	return dvl::WinMain(NULL, NULL, (char *)cmdline.c_str(), 0);
}
#endif

