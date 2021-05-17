#include "push_aulib_decoder.h"

#include <algorithm>
#include <cstring>
#include <limits>
#include <mutex>

#include <aulib.h>

#include "appfat.h"

namespace devilution {

void PushAulibDecoder::PushSamples(const std::int16_t *data, unsigned size) noexcept
{
	AudioQueueItem item;
	item.data.reset(new std::int16_t[size]);
	std::memcpy(item.data.get(), data, size * sizeof(data[0]));
	item.len = size;
	item.pos = item.data.get();
	const std::lock_guard<SdlMutex> lock(queue_mutex_);
	queue_.push(std::move(item));
}

void PushAulibDecoder::PushSamples(const std::uint8_t *data, unsigned size) noexcept
{
	AudioQueueItem item;
	item.data.reset(new std::int16_t[size]);
	constexpr std::int16_t Center = 128;
	constexpr std::int16_t Scale = 256;
	for (unsigned i = 0; i < size; ++i)
		item.data[i] = static_cast<std::int16_t>((data[i] - Center) * Scale);
	item.len = size;
	item.pos = item.data.get();
	const std::lock_guard<SdlMutex> lock(queue_mutex_);
	queue_.push(std::move(item));
}

void PushAulibDecoder::DiscardPendingSamples() noexcept
{
	const std::lock_guard<SdlMutex> lock(queue_mutex_);
	queue_ = std::queue<AudioQueueItem>();
}

bool PushAulibDecoder::open([[maybe_unused]] SDL_RWops *rwops)
{
	assert(rwops == nullptr);
	return true;
}

bool PushAulibDecoder::rewind()
{
	return false;
}

std::chrono::microseconds PushAulibDecoder::duration() const
{
	return {};
}

bool PushAulibDecoder::seekToTime([[maybe_unused]] std::chrono::microseconds pos)
{
	return false;
}

int PushAulibDecoder::doDecoding(float buf[], int len, bool &callAgain)
{
	callAgain = false;

	const auto writeFloats = [&buf](const std::int16_t *samples, unsigned count) {
		constexpr float Scale = std::numeric_limits<std::int16_t>::max() + 1;
		for (unsigned i = 0; i < count; ++i) {
			buf[i] = static_cast<float>(samples[i]) / Scale;
		}
	};

	unsigned remaining = len;
	{
		const std::lock_guard<SdlMutex> lock(queue_mutex_);
		AudioQueueItem *item;
		while ((item = Next()) != nullptr) {
			if (static_cast<unsigned>(remaining) <= item->len) {
				writeFloats(item->pos, remaining);
				item->pos += remaining;
				item->len -= remaining;
				return len;
			}

			writeFloats(item->pos, item->len);
			buf += item->len;
			remaining -= static_cast<int>(item->len);
			queue_.pop();
		}
	}
	std::memset(buf, 0, remaining * sizeof(buf[0]));
	return len;
}

PushAulibDecoder::AudioQueueItem *PushAulibDecoder::Next()
{
	while (!queue_.empty() && queue_.front().len == 0)
		queue_.pop();
	if (queue_.empty())
		return nullptr;
	return &queue_.front();
}

} // namespace devilution
