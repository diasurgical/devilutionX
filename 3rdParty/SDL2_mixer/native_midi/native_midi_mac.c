/*
  native_midi_mac:  Native Midi support on MacOS for the SDL_mixer library
  Copyright (C) 2001  Max Horn <max@quendi.de>

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
#include "SDL_config.h"
#include "SDL_endian.h"
#include "../mixer.h"

#if __MACOS__ /*|| __MACOSX__ */

#include "native_midi.h"
#include "native_midi_common.h"

#if __MACOSX__
#include <QuickTime/QuickTimeMusic.h>
#else
#include <QuickTimeMusic.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <string.h>


/* Native Midi song */
struct _NativeMidiSong
{
    Uint32      *tuneSequence;
    Uint32      *tuneHeader;
};

enum
{
    /* number of (32-bit) long words in a note request event */
    kNoteRequestEventLength = ((sizeof(NoteRequest)/sizeof(long)) + 2),

    /* number of (32-bit) long words in a marker event */
    kMarkerEventLength  = 1,

    /* number of (32-bit) long words in a general event, minus its data */
    kGeneralEventLength = 2
};

#define ERROR_BUF_SIZE          256
#define BUFFER_INCREMENT        5000

#define REST_IF_NECESSARY() do {\
            int timeDiff = eventPos->time - lastEventTime;  \
            if(timeDiff)    \
            {   \
                timeDiff = (int)(timeDiff*tick);    \
                qtma_StuffRestEvent(*tunePos, timeDiff);    \
                tunePos++;  \
                lastEventTime = eventPos->time; \
            }   \
        } while(0)


static Uint32 *BuildTuneSequence(MIDIEvent *evntlist, int ppqn, int part_poly_max[32], int part_to_inst[32], int *numParts);
static Uint32 *BuildTuneHeader(int part_poly_max[32], int part_to_inst[32], int numParts);

/* The global TunePlayer instance */
static TunePlayer   gTunePlayer = NULL;
static int          gInstaceCount = 0;
static Uint32       *gCurrentTuneSequence = NULL;
static char         gErrorBuffer[ERROR_BUF_SIZE] = "";


/* Check whether QuickTime is available */
int native_midi_detect(void)
{
    /* TODO */
    return 1;
}

NativeMidiSong *native_midi_loadsong_RW(SDL_RWops *src, int freesrc)
{
    NativeMidiSong  *song = NULL;
    MIDIEvent       *evntlist = NULL;
    int             part_to_inst[32];
    int             part_poly_max[32];
    int             numParts = 0;
    Uint16          ppqn;

    /* Init the arrays */
    memset(part_poly_max,0,sizeof(part_poly_max));
    memset(part_to_inst,-1,sizeof(part_to_inst));

    /* Attempt to load the midi file */
    evntlist = CreateMIDIEventList(src, &ppqn);
    if (!evntlist)
        goto bail;

    /* Allocate memory for the song struct */
    song = malloc(sizeof(NativeMidiSong));
    if (!song)
        goto bail;

    /* Build a tune sequence from the event list */
    song->tuneSequence = BuildTuneSequence(evntlist, ppqn, part_poly_max, part_to_inst, &numParts);
    if(!song->tuneSequence)
        goto bail;

    /* Now build a tune header from the data we collect above, create
       all parts as needed and assign them the correct instrument.
    */
    song->tuneHeader = BuildTuneHeader(part_poly_max, part_to_inst, numParts);
    if(!song->tuneHeader)
        goto bail;

    /* Increment the instance count */
    gInstaceCount++;
    if (gTunePlayer == NULL)
        gTunePlayer = OpenDefaultComponent(kTunePlayerComponentType, 0);

    /* Finally, free the event list */
    FreeMIDIEventList(evntlist);

    if (freerw) {
        SDL_RWclose(rw);
    }
    return song;

bail:
    if (evntlist)
        FreeMIDIEventList(evntlist);

    if (song)
    {
        if(song->tuneSequence)
            free(song->tuneSequence);

        if(song->tuneHeader)
            DisposePtr((Ptr)song->tuneHeader);

        free(song);
    }
    return NULL;
}

void native_midi_freesong(NativeMidiSong *song)
{
    if(!song || !song->tuneSequence)
        return;

    /* If this is the currently playing song, stop it now */
    if (song->tuneSequence == gCurrentTuneSequence)
        native_midi_stop();

    /* Finally, free the data storage */
    free(song->tuneSequence);
    DisposePtr((Ptr)song->tuneHeader);
    free(song);

    /* Increment the instance count */
    gInstaceCount--;
    if ((gTunePlayer != NULL) && (gInstaceCount == 0))
    {
        CloseComponent(gTunePlayer);
        gTunePlayer = NULL;
    }
}

void native_midi_start(NativeMidiSong *song, int loops)
{
    UInt32      queueFlags = 0;
    ComponentResult tpError;

    assert (gTunePlayer != NULL);

    /* FIXME: is this code even used anymore? */
    assert (loops == 0);

    SDL_PauseAudio(1);
    Mix_UnlockAudio();

    /* First, stop the currently playing music */
    native_midi_stop();

    /* Set up the queue flags */
    queueFlags = kTuneStartNow;

    /* Set the time scale (units per second), we want milliseconds */
    tpError = TuneSetTimeScale(gTunePlayer, 1000);
    if (tpError != noErr)
    {
        strncpy (gErrorBuffer, "MIDI error during TuneSetTimeScale", ERROR_BUF_SIZE);
        goto done;
    }

    /* Set the header, to tell what instruments are used */
    tpError = TuneSetHeader(gTunePlayer, (UInt32 *)song->tuneHeader);
    if (tpError != noErr)
    {
        strncpy (gErrorBuffer, "MIDI error during TuneSetHeader", ERROR_BUF_SIZE);
        goto done;
    }

    /* Have it allocate whatever resources are needed */
    tpError = TunePreroll(gTunePlayer);
    if (tpError != noErr)
    {
        strncpy (gErrorBuffer, "MIDI error during TunePreroll", ERROR_BUF_SIZE);
        goto done;
    }

    /* We want to play at normal volume */
    tpError = TuneSetVolume(gTunePlayer, 0x00010000);
    if (tpError != noErr)
    {
        strncpy (gErrorBuffer, "MIDI error during TuneSetVolume", ERROR_BUF_SIZE);
        goto done;
    }

    /* Finally, start playing the full song */
    gCurrentTuneSequence = song->tuneSequence;
    tpError = TuneQueue(gTunePlayer, (UInt32 *)song->tuneSequence, 0x00010000, 0, 0xFFFFFFFF, queueFlags, NULL, 0);
    if (tpError != noErr)
    {
        strncpy (gErrorBuffer, "MIDI error during TuneQueue", ERROR_BUF_SIZE);
        goto done;
    }

done:
    Mix_LockAudio();
    SDL_PauseAudio(0);
}

void native_midi_pause(void)
{
}

void native_midi_resume(void)
{
}

void native_midi_stop(void)
{
    if (gTunePlayer == NULL)
        return;

    /* Stop music */
    TuneStop(gTunePlayer, 0);

    /* Deallocate all instruments */
    TuneUnroll(gTunePlayer);
}

int native_midi_active(void)
{
    if (gTunePlayer != NULL)
    {
        TuneStatus  ts;

        TuneGetStatus(gTunePlayer,&ts);
        return ts.queueTime != 0;
    }
    else
        return 0;
}

void native_midi_setvolume(int volume)
{
    if (gTunePlayer == NULL)
        return;

    /* QTMA olume may range from 0.0 to 1.0 (in 16.16 fixed point encoding) */
    TuneSetVolume(gTunePlayer, (0x00010000 * volume)/SDL_MIX_MAXVOLUME);
}

const char *native_midi_error(void)
{
    return gErrorBuffer;
}

Uint32 *BuildTuneSequence(MIDIEvent *evntlist, int ppqn, int part_poly_max[32], int part_to_inst[32], int *numParts)
{
    int         part_poly[32];
    int         channel_to_part[16];

    int         channel_pan[16];
    int         channel_vol[16];
    int         channel_pitch_bend[16];

    int         lastEventTime = 0;
    int         tempo = 500000;
    double      Ippqn = 1.0 / (1000*ppqn);
    double      tick = tempo * Ippqn;
    MIDIEvent   *eventPos = evntlist;
    MIDIEvent   *noteOffPos;
    Uint32      *tunePos, *endPos;
    Uint32      *tuneSequence;
    size_t      tuneSize;

    /* allocate space for the tune header */
    tuneSize = 5000;
    tuneSequence = (Uint32 *)malloc(tuneSize * sizeof(Uint32));
    if (tuneSequence == NULL)
        return NULL;

    /* Set starting position in our tune memory */
    tunePos = tuneSequence;
    endPos = tuneSequence + tuneSize;

    /* Initialise the arrays */
    memset(part_poly,0,sizeof(part_poly));

    memset(channel_to_part,-1,sizeof(channel_to_part));
    memset(channel_pan,-1,sizeof(channel_pan));
    memset(channel_vol,-1,sizeof(channel_vol));
    memset(channel_pitch_bend,-1,sizeof(channel_pitch_bend));

    *numParts = 0;

    /*
     * Now the major work - iterate over all GM events,
     * and turn them into QuickTime Music format.
     * At the same time, calculate the max. polyphony for each part,
     * and also the part->instrument mapping.
     */
    while(eventPos)
    {
        int status = (eventPos->status&0xF0)>>4;
        int channel = eventPos->status&0x0F;
        int part = channel_to_part[channel];
        int velocity, pitch;
        int value, controller;
        int bend;
        int newInst;

        /* Check if we are running low on space... */
        if((tunePos+16) > endPos)
        {
            /* Resize our data storage. */
            Uint32      *oldTuneSequence = tuneSequence;

            tuneSize += BUFFER_INCREMENT;
            tuneSequence = (Uint32 *)realloc(tuneSequence, tuneSize * sizeof(Uint32));
            if(oldTuneSequence != tuneSequence)
                tunePos += tuneSequence - oldTuneSequence;
            endPos = tuneSequence + tuneSize;
        }

        switch (status)
        {
        case MIDI_STATUS_NOTE_OFF:
            assert(part>=0 && part<=31);

            /* Keep track of the polyphony of the current part */
            part_poly[part]--;
            break;
        case MIDI_STATUS_NOTE_ON:
            if (part < 0)
            {
                /* If no part is specified yet, we default to the first instrument, which
                   is piano (or the first drum kit if we are on the drum channel)
                */
                int newInst;

                if (channel == 9)
                    newInst = kFirstDrumkit + 1;        /* the first drum kit is the "no drum" kit! */
                else
                    newInst = kFirstGMInstrument;
                part = channel_to_part[channel] = *numParts;
                part_to_inst[(*numParts)++] = newInst;
            }
            /* TODO - add support for more than 32 parts using eXtended QTMA events */
            assert(part<=31);

            /* Decode pitch & velocity */
            pitch = eventPos->data[0];
            velocity = eventPos->data[1];

            if (velocity == 0)
            {
                /* was a NOTE OFF in disguise, so we decrement the polyphony */
                part_poly[part]--;
            }
            else
            {
                /* Keep track of the polyphony of the current part */
                int foo = ++part_poly[part];
                if (part_poly_max[part] < foo)
                    part_poly_max[part] = foo;

                /* Now scan forward to find the matching NOTE OFF event */
                for(noteOffPos = eventPos; noteOffPos; noteOffPos = noteOffPos->next)
                {
                    if ((noteOffPos->status&0xF0)>>4 == MIDI_STATUS_NOTE_OFF
                        && channel == (eventPos->status&0x0F)
                        && pitch == noteOffPos->data[0])
                        break;
                    /* NOTE ON with velocity == 0 is the same as a NOTE OFF */
                    if ((noteOffPos->status&0xF0)>>4 == MIDI_STATUS_NOTE_ON
                        && channel == (eventPos->status&0x0F)
                        && pitch == noteOffPos->data[0]
                        && 0 == noteOffPos->data[1])
                        break;
                }

                /* Did we find a note off? Should always be the case, but who knows... */
                if (noteOffPos)
                {
                    /* We found a NOTE OFF, now calculate the note duration */
                    int duration = (int)((noteOffPos->time - eventPos->time)*tick);

                    REST_IF_NECESSARY();
                    /* Now we need to check if we get along with a normal Note Event, or if we need an extended one... */
                    if (duration < 2048 && pitch>=32 && pitch<=95 && velocity>=0 && velocity<=127)
                    {
                        qtma_StuffNoteEvent(*tunePos, part, pitch, velocity, duration);
                        tunePos++;
                    }
                    else
                    {
                        qtma_StuffXNoteEvent(*tunePos, *(tunePos+1), part, pitch, velocity, duration);
                        tunePos+=2;
                    }
                }
            }
            break;
        case MIDI_STATUS_AFTERTOUCH:
            /* NYI - use kControllerAfterTouch. But how are the parameters to be mapped? */
            break;
        case MIDI_STATUS_CONTROLLER:
            controller = eventPos->data[0];
            value = eventPos->data[1];

            switch(controller)
            {
            case 0: /* bank change - igore for now */
                break;
            case kControllerVolume:
                if(channel_vol[channel] != value<<8)
                {
                    channel_vol[channel] = value<<8;
                    if(part>=0 && part<=31)
                    {
                        REST_IF_NECESSARY();
                        qtma_StuffControlEvent(*tunePos, part, kControllerVolume, channel_vol[channel]);
                        tunePos++;
                    }
                }
                break;
            case kControllerPan:
                if(channel_pan[channel] != (value << 1) + 256)
                {
                    channel_pan[channel] = (value << 1) + 256;
                    if(part>=0 && part<=31)
                    {
                        REST_IF_NECESSARY();
                        qtma_StuffControlEvent(*tunePos, part, kControllerPan, channel_pan[channel]);
                        tunePos++;
                    }
                }
                break;
            default:
                /* No other controllers implemented yet */;
                break;
            }

            break;
        case MIDI_STATUS_PROG_CHANGE:
            /* Instrument changed */
            newInst = eventPos->data[0];

            /* Channel 9 (the 10th channel) is different, it indicates a drum kit */
            if (channel == 9)
                newInst += kFirstDrumkit;
            else
                newInst += kFirstGMInstrument;
            /* Only if the instrument for this channel *really* changed, add a new part. */
            if(newInst != part_to_inst[part])
            {
                /* TODO maybe make use of kGeneralEventPartChange here,
                   to help QT reuse note channels?
                */
                part = channel_to_part[channel] = *numParts;
                part_to_inst[(*numParts)++] = newInst;

                if(channel_vol[channel] >= 0)
                {
                    REST_IF_NECESSARY();
                    qtma_StuffControlEvent(*tunePos, part, kControllerVolume, channel_vol[channel]);
                    tunePos++;
                }
                if(channel_pan[channel] >= 0)
                {
                    REST_IF_NECESSARY();
                    qtma_StuffControlEvent(*tunePos, part, kControllerPan, channel_pan[channel]);
                    tunePos++;
                }
                if(channel_pitch_bend[channel] >= 0)
                {
                    REST_IF_NECESSARY();
                    qtma_StuffControlEvent(*tunePos, part, kControllerPitchBend, channel_pitch_bend[channel]);
                    tunePos++;
                }
            }
            break;
        case MIDI_STATUS_PRESSURE:
            /* NYI */
            break;
        case MIDI_STATUS_PITCH_WHEEL:
            /* In the midi spec, 0x2000 = center, 0x0000 = - 2 semitones, 0x3FFF = +2 semitones
               but for QTMA, we specify it as a 8.8 fixed point of semitones
               TODO: detect "pitch bend range changes" & honor them!
            */
            bend = (eventPos->data[0] & 0x7f) | ((eventPos->data[1] & 0x7f) << 7);

            /* "Center" the bend */
            bend -= 0x2000;

            /* Move it to our format: */
            bend <<= 4;

            /* If it turns out the pitch bend didn't change, stop here */
            if(channel_pitch_bend[channel] == bend)
                break;

            channel_pitch_bend[channel] = bend;
            if(part>=0 && part<=31)
            {
                /* Stuff a control event */
                REST_IF_NECESSARY();
                qtma_StuffControlEvent(*tunePos, part, kControllerPitchBend, bend);
                tunePos++;
            }
            break;
        case MIDI_STATUS_SYSEX:
            if (eventPos->status == 0xFF && eventPos->data[0] == 0x51) /* Tempo change */
            {
                tempo = (eventPos->extraData[0] << 16) +
                    (eventPos->extraData[1] << 8) +
                    eventPos->extraData[2];

                tick = tempo * Ippqn;
            }
            break;
        }

        /* on to the next event */
        eventPos = eventPos->next;
    }

    /* Finally, place an end marker */
    *tunePos = kEndMarkerValue;

    return tuneSequence;
}

Uint32 *BuildTuneHeader(int part_poly_max[32], int part_to_inst[32], int numParts)
{
    Uint32          *myHeader;
    Uint32          *myPos1, *myPos2;       /* pointers to the head and tail long words of a music event */
    NoteRequest     *myNoteRequest;
    NoteAllocator   myNoteAllocator;        /* for the NAStuffToneDescription call */
    ComponentResult myErr = noErr;
    int             part;

    myHeader = NULL;
    myNoteAllocator = NULL;

    /*
     * Open up the Note Allocator
     */
    myNoteAllocator = OpenDefaultComponent(kNoteAllocatorComponentType,0);
    if (myNoteAllocator == NULL)
        goto bail;

    /*
     * Allocate space for the tune header
     */
    myHeader = (Uint32 *)
            NewPtrClear((numParts * kNoteRequestEventLength + kMarkerEventLength) * sizeof(Uint32));
    if (myHeader == NULL)
        goto bail;

    myPos1 = myHeader;

    /*
     * Loop over all parts
     */
    for(part = 0; part < numParts; ++part)
    {
        /*
         * Stuff request for the instrument with the given polyphony
         */
        myPos2 = myPos1 + (kNoteRequestEventLength - 1); /* last longword of general event */
        qtma_StuffGeneralEvent(*myPos1, *myPos2, part, kGeneralEventNoteRequest, kNoteRequestEventLength);
        myNoteRequest = (NoteRequest *)(myPos1 + 1);
        myNoteRequest->info.flags = 0;
        /* I'm told by the Apple people that the Quicktime types were poorly designed and it was
         * too late to change them. On little endian, the BigEndian(Short|Fixed) types are structs
         * while on big endian they are primitive types. Furthermore, Quicktime failed to
         * provide setter and getter functions. To get this to work, we need to case the
         * code for the two possible situations.
         * My assumption is that the right-side value was always expected to be BigEndian
         * as it was written way before the Universal Binary transition. So in the little endian
         * case, OSSwap is used.
         */
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
        myNoteRequest->info.polyphony.bigEndianValue = OSSwapHostToBigInt16(part_poly_max[part]);
        myNoteRequest->info.typicalPolyphony.bigEndianValue = OSSwapHostToBigInt32(0x00010000);
#else
        myNoteRequest->info.polyphony = part_poly_max[part];
        myNoteRequest->info.typicalPolyphony = 0x00010000;
#endif
        myErr = NAStuffToneDescription(myNoteAllocator,part_to_inst[part],&myNoteRequest->tone);
        if (myErr != noErr)
            goto bail;

        /* move pointer to beginning of next event */
        myPos1 += kNoteRequestEventLength;
    }

    *myPos1 = kEndMarkerValue;      /* end of sequence marker */


bail:
    if(myNoteAllocator)
        CloseComponent(myNoteAllocator);

    /* if we encountered an error, dispose of the storage we allocated and return NULL */
    if (myErr != noErr) {
        DisposePtr((Ptr)myHeader);
        myHeader = NULL;
    }

    return myHeader;
}

#endif /* MacOS native MIDI support */
