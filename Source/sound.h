/**
 * @file sound.h
 *
 * Interface of functions setting up the audio pipeline.
 */
#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "miniwin/miniwin.h"

#ifndef NOSOUND
#include "utils/soundsample.h"
#endif

namespace devilution {

#define VOLUME_MIN -1600
#define VOLUME_MAX 0
#define VOLUME_STEPS 64

#define ATTENUATION_MIN -6400
#define ATTENUATION_MAX 0

#define PAN_MIN -6400
#define PAN_MAX 6400

enum _music_id : uint8_t {
	TMUSIC_TOWN,
	TMUSIC_L1,
	TMUSIC_L2,
	TMUSIC_L3,
	TMUSIC_L4,
	TMUSIC_L5,
	TMUSIC_L6,
	TMUSIC_INTRO,
	NUM_MUSIC,
};

struct TSnd {
	uint32_t start_tc;

#ifndef NOSOUND
	SoundSample DSB;

	bool isPlaying()
	{
		return DSB.IsPlaying();
	}
#else
	bool isPlaying()
	{
		return false;
	}
#endif

	~TSnd();
};

extern bool gbSndInited;
void ClearDuplicateSounds();
void snd_stop_snd(TSnd *pSnd);
void snd_play_snd(TSnd *pSnd, int lVolume, int lPan);
std::unique_ptr<TSnd> sound_file_load(const char *path, bool stream = false);
void snd_init();
void snd_deinit();
void music_stop();
void music_start(uint8_t nTrack);
void sound_disable_music(bool disable);
int sound_get_or_set_music_volume(int volume);
int sound_get_or_set_sound_volume(int volume);
void music_mute();
void music_unmute();

/* data */

extern bool gbMusicOn;
extern bool gbSoundOn;

} // namespace devilution
