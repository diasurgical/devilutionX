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
	// clang-format on
};

class DirectSound : public IDirectSound {
public:
	virtual ULONG Release();
	virtual HRESULT CreateSoundBuffer(LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER *ppDSBuffer, LPUNKNOWN pUnkOute);
	virtual HRESULT GetCaps(LPDSCAPS pDSCaps);
	virtual HRESULT DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER pDSBufferOriginal, LPDIRECTSOUNDBUFFER *ppDSBufferDuplicate);
};

const auto DVL__FACDS = 0x878;

template<class T> constexpr HRESULT DVL_MAKE_DSHRESULT(T&& code)
{
	return DVL_MAKE_HRESULT(1, DVL__FACDS, code);
}

const auto DVL_DSBCAPS_PRIMARYBUFFER = 0x00000001;
const auto DVL_DSBCAPS_STATIC = 0x00000002;
const auto DVL_DSBCAPS_CTRLPAN = 0x00000040;
const auto DVL_DSBCAPS_CTRLVOLUME = 0x00000080;
const auto DVL_WAVE_FORMAT_PCM = 1;
const auto DVL_DS_OK = 0;
const auto DVL_DSERR_INVALIDPARAM = 0x80070057;
const auto DVL_ERROR_SUCCESS = 0L;
const auto DVL_DSSCL_EXCLUSIVE = 0x00000003;
const auto DVL_DSBSTATUS_PLAYING = 0x00000001;
const auto DVL_DSERR_BUFFERLOST = DVL_MAKE_DSHRESULT(150);
