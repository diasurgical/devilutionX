#pragma once

#include "miniwin.h"

typedef void *LPDSBCAPS, *LPDIRECTSOUND, *LPCDSBUFFERDESC;

DECLARE_INTERFACE_(IDirectSoundBuffer, IUnknown)
{
	// clang-format off
	STDMETHOD(GetCaps)(THIS_ LPDSBCAPS pDSBufferCaps);
	STDMETHOD(GetCurrentPosition)(THIS_ LPDWORD pdwCurrentPlayCursor, LPDWORD pdwCurrentWriteCursor);
	STDMETHOD(GetFormat)(THIS_ LPWAVEFORMATEX pwfxFormat, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten);
	STDMETHOD(GetVolume)(THIS_ LPLONG plVolume);
	STDMETHOD(GetPan)(THIS_ LPLONG plPan);
	STDMETHOD(GetFrequency)(THIS_ LPDWORD pdwFrequency);
	STDMETHOD(GetStatus)(THIS_ LPDWORD pdwStatus);
	STDMETHOD(Initialize)(THIS_ LPDIRECTSOUND pDirectSound, LPCDSBUFFERDESC pcDSBufferDesc);
	STDMETHOD(Lock)(THIS_ DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1,
			LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags);
	STDMETHOD(Play)(THIS_ DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags);
	STDMETHOD(SetCurrentPosition)(THIS_ DWORD dwNewPosition);
	STDMETHOD(SetFormat)(THIS_ LPCWAVEFORMATEX pcfxFormat);
	STDMETHOD(SetVolume)(THIS_ LONG lVolume);
	STDMETHOD(SetPan)(THIS_ LONG lPan);
	STDMETHOD(SetFrequency)(THIS_ DWORD dwFrequency);
	STDMETHOD(Stop)(THIS);
	STDMETHOD(Unlock)(THIS_ LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2);
	STDMETHOD(Restore)(THIS);
	// clang-format on
};

typedef void *LPDIRECTSOUNDBUFFER, *LPDSCAPS, *LPUNKNOWN, *LPCGUID;

DECLARE_INTERFACE_(IDirectSound, IUnknown)
{
	// clang-format off
	STDMETHOD(CreateSoundBuffer)(THIS_ LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER *ppDSBuffer, LPUNKNOWN pUnkOuter);
	STDMETHOD(GetCaps)(THIS_ LPDSCAPS pDSCaps);
	STDMETHOD(DuplicateSoundBuffer)(THIS_ LPDIRECTSOUNDBUFFER pDSBufferOriginal, LPDIRECTSOUNDBUFFER *ppDSBufferDuplicate);
	STDMETHOD(SetCooperativeLevel)(THIS_ HWND hwnd, DWORD dwLevel);
	STDMETHOD(Compact)(THIS);
	STDMETHOD(GetSpeakerConfig)(THIS_ LPDWORD pdwSpeakerConfig);
	STDMETHOD(SetSpeakerConfig)(THIS_ DWORD dwSpeakerConfig);
	STDMETHOD(Initialize)(THIS_ LPCGUID pcGuidDevice);
	// clang-format on
};
