//HEADER_GOES_HERE

#include "../types.h"
#include "pch.h"

LPDIRECTSOUNDBUFFER DSBs[8];
LPDIRECTSOUND sglpDS;
char gbSndInited;
int sglMusicVolume;
int sglSoundVolume;
HMODULE hDsound_dll;
HANDLE sgpMusicTrack;
LPDIRECTSOUNDBUFFER sglpDSB;

/* data */

BYTE gbMusicOn = TRUE;
BYTE gbSoundOn = TRUE;
BYTE gbDupSounds = TRUE;
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

void __fastcall snd_update(BOOL bStopAll)
{
	DWORD error_code, i;

	if (!bStopAll)
		return;

	Mix_HaltChannel(-1);
}

void __fastcall snd_stop_snd(TSnd *pSnd)
{
	DUMMY_ONCE();
	if (pSnd && pSnd->DSB)
		Mix_HaltChannel(-1);
}

BOOL __fastcall snd_playing(TSnd *pSnd)
{
	DWORD error_code; // TODO should probably be HRESULT

	if (!pSnd)
		return FALSE;

	if (pSnd->DSB == NULL)
		return FALSE;

	DUMMY_ONCE();

	return FALSE;
}

void __fastcall snd_play_snd(TSnd *pSnd, int lVolume, int lPan)
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
		pSnd->start_tc = GetTickCount();
		return;
	}

	lVolume += sglSoundVolume;
	if (lVolume < VOLUME_MIN) {
		lVolume = VOLUME_MIN;
	} else if (lVolume > VOLUME_MAX) {
		lVolume = VOLUME_MAX;
	}

	Mix_VolumeChunk((Mix_Chunk *)(pSnd->DSB), MIX_MAX_VOLUME - MIX_MAX_VOLUME * lVolume / VOLUME_MIN);
	int channel = Mix_PlayChannel(-1, (Mix_Chunk *)(pSnd->DSB), 0);
	if (channel != -1) {
		int panned = 255 * abs(lPan) / 10000;
		Mix_SetPanning(channel, lPan <= 0 ? 255 : panned, lPan >= 0 ? 255 : panned);
	}

	if (channel == -1) {
		SDL_Log("Mix_PlayChannel: %s\n", SDL_GetError());
	} else if (sound_file_reload(pSnd, DSB)) {
		UNIMPLEMENTED();
	}

	pSnd->start_tc = tc;
}

LPDIRECTSOUNDBUFFER __fastcall sound_dup_channel(LPDIRECTSOUNDBUFFER DSB)
{
	DWORD i;

	if (!gbDupSounds) {
		return NULL;
	}

	for (i = 0; i < 8; i++) {
		UNIMPLEMENTED();
	}

	return NULL;
}

BOOL __fastcall sound_file_reload(TSnd *sound_file, LPDIRECTSOUNDBUFFER DSB)
{
	HANDLE file;
	LPVOID buf1, buf2;
	DWORD size1, size2;
	BOOL rv;

	DUMMY_ONCE();

	rv = FALSE;

	WOpenFile(sound_file->sound_path, &file, FALSE);
	WSetFilePointer(file, sound_file->chunk.dwOffset, NULL, 0);

	WCloseFile(file);

	return rv;
}

TSnd *__fastcall sound_file_load(char *path)
{
	HANDLE file;
	BYTE *wave_file;
	TSnd *pSnd;
	LPVOID buf1, buf2;
	DWORD size1, size2;
	HRESULT error_code;

	if (!gbSndInited)
		return NULL;

	WOpenFile(path, &file, FALSE);
	pSnd = (TSnd *)DiabloAllocPtr(sizeof(TSnd));
	memset(pSnd, 0, sizeof(TSnd));
	pSnd->sound_path = path;
	pSnd->start_tc = GetTickCount() - 81;

	//This opens the file and reads it, makes Mix_chunk pointer to it.
	//Once this is done the pointer is stored TSnd Struct

	size1 = (int)SFileGetFileSize((HANDLE)file, 0);
	wave_file = DiabloAllocPtr(size1);
	SFileReadFile(file, (void *)wave_file, size1, (LPDWORD)&size2, 0);
	SDL_RWops *rw = SDL_RWFromConstMem((void *)wave_file, size1);
	Mix_Chunk *SoundFX = Mix_LoadWAV_RW(rw, 1);
	pSnd->DSB = (LPDIRECTSOUNDBUFFER)SoundFX;
	if (!pSnd->DSB)
		TermMsg("Invalid sound format on file %s", pSnd->sound_path);

	sound_CreateSoundBuffer(pSnd);

	mem_free_dbg((void *)wave_file);
	WCloseFile(file);

	return pSnd;
}
// 456F07: could not find valid save-restore pair for esi

void __fastcall sound_CreateSoundBuffer(TSnd *sound_file)
{
	DUMMY_ONCE();
	DSBUFFERDESC DSB;
	HRESULT error_code;
	memset(&DSB, 0, sizeof(DSBUFFERDESC));

	DSB.dwBufferBytes = sound_file->chunk.dwSize;
	DSB.lpwfxFormat = &sound_file->fmt;
	DSB.dwSize = sizeof(DSBUFFERDESC);
	DSB.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_STATIC;

	if (error_code != ERROR_SUCCESS)
		DSErrMsg(error_code, 282, "C:\\Src\\Diablo\\Source\\SOUND.CPP");
}

void __fastcall sound_file_cleanup(TSnd *sound_file)
{
	if (sound_file) {
		if (sound_file->DSB) {
			DUMMY_ONCE();
			Mix_FreeChunk((Mix_Chunk *)sound_file->DSB);
			sound_file->DSB = NULL;
		}

		mem_free_dbg(sound_file);
	}
}

void __fastcall snd_init(HWND hWnd)
{
	DUMMY();
	sound_load_volume("Sound Volume", &sglSoundVolume);
	gbSoundOn = sglSoundVolume > VOLUME_MIN;

	sound_load_volume("Music Volume", &sglMusicVolume);
	gbMusicOn = sglMusicVolume > VOLUME_MIN;

	if (Mix_OpenAudio(44100, AUDIO_S16LSB, 2, 1024) != 0) {
		sglpDS = NULL;

		sound_create_primary_buffer(NULL);
	}

	SVidInitialize(sglpDS);
	SFileDdaInitialize(sglpDS);

	gbSndInited = true;
}

void __fastcall sound_load_volume(char *value_name, int *value)
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

void __fastcall sound_create_primary_buffer(HANDLE music_track)
{
	DUMMY();
	HRESULT error_code;
	DSBUFFERDESC dsbuf;
	WAVEFORMATEX format;

	if (!music_track) {
		memset(&dsbuf, 0, sizeof(DSBUFFERDESC));
		dsbuf.dwSize = sizeof(DSBUFFERDESC);
		dsbuf.dwFlags = DSBCAPS_PRIMARYBUFFER;

#ifdef __cplusplus
		error_code = sglpDS->CreateSoundBuffer(&dsbuf, &sglpDSB, NULL);
#else
		error_code = sglpDS->lpVtbl->CreateSoundBuffer(sglpDS, &dsbuf, &sglpDSB, NULL);
#endif
		if (error_code != DS_OK)
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
		if (error_code != DS_OK)
			DSErrMsg(error_code, 383, "C:\\Src\\Diablo\\Source\\SOUND.CPP");

		if (!music_track || !LoadWaveFormat(music_track, &format)) {
			memset(&format, 0, sizeof(WAVEFORMATEX));
			format.wFormatTag = WAVE_FORMAT_PCM;
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

HRESULT __fastcall sound_DirectSoundCreate(LPGUID lpGuid, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
{
	UNIMPLEMENTED();
	HRESULT(WINAPI * DirectSoundCreate)
	(LPGUID lpGuid, LPDIRECTSOUND * ppDS, LPUNKNOWN pUnkOuter);

	if (hDsound_dll == NULL) {
		if (hDsound_dll == NULL) {
		}
	}

	DirectSoundCreate = NULL;
	if (DirectSoundCreate == NULL) {
		ErrDlg(IDD_DIALOG5, GetLastError(), "C:\\Src\\Diablo\\Source\\SOUND.CPP", 427);
	}
	return DirectSoundCreate(lpGuid, ppDS, pUnkOuter);
}

void __cdecl sound_cleanup()
{
	snd_update(TRUE);
	SVidDestroy();
	SFileDdaDestroy();

	if (gbSndInited) {
		DUMMY();
		sglpDS = NULL;
	}

	if (gbSndInited) {
		gbSndInited = FALSE;
		sound_store_volume("Sound Volume", sglSoundVolume);
		sound_store_volume("Music Volume", sglMusicVolume);
	}
}

void __fastcall sound_store_volume(char *key, int value)
{
	SRegSaveValue("Diablo", key, 0, value);
}

void __cdecl music_stop()
{
	if (sgpMusicTrack) {
		Mix_HaltMusic();
		SFileCloseFile(sgpMusicTrack);
		sgpMusicTrack = NULL;
		sgnMusicTrack = 6;
	}
}

void __fastcall music_start(int nTrack)
{
	BOOL success;

	/// ASSERT: assert((DWORD) nTrack < NUM_MUSIC);
	music_stop();
	if (gbSndInited && gbMusicOn) {
#ifdef _DEBUG
		SFileEnableDirectAccess(FALSE);
#endif
		success = SFileOpenFile(sgszMusicTracks[nTrack], &sgpMusicTrack);
#ifdef _DEBUG
		SFileEnableDirectAccess(TRUE);
#endif
		sound_create_primary_buffer(sgpMusicTrack);
		if (!success) {
			sgpMusicTrack = NULL;
		} else {
			int bytestoread = (int)SFileGetFileSize((HANDLE)sgpMusicTrack, 0);
			char *buffer = (char *)DiabloAllocPtr(bytestoread);
			SFileReadFile(sgpMusicTrack, buffer, bytestoread, NULL, 0);

			SDL_RWops *rw = SDL_RWFromConstMem(buffer, bytestoread);
			Mix_Music *Song = Mix_LoadMUS_RW(rw, 1);
			Mix_VolumeMusic(MIX_MAX_VOLUME - MIX_MAX_VOLUME * sglMusicVolume / VOLUME_MIN);
			Mix_PlayMusic(Song, -1);

			sgnMusicTrack = nTrack;
		}
	}
}

void __fastcall sound_disable_music(BOOL disable)
{
	if (disable) {
		music_stop();
	} else if (sgnMusicTrack != 6) {
		music_start(sgnMusicTrack);
	}
}

int __fastcall sound_get_or_set_music_volume(int volume)
{
	if (volume == 1)
		return sglMusicVolume;

	sglMusicVolume = volume;

	if (sgpMusicTrack)
		SFileDdaSetVolume(sgpMusicTrack, volume, 0);

	return sglMusicVolume;
}

int __fastcall sound_get_or_set_sound_volume(int volume)
{
	if (volume == 1)
		return sglSoundVolume;

	sglSoundVolume = volume;

	return sglSoundVolume;
}

/*
SDL_AudioSpec wanted;
static Uint32 audio_len;
static Uint8 *audio_pos;

void fill_audio(void *udata, Uint8 *stream, int len)
{
	//DUMMY();
	//wanted.freq = 44100; //seems not to be used at this time
	//wanted.format = AUDIO_S16LSB; //seems not to be used at this time
	//wanted.channels = 2; // 1 = mono, 2 = stereo, seems not to be used at this time
	//wanted.samples = 1024; // Good low-latency value for callback, seems not to be used at this time
	//wanted.callback = fill_audio; //seems not to be used at this time
	//wanted.userdata = NULL; //seems not to be used at this time

	// Only play if we have data left
	if (audio_len == 0)
		return;

	// Mix as much data as possible
	len = (len > audio_len ? audio_len : len);
	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
	audio_pos += len;
	audio_len -= len;
}
*/
