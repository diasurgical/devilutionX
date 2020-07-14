#include "devilution.h"
#include "miniwin/dsound.h"
#include "stubs.h"
#include <SDL.h>
#include <SDL_mixer.h>

namespace dvl {

BOOLEAN gbSndInited;
int sglMusicVolume;
int sglSoundVolume;
HANDLE sgpMusicTrack;

Mix_Music *music;
SDL_RWops *musicRw;
char *musicBuffer;

/* data */

BOOLEAN gbMusicOn = true;
BOOLEAN gbSoundOn = true;
int sgnMusicTrack = NUM_MUSIC;

char *sgszMusicTracks[NUM_MUSIC] = {
#ifdef SPAWN
	"Music\\sTowne.wav",
	"Music\\sLvlA.wav",
	"Music\\sintro.wav"
#else
	"Music\\DTowne.wav",
	"Music\\DLvlA.wav",
	"Music\\DLvlB.wav",
	"Music\\DLvlC.wav",
	"Music\\DLvlD.wav",
	"Music\\Dintro.wav"
#endif
};

BOOL snd_playing(TSnd *pSnd)
{
	DWORD dwStatus;

	if (!pSnd)
		return false;

	if (pSnd->DSB == NULL)
		return false;

	pSnd->DSB->GetStatus(&dwStatus);

	return dwStatus == DVL_DSBSTATUS_PLAYING;
}

void snd_play_snd(TSnd *pSnd, int lVolume, int lPan)
{
	LPDIRECTSOUNDBUFFER DSB;
	DWORD tc;

	if (!pSnd || !gbSoundOn) {
		return;
	}

	DSB = pSnd->DSB;
	if (!DSB) {
		return;
	}

	tc = GetTickCount();
	if (tc - pSnd->start_tc < 80) {
		return;
	}

	lVolume += sglSoundVolume;
	if (lVolume < VOLUME_MIN) {
		lVolume = VOLUME_MIN;
	} else if (lVolume > VOLUME_MAX) {
		lVolume = VOLUME_MAX;
	}
	DSB->Play(lVolume, lPan);
	pSnd->start_tc = tc;


}

TSnd *sound_file_load(char *path)
{
	HANDLE file;
	BYTE *wave_file;
	TSnd *pSnd;
	DWORD dwBytes;
	int error;

	WOpenFile(path, &file, false);
	pSnd = (TSnd *)DiabloAllocPtr(sizeof(TSnd));
	memset(pSnd, 0, sizeof(TSnd));
	pSnd->sound_path = path;
	pSnd->start_tc = GetTickCount() - 81;

	dwBytes = SFileGetFileSize(file, NULL);
	wave_file = DiabloAllocPtr(dwBytes);
	SFileReadFile(file, wave_file, dwBytes, NULL, NULL);

	pSnd->DSB = new DirectSoundBuffer();
	error = pSnd->DSB->SetChunk(wave_file, dwBytes);
	WCloseFile(file);
	mem_free_dbg(wave_file);
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
			delete static_cast<DirectSoundBuffer *>(sound_file->DSB);
			sound_file->DSB = NULL;
		}

		mem_free_dbg(sound_file);
	}
}

void snd_init(HWND hWnd)
{
	sound_load_volume("Sound Volume", &sglSoundVolume);
	gbSoundOn = sglSoundVolume > VOLUME_MIN;

	sound_load_volume("Music Volume", &sglMusicVolume);
	gbMusicOn = sglMusicVolume > VOLUME_MIN;

	int result = Mix_OpenAudio(22050, AUDIO_S16LSB, 2, 1024);
	if (result < 0) {
		SDL_Log(Mix_GetError());
	}
	Mix_AllocateChannels(25);
	Mix_ReserveChannels(1); // reserve one channel for naration (SFileDda*)

	gbSndInited = true;
}

void sound_load_volume(char *value_name, int *value)
{
	int v = *value;
	if (!SRegLoadValue("Diablo", value_name, 0, &v)) {
		v = VOLUME_MAX;
	}
	*value = v;

	if (*value < VOLUME_MIN) {
		*value = VOLUME_MIN;
	} else if (*value > VOLUME_MAX) {
		*value = VOLUME_MAX;
	}
	*value -= *value % 100;
}

void sound_cleanup()
{
	SFileDdaDestroy();

	if (gbSndInited) {
		gbSndInited = false;
		sound_store_volume("Sound Volume", sglSoundVolume);
		sound_store_volume("Music Volume", sglMusicVolume);
	}
}

void sound_store_volume(char *key, int value)
{
	SRegSaveValue("Diablo", key, 0, value);
}

void music_stop()
{
	if (sgpMusicTrack) {
		Mix_HaltMusic();
		SFileCloseFile(sgpMusicTrack);
		sgpMusicTrack = NULL;
		Mix_FreeMusic(music);
		music = NULL;
		musicRw = NULL;
		mem_free_dbg(musicBuffer);
		sgnMusicTrack = NUM_MUSIC;
	}
}

void music_start(int nTrack)
{
	BOOL success;

	assert((DWORD) nTrack < NUM_MUSIC);
	music_stop();
	if (gbMusicOn) {
		success = SFileOpenFile(sgszMusicTracks[nTrack], &sgpMusicTrack);
		if (!success) {
			sgpMusicTrack = NULL;
		} else {
			int bytestoread = SFileGetFileSize(sgpMusicTrack, 0);
			musicBuffer = (char *)DiabloAllocPtr(bytestoread);
			SFileReadFile(sgpMusicTrack, musicBuffer, bytestoread, NULL, 0);

			musicRw = SDL_RWFromConstMem(musicBuffer, bytestoread);
			if (musicRw == NULL) {
				ErrSdl();
			}
			music = Mix_LoadMUSType_RW(musicRw, MUS_NONE, 1);
			Mix_VolumeMusic(MIX_MAX_VOLUME - MIX_MAX_VOLUME * sglMusicVolume / VOLUME_MIN);
			Mix_PlayMusic(music, -1);

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
		return sglMusicVolume;

	sglMusicVolume = volume;

	if (sgpMusicTrack)
		SFileDdaSetVolume(sgpMusicTrack, volume, 0);

	return sglMusicVolume;
}

int sound_get_or_set_sound_volume(int volume)
{
	if (volume == 1)
		return sglSoundVolume;

	sglSoundVolume = volume;

	return sglSoundVolume;
}

} // namespace dvl
