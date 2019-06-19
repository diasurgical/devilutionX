#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

BOOL SystemSupported()
{
#ifndef SWITCH
	OSVERSIONINFO VersionInformation;
	BOOL ret = FALSE;

	memset(&VersionInformation, 0, sizeof(VersionInformation));
	VersionInformation.dwOSVersionInfoSize = sizeof(VersionInformation);
	if (GetVersionEx(&VersionInformation)
	    && VersionInformation.dwPlatformId == VER_PLATFORM_WIN32_NT
	    && VersionInformation.dwMajorVersion >= 5) {
		ret = TRUE;
	}
	return ret;
#else
	return TRUE;
#endif
}

BOOL RestrictedTest()
{
#ifndef SWITCH
	FILE *f;
	char Buffer[MAX_PATH];
	BOOL ret = FALSE;

	if (SystemSupported() && GetWindowsDirectory(Buffer, sizeof(Buffer))) {
		strcat(Buffer, "\\Diablo1RestrictedTest.foo");
		f = fopen(Buffer, "wt");
		if (f) {
			fclose(f);
			remove(Buffer);
		} else {
			ret = TRUE;
		}
	}
	return ret;
#else
	return TRUE;
#endif
}

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
