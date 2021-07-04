#pragma once

#include <SDL.h>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

namespace devilution {

typedef struct event_emul {
	SDL_mutex *mutex;
	SDL_cond *cond;
} event_emul;

event_emul *StartEvent();
void EndEvent(event_emul *event);
void SetEvent(event_emul *e);
void ResetEvent(event_emul *e);
int WaitForEvent(event_emul *e);
SDL_Thread *CreateThread(void (*handler)(), SDL_threadID *ThreadID);

} // namespace devilution
