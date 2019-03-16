#pragma once

struct IDirectSound;
typedef IDirectSound *LPDIRECTSOUND;

typedef struct _DSBUFFERDESC {
	DWORD dwSize;
	DWORD dwFlags;
	DWORD dwBufferBytes;
	DWORD dwReserved;
	LPWAVEFORMATEX lpwfxFormat;
} DSBUFFERDESC, *LPDSBUFFERDESC;

typedef DSBUFFERDESC *LPDSBCAPS, *LPCDSBUFFERDESC;

DECLARE_INTERFACE_(IDirectSoundBuffer, IUnknown)
{
	// clang-format off
	STDMETHOD(GetStatus)(THIS_ LPDWORD pdwStatus) PURE;
	STDMETHOD(Lock)(THIS_ DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1,
			LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags) PURE;
	STDMETHOD(Play)(THIS_ DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags) PURE;
	STDMETHOD(SetFormat)(THIS_ LPCWAVEFORMATEX pcfxFormat) PURE;
	STDMETHOD(SetVolume)(THIS_ LONG lVolume) PURE;
	STDMETHOD(SetPan)(THIS_ LONG lPan) PURE;
	STDMETHOD(Stop)(THIS) PURE;
	STDMETHOD(Unlock)(THIS_ LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2) PURE;
	STDMETHOD(Restore)(THIS) PURE;
	// clang-format on
};

#define DSBCAPS_PRIMARYBUFFER 0x00000001
#define DSBCAPS_STATIC 0x00000002
#define DSBCAPS_CTRLPAN 0x00000040
#define DSBCAPS_CTRLVOLUME 0x00000080

#define DSSCL_EXCLUSIVE 0x00000003

#define WAVE_FORMAT_PCM 1

#define ERROR_SUCCESS 0L

typedef IDirectSoundBuffer *LPDIRECTSOUNDBUFFER;
typedef void *LPUNKNOWN, *LPCGUID;

typedef struct _DSCAPS {
	DWORD dwSize;
	DWORD dwFlags;
	DWORD dwMinSecondarySampleRate;
	DWORD dwMaxSecondarySampleRate;
	DWORD dwPrimaryBuffers;
	DWORD dwMaxHwMixingAllBuffers;
	DWORD dwMaxHwMixingStaticBuffers;
	DWORD dwMaxHwMixingStreamingBuffers;
	DWORD dwFreeHwMixingAllBuffers;
	DWORD dwFreeHwMixingStaticBuffers;
	DWORD dwFreeHwMixingStreamingBuffers;
	DWORD dwMaxHw3DAllBuffers;
	DWORD dwMaxHw3DStaticBuffers;
	DWORD dwMaxHw3DStreamingBuffers;
	DWORD dwFreeHw3DAllBuffers;
	DWORD dwFreeHw3DStaticBuffers;
	DWORD dwFreeHw3DStreamingBuffers;
	DWORD dwTotalHwMemBytes;
	DWORD dwFreeHwMemBytes;
	DWORD dwMaxContigFreeHwMemBytes;
	DWORD dwUnlockTransferRateHwBuffers;
	DWORD dwPlayCpuOverheadSwBuffers;
	DWORD dwReserved1;
	DWORD dwReserved2;
} DSCAPS, *LPDSCAPS;

DECLARE_INTERFACE_(IDirectSound, IUnknown)
{
	// clang-format off
	STDMETHOD(CreateSoundBuffer)(THIS_ LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER *ppDSBuffer, LPUNKNOWN pUnkOuter) PURE;
	STDMETHOD(GetCaps)(THIS_ LPDSCAPS pDSCaps) PURE;
	STDMETHOD(DuplicateSoundBuffer)(THIS_ LPDIRECTSOUNDBUFFER pDSBufferOriginal, LPDIRECTSOUNDBUFFER *ppDSBufferDuplicate) PURE;
	STDMETHOD(SetCooperativeLevel)(THIS_ HWND hwnd, DWORD dwLevel) PURE;
	// clang-format on
};

#define _FACDS 0x878
#define MAKE_DSHRESULT(code) MAKE_HRESULT(1, _FACDS, code)

#define DSBSTATUS_PLAYING 0x00000001

#define E_NOINTERFACE 0x80004002
#define DSERR_PRIOLEVELNEEDED MAKE_DSHRESULT(70)
#define DSERR_BADFORMAT MAKE_DSHRESULT(100)
#define DSERR_NODRIVER MAKE_DSHRESULT(120)
#define DSERR_ALREADYINITIALIZED MAKE_DSHRESULT(130)
#define DSERR_BUFFERLOST MAKE_DSHRESULT(150)
#define DS_OK 0
#define DSERR_INVALIDCALL MAKE_DSHRESULT(50)
#define DSERR_NOAGGREGATION 0x80040110
#define DSERR_OUTOFMEMORY 0x8007000E
#define DSERR_INVALIDPARAM 0x80070057
#define DSERR_ALLOCATED MAKE_DSHRESULT(10)
#define DSERR_CONTROLUNAVAIL MAKE_DSHRESULT(30)

class DirectSound : public IDirectSound {
public:
	METHOD ULONG Release();
	METHOD HRESULT CreateSoundBuffer(LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER *ppDSBuffer, LPUNKNOWN pUnkOute);
	METHOD HRESULT GetCaps(LPDSCAPS pDSCaps);
	METHOD HRESULT DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER pDSBufferOriginal, LPDIRECTSOUNDBUFFER *ppDSBufferDuplicate);
	METHOD HRESULT SetCooperativeLevel(HWND hwnd, DWORD dwLevel);
};
