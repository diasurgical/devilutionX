/**
 * @file sound.h
 *
 * Interface of functions setting up the audio pipeline.
 */
#pragma once

#include <stdint.h>

namespace devilution {

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
	const char *sound_path;
	/** Used for streamed audio */
	HANDLE file_handle;
	SoundSample *DSB;
	Uint32 start_tc;
};

extern bool gbSndInited;

void snd_update(bool bStopAll);
void snd_stop_snd(TSnd *pSnd);
bool snd_playing(TSnd *pSnd);
void snd_play_snd(TSnd *pSnd, int lVolume, int lPan);
TSnd *sound_file_load(const char *path, bool stream = false);
void sound_file_cleanup(TSnd *sound_file);
void snd_init();
void sound_cleanup();
void music_stop();
void music_start(uint8_t nTrack);
void sound_disable_music(bool disable);
int sound_get_or_set_music_volume(int volume);
int sound_get_or_set_sound_volume(int volume);

/* data */

extern bool gbMusicOn;
extern bool gbSoundOn;
extern bool gbDupSounds;

}
