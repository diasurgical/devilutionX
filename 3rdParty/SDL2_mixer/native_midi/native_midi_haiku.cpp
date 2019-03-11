/*
  native_midi_haiku:  Native Midi support on Haiku for the SDL_mixer library
  Copyright (C) 2010  Egor Suvorov <egor_suvorov@mail.ru>

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

#ifdef __HAIKU__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MidiStore.h>
#include <MidiDefs.h>
#include <MidiSynthFile.h>
#include <algorithm>
#include <assert.h>
extern "C" {
#include "native_midi.h"
#include "native_midi_common.h"
}

bool compareMIDIEvent(const MIDIEvent &a, const MIDIEvent &b)
{
  return a.time < b.time;
}

class MidiEventsStore : public BMidi
{
  public:
  MidiEventsStore()
  {
    fPlaying = false;
    fLoops = 0;
  }
  virtual status_t Import(SDL_RWops *src)
  {
    fEvs = CreateMIDIEventList(src, &fDivision);
    if (!fEvs) {
      return B_BAD_MIDI_DATA;
    }
    fTotal = 0;
    for (MIDIEvent *x = fEvs; x; x = x->next) fTotal++;
    fPos = fTotal;

    sort_events();
    return B_OK;
  }
  virtual void Run()
  {
    fPlaying = true;
    fPos = 0;
    MIDIEvent *ev = fEvs;

    uint32 startTime = B_NOW;
    while (KeepRunning())
    {
      if (!ev) {
        if (fLoops && fEvs) {
          if (fLoops > 0) --fLoops;
          fPos = 0;
          ev = fEvs;
        } else
          break;
      }
      SprayEvent(ev, ev->time + startTime);
      ev = ev->next;
      fPos++;
    }
    fPos = fTotal;
    fPlaying = false;
  }
  virtual ~MidiEventsStore()
  {
    if (!fEvs) return;
    FreeMIDIEventList(fEvs);
    fEvs = 0;
  }

  bool IsPlaying()
  {
    return fPlaying;
  }

  void SetLoops(int loops)
  {
    fLoops = loops;
  }

  protected:
  MIDIEvent *fEvs;
  Uint16 fDivision;

  int fPos, fTotal;
  int fLoops;
  bool fPlaying;

  void SprayEvent(MIDIEvent *ev, uint32 time)
  {
    switch (ev->status & 0xF0)
    {
    case B_NOTE_OFF:
      SprayNoteOff((ev->status & 0x0F) + 1, ev->data[0], ev->data[1], time);
      break;
    case B_NOTE_ON:
      SprayNoteOn((ev->status & 0x0F) + 1, ev->data[0], ev->data[1], time);
      break;
    case B_KEY_PRESSURE:
      SprayKeyPressure((ev->status & 0x0F) + 1, ev->data[0], ev->data[1], time);
      break;
    case B_CONTROL_CHANGE:
      SprayControlChange((ev->status & 0x0F) + 1, ev->data[0], ev->data[1], time);
      break;
    case B_PROGRAM_CHANGE:
      SprayProgramChange((ev->status & 0x0F) + 1, ev->data[0], time);
      break;
    case B_CHANNEL_PRESSURE:
      SprayChannelPressure((ev->status & 0x0F) + 1, ev->data[0], time);
      break;
    case B_PITCH_BEND:
      SprayPitchBend((ev->status & 0x0F) + 1, ev->data[0], ev->data[1], time);
      break;
    case 0xF:
      switch (ev->status)
      {
      case B_SYS_EX_START:
        SpraySystemExclusive(ev->extraData, ev->extraLen, time);
    break;
      case B_MIDI_TIME_CODE:
      case B_SONG_POSITION:
      case B_SONG_SELECT:
      case B_CABLE_MESSAGE:
      case B_TUNE_REQUEST:
      case B_SYS_EX_END:
        SpraySystemCommon(ev->status, ev->data[0], ev->data[1], time);
    break;
      case B_TIMING_CLOCK:
      case B_START:
      case B_STOP:
      case B_CONTINUE:
      case B_ACTIVE_SENSING:
        SpraySystemRealTime(ev->status, time);
    break;
      case B_SYSTEM_RESET:
        if (ev->data[0] == 0x51 && ev->data[1] == 0x03)
    {
      assert(ev->extraLen == 3);
      int val = (ev->extraData[0] << 16) | (ev->extraData[1] << 8) | ev->extraData[2];
      int tempo = 60000000 / val;
      SprayTempoChange(tempo, time);
    }
    else
    {
      SpraySystemRealTime(ev->status, time);
    }
      }
      break;
    }
  }

  void sort_events()
  {
    MIDIEvent *items = new MIDIEvent[fTotal];
    MIDIEvent *x = fEvs;
    for (int i = 0; i < fTotal; i++)
    {
      memcpy(items + i, x, sizeof(MIDIEvent));
      x = x->next;
    }
    std::sort(items, items + fTotal, compareMIDIEvent);

    x = fEvs;
    for (int i = 0; i < fTotal; i++)
    {
      MIDIEvent *ne = x->next;
      memcpy(x, items + i, sizeof(MIDIEvent));
      x->next = ne;
      x = ne;
    }

    for (x = fEvs; x && x->next; x = x->next)
      assert(x->time <= x->next->time);

    delete[] items;
  }
};

BMidiSynth synth;
struct _NativeMidiSong {
  MidiEventsStore *store;
} *currentSong = NULL;

char lasterr[1024];

int native_midi_detect(void)
{
  status_t res = synth.EnableInput(true, false);
  return res == B_OK;
}

void native_midi_setvolume(int volume)
{
  if (volume < 0) volume = 0;
  if (volume > 128) volume = 128;
  synth.SetVolume(volume / 128.0);
}

NativeMidiSong *native_midi_loadsong_RW(SDL_RWops *src, int freesrc)
{
  NativeMidiSong *song = new NativeMidiSong;
  song->store = new MidiEventsStore;
  status_t res = song->store->Import(src);

  if (res != B_OK)
  {
    snprintf(lasterr, sizeof lasterr, "Cannot Import() midi file: status_t=%d", res);
    delete song->store;
    delete song;
    return NULL;
  }
  else
  {
    if (freesrc) {
      SDL_RWclose(src);
    }
  }
  return song;
}

void native_midi_freesong(NativeMidiSong *song)
{
  if (song == NULL) return;
  song->store->Stop();
  song->store->Disconnect(&synth);
  if (currentSong == song)
  {
    currentSong = NULL;
  }
  delete song->store;
  delete song; song = 0;
}

void native_midi_start(NativeMidiSong *song, int loops)
{
  native_midi_stop();
  song->store->Connect(&synth);
  song->store->SetLoops(loops);
  song->store->Start();
  currentSong = song;
}

void native_midi_pause(void)
{
}

void native_midi_resume(void)
{
}

void native_midi_stop(void)
{
  if (currentSong == NULL) return;
  currentSong->store->Stop();
  currentSong->store->Disconnect(&synth);
  while (currentSong->store->IsPlaying())
    usleep(1000);
  currentSong = NULL;
}

int native_midi_active(void)
{
  if (currentSong == NULL) return 0;
  return currentSong->store->IsPlaying();
}

const char* native_midi_error(void)
{
  return lasterr;
}

#endif /* __HAIKU__ */
