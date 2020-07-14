#pragma once

namespace dvl {

struct IDirectSoundBuffer {
	virtual void Release() = 0;
	virtual void GetStatus(LPDWORD pdwStatus) = 0;
	virtual void Play(int lVolume, int lPan) = 0;
	virtual void Stop() = 0;
	virtual int SetChunk(BYTE *fileData, DWORD dwBytes) = 0;
	
/*

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

*/

};

typedef IDirectSoundBuffer *LPDIRECTSOUNDBUFFER;

const auto DVL_DS_OK = 0;
const auto DVL_ERROR_SUCCESS = 0L;
const auto DVL_DSBSTATUS_PLAYING = 0x00000001;

constexpr auto DVL_SW_HIDE = 0;
constexpr auto DVL_SW_SHOWNORMAL = 1;

} // namespace dvl
