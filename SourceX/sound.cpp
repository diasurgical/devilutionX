#include "devilution.h"
#include "stubs.h"
#include <SDL.h>
#include <SDL_mixer.h>

namespace dvl {

LPDIRECTSOUNDBUFFER DSBs[8];
LPDIRECTSOUND sglpDS;
BOOLEAN gbSndInited;
int sglMusicVolume;
int sglSoundVolume;
HMODULE hDsound_dll;
HANDLE sgpMusicTrack;
LPDIRECTSOUNDBUFFER sglpDSB;

Mix_Music *music;
SDL_RWops *musicRw;
char *musicBuffer;

/* data */

BYTE gbMusicOn = true;
BYTE gbSoundOn = true;
BYTE gbDupSounds = true;
int sgnMusicTrack = 6;
char *sgszMusicTracks[NUM_MUSIC] = {
	"Music\\DTowne.wav",
	"Music\\DLvlA.wav",
	"Music\\DLvlB.wav",
	"Music\\DLvlC.wav",
	"Music\\DLvlD.wav",
	"Music\\Dintro.wav"
};
char unk_volume[4][2] = {
	{ 15, -16 },
	{ 15, -16 },
	{ 30, -31 },
	{ 30, -31 }
};

void snd_update(BOOL bStopAll)
{
	DWORD dwStatus, i;

	for (i = 0; i < 8; i++) {
		if (!DSBs[i])
			continue;

#ifdef __cplusplus
		if (!bStopAll && DSBs[i]->GetStatus(&dwStatus) == DVL_DS_OK && dwStatus == DVL_DSBSTATUS_PLAYING)
			continue;

		DSBs[i]->Stop();
		DSBs[i]->Release();
#else
		if (!bStopAll && DSBs[i]->lpVtbl->GetStatus(DSBs[i], &dwStatus) == DVL_DS_OK && dwStatus == DSBSTATUS_PLAYING)
			continue;

		DSBs[i]->lpVtbl->Stop(DSBs[i]);
		DSBs[i]->lpVtbl->Release(DSBs[i]);
#endif

		DSBs[i] = NULL;
	}
}

void snd_stop_snd(TSnd *pSnd)
{
	if (pSnd && pSnd->DSB)
#ifdef __cplusplus
		pSnd->DSB->Stop();
#else
		pSnd->DSB->lpVtbl->Stop(pSnd->DSB);
#endif
}

BOOL snd_playing(TSnd *pSnd)
{
	DWORD dwStatus;

	if (!pSnd)
		return false;

	if (pSnd->DSB == NULL)
		return false;

#ifdef __cplusplus
	if (pSnd->DSB->GetStatus(&dwStatus) != DVL_DS_OK)
#else
	if (pSnd->DSB->lpVtbl->GetStatus(pSnd->DSB, &dwStatus) != DVL_DS_OK)
#endif
		return false;

	return dwStatus == DVL_DSBSTATUS_PLAYING;
}

void snd_play_snd(TSnd *pSnd, int lVolume, int lPan)
{
	LPDIRECTSOUNDBUFFER DSB;
	DWORD tc;
	HRESULT error_code;

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

	if (snd_playing(pSnd)) {
		DSB = sound_dup_channel(pSnd->DSB);
		if (DSB == NULL) {
			return;
		}
	}

	lVolume += sglSoundVolume;
	if (lVolume < VOLUME_MIN) {
		lVolume = VOLUME_MIN;
	} else if (lVolume > VOLUME_MAX) {
		lVolume = VOLUME_MAX;
	}
#ifdef __cplusplus
	DSB->SetVolume(lVolume);
	DSB->SetPan(lPan);

	error_code = DSB->Play(0, 0, 0);
#else
	DSB->lpVtbl->SetVolume(DSB, lVolume);
	DSB->lpVtbl->SetPan(DSB, lPan);

	error_code = DSB->lpVtbl->Play(DSB, 0, 0, 0);
#endif

	if (error_code != DVL_DSERR_BUFFERLOST) {
		if (error_code != DVL_DS_OK) {
			DSErrMsg(error_code, 261, "C:\\Src\\Diablo\\Source\\SOUND.CPP");
		}
	} else if (sound_file_reload(pSnd, DSB)) {
#ifdef __cplusplus
		DSB->Play(0, 0, 0);
#else
		DSB->lpVtbl->Play(DSB, 0, 0, 0);
#endif
	}

	pSnd->start_tc = tc;
}

LPDIRECTSOUNDBUFFER sound_dup_channel(LPDIRECTSOUNDBUFFER DSB)
{
	DWORD i;

	if (!gbDupSounds) {
		return NULL;
	}

	for (i = 0; i < 8; i++) {
		if (!DSBs[i]) {
#ifdef __cplusplus
			if (sglpDS->DuplicateSoundBuffer(DSB, &DSBs[i]) != DVL_DS_OK) {
#else
			if (sglpDS->lpVtbl->DuplicateSoundBuffer(sglpDS, DSB, &DSBs[i]) != DVL_DS_OK) {
#endif
				return NULL;
			}

			return DSBs[i];
		}
	}

	return NULL;
}

BOOL sound_file_reload(TSnd *sound_file, LPDIRECTSOUNDBUFFER DSB)
{
	HANDLE file;
	LPVOID buf1, buf2;
	DWORD size1, size2;
	BOOL rv;

#ifdef __cplusplus
	if (DSB->Restore() != DVL_DS_OK)
#else
	if (DSB->lpVtbl->Restore(DSB) != DVL_DS_OK)
#endif
		return false;

	rv = false;

	WOpenFile(sound_file->sound_path, &file, false);
	WSetFilePointer(file, sound_file->chunk.dwOffset, NULL, 0);

#ifdef __cplusplus
	if (DSB->Lock(0, sound_file->chunk.dwSize, &buf1, &size1, &buf2, &size2, 0) == DVL_DS_OK) {
		WReadFile(file, buf1, size1);
		if (DSB->Unlock(buf1, size1, buf2, size2) == DVL_DS_OK)
			rv = true;
	}
#else
	if (DSB->lpVtbl->Lock(DSB, 0, sound_file->chunk.dwSize, &buf1, &size1, &buf2, &size2, 0) == DVL_DS_OK) {
		WReadFile(file, buf1, size1);
		if (DSB->lpVtbl->Unlock(DSB, buf1, size1, buf2, size2) == DVL_DS_OK)
			rv = true;
	}
#endif

	WCloseFile(file);

	return rv;
}

TSnd *sound_file_load(char *path)
{
	HANDLE file;
	BYTE *wave_file;
	TSnd *pSnd;
	LPVOID buf1, buf2;
	DWORD size1, size2;
	HRESULT error_code;

	if (!sglpDS)
		return NULL;

	WOpenFile(path, &file, false);
	pSnd = (TSnd *)DiabloAllocPtr(sizeof(TSnd));
	memset(pSnd, 0, sizeof(TSnd));
	pSnd->sound_path = path;
	pSnd->start_tc = GetTickCount() - 81;

	wave_file = LoadWaveFile(file, &pSnd->fmt, &pSnd->chunk);
	if (!wave_file)
		app_fatal("Invalid sound format on file %s", pSnd->sound_path);

	sound_CreateSoundBuffer(pSnd);

#ifdef __cplusplus
	error_code = pSnd->DSB->Lock(0, pSnd->chunk.dwSize, &buf1, &size1, &buf2, &size2, 0);
#else
	error_code = pSnd->DSB->lpVtbl->Lock(pSnd->DSB, 0, pSnd->chunk.dwSize, &buf1, &size1, &buf2, &size2, 0);
#endif
	if (error_code != DVL_DS_OK)
		DSErrMsg(error_code, 318, "C:\\Src\\Diablo\\Source\\SOUND.CPP");

	memcpy(buf1, wave_file + pSnd->chunk.dwOffset, size1);

#ifdef __cplusplus
	error_code = pSnd->DSB->Unlock(buf1, size1, buf2, size2);
#else
	error_code = pSnd->DSB->lpVtbl->Unlock(pSnd->DSB, buf1, size1, buf2, size2);
#endif
	if (error_code != DVL_DS_OK)
		DSErrMsg(error_code, 325, "C:\\Src\\Diablo\\Source\\SOUND.CPP");

	mem_free_dbg((void *)wave_file);
	WCloseFile(file);

	return pSnd;
}
// 456F07: could not find valid save-restore pair for esi

void sound_CreateSoundBuffer(TSnd *sound_file)
{
	DUMMY_ONCE();
	DSBUFFERDESC DSB;
	HRESULT error_code;
	memset(&DSB, 0, sizeof(DSBUFFERDESC));

	DSB.dwBufferBytes = sound_file->chunk.dwSize;
	DSB.lpwfxFormat = &sound_file->fmt;
	DSB.dwSize = sizeof(DSBUFFERDESC);
	DSB.dwFlags = DVL_DSBCAPS_CTRLVOLUME | DVL_DSBCAPS_CTRLPAN | DVL_DSBCAPS_STATIC;

	sound_file->chunk.dwSize += sound_file->chunk.dwOffset;
	sound_file->chunk.dwOffset = 0;

#ifdef __cplusplus
	error_code = sglpDS->CreateSoundBuffer(&DSB, &sound_file->DSB, NULL);
#else
	error_code = sglpDS->lpVtbl->CreateSoundBuffer(sglpDS, &DSB, &sound_file->DSB, NULL);
#endif
	if (error_code != DVL_ERROR_SUCCESS)
		DSErrMsg(error_code, 282, "C:\\Src\\Diablo\\Source\\SOUND.CPP");
}

void sound_file_cleanup(TSnd *sound_file)
{
	if (sound_file) {
		if (sound_file->DSB) {
#ifdef __cplusplus
			sound_file->DSB->Stop();
			sound_file->DSB->Release();
#else
			sound_file->DSB->lpVtbl->Stop(sound_file->DSB);
			sound_file->DSB->lpVtbl->Release(sound_file->DSB);
#endif
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

	if (sound_DirectSoundCreate(NULL, &sglpDS, NULL) != DVL_DS_OK)
		sglpDS = NULL;

#ifdef __cplusplus
	if (sglpDS && sglpDS->SetCooperativeLevel(hWnd, DVL_DSSCL_EXCLUSIVE) == DVL_DS_OK)
#else
	if (sglpDS && sglpDS->lpVtbl->SetCooperativeLevel(sglpDS, hWnd, DVL_DSSCL_EXCLUSIVE) == DVL_DS_OK)
#endif
		sound_create_primary_buffer(NULL);

	SVidInitialize(sglpDS);
	SFileDdaInitialize(sglpDS);

	gbSndInited = sglpDS != NULL;
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

void sound_create_primary_buffer(HANDLE music_track)
{
	HRESULT error_code;
	DSBUFFERDESC dsbuf;
	WAVEFORMATEX format;

	if (!music_track) {
		memset(&dsbuf, 0, sizeof(DSBUFFERDESC));
		dsbuf.dwSize = sizeof(DSBUFFERDESC);
		dsbuf.dwFlags = DVL_DSBCAPS_PRIMARYBUFFER;

#ifdef __cplusplus
		error_code = sglpDS->CreateSoundBuffer(&dsbuf, &sglpDSB, NULL);
#else
		error_code = sglpDS->lpVtbl->CreateSoundBuffer(sglpDS, &dsbuf, &sglpDSB, NULL);
#endif
		if (error_code != DVL_DS_OK)
			DSErrMsg(error_code, 375, "C:\\Src\\Diablo\\Source\\SOUND.CPP");
	}

	if (sglpDSB) {
		DSCAPS dsbcaps;
		dsbcaps.dwSize = sizeof(DSCAPS);

#ifdef __cplusplus
		error_code = sglpDS->GetCaps(&dsbcaps);
#else
		error_code = sglpDS->lpVtbl->GetCaps(sglpDS, &dsbcaps);
#endif
		if (error_code != DVL_DS_OK)
			DSErrMsg(error_code, 383, "C:\\Src\\Diablo\\Source\\SOUND.CPP");

		if (!music_track || !LoadWaveFormat(music_track, &format)) {
			memset(&format, 0, sizeof(WAVEFORMATEX));
			format.wFormatTag = DVL_WAVE_FORMAT_PCM;
			format.nSamplesPerSec = 22050;
			format.wBitsPerSample = 16;
			format.cbSize = 0;
		}

		format.nChannels = 2;
		format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
		format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

#ifdef __cplusplus
		sglpDSB->SetFormat(&format);
#else
		sglpDSB->lpVtbl->SetFormat(sglpDSB, &format);
#endif
	}
}
// 69F100: using guessed type int sglpDSB;

HRESULT sound_DirectSoundCreate(LPGUID lpGuid, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
{
	DUMMY();
	HRESULT(*DirectSoundCreate)
	(LPGUID lpGuid, LPDIRECTSOUND * ppDS, LPUNKNOWN pUnkOuter);

	if (hDsound_dll == NULL) {
		if (hDsound_dll == NULL) {
		}
	}

	DirectSoundCreate = NULL;
	if (DirectSoundCreate == NULL) {
	}
	*ppDS = new DirectSound();
	int result = Mix_OpenAudio(22050, AUDIO_S16LSB, 2, 1024);
	if (result < 0) {
		SDL_Log(Mix_GetError());
	}
	Mix_AllocateChannels(25);
	Mix_ReserveChannels(1); // reserve one channel for naration (SFileDda*)
	return result;
}

void sound_cleanup()
{
	snd_update(true);
	SVidDestroy();
	SFileDdaDestroy();

	if (sglpDS) {
#ifdef __cplusplus
		sglpDS->Release();
		delete sglpDS;
#else
		sglpDS->lpVtbl->Release(sglpDS);
#endif
		sglpDS = NULL;
	}

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
		sgnMusicTrack = 6;
	}
}

void music_start(int nTrack)
{
	BOOL success;

	/// ASSERT: assert((DWORD) nTrack < NUM_MUSIC);
	music_stop();
	if (sglpDS && gbMusicOn) {
#ifdef _DEBUG
		SFileEnableDirectAccess(false);
#endif
		success = SFileOpenFile(sgszMusicTracks[nTrack], &sgpMusicTrack);
#ifdef _DEBUG
		SFileEnableDirectAccess(true);
#endif
		sound_create_primary_buffer(sgpMusicTrack);
		if (!success) {
			sgpMusicTrack = NULL;
		} else {
			int bytestoread = SFileGetFileSize(sgpMusicTrack, 0);
			musicBuffer = (char *)DiabloAllocPtr(bytestoread);
			SFileReadFile(sgpMusicTrack, musicBuffer, bytestoread, NULL, 0);

			musicRw = SDL_RWFromConstMem(musicBuffer, bytestoread);
			if (musicRw == NULL) {
				SDL_Log(SDL_GetError());
			}
			music = Mix_LoadMUS_RW(musicRw, 1);
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
	} else if (sgnMusicTrack != 6) {
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
