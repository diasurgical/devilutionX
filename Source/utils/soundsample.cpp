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
#include "utils/math.h"
#include "utils/stubs.h"

namespace devilution {

namespace {

constexpr float LogBase = 10.0f;

/**
 * Scaling factor for attenuating volume.
 * Picked so that a volume change of -10 dB results in half perceived loudness.
 * VolumeScale = -1000 / log(0.5)
 */
constexpr float VolumeScale = 3321.9281f;

/**
 * Min and max volume range, in millibel.
 * -100 dB (muted) to 0 dB (max. loudness).
 */
constexpr float MillibelMin = -10000.0f;
constexpr float MillibelMax = 0.0f;

/**
 * Stereo separation factor for left/right speaker panning. Lower values increase separation, moving
 * sounds further left/right, while higher values will pull sounds more towards the middle, reducing separation.
 * Current value is tuned to have ~2:1 mix for sounds that happen on the edge of a 640x480 screen.
 */
constexpr float StereoSeparation = 6000.0f;

float PanLogToLinear(int logPan)
{
	if (logPan == 0)
		return 0;

	auto factor = std::pow(LogBase, static_cast<float>(-std::abs(logPan)) / StereoSeparation);

	return copysign(1.0f - factor, static_cast<float>(logPan));
}

} // namespace

float VolumeLogToLinear(int logVolume, int logMin, int logMax)
{
	const auto logScaled = math::Remap<float>(logMin, logMax, MillibelMin, MillibelMax, logVolume);
	const auto linVolume = std::pow(LogBase, static_cast<float>(logScaled) / VolumeScale);
	return linVolume;
}

///// SoundSample /////

void SoundSample::Release()
{
	stream_ = nullptr;
#ifndef STREAM_ALL_AUDIO
	file_data_ = nullptr;
	file_data_size_ = 0;
#endif
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
void SoundSample::Play(int logSoundVolume, int logUserVolume, int logPan, int channel)
{
	if (!stream_)
		return;

	const int combinedLogVolume = logSoundVolume + logUserVolume * (ATTENUATION_MIN / VOLUME_MIN);
	const float linearVolume = VolumeLogToLinear(combinedLogVolume, ATTENUATION_MIN, 0);
	stream_->setVolume(linearVolume);

	const float linearPan = PanLogToLinear(logPan);
	stream_->setStereoPosition(linearPan);

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

#ifndef STREAM_ALL_AUDIO
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
#endif

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
