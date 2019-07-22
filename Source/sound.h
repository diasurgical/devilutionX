//HEADER_GOES_HERE
#ifndef __SOUND_H__
#define __SOUND_H__

extern IDirectSoundBuffer *DSBs[8];
extern BOOLEAN gbSndInited;
extern HMODULE hDsound_dll;

void snd_update(BOOL bStopAll);
void snd_stop_snd(TSnd *pSnd);
BOOL snd_playing(TSnd *pSnd);
void snd_play_snd(TSnd *pSnd, int lVolume, int lPan);
IDirectSoundBuffer *sound_dup_channel(IDirectSoundBuffer *DSB);
BOOL sound_file_reload(TSnd *sound_file, IDirectSoundBuffer *DSB);
TSnd *sound_file_load(char *path);
void sound_CreateSoundBuffer(TSnd *sound_file);
void sound_file_cleanup(TSnd *sound_file);
void snd_init(HWND hWnd);
void sound_load_volume(char *value_name, int *value);
void sound_create_primary_buffer(HANDLE music_track);
HRESULT sound_DirectSoundCreate(LPGUID lpGuid, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter);
void sound_cleanup();
void sound_store_volume(char *key, int value);
void music_stop();
void music_start(int nTrack);
void sound_disable_music(BOOL disable);
int sound_get_or_set_music_volume(int volume);
int sound_get_or_set_sound_volume(int volume);

/* rdata */

/* data */

extern BOOLEAN gbMusicOn;
extern BOOLEAN gbSoundOn;
extern BOOLEAN gbDupSounds;
extern char unk_volume[4][2];

#endif /* __SOUND_H__ */
