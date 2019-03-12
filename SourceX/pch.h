#include <assert.h>
#include <deque>
#include <dirent.h>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <set>
#include <map>
#include <stdarg.h>
#include <string>
#include <utility>
#include <vector>

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_thread.h>
#include <SDL_ttf.h>

#include "devilution.h"
#include "DiabloUI/diabloui.h"


struct DirectSoundBuffer : public IDirectSoundBuffer {
public:
	METHOD ULONG Release();
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
