/**
 * @file sound.cpp
 *
 * Implementation of functions setting up the audio pipeline.
 */
#include "sound.h"

#include <cstdint>
#include <list>
#include <memory>
#include <mutex>

#include <Aulib/DecoderDrwav.h>
#include <Aulib/ResamplerSpeex.h>
#include <Aulib/Stream.h>
#include <SDL.h>
#include <aulib.h>

#include "init.h"
#include "options.h"
#include "storm/storm.h"
#include "storm/storm_sdl_rw.h"
#include "utils/log.hpp"
#include "utils/math.h"
#include "utils/sdl_mutex.h"
#include "utils/stdcompat/algorithm.hpp"
#include "utils/stdcompat/optional.hpp"
#include "utils/stdcompat/shared_ptr_array.hpp"
#include "utils/stubs.h"

namespace devilution {

bool gbSndInited;
/** The active background music track id. */
_music_id sgnMusicTrack = NUM_MUSIC;

namespace {

std::optional<Aulib::Stream> music;

#ifdef DISABLE_STREAMING_MUSIC
char *musicBuffer;
#endif

void LoadMusic(HANDLE handle)
{
#ifndef DISABLE_STREAMING_MUSIC
	SDL_RWops *musicRw = SFileRw_FromStormHandle(handle);
#else
	int bytestoread = SFileGetFileSize(handle);
	musicBuffer = new char[bytestoread];
	SFileReadFileThreadSafe(handle, musicBuffer, bytestoread);
	SFileCloseFileThreadSafe(handle);

	SDL_RWops *musicRw = SDL_RWFromConstMem(musicBuffer, bytestoread);
#endif
	music.emplace(musicRw, std::make_unique<Aulib::DecoderDrwav>(),
	    std::make_unique<Aulib::ResamplerSpeex>(sgOptions.Audio.nResamplingQuality), /*closeRw=*/true);
}

void CleanupMusic()
{
	music = std::nullopt;
	sgnMusicTrack = NUM_MUSIC;
#ifdef DISABLE_STREAMING_MUSIC
	if (musicBuffer != nullptr) {
		delete[] musicBuffer;
		musicBuffer = nullptr;
	}
#endif
}

std::list<std::unique_ptr<SoundSample>> duplicateSounds;
std::optional<SdlMutex> duplicateSoundsMutex;

SoundSample *DuplicateSound(const SoundSample &sound)
{
	auto duplicate = std::make_unique<SoundSample>();
	if (duplicate->DuplicateFrom(sound) != 0)
		return nullptr;
	auto *result = duplicate.get();
	decltype(duplicateSounds.begin()) it;
	{
		const std::lock_guard<SdlMutex> lock(*duplicateSoundsMutex);
		duplicateSounds.push_back(std::move(duplicate));
		it = duplicateSounds.end();
		--it;
	}
	result->SetFinishCallback([it]([[maybe_unused]] Aulib::Stream &stream) {
		const std::lock_guard<SdlMutex> lock(*duplicateSoundsMutex);
		duplicateSounds.erase(it);
	});
	return result;
}

} // namespace

/* data */

bool gbMusicOn = true;
/** Specifies whether sound effects are enabled. */
bool gbSoundOn = true;

/** Maps from track ID to track name in spawn. */
const char *const SpawnMusicTracks[NUM_MUSIC] = {
	"Music\\sTowne.wav",
	"Music\\sLvlA.wav",
	"Music\\sLvlA.wav",
	"Music\\sLvlA.wav",
	"Music\\sLvlA.wav",
	"Music\\DLvlE.wav",
	"Music\\DLvlF.wav",
	"Music\\sintro.wav",
};
/** Maps from track ID to track name. */
const char *const MusicTracks[NUM_MUSIC] = {
	"Music\\DTowne.wav",
	"Music\\DLvlA.wav",
	"Music\\DLvlB.wav",
	"Music\\DLvlC.wav",
	"Music\\DLvlD.wav",
	"Music\\DLvlE.wav",
	"Music\\DLvlF.wav",
	"Music\\Dintro.wav",
};

static int CapVolume(int volume)
{
	return clamp(volume, VOLUME_MIN, VOLUME_MAX);
}

void ClearDuplicateSounds()
{
	const std::lock_guard<SdlMutex> lock(*duplicateSoundsMutex);
	duplicateSounds.clear();
}

void snd_play_snd(TSnd *pSnd, int lVolume, int lPan)
{
	DWORD tc;

	if (pSnd == nullptr || !gbSoundOn) {
		return;
	}

	tc = SDL_GetTicks();
	if (tc - pSnd->start_tc < 80) {
		return;
	}

	SoundSample *sound = &pSnd->DSB;
	if (sound->IsPlaying()) {
		sound = DuplicateSound(*sound);
		if (sound == nullptr)
			return;
	}

	sound->Play(lVolume, sgOptions.Audio.nSoundVolume, lPan);
	pSnd->start_tc = tc;
}

std::unique_ptr<TSnd> sound_file_load(const char *path, bool stream)
{

	auto snd = std::make_unique<TSnd>();
	snd->start_tc = SDL_GetTicks() - 80 - 1;

#ifndef STREAM_ALL_AUDIO
	if (stream) {
#endif
		if (snd->DSB.SetChunkStream(path) != 0) {
			ErrSdl();
		}
#ifndef STREAM_ALL_AUDIO
	} else {
		HANDLE file;
		if (!SFileOpenFile(path, &file)) {
			ErrDlg("SFileOpenFile failed", path, __FILE__, __LINE__);
		}
		DWORD dwBytes = SFileGetFileSize(file);
		auto waveFile = MakeArraySharedPtr<std::uint8_t>(dwBytes);
		SFileReadFileThreadSafe(file, waveFile.get(), dwBytes);
		int error = snd->DSB.SetChunk(waveFile, dwBytes);
		SFileCloseFileThreadSafe(file);
		if (error != 0) {
			ErrSdl();
		}
	}
#endif

	return snd;
}

#ifndef NOSOUND
TSnd::~TSnd()
{
	DSB.Stop();
	DSB.Release();
}
#endif

void snd_init()
{
	sgOptions.Audio.nSoundVolume = CapVolume(sgOptions.Audio.nSoundVolume);
	gbSoundOn = sgOptions.Audio.nSoundVolume > VOLUME_MIN;
	sgbSaveSoundOn = gbSoundOn;

	sgOptions.Audio.nMusicVolume = CapVolume(sgOptions.Audio.nMusicVolume);
	gbMusicOn = sgOptions.Audio.nMusicVolume > VOLUME_MIN;

	// Initialize the SDL_audiolib library. Set the output sample rate to
	// 22kHz, the audio format to 16-bit signed, use 2 output channels
	// (stereo), and a 2KiB output buffer.
	if (!Aulib::init(sgOptions.Audio.nSampleRate, AUDIO_S16, sgOptions.Audio.nChannels, sgOptions.Audio.nBufferSize)) {
		LogError(LogCategory::Audio, "Failed to initialize audio (Aulib::init): {}", SDL_GetError());
		return;
	}
	LogVerbose(LogCategory::Audio, "Aulib sampleRate={} channels={} frameSize={} format={:#x}",
	    Aulib::sampleRate(), Aulib::channelCount(), Aulib::frameSize(), Aulib::sampleFormat());

	duplicateSoundsMutex.emplace();
	gbSndInited = true;
}

void snd_deinit()
{
	if (gbSndInited) {
		Aulib::quit();
		duplicateSoundsMutex = std::nullopt;
	}

	gbSndInited = false;
}

void music_stop()
{
	if (music)
		CleanupMusic();
}

void music_start(uint8_t nTrack)
{
	bool success;
	const char *trackPath;

	assert((DWORD)nTrack < NUM_MUSIC);
	music_stop();
	if (gbMusicOn) {
		if (spawn_mpq != nullptr)
			trackPath = SpawnMusicTracks[nTrack];
		else
			trackPath = MusicTracks[nTrack];
		HANDLE handle;
		success = SFileOpenFile(trackPath, &handle);
		if (!success) {
			handle = nullptr;
		} else {
			LoadMusic(handle);
			if (!music->open()) {
				LogError(LogCategory::Audio, "Aulib::Stream::open (from music_start): {}", SDL_GetError());
				CleanupMusic();
				return;
			}

			music->setVolume(VolumeLogToLinear(sgOptions.Audio.nMusicVolume, VOLUME_MIN, VOLUME_MAX));
			if (!music->play(/*iterations=*/0)) {
				LogError(LogCategory::Audio, "Aulib::Stream::play (from music_start): {}", SDL_GetError());
				CleanupMusic();
				return;
			}

			sgnMusicTrack = (_music_id)nTrack;
		}
	}
}

void sound_disable_music(bool disable)
{
	if (disable) {
		music_stop();
	} else if (sgnMusicTrack != NUM_MUSIC) {
		music_start(sgnMusicTrack);
	}
}

int sound_get_or_set_music_volume(int volume)
{
	if (volume == 1)
		return sgOptions.Audio.nMusicVolume;

	sgOptions.Audio.nMusicVolume = volume;

	if (music)
		music->setVolume(VolumeLogToLinear(sgOptions.Audio.nMusicVolume, VOLUME_MIN, VOLUME_MAX));

	return sgOptions.Audio.nMusicVolume;
}

int sound_get_or_set_sound_volume(int volume)
{
	if (volume == 1)
		return sgOptions.Audio.nSoundVolume;

	sgOptions.Audio.nSoundVolume = volume;

	return sgOptions.Audio.nSoundVolume;
}

void music_mute()
{
	if (music)
		music->setVolume(VolumeLogToLinear(VOLUME_MIN, VOLUME_MIN, VOLUME_MAX));
}

void music_unmute()
{
	if (music)
		music->setVolume(VolumeLogToLinear(sgOptions.Audio.nMusicVolume, VOLUME_MIN, VOLUME_MAX));
}

} // namespace devilution
