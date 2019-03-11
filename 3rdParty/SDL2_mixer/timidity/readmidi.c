/*

    TiMidity -- Experimental MIDI to WAVE converter
    Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the Perl Artistic License, available in COPYING.
*/

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "options.h"
#include "timidity.h"
#include "common.h"
#include "instrum.h"
#include "playmidi.h"

/* Computes how many (fractional) samples one MIDI delta-time unit contains */
static void compute_sample_increment(MidiSong *song, Sint32 tempo,
				     Sint32 divisions)
{
  double a;
  a = (double) (tempo) * (double) (song->rate) * (65536.0/1000000.0) /
    (double)(divisions);

  song->sample_correction = (Sint32)(a) & 0xFFFF;
  song->sample_increment = (Sint32)(a) >> 16;

  SNDDBG(("Samples per delta-t: %d (correction %d)",
	  song->sample_increment, song->sample_correction));
}

/* Read variable-length number (7 bits per byte, MSB first) */
static Sint32 getvl(SDL_RWops *rw)
{
  Sint32 l=0;
  Uint8 c;
  for (;;)
    {
      if (!SDL_RWread(rw, &c, 1, 1)) return l;
      l += (c & 0x7f);
      if (!(c & 0x80)) return l;
      l<<=7;
    }
}

/* Print a string from the file, followed by a newline. Any non-ASCII
   or unprintable characters will be converted to periods. */
static int dumpstring(SDL_RWops *rw, Sint32 len, char *label)
{
  signed char *s=safe_malloc(len+1);
  if (len != (Sint32) SDL_RWread(rw, s, 1, len))
    {
      free(s);
      return -1;
    }
  s[len]='\0';
  while (len--)
    {
      if (s[len]<32)
	s[len]='.';
    }
  SNDDBG(("%s%s", label, s));
  free(s);
  return 0;
}

#define MIDIEVENT(at,t,ch,pa,pb) \
  new=safe_malloc(sizeof(MidiEventList)); \
  new->event.time=at; new->event.type=t; new->event.channel=ch; \
  new->event.a=pa; new->event.b=pb; new->next=0;\
  return new;

#define MAGIC_EOT ((MidiEventList *)(-1))

/* Read a MIDI event, returning a freshly allocated element that can
   be linked to the event list */
static MidiEventList *read_midi_event(MidiSong *song)
{
  static Uint8 laststatus, lastchan;
  static Uint8 nrpn=0, rpn_msb[16], rpn_lsb[16]; /* one per channel */
  Uint8 me, type, a,b,c;
  Sint32 len;
  MidiEventList *new;

  for (;;)
    {
      song->at += getvl(song->rw);
      if (SDL_RWread(song->rw, &me, 1, 1) != 1)
	{
	  SNDDBG(("read_midi_event: SDL_RWread() failure\n"));
	  return NULL;
	}
      
      if(me==0xF0 || me == 0xF7) /* SysEx event */
	{
	  len=getvl(song->rw);
	  SDL_RWseek(song->rw, len, RW_SEEK_CUR);
	}
      else if(me==0xFF) /* Meta event */
	{
	  SDL_RWread(song->rw, &type, 1, 1);
	  len=getvl(song->rw);
	  if (type>0 && type<16)
	    {
	      static char *label[]={
		"Text event: ", "Text: ", "Copyright: ", "Track name: ",
		"Instrument: ", "Lyric: ", "Marker: ", "Cue point: "};
	      dumpstring(song->rw, len, label[(type>7) ? 0 : type]);
	    }
	  else
	    switch(type)
	      {
	      case 0x2F: /* End of Track */
		return MAGIC_EOT;

	      case 0x51: /* Tempo */
		SDL_RWread(song->rw, &a, 1, 1);
		SDL_RWread(song->rw, &b, 1, 1);
		SDL_RWread(song->rw, &c, 1, 1);
		MIDIEVENT(song->at, ME_TEMPO, c, a, b);
		
	      default:
		SNDDBG(("(Meta event type 0x%02x, length %d)\n", type, len));
		SDL_RWseek(song->rw, len, RW_SEEK_CUR);
		break;
	      }
	}
      else
	{
	  a=me;
	  if (a & 0x80) /* status byte */
	    {
	      lastchan=a & 0x0F;
	      laststatus=(a>>4) & 0x07;
	      SDL_RWread(song->rw, &a, 1, 1);
	      a &= 0x7F;
	    }
	  switch(laststatus)
	    {
	    case 0: /* Note off */
	      SDL_RWread(song->rw, &b, 1, 1);
	      b &= 0x7F;
	      MIDIEVENT(song->at, ME_NOTEOFF, lastchan, a,b);

	    case 1: /* Note on */
	      SDL_RWread(song->rw, &b, 1, 1);
	      b &= 0x7F;
	      MIDIEVENT(song->at, ME_NOTEON, lastchan, a,b);

	    case 2: /* Key Pressure */
	      SDL_RWread(song->rw, &b, 1, 1);
	      b &= 0x7F;
	      MIDIEVENT(song->at, ME_KEYPRESSURE, lastchan, a, b);

	    case 3: /* Control change */
	      SDL_RWread(song->rw, &b, 1, 1);
	      b &= 0x7F;
	      {
		int control=255;
		switch(a)
		  {
		  case 7: control=ME_MAINVOLUME; break;
		  case 10: control=ME_PAN; break;
		  case 11: control=ME_EXPRESSION; break;
		  case 64: control=ME_SUSTAIN; b = (b >= 64); break;
		  case 120: control=ME_ALL_SOUNDS_OFF; break;
		  case 121: control=ME_RESET_CONTROLLERS; break;
		  case 123: control=ME_ALL_NOTES_OFF; break;

		    /* These should be the SCC-1 tone bank switch
		       commands. I don't know why there are two, or
		       why the latter only allows switching to bank 0.
		       Also, some MIDI files use 0 as some sort of
		       continuous controller. This will cause lots of
		       warnings about undefined tone banks. */
		  case 0: control=ME_TONE_BANK; break;
		  case 32:
		    if (b!=0) {
		      SNDDBG(("(Strange: tone bank change 0x%02x)\n", b));
		    }
#if 0	/* `Bank Select LSB' is not worked at GS. Please ignore it. */
		    else
		      control=ME_TONE_BANK;
#endif
		    break;

		  case 100: nrpn=0; rpn_msb[lastchan]=b; break;
		  case 101: nrpn=0; rpn_lsb[lastchan]=b; break;
		  case 99: nrpn=1; rpn_msb[lastchan]=b; break;
		  case 98: nrpn=1; rpn_lsb[lastchan]=b; break;
		    
		  case 6:
		    if (nrpn)
		      {
			SNDDBG(("(Data entry (MSB) for NRPN %02x,%02x: %d)\n",
				rpn_msb[lastchan], rpn_lsb[lastchan], b));
			break;
		      }
		    
		    switch((rpn_msb[lastchan]<<8) | rpn_lsb[lastchan])
		      {
		      case 0x0000: /* Pitch bend sensitivity */
			control=ME_PITCH_SENS;
			break;

		      case 0x7F7F: /* RPN reset */
			/* reset pitch bend sensitivity to 2 */
			MIDIEVENT(song->at, ME_PITCH_SENS, lastchan, 2, 0);

		      default:
			SNDDBG(("(Data entry (MSB) for RPN %02x,%02x: %d)\n",
				rpn_msb[lastchan], rpn_lsb[lastchan], b));
			break;
		      }
		    break;
		    
		  default:
		    SNDDBG(("(Control %d: %d)\n", a, b));
		    break;
		  }
		if (control != 255)
		  { 
		    MIDIEVENT(song->at, control, lastchan, b, 0); 
		  }
	      }
	      break;

	    case 4: /* Program change */
	      a &= 0x7f;
	      MIDIEVENT(song->at, ME_PROGRAM, lastchan, a, 0);

	    case 5: /* Channel pressure - NOT IMPLEMENTED */
	      break;

	    case 6: /* Pitch wheel */
	      SDL_RWread(song->rw, &b, 1, 1);
	      b &= 0x7F;
	      MIDIEVENT(song->at, ME_PITCHWHEEL, lastchan, a, b);

	    default: 
	      SNDDBG(("*** Can't happen: status 0x%02X, channel 0x%02X\n",
		      laststatus, lastchan));
	      break;
	    }
	}
    }
  
  return new;
}

#undef MIDIEVENT

/* Read a midi track into the linked list, either merging with any previous
   tracks or appending to them. */
static int read_track(MidiSong *song, int append)
{
  MidiEventList *meep;
  MidiEventList *next, *new;
  Sint32 len;
  Sint64 next_pos, pos;
  char tmp[4];

  meep = song->evlist;
  if (append && meep)
    {
      /* find the last event in the list */
      for (; meep->next; meep=meep->next)
	;
      song->at = meep->event.time;
    }
  else
    song->at=0;

  /* Check the formalities */
  
  if (SDL_RWread(song->rw, tmp, 1, 4) != 4 || SDL_RWread(song->rw, &len, 4, 1) != 1)
    {
      SNDDBG(("Can't read track header.\n"));
      return -1;
    }
  len=SDL_SwapBE32(len);
  next_pos = SDL_RWtell(song->rw) + len;
  if (memcmp(tmp, "MTrk", 4))
    {
      SNDDBG(("Corrupt MIDI file.\n"));
      return -2;
    }

  for (;;)
    {
      if (!(new=read_midi_event(song))) /* Some kind of error  */
	return -2;

      if (new==MAGIC_EOT) /* End-of-track Hack. */
	{
          pos = SDL_RWtell(song->rw);
          if (pos < next_pos)
            SDL_RWseek(song->rw, next_pos - pos, RW_SEEK_CUR);
	  return 0;
	}

      next=meep->next;
      while (next && (next->event.time < new->event.time))
	{
	  meep=next;
	  next=meep->next;
	}
	  
      new->next=next;
      meep->next=new;

      song->event_count++; /* Count the event. (About one?) */
      meep=new;
    }
}

/* Free the linked event list from memory. */
static void free_midi_list(MidiSong *song)
{
  MidiEventList *meep, *next;
  if (!(meep = song->evlist)) return;
  while (meep)
    {
      next=meep->next;
      free(meep);
      meep=next;
    }
  song->evlist=NULL;
}

/* Allocate an array of MidiEvents and fill it from the linked list of
   events, marking used instruments for loading. Convert event times to
   samples: handle tempo changes. Strip unnecessary events from the list.
   Free the linked list. */
static MidiEvent *groom_list(MidiSong *song, Sint32 divisions,Sint32 *eventsp,
			     Sint32 *samplesp)
{
  MidiEvent *groomed_list, *lp;
  MidiEventList *meep;
  Sint32 i, our_event_count, tempo, skip_this_event, new_value;
  Sint32 sample_cum, samples_to_do, at, st, dt, counting_time;

  int current_bank[MAXCHAN], current_set[MAXCHAN], current_program[MAXCHAN]; 
  /* Or should each bank have its own current program? */

  for (i=0; i<MAXCHAN; i++)
    {
      current_bank[i]=0;
      current_set[i]=0;
      current_program[i]=song->default_program;
    }

  tempo=500000;
  compute_sample_increment(song, tempo, divisions);

  /* This may allocate a bit more than we need */
  groomed_list=lp=safe_malloc(sizeof(MidiEvent) * (song->event_count+1));
  meep=song->evlist;

  our_event_count=0;
  st=at=sample_cum=0;
  counting_time=2; /* We strip any silence before the first NOTE ON. */

  for (i = 0; i < song->event_count; i++)
    {
      skip_this_event=0;

      if (meep->event.type==ME_TEMPO)
	{
	  skip_this_event=1;
	}
      else if (meep->event.channel >= MAXCHAN)
        skip_this_event=1;
      else switch (meep->event.type)
	{
	case ME_PROGRAM:
	  if (ISDRUMCHANNEL(song, meep->event.channel))
	    {
	      if (song->drumset[meep->event.a]) /* Is this a defined drumset? */
		new_value=meep->event.a;
	      else
		{
		  SNDDBG(("Drum set %d is undefined\n", meep->event.a));
		  new_value=meep->event.a=0;
		}
	      if (current_set[meep->event.channel] != new_value)
		current_set[meep->event.channel]=new_value;
	      else 
		skip_this_event=1;
	    }
	  else
	    {
	      new_value=meep->event.a;
	      if ((current_program[meep->event.channel] != SPECIAL_PROGRAM)
		  && (current_program[meep->event.channel] != new_value))
		current_program[meep->event.channel] = new_value;
	      else
		skip_this_event=1;
	    }
	  break;

	case ME_NOTEON:
	  if (counting_time)
	    counting_time=1;
	  if (ISDRUMCHANNEL(song, meep->event.channel))
	    {
	      /* Mark this instrument to be loaded */
	      if (!(song->drumset[current_set[meep->event.channel]]
		    ->instrument[meep->event.a]))
		song->drumset[current_set[meep->event.channel]]
		  ->instrument[meep->event.a] = MAGIC_LOAD_INSTRUMENT;
	    }
	  else
	    {
	      if (current_program[meep->event.channel]==SPECIAL_PROGRAM)
		break;
	      /* Mark this instrument to be loaded */
	      if (!(song->tonebank[current_bank[meep->event.channel]]
		    ->instrument[current_program[meep->event.channel]]))
		song->tonebank[current_bank[meep->event.channel]]
		  ->instrument[current_program[meep->event.channel]] =
		    MAGIC_LOAD_INSTRUMENT;
	    }
	  break;

	case ME_TONE_BANK:
	  if (ISDRUMCHANNEL(song, meep->event.channel))
	    {
	      skip_this_event=1;
	      break;
	    }
	  if (song->tonebank[meep->event.a]) /* Is this a defined tone bank? */
	    new_value=meep->event.a;
	  else 
	    {
	      SNDDBG(("Tone bank %d is undefined\n", meep->event.a));
	      new_value=meep->event.a=0;
	    }
	  if (current_bank[meep->event.channel]!=new_value)
	    current_bank[meep->event.channel]=new_value;
	  else
	    skip_this_event=1;
	  break;
	}

      /* Recompute time in samples*/
      if ((dt=meep->event.time - at) && !counting_time)
	{
	  if (song->sample_increment  > 2147483647/dt ||
	      song->sample_correction > 2147483647/dt) {
	      goto _overflow;
	    }
	  samples_to_do = song->sample_increment * dt;
	  sample_cum += song->sample_correction * dt;
	  if (sample_cum & 0xFFFF0000)
	    {
	      samples_to_do += ((sample_cum >> 16) & 0xFFFF);
	      sample_cum &= 0x0000FFFF;
	    }
	  if (st >= 2147483647 - samples_to_do) {
	  _overflow:
	      SNDDBG(("Overflow in sample counter\n"));
	      free_midi_list(song);
	      free(groomed_list);
	      return NULL;
	    }
	  st += samples_to_do;
	}
      else if (counting_time==1) counting_time=0;
      if (meep->event.type==ME_TEMPO)
	{
	  tempo=
	    meep->event.channel + meep->event.b * 256 + meep->event.a * 65536;
	  compute_sample_increment(song, tempo, divisions);
	}
      if (!skip_this_event)
	{
	  /* Add the event to the list */
	  *lp=meep->event;
	  lp->time=st;
	  lp++;
	  our_event_count++;
	}
      at=meep->event.time;
      meep=meep->next;
    }
  /* Add an End-of-Track event */
  lp->time=st;
  lp->type=ME_EOT;
  our_event_count++;
  free_midi_list(song);

  *eventsp=our_event_count;
  *samplesp=st;
  return groomed_list;
}

MidiEvent *read_midi_file(MidiSong *song, Sint32 *count, Sint32 *sp)
{
  Sint32 len, divisions;
  Sint16 format, tracks, divisions_tmp;
  int i;
  char tmp[4];

  song->event_count=0;
  song->at=0;
  song->evlist = NULL;

  if (SDL_RWread(song->rw, tmp, 1, 4) != 4 || SDL_RWread(song->rw, &len, 4, 1) != 1)
    {
      SNDDBG(("Not a MIDI file!\n"));
      return NULL;
    }
  if (memcmp(tmp, "RIFF", 4) == 0) { /* RMID ?? */
    if (SDL_RWread(song->rw, tmp, 1, 4) != 4 || memcmp(tmp, "RMID", 4) != 0 ||
	SDL_RWread(song->rw, tmp, 1, 4) != 4 || memcmp(tmp, "data", 4) != 0 ||
	SDL_RWread(song->rw, tmp, 1, 4) != 4 ||
	/* SMF must begin from here onwards: */
	SDL_RWread(song->rw, tmp, 1, 4) != 4 || SDL_RWread(song->rw, &len, 4, 1) != 1)
      {
	SNDDBG(("Not an RMID file!\n"));
	return NULL;
      }
  }
  len=SDL_SwapBE32(len);
  if (memcmp(tmp, "MThd", 4) || len < 6)
    {
      SNDDBG(("Not a MIDI file!\n"));
      return NULL;
    }

  SDL_RWread(song->rw, &format, 2, 1);
  SDL_RWread(song->rw, &tracks, 2, 1);
  SDL_RWread(song->rw, &divisions_tmp, 2, 1);
  format=SDL_SwapBE16(format);
  tracks=SDL_SwapBE16(tracks);
  divisions_tmp=SDL_SwapBE16(divisions_tmp);

  if (divisions_tmp<0)
    {
      /* SMPTE time -- totally untested. Got a MIDI file that uses this? */
      divisions=
	(Sint32)(-(divisions_tmp/256)) * (Sint32)(divisions_tmp & 0xFF);
    }
  else divisions=(Sint32)(divisions_tmp);

  if (len > 6)
    {
      SNDDBG(("MIDI file header size %u bytes", len));
      SDL_RWseek(song->rw, len-6, RW_SEEK_CUR); /* skip the excess */
    }
  if (format<0 || format >2)
    {
      SNDDBG(("Unknown MIDI file format %d\n", format));
      return NULL;
    }
  if (tracks<1)
    {
      SNDDBG(("Bad number of tracks %d\n", tracks));
      return NULL;
    }
  if (format==0 && tracks!=1)
    {
      SNDDBG(("%d tracks with Type-0 MIDI (must be 1.)\n", tracks));
      return NULL;
    }
  SNDDBG(("Format: %d  Tracks: %d  Divisions: %d\n",
	  format, tracks, divisions));

  /* Put a do-nothing event first in the list for easier processing */
  song->evlist=safe_malloc(sizeof(MidiEventList));
  memset(song->evlist, 0, sizeof(MidiEventList));
  song->event_count++;

  switch(format)
    {
    case 0:
      if (read_track(song, 0))
	{
	  free_midi_list(song);
	  return NULL;
	}
      break;

    case 1:
      for (i=0; i<tracks; i++)
	if (read_track(song, 0))
	  {
	    free_midi_list(song);
	    return NULL;
	  }
      break;

    case 2: /* We simply play the tracks sequentially */
      for (i=0; i<tracks; i++)
	if (read_track(song, 1))
	  {
	    free_midi_list(song);
	    return NULL;
	  }
      break;
    }

  return groom_list(song, divisions, count, sp);
}
