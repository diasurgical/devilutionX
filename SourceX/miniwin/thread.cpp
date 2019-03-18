#include <set>
#include <SDL.h>

#include "devilution.h"
#include "stubs.h"

namespace dvl {

static std::set<uintptr_t> threads;
static std::set<uintptr_t> events;

struct event_emul {
	SDL_mutex *mutex;
	SDL_cond *cond;
};

struct func_translate {
	unsigned int (*func)(void*);
	void* arg;
};

static int SDLCALL thread_translate(void* ptr)
{
	func_translate* ftptr = static_cast<func_translate*>(ptr);
	auto ret = ftptr->func(ftptr->arg);
	delete ftptr;
	return ret;
}

uintptr_t DVL_beginthreadex(void *_Security, unsigned _StackSize, unsigned(*_StartAddress)(void *),
    void *_ArgList, unsigned _InitFlag, unsigned *_ThrdAddr)
{
	if (_Security != NULL)
		UNIMPLEMENTED();
	if (_StackSize != 0)
		UNIMPLEMENTED();
	if (_InitFlag != 0)
		UNIMPLEMENTED();
	func_translate* ft = new func_translate;
	ft->func = _StartAddress;
	ft->arg = _ArgList;
	SDL_Thread *ret = SDL_CreateThread(thread_translate, NULL, ft);
	*_ThrdAddr = SDL_GetThreadID(ret);
	threads.insert((uintptr_t)ret);
	return (uintptr_t)ret;
}

DWORD GetCurrentThreadId()
{
	// DWORD is compatible with SDL_threadID
	return SDL_GetThreadID(NULL);
}

HANDLE GetCurrentThread()
{
	// Only used for SetThreadPriority, which is unimplemented
	return NULL;
}

WINBOOL SetThreadPriority(HANDLE hThread, int nPriority)
{
	// SDL cannot set the priority of the non-current thread
	// (and e.g. unprivileged processes on Linux cannot increase it)
	return true;
}

void InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	SDL_mutex *m = SDL_CreateMutex();
	*lpCriticalSection = m;
}

void EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	SDL_LockMutex(*((SDL_mutex **)lpCriticalSection));
}

void LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	SDL_UnlockMutex(*((SDL_mutex **)lpCriticalSection));
}

void DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	SDL_DestroyMutex(*((SDL_mutex **)lpCriticalSection));
}

HANDLE CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, WINBOOL bManualReset, WINBOOL bInitialState,
    LPCSTR lpName)
{
	if (lpName != NULL && !strcmp(lpName, "DiabloEvent")) {
		// This is used by diablo.cpp to check whether
		// the game is already running
		// (we do not want to replicate this behaviour anyway)
		return NULL;
	}
	if (lpEventAttributes != NULL)
		UNIMPLEMENTED();
	if (bManualReset != true)
		UNIMPLEMENTED();
	if (bInitialState != false)
		UNIMPLEMENTED();
	if (lpName != NULL)
		UNIMPLEMENTED();
	struct event_emul *ret;
	ret = (struct event_emul *)malloc(sizeof(struct event_emul));
	ret->mutex = SDL_CreateMutex();
	ret->cond = SDL_CreateCond();
	events.insert((uintptr_t)ret);
	return ret;
}

BOOL SetEvent(HANDLE hEvent)
{
	struct event_emul *e = (struct event_emul *)hEvent;
	SDL_LockMutex(e->mutex);
	SDL_CondSignal(e->cond);
	SDL_UnlockMutex(e->mutex);
	return 1;
}

BOOL ResetEvent(HANDLE hEvent)
{
	struct event_emul *e = (struct event_emul *)hEvent;
	SDL_LockMutex(e->mutex);
	SDL_CondWaitTimeout(e->cond, e->mutex, 0);
	SDL_UnlockMutex(e->mutex);
	return 1;
}

static DWORD wait_for_sdl_cond(HANDLE hHandle, DWORD dwMilliseconds)
{
	struct event_emul *e = (struct event_emul *)hHandle;
	SDL_LockMutex(e->mutex);
	DWORD ret;
	if (dwMilliseconds == DVL_INFINITE)
		ret = SDL_CondWait(e->cond, e->mutex);
	else
		ret = SDL_CondWaitTimeout(e->cond, e->mutex, dwMilliseconds);
	SDL_CondSignal(e->cond);
	SDL_UnlockMutex(e->mutex);
	return ret;
}

static DWORD wait_for_sdl_thread(HANDLE hHandle, DWORD dwMilliseconds)
{
	if (dwMilliseconds != DVL_INFINITE)
		UNIMPLEMENTED();
	SDL_Thread *t = (SDL_Thread *)hHandle;
	SDL_WaitThread(t, NULL);
	return 0;
}

DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds)
{
	// return value different from WinAPI
	if (threads.find((uintptr_t)hHandle) != threads.end())
		return wait_for_sdl_thread(hHandle, dwMilliseconds);
	if (events.find((uintptr_t)hHandle) != threads.end())
		return wait_for_sdl_cond(hHandle, dwMilliseconds);
	UNIMPLEMENTED();
}

}
