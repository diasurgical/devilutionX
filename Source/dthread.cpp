/**
 * @file dthread.cpp
 *
 * Implementation of functions for updating game state from network commands.
 */

#include <list>
#include <mutex>

#include "nthread.h"
#include "utils/sdl_cond.h"
#include "utils/sdl_thread.h"

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

std::optional<SdlMutex> DthreadMutex;
std::list<DThreadPkt> InfoList;
bool DthreadRunning;
std::optional<SdlCond> WorkToDo;

/* rdata */
SdlThread Thread;

void DthreadHandler()
{
	std::lock_guard<SdlMutex> lock(*DthreadMutex);
	while (true) {
		while (!InfoList.empty()) {
			DThreadPkt pkt = std::move(InfoList.front());
			InfoList.pop_front();

			DthreadMutex->unlock();
			multi_send_zero_packet(pkt.pnum, pkt.cmd, pkt.data.get(), pkt.len);
			DthreadMutex->lock();
		}
		if (!DthreadRunning)
			return;
		WorkToDo->wait(*DthreadMutex);
	}
}

} // namespace

void dthread_remove_player(uint8_t pnum)
{
	std::lock_guard<SdlMutex> lock(*DthreadMutex);
	InfoList.remove_if([&](auto &pkt) {
		return pkt.pnum == pnum;
	});
}

void dthread_send_delta(int pnum, _cmd_id cmd, std::unique_ptr<byte[]> data, uint32_t len)
{
	if (!gbIsMultiplayer)
		return;

	DThreadPkt pkt { pnum, cmd, std::move(data), len };

	std::lock_guard<SdlMutex> lock(*DthreadMutex);
	InfoList.push_back(std::move(pkt));
	WorkToDo->signal();
}

void dthread_start()
{
	if (!gbIsMultiplayer)
		return;

	DthreadRunning = true;
	DthreadMutex.emplace();
	WorkToDo.emplace();
	Thread = SdlThread { DthreadHandler };
}

void DThreadCleanup()
{
	if (!DthreadRunning)
		return;

	{
		std::lock_guard<SdlMutex> lock(*DthreadMutex);
		DthreadRunning = false;
		InfoList.clear();
		WorkToDo->signal();
	}

	Thread.join();
	DthreadMutex = std::nullopt;
	WorkToDo = std::nullopt;
}

} // namespace devilution
