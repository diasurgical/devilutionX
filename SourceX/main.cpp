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
		printf("Error creating directory!\n");
		exit(1);
	}
}

// Copy images if they exist --
if ( FileExists("/sdcard/devilutionx/dpad.png") == false) {
	const int dir_errx = system("cp  /sdcard/dpad.png /sdcard/devilutionx/");
	if (-1 == dir_errx) {
		printf("Error copying dpad.png !\n");
		exit(1);
	}
}

if ( FileExists("/sdcard/devilutionx/shift.png") == false) {
	const int dir_errc = system("cp  /sdcard/shift.png /sdcard/devilutionx/");
	if (-1 == dir_errc) {
		printf("Error copying shift.png\n");
		exit(1);
	}
}
if ( FileExists("/sdcard/devilutionx/demosq.png") == false) {
	const int dir_errv = system("cp  /sdcard/demosq.png /sdcard/devilutionx/");
	if (-1 == dir_errv) {
		printf("Error copying demosq.png\n");
		exit(1);
	}
}
if ( FileExists("/sdcard/devilutionx/input_attack.png") == false) {
	const int dir_errb = system("cp  /sdcard/input_attack.png /sdcard/devilutionx/");
	if (-1 == dir_errb) {
		printf("Error copying input_attack.png\n");
		exit(1);
	}
}

if ( FileExists("/sdcard/devilutionx/input_cast.png/") == false) {
	const int dir_errcc = system("cp  /sdcard/input_cast.png /sdcard/devilutionx/");
	if (-1 == dir_errcc) {
		printf("Error copying input_attack.png\n");
		exit(1);
	}
}

if ( FileExists("/sdcard/devilutionx/diabdat.mpq") == false) {
	const int dir_errbz = system("cp  /sdcard/diabdat.mpq /sdcard/devilutionx/");
	if (-1 == dir_errbz) {
		printf("Error copying diabdat.mpq \n");
		exit(1);
	}
}

//org.diasurgical.devilutionx
if ( FileExists("/sdcard/Android/data/org.diasurgical.devilutionx") == false) {
	const int dir_errn = system("mkdir -p /sdcard/Android/data/org.diasurgical.devilutionx");
	if (-1 == dir_errn) {
		printf("Error creating directory!\n");
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

