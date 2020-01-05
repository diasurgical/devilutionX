#include <SDL.h>
#ifdef __SWITCH__
#include "platform/switch/network.h"
#endif
#include <libintl.h>

#include "devilution.h"

#if !defined(__APPLE__)
extern "C" const char *__asan_default_options()
{
	return "halt_on_error=0";
}
#endif

int main(int argc, char **argv)
{
	setlocale(LC_ALL, "");
	bindtextdomain("devilutionx", "./locale");
	bind_textdomain_codeset("devilutionx", "UTF-8");
	textdomain("devilutionx");

#ifdef __SWITCH__
	switch_enable_network();
#endif

	return dvl::DiabloMain(argc, argv);
}
