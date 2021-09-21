#pragma once

#include <memory>

#include <SDL_mutex.h>

#include "appfat.h"

namespace devilution {

/*
 * RAII wrapper for SDL_mutex. Satisfies std's "Lockable" (SDL 2) or "BasicLockable" (SDL 1)
 * requirements so it can be used with std::lock_guard and friends.
 */
class SdlMutex final {
public:
	SdlMutex()
	    : mutex_(SDL_CreateMutex())
	{
		if (mutex_ == nullptr)
			ErrSdl();
	}

	~SdlMutex()
	{
		SDL_DestroyMutex(mutex_);
	}

	SdlMutex(const SdlMutex &) = delete;
	SdlMutex(SdlMutex &&) = delete;
	SdlMutex &operator=(const SdlMutex &) = delete;
	SdlMutex &operator=(SdlMutex &&) = delete;

	void lock() noexcept // NOLINT(readability-identifier-naming)
	{
		int err = SDL_LockMutex(mutex_);
		if (err == -1)
			ErrSdl();
	}

	bool try_lock() noexcept // NOLINT(readability-identifier-naming)
	{
		int err = SDL_TryLockMutex(mutex_);
		if (err == -1)
			ErrSdl();
		return err == 0;
	}

	void unlock() noexcept // NOLINT(readability-identifier-naming)
	{
		int err = SDL_UnlockMutex(mutex_);
		if (err == -1)
			ErrSdl();
	}

	SDL_mutex *get()
	{
		return mutex_;
	}

private:
	SDL_mutex *mutex_;
};

} // namespace devilution
