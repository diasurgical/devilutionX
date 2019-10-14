#pragma once

namespace dvl {

struct IDirectSoundBuffer {
	virtual void Release() = 0;
	virtual void GetStatus(LPDWORD pdwStatus) = 0;
	virtual void Play(int lVolume, int lPan) = 0;
	virtual void Stop() = 0;
	virtual int SetChunk(BYTE *fileData, DWORD dwBytes) = 0;
};

typedef IDirectSoundBuffer *LPDIRECTSOUNDBUFFER;

const auto DVL_DS_OK = 0;
const auto DVL_ERROR_SUCCESS = 0L;
const auto DVL_DSBSTATUS_PLAYING = 0x00000001;

constexpr auto DVL_SW_HIDE = 0;
constexpr auto DVL_SW_SHOWNORMAL = 1;

} // namespace dvl
