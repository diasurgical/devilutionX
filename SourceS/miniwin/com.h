#pragma once

namespace dvl {

struct IDirectSoundBuffer {
	virtual void Release() = 0;
	virtual bool IsPlaying() = 0;
	virtual void Play(int lVolume, int lPan) = 0;
	virtual void Stop() = 0;
	virtual int SetChunk(BYTE *fileData, DWORD dwBytes) = 0;
};

typedef IDirectSoundBuffer *LPDIRECTSOUNDBUFFER;

const auto DVL_ERROR_SUCCESS = 0L;

} // namespace dvl
