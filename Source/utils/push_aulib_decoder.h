#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <queue>

#include <Aulib/Decoder.h>
#include <SDL_mutex.h>

#include "utils/sdl_mutex.h"

namespace devilution {

/**
 * @brief A Decoder interface implementations that simply has the samples pushed into it by the user.
 */
class PushAulibDecoder final : public ::Aulib::Decoder {
public:
	PushAulibDecoder(int numChannels, int sampleRate)
	    : numChannels_(numChannels)
	    , sampleRate_(sampleRate)
	{
	}

	void PushSamples(const std::uint8_t *data, unsigned size) noexcept;
	void PushSamples(const std::int16_t *data, unsigned size) noexcept;
	void DiscardPendingSamples() noexcept;

	bool open(SDL_RWops *rwops) override;

	[[nodiscard]] int getChannels() const override
	{
		return numChannels_;
	}

	[[nodiscard]] int getRate() const override
	{
		return sampleRate_;
	}

	bool rewind() override;
	[[nodiscard]] std::chrono::microseconds duration() const override;
	bool seekToTime(std::chrono::microseconds pos) override;

protected:
	int doDecoding(float buf[], int len, bool &callAgain) override;

private:
	struct AudioQueueItem {
		std::unique_ptr<std::int16_t[]> data;
		unsigned len;
		const std::int16_t *pos;
	};

	const int numChannels_;
	const int sampleRate_;

	// Requires holding the queue_mutex_.
	AudioQueueItem *Next();

	std::queue<AudioQueueItem> queue_;
	SdlMutex queue_mutex_;
};

} // namespace devilution
