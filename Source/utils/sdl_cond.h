#pragma once

#include <SDL.h>

#include "sdl_mutex.h"

namespace devilution {

/**
 * RAII wrapper for SDL_cond.
 */
class SdlCond final {
public:
	SdlCond()
	    : cond(SDL_CreateCond())
	{
		if (cond == nullptr)
			ErrSdl();
	}

	~SdlCond()
	{
		SDL_DestroyCond(cond);
	}

	SdlCond(const SdlCond &) = delete;
	SdlCond(SdlCond &&) = delete;
	SdlCond &operator=(const SdlCond &) = delete;
	SdlCond &operator=(SdlCond &&) = delete;

	void signal()
	{
		int err = SDL_CondSignal(cond);
		if (err < 0)
			ErrSdl();
	}

	void wait(SdlMutex &mutex)
	{
		int err = SDL_CondWait(cond, mutex.get());
		if (err < 0)
			ErrSdl();
	}

private:
	SDL_cond *cond;
};

} // namespace devilution
