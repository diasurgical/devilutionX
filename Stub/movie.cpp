#include "../types.h"
#include "stubs.h"

#ifndef NO_GLOBALS
BOOL loop_movie;
#endif

void __fastcall play_movie(char *pszMovie, BOOL user_can_close)
{
	DUMMY_PRINT("%s", pszMovie);
}
