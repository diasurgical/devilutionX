/**
 * @file nthread.cpp
 *
 * Implementation of functions for managing game ticks.
 */

#include "nthread.h"
#include "diablo.h"
#include "engine/demomode.h"
#include "gmenu.h"
#include "storm/storm_net.hpp"
#include "utils/sdl_mutex.h"
#include "utils/sdl_thread.h"

namespace devilution {

BYTE sgbNetUpdateRate;
size_t gdwMsgLenTbl[MAX_PLRS];
uint32_t gdwTurnsInTransit;
uintptr_t glpMsgTbl[MAX_PLRS];
uint32_t gdwLargestMsgSize;
uint32_t gdwNormalMsgSize;
int last_tick;
float gfProgressToNextGameTick = 0.0;

namespace {

SdlMutex MemCrit;
DWORD gdwDeltaBytesSec;
bool nthread_should_run;
char sgbSyncCountdown;
uint32_t turn_upper_bit;
bool sgbTicsOutOfSync;
char sgbPacketCountdown;
bool sgbThreadIsRunning;
SdlThread Thread;

void NthreadHandler()
{
	if (!nthread_should_run) {
		return;
	}

	while (true) {
		MemCrit.lock();
		if (!nthread_should_run) {
			MemCrit.unlock();
			break;
		}
		nthread_send_and_recv_turn(0, 0);
		int delta = gnTickDelay;
		if (nthread_recv_turns())
			delta = last_tick - SDL_GetTicks();
		MemCrit.unlock();
		if (delta > 0)
			SDL_Delay(delta);
		if (!nthread_should_run)
			return;
	}
}

} // namespace

void nthread_terminate_game(const char *pszFcn)
{
	uint32_t sErr = SErrGetLastError();
	if (sErr == STORM_ERROR_INVALID_PLAYER) {
		return;
	}
	if (sErr != STORM_ERROR_GAME_TERMINATED && sErr != STORM_ERROR_NOT_IN_GAME) {
		app_fatal("%s:\n%s", pszFcn, SDL_GetError());
	}

	gbGameDestroyed = true;
}

uint32_t nthread_send_and_recv_turn(uint32_t curTurn, int turnDelta)
{
	uint32_t curTurnsInTransit;
	if (!SNetGetTurnsInTransit(&curTurnsInTransit)) {
		nthread_terminate_game("SNetGetTurnsInTransit");
		return 0;
	}
	while (curTurnsInTransit++ < gdwTurnsInTransit) {

		uint32_t turnTmp = turn_upper_bit | (curTurn & 0x7FFFFFFF);
		turn_upper_bit = 0;
		uint32_t turn = turnTmp;

		if (!SNetSendTurn((char *)&turn, sizeof(turn))) {
			nthread_terminate_game("SNetSendTurn");
			return 0;
		}

		curTurn += turnDelta;
		if (curTurn >= 0x7FFFFFFF)
			curTurn &= 0xFFFF;
	}
	return curTurn;
}

bool nthread_recv_turns(bool *pfSendAsync)
{
	if (pfSendAsync != nullptr)
		*pfSendAsync = false;
	sgbPacketCountdown--;
	if (sgbPacketCountdown > 0) {
		last_tick += gnTickDelay;
		return true;
	}
	sgbSyncCountdown--;
	sgbPacketCountdown = sgbNetUpdateRate;
	if (sgbSyncCountdown != 0) {
		if (pfSendAsync != nullptr)
			*pfSendAsync = true;
		last_tick += gnTickDelay;
		return true;
	}
	if (!SNetReceiveTurns(MAX_PLRS, (char **)glpMsgTbl, gdwMsgLenTbl, &player_state[0])) {
		if (SErrGetLastError() != STORM_ERROR_NO_MESSAGES_WAITING)
			nthread_terminate_game("SNetReceiveTurns");
		sgbTicsOutOfSync = false;
		sgbSyncCountdown = 1;
		sgbPacketCountdown = 1;
		return false;
	}
	if (!sgbTicsOutOfSync) {
		sgbTicsOutOfSync = true;
		last_tick = SDL_GetTicks();
	}
	sgbSyncCountdown = 4;
	multi_msg_countdown();
	if (pfSendAsync != nullptr)
		*pfSendAsync = true;
	last_tick += gnTickDelay;
	return true;
}

void nthread_set_turn_upper_bit()
{
	turn_upper_bit = 0x80000000;
}

void nthread_start(bool setTurnUpperBit)
{
	last_tick = SDL_GetTicks();
	sgbPacketCountdown = 1;
	sgbSyncCountdown = 1;
	sgbTicsOutOfSync = true;
	if (setTurnUpperBit)
		nthread_set_turn_upper_bit();
	else
		turn_upper_bit = 0;
	_SNETCAPS caps;
	caps.size = 36;
	SNetGetProviderCaps(&caps);
	gdwTurnsInTransit = caps.defaultturnsintransit;
	if (gdwTurnsInTransit == 0)
		gdwTurnsInTransit = 1;
	if (caps.defaultturnssec <= 20 && caps.defaultturnssec != 0)
		sgbNetUpdateRate = 20 / caps.defaultturnssec;
	else
		sgbNetUpdateRate = 1;
	uint32_t largestMsgSize = 512;
	if (caps.maxmessagesize < 0x200)
		largestMsgSize = caps.maxmessagesize;
	gdwDeltaBytesSec = caps.bytessec / 4;
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
	if (gbIsMultiplayer) {
		sgbThreadIsRunning = false;
		MemCrit.lock();
		nthread_should_run = true;
		Thread = SdlThread { NthreadHandler };
	}
}

void nthread_cleanup()
{
	nthread_should_run = false;
	gdwTurnsInTransit = 0;
	gdwNormalMsgSize = 0;
	gdwLargestMsgSize = 0;
	if (Thread.joinable() && Thread.get_id() != this_sdl_thread::get_id()) {
		if (!sgbThreadIsRunning)
			MemCrit.unlock();
		Thread.join();
	}
}

void nthread_ignore_mutex(bool bStart)
{
	if (!Thread.joinable())
		return;

	if (bStart)
		MemCrit.unlock();
	else
		MemCrit.lock();
	sgbThreadIsRunning = bStart;
}

bool nthread_has_500ms_passed()
{
	int currentTickCount = SDL_GetTicks();
	int ticksElapsed = currentTickCount - last_tick;
	if (!gbIsMultiplayer && ticksElapsed > gnTickDelay * 10) {
		last_tick = currentTickCount;
		ticksElapsed = 0;
	}
	return ticksElapsed >= 0;
}

void nthread_UpdateProgressToNextGameTick()
{
	if (!gbRunGame || PauseMode != 0 || (!gbIsMultiplayer && gmenu_is_active()) || !gbProcessPlayers || demo::IsRunning()) // if game is not running or paused there is no next gametick in the near future
		return;
	int currentTickCount = SDL_GetTicks();
	int ticksElapsed = last_tick - currentTickCount;
	if (ticksElapsed <= 0) {
		gfProgressToNextGameTick = 1.0; // game tick is due
		return;
	}
	int ticksAdvanced = gnTickDelay - ticksElapsed;
	float fraction = (float)ticksAdvanced / (float)gnTickDelay;
	if (fraction > 1.0)
		gfProgressToNextGameTick = 1.0;
	if (fraction < 0.0)
		gfProgressToNextGameTick = 0.0;
	gfProgressToNextGameTick = fraction;
}

} // namespace devilution
