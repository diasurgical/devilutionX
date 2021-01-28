#pragma once
#include <SDL_mixer.h>

namespace dvl {

typedef struct SoundSample final {
public:
	void Release();
	bool IsPlaying();
	void Play(int lVolume, int lPan, int channel = -1);
	void Stop();
	int SetChunk(BYTE *fileData, DWORD dwBytes);
	int GetLength();

private:
	Mix_Chunk *chunk;
} SoundSample;

} // namespace dvl
