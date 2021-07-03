/**
 * @file dthread.cpp
 *
 * Implementation of functions for updating game state from network commands.
 */

#include "nthread.h"
#include "storm/storm.h"
#include "utils/thread.h"

namespace devilution {

static CCritSect sgMemCrit;
SDL_threadID glpDThreadId;
TMegaPkt *sgpInfoHead; /* may not be right struct */
bool dthread_running;
event_emul *sghWorkToDoEvent;

/* rdata */
static SDL_Thread *sghThread = nullptr;

static unsigned int DthreadHandler(void * /*data*/)
{
	const char *errorBuf;
	TMegaPkt *pkt;
	DWORD dwMilliseconds;

	while (dthread_running) {
		if (sgpInfoHead == nullptr && WaitForEvent(sghWorkToDoEvent) == -1) {
			errorBuf = SDL_GetError();
			app_fatal("dthread4:\n%s", errorBuf);
		}

		sgMemCrit.Enter();
		pkt = sgpInfoHead;
		if (sgpInfoHead != nullptr)
			sgpInfoHead = sgpInfoHead->pNext;
		else
			ResetEvent(sghWorkToDoEvent);
		sgMemCrit.Leave();

		if (pkt != nullptr) {
			if (pkt->dwSpaceLeft != MAX_PLRS)
				multi_send_zero_packet(pkt->dwSpaceLeft, static_cast<_cmd_id>(pkt->data[0]), &pkt->data[8], *(DWORD *)&pkt->data[4]);

			dwMilliseconds = 1000 * *(DWORD *)&pkt->data[4] / gdwDeltaBytesSec;
			if (dwMilliseconds >= 1)
				dwMilliseconds = 1;

			std::free(pkt);

			if (dwMilliseconds != 0)
				SDL_Delay(dwMilliseconds);
		}
	}

	return 0;
}

void dthread_remove_player(uint8_t pnum)
{
	TMegaPkt *pkt;

	sgMemCrit.Enter();
	for (pkt = sgpInfoHead; pkt != nullptr; pkt = pkt->pNext) {
		if (pkt->dwSpaceLeft == pnum)
			pkt->dwSpaceLeft = MAX_PLRS;
	}
	sgMemCrit.Leave();
}

void dthread_send_delta(int pnum, _cmd_id cmd, byte *pbSrc, int dwLen)
{
	TMegaPkt *pkt;
	TMegaPkt *p;

	if (!gbIsMultiplayer) {
		return;
	}

	pkt = static_cast<TMegaPkt *>(std::malloc(dwLen + 20));
	pkt->pNext = nullptr;
	pkt->dwSpaceLeft = pnum;
	pkt->data[0] = static_cast<byte>(cmd);
	*(DWORD *)&pkt->data[4] = dwLen;
	memcpy(&pkt->data[8], pbSrc, dwLen);
	sgMemCrit.Enter();
	p = (TMegaPkt *)&sgpInfoHead;
	while (p->pNext != nullptr) {
		p = p->pNext;
	}
	p->pNext = pkt;

	SetEvent(sghWorkToDoEvent);
	sgMemCrit.Leave();
}

void dthread_start()
{
	const char *errorBuf;

	if (!gbIsMultiplayer) {
		return;
	}

	sghWorkToDoEvent = StartEvent();
	if (sghWorkToDoEvent == nullptr) {
		errorBuf = SDL_GetError();
		app_fatal("dthread:1\n%s", errorBuf);
	}

	dthread_running = true;

	sghThread = CreateThread(DthreadHandler, &glpDThreadId);
	if (sghThread == nullptr) {
		errorBuf = SDL_GetError();
		app_fatal("dthread2:\n%s", errorBuf);
	}
}

void dthread_cleanup()
{
	TMegaPkt *tmp;

	if (sghWorkToDoEvent == nullptr) {
		return;
	}

	dthread_running = false;
	SetEvent(sghWorkToDoEvent);
	if (sghThread != nullptr && glpDThreadId != SDL_GetThreadID(nullptr)) {
		SDL_WaitThread(sghThread, nullptr);
		sghThread = nullptr;
	}
	EndEvent(sghWorkToDoEvent);
	sghWorkToDoEvent = nullptr;

	while (sgpInfoHead != nullptr) {
		tmp = sgpInfoHead->pNext;
		std::free(sgpInfoHead);
		sgpInfoHead = tmp;
	}
}

} // namespace devilution
