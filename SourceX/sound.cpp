/**
 * @file sound.cpp
 *
 * Implementation of functions setting up the audio pipeline.
 */
#include "all.h"
#include "options.h"
#include "../3rdParty/Storm/Source/storm.h"
#include "stubs.h"
#include "storm_sdl_rw.h"
#include <SDL.h>
#include <SDL_mixer.h>

namespace dvl {

BOOLEAN gbSndInited;
/** Specifies whether background music is enabled. */
HANDLE sghMusic;

namespace {

Mix_Music *music;

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

BOOLEAN gbMusicOn = true;
/** Specifies whether sound effects are enabled. */
BOOLEAN gbSoundOn = true;
/** Specifies the active background music track id. */
int sgnMusicTrack = NUM_MUSIC;
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

BOOL snd_playing(TSnd *pSnd)
{
	if (pSnd == NULL || pSnd->DSB == NULL)
		return false;

	return pSnd->DSB->IsPlaying();
}

void snd_play_snd(TSnd *pSnd, int lVolume, int lPan)
{
	SoundSample *DSB;
	DWORD tc;

	if (!pSnd || !gbSoundOn) {
		return;
	}

	DSB = pSnd->DSB;
	if (DSB == NULL) {
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
		DWORD dwBytes = SFileGetFileSize(file, NULL);
		BYTE *wave_file = DiabloAllocPtr(dwBytes);
		SFileReadFile(file, wave_file, dwBytes, NULL, NULL);
		error = pSnd->DSB->SetChunk(wave_file, dwBytes);
		SFileCloseFile(file);
		mem_free_dbg(wave_file);
	}
	if (error != 0) {
		ErrSdl();
	}

	return pSnd;
}

void sound_file_cleanup(TSnd *sound_file)
{
	if (sound_file) {
		if (sound_file->DSB) {
			sound_file->DSB->Stop();
			sound_file->DSB->Release();
			delete sound_file->DSB;
			sound_file->DSB = NULL;
		}
		if (sound_file->file_handle != NULL)
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

	int result = Mix_OpenAudio(22050, AUDIO_S16LSB, 2, 1024);
	if (result < 0) {
		SDL_Log(Mix_GetError());
	}
	Mix_AllocateChannels(25);
	Mix_ReserveChannels(1); // reserve one channel for naration (SFileDda*)

	gbSndInited = true;
}

void music_stop()
{
	if (music != nullptr) {
		Mix_HaltMusic();
		Mix_FreeMusic(music);
		music = NULL;
#ifndef DISABLE_STREAMING_MUSIC
		SFileCloseFile(sghMusic);
		sghMusic = NULL;
#endif
		sgnMusicTrack = NUM_MUSIC;
#ifdef DISABLE_STREAMING_MUSIC
		FreeMusicBuffer();
#endif
	}
}

void music_start(int nTrack)
{
	BOOL success;
	const char *trackPath;

	assert((DWORD)nTrack < NUM_MUSIC);
	music_stop();
	if (gbMusicOn) {
		if (spawn_mpq)
			trackPath = sgszSpawnMusicTracks[nTrack];
		else
			trackPath = sgszMusicTracks[nTrack];
		success = SFileOpenFile(trackPath, &sghMusic);
		if (!success) {
			sghMusic = NULL;
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
			music = Mix_LoadMUSType_RW(musicRw, MUS_NONE, /*freesrc=*/1);
			if (music == NULL) {
				SDL_Log("Mix_LoadMUSType_RW: %s", Mix_GetError());
#ifndef DISABLE_STREAMING_MUSIC
				SFileCloseFile(sghMusic);
				sghMusic = NULL;
#endif
				sgnMusicTrack = NUM_MUSIC;
#ifdef DISABLE_STREAMING_MUSIC
				FreeMusicBuffer();
#endif
				return;
			}

			Mix_VolumeMusic(MIX_MAX_VOLUME - MIX_MAX_VOLUME * sgOptions.Audio.nMusicVolume / VOLUME_MIN);
			if (Mix_PlayMusic(music, -1) < 0) {
				SDL_Log("Mix_PlayMusic: %s", Mix_GetError());
				Mix_FreeMusic(music);
				music = NULL;
#ifndef DISABLE_STREAMING_MUSIC
				SFileCloseFile(sghMusic);
				sghMusic = NULL;
#endif
				sgnMusicTrack = NUM_MUSIC;
#ifdef DISABLE_STREAMING_MUSIC
				FreeMusicBuffer();
#endif
				return;
			}

			sgnMusicTrack = nTrack;
		}
	}
}

void sound_disable_music(BOOL disable)
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

	if (music != nullptr)
		Mix_VolumeMusic(MIX_MAX_VOLUME - MIX_MAX_VOLUME * volume / VOLUME_MIN);

	return sgOptions.Audio.nMusicVolume;
}

int sound_get_or_set_sound_volume(int volume)
{
	if (volume == 1)
		return sgOptions.Audio.nSoundVolume;

	sgOptions.Audio.nSoundVolume = volume;

	return sgOptions.Audio.nSoundVolume;
}

} // namespace dvl
