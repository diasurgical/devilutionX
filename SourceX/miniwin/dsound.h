#include "devilution.h"

#ifdef VITA
#ifdef USE_SDL1
#include <SDL/SDL_mixer.h>
#else
#include <SDL2/SDL_mixer.h>
#endif
#else
#include <SDL_mixer.h>
#endif

namespace dvl {

struct DirectSoundBuffer final : public IDirectSoundBuffer {
public:
	void Release() override;
	void GetStatus(LPDWORD pdwStatus) override;
	void Play(int lVolume, int lPan) override;
	void Stop() override;
	int SetChunk(BYTE *fileData, DWORD dwBytes) override;

private:
	Mix_Chunk *chunk;
};

} // namespace dvl
