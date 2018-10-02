#include <algorithm>

#include "../types.h"
#include "stubs.h"

#ifndef NO_GLOBALS
int gdwNormalMsgSize;
int gdwLargestMsgSize;
int gdwMsgLenTbl[4];
int glpMsgTbl[4];
int gdwTurnsInTransit;
char byte_679704;
#endif

void __fastcall nthread_start(bool set_turn_upper_bit)
{
	DUMMY();
	byte_679704 = 1;
	gdwTurnsInTransit = 1;
	gdwLargestMsgSize = 496;
	gdwNormalMsgSize = 496;
}

void __cdecl nthread_cleanup()
{
	DUMMY();
}

void __fastcall nthread_terminate_game(const char *pszFcn)
{
	UNIMPLEMENTED();
}

void __fastcall nthread_ignore_mutex(bool bStart)
{
	DUMMY();
}

bool __cdecl nthread_has_500ms_passed()
{
	DUMMY_ONCE();
	return TRUE;
}

DWORD last_frame_time = 0;
const int MSEC_PER_FRAME = 1000 / 35;

static void frame_rate_limiter()
{
	if (last_frame_time) {
		int elapsed = GetTickCount() - last_frame_time;
		int remaining = MSEC_PER_FRAME - elapsed;
		if (remaining > 0) {
			Sleep(std::max(remaining, MSEC_PER_FRAME));
		}
	}
	last_frame_time = GetTickCount();
}

int __fastcall nthread_send_and_recv_turn(int cur_turn, int turn_delta)
{
	DUMMY_ONCE();
	// DUMMY_PRINT("cur_turn: %d turn_delta: %d", cur_turn, turn_delta);

	frame_rate_limiter();

	return 1;
}

void __cdecl nthread_set_turn_upper_bit()
{
	UNIMPLEMENTED();
}

int __fastcall nthread_recv_turns(int *pfSendAsync)
{
	DUMMY_ONCE();
	*pfSendAsync = 0;
	return TRUE;
}
