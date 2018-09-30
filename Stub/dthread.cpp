#include "../types.h"
#include "stubs.h"

void __cdecl dthread_start()
{
	DUMMY();
}

void __cdecl dthread_cleanup()
{
	DUMMY();
}

void __fastcall dthread_send_delta(int pnum, char cmd, void *pbSrc, int dwLen)
{
	DUMMY();
}

void __fastcall dthread_remove_player(int pnum)
{
	UNIMPLEMENTED();
}
