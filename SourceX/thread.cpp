#include "all.h"
#include "stubs.h"
#include <SDL.h>
#include <set>

namespace devilution {

static int SDLCALL thread_translate(void *ptr)
{
	unsigned int (*handler)(void *) = (unsigned int (*)(void *))ptr;

	return handler(nullptr);
}

SDL_Thread *CreateThread(unsigned int (*handler)(void *), SDL_threadID *ThreadID)
{
#ifdef USE_SDL1
	SDL_Thread *ret = SDL_CreateThread(thread_translate, (void *)handler);
#else
	SDL_Thread *ret = SDL_CreateThread(thread_translate, nullptr, (void *)handler);
#endif
	if (ret == nullptr) {
		ErrSdl();
	}
	*ThreadID = SDL_GetThreadID(ret);
	return ret;
}

event_emul *StartEvent()
{
	event_emul *ret;
	ret = (event_emul *)malloc(sizeof(event_emul));
	ret->mutex = SDL_CreateMutex();
	if (ret->mutex == nullptr) {
		ErrSdl();
	}
	ret->cond = SDL_CreateCond();
	if (ret->cond == nullptr) {
		ErrSdl();
	}
	return ret;
}

void EndEvent(event_emul *event)
{
	SDL_DestroyCond(event->cond);
	SDL_DestroyMutex(event->mutex);
	free(event);
}

void SetEvent(event_emul *e)
{
	if (SDL_LockMutex(e->mutex) <= -1 || SDL_CondSignal(e->cond) <= -1 || SDL_UnlockMutex(e->mutex) <= -1) {
		ErrSdl();
	}
}

void ResetEvent(event_emul *e)
{
	if (SDL_LockMutex(e->mutex) <= -1 || SDL_CondWaitTimeout(e->cond, e->mutex, 0) <= -1 || SDL_UnlockMutex(e->mutex) <= -1) {
		ErrSdl();
	}
}

int WaitForEvent(event_emul *e)
{
	if (SDL_LockMutex(e->mutex) <= -1) {
		ErrSdl();
	}
	int ret = SDL_CondWait(e->cond, e->mutex);
	if (ret <= -1 || SDL_CondSignal(e->cond) <= -1 || SDL_UnlockMutex(e->mutex) <= -1) {
		SDL_Log(SDL_GetError());
		return -1;
	}
	return ret;
}

} // namespace devilution
