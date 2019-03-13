/*
  PLAYMUS:  A test application for the SDL mixer library.
  Copyright (C) 1997-2018 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/* Quiet windows compiler warnings */
#define _CRT_SECURE_NO_WARNINGS

/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef unix
#include <unistd.h>
#endif

#include "SDL.h"
#include "SDL_mixer.h"

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif


static int audio_open = 0;
static Mix_Music *music = NULL;
static int next_track = 0;

void CleanUp(int exitcode)
{
    if(Mix_PlayingMusic()) {
        Mix_FadeOutMusic(1500);
        SDL_Delay(1500);
    }
    if (music) {
        Mix_FreeMusic(music);
        music = NULL;
    }
    if (audio_open) {
        Mix_CloseAudio();
        audio_open = 0;
    }
    SDL_Quit();
    exit(exitcode);
}

void Usage(char *argv0)
{
    SDL_Log("Usage: %s [-i] [-l] [-8] [-f32] [-r rate] [-c channels] [-b buffers] [-v N] [-rwops] <musicfile>\n", argv0);
}

void Menu(void)
{
    char buf[10];

    printf("Available commands: (p)ause (r)esume (h)alt volume(v#) > ");
    fflush(stdin);
    if (scanf("%s",buf) == 1) {
        switch(buf[0]){
        case 'p': case 'P':
            Mix_PauseMusic();
            break;
        case 'r': case 'R':
            Mix_ResumeMusic();
            break;
        case 'h': case 'H':
            Mix_HaltMusic();
            break;
        case 'v': case 'V':
            Mix_VolumeMusic(atoi(buf+1));
            break;
        }
    }
    printf("Music playing: %s Paused: %s\n", Mix_PlayingMusic() ? "yes" : "no",
           Mix_PausedMusic() ? "yes" : "no");
}

#ifdef HAVE_SIGNAL_H

void IntHandler(int sig)
{
    switch (sig) {
            case SIGINT:
            next_track++;
            break;
    }
}

#endif

int main(int argc, char *argv[])
{
    int audio_rate;
    Uint16 audio_format;
    int audio_channels;
    int audio_buffers;
    int audio_volume = MIX_MAX_VOLUME;
    int looping = 0;
    int interactive = 0;
    int rwops = 0;
    int i;

    /* Initialize variables */
    audio_rate = 22050;
    audio_format = AUDIO_S16;
    audio_channels = 2;
    audio_buffers = 4096;

    /* Check command line usage */
    for (i=1; argv[i] && (*argv[i] == '-'); ++i) {
        if ((strcmp(argv[i], "-r") == 0) && argv[i+1]) {
            ++i;
            audio_rate = atoi(argv[i]);
        } else
        if (strcmp(argv[i], "-m") == 0) {
            audio_channels = 1;
        } else
        if ((strcmp(argv[i], "-c") == 0) && argv[i+1]) {
            ++i;
            audio_channels = atoi(argv[i]);
        } else
        if ((strcmp(argv[i], "-b") == 0) && argv[i+1]) {
            ++i;
            audio_buffers = atoi(argv[i]);
        } else
        if ((strcmp(argv[i], "-v") == 0) && argv[i+1]) {
            ++i;
            audio_volume = atoi(argv[i]);
        } else
        if (strcmp(argv[i], "-l") == 0) {
            looping = -1;
        } else
        if (strcmp(argv[i], "-i") == 0) {
            interactive = 1;
        } else
        if (strcmp(argv[i], "-8") == 0) {
            audio_format = AUDIO_U8;
        } else
        if (strcmp(argv[i], "-f32") == 0) {
            audio_format = AUDIO_F32;
        } else
        if (strcmp(argv[i], "-rwops") == 0) {
            rwops = 1;
        } else {
            Usage(argv[0]);
            return(1);
        }
    }
    if (! argv[i]) {
        Usage(argv[0]);
        return(1);
    }

    /* Initialize the SDL library */
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        SDL_Log("Couldn't initialize SDL: %s\n",SDL_GetError());
        return(255);
    }

#ifdef HAVE_SIGNAL_H
    signal(SIGINT, IntHandler);
    signal(SIGTERM, CleanUp);
#endif

    /* Open the audio device */
    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) < 0) {
        SDL_Log("Couldn't open audio: %s\n", SDL_GetError());
        return(2);
    } else {
        Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
        SDL_Log("Opened audio at %d Hz %d bit%s %s %d bytes audio buffer\n", audio_rate,
            (audio_format&0xFF),
            (SDL_AUDIO_ISFLOAT(audio_format) ? " (float)" : ""),
            (audio_channels > 2) ? "surround" : (audio_channels > 1) ? "stereo" : "mono",
            audio_buffers);
    }
    audio_open = 1;

    /* Set the music volume */
    Mix_VolumeMusic(audio_volume);

    /* Set the external music player, if any */
    Mix_SetMusicCMD(SDL_getenv("MUSIC_CMD"));

    while (argv[i]) {
        next_track = 0;

        /* Load the requested music file */
        if (rwops) {
            music = Mix_LoadMUS_RW(SDL_RWFromFile(argv[i], "rb"), SDL_TRUE);
        } else {
            music = Mix_LoadMUS(argv[i]);
        }
        if (music == NULL) {
            SDL_Log("Couldn't load %s: %s\n",
                argv[i], SDL_GetError());
            CleanUp(2);
        }

        /* Play and then exit */
        SDL_Log("Playing %s\n", argv[i]);
        Mix_FadeInMusic(music,looping,2000);
        while (!next_track && (Mix_PlayingMusic() || Mix_PausedMusic())) {
            if(interactive)
                Menu();
            else
                SDL_Delay(100);
        }
        Mix_FreeMusic(music);
        music = NULL;

        /* If the user presses Ctrl-C more than once, exit. */
        SDL_Delay(500);
        if (next_track > 1) break;

        i++;
    }
    CleanUp(0);

    /* Not reached, but fixes compiler warnings */
    return 0;
}

/* vi: set ts=4 sw=4 expandtab: */
