#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

BOOL ReadOnlyTest()
{
	FILE *f;
	char path[MAX_PATH], Filename[MAX_PATH];

	GetPrefPath(path, MAX_PATH);
	snprintf(Filename, DVL_MAX_PATH, "%sDiablo1ReadOnlyTest.foo", path);

	f = fopen(Filename, "wt");
	if (f) {
		fclose(f);
		remove(Filename);
		return FALSE;
	}

	return TRUE;
}

DEVILUTION_END_NAMESPACE
