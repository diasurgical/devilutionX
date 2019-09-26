#include "devilution.h"

#include <SDL_mixer.h>

namespace dvl {

struct DirectSoundBuffer final : public IDirectSoundBuffer {
public:
	void Release() override;
	void GetStatus(LPDWORD pdwStatus) override;
	void Play(int lVolume, int lPan) override;
	void Stop() override;
	const char *SetChunk(BYTE *fileData, DWORD dwBytes) override;

private:
	Mix_Chunk *chunk;
};

} // namespace dvl
