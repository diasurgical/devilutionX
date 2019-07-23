#include <string>
#include <SDL.h>
#ifdef SWITCH
#include <switch.h>
static int nxlink_sock = -1; // for stdio on Switch
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
	auto cmdline = build_cmdline(argc, argv);

#ifdef SWITCH
	// enable network and stdio on Switch
	socketInitializeDefault();
	// enable error messages via nxlink on Switch
	nxlink_sock = nxlinkStdio();
#endif

	return dvl::WinMain(NULL, NULL, (char *)cmdline.c_str(), 0);

#ifdef SWITCH
	// disable network and stdio on Switch
	if (nxlink_sock != -1)
		close(nxlink_sock);
	socketExit();
#endif
}
