#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

BOOL ReadOnlyTest()
{
#ifndef SWITCH
	char *c;
	FILE *f;
	char Filename[MAX_PATH];
	BOOL ret = FALSE;

	if (GetModuleFileName(ghInst, Filename, sizeof(Filename))) {
		c = strrchr(Filename, '\\');
		if (c) {
			strcpy(c + 1, "Diablo1ReadOnlyTest.foo");
			f = fopen(Filename, "wt");
			if (f) {
				fclose(f);
				remove(Filename);
			} else {
				ret = TRUE;
			}
		}
	}
	return ret;
#else
	return FALSE;
#endif
}

DEVILUTION_END_NAMESPACE
