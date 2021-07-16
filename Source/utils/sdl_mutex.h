#pragma once

#include <memory>

#include <SDL_mutex.h>
#include <SDL_version.h>

#include "../appfat.h"

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
		SDL_LockMutex(mutex_);
	}

#if SDL_VERSION_ATLEAST(2, 0, 0)
	bool try_lock() noexcept // NOLINT(readability-identifier-naming)
	{
		return SDL_TryLockMutex(mutex_) == 0;
	}
#endif

	void unlock() noexcept // NOLINT(readability-identifier-naming)
	{
		SDL_UnlockMutex(mutex_);
	}

private:
	SDL_mutex *mutex_;
};

} // namespace devilution
