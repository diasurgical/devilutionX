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
/** Specifies whether background music is enabled. */
HANDLE sghMusic;

namespace {

std::optional<Aulib::Stream> music;

#ifdef DISABLE_STREAMING_MUSIC
char *musicBuffer;

void FreeMusicBuffer()
{
	if (musicBuffer != nullptr) {
		mem_free_dbg(musicBuffer);
		musicBuffer = nullptr;
	}
}
#endif // DISABLE_STREAMING_MUSIC

} // namespace

/* data */

bool gbMusicOn = true;
/** Specifies whether sound effects are enabled. */
bool gbSoundOn = true;
/** Specifies the active background music track id. */
_music_id sgnMusicTrack = NUM_MUSIC;
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

bool snd_playing(TSnd *pSnd)
{
	if (pSnd == nullptr || pSnd->DSB == nullptr)
		return false;

	return pSnd->DSB->IsPlaying();
}

void snd_play_snd(TSnd *pSnd, int lVolume, int lPan)
{
	SoundSample *DSB;
	DWORD tc;

	if (pSnd == nullptr || !gbSoundOn) {
		return;
	}

	DSB = pSnd->DSB;
	if (DSB == nullptr) {
		return;
	}

	tc = SDL_GetTicks();
	if (tc - pSnd->start_tc < 80) {
		return;
	}

	lVolume = CapVolume(lVolume + sgOptions.Audio.nSoundVolume);
	DSB->Play(lVolume, lPan);
	pSnd->start_tc = tc;
}

TSnd *sound_file_load(const char *path, bool stream)
{
	HANDLE file;
	TSnd *pSnd;
	int error = 0;

	if (!SFileOpenFile(path, &file)) {
		ErrDlg("SFileOpenFile failed", path, __FILE__, __LINE__);
	}
	pSnd = (TSnd *)DiabloAllocPtr(sizeof(TSnd));
	memset(pSnd, 0, sizeof(TSnd));
	pSnd->sound_path = path;
	pSnd->start_tc = SDL_GetTicks() - 80 - 1;
	pSnd->DSB = new SoundSample();

	if (stream) {
		pSnd->file_handle = file;
		error = pSnd->DSB->SetChunkStream(file);
		if (error != 0) {
			SFileCloseFile(file);
			ErrSdl();
		}
	} else {
		DWORD dwBytes = SFileGetFileSize(file, nullptr);
		auto wave_file = std::make_unique<std::uint8_t[]>(dwBytes);
		SFileReadFile(file, wave_file.get(), dwBytes, nullptr, nullptr);
		error = pSnd->DSB->SetChunk(std::move(wave_file), dwBytes);
		SFileCloseFile(file);
	}
	if (error != 0) {
		ErrSdl();
	}

	return pSnd;
}

void sound_file_cleanup(TSnd *sound_file)
{
	if (sound_file != nullptr) {
		if (sound_file->DSB != nullptr) {
			sound_file->DSB->Stop();
			sound_file->DSB->Release();
			delete sound_file->DSB;
			sound_file->DSB = nullptr;
		}
		if (sound_file->file_handle != nullptr)
			SFileCloseFile(sound_file->file_handle);

		mem_free_dbg(sound_file);
	}
}

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
	if (music) {
		music = std::nullopt;
#ifndef DISABLE_STREAMING_MUSIC
		SFileCloseFile(sghMusic);
		sghMusic = nullptr;
#endif
		sgnMusicTrack = NUM_MUSIC;
#ifdef DISABLE_STREAMING_MUSIC
		FreeMusicBuffer();
#endif
	}
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
#ifndef DISABLE_STREAMING_MUSIC
			SDL_RWops *musicRw = SFileRw_FromStormHandle(sghMusic);
#else
			int bytestoread = SFileGetFileSize(sghMusic, 0);
			musicBuffer = (char *)DiabloAllocPtr(bytestoread);
			SFileReadFile(sghMusic, musicBuffer, bytestoread, NULL, 0);
			SFileCloseFile(sghMusic);
			sghMusic = NULL;

			SDL_RWops *musicRw = SDL_RWFromConstMem(musicBuffer, bytestoread);
			if (musicRw == nullptr)
				ErrSdl();
#endif
			music.emplace(musicRw, std::make_unique<Aulib::DecoderDrwav>(),
			    std::make_unique<Aulib::ResamplerSpeex>(sgOptions.Audio.nResamplingQuality), /*closeRw=*/true);
			if (!music->open()) {
				LogError(LogCategory::Audio, "Aulib::Stream::open (from music_start): {}", SDL_GetError());
				music = std::nullopt;
#ifndef DISABLE_STREAMING_MUSIC
				SFileCloseFile(sghMusic);
				sghMusic = nullptr;
#endif
				sgnMusicTrack = NUM_MUSIC;
#ifdef DISABLE_STREAMING_MUSIC
				FreeMusicBuffer();
#endif
				return;
			}

			music->setVolume(1.F - static_cast<float>(sgOptions.Audio.nMusicVolume) / VOLUME_MIN);
			if (!music->play()) {
				LogError(LogCategory::Audio, "Aulib::Stream::play (from music_start): {}", SDL_GetError());
				music = std::nullopt;
#ifndef DISABLE_STREAMING_MUSIC
				SFileCloseFile(sghMusic);
				sghMusic = nullptr;
#endif
				sgnMusicTrack = NUM_MUSIC;
#ifdef DISABLE_STREAMING_MUSIC
				FreeMusicBuffer();
#endif
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
