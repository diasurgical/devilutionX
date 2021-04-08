/**
 * @file sound.h
 *
 * Interface of functions setting up the audio pipeline.
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TSnd {
	const char *sound_path;
	/** Used for streamed audio */
	HANDLE file_handle;
	SoundSample *DSB;
	Uint32 start_tc;
} TSnd;

extern BOOLEAN gbSndInited;

void snd_update(BOOL bStopAll);
void snd_stop_snd(TSnd *pSnd);
BOOL snd_playing(TSnd *pSnd);
void snd_play_snd(TSnd *pSnd, int lVolume, int lPan);
TSnd *sound_file_load(const char *path, bool stream = false);
void sound_file_cleanup(TSnd *sound_file);
void snd_init();
void sound_cleanup();
void music_stop();
void music_start(int nTrack);
void sound_disable_music(BOOL disable);
int sound_get_or_set_music_volume(int volume);
int sound_get_or_set_sound_volume(int volume);

/* data */

extern BOOLEAN gbMusicOn;
extern BOOLEAN gbSoundOn;
extern BOOLEAN gbDupSounds;

#ifdef __cplusplus
}
#endif

}
