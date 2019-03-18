#include "devilution.h"

#include <SDL_mixer.h>

namespace dvl {

struct DirectSoundBuffer : public IDirectSoundBuffer {
public:
        virtual ULONG Release();
        BOOL GetStatus(LPDWORD pdwStatus);
        HRESULT Lock(DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1,
            LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags);
        HRESULT Play(DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags);
        HRESULT SetFormat(LPCWAVEFORMATEX pcfxFormat);
        HRESULT SetVolume(LONG lVolume);
        HRESULT SetPan(LONG lPan);
        HRESULT Stop();
        HRESULT Unlock(LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2);
        HRESULT Restore();
    private:
        int volume = 0;
        int pan = 0;
        Mix_Chunk *chunk;
};

}  // namespace dvl
