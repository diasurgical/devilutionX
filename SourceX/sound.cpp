//HEADER_GOES_HERE

#include "../types.h"
#include "pch.h"

LPDIRECTSOUNDBUFFER DSBs[8];
LPDIRECTSOUND sglpDS;
char gbSndInited;
int sglMusicVolume;
int sglSoundVolume;
HMODULE hDsound_dll;
HANDLE sgpMusicTrack;
LPDIRECTSOUNDBUFFER sglpDSB;

/* data */

BYTE gbMusicOn = TRUE;
BYTE gbSoundOn = TRUE;
BYTE gbDupSounds = TRUE;
int sgnMusicTrack = 6;
char *sgszMusicTracks[NUM_MUSIC] = {
	"Music\\DTowne.wav",
	"Music\\DLvlA.wav",
	"Music\\DLvlB.wav",
	"Music\\DLvlC.wav",
	"Music\\DLvlD.wav",
	"Music\\Dintro.wav"
};
char unk_volume[4][2] = {
	{ 15, -16 },
	{ 15, -16 },
	{ 30, -31 },
	{ 30, -31 }
};

void __fastcall snd_update(BOOL bStopAll)
{
	DUMMY_ONCE();
}

void __fastcall snd_stop_snd(TSnd *pSnd)
{
	DUMMY_ONCE();
	Mix_HaltMusic();
}

BOOL __fastcall snd_playing(TSnd *pSnd)
{
	DUMMY();
	return false;
}

void __fastcall snd_play_snd(TSnd *pSnd, int lVolume, int lPan)
{
	Mix_PlayChannel(-1, (Mix_Chunk *)(pSnd->DSB), 0);
}

TSnd *__fastcall sound_file_load(char *path)
{
	int bytestoread;
	int nrread;
	HANDLE file;

	//This opens the file and reads it, makes Mix_chunk pointer to it.
	//Once this is done the pointer is stored TSnd Struct

	SFileOpenFile(path, &file);
	bytestoread = (int)SFileGetFileSize((HANDLE)file, 0);
	unsigned char *MSFXBuffer = DiabloAllocPtr(bytestoread);
	SFileReadFile(file, MSFXBuffer, bytestoread, (LPDWORD)&nrread, 0);
	SDL_RWops *rw = SDL_RWFromMem(MSFXBuffer, bytestoread);
	Mix_Chunk *SoundFX = Mix_LoadWAV_RW(rw, 1);

	TSnd *fx = (TSnd *)malloc(sizeof(TSnd));
	memset(fx, 0, sizeof(TSnd));
	fx->DSB = (LPDIRECTSOUNDBUFFER)SoundFX;
	fx->start_tc = 0;
	fx->sound_path = NULL;

	return fx;
}

void __fastcall sound_file_cleanup(TSnd *sound_file)
{
	DUMMY_ONCE();
}

void __fastcall snd_init(HWND hWnd)
{
	DUMMY();
	printf("\nSND INIT\n");

	/* following function Mix_AllocateChannels allocates the number of channels of simultaneously played sounds.*/
	printf("Opened %i sound channels\n\n", Mix_AllocateChannels(8));

	sound_load_volume("Sound Volume", &sglSoundVolume);
	gbSoundOn = sglSoundVolume > VOLUME_MIN;

	sound_load_volume("Music Volume", &sglMusicVolume);
	gbMusicOn = sglMusicVolume > VOLUME_MIN;

	if (Mix_OpenAudio(44100, AUDIO_S16LSB, 2, 1024) < 0) {
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		return;
	}

	gbSndInited = 1;
}

void __fastcall sound_load_volume(char *value_name, int *value)
{
	int v = *value;
	if (!SRegLoadValue("Diablo", value_name, 0, &v)) {
		v = VOLUME_MAX;
	}
	*value = v;

	if (*value < VOLUME_MIN) {
		*value = VOLUME_MIN;
	} else if (*value > VOLUME_MAX) {
		*value = VOLUME_MAX;
	}
	*value -= *value % 100;
}

void __fastcall sound_create_primary_buffer(HANDLE music_track)
{
	DUMMY();
}

void __cdecl sound_cleanup()
{
	snd_update(TRUE);
	//SVidDestroy();
	SFileDdaDestroy();

	if (sglpDS) {
#ifdef __cplusplus
		sglpDS->Release();
#else
		sglpDS->lpVtbl->Release(sglpDS);
#endif
		sglpDS = NULL;
	}

	if (gbSndInited) {
		gbSndInited = FALSE;
		sound_store_volume("Sound Volume", sglSoundVolume);
		sound_store_volume("Music Volume", sglMusicVolume);
	}
}

void __fastcall sound_store_volume(char *key, int value)
{
	SRegSaveValue("Diablo", key, 0, value);
}

void __cdecl music_stop()
{
	DUMMY();
	Mix_HaltMusic();
	Mix_HaltChannel(-1);
}

void *buffer;
void __fastcall music_start(int nTrack)
{
	BOOL success;

	/// ASSERT: assert((DWORD) nTrack < NUM_MUSIC);
	music_stop();
	if (gbSndInited && gbMusicOn) {
#ifdef _DEBUG
		SFileEnableDirectAccess(FALSE);
#endif
		success = SFileOpenFile(sgszMusicTracks[nTrack], &sgpMusicTrack);
#ifdef _DEBUG
		SFileEnableDirectAccess(TRUE);
#endif
		sound_create_primary_buffer(sgpMusicTrack);
		if (!success) {
			sgpMusicTrack = 0;
		} else {
			int bytestoread = (int)SFileGetFileSize((HANDLE)sgpMusicTrack, 0);
			char *buffer = (char *)DiabloAllocPtr(bytestoread);
			SFileReadFile(sgpMusicTrack, buffer, bytestoread, NULL, 0);

			SDL_RWops *rw = SDL_RWFromMem(buffer, bytestoread);
			Mix_Music *Song = Mix_LoadMUS_RW(rw, 1);
			Mix_PlayMusic(Song, -1);

			sgnMusicTrack = nTrack;
		}
	}
}

void __fastcall sound_disable_music(BOOL disable)
{
	if (disable) {
		music_stop();
	} else if (sgnMusicTrack != 6) {
		music_start(sgnMusicTrack);
	}
}

int __fastcall sound_get_or_set_music_volume(int volume)
{
	if (volume == 1)
		return sglMusicVolume;

	sglMusicVolume = volume;

	if (sgpMusicTrack)
		SFileDdaSetVolume(sgpMusicTrack, volume, 0);

	return sglMusicVolume;
}

int __fastcall sound_get_or_set_sound_volume(int volume)
{
	if (volume == 1)
		return sglSoundVolume;

	sglSoundVolume = volume;

	return sglSoundVolume;
}

/*
SDL_AudioSpec wanted;
static Uint32 audio_len;
static Uint8 *audio_pos;

void fill_audio(void *udata, Uint8 *stream, int len)
{
	//DUMMY();
	//wanted.freq = 44100; //seems not to be used at this time
	//wanted.format = AUDIO_S16LSB; //seems not to be used at this time
	//wanted.channels = 2; // 1 = mono, 2 = stereo, seems not to be used at this time
	//wanted.samples = 1024; // Good low-latency value for callback, seems not to be used at this time
	//wanted.callback = fill_audio; //seems not to be used at this time
	//wanted.userdata = NULL; //seems not to be used at this time

	// Only play if we have data left
	if (audio_len == 0)
		return;

	// Mix as much data as possible
	len = (len > audio_len ? audio_len : len);
	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
	audio_pos += len;
	audio_len -= len;
}
*/
