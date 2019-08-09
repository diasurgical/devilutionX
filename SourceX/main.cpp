#include <SDL.h>
#include <string>

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

	// TODO (cmhobbs)
	// - [X] catch -h and --help in cmdline()
	// - [X] add debug output options
	//   - ref: https://github.com/diasurgical/devilutionX/blob/master/docs/debug.md
	// - [ ] add options from GHI
	//   - ref: https://github.com/diasurgical/devilutionX/issues/219
	//   - [ ] -h and --help to print help options
	//   - [ ] -v and --version to show version or git branch/rev/commit
	//   - [ ] --data-dir path to diabdat.mpq
	if ((cmdline == "-h") || (cmdline == "--help")) {
		fprintf(stdout, "Diablo build for modern operating systems.\n");
		fprintf(stdout, "\nDebug options (if enabled in build, see docs/debug.md for hotkeys):\n");
		fprintf(stdout, "    %-20s %-30s\n", "-^", "enable god mode and debug tools");
		fprintf(stdout, "    %-20s %-30s\n", "-$", "enable god mode with less stuff (not yet implemented)");
		fprintf(stdout, "    %-20s %-30s\n", "-b", "enables item drop log (not yet implemented)");
		fprintf(stdout, "    %-20s %-30s\n", "-d", "disable startup video + increaased item drops (partially implemented)");
		fprintf(stdout, "    %-20s %-30s\n", "-f", "display frames per second");
		fprintf(stdout, "    %-20s %-30s\n", "-i", "display network timeout");
		fprintf(stdout, "    %-20s %-30s\n", "-n", "disable startup video");
		fprintf(stdout, "    %-20s %-30s\n", "-s", "unused");
		fprintf(stdout, "    %-20s %-30s\n", "-v", "draw yellow debug tiles");
		fprintf(stdout, "    %-20s %-30s\n", "-w", "enable cheats");
		fprintf(stdout, "    %-20s %-30s\n", "-x", "disable exclusive DreictDraw access (not yet implemented)");
		fprintf(stdout, "    %-20s %-30s\n", "-j <##>", "init trigger at level (not yet implemented)");
		fprintf(stdout, "    %-20s %-30s\n", "-l <##> <##>", "start in level as type");
		fprintf(stdout, "    %-20s %-30s\n", "-m <##>", "add debug monster, up to 10 allowed");
		fprintf(stdout, "    %-20s %-30s\n", "-q <#>", "force a certain quest");
		fprintf(stdout, "    %-20s %-30s\n", "-r <##########>", "set map seed to");
		fprintf(stdout, "    %-20s %-30s\n", "-t <##>", "set current quest level");
		fprintf(stdout, "\nMultiplayer settings:\n");
		fprintf(stdout, "    %-20s %-30s\n", "TCP/IP", "host is required to expose port 6112");
		fprintf(stdout, "    %-20s %-30s\n", "UDP/IP", "all players are required to expose port 6112");
		fprintf(stdout, "\nhttps://github.com/diasurgical/devilutionX/\n");
		return 0;
	} else {
		return dvl::WinMain(NULL, NULL, (char *)cmdline.c_str(), 0);
	}
}
