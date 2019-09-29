#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

BOOL ReadOnlyTest()
{
	FILE *f;

	const std::string Filename = GetPrefPath() + "Diablo1ReadOnlyTest.foo";

	f = fopen(Filename.c_str(), "wt");
	if (f) {
		fclose(f);
		remove(Filename.c_str());
		return FALSE;
	}

	return TRUE;
}

DEVILUTION_END_NAMESPACE
