#include "../types.h"
#include "stubs.h"
#include <SDL_image.h>
#include <SDL_mixer.h>

bool SoundInited;
char gbSndInited;
char gbDupSounds;
UCHAR gbMusicOn;
UCHAR gbSoundOn;
Mix_Music *gMusic = NULL;
Mix_Chunk *sample;
Mix_Music *music;

char *sgszMusicTracks[6] = {"Music\\DTowne.wav", "Music\\DLvlA.wav", "Music\\DLvlB.wav",
                            "Music\\DLvlC.wav",  "Music\\DLvlD.wav", "Music\\Dintro.wav"};

bool IsMusicPlaying = 0;

void __fastcall snd_init()
{
	printf("SND INIT\n\n");
	// Initialize SDL.
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {

		printf("ERROR : %s\n\n", SDL_GetError());
	}
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
	}

	gbSndInited = 1;
	gbSoundOn = 1;
	SoundInited = 1;
}

void *sgpMusicTrack;
void *buffer;
int bytestoread;
int channel = 2;



SDL_AudioSpec wanted;
    static Uint8 *audio_chunk;
    static Uint32 audio_len;
    static Uint8 *audio_pos;


void fill_audio(void *udata, Uint8 *stream, int len)
    {
        /* Only play if we have data left */
        if ( audio_len == 0 )
            return;

        /* Mix as much data as possible */
        len = ( len > audio_len ? audio_len : len );
        SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
        audio_pos += len;
        audio_len -= len;
    }


void __fastcall music_start(int nTrack)
{
	wanted.freq = 22050;
    wanted.format = AUDIO_S8;
    wanted.channels = 5;    /* 1 = mono, 2 = stereo */
    wanted.samples = 1024;  /* Good low-latency value for callback */
    wanted.callback = fill_audio;
    wanted.userdata = NULL;


	gbSoundOn = true;
	gbMusicOn = true;
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

		

		//SDL_OpenAudio(&wanted, NULL);

		 if (nTrack == 0) {
		   Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 16);
			
		 	channel = 2;
		 }
		 if (nTrack == 1) {
		 	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 1, 16);
		 	channel = 1;
		 }

		file = sgpMusicTrack;
		bytestoread = (int)SFileGetFileSize((HANDLE)file, 0);
		buffer = DiabloAllocPtr(bytestoread);
		SFileReadFile(file, (char *)buffer, bytestoread, (unsigned long *)&nread, 0);
		Mix_Chunk *Music = Mix_QuickLoad_WAV((Uint8 *)buffer);



		Mix_PlayChannel(channel, Music, 0);
	}
}

void IsPlayMusic()
{
	// if (Mix_PlayingMusic() == 0) {
	// 	// Play the music
	// 	Mix_PlayMusic(gMusic, -1);
	// }
}

void __cdecl music_stop()
{

	// memset(buffer, 0, sizeof(bytestoread));
	// Mix_HaltMusic();
	// DUMMY();
}

bool __fastcall snd_playing(TSnd *pSnd)
{
	printf("snd_playing!!!!\n");
	// UNIMPLEMENTED();
}

void *SFXbuffer;
int SFXsoundch = 1;
void LoadAndPlaySound(char *FilePath, int lVolume, int lPan)
{
	int nrread;
	void *file;

	SFileOpenFile(FilePath, &SFXbuffer);
	//Mix_OpenAudio(20000, AUDIO_S8, 3, 4096);
	file = SFXbuffer;
	bytestoread = (int)SFileGetFileSize((HANDLE)file, 0);
	SFXbuffer = DiabloAllocPtr(bytestoread);
	SFileReadFile(file, (char *)SFXbuffer, bytestoread, (unsigned long *)&nrread, 0);
	Mix_Chunk *SoundFX = Mix_QuickLoad_WAV((Uint8 *)SFXbuffer);

	Mix_PlayChannel(3, SoundFX, 0);
	//Mix_FreeChunk(SoundFX);
}

void __fastcall snd_play_snd(TSnd *pSnd, int lVolume, int lPan)
{

	void *file;

	file = pSnd;

	// bytestoread = (int)SFileGetFileSize((HANDLE)file, 0);
	// sbuffer = DiabloAllocPtr(bytestoread);
	// SFileReadFile(file, (char *)sbuffer, bytestoread, (unsigned long *)&nrread, 0);
	// Mix_Chunk *sound = Mix_QuickLoad_WAV((Uint8 *)sbuffer);
	// Mix_PlayChannel(channel, sound, 0);

	printf("snd_play_snd!!!!\n");

	// UNIMPLEMENTED();
}

void __fastcall snd_stop_snd(TSnd *pSnd)
{
	DUMMY();
}
// effects_play_sound
// void effects_play_sound()

// added
void __fastcall SDL_PlayEffect(int i, int mode)
{

	printf("113Debug Mode delete me\n\n");
	UNIMPLEMENTED();
}

// void *ptr[900];
TSnd *__fastcall sound_file_load(char *path)
{
	printf("File Is Being Loaded %s\n", path);

	SFileOpenFile(path, &SFXbuffer);

	// UNIMPLEMENTED();
}

void __fastcall sound_file_cleanup(TSnd *sound_file)
{
	printf("125Debug Mode delete me\n\n");
	UNIMPLEMENTED();
}

int __fastcall sound_get_or_set_sound_volume(int volume)
{
	printf("131Debug Mode delete me\n\n");
	DUMMY_PRINT("volume: %d", volume);
	return volume;
}

int __fastcall sound_get_or_set_music_volume(int volume)
{
	printf("138Debug Mode delete me\n\n");
	Mix_Volume(1, MIX_MAX_VOLUME / 2);

	DUMMY_PRINT("volume: %d", volume);
	return volume;
}

void __fastcall snd_update(bool bStopAll)
{
	// printf("147Debug Mode delete me\n\n");
	// DUMMY_PRINT("stopall: %d", bStopAll);
}
