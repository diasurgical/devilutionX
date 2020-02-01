#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

BYTE sgbNetUpdateRate;
DWORD gdwMsgLenTbl[MAX_PLRS];
static CCritSect sgMemCrit;
DWORD gdwDeltaBytesSec;
BOOLEAN nthread_should_run;
DWORD gdwTurnsInTransit;
uintptr_t glpMsgTbl[MAX_PLRS];
unsigned int glpNThreadId;
char sgbSyncCountdown;
int turn_upper_bit;
BOOLEAN sgbTicsOutOfSync;
char sgbPacketCountdown;
BOOLEAN sgbThreadIsRunning;
DWORD gdwLargestMsgSize;
DWORD gdwNormalMsgSize;
int last_tick;

/* data */
static HANDLE sghThread = INVALID_HANDLE_VALUE;

void nthread_terminate_game(const char *pszFcn)
{
	DWORD sErr;

	sErr = SErrGetLastError();
	if (sErr == STORM_ERROR_INVALID_PLAYER) {
		return;
	} else if (sErr == STORM_ERROR_GAME_TERMINATED) {
		gbGameDestroyed = TRUE;
	} else if (sErr == STORM_ERROR_NOT_IN_GAME) {
		gbGameDestroyed = TRUE;
	} else {
		app_fatal("%s:\n%s", pszFcn, TraceLastError());
	}
}

DWORD nthread_send_and_recv_turn(DWORD cur_turn, int turn_delta)
{
	DWORD new_cur_turn;
	int turn, turn_tmp;
	int curTurnsInTransit;

	new_cur_turn = cur_turn;
	if (!SNetGetTurnsInTransit(&curTurnsInTransit)) {
		nthread_terminate_game("SNetGetTurnsInTransit");
		return 0;
	}
	while (curTurnsInTransit < gdwTurnsInTransit) {
		curTurnsInTransit++;

		turn_tmp = turn_upper_bit | new_cur_turn & 0x7FFFFFFF;
		turn_upper_bit = 0;
		turn = turn_tmp;

		if (!SNetSendTurn((char *)&turn, sizeof(turn))) {
			nthread_terminate_game("SNetSendTurn");
			return 0;
		}

		new_cur_turn += turn_delta;
		if (new_cur_turn >= 0x7FFFFFFF)
			new_cur_turn &= 0xFFFF;
	}
	return new_cur_turn;
}

BOOL nthread_recv_turns(BOOL *pfSendAsync)
{
	*pfSendAsync = FALSE;
	sgbPacketCountdown--;
	if (sgbPacketCountdown) {
		last_tick += 50;
		return TRUE;
	}
	sgbSyncCountdown--;
	sgbPacketCountdown = sgbNetUpdateRate;
	if (sgbSyncCountdown != 0) {

		*pfSendAsync = TRUE;
		last_tick += 50;
		return TRUE;
	}
	if (!SNetReceiveTurns(0, MAX_PLRS, (char **)glpMsgTbl, gdwMsgLenTbl, (LPDWORD)player_state)) {
		if (SErrGetLastError() != STORM_ERROR_NO_MESSAGES_WAITING)
			nthread_terminate_game("SNetReceiveTurns");
		sgbTicsOutOfSync = FALSE;
		sgbSyncCountdown = 1;
		sgbPacketCountdown = 1;
		return FALSE;
	} else {
		if (!sgbTicsOutOfSync) {
			sgbTicsOutOfSync = TRUE;
			last_tick = SDL_GetTicks();
		}
		sgbSyncCountdown = 4;
		multi_msg_countdown();
		*pfSendAsync = TRUE;
		last_tick += 50;
		return TRUE;
	}
}

void nthread_set_turn_upper_bit()
{
	turn_upper_bit = 0x80000000;
}

void nthread_start(BOOL set_turn_upper_bit)
{
	const char *err, *err2;
	DWORD largestMsgSize;
	_SNETCAPS caps;

	last_tick = SDL_GetTicks();
	sgbPacketCountdown = 1;
	sgbSyncCountdown = 1;
	sgbTicsOutOfSync = TRUE;
	if (set_turn_upper_bit)
		nthread_set_turn_upper_bit();
	else
		turn_upper_bit = 0;
	caps.size = 36;
	if (!SNetGetProviderCaps(&caps)) {
		err = TraceLastError();
		app_fatal("SNetGetProviderCaps:\n%s", err);
	}
	gdwTurnsInTransit = caps.defaultturnsintransit;
	if (!caps.defaultturnsintransit)
		gdwTurnsInTransit = 1;
	if (caps.defaultturnssec <= 20 && caps.defaultturnssec)
		sgbNetUpdateRate = 20 / caps.defaultturnssec;
	else
		sgbNetUpdateRate = 1;
	largestMsgSize = 512;
	if (caps.maxmessagesize < 0x200)
		largestMsgSize = caps.maxmessagesize;
	gdwDeltaBytesSec = caps.bytessec >> 2;
	gdwLargestMsgSize = largestMsgSize;
	gdwNormalMsgSize = caps.bytessec * sgbNetUpdateRate / 20;
	gdwNormalMsgSize *= 3;
	gdwNormalMsgSize >>= 2;
	if (caps.maxplayers > MAX_PLRS)
		caps.maxplayers = MAX_PLRS;
	gdwNormalMsgSize /= caps.maxplayers;
	while (gdwNormalMsgSize < 0x80) {
		gdwNormalMsgSize *= 2;
		sgbNetUpdateRate *= 2;
	}
	if (gdwNormalMsgSize > largestMsgSize)
		gdwNormalMsgSize = largestMsgSize;
	if (gbMaxPlayers > 1) {
		sgbThreadIsRunning = FALSE;
		sgMemCrit.Enter();
		nthread_should_run = TRUE;
		sghThread = (HANDLE)_beginthreadex(NULL, 0, nthread_handler, NULL, 0, &glpNThreadId);
		if (sghThread == INVALID_HANDLE_VALUE) {
			err2 = TraceLastError();
			app_fatal("nthread2:\n%s", err2);
		}
		SetThreadPriority(sghThread, THREAD_PRIORITY_HIGHEST);
	}
}

unsigned int nthread_handler(void *)
{
	int delta;
	BOOL received;

	if (nthread_should_run) {
		while (1) {
			sgMemCrit.Enter();
			if (!nthread_should_run)
				break;
			nthread_send_and_recv_turn(0, 0);
			if (nthread_recv_turns(&received))
				delta = last_tick - SDL_GetTicks();
			else
				delta = 50;
			sgMemCrit.Leave();
			if (delta > 0)
				SDL_Delay(delta);
			if (!nthread_should_run)
				return 0;
		}
		sgMemCrit.Leave();
	}
	return 0;
}

void nthread_cleanup()
{
	nthread_should_run = FALSE;
	gdwTurnsInTransit = 0;
	gdwNormalMsgSize = 0;
	gdwLargestMsgSize = 0;
	if (sghThread != INVALID_HANDLE_VALUE && glpNThreadId != GetCurrentThreadId()) {
		if (!sgbThreadIsRunning)
			sgMemCrit.Leave();
		if (WaitForSingleObject(sghThread, 0xFFFFFFFF) == -1) {
			app_fatal("nthread3:\n(%s)", TraceLastError());
		}
		CloseEvent(sghThread);
		sghThread = INVALID_HANDLE_VALUE;
	}
}

void nthread_ignore_mutex(BOOL bStart)
{
	if (sghThread != INVALID_HANDLE_VALUE) {
		if (bStart)
			sgMemCrit.Leave();
		else
			sgMemCrit.Enter();
		sgbThreadIsRunning = bStart;
	}
}

BOOL nthread_has_500ms_passed(BOOL unused)
{
	DWORD currentTickCount;
	int ticksElapsed;

	currentTickCount = SDL_GetTicks();
	ticksElapsed = currentTickCount - last_tick;
	if (gbMaxPlayers == 1 && ticksElapsed > 500) {
		last_tick = currentTickCount;
		ticksElapsed = 0;
	}
	return ticksElapsed >= 0;
}

DEVILUTION_END_NAMESPACE
