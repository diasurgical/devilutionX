#pragma once

namespace dvl {

struct IDirectSoundBuffer {
	virtual WINAPI void Release() = 0;
	virtual WINAPI void GetStatus(LPDWORD pdwStatus) = 0;
	virtual WINAPI void Play(int lVolume, int lPan) = 0;
	virtual WINAPI void Stop() = 0;
	virtual WINAPI const char *SetChunk(BYTE *fileData, DWORD dwBytes) = 0;
};

typedef IDirectSoundBuffer *LPDIRECTSOUNDBUFFER;

const auto DVL_DS_OK = 0;
const auto DVL_ERROR_SUCCESS = 0L;
const auto DVL_DSBSTATUS_PLAYING = 0x00000001;

constexpr HRESULT DVL_E_FAIL = 0x80004005L;
constexpr HRESULT DVL_S_OK = 0;

constexpr auto DVL_SW_HIDE = 0;
constexpr auto DVL_SW_SHOWNORMAL = 1;
constexpr auto DVL_SM_CXSCREEN = 0;
constexpr auto DVL_SM_CYSCREEN = 1;

} // namespace dvl
