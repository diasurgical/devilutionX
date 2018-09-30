//HEADER_GOES_HERE

#include "../types.h"

int nthread_cpp_init_value; // weak
char byte_679704; // weak
int gdwMsgLenTbl[4];
static CRITICAL_SECTION sgMemCrit;
int gdwDeltaBytesSec; // weak
char byte_679734; // weak
int gdwTurnsInTransit; // weak
int glpMsgTbl[4];
unsigned int glpNThreadId;
char sgbSyncCountdown; // weak
int dword_679754; // weak
char byte_679758; // weak
char sgbPacketCountdown; // weak
char sgbThreadIsRunning; // weak
int gdwLargestMsgSize; // weak
int gdwNormalMsgSize; // weak
int dword_679764; // weak

const int nthread_inf = 0x7F800000; // weak

/* data */
static HANDLE sghThread = (HANDLE)0xFFFFFFFF; // idb

struct nthread_cpp_init_1
{
	nthread_cpp_init_1()
	{
		nthread_cpp_init_value = nthread_inf;
	}
} _nthread_cpp_init_1;
// 47F164: using guessed type int nthread_inf;
// 679700: using guessed type int nthread_cpp_init_value;

#ifndef MINIWIN
struct nthread_cpp_init_2
{
	nthread_cpp_init_2()
	{
		nthread_init_mutex();
		nthread_cleanup_mutex_atexit();
	}
} _nthread_cpp_init_2;
#endif

void __cdecl nthread_init_mutex()
{
	InitializeCriticalSection(&sgMemCrit);
}

void __cdecl nthread_cleanup_mutex_atexit()
{
	atexit(nthread_cleanup_mutex);
}

void __cdecl nthread_cleanup_mutex()
{
	DeleteCriticalSection(&sgMemCrit);
}

void __fastcall nthread_terminate_game(char *pszFcn)
{
	char *v1; // esi
	int v2; // eax
	char *v3; // eax

	v1 = pszFcn;
	v2 = SErrGetLastError();
	if ( v2 != STORM_ERROR_INVALID_PLAYER )
	{
		if ( v2 == STORM_ERROR_GAME_TERMINATED || v2 == STORM_ERROR_NOT_IN_GAME )
		{
			gbGameDestroyed = 1;
		}
		else
		{
			v3 = TraceLastError();
			TermMsg("%s:\n%s", v1, v3);
		}
	}
}
// 67862D: using guessed type char gbGameDestroyed;

int __fastcall nthread_send_and_recv_turn(int cur_turn, int turn_delta)
{
	int v2; // ebx
	unsigned int v3; // edi
	char *v5; // ecx
	int v6; // eax
	int turn; // [esp+Ch] [ebp-8h]
	int turns; // [esp+10h] [ebp-4h]

	v2 = turn_delta;
	v3 = cur_turn;
	if ( SNetGetTurnsInTransit(&turns) )
	{
		if ( turns >= (unsigned int)gdwTurnsInTransit )
			return v3;
		while ( 1 )
		{
			++turns;
			v6 = dword_679754 | v3 & 0x7FFFFFFF;
			dword_679754 = 0;
			turn = v6;
			if ( !SNetSendTurn((char *)&turn, 4) )
				break;
			v3 += v2;
			if ( v3 >= 0x7FFFFFFF )
				v3 = (unsigned short)v3;
			if ( turns >= (unsigned int)gdwTurnsInTransit )
				return v3;
		}
		v5 = "SNetSendTurn";
	}
	else
	{
		v5 = "SNetGetTurnsInTransit";
	}
	nthread_terminate_game(v5);
	return 0;
}
// 679738: using guessed type int gdwTurnsInTransit;
// 679754: using guessed type int dword_679754;

int __fastcall nthread_recv_turns(int *pfSendAsync)
{
	int *v1; // esi
	bool v2; // zf

	v1 = pfSendAsync;
	*pfSendAsync = 0;
	if ( --sgbPacketCountdown )
	{
		dword_679764 += 50;
		return 1;
	}
	v2 = sgbSyncCountdown-- == 1;
	sgbPacketCountdown = byte_679704;
	if ( !v2 )
		goto LABEL_11;
	if ( SNetReceiveTurns(0, 4, (char **)glpMsgTbl, (unsigned int *)gdwMsgLenTbl, (unsigned long *)player_state) )
	{
		if ( !byte_679758 )
		{
			byte_679758 = 1;
			dword_679764 = GetTickCount();
		}
		sgbSyncCountdown = 4;
		multi_msg_countdown();
LABEL_11:
		*v1 = 1;
		dword_679764 += 50;
		return 1;
	}
	if ( SErrGetLastError() != STORM_ERROR_NO_MESSAGES_WAITING )
		nthread_terminate_game("SNetReceiveTurns");
	byte_679758 = 0;
	sgbSyncCountdown = 1;
	sgbPacketCountdown = 1;
	return 0;
}
// 679704: using guessed type char byte_679704;
// 679750: using guessed type char sgbSyncCountdown;
// 679758: using guessed type char byte_679758;
// 679759: using guessed type char sgbPacketCountdown;
// 679764: using guessed type int dword_679764;

void __cdecl nthread_set_turn_upper_bit()
{
	dword_679754 = 0x80000000;
}
// 679754: using guessed type int dword_679754;

void __fastcall nthread_start(bool set_turn_upper_bit)
{
	BOOL v1; // esi
	char *v3; // eax
	unsigned int v4; // esi
	unsigned int v5; // eax
	char *v6; // eax
	_SNETCAPS caps; // [esp+8h] [ebp-24h]

	v1 = set_turn_upper_bit;
	dword_679764 = GetTickCount();
	sgbPacketCountdown = 1;
	sgbSyncCountdown = 1;
	byte_679758 = 1;
	if ( v1 )
		nthread_set_turn_upper_bit();
	else
		dword_679754 = 0;
	caps.size = 36;
	if ( !SNetGetProviderCaps(&caps) )
	{
		v3 = TraceLastError();
		TermMsg("SNetGetProviderCaps:\n%s", v3);
	}
	gdwTurnsInTransit = caps.defaultturnsintransit;
	if ( !caps.defaultturnsintransit )
		gdwTurnsInTransit = 1;
	if ( caps.defaultturnssec <= 0x14u && caps.defaultturnssec )
		byte_679704 = 0x14u / caps.defaultturnssec;
	else
		byte_679704 = 1;
	v4 = 512;
	if ( caps.maxmessagesize < 0x200u )
		v4 = caps.maxmessagesize;
	gdwDeltaBytesSec = (unsigned int)caps.bytessec >> 2;
	gdwLargestMsgSize = v4;
	if ( caps.maxplayers > 4u )
		caps.maxplayers = 4;
	v5 = (3 * (caps.bytessec * (unsigned int)(unsigned char)byte_679704 / 0x14) >> 2) / caps.maxplayers;
	gdwNormalMsgSize = v5;
	if ( v5 < 0x80 )
	{
		do
		{
			byte_679704 *= 2;
			v5 *= 2;
		}
		while ( v5 < 0x80 );
		gdwNormalMsgSize = v5;
	}
	if ( v5 > v4 )
		gdwNormalMsgSize = v4;
	if ( (unsigned char)gbMaxPlayers > 1u )
	{
		sgbThreadIsRunning = 0;
		EnterCriticalSection(&sgMemCrit);
		byte_679734 = 1;
		sghThread = (HANDLE)_beginthreadex(NULL, 0, nthread_handler, NULL, 0, &glpNThreadId);
		if ( sghThread == (HANDLE)-1 )
		{
			v6 = TraceLastError();
			TermMsg("nthread2:\n%s", v6);
		}
		SetThreadPriority(sghThread, THREAD_PRIORITY_HIGHEST);
	}
}
// 679660: using guessed type char gbMaxPlayers;
// 679704: using guessed type char byte_679704;
// 679730: using guessed type int gdwDeltaBytesSec;
// 679734: using guessed type char byte_679734;
// 679738: using guessed type int gdwTurnsInTransit;
// 679750: using guessed type char sgbSyncCountdown;
// 679754: using guessed type int dword_679754;
// 679758: using guessed type char byte_679758;
// 679759: using guessed type char sgbPacketCountdown;
// 67975A: using guessed type char sgbThreadIsRunning;
// 67975C: using guessed type int gdwLargestMsgSize;
// 679760: using guessed type int gdwNormalMsgSize;
// 679764: using guessed type int dword_679764;

unsigned int __stdcall nthread_handler(void *a1)
{
	signed int v1; // esi
	int recieved; // [esp+Ch] [ebp-4h]

	if ( byte_679734 )
	{
		while ( 1 )
		{
			EnterCriticalSection(&sgMemCrit);
			if ( !byte_679734 )
				break;
			nthread_send_and_recv_turn(0, 0);
			if ( nthread_recv_turns(&recieved) )
				v1 = dword_679764 - GetTickCount();
			else
				v1 = 50;
			LeaveCriticalSection(&sgMemCrit);
			if ( v1 > 0 )
				Sleep(v1);
			if ( !byte_679734 )
				return 0;
		}
		LeaveCriticalSection(&sgMemCrit);
	}
	return 0;
}
// 679734: using guessed type char byte_679734;
// 679764: using guessed type int dword_679764;

void __cdecl nthread_cleanup()
{
	char *v0; // eax

	byte_679734 = 0;
	gdwTurnsInTransit = 0;
	gdwNormalMsgSize = 0;
	gdwLargestMsgSize = 0;
	if ( sghThread != (HANDLE)-1 && glpNThreadId != GetCurrentThreadId() )
	{
		if ( !sgbThreadIsRunning )
			LeaveCriticalSection(&sgMemCrit);
		if ( WaitForSingleObject(sghThread, 0xFFFFFFFF) == -1 )
		{
			v0 = TraceLastError();
			TermMsg("nthread3:\n(%s)", v0);
		}
		CloseHandle(sghThread);
		sghThread = (HANDLE)-1;
	}
}
// 679734: using guessed type char byte_679734;
// 679738: using guessed type int gdwTurnsInTransit;
// 67975A: using guessed type char sgbThreadIsRunning;
// 67975C: using guessed type int gdwLargestMsgSize;
// 679760: using guessed type int gdwNormalMsgSize;

void __fastcall nthread_ignore_mutex(bool bStart)
{
	bool v1; // bl

	v1 = bStart;
	if ( sghThread != (HANDLE)-1 )
	{
		if ( bStart )
			LeaveCriticalSection(&sgMemCrit);
		else
			EnterCriticalSection(&sgMemCrit);
		sgbThreadIsRunning = v1;
	}
}
// 67975A: using guessed type char sgbThreadIsRunning;

bool __cdecl nthread_has_500ms_passed()
{
	DWORD v0; // eax
	int v1; // ecx

	v0 = GetTickCount();
	v1 = v0 - dword_679764;
	if ( gbMaxPlayers == 1 && v1 > 500 )
	{
		dword_679764 = v0;
		v1 = 0;
	}
	return v1 >= 0;
}
// 679660: using guessed type char gbMaxPlayers;
// 679764: using guessed type int dword_679764;
