//HEADER_GOES_HERE

#include "../types.h"

DEVILUTION_BEGIN_NAMESPACE

static CRITICAL_SECTION sgMemCrit; // idb
unsigned int glpDThreadId;         // idb
TMegaPkt *sgpInfoHead;             /* may not be right struct */
BOOLEAN dthread_running;
HANDLE sghWorkToDoEvent;

/* rdata */
static HANDLE sghThread = INVALID_HANDLE_VALUE;

#ifndef _MSC_VER
__attribute__((constructor))
#endif
static void
dthread_c_init(void)
{
	dthread_init_mutex();
	dthread_cleanup_mutex_atexit();
}

SEG_ALLOCATE(SEGMENT_C_INIT)
_PVFV dthread_c_init_funcs[] = { &dthread_c_init };

void __cdecl dthread_init_mutex()
{
	InitializeCriticalSection(&sgMemCrit);
}

void __cdecl dthread_cleanup_mutex_atexit()
{
	atexit(dthread_cleanup_mutex);
}

void __cdecl dthread_cleanup_mutex()
{
	DeleteCriticalSection(&sgMemCrit);
}

void __fastcall dthread_remove_player(int pnum)
{
	TMegaPkt *pkt;

	EnterCriticalSection(&sgMemCrit);
	for (pkt = sgpInfoHead; pkt; pkt = pkt->pNext) {
		if (pkt->dwSpaceLeft == pnum)
			pkt->dwSpaceLeft = 4;
	}
	LeaveCriticalSection(&sgMemCrit);
}

void __fastcall dthread_send_delta(int pnum, char cmd, void *pbSrc, int dwLen)
{
	TMegaPkt *pkt;
	TMegaPkt *p;

	if (gbMaxPlayers == 1) {
		return;
	}

	pkt = (TMegaPkt *)DiabloAllocPtr(dwLen + 20);
	pkt->pNext = NULL;
	pkt->dwSpaceLeft = pnum;
	pkt->data[0] = cmd;
	*(_DWORD *)&pkt->data[4] = dwLen;
	memcpy(&pkt->data[8], pbSrc, dwLen);
	EnterCriticalSection(&sgMemCrit);
	p = (TMegaPkt *)&sgpInfoHead;
	while (p->pNext) {
		p = p->pNext;
	}
	p->pNext = pkt;

	SetEvent(sghWorkToDoEvent);
	LeaveCriticalSection(&sgMemCrit);
}

void __cdecl dthread_start()
{
	char *error_buf;

	if (gbMaxPlayers == 1) {
		return;
	}

	sghWorkToDoEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!sghWorkToDoEvent) {
		error_buf = TraceLastError();
		app_fatal("dthread:1\n%s", error_buf);
	}

	dthread_running = TRUE;

	sghThread = (HANDLE)_beginthreadex(NULL, 0, dthread_handler, NULL, 0, &glpDThreadId);
	if (sghThread == INVALID_HANDLE_VALUE) {
		error_buf = TraceLastError();
		app_fatal("dthread2:\n%s", error_buf);
	}
}

unsigned int __stdcall dthread_handler(void *unused)
{
	char *error_buf;
	TMegaPkt *pkt;
	DWORD dwMilliseconds;

	while (dthread_running) {
		if (!sgpInfoHead && WaitForSingleObject(sghWorkToDoEvent, 0xFFFFFFFF) == -1) {
			error_buf = TraceLastError();
			app_fatal("dthread4:\n%s", error_buf);
		}

		EnterCriticalSection(&sgMemCrit);
		pkt = sgpInfoHead;
		if (sgpInfoHead)
			sgpInfoHead = sgpInfoHead->pNext;
		else
			ResetEvent(sghWorkToDoEvent);
		LeaveCriticalSection(&sgMemCrit);

		if (pkt) {
			if (pkt->dwSpaceLeft != 4)
				multi_send_zero_packet(pkt->dwSpaceLeft, pkt->data[0], &pkt->data[8], *(_DWORD *)&pkt->data[4]);

			dwMilliseconds = 1000 * *(_DWORD *)&pkt->data[4] / gdwDeltaBytesSec;
			if (dwMilliseconds >= 1)
				dwMilliseconds = 1;

			mem_free_dbg(pkt);

			if (dwMilliseconds)
				Sleep(dwMilliseconds);
		}
	}

	return 0;
}
// 679730: using guessed type int gdwDeltaBytesSec;

void __cdecl dthread_cleanup()
{
	char *error_buf;
	TMegaPkt *tmp1, *tmp2;

	if (sghWorkToDoEvent == NULL) {
		return;
	}

	dthread_running = FALSE;
	SetEvent(sghWorkToDoEvent);
	if (sghThread != INVALID_HANDLE_VALUE && glpDThreadId != GetCurrentThreadId()) {
		if (WaitForSingleObject(sghThread, 0xFFFFFFFF) == -1) {
			error_buf = TraceLastError();
			app_fatal("dthread3:\n(%s)", error_buf);
		}
		CloseHandle(sghThread);
		sghThread = INVALID_HANDLE_VALUE;
	}
	CloseHandle(sghWorkToDoEvent);
	sghWorkToDoEvent = NULL;

	while (sgpInfoHead) {
		tmp1 = sgpInfoHead->pNext;
		tmp2 = sgpInfoHead;
		sgpInfoHead = NULL;
		mem_free_dbg(tmp2);
		sgpInfoHead = tmp1;
	}
}

DEVILUTION_END_NAMESPACE
