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
std::shared_ptr<TMegaPkt> sgpInfoHead; /* may not be right struct */
bool dthread_running;
event_emul *sghWorkToDoEvent;

/* rdata */
static SDL_Thread *sghThread = nullptr;

static unsigned int dthread_handler(void *data)
{
	const char *error_buf;
	std::shared_ptr<TMegaPkt> pkt;
	DWORD dwMilliseconds;

	while (dthread_running) {
		if (sgpInfoHead == nullptr && WaitForEvent(sghWorkToDoEvent) == -1) {
			error_buf = SDL_GetError();
			app_fatal("dthread4:\n%s", error_buf);
		}

		sgMemCrit.Enter();
		pkt = sgpInfoHead;
		if (sgpInfoHead != nullptr)
			sgpInfoHead = sgpInfoHead->next;
		else
			ResetEvent(sghWorkToDoEvent);
		sgMemCrit.Leave();

		if (pkt != nullptr) {
			if (pkt->spaceLeft != MAX_PLRS)
				multi_send_zero_packet(pkt->spaceLeft, static_cast<_cmd_id>(pkt->data[0]), &pkt->data[8], *(DWORD *)&pkt->data[4]);

			dwMilliseconds = 1000 * *(DWORD *)&pkt->data[4] / gdwDeltaBytesSec;
			if (dwMilliseconds >= 1)
				dwMilliseconds = 1;

			if (dwMilliseconds != 0)
				SDL_Delay(dwMilliseconds);
		}
	}

	return 0;
}

void dthread_remove_player(uint8_t pnum)
{
	sgMemCrit.Enter();
	for (TMegaPkt *pkt = sgpInfoHead.get(); pkt != nullptr; pkt = pkt->next.get()) {
		if (pkt->spaceLeft == pnum)
			pkt->spaceLeft = MAX_PLRS;
	}
	sgMemCrit.Leave();
}

void dthread_send_delta(int pnum, char cmd, void *pbSrc, int dwLen)
{
	if (!gbIsMultiplayer) {
		return;
	}

	std::shared_ptr<TMegaPkt> pkt = TMegaPkt::make(dwLen + 20);
	pkt->data[0] = cmd;
	pkt->spaceLeft = pnum;
	*(DWORD *)&pkt->data[4] = dwLen;
	memcpy(&pkt->data[8], pbSrc, dwLen);
	sgMemCrit.Enter();
	TMegaPkt *p = sgpInfoHead.get();
	while (p->next != nullptr) {
		p = p->next.get();
	}
	p->next = pkt;

	SetEvent(sghWorkToDoEvent);
	sgMemCrit.Leave();
}

void dthread_start()
{
	const char *error_buf;

	if (!gbIsMultiplayer) {
		return;
	}

	sghWorkToDoEvent = StartEvent();
	if (sghWorkToDoEvent == nullptr) {
		error_buf = SDL_GetError();
		app_fatal("dthread:1\n%s", error_buf);
	}

	dthread_running = true;

	sghThread = CreateThread(dthread_handler, &glpDThreadId);
	if (sghThread == nullptr) {
		error_buf = SDL_GetError();
		app_fatal("dthread2:\n%s", error_buf);
	}
}

void dthread_cleanup()
{
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

	while (sgpInfoHead != nullptr)
		sgpInfoHead = sgpInfoHead->next;
}

} // namespace devilution
