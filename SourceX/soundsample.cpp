#include "all.h"
#include "stubs.h"
#include <SDL.h>

namespace dvl {

///// SoundSample /////

void SoundSample::Release()
{
	Mix_FreeChunk(chunk);
};

/**
 * @brief Check if a the sound is being played atm
 */
bool SoundSample::IsPlaying()
{
	if (chunk == NULL)
		return false;

	int channels = Mix_AllocateChannels(-1);
	for (int i = 0; i < channels; i++) {
		if (Mix_GetChunk(i) == chunk && Mix_Playing(i)) {
			return true;
		}
	}

	return false;
};

/**
 * @brief Start playing the sound
 */
void SoundSample::Play(int lVolume, int lPan, int channel)
{
	if (chunk == NULL)
		return;

	channel = Mix_PlayChannel(channel, chunk, 0);
	if (channel == -1) {
		SDL_Log("Too few channels, skipping sound");
		return;
	}

	Mix_Volume(channel, pow((double)10, (double)lVolume / 2000.0) * MIX_MAX_VOLUME);
	int pan = copysign(pow((double)10, -abs(lPan) / 2000.0) * 255, (double)lPan);
	Mix_SetPanning(channel, pan > 0 ? pan : 255, pan < 0 ? abs(pan) : 255);
};

/**
 * @brief Stop playing the sound
 */
void SoundSample::Stop()
{
	if (chunk == NULL)
		return;

	int channels = Mix_AllocateChannels(-1);
	for (int i = 0; i < channels; i++) {
		if (Mix_GetChunk(i) != chunk) {
			continue;
		}

		Mix_HaltChannel(i);
	}
};

/**
 * @brief This can load WAVE, AIFF, RIFF, OGG, and VOC formats
 * @param fileData Buffer containing file data
 * @param dwBytes Length of buffer
 * @return 0 on success, -1 otherwise
 */
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

/**
 * @return Audio duration in ms
 */
int SoundSample::GetLength()
{
	if (chunk == NULL)
		return 0;

	int frequency, channels;
	Uint16 format;
	Mix_QuerySpec(&frequency, &format, &channels);

	int bytePerSample = 2;
	if (format == AUDIO_U8 || format == AUDIO_S8) {
		bytePerSample = 1;
	}

	return chunk->alen * 1000 / (frequency * channels * bytePerSample);
};

} // namespace dvl
