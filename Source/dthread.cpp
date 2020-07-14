/**
 * @file dthread.cpp
 *
 * Implementation of functions for updating game state from network commands.
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

static CCritSect sgMemCrit;
SDL_threadID glpDThreadId;
TMegaPkt *sgpInfoHead; /* may not be right struct */
BOOLEAN dthread_running;
event_emul *sghWorkToDoEvent;

/* rdata */
static SDL_Thread *sghThread = NULL;

void dthread_remove_player(int pnum)
{
	TMegaPkt *pkt;

	sgMemCrit.Enter();
	for (pkt = sgpInfoHead; pkt; pkt = pkt->pNext) {
		if (pkt->dwSpaceLeft == pnum)
			pkt->dwSpaceLeft = MAX_PLRS;
	}
	sgMemCrit.Leave();
}

void dthread_send_delta(int pnum, char cmd, void *pbSrc, int dwLen)
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
	*(DWORD *)&pkt->data[4] = dwLen;
	memcpy(&pkt->data[8], pbSrc, dwLen);
	sgMemCrit.Enter();
	p = (TMegaPkt *)&sgpInfoHead;
	while (p->pNext) {
		p = p->pNext;
	}
	p->pNext = pkt;

	SetEvent(sghWorkToDoEvent);
	sgMemCrit.Leave();
}

void dthread_start()
{
	const char *error_buf;

	if (gbMaxPlayers == 1) {
		return;
	}

	sghWorkToDoEvent = StartEvent();
	if (sghWorkToDoEvent == NULL) {
		error_buf = TraceLastError();
		app_fatal("dthread:1\n%s", error_buf);
	}

	dthread_running = TRUE;

	sghThread = CreateThread(dthread_handler, &glpDThreadId);
	if (sghThread == NULL) {
		error_buf = TraceLastError();
		app_fatal("dthread2:\n%s", error_buf);
	}
}

unsigned int dthread_handler(void *data)
{
	const char *error_buf;
	TMegaPkt *pkt;
	DWORD dwMilliseconds;

	while (dthread_running) {
		if (!sgpInfoHead && WaitForEvent(sghWorkToDoEvent) == -1) {
			error_buf = TraceLastError();
			app_fatal("dthread4:\n%s", error_buf);
		}

		sgMemCrit.Enter();
		pkt = sgpInfoHead;
		if (sgpInfoHead)
			sgpInfoHead = sgpInfoHead->pNext;
		else
			ResetEvent(sghWorkToDoEvent);
		sgMemCrit.Leave();

		if (pkt) {
			if (pkt->dwSpaceLeft != MAX_PLRS)
				multi_send_zero_packet(pkt->dwSpaceLeft, pkt->data[0], &pkt->data[8], *(DWORD *)&pkt->data[4]);

			dwMilliseconds = 1000 * *(DWORD *)&pkt->data[4] / gdwDeltaBytesSec;
			if (dwMilliseconds >= 1)
				dwMilliseconds = 1;

			mem_free_dbg(pkt);

			if (dwMilliseconds)
				SDL_Delay(dwMilliseconds);
		}
	}

	return 0;
}

void dthread_cleanup()
{
	TMegaPkt *tmp;

	if (sghWorkToDoEvent == NULL) {
		return;
	}

	dthread_running = FALSE;
	SetEvent(sghWorkToDoEvent);
	if (sghThread != NULL && glpDThreadId != SDL_GetThreadID(NULL)) {
		SDL_WaitThread(sghThread, NULL);
		sghThread = NULL;
	}
	EndEvent(sghWorkToDoEvent);
	sghWorkToDoEvent = NULL;

	while (sgpInfoHead) {
		tmp = sgpInfoHead->pNext;
		MemFreeDbg(sgpInfoHead);
		sgpInfoHead = tmp;
	}
}

DEVILUTION_END_NAMESPACE
