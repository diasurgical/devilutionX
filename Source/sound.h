//HEADER_GOES_HERE
#ifndef __SOUND_H__
#define __SOUND_H__

extern IDirectSoundBuffer *DSBs[8];
extern BOOLEAN gbSndInited;
extern HMODULE hDsound_dll;

void __fastcall snd_update(BOOL bStopAll);
void __fastcall snd_stop_snd(TSnd *pSnd);
BOOL __fastcall snd_playing(TSnd *pSnd);
void __fastcall snd_play_snd(TSnd *pSnd, int lVolume, int lPan);
IDirectSoundBuffer *__fastcall sound_dup_channel(IDirectSoundBuffer *DSB);
BOOL __fastcall sound_file_reload(TSnd *sound_file, IDirectSoundBuffer *DSB);
TSnd *__fastcall sound_file_load(char *path);
void __fastcall sound_CreateSoundBuffer(TSnd *sound_file);
void __fastcall sound_file_cleanup(TSnd *sound_file);
void __fastcall snd_init(HWND hWnd);
void __fastcall sound_load_volume(char *value_name, int *value);
void __fastcall sound_create_primary_buffer(HANDLE music_track);
HRESULT __fastcall sound_DirectSoundCreate(LPGUID lpGuid, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter);
void __cdecl sound_cleanup();
void __fastcall sound_store_volume(char *key, int value);
void __cdecl music_stop();
void __fastcall music_start(int nTrack);
void __fastcall sound_disable_music(BOOL disable);
int __fastcall sound_get_or_set_music_volume(int volume);
int __fastcall sound_get_or_set_sound_volume(int volume);

/* rdata */

/* data */

extern BYTE gbMusicOn;
extern BYTE gbSoundOn;
extern BYTE gbDupSounds;
extern signed char unk_volume[4][2];

#endif /* __SOUND_H__ */
