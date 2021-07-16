/**
 * @file dthread.cpp
 *
 * Implementation of functions for updating game state from network commands.
 */

#include <list>
#include <atomic>

#include "nthread.h"
#include "storm/storm.h"
#include "utils/thread.h"

namespace devilution {

struct DThreadPkt {
	int pnum;
	_cmd_id cmd;
	std::unique_ptr<byte[]> data;
	uint32_t len;

	DThreadPkt(int pnum, _cmd_id(cmd), std::unique_ptr<byte[]> data, uint32_t len)
		: pnum(pnum)
		, cmd(cmd)
		, data(std::move(data))
		, len(len)
	{
	}
};

namespace {

CCritSect sgMemCrit;
SDL_threadID glpDThreadId;
std::list<DThreadPkt> sgpInfoList;
std::atomic_bool dthread_running;
event_emul *sghWorkToDoEvent;

/* rdata */
SDL_Thread *sghThread = nullptr;

void DthreadHandler()
{
	while (dthread_running) {
		if (sgpInfoList.empty() && WaitForEvent(sghWorkToDoEvent) == -1) {
			const char *errorBuf = SDL_GetError();
			app_fatal("dthread4:\n%s", errorBuf);
		}

		sgMemCrit.Enter();
		if (sgpInfoList .empty()) {
			ResetEvent(sghWorkToDoEvent);
			sgMemCrit.Leave();
		}
		DThreadPkt pkt = std::move(sgpInfoList.front());
		sgpInfoList.pop_front();
		sgMemCrit.Leave();

		if (pkt.pnum != MAX_PLRS)
			multi_send_zero_packet(pkt.pnum, pkt.cmd, pkt.data.get(), pkt.len);

		DWORD dwMilliseconds = 1000 * pkt.len / gdwDeltaBytesSec;
		if (dwMilliseconds >= 1)
			dwMilliseconds = 1;

		if (dwMilliseconds != 0)
			SDL_Delay(dwMilliseconds);
	}
}

} // namespace

void dthread_remove_player(uint8_t pnum)
{
	sgMemCrit.Enter();
	for (auto &pkt : sgpInfoList) {
		if (pkt.pnum == pnum)
			pkt.pnum = MAX_PLRS;
	}
	sgMemCrit.Leave();
}

void dthread_send_delta(int pnum, _cmd_id cmd, std::unique_ptr<byte[]> data, uint32_t len)
{
	if (!gbIsMultiplayer)
		return;

	DThreadPkt pkt { pnum, cmd, std::move(data), len };

	sgMemCrit.Enter();
	sgpInfoList.push_back(std::move(pkt));
	SetEvent(sghWorkToDoEvent);
	sgMemCrit.Leave();
}

void dthread_start()
{
	if (!gbIsMultiplayer)
		return;

	sghWorkToDoEvent = StartEvent();
	dthread_running = true;
	sghThread = CreateThread(DthreadHandler, &glpDThreadId);
}

void DThreadCleanup()
{
	if (sghWorkToDoEvent == nullptr)
		return;

	dthread_running = false;
	SetEvent(sghWorkToDoEvent);
	if (sghThread != nullptr && glpDThreadId != SDL_GetThreadID(nullptr)) {
		SDL_WaitThread(sghThread, nullptr);
		sghThread = nullptr;
	}
	EndEvent(sghWorkToDoEvent);
	sghWorkToDoEvent = nullptr;

	sgpInfoList.clear();
}

} // namespace devilution
