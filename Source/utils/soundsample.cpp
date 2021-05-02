#include "utils/soundsample.h"

#include <cmath>
#include <chrono>

#include <Aulib/DecoderDrwav.h>
#include <Aulib/ResamplerSpeex.h>
#include <SDL.h>
#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#else
#include "utils/sdl2_backports.h"
#endif

#include "options.h"
#include "storm/storm_sdl_rw.h"
#include "storm/storm.h"
#include "utils/log.hpp"
#include "utils/stubs.h"

namespace devilution {

void SoundSample::Release()
{
	stream_ = nullptr;
	file_data_ = nullptr;
	file_data_size_ = 0;
};

/**
 * @brief Check if a the sound is being played atm
 */
bool SoundSample::IsPlaying()
{
	return stream_ && stream_->isPlaying();
};

/**
 * @brief Start playing the sound
 */
void SoundSample::Play(int lVolume, int lPan, int channel)
{
	if (!stream_)
		return;

	constexpr float Base = 10.F;
	constexpr float Scale = 2000.F;
	stream_->setVolume(std::pow(Base, static_cast<float>(lVolume) / Scale));
	stream_->setStereoPosition(
	    lPan == 0 ? 0
	              : copysign(1.F - std::pow(Base, static_cast<float>(-std::fabs(lPan) / Scale)),
	                  static_cast<float>(lPan)));

	if (!stream_->play()) {
		LogError(LogCategory::Audio, "Aulib::Stream::play (from SoundSample::Play): {}", SDL_GetError());
		return;
	}
};

/**
 * @brief Stop playing the sound
 */
void SoundSample::Stop()
{
	if (stream_)
		stream_->stop();
};

int SoundSample::SetChunkStream(std::string filePath)
{
	file_path_ = std::move(filePath);
	HANDLE handle;
	if (!SFileOpenFile(file_path_.c_str(), &handle)) {
		LogError(LogCategory::Audio, "SFileOpenFile failed (from SoundSample::SetChunkStream): {}", SErrGetLastError());
		return -1;
	}

	stream_ = std::make_unique<Aulib::Stream>(SFileRw_FromStormHandle(handle), std::make_unique<Aulib::DecoderDrwav>(),
	    std::make_unique<Aulib::ResamplerSpeex>(sgOptions.Audio.nResamplingQuality), /*closeRw=*/true);
	if (!stream_->open()) {
		stream_ = nullptr;
		LogError(LogCategory::Audio, "Aulib::Stream::open (from SoundSample::SetChunkStream): {}", SDL_GetError());
		return -1;
	}
	return 0;
}

int SoundSample::SetChunk(ArraySharedPtr<std::uint8_t> fileData, std::size_t dwBytes)
{
	file_data_ = fileData;
	file_data_size_ = dwBytes;
	SDL_RWops *buf = SDL_RWFromConstMem(file_data_.get(), dwBytes);
	if (buf == nullptr) {
		return -1;
	}

	stream_ = std::make_unique<Aulib::Stream>(buf, std::make_unique<Aulib::DecoderDrwav>(),
	    std::make_unique<Aulib::ResamplerSpeex>(sgOptions.Audio.nResamplingQuality), /*closeRw=*/true);
	if (!stream_->open()) {
		stream_ = nullptr;
		file_data_ = nullptr;
		LogError(LogCategory::Audio, "Aulib::Stream::open (from SoundSample::SetChunk): {}", SDL_GetError());
		return -1;
	}

	return 0;
};

/**
 * @return Audio duration in ms
 */
int SoundSample::GetLength() const
{
	if (!stream_)
		return 0;
	return std::chrono::duration_cast<std::chrono::milliseconds>(stream_->duration()).count();
};

} // namespace devilution
