#pragma once

#include <cstdint>
#include <memory>

#include <Aulib/Stream.h>

#include "miniwin/miniwin.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

class SoundSample final {
public:
	void Release();
	bool IsPlaying();
	void Play(int lVolume, int lPan, int channel = -1);
	void Stop();
	int SetChunkStream(HANDLE stormHandle);

	/**
	 * @brief Sets the sample's WAV, FLAC, or Ogg/Vorbis data.
	 * @param fileData Buffer containing the data
	 * @param dwBytes Length of buffer
	 * @return 0 on success, -1 otherwise
	 */
	int SetChunk(std::unique_ptr<std::uint8_t[]> file_data, size_t dwBytes);

	int GetLength();

private:
	std::unique_ptr<std::uint8_t[]> file_data_;
	std::optional<Aulib::Stream> stream_;
};

} // namespace devilution
