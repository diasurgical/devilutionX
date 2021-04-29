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
#include "utils/stubs.h"
#include "utils/log.hpp"

namespace devilution {

///// SoundSample /////

void SoundSample::Release()
{
	stream_ = std::nullopt;
	file_data_ = nullptr;
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

	stream_->rewind();
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

int SoundSample::SetChunkStream(HANDLE stormHandle)
{
	stream_.emplace(SFileRw_FromStormHandle(stormHandle), std::make_unique<Aulib::DecoderDrwav>(),
	    std::make_unique<Aulib::ResamplerSpeex>(sgOptions.Audio.nResamplingQuality), /*closeRw=*/true);
	if (!stream_->open()) {
		stream_ = std::nullopt;
		LogError(LogCategory::Audio, "Aulib::Stream::open (from SoundSample::SetChunkStream): {}", SDL_GetError());
		return -1;
	}
	return 0;
}

int SoundSample::SetChunk(std::unique_ptr<std::uint8_t[]> fileData, size_t dwBytes)
{
	file_data_ = std::move(fileData);
	SDL_RWops *buf = SDL_RWFromConstMem(file_data_.get(), dwBytes);
	if (buf == nullptr) {
		return -1;
	}

	stream_.emplace(buf, std::make_unique<Aulib::DecoderDrwav>(),
	    std::make_unique<Aulib::ResamplerSpeex>(sgOptions.Audio.nResamplingQuality), /*closeRw=*/true);
	if (!stream_->open()) {
		stream_ = std::nullopt;
		file_data_ = nullptr;
		LogError(LogCategory::Audio, "Aulib::Stream::open (from SoundSample::SetChunk): {}", SDL_GetError());
		return -1;
	}

	return 0;
};

/**
 * @return Audio duration in ms
 */
int SoundSample::GetLength()
{
	if (!stream_)
		return 0;
	return std::chrono::duration_cast<std::chrono::milliseconds>(stream_->duration()).count();
};

} // namespace devilution
