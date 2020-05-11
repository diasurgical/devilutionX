#include "all.h"
#include "stubs.h"
#include <SDL.h>

namespace dvl {

#ifdef _XBOX
typedef union
{
    double value;
    struct
    {
        unsigned int lsw;
        unsigned int msw;
    } parts;
} ieee_double_shape_type;

#define GET_HIGH_WORD(i,d)					\
do {								\
  ieee_double_shape_type gh_u;					\
  gh_u.value = (d);						\
  (i) = gh_u.parts.msw;						\
} while (0)

#define SET_HIGH_WORD(d,v)					\
do {								\
  ieee_double_shape_type sh_u;					\
  sh_u.value = (d);						\
  sh_u.parts.msw = (v);						\
  (d) = sh_u.value;						\
} while (0)

double copysign(double x, double y)
{
	unsigned int hx,hy;
	GET_HIGH_WORD(hx,x);
	GET_HIGH_WORD(hy,y);
	SET_HIGH_WORD(x,(hx&0x7fffffff)|(hy&0x80000000));
        return x;
}
#endif

///// SoundSample /////

void SoundSample::Release()
{
	Mix_FreeChunk(chunk);
};

bool SoundSample::IsPlaying()
{
	for (int i = 1; i < Mix_AllocateChannels(-1); i++) {
		if (Mix_GetChunk(i) == chunk && Mix_Playing(i)) {
			return true;
		}
	}

	return false;
};

void SoundSample::Play(int lVolume, int lPan)
{
	int channel = Mix_PlayChannel(-1, chunk, 0);
	if (channel == -1) {
		SDL_Log("Too few channels, skipping sound");
		return;
	}

	Mix_Volume(channel, pow((double)10, (double)lVolume / 2000.0) * MIX_MAX_VOLUME);
	int pan = copysign(pow((double)10, -abs(lPan) / 2000.0) * 255, (double)lPan);
	Mix_SetPanning(channel, pan > 0 ? pan : 255, pan < 0 ? abs(pan) : 255);
};

void SoundSample::Stop()
{
	for (int i = 1; i < Mix_AllocateChannels(-1); i++) {
		if (Mix_GetChunk(i) != chunk) {
			continue;
		}

		Mix_HaltChannel(i);
	}
};

int SoundSample::SetChunk(BYTE *fileData, DWORD dwBytes)
{
	SDL_RWops *buf1 = SDL_RWFromConstMem(fileData, dwBytes);
	if (buf1 == NULL) {
		return -1;
	}

	chunk = Mix_LoadWAV_RW(buf1, 1);
	if (chunk == NULL) {
		return -1;
	}

	return 0;
};

} // namespace dvl
