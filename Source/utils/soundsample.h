#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include <Aulib/Stream.h>

#include "miniwin/miniwin.h"
#include "utils/stdcompat/shared_ptr_array.hpp"

namespace devilution {

class SoundSample final {
public:
	SoundSample() = default;
	SoundSample(SoundSample &&) noexcept = default;
	SoundSample &operator=(SoundSample &&) noexcept = default;

	void Release();
	bool IsPlaying();
	void Play(int lVolume, int lPan, int channel = -1);
	void Stop();
	int SetChunkStream(std::string filePath);

	/**
	 * @brief Sets the sample's WAV, FLAC, or Ogg/Vorbis data.
	 * @param fileData Buffer containing the data
	 * @param dwBytes Length of buffer
	 * @return 0 on success, -1 otherwise
	 */
	int SetChunk(ArraySharedPtr<std::uint8_t> file_data, std::size_t dwBytes);

	[[nodiscard]] bool IsStreaming() const
	{
		return file_data_ == nullptr;
	}

	int DuplicateFrom(const SoundSample &other)
	{
		if (other.IsStreaming())
			return SetChunkStream(other.file_path_);
		return SetChunk(other.file_data_, other.file_data_size_);
	}

	int GetLength() const;

private:
	// Non-streaming audio fields:
	ArraySharedPtr<std::uint8_t> file_data_;
	std::size_t file_data_size_;

	// Set for streaming audio to allow for duplicating it:
	std::string file_path_;

	std::unique_ptr<Aulib::Stream> stream_;
};

} // namespace devilution
