#include "pch.h"

static std::set<uintptr_t> threads;
static std::set<uintptr_t> events;

struct event_emul {
	SDL_mutex *mutex;
	SDL_cond *cond;
};

uintptr_t __cdecl _beginthreadex(void *_Security, unsigned _StackSize, unsigned(__stdcall *_StartAddress)(void *),
    void *_ArgList, unsigned _InitFlag, unsigned *_ThrdAddr)
{
	if (_Security != NULL)
		UNIMPLEMENTED();
	if (_StackSize != 0)
		UNIMPLEMENTED();
	if (_InitFlag != 0)
		UNIMPLEMENTED();
	// WARNING: wrong return type of _StartAddress
	SDL_Thread *ret = SDL_CreateThread((SDL_ThreadFunction)_StartAddress, NULL, _ArgList);
	*_ThrdAddr = SDL_GetThreadID(ret);
	threads.insert((uintptr_t)ret);
	return (uintptr_t)ret;
}

DWORD WINAPI GetCurrentThreadId(VOID)
{
	// DWORD is compatible with SDL_threadID
	return SDL_GetThreadID(NULL);
}

HANDLE WINAPI GetCurrentThread(VOID)
{
	// Only used for SetThreadPriority, which is unimplemented
	return NULL;
}

WINBOOL WINAPI SetThreadPriority(HANDLE hThread, int nPriority)
{
	// SDL cannot set the priority of the non-current thread
	// (and e.g. unprivileged processes on Linux cannot increase it)
	return TRUE;
}

VOID WINAPI InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	SDL_mutex *m = SDL_CreateMutex();
	*lpCriticalSection = m;
}

VOID WINAPI EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	SDL_LockMutex(*((SDL_mutex **)lpCriticalSection));
}

VOID WINAPI LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	SDL_UnlockMutex(*((SDL_mutex **)lpCriticalSection));
}

VOID WINAPI DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	SDL_DestroyMutex(*((SDL_mutex **)lpCriticalSection));
}

HANDLE WINAPI CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, WINBOOL bManualReset, WINBOOL bInitialState,
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
	if (bManualReset != TRUE)
		UNIMPLEMENTED();
	if (bInitialState != FALSE)
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

BOOL WINAPI SetEvent(HANDLE hEvent)
{
	struct event_emul *e = (struct event_emul *)hEvent;
	SDL_LockMutex(e->mutex);
	SDL_CondSignal(e->cond);
	SDL_UnlockMutex(e->mutex);
	return 1;
}

BOOL WINAPI ResetEvent(HANDLE hEvent)
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
	if (dwMilliseconds == INFINITE)
		ret = SDL_CondWait(e->cond, e->mutex);
	else
		ret = SDL_CondWaitTimeout(e->cond, e->mutex, dwMilliseconds);
	SDL_CondSignal(e->cond);
	SDL_UnlockMutex(e->mutex);
	return ret;
}

static DWORD wait_for_sdl_thread(HANDLE hHandle, DWORD dwMilliseconds)
{
	if (dwMilliseconds != INFINITE)
		UNIMPLEMENTED();
	SDL_Thread *t = (SDL_Thread *)hHandle;
	SDL_WaitThread(t, NULL);
	return 0;
}

DWORD WINAPI WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds)
{
	// return value different from WinAPI
	if (threads.find((uintptr_t)hHandle) != threads.end())
		return wait_for_sdl_thread(hHandle, dwMilliseconds);
	if (events.find((uintptr_t)hHandle) != threads.end())
		return wait_for_sdl_cond(hHandle, dwMilliseconds);
	UNIMPLEMENTED();
}
