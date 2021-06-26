#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include <Aulib/Stream.h>

#include "utils/stdcompat/shared_ptr_array.hpp"

namespace devilution {

/**
 * @brief Converts log volume passed in into linear volume.
 * @param logVolume Logarithmic volume in the range [logMin..logMax]
 * @param logMin Volume range minimum (usually ATTENUATION_MIN for game sounds and VOLUME_MIN for volume sliders)
 * @param logMax Volume range maximum (usually 0)
 * @return Linear volume in the range [0..1]
*/
float VolumeLogToLinear(int logVolume, int logMin, int logMax);

class SoundSample final {
public:
	SoundSample() = default;
	SoundSample(SoundSample &&) noexcept = default;
	SoundSample &operator=(SoundSample &&) noexcept = default;

	void Release();
	bool IsPlaying();
	void Play(int logSoundVolume, int logUserVolume, int logPan, int channel = -1);
	void Stop();
	int SetChunkStream(std::string filePath);

	void SetFinishCallback(Aulib::Stream::Callback &&callback)
	{
		stream_->setFinishCallback(std::forward<Aulib::Stream::Callback>(callback));
	}

#ifndef STREAM_ALL_AUDIO
	/**
	 * @brief Sets the sample's WAV, FLAC, or Ogg/Vorbis data.
	 * @param fileData Buffer containing the data
	 * @param dwBytes Length of buffer
	 * @return 0 on success, -1 otherwise
	 */
	int SetChunk(ArraySharedPtr<std::uint8_t> fileData, std::size_t dwBytes);
#endif

#ifndef STREAM_ALL_AUDIO
	[[nodiscard]] bool IsStreaming() const
	{
		return file_data_ == nullptr;
	}
#endif

	int DuplicateFrom(const SoundSample &other)
	{
#ifdef STREAM_ALL_AUDIO
		return SetChunkStream(other.file_path_);
#else
		if (other.IsStreaming())
			return SetChunkStream(other.file_path_);
		return SetChunk(other.file_data_, other.file_data_size_);
#endif
	}

	int GetLength() const;

private:
#ifndef STREAM_ALL_AUDIO
	// Non-streaming audio fields:
	ArraySharedPtr<std::uint8_t> file_data_;
	std::size_t file_data_size_;
#endif

	// Set for streaming audio to allow for duplicating it:
	std::string file_path_;

	std::unique_ptr<Aulib::Stream> stream_;
};

} // namespace devilution
