#include "devilution.h"

#include <SDL_mixer.h>

namespace dvl {

struct DirectSoundBuffer : public IDirectSoundBuffer {
public:
        virtual void Release();
        void GetStatus(LPDWORD pdwStatus);
        void Play(int lVolume, int lPan);
        void Stop();
        const char *SetChunk(BYTE *fileData, DWORD dwBytes);
    private:
        Mix_Chunk *chunk;
};

}  // namespace dvl
