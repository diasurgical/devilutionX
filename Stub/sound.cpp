#include "../types.h"
#include "stubs.h"
//#include <SDL_image.h>
#include <SDL_mixer.h>
#include <functional>
#include <iostream>
#include <utility>

/*
#ifndef NO_GLOBALS
char gbSndInited;
char gbDupSounds;
UCHAR gbMusicOn;
UCHAR gbSoundOn;
#endif
*/

bool SoundInited;
char gbSndInited;
char gbDupSounds;
unsigned char channels = 8; // defines how many channels, respectively how many *.wav files can be played at the same time
UCHAR gbMusicOn;
UCHAR gbSoundOn;
Mix_Music *gMusic = NULL;
Mix_Chunk *sample;
Mix_Music *music;

char *sgszMusicTracks[6] = {"Music\\DTowne.wav", "Music\\DLvlA.wav", "Music\\DLvlB.wav",
                            "Music\\DLvlC.wav",  "Music\\DLvlD.wav", "Music\\Dintro.wav"};

void __fastcall snd_init(HWND hWnd)
{
	DUMMY();
    /* for some reason function __fastcall snd_init seems to be used twice at this time*/
	printf("\nSND INIT\n");
	// Initialize SDL.
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		printf("ERROR : %s\n\n", SDL_GetError());
	}
	
    /* following function Mix_AllocateChannels allocates the number of channels of simultaneously played sounds.*/
    printf("Opened %i sound channels\n\n", Mix_AllocateChannels(channels));
    /* for example following possible channels:
    1. music is playing
    2. walking at the same time (walking sound)
    3. reading a quest book at the same time
    4. perform a magic spell at the same time
    5. hitting an enemy at the same time
    5. enemy is dieing
    6. enemy is losing an object an so on
    So it is possible that the channels value has to be modified to adjust an amount of simultaneously played sounds in the future.
    At this time 8 channels seem to be sufficient */

	if (Mix_OpenAudio(44100 /*sampling frequency*/, AUDIO_S16LSB /*Signed 16-bit samples, in little-endian byte order*/, 2 /*2 for stereo sound*/, 1024 /*size of each mixed sample*/) < 0) {
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
	}

	gbSndInited = 1;
	gbSoundOn = 1;
	SoundInited = 1;
}

void *sgpMusicTrack;
void *buffer;
// int bytestoread;
int channel = 2;

SDL_AudioSpec wanted;
static Uint8 *audio_chunk;
static Uint32 audio_len;
static Uint8 *audio_pos;

void fill_audio(void *udata, Uint8 *stream, int len)
{
	/* Only play if we have data left */
	if (audio_len == 0)
		return;

	/* Mix as much data as possible */
	len = (len > audio_len ? audio_len : len);
	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
	audio_pos += len;
	audio_len -= len;
}

void __fastcall music_start(int nTrack)
{
	//DUMMY();
	//wanted.freq = 44100; //seems not to be used at this time
	//wanted.format = AUDIO_S16LSB; //seems not to be used at this time
	//wanted.channels = 2; /* 1 = mono, 2 = stereo */ //seems not to be used at this time
	//wanted.samples = 1024; /* Good low-latency value for callback */ //seems not to be used at this time
	//wanted.callback = fill_audio; //seems not to be used at this time
	//wanted.userdata = NULL; //seems not to be used at this time

	gbSoundOn = true;
	gbMusicOn = true;
	int bytestoread;
	int nread;
	void *file;
	int v6;

	if (buffer != NULL) {
		music_stop();
	}

	if (SoundInited) {
		/// I know this needs clean up... I haven't the time to do this.
		SFileOpenFile(sgszMusicTracks[nTrack], &sgpMusicTrack);

		// This is a hack.... I don't like it .
		// If you know this better than I , please help clean it up.


		//Mix_OpenAudio(44100, AUDIO_S16LSB, 1, 1024); //No need to Mix_OpenAudio. Already done in the function __fastcall snd_init

		//Mix_OpenAudio(44100, AUDIO_S16LSB, 2, 1024);

		file = sgpMusicTrack;
		bytestoread = (int)SFileGetFileSize((HANDLE)file, 0);
		buffer = DiabloAllocPtr(bytestoread);
		SFileReadFile(file, (char *)buffer, bytestoread, (unsigned long *)&nread, 0);

		SDL_RWops *rw = SDL_RWFromMem(buffer, bytestoread);

		Mix_Music *Song = Mix_LoadMUS_RW(rw, 1);

		Mix_PlayMusic(Song, -1);
		// Mix_PlayChannel(2, Music, 0);
	}
}

void *SFXbuffer;
int SFXsoundch = 1;
void LoadAndPlaySound(char *FilePath, int lVolume, int lPan)
{
	int bytestoread;
	int nrread;
	void *file;

	SFileOpenFile(FilePath, &SFXbuffer);
	file = SFXbuffer;
	bytestoread = (int)SFileGetFileSize((HANDLE)file, 0);
	SFXbuffer = DiabloAllocPtr(bytestoread);
	SFileReadFile(file, (char *)SFXbuffer, bytestoread, (unsigned long *)&nrread, 0);

	SDL_RWops *rw = SDL_RWFromMem(SFXbuffer, bytestoread);
	Mix_Chunk *SoundFX = Mix_LoadWAV_RW(rw, 1);

	Mix_PlayChannel(-1, SoundFX, 0);
	// Mix_FreeChunk(SoundFX);
}

void __cdecl music_stop()
{
	DUMMY();
	Mix_HaltMusic();
}

BOOL __fastcall snd_playing(TSnd *pSnd)
{

	printf("snd_playing \n");
	// DUMMY();
	// UNIMPLEMENTED();
}


//
void __fastcall snd_play_snd(TSnd *pSnd, int lVolume, int lPan)
{


	Mix_PlayChannel(-1, (Mix_Chunk*)pSnd, 0);

}

void __fastcall snd_stop_snd(TSnd *pSnd)
{
	DUMMY();
	Mix_HaltMusic();
}





void *MSFXBuffer;
TSnd *__fastcall sound_file_load(char *path)
{

	int bytestoread;
	int nrread;
	void *file;


//This opens the file and reads it, makes Mix_chunk pointer to it.
//Once this is done the pointer is stored TSnd Struct

	SFileOpenFile(path, &file);
	 bytestoread = (int)SFileGetFileSize((HANDLE)file, 0);	 
	 MSFXBuffer = DiabloAllocPtr(bytestoread);
	 SFileReadFile(file, (char *)MSFXBuffer, bytestoread, (unsigned long *)&nrread, 0);
	 SDL_RWops *rw = SDL_RWFromMem(MSFXBuffer, bytestoread);
	 Mix_Chunk *SoundFX = Mix_LoadWAV_RW(rw, 1);

	



	return (TSnd*)SoundFX;

//	printf("Sound_File_Load %s\n", path);
	// UNIMPLEMENTED();
}

void __fastcall sound_file_cleanup(TSnd *sound_file)
{
	UNIMPLEMENTED();
}

int __fastcall sound_get_or_set_sound_volume(int volume)
{
	DUMMY_PRINT("volume: %d", volume);
	return volume;
}

int __fastcall sound_get_or_set_music_volume(int volume)
{
	DUMMY_PRINT("volume: %d", volume);
	return volume;
}

void __fastcall snd_update(BOOL bStopAll)
{

	// DUMMY_PRINT("stopall: %d", bStopAll);
}
