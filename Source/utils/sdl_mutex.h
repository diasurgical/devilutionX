#pragma once

#include <memory>

#include <SDL_mutex.h>

namespace devilution {

/**
 * @brief Deletes the SDL mutex using `SDL_DestroyMutex`.
 */
struct SDLMutexDeleter {
	void operator()(SDL_mutex *mutex) const
	{
		SDL_DestroyMutex(mutex);
	}
};

using SDLMutexUniquePtr = std::unique_ptr<SDL_mutex, SDLMutexDeleter>;

struct SDLMutexLockGuard {
public:
	explicit SDLMutexLockGuard(SDL_mutex *mutex)
	    : mutex_(mutex)
	{
		SDL_LockMutex(mutex_);
	}

	~SDLMutexLockGuard()
	{
		SDL_UnlockMutex(mutex_);
	}

	SDLMutexLockGuard(const SDLMutexLockGuard &) = delete;
	SDLMutexLockGuard(SDLMutexLockGuard &&) = delete;
	SDLMutexLockGuard &operator=(const SDLMutexLockGuard &) = delete;
	SDLMutexLockGuard &operator=(SDLMutexLockGuard &&) = delete;

private:
	SDL_mutex *mutex_;
};

} // namespace devilution
