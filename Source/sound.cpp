//HEADER_GOES_HERE

#include "../types.h"

float sound_cpp_init_value;

#ifndef NO_GLOBALS
LPDIRECTSOUNDBUFFER DSBs[8];
LPDIRECTSOUND sglpDS;
char gbSndInited;
int sglMusicVolume;
int sglSoundVolume;
HMODULE hDsound_dll; // idb
HANDLE sgpMusicTrack;
LPDIRECTSOUNDBUFFER sglpDSB;
#endif

const int sound_inf = 0x7F800000; // weak

/* data */

UCHAR gbMusicOn = 1; // weak
UCHAR gbSoundOn = 1; // weak
char gbDupSounds = 1; // weak
int sgnMusicTrack = 6;
char *sgszMusicTracks[NUM_MUSIC] =
{
  "Music\\DTowne.wav",
  "Music\\DLvlA.wav",
  "Music\\DLvlB.wav",
  "Music\\DLvlC.wav",
  "Music\\DLvlD.wav",
  "Music\\Dintro.wav"
};
char unk_volume[4][2] =
{
	{ 15, -16 },
	{ 15, -16 },
	{ 30, -31 },
	{ 30, -31 }
};

struct sound_cpp_init
{
	sound_cpp_init()
	{
		sound_cpp_init_value = sound_inf;
	}
} _sound_cpp_init;
// 47F24C: using guessed type int sound_inf;

void __fastcall snd_update(BOOL bStopAll)
{
	DWORD error_code;

	for (DWORD i = 0; i < 8; i++) {
		if ( !DSBs[i] )
			continue;

		if ( !bStopAll && !DSBs[i]->GetStatus(&error_code) && error_code == DSBSTATUS_PLAYING )
			continue;

		DSBs[i]->Stop();
		DSBs[i]->Release();
		DSBs[i] = NULL;
	}
}

void __fastcall snd_stop_snd(TSnd *pSnd)
{
	if ( pSnd && pSnd->DSB )
		pSnd->DSB->Stop();
}

BOOL __fastcall snd_playing(TSnd *pSnd)
{
	DWORD error_code;

	if ( !pSnd )
		return FALSE;

	if ( pSnd->DSB == NULL )
		return FALSE;

	if ( pSnd->DSB->GetStatus(&error_code) )
		return FALSE;

	return error_code == DSBSTATUS_PLAYING;
}

void __fastcall snd_play_snd(TSnd *pSnd, int lVolume, int lPan)
{
	if ( !pSnd || !gbSoundOn ) {
		return;
	}

	LPDIRECTSOUNDBUFFER DSB = pSnd->DSB;
	if ( !DSB ) {
		return;
	}

	DWORD tc = GetTickCount();
	if ( tc - pSnd->start_tc < 80 ) {
		pSnd->start_tc = GetTickCount();
		return;
	}

	if ( snd_playing(pSnd) ) {
		DSB = sound_dup_channel(pSnd->DSB);
		if (DSB == 0) {
			return;
		}
	}

	lVolume += sglSoundVolume;
	if ( lVolume < VOLUME_MIN ) {
		lVolume = VOLUME_MIN;
	} else if ( lVolume > VOLUME_MAX ) {
		lVolume = VOLUME_MAX;
	}
	DSB->SetVolume(lVolume);

	DSB->SetPan(lPan);

	HRESULT error_code = DSB->Play(0, 0, 0);
	if ( error_code != DSERR_BUFFERLOST) {
		if ( error_code != DS_OK ) {
			DSErrMsg(error_code, 261, "C:\\Src\\Diablo\\Source\\SOUND.CPP");
		}
	} else if ( sound_file_reload(pSnd, DSB) ) {
		DSB->Play(0, 0, 0);
	}

	pSnd->start_tc = tc;
}
// 4A22D5: using guessed type char gbSoundOn;

LPDIRECTSOUNDBUFFER __fastcall sound_dup_channel(LPDIRECTSOUNDBUFFER DSB)
{
	if ( !gbDupSounds ) {
		return NULL;
	}

	for ( DWORD i = 0; i < 8; i++ ) {
		if (!DSBs[i]) {
			if ( sglpDS->DuplicateSoundBuffer(DSB, &DSBs[i]) != DS_OK ) {
				return NULL;
			}

			return DSBs[i];
		}
	}

	return NULL;

}
// 4A22D6: using guessed type char gbDupSounds;

BOOL __fastcall sound_file_reload(TSnd *sound_file, LPDIRECTSOUNDBUFFER DSB)
{
	if ( DSB->Restore() )
		return FALSE;

	HANDLE file;
	LPVOID buf1, buf2;
	DWORD size1, size2;

	BOOL rv = FALSE;

	WOpenFile(sound_file->sound_path, &file, 0);
	WSetFilePointer(file, sound_file->offset, 0, 0);
	if ( DSB->Lock(0, sound_file->len, &buf1, &size1, &buf2, &size2, 0) == DS_OK ) {
		WReadFile(file, buf1, size1);
		if ( DSB->Unlock(buf1, size1, buf2, size2) == DS_OK ) {
			rv = TRUE;
		}
	}

	WCloseFile(file);

	return rv;
}

TSnd *__fastcall sound_file_load(char *path)
{
	if ( !sglpDS )
		return NULL;

	void *file, *wave_file;
	WOpenFile(path, &file, 0);
	TSnd *pSnd = (TSnd *)DiabloAllocPtr(40);
	memset(pSnd, 0, sizeof(TSnd));
	pSnd->sound_path = path;
	pSnd->start_tc = GetTickCount() - 81;

	wave_file = LoadWaveFile(file, &pSnd->fmt, &pSnd->len);
	if ( !wave_file )
		TermMsg("Invalid sound format on file %s", pSnd->sound_path);

	sound_CreateSoundBuffer(pSnd);

	LPVOID buf1, buf2;
	DWORD size1, size2;
	HRESULT error_code = pSnd->DSB->Lock(0, pSnd->len, &buf1, &size1, &buf2, &size2, 0);
	if ( error_code != DS_OK )
		DSErrMsg(error_code, 318, "C:\\Src\\Diablo\\Source\\SOUND.CPP");

	memcpy(buf1, (char *)wave_file + pSnd->offset, size1);

	error_code = pSnd->DSB->Unlock(buf1, size1, buf2, size2);
	if ( error_code != DS_OK )
		DSErrMsg(error_code, 325, "C:\\Src\\Diablo\\Source\\SOUND.CPP");

	mem_free_dbg(wave_file);
	WCloseFile(file);

	return pSnd;
}
// 456F07: could not find valid save-restore pair for esi

void __fastcall sound_CreateSoundBuffer(TSnd *sound_file)
{
	DSBUFFERDESC DSB;
	memset(&DSB, 0, sizeof(DSBUFFERDESC));

	DSB.dwBufferBytes = sound_file->len;
	DSB.lpwfxFormat = &sound_file->fmt;
	DSB.dwSize = sizeof(DSBUFFERDESC);
	DSB.dwFlags = DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN|DSBCAPS_STATIC;

	HRESULT error_code = sglpDS->CreateSoundBuffer(&DSB, &sound_file->DSB, NULL);
	if ( error_code != ERROR_SUCCESS )
		DSErrMsg(error_code, 282, "C:\\Src\\Diablo\\Source\\SOUND.CPP");
}

void __fastcall sound_file_cleanup(TSnd *sound_file)
{
	if ( sound_file ) {
		if ( sound_file->DSB ) {
			sound_file->DSB->Stop();
			sound_file->DSB->Release();
			sound_file->DSB = NULL;
		}

		mem_free_dbg(sound_file);
	}
}

void __fastcall snd_init(HWND hWnd)
{
	sound_load_volume("Sound Volume", &sglSoundVolume);
	gbSoundOn = sglSoundVolume > VOLUME_MIN;

	sound_load_volume("Music Volume", &sglMusicVolume);
	gbMusicOn = sglMusicVolume > VOLUME_MIN;

	if ( sound_DirectSoundCreate(NULL, &sglpDS, NULL) != DS_OK )
		sglpDS = NULL;

	if ( sglpDS && sglpDS->SetCooperativeLevel(hWnd, DSSCL_EXCLUSIVE) == DS_OK )
		sound_create_primary_buffer(0);

	SVidInitialize(sglpDS);
	SFileDdaInitialize(sglpDS);

	gbSndInited = sglpDS != NULL;
}
// 4A22D4: using guessed type char gbMusicOn;
// 4A22D5: using guessed type char gbSoundOn;

void __fastcall sound_load_volume(char *value_name, int *value)
{
	int v = *value;
	if ( !SRegLoadValue("Diablo", value_name, 0, &v) ) {
		v = VOLUME_MAX;
	}
	*value = v;

	if ( *value < VOLUME_MIN ) {
		*value = VOLUME_MIN;
	} else if ( *value > VOLUME_MAX ) {
		*value = VOLUME_MAX;
	}
	*value -= *value % 100;
}

void __fastcall sound_create_primary_buffer(HANDLE music_track)
{
	HRESULT error_code;
	DSBUFFERDESC dsbuf;
	WAVEFORMATEX format;

	if ( !music_track ) {
		memset(&dsbuf, 0, sizeof(DSBUFFERDESC));
		dsbuf.dwSize = sizeof(DSBUFFERDESC);
		dsbuf.dwFlags = DSBCAPS_PRIMARYBUFFER;

		error_code = sglpDS->CreateSoundBuffer(&dsbuf, &sglpDSB, NULL);
		if ( error_code != DS_OK )
			DSErrMsg(error_code, 375, "C:\\Src\\Diablo\\Source\\SOUND.CPP");
	}

	if ( sglpDSB ) {
		DSCAPS dsbcaps;
		dsbcaps.dwSize = sizeof(DSCAPS);

		error_code = sglpDS->GetCaps(&dsbcaps);
		if ( error_code != DS_OK )
			DSErrMsg(error_code, 383, "C:\\Src\\Diablo\\Source\\SOUND.CPP");

		if ( !music_track || !LoadWaveFormat(music_track, &format) ) {
			memset(&format, 0, sizeof(WAVEFORMATEX));
			format.wFormatTag = WAVE_FORMAT_PCM;
			format.nSamplesPerSec = 22050;
			format.wBitsPerSample = 16;
			format.cbSize = 0;
		}

		format.nChannels = 2;
		format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
		format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

		sglpDSB->SetFormat(&format);
	}
}
// 69F100: using guessed type int sglpDSB;

HRESULT __fastcall sound_DirectSoundCreate(LPGUID lpGuid, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
{
	if ( hDsound_dll == NULL ) {
		hDsound_dll = LoadLibrary("dsound.dll");
		if ( hDsound_dll == NULL ) {
			ErrDlg(IDD_DIALOG5, GetLastError(), "C:\\Src\\Diablo\\Source\\SOUND.CPP", 422);
		}
	}

	HRESULT (WINAPI * DirectSoundCreate)(LPGUID lpGuid, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter);
	DirectSoundCreate = (HRESULT(WINAPI*)(LPGUID, LPDIRECTSOUND*, LPUNKNOWN))GetProcAddress(hDsound_dll, "DirectSoundCreate");
	if ( DirectSoundCreate == NULL ) {
		ErrDlg(IDD_DIALOG5, GetLastError(), "C:\\Src\\Diablo\\Source\\SOUND.CPP", 427);
	}
	return DirectSoundCreate(lpGuid, ppDS, pUnkOuter);
}

void __cdecl sound_cleanup()
{
	snd_update(TRUE);
	SVidDestroy();
	SFileDdaDestroy();

	if ( sglpDS ) {
		sglpDS->Release();
		sglpDS = NULL;
	}

	if ( gbSndInited ) {
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
	if ( sgpMusicTrack ) {
		SFileDdaEnd(sgpMusicTrack);
		SFileCloseFile(sgpMusicTrack);
		sgpMusicTrack = 0;
		sgnMusicTrack = 6;
	}
}

void __fastcall music_start(int nTrack)
{
	music_stop();
	if ( sglpDS && gbMusicOn ) {
		BOOL success = SFileOpenFile(sgszMusicTracks[nTrack], &sgpMusicTrack);
		sound_create_primary_buffer(sgpMusicTrack);
		if ( !success ) {
			sgpMusicTrack = 0;
		} else {
			SFileDdaBeginEx(sgpMusicTrack, 0x40000, 0x40000, 0, sglMusicVolume, 0, 0);
			sgnMusicTrack = nTrack;
		}
	}
}
// 4A22D4: using guessed type char gbMusicOn;

void __fastcall sound_disable_music(BOOL disable)
{
	if ( disable ) {
		music_stop();
	} else if ( sgnMusicTrack != 6 ) {
		music_start(sgnMusicTrack);
	}
}

int __fastcall sound_get_or_set_music_volume(int volume)
{
	if ( volume == 1 )
		return sglMusicVolume;

	sglMusicVolume = volume;

	if ( sgpMusicTrack )
		SFileDdaSetVolume(sgpMusicTrack, volume, 0);

	return sglMusicVolume;
}

int __fastcall sound_get_or_set_sound_volume(int volume)
{
	if ( volume == 1 )
		return sglSoundVolume;

	sglSoundVolume = volume;

	return sglSoundVolume;
}
