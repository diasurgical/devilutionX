/*

    TiMidity -- Experimental MIDI to WAVE converter
    Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the Perl Artistic License, available in COPYING.

   instrum.c 
   
   Code to load and unload GUS-compatible instrument patches.

*/

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "SDL.h"

#include "timidity.h"
#include "options.h"
#include "common.h"
#include "instrum.h"
#include "resample.h"
#include "tables.h"

static void free_instrument(Instrument *ip)
{
  Sample *sp;
  int i;
  if (!ip) return;
  for (i=0; i<ip->samples; i++)
    {
      sp=&(ip->sample[i]);
      free(sp->data);
    }
  free(ip->sample);
  free(ip);
}

static void free_bank(MidiSong *song, int dr, int b)
{
  int i;
  ToneBank *bank=((dr) ? song->drumset[b] : song->tonebank[b]);
  for (i=0; i<MAXBANK; i++)
    if (bank->instrument[i])
      {
	/* Not that this could ever happen, of course */
	if (bank->instrument[i] != MAGIC_LOAD_INSTRUMENT)
	  free_instrument(bank->instrument[i]);
	bank->instrument[i]=0;
      }
}

static Sint32 convert_envelope_rate(MidiSong *song, Uint8 rate)
{
  Sint32 r;
  
  r = 3 - ((rate >> 6) & 0x3);
  r *= 3;
  r = (Sint32) (rate & 0x3f) << r; /* 6.9 fixed point */

  /* 15.15 fixed point. */
  r = ((r * 44100) / song->rate) * song->control_ratio;

#ifdef FAST_DECAY
  return r << 10;
#else
  return r << 9;
#endif
}

static Sint32 convert_envelope_offset(Uint8 offset)
{
  /* This is not too good... Can anyone tell me what these values mean?
     Are they GUS-style "exponential" volumes? And what does that mean? */

  /* 15.15 fixed point */
  return offset << (7+15);
}

static Sint32 convert_tremolo_sweep(MidiSong *song, Uint8 sweep)
{
  if (!sweep)
    return 0;

  return
    ((song->control_ratio * SWEEP_TUNING) << SWEEP_SHIFT) /
      (song->rate * sweep);
}

static Sint32 convert_vibrato_sweep(MidiSong *song, Uint8 sweep,
				    Sint32 vib_control_ratio)
{
  if (!sweep)
    return 0;

  return
    (Sint32) (FSCALE((double) (vib_control_ratio) * SWEEP_TUNING, SWEEP_SHIFT)
	     / (double)(song->rate * sweep));

  /* this was overflowing with seashore.pat

      ((vib_control_ratio * SWEEP_TUNING) << SWEEP_SHIFT) /
      (song->rate * sweep); */
}

static Sint32 convert_tremolo_rate(MidiSong *song, Uint8 rate)
{
  return
    ((SINE_CYCLE_LENGTH * song->control_ratio * rate) << RATE_SHIFT) /
      (TREMOLO_RATE_TUNING * song->rate);
}

static Sint32 convert_vibrato_rate(MidiSong *song, Uint8 rate)
{
  /* Return a suitable vibrato_control_ratio value */
  return
    (VIBRATO_RATE_TUNING * song->rate) / 
      (rate * 2 * VIBRATO_SAMPLE_INCREMENTS);
}

static void reverse_data(Sint16 *sp, Sint32 ls, Sint32 le)
{
  Sint16 s, *ep=sp+le;
  sp+=ls;
  le-=ls;
  le/=2;
  while (le--)
    {
      s=*sp;
      *sp++=*ep;
      *ep--=s;
    }
}

/* 
   If panning or note_to_use != -1, it will be used for all samples,
   instead of the sample-specific values in the instrument file. 

   For note_to_use, any value <0 or >127 will be forced to 0.
 
   For other parameters, 1 means yes, 0 means no, other values are
   undefined.

   TODO: do reverse loops right */
static Instrument *load_instrument(MidiSong *song, char *name, int percussion,
				   int panning, int amp, int note_to_use,
				   int strip_loop, int strip_envelope,
				   int strip_tail)
{
  Instrument *ip;
  Sample *sp;
  SDL_RWops *rw;
  char tmp[1024];
  int i,j,noluck=0;
  static char *patch_ext[] = PATCH_EXT_LIST;

  if (!name) return 0;
  
  /* Open patch file */
  if ((rw=open_file(name)) == NULL)
    {
      noluck=1;
      /* Try with various extensions */
      for (i=0; patch_ext[i]; i++)
	{
	  if (strlen(name)+strlen(patch_ext[i])<1024)
	    {
	      strcpy(tmp, name);
	      strcat(tmp, patch_ext[i]);
	      if ((rw=open_file(tmp)) != NULL)
		{
		  noluck=0;
		  break;
		}
	    }
	}
    }
  
  if (noluck)
    {
      SNDDBG(("Instrument `%s' can't be found.\n", name));
      return 0;
    }
      
  SNDDBG(("Loading instrument %s\n", tmp));
  
  /* Read some headers and do cursory sanity checks. There are loads
     of magic offsets. This could be rewritten... */

  if ((239 != SDL_RWread(rw, tmp, 1, 239)) ||
      (memcmp(tmp, "GF1PATCH110\0ID#000002", 22) &&
       memcmp(tmp, "GF1PATCH100\0ID#000002", 22))) /* don't know what the
						      differences are */
    {
      SNDDBG(("%s: not an instrument\n", name));
      SDL_RWclose(rw);
      return 0;
    }
  
  if (tmp[82] != 1 && tmp[82] != 0) /* instruments. To some patch makers, 
				       0 means 1 */
    {
      SNDDBG(("Can't handle patches with %d instruments\n", tmp[82]));
      SDL_RWclose(rw);
      return 0;
    }

  if (tmp[151] != 1 && tmp[151] != 0) /* layers. What's a layer? */
    {
      SNDDBG(("Can't handle instruments with %d layers\n", tmp[151]));
      SDL_RWclose(rw);
      return 0;
    }
  
  ip=safe_malloc(sizeof(Instrument));
  ip->samples = tmp[198];
  ip->sample = safe_malloc(sizeof(Sample) * ip->samples);
  for (i=0; i<ip->samples; i++)
    {

      Uint8 fractions;
      Sint32 tmplong;
      Uint16 tmpshort;
      Uint8 tmpchar;

#define READ_CHAR(thing) \
      if (1 != SDL_RWread(rw, &tmpchar, 1, 1)) goto fail; \
      thing = tmpchar;
#define READ_SHORT(thing) \
      if (1 != SDL_RWread(rw, &tmpshort, 2, 1)) goto fail; \
      thing = SDL_SwapLE16(tmpshort);
#define READ_LONG(thing) \
      if (1 != SDL_RWread(rw, &tmplong, 4, 1)) goto fail; \
      thing = SDL_SwapLE32(tmplong);

      SDL_RWseek(rw, 7, RW_SEEK_CUR); /* Skip the wave name */

      if (1 != SDL_RWread(rw, &fractions, 1, 1))
	{
	fail:
	  SNDDBG(("Error reading sample %d\n", i));
	  for (j=0; j<i; j++)
	    free(ip->sample[j].data);
	  free(ip->sample);
	  free(ip);
	  SDL_RWclose(rw);
	  return 0;
	}

      sp=&(ip->sample[i]);
      
      READ_LONG(sp->data_length);
      READ_LONG(sp->loop_start);
      READ_LONG(sp->loop_end);
      READ_SHORT(sp->sample_rate);
      READ_LONG(sp->low_freq);
      READ_LONG(sp->high_freq);
      READ_LONG(sp->root_freq);
      SDL_RWseek(rw, 2, RW_SEEK_CUR); /* Why have a "root frequency" and then
				    * "tuning"?? */
      
      READ_CHAR(tmp[0]);

      if (panning==-1)
	sp->panning = (tmp[0] * 8 + 4) & 0x7f;
      else
	sp->panning=(Uint8)(panning & 0x7F);

      /* envelope, tremolo, and vibrato */
      if (18 != SDL_RWread(rw, tmp, 1, 18)) goto fail; 

      if (!tmp[13] || !tmp[14])
	{
	  sp->tremolo_sweep_increment=
	    sp->tremolo_phase_increment=sp->tremolo_depth=0;
	  SNDDBG((" * no tremolo\n"));
	}
      else
	{
	  sp->tremolo_sweep_increment=convert_tremolo_sweep(song, tmp[12]);
	  sp->tremolo_phase_increment=convert_tremolo_rate(song, tmp[13]);
	  sp->tremolo_depth=tmp[14];
	  SNDDBG((" * tremolo: sweep %d, phase %d, depth %d\n",
	       sp->tremolo_sweep_increment, sp->tremolo_phase_increment,
	       sp->tremolo_depth));
	}

      if (!tmp[16] || !tmp[17])
	{
	  sp->vibrato_sweep_increment=
	    sp->vibrato_control_ratio=sp->vibrato_depth=0;
	  SNDDBG((" * no vibrato\n"));
	}
      else
	{
	  sp->vibrato_control_ratio=convert_vibrato_rate(song, tmp[16]);
	  sp->vibrato_sweep_increment=
	    convert_vibrato_sweep(song, tmp[15], sp->vibrato_control_ratio);
	  sp->vibrato_depth=tmp[17];
	  SNDDBG((" * vibrato: sweep %d, ctl %d, depth %d\n",
	       sp->vibrato_sweep_increment, sp->vibrato_control_ratio,
	       sp->vibrato_depth));

	}

      READ_CHAR(sp->modes);

      SDL_RWseek(rw, 40, RW_SEEK_CUR); /* skip the useless scale frequency, scale
				       factor (what's it mean?), and reserved
				       space */

      /* Mark this as a fixed-pitch instrument if such a deed is desired. */
      if (note_to_use!=-1)
	sp->note_to_use=(Uint8)(note_to_use);
      else
	sp->note_to_use=0;
      
      /* seashore.pat in the Midia patch set has no Sustain. I don't
         understand why, and fixing it by adding the Sustain flag to
         all looped patches probably breaks something else. We do it
         anyway. */
	 
      if (sp->modes & MODES_LOOPING) 
	sp->modes |= MODES_SUSTAIN;

      /* Strip any loops and envelopes we're permitted to */
      if ((strip_loop==1) && 
	  (sp->modes & (MODES_SUSTAIN | MODES_LOOPING | 
			MODES_PINGPONG | MODES_REVERSE)))
	{
	  SNDDBG((" - Removing loop and/or sustain\n"));
	  sp->modes &=~(MODES_SUSTAIN | MODES_LOOPING | 
			MODES_PINGPONG | MODES_REVERSE);
	}

      if (strip_envelope==1)
	{
	  if (sp->modes & MODES_ENVELOPE) {
	    SNDDBG((" - Removing envelope\n"));
	  }
	  sp->modes &= ~MODES_ENVELOPE;
	}
      else if (strip_envelope != 0)
	{
	  /* Have to make a guess. */
	  if (!(sp->modes & (MODES_LOOPING | MODES_PINGPONG | MODES_REVERSE)))
	    {
	      /* No loop? Then what's there to sustain? No envelope needed
		 either... */
	      sp->modes &= ~(MODES_SUSTAIN|MODES_ENVELOPE);
	      SNDDBG((" - No loop, removing sustain and envelope\n"));
	    }
	  else if (!memcmp(tmp, "??????", 6) || tmp[11] >= 100) 
	    {
	      /* Envelope rates all maxed out? Envelope end at a high "offset"?
		 That's a weird envelope. Take it out. */
	      sp->modes &= ~MODES_ENVELOPE;
	      SNDDBG((" - Weirdness, removing envelope\n"));
	    }
	  else if (!(sp->modes & MODES_SUSTAIN))
	    {
	      /* No sustain? Then no envelope.  I don't know if this is
		 justified, but patches without sustain usually don't need the
		 envelope either... at least the Gravis ones. They're mostly
		 drums.  I think. */
	      sp->modes &= ~MODES_ENVELOPE;
	      SNDDBG((" - No sustain, removing envelope\n"));
	    }
	}

      for (j=0; j<6; j++)
	{
	  sp->envelope_rate[j]=
	    convert_envelope_rate(song, tmp[j]);
	  sp->envelope_offset[j]= 
	    convert_envelope_offset(tmp[6+j]);
	}

      /* Then read the sample data */
      sp->data = (sample_t *) safe_malloc(sp->data_length+4);
      if (1 != SDL_RWread(rw, sp->data, sp->data_length, 1))
	goto fail;
      
      if (!(sp->modes & MODES_16BIT)) /* convert to 16-bit data */
	{
	  Sint32 k=sp->data_length;
	  Uint8 *cp=(Uint8 *)(sp->data);
	  Uint16 *tmp16,*new16;
	  sp->data_length *= 2;
	  sp->loop_start *= 2;
	  sp->loop_end *= 2;
	  tmp16 = new16 = (Uint16 *) safe_malloc(sp->data_length+4);
	  while (k--)
	    *tmp16++ = (Uint16)(*cp++) << 8;
	  free(sp->data);
	  sp->data = (sample_t *)new16;
	}
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
      else
	/* convert to machine byte order */
	{
	  Sint32 k=sp->data_length/2;
	  Sint16 *tmp16=(Sint16 *)sp->data,s;
	  while (k--)
	    {
	      s=SDL_SwapLE16(*tmp16);
	      *tmp16++=s;
	    }
	}
#endif
      
      if (sp->modes & MODES_UNSIGNED) /* convert to signed data */
	{
	  Sint32 k=sp->data_length/2;
	  Sint16 *tmp16=(Sint16 *)sp->data;
	  while (k--)
	    *tmp16++ ^= 0x8000;
	}

      /* Reverse reverse loops and pass them off as normal loops */
      if (sp->modes & MODES_REVERSE)
	{
	  Sint32 t;
	  /* The GUS apparently plays reverse loops by reversing the
	     whole sample. We do the same because the GUS does not SUCK. */

	  SNDDBG(("Reverse loop in %s\n", name));
	  reverse_data((Sint16 *)sp->data, 0, sp->data_length/2);

	  t=sp->loop_start;
	  sp->loop_start=sp->data_length - sp->loop_end;
	  sp->loop_end=sp->data_length - t;

	  sp->modes &= ~MODES_REVERSE;
	  sp->modes |= MODES_LOOPING; /* just in case */
	}

#ifdef ADJUST_SAMPLE_VOLUMES
      if (amp!=-1)
	sp->volume=(float)((amp) / 100.0);
      else
	{
	  /* Try to determine a volume scaling factor for the sample.
	     This is a very crude adjustment, but things sound more
	     balanced with it. Still, this should be a runtime option. */
	  Sint32 k=sp->data_length/2;
	  Sint16 maxamp=0,a;
	  Sint16 *tmp16=(Sint16 *)sp->data;
	  while (k--)
	    {
	      a=*tmp16++;
	      if (a<0) a=-a;
	      if (a>maxamp)
		maxamp=a;
	    }
	  sp->volume=(float)(32768.0 / maxamp);
	  SNDDBG((" * volume comp: %f\n", sp->volume));
	}
#else
      if (amp!=-1)
	sp->volume=(double)(amp) / 100.0;
      else
	sp->volume=1.0;
#endif

      sp->data_length /= 2; /* These are in bytes. Convert into samples. */
      sp->loop_start /= 2;
      sp->loop_end /= 2;

      /* initialize the added extra sample space (see the +4 bytes in
	 allocation) using the last actual sample:  */
      sp->data[sp->data_length] = sp->data[sp->data_length+1] = 0;

      /* Then fractional samples */
      sp->data_length <<= FRACTION_BITS;
      sp->loop_start <<= FRACTION_BITS;
      sp->loop_end <<= FRACTION_BITS;

      /* Adjust for fractional loop points. This is a guess. Does anyone
	 know what "fractions" really stands for? */
      sp->loop_start |=
	(fractions & 0x0F) << (FRACTION_BITS-4);
      sp->loop_end |=
	((fractions>>4) & 0x0F) << (FRACTION_BITS-4);

      /* If this instrument will always be played on the same note,
	 and it's not looped, we can resample it now. */
      if (sp->note_to_use && !(sp->modes & MODES_LOOPING))
	pre_resample(song, sp);

      if (strip_tail==1)
	{
	  /* Let's not really, just say we did. */
	  SNDDBG((" - Stripping tail\n"));
	  sp->data_length = sp->loop_end;
	}
    }

  SDL_RWclose(rw);
  return ip;
}

static int fill_bank(MidiSong *song, int dr, int b)
{
  int i, errors=0;
  ToneBank *bank=((dr) ? song->drumset[b] : song->tonebank[b]);
  if (!bank)
    {
      SNDDBG(("Huh. Tried to load instruments in non-existent %s %d\n",
	   (dr) ? "drumset" : "tone bank", b));
      return 0;
    }
  for (i=0; i<MAXBANK; i++)
    {
      if (bank->instrument[i]==MAGIC_LOAD_INSTRUMENT)
	{
	  if (!(bank->tone[i].name))
	    {
	      SNDDBG(("No instrument mapped to %s %d, program %d%s\n",
		   (dr)? "drum set" : "tone bank", b, i, 
		   (b!=0) ? "" : " - this instrument will not be heard"));
	      if (b!=0)
		{
		  /* Mark the corresponding instrument in the default
		     bank / drumset for loading (if it isn't already) */
		  if (!dr)
		    {
		      if (!(song->tonebank[0]->instrument[i]))
			song->tonebank[0]->instrument[i] =
			  MAGIC_LOAD_INSTRUMENT;
		    }
		  else
		    {
		      if (!(song->drumset[0]->instrument[i]))
			song->drumset[0]->instrument[i] =
			  MAGIC_LOAD_INSTRUMENT;
		    }
		}
	      bank->instrument[i] = 0;
	      errors++;
	    }
	  else if (!(bank->instrument[i] =
		     load_instrument(song,
				     bank->tone[i].name, 
				     (dr) ? 1 : 0,
				     bank->tone[i].pan,
				     bank->tone[i].amp,
				     (bank->tone[i].note!=-1) ? 
				     bank->tone[i].note :
				     ((dr) ? i : -1),
				     (bank->tone[i].strip_loop!=-1) ?
				     bank->tone[i].strip_loop :
				     ((dr) ? 1 : -1),
				     (bank->tone[i].strip_envelope != -1) ? 
				     bank->tone[i].strip_envelope :
				     ((dr) ? 1 : -1),
				     bank->tone[i].strip_tail )))
	    {
	      SNDDBG(("Couldn't load instrument %s (%s %d, program %d)\n",
		   bank->tone[i].name,
		   (dr)? "drum set" : "tone bank", b, i));
	      errors++;
	    }
	}
    }
  return errors;
}

int load_missing_instruments(MidiSong *song)
{
  int i=MAXBANK,errors=0;
  while (i--)
    {
      if (song->tonebank[i])
	errors+=fill_bank(song,0,i);
      if (song->drumset[i])
	errors+=fill_bank(song,1,i);
    }
  return errors;
}

void free_instruments(MidiSong *song)
{
  int i=MAXBANK;
  while(i--)
    {
      if (song->tonebank[i])
	free_bank(song, 0, i);
      if (song->drumset[i])
	free_bank(song, 1, i);
    }
}

int set_default_instrument(MidiSong *song, char *name)
{
  Instrument *ip;
  if (!(ip=load_instrument(song, name, 0, -1, -1, -1, 0, 0, 0)))
    return -1;
  song->default_instrument = ip;
  song->default_program = SPECIAL_PROGRAM;
  return 0;
}
