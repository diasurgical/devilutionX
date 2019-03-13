/*
  PLAYWAVE:  A test application for the SDL mixer library.
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


/*
 * rcg06132001 various mixer tests. Define the ones you want.
 */
/*#define TEST_MIX_DECODERS*/
/*#define TEST_MIX_VERSIONS*/
/*#define TEST_MIX_CHANNELFINISHED*/
/*#define TEST_MIX_PANNING*/
/*#define TEST_MIX_DISTANCE*/
/*#define TEST_MIX_POSITION*/


#if (defined TEST_MIX_POSITION)

#if (defined TEST_MIX_PANNING)
#error TEST_MIX_POSITION interferes with TEST_MIX_PANNING.
#endif

#if (defined TEST_MIX_DISTANCE)
#error TEST_MIX_POSITION interferes with TEST_MIX_DISTANCE.
#endif

#endif


/* rcg06192001 for debugging purposes. */
static void output_test_warnings(void)
{
#if (defined TEST_MIX_CHANNELFINISHED)
    SDL_Log("Warning: TEST_MIX_CHANNELFINISHED is enabled in this binary...\n");
#endif
#if (defined TEST_MIX_PANNING)
    SDL_Log("Warning: TEST_MIX_PANNING is enabled in this binary...\n");
#endif
#if (defined TEST_MIX_VERSIONS)
    SDL_Log("Warning: TEST_MIX_VERSIONS is enabled in this binary...\n");
#endif
#if (defined TEST_MIX_DISTANCE)
    SDL_Log("Warning: TEST_MIX_DISTANCE is enabled in this binary...\n");
#endif
#if (defined TEST_MIX_POSITION)
    SDL_Log("Warning: TEST_MIX_POSITION is enabled in this binary...\n");
#endif
}


static int audio_open = 0;
static Mix_Chunk *wave = NULL;

/* rcg06042009 Report available decoders. */
#if (defined TEST_MIX_DECODERS)
static void report_decoders(void)
{
    int i, total;

    SDL_Log("Supported decoders...\n");
    total = Mix_GetNumChunkDecoders();
    for (i = 0; i < total; i++) {
        SDL_Log(" - chunk decoder: %s\n", Mix_GetChunkDecoder(i));
    }

    total = Mix_GetNumMusicDecoders();
    for (i = 0; i < total; i++) {
        SDL_Log(" - music decoder: %s\n", Mix_GetMusicDecoder(i));
    }
}
#endif

/* rcg06192001 Check new Mixer version API. */
#if (defined TEST_MIX_VERSIONS)
static void output_versions(const char *libname, const SDL_version *compiled,
                            const SDL_version *linked)
{
    SDL_Log("This program was compiled against %s %d.%d.%d,\n"
            " and is dynamically linked to %d.%d.%d.\n", libname,
            compiled->major, compiled->minor, compiled->patch,
            linked->major, linked->minor, linked->patch);
}

static void test_versions(void)
{
    SDL_version compiled;
    const SDL_version *linked;

    SDL_VERSION(&compiled);
    linked = SDL_Linked_Version();
    output_versions("SDL", &compiled, linked);

    SDL_MIXER_VERSION(&compiled);
    linked = Mix_Linked_Version();
    output_versions("SDL_mixer", &compiled, linked);
}
#endif


#ifdef TEST_MIX_CHANNELFINISHED  /* rcg06072001 */
static volatile int channel_is_done = 0;
static void SDLCALL channel_complete_callback (int chan)
{
    Mix_Chunk *done_chunk = Mix_GetChunk(chan);
    SDL_Log("We were just alerted that Mixer channel #%d is done.\n", chan);
    SDL_Log("Channel's chunk pointer is (%p).\n", done_chunk);
    SDL_Log(" Which %s correct.\n", (wave == done_chunk) ? "is" : "is NOT");
    channel_is_done = 1;
}
#endif


/* rcg06192001 abstract this out for testing purposes. */
static int still_playing(void)
{
#ifdef TEST_MIX_CHANNELFINISHED
    return(!channel_is_done);
#else
    return(Mix_Playing(0));
#endif
}


#if (defined TEST_MIX_PANNING)
static void do_panning_update(void)
{
    static Uint8 leftvol = 128;
    static Uint8 rightvol = 128;
    static Uint8 leftincr = -1;
    static Uint8 rightincr = 1;
    static int panningok = 1;
    static Uint32 next_panning_update = 0;

    if ((panningok) && (SDL_GetTicks() >= next_panning_update)) {
        panningok = Mix_SetPanning(0, leftvol, rightvol);
        if (!panningok) {
            SDL_Log("Mix_SetPanning(0, %d, %d) failed!\n",
                    (int) leftvol, (int) rightvol);
            SDL_Log("Reason: [%s].\n", Mix_GetError());
        }

        if ((leftvol == 255) || (leftvol == 0)) {
            if (leftvol == 255)
                SDL_Log("All the way in the left speaker.\n");
                leftincr *= -1;
        }

        if ((rightvol == 255) || (rightvol == 0)) {
            if (rightvol == 255)
                SDL_Log("All the way in the right speaker.\n");
            rightincr *= -1;
        }

        leftvol += leftincr;
        rightvol += rightincr;
        next_panning_update = SDL_GetTicks() + 10;
    }
}
#endif


#if (defined TEST_MIX_DISTANCE)
static void do_distance_update(void)
{
    static Uint8 distance = 1;
    static Uint8 distincr = 1;
    static int distanceok = 1;
    static Uint32 next_distance_update = 0;

    if ((distanceok) && (SDL_GetTicks() >= next_distance_update)) {
        distanceok = Mix_SetDistance(0, distance);
        if (!distanceok) {
            SDL_Log("Mix_SetDistance(0, %d) failed!\n", (int) distance);
            SDL_Log("Reason: [%s].\n", Mix_GetError());
        }

        if (distance == 0) {
            SDL_Log("Distance at nearest point.\n");
            distincr *= -1;
        }
        else if (distance == 255) {
            SDL_Log("Distance at furthest point.\n");
            distincr *= -1;
        }

        distance += distincr;
        next_distance_update = SDL_GetTicks() + 15;
    }
}
#endif


#if (defined TEST_MIX_POSITION)
static void do_position_update(void)
{
    static Sint16 distance = 1;
    static Sint8 distincr = 1;
    static Uint16 angle = 0;
    static Sint8 angleincr = 1;
    static int positionok = 1;
    static Uint32 next_position_update = 0;

    if ((positionok) && (SDL_GetTicks() >= next_position_update)) {
        positionok = Mix_SetPosition(0, angle, distance);
        if (!positionok) {
            SDL_Log("Mix_SetPosition(0, %d, %d) failed!\n",
                    (int) angle, (int) distance);
            SDL_Log("Reason: [%s].\n", Mix_GetError());
        }

        if (angle == 0) {
            SDL_Log("Due north; now rotating clockwise...\n");
            angleincr = 1;
        }

        else if (angle == 360) {
            SDL_Log("Due north; now rotating counter-clockwise...\n");
            angleincr = -1;
        }

        distance += distincr;

        if (distance < 0) {
            distance = 0;
            distincr = 3;
            SDL_Log("Distance is very, very near. Stepping away by threes...\n");
        } else if (distance > 255) {
            distance = 255;
            distincr = -3;
            SDL_Log("Distance is very, very far. Stepping towards by threes...\n");
        }

        angle += angleincr;
        next_position_update = SDL_GetTicks() + 30;
    }
}
#endif


static void CleanUp(int exitcode)
{
    if (wave) {
        Mix_FreeChunk(wave);
        wave = NULL;
    }
    if (audio_open) {
        Mix_CloseAudio();
        audio_open = 0;
    }
    SDL_Quit();

    exit(exitcode);
}


static void Usage(char *argv0)
{
    SDL_Log("Usage: %s [-8] [-f32] [-r rate] [-c channels] [-f] [-F] [-l] [-m] <wavefile>\n", argv0);
}


/*
 * rcg06182001 This is sick, but cool.
 *
 *  Actually, it's meant to be an example of how to manipulate a voice
 *  without having to use the mixer effects API. This is more processing
 *  up front, but no extra during the mixing process. Also, in a case like
 *  this, when you need to touch the whole sample at once, it's the only
 *  option you've got. And, with the effects API, you are altering a copy of
 *  the original sample for each playback, and thus, your changes aren't
 *  permanent; here, you've got a reversed sample, and that's that until
 *  you either reverse it again, or reload it.
 */
static void flip_sample(Mix_Chunk *wave)
{
    Uint16 format;
    int channels, i, incr;
    Uint8 *start = wave->abuf;
    Uint8 *end = wave->abuf + wave->alen;

    Mix_QuerySpec(NULL, &format, &channels);
    incr = (format & 0xFF) * channels;

    end -= incr;

    switch (incr) {
        case 8:
            for (i = wave->alen / 2; i >= 0; i -= 1) {
                Uint8 tmp = *start;
                *start = *end;
                *end = tmp;
                start++;
                end--;
            }
            break;

        case 16:
            for (i = wave->alen / 2; i >= 0; i -= 2) {
                Uint16 tmp = *start;
                *((Uint16 *) start) = *((Uint16 *) end);
                *((Uint16 *) end) = tmp;
                start += 2;
                end -= 2;
            }
            break;

        case 32:
            for (i = wave->alen / 2; i >= 0; i -= 4) {
                Uint32 tmp = *start;
                *((Uint32 *) start) = *((Uint32 *) end);
                *((Uint32 *) end) = tmp;
                start += 4;
                end -= 4;
            }
            break;

        default:
            SDL_Log("Unhandled format in sample flipping.\n");
            return;
    }
}


int main(int argc, char *argv[])
{
    int audio_rate;
    Uint16 audio_format;
    int audio_channels;
    int loops = 0;
    int i;
    int reverse_stereo = 0;
    int reverse_sample = 0;

#ifdef HAVE_SETBUF
    setbuf(stdout, NULL);    /* rcg06132001 for debugging purposes. */
    setbuf(stderr, NULL);    /* rcg06192001 for debugging purposes, too. */
#endif
    output_test_warnings();

    /* Initialize variables */
    audio_rate = MIX_DEFAULT_FREQUENCY;
    audio_format = MIX_DEFAULT_FORMAT;
    audio_channels = 2;

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
        if (strcmp(argv[i], "-l") == 0) {
            loops = -1;
        } else
        if (strcmp(argv[i], "-8") == 0) {
            audio_format = AUDIO_U8;
        } else
        if (strcmp(argv[i], "-f32") == 0) {
            audio_format = AUDIO_F32;
        } else
        if (strcmp(argv[i], "-f") == 0) { /* rcg06122001 flip stereo */
            reverse_stereo = 1;
        } else
        if (strcmp(argv[i], "-F") == 0) { /* rcg06172001 flip sample */
            reverse_sample = 1;
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
    signal(SIGINT, CleanUp);
    signal(SIGTERM, CleanUp);
#endif

    /* Open the audio device */
    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, 4096) < 0) {
        SDL_Log("Couldn't open audio: %s\n", SDL_GetError());
        CleanUp(2);
    } else {
        Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
        SDL_Log("Opened audio at %d Hz %d bit%s %s", audio_rate,
            (audio_format&0xFF),
            (SDL_AUDIO_ISFLOAT(audio_format) ? " (float)" : ""),
            (audio_channels > 2) ? "surround" :
            (audio_channels > 1) ? "stereo" : "mono");
        if (loops) {
          SDL_Log(" (looping)\n");
        } else {
          putchar('\n');
        }
    }
    audio_open = 1;

#if (defined TEST_MIX_VERSIONS)
    test_versions();
#endif

#if (defined TEST_MIX_DECODERS)
    report_decoders();
#endif

    /* Load the requested wave file */
    wave = Mix_LoadWAV(argv[i]);
    if (wave == NULL) {
        SDL_Log("Couldn't load %s: %s\n",
                        argv[i], SDL_GetError());
        CleanUp(2);
    }

    if (reverse_sample) {
        flip_sample(wave);
    }

#ifdef TEST_MIX_CHANNELFINISHED  /* rcg06072001 */
    Mix_ChannelFinished(channel_complete_callback);
#endif

    if ((!Mix_SetReverseStereo(MIX_CHANNEL_POST, reverse_stereo)) &&
         (reverse_stereo))
    {
        SDL_Log("Failed to set up reverse stereo effect!\n");
        SDL_Log("Reason: [%s].\n", Mix_GetError());
    }

    /* Play and then exit */
    Mix_PlayChannel(0, wave, loops);

    while (still_playing()) {

#if (defined TEST_MIX_PANNING)  /* rcg06132001 */
        do_panning_update();
#endif

#if (defined TEST_MIX_DISTANCE) /* rcg06192001 */
        do_distance_update();
#endif

#if (defined TEST_MIX_POSITION) /* rcg06202001 */
        do_position_update();
#endif

        SDL_Delay(1);

    } /* while still_playing() loop... */

    CleanUp(0);

    /* Not reached, but fixes compiler warnings */
    return 0;
}

/* end of playwave.c ... */

/* vi: set ts=4 sw=4 expandtab: */
