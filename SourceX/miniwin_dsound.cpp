#include "pch.h"

ULONG __stdcall DirectSound::Release()
{
	Mix_CloseAudio();
	return 0;
};

HRESULT __stdcall DirectSound::CreateSoundBuffer(LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER *ppDSBuffer, LPUNKNOWN pUnkOute)
{
	*ppDSBuffer = new DirectSoundBuffer();
	return DS_OK;
};

HRESULT __stdcall DirectSound::GetCaps(LPDSCAPS pDSCaps)
{
	return DS_OK;
};

/**
 * @brief SDL handels this for us when using Mix_PlayChannel(-1);
 */
HRESULT __stdcall DirectSound::DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER pDSBufferOriginal, LPDIRECTSOUNDBUFFER *ppDSBufferDuplicate)
{
	UNIMPLEMENTED();
	return DS_OK;
};

HRESULT __stdcall DirectSound::SetCooperativeLevel(HWND hwnd, DWORD dwLevel)
{
	return DS_OK;
};

///// DirectSoundBuffer /////

ULONG __stdcall DirectSoundBuffer::Release()
{
	Mix_FreeChunk(chunk);

	return 0;
};

/**
 * @brief Only used for handeling duplicates 
 */
HRESULT __stdcall DirectSoundBuffer::GetStatus(LPDWORD pdwStatus)
{
	return DSERR_INVALIDPARAM;
};

HRESULT __stdcall DirectSoundBuffer::Lock(DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1,
    LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags)
{
	*pdwAudioBytes1 = dwBytes;
	*ppvAudioPtr1 = malloc(dwBytes);

	return DS_OK;
};

HRESULT __stdcall DirectSoundBuffer::Play(DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags)
{
	int channel = Mix_PlayChannel(-1, chunk, 0);
	if (channel == -1) {
		SDL_Log("To few channels, skipping sound\n");
		return DS_OK;
	}

	Mix_Volume(channel, volume);
	int panned = 255 - 255 * abs(pan) / 10000;
	Mix_SetPanning(channel, pan <= 0 ? 255 : panned, pan >= 0 ? 255 : panned);

	return DS_OK;
};

HRESULT __stdcall DirectSoundBuffer::SetFormat(LPCWAVEFORMATEX pcfxFormat)
{
	return DS_OK;
};

HRESULT __stdcall DirectSoundBuffer::SetVolume(LONG lVolume)
{
	volume = MIX_MAX_VOLUME - MIX_MAX_VOLUME * lVolume / VOLUME_MIN;

	return DS_OK;
};

HRESULT __stdcall DirectSoundBuffer::SetPan(LONG lPan)
{
	pan = lPan;
	return DS_OK;
};

HRESULT __stdcall DirectSoundBuffer::Stop()
{
	for (int i = 1; i < Mix_AllocateChannels(-1); i++){
		if (Mix_GetChunk(i) != chunk) {
			continue;
		}

		Mix_HaltChannel(i);
	}

	return DS_OK;
};

HRESULT __stdcall DirectSoundBuffer::Unlock(LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2)
{
	SDL_RWops *rw = SDL_RWFromConstMem(pvAudioPtr1, dwAudioBytes1);
	chunk = Mix_LoadWAV_RW(rw, 1);
	free(pvAudioPtr1);

	return DS_OK;
};

HRESULT __stdcall DirectSoundBuffer::Restore()
{
	return DS_OK;
};
