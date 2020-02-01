#include "devilution.h"
#include "stubs.h"
#include <SDL.h>
#include <set>

namespace dvl {

static std::set<uintptr_t> threads;
static std::set<uintptr_t> events;

struct event_emul {
	SDL_mutex *mutex;
	SDL_cond *cond;
};

struct func_translate {
	unsigned int (*func)(void *);
	void *arg;
};

static int SDLCALL thread_translate(void *ptr)
{
	func_translate *ftptr = static_cast<func_translate *>(ptr);
	auto ret = ftptr->func(ftptr->arg);
	delete ftptr;
	return ret;
}

uintptr_t DVL_beginthreadex(void *_Security, unsigned _StackSize, unsigned (*_StartAddress)(void *),
    void *_ArgList, unsigned _InitFlag, unsigned *_ThrdAddr)
{
	if (_Security != NULL)
		UNIMPLEMENTED();
	if (_StackSize != 0)
		UNIMPLEMENTED();
	if (_InitFlag != 0)
		UNIMPLEMENTED();
	func_translate *ft = new func_translate;
	ft->func = _StartAddress;
	ft->arg = _ArgList;
#ifdef USE_SDL1
	SDL_Thread *ret = SDL_CreateThread(thread_translate, ft);
#else
	SDL_Thread *ret = SDL_CreateThread(thread_translate, NULL, ft);
#endif
	if (ret == NULL) {
		ErrSdl();
	}
	*_ThrdAddr = SDL_GetThreadID(ret);
	threads.insert((uintptr_t)ret);
	return (uintptr_t)ret;
}

DWORD GetCurrentThreadId()
{
	// DWORD is compatible with SDL_threadID
	return SDL_GetThreadID(NULL);
}

bool SetThreadPriority(HANDLE hThread, int nPriority)
{
	// SDL cannot set the priority of the non-current thread
	// (and e.g. unprivileged processes on Linux cannot increase it)
	return true;
}

void InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	SDL_mutex *m = SDL_CreateMutex();
	if (m == NULL) {
		ErrSdl();
	}
	*lpCriticalSection = m;
}

void EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	if (SDL_LockMutex(*((SDL_mutex **)lpCriticalSection)) <= -1) {
		ErrSdl();
	}
}

void LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	if (SDL_UnlockMutex(*((SDL_mutex **)lpCriticalSection)) <= -1) {
		ErrSdl();
	}
}

void DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	SDL_DestroyMutex(*((SDL_mutex **)lpCriticalSection));
}

HANDLE CreateEventA()
{
	struct event_emul *ret;
	ret = (struct event_emul *)malloc(sizeof(struct event_emul));
	ret->mutex = SDL_CreateMutex();
	if (ret->mutex == NULL) {
		ErrSdl();
	}
	ret->cond = SDL_CreateCond();
	if (ret->cond == NULL) {
		ErrSdl();
	}
	events.insert((uintptr_t)ret);
	return ret;
}

BOOL CloseEvent(HANDLE hObject)
{
	struct event_emul *event = static_cast<struct event_emul *>(hObject);
	if (events.find((uintptr_t)event) == events.end())
		return false;
	SDL_DestroyCond(event->cond);
	SDL_DestroyMutex(event->mutex);
	events.erase((uintptr_t)event);
	free(event);
	return true;
}

void SetEvent(HANDLE hEvent)
{
	struct event_emul *e = (struct event_emul *)hEvent;
	if (SDL_LockMutex(e->mutex) <= -1 || SDL_CondSignal(e->cond) <= -1 || SDL_UnlockMutex(e->mutex) <= -1) {
		ErrSdl();
	}
}

void ResetEvent(HANDLE hEvent)
{
	struct event_emul *e = (struct event_emul *)hEvent;
	if (SDL_LockMutex(e->mutex) <= -1 || SDL_CondWaitTimeout(e->cond, e->mutex, 0) <= -1 || SDL_UnlockMutex(e->mutex) <= -1) {
		ErrSdl();
	}
}

static int wait_for_sdl_cond(HANDLE hHandle, DWORD dwMilliseconds)
{
	struct event_emul *e = (struct event_emul *)hHandle;
	if (SDL_LockMutex(e->mutex) <= -1) {
		ErrSdl();
	}
	int ret;
	if (dwMilliseconds == DVL_INFINITE)
		ret = SDL_CondWait(e->cond, e->mutex);
	else
		ret = SDL_CondWaitTimeout(e->cond, e->mutex, dwMilliseconds);
	if (ret <= -1 || SDL_CondSignal(e->cond) <= -1 || SDL_UnlockMutex(e->mutex) <= -1) {
		SDL_Log(SDL_GetError());
		return -1;
	}
	return ret;
}

static int wait_for_sdl_thread(HANDLE hHandle, DWORD dwMilliseconds)
{
	if (dwMilliseconds != DVL_INFINITE)
		UNIMPLEMENTED();
	SDL_Thread *t = (SDL_Thread *)hHandle;
	SDL_WaitThread(t, NULL);
	return 0;
}

int WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds)
{
	// return value different from WinAPI
	if (threads.find((uintptr_t)hHandle) != threads.end())
		return wait_for_sdl_thread(hHandle, dwMilliseconds);
	if (events.find((uintptr_t)hHandle) != events.end())
		return wait_for_sdl_cond(hHandle, dwMilliseconds);
	UNIMPLEMENTED();
}

}
