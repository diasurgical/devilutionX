/**
 * @file sound.cpp
 *
 * Implementation of functions setting up the audio pipeline.
 */
#include <cstdint>
#include <memory>

#include <aulib.h>
#include <Aulib/DecoderDrwav.h>
#include <Aulib/Stream.h>
#include <Aulib/ResamplerSpeex.h>
#include <SDL.h>

#include "init.h"
#include "options.h"
#include "storm/storm_sdl_rw.h"
#include "storm/storm.h"
#include "utils/log.hpp"
#include "utils/stdcompat/optional.hpp"
#include "utils/stubs.h"

namespace devilution {

bool gbSndInited;
/** Handle to the music Storm file. */
HANDLE sghMusic;
/** The active background music track id. */
_music_id sgnMusicTrack = NUM_MUSIC;

namespace {

std::optional<Aulib::Stream> music;

#ifdef DISABLE_STREAMING_MUSIC
char *musicBuffer;
#endif

void LoadMusic()
{
#ifndef DISABLE_STREAMING_MUSIC
	SDL_RWops *musicRw = SFileRw_FromStormHandle(sghMusic);
#else
	int bytestoread = SFileGetFileSize(sghMusic, 0);
	musicBuffer = (char *)DiabloAllocPtr(bytestoread);
	SFileReadFile(sghMusic, musicBuffer, bytestoread, NULL, 0);
	SFileCloseFile(sghMusic);
	sghMusic = NULL;

	SDL_RWops *musicRw = SDL_RWFromConstMem(musicBuffer, bytestoread);
#endif
	music.emplace(musicRw, std::make_unique<Aulib::DecoderDrwav>(),
			std::make_unique<Aulib::ResamplerSpeex>(sgOptions.Audio.nResamplingQuality), /*closeRw=*/true);
}

void CleanupMusic()
{
		music = std::nullopt;
		sgnMusicTrack = NUM_MUSIC;
#ifndef DISABLE_STREAMING_MUSIC
		SFileCloseFile(sghMusic);
		sghMusic = nullptr;
#else
	if (musicBuffer != nullptr) {
		mem_free_dbg(musicBuffer);
		musicBuffer = nullptr;
	}
#endif
}

} // namespace

/* data */

bool gbMusicOn = true;
/** Specifies whether sound effects are enabled. */
bool gbSoundOn = true;

/** Maps from track ID to track name in spawn. */
const char *const sgszSpawnMusicTracks[NUM_MUSIC] = {
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
const char *const sgszMusicTracks[NUM_MUSIC] = {
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
	if (volume < VOLUME_MIN) {
		volume = VOLUME_MIN;
	} else if (volume > VOLUME_MAX) {
		volume = VOLUME_MAX;
	}
	return volume - volume % 100;
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

	lVolume = CapVolume(lVolume + sgOptions.Audio.nSoundVolume);
	pSnd->DSB.Play(lVolume, lPan);
	pSnd->start_tc = tc;
}

std::unique_ptr<TSnd> sound_file_load(const char *path, bool stream)
{
	HANDLE file;
	int error = 0;

	if (!SFileOpenFile(path, &file)) {
		ErrDlg("SFileOpenFile failed", path, __FILE__, __LINE__);
	}
	auto snd = std::make_unique<TSnd>();
	snd->sound_path = std::string(path);
	snd->start_tc = SDL_GetTicks() - 80 - 1;

	if (stream) {
		snd->file_handle = file;
		error = snd->DSB.SetChunkStream(file);
		if (error != 0) {
			SFileCloseFile(file);
			ErrSdl();
		}
	} else {
		DWORD dwBytes = SFileGetFileSize(file, nullptr);
		auto wave_file = std::make_unique<std::uint8_t[]>(dwBytes);
		SFileReadFile(file, wave_file.get(), dwBytes, nullptr, nullptr);
		error = snd->DSB.SetChunk(std::move(wave_file), dwBytes);
		SFileCloseFile(file);
	}
	if (error != 0) {
		ErrSdl();
	}

	return snd;
}

#ifndef NOSOUND
TSnd::~TSnd()
{
	DSB.Stop();
	DSB.Release();
	if (file_handle != nullptr)
		SFileCloseFile(file_handle);
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

	gbSndInited = true;
}

void snd_deinit() {
	if (gbSndInited) {
		Aulib::quit();
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
			trackPath = sgszSpawnMusicTracks[nTrack];
		else
			trackPath = sgszMusicTracks[nTrack];
		success = SFileOpenFile(trackPath, &sghMusic);
		if (!success) {
			sghMusic = nullptr;
		} else {
			LoadMusic();
			if (!music->open()) {
				LogError(LogCategory::Audio, "Aulib::Stream::open (from music_start): {}", SDL_GetError());
				CleanupMusic();
				return;
			}

			music->setVolume(1.F - static_cast<float>(sgOptions.Audio.nMusicVolume) / VOLUME_MIN);
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
		music->setVolume(1.F - static_cast<float>(sgOptions.Audio.nMusicVolume) / VOLUME_MIN);

	return sgOptions.Audio.nMusicVolume;
}

int sound_get_or_set_sound_volume(int volume)
{
	if (volume == 1)
		return sgOptions.Audio.nSoundVolume;

	sgOptions.Audio.nSoundVolume = volume;

	return sgOptions.Audio.nSoundVolume;
}

} // namespace devilution
