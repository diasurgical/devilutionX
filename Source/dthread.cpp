/**
 * @file dthread.cpp
 *
 * Implementation of functions for updating game state from network commands.
 */

#include <list>
#include <atomic>
#include <mutex>

#include "nthread.h"
#include "utils/sdl_mutex.h"
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

SdlMutex DthreadMutex;
SDL_threadID glpDThreadId;
std::list<DThreadPkt> InfoList;
std::atomic_bool dthread_running;
event_emul *sghWorkToDoEvent;

/* rdata */
SDL_Thread *sghThread = nullptr;

void DthreadHandler()
{
	while (dthread_running) {
		if (InfoList.empty() && WaitForEvent(sghWorkToDoEvent) == -1)
			app_fatal("dthread4:\n%s", SDL_GetError());

		DthreadMutex.lock();
		if (InfoList.empty()) {
			ResetEvent(sghWorkToDoEvent);
			DthreadMutex.unlock();
			continue;
		}
		DThreadPkt pkt = std::move(InfoList.front());
		InfoList.pop_front();
		DthreadMutex.unlock();

		multi_send_zero_packet(pkt.pnum, pkt.cmd, pkt.data.get(), pkt.len);
	}
}

} // namespace

void dthread_remove_player(uint8_t pnum)
{
	std::lock_guard lock(DthreadMutex);
	InfoList.remove_if([&](auto &pkt) {
		return pkt.pnum == pnum;
	});
}

void dthread_send_delta(int pnum, _cmd_id cmd, std::unique_ptr<byte[]> data, uint32_t len)
{
	if (!gbIsMultiplayer)
		return;

	DThreadPkt pkt { pnum, cmd, std::move(data), len };

	std::lock_guard lock(DthreadMutex);
	InfoList.push_back(std::move(pkt));
	SetEvent(sghWorkToDoEvent);
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

	InfoList.clear();
}

} // namespace devilution
