//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
bool sgbIsScrolling; // weak
int track_cpp_init_value; // weak
int sgdwLastWalk; // weak
bool sgbIsWalking; // weak
#endif

const int track_inf = 0x7F800000; // weak

struct track_cpp_init
{
	track_cpp_init()
	{
		track_cpp_init_value = track_inf;
	}
} _track_cpp_init;
// 4802D0: using guessed type int track_inf;
// 6ABABC: using guessed type int track_cpp_init_value;

void __cdecl track_process()
{
	int v0; // eax
	DWORD v1; // eax

	if ( sgbIsWalking )
	{
		if ( cursmx >= 0 && cursmx < 111 && cursmy >= 0 && cursmy < 111 )
		{
			v0 = myplr;
			if ( (plr[myplr]._pVar8 > 6 || plr[v0]._pmode == PM_STAND)
			  && (cursmx != plr[v0]._ptargx || cursmy != plr[v0]._ptargy) )
			{
				v1 = GetTickCount();
				if ( v1 - sgdwLastWalk >= 300 )
				{
					sgdwLastWalk = v1;
					NetSendCmdLoc(1u, CMD_WALKXY, cursmx, cursmy);
					if ( !sgbIsScrolling )
						sgbIsScrolling = 1;
				}
			}
		}
	}
}
// 6ABAB8: using guessed type char sgbIsScrolling;
// 6ABAC0: using guessed type int sgdwLastWalk;
// 6ABAC4: using guessed type int sgbIsWalking;

void __fastcall track_repeat_walk(bool rep)
{
	if ( sgbIsWalking != rep )
	{
		sgbIsWalking = rep;
		if ( rep )
		{
			sgbIsScrolling = 0;
			sgdwLastWalk = GetTickCount() - 50;
			NetSendCmdLoc(1u, CMD_WALKXY, cursmx, cursmy);
		}
		else if ( sgbIsScrolling )
		{
			sgbIsScrolling = 0;
		}
	}
}
// 6ABAB8: using guessed type char sgbIsScrolling;
// 6ABAC0: using guessed type int sgdwLastWalk;
// 6ABAC4: using guessed type int sgbIsWalking;

bool __cdecl track_isscrolling()
{
	return sgbIsScrolling;
}
// 6ABAB8: using guessed type char sgbIsScrolling;
