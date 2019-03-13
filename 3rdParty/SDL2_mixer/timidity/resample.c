/*

    TiMidity -- Experimental MIDI to WAVE converter
    Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the Perl Artistic License, available in COPYING.

    resample.c
*/

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#include "timidity.h"
#include "options.h"
#include "common.h"
#include "instrum.h"
#include "playmidi.h"
#include "tables.h"
#include "resample.h"

#define PRECALC_LOOP_COUNT(start, end, incr) (((end) - (start) + (incr) - 1) / (incr))

/*************** resampling with fixed increment *****************/

static sample_t *rs_plain(MidiSong *song, int v, Sint32 *countptr)
{

  /* Play sample until end, then free the voice. */

  sample_t v1, v2;
  Voice 
    *vp=&(song->voice[v]);
  sample_t 
    *dest=song->resample_buffer,
    *src=vp->sample->data;
  Sint32 
    ofs=vp->sample_offset,
    incr=vp->sample_increment,
    le=vp->sample->data_length,
    count=*countptr;
  Sint32 i, j;

  if (incr<0) incr = -incr; /* In case we're coming out of a bidir loop */

  /* Precalc how many times we should go through the loop.
     NOTE: Assumes that incr > 0 and that ofs <= le */
  i = PRECALC_LOOP_COUNT(ofs, le, incr);

  if (i > count)
    {
      i = count;
      count = 0;
    } 
  else count -= i;

  for (j = 0; j < i; j++)
    {
      v1 = src[ofs >> FRACTION_BITS];
      v2 = src[(ofs >> FRACTION_BITS)+1];
      *dest++ = v1 + (((v2 - v1) * (ofs & FRACTION_MASK)) >> FRACTION_BITS);
      ofs += incr;
    }

  if (ofs >= le) 
    {
      if (ofs == le)
	*dest++ = src[(ofs>>FRACTION_BITS)-1]/2;
      vp->status=VOICE_FREE;
      *countptr-=count+1;
    }
  
  vp->sample_offset=ofs; /* Update offset */
  return song->resample_buffer;
}

static sample_t *rs_loop(MidiSong *song, Voice *vp, Sint32 count)
{

  /* Play sample until end-of-loop, skip back and continue. */

  sample_t v1, v2;
  Sint32 
    ofs=vp->sample_offset, 
    incr=vp->sample_increment,
    le=vp->sample->loop_end, 
    ll=le - vp->sample->loop_start;
  sample_t
    *dest=song->resample_buffer,
    *src=vp->sample->data;
  Sint32 i, j;
  
  while (count) 
    {
      while (ofs >= le)
	ofs -= ll;
      /* Precalc how many times we should go through the loop */
      i = PRECALC_LOOP_COUNT(ofs, le, incr);
      if (i > count) 
	{
	  i = count;
	  count = 0;
	} 
      else count -= i;
      for (j = 0; j < i; j++)
	{
          v1 = src[ofs >> FRACTION_BITS];
          v2 = src[(ofs >> FRACTION_BITS)+1];
          *dest++ = v1 + (((v2 - v1) * (ofs & FRACTION_MASK)) >> FRACTION_BITS);
	  ofs += incr;
	}
    }

  vp->sample_offset=ofs; /* Update offset */
  return song->resample_buffer;
}

static sample_t *rs_bidir(MidiSong *song, Voice *vp, Sint32 count)
{
  sample_t v1, v2;
  Sint32 
    ofs=vp->sample_offset,
    incr=vp->sample_increment,
    le=vp->sample->loop_end,
    ls=vp->sample->loop_start;
  sample_t 
    *dest=song->resample_buffer, 
    *src=vp->sample->data;
  Sint32
    le2 = le<<1,
    ls2 = ls<<1,
    i, j;
  /* Play normally until inside the loop region */

  if (incr > 0 && ofs < ls)
    {
      /* NOTE: Assumes that incr > 0, which is NOT always the case
	 when doing bidirectional looping.  I have yet to see a case
	 where both ofs <= ls AND incr < 0, however. */
      i = PRECALC_LOOP_COUNT(ofs, ls, incr);
      if (i > count) 
	{
	  i = count;
	  count = 0;
	} 
      else count -= i;
      for (j = 0; j < i; j++)
	{
          v1 = src[ofs >> FRACTION_BITS];
          v2 = src[(ofs >> FRACTION_BITS)+1];
          *dest++ = v1 + (((v2 - v1) * (ofs & FRACTION_MASK)) >> FRACTION_BITS);
	  ofs += incr;
	}
    }

  /* Then do the bidirectional looping */
  
  while(count) 
    {
      /* Precalc how many times we should go through the loop */
      i = PRECALC_LOOP_COUNT(ofs, incr > 0 ? le : ls, incr);
      if (i > count) 
	{
	  i = count;
	  count = 0;
	} 
      else count -= i;
      for (j = 0; j < i; j++)
	{
          v1 = src[ofs >> FRACTION_BITS];
          v2 = src[(ofs >> FRACTION_BITS)+1];
          *dest++ = v1 + (((v2 - v1) * (ofs & FRACTION_MASK)) >> FRACTION_BITS);
	  ofs += incr;
	}
      if (ofs>=le) 
	{
	  /* fold the overshoot back in */
	  ofs = le2 - ofs;
	  incr *= -1;
	} 
      else if (ofs <= ls) 
	{
	  ofs = ls2 - ofs;
	  incr *= -1;
	}
    }

  vp->sample_increment=incr;
  vp->sample_offset=ofs; /* Update offset */
  return song->resample_buffer;
}

/*********************** vibrato versions ***************************/

/* We only need to compute one half of the vibrato sine cycle */
static int vib_phase_to_inc_ptr(int phase)
{
  if (phase < VIBRATO_SAMPLE_INCREMENTS/2)
    return VIBRATO_SAMPLE_INCREMENTS/2-1-phase;
  else if (phase >= 3*VIBRATO_SAMPLE_INCREMENTS/2)
    return 5*VIBRATO_SAMPLE_INCREMENTS/2-1-phase;
  else
    return phase-VIBRATO_SAMPLE_INCREMENTS/2;
}

static Sint32 update_vibrato(MidiSong *song, Voice *vp, int sign)
{
  Sint32 depth;
  int phase, pb;
  double a;

  if (vp->vibrato_phase++ >= 2*VIBRATO_SAMPLE_INCREMENTS-1)
    vp->vibrato_phase=0;
  phase=vib_phase_to_inc_ptr(vp->vibrato_phase);
  
  if (vp->vibrato_sample_increment[phase])
    {
      if (sign)
	return -vp->vibrato_sample_increment[phase];
      else
	return vp->vibrato_sample_increment[phase];
    }

  /* Need to compute this sample increment. */
    
  depth=vp->sample->vibrato_depth<<7;

  if (vp->vibrato_sweep)
    {
      /* Need to update sweep */
      vp->vibrato_sweep_position += vp->vibrato_sweep;
      if (vp->vibrato_sweep_position >= (1<<SWEEP_SHIFT))
	vp->vibrato_sweep=0;
      else
	{
	  /* Adjust depth */
	  depth *= vp->vibrato_sweep_position;
	  depth >>= SWEEP_SHIFT;
	}
    }

  a = FSCALE(((double)(vp->sample->sample_rate) *
	      (double)(vp->frequency)) /
	     ((double)(vp->sample->root_freq) *
	      (double)(song->rate)),
	     FRACTION_BITS);

  pb=(int)((sine(vp->vibrato_phase * 
		 (SINE_CYCLE_LENGTH/(2*VIBRATO_SAMPLE_INCREMENTS)))
	    * (double)(depth) * VIBRATO_AMPLITUDE_TUNING));

  if (pb<0)
    {
      pb=-pb;
      a /= bend_fine[(pb>>5) & 0xFF] * bend_coarse[pb>>13];
    }
  else
    a *= bend_fine[(pb>>5) & 0xFF] * bend_coarse[pb>>13];
  
  /* If the sweep's over, we can store the newly computed sample_increment */
  if (!vp->vibrato_sweep)
    vp->vibrato_sample_increment[phase]=(Sint32) a;

  if (sign)
    a = -a; /* need to preserve the loop direction */

  return (Sint32) a;
}

static sample_t *rs_vib_plain(MidiSong *song, int v, Sint32 *countptr)
{

  /* Play sample until end, then free the voice. */

  sample_t v1, v2;
  Voice *vp=&(song->voice[v]);
  sample_t 
    *dest=song->resample_buffer, 
    *src=vp->sample->data;
  Sint32 
    le=vp->sample->data_length,
    ofs=vp->sample_offset, 
    incr=vp->sample_increment, 
    count=*countptr;
  int 
    cc=vp->vibrato_control_counter;

  /* This has never been tested */

  if (incr<0) incr = -incr; /* In case we're coming out of a bidir loop */

  while (count--)
    {
      if (!cc--)
	{
	  cc=vp->vibrato_control_ratio;
	  incr=update_vibrato(song, vp, 0);
	}
      v1 = src[ofs >> FRACTION_BITS];
      v2 = src[(ofs >> FRACTION_BITS)+1];
      *dest++ = v1 + (((v2 - v1) * (ofs & FRACTION_MASK)) >> FRACTION_BITS);
      ofs += incr;
      if (ofs >= le)
	{
	  if (ofs == le)
	    *dest++ = src[(ofs>>FRACTION_BITS)-1]/2;
	  vp->status=VOICE_FREE;
	  *countptr-=count+1;
	  break;
	}
    }
  
  vp->vibrato_control_counter=cc;
  vp->sample_increment=incr;
  vp->sample_offset=ofs; /* Update offset */
  return song->resample_buffer;
}

static sample_t *rs_vib_loop(MidiSong *song, Voice *vp, Sint32 count)
{

  /* Play sample until end-of-loop, skip back and continue. */
  
  sample_t v1, v2;
  Sint32 
    ofs=vp->sample_offset, 
    incr=vp->sample_increment, 
    le=vp->sample->loop_end,
    ll=le - vp->sample->loop_start;
  sample_t 
    *dest=song->resample_buffer, 
    *src=vp->sample->data;
  int 
    cc=vp->vibrato_control_counter;
  Sint32 i, j;
  int
    vibflag=0;

  while (count) 
    {
      /* Hopefully the loop is longer than an increment */
      while(ofs >= le)
	ofs -= ll;
      /* Precalc how many times to go through the loop, taking
	 the vibrato control ratio into account this time. */
      i = PRECALC_LOOP_COUNT(ofs, le, incr);
      if(i > count) i = count;
      if(i > cc)
	{
	  i = cc;
	  vibflag = 1;
	} 
      else cc -= i;
      count -= i;
      for (j = 0; j < i; j++)
	{
          v1 = src[ofs >> FRACTION_BITS];
          v2 = src[(ofs >> FRACTION_BITS)+1];
          *dest++ = v1 + (((v2 - v1) * (ofs & FRACTION_MASK)) >> FRACTION_BITS);
	  ofs += incr;
	}
      if(vibflag) 
	{
	  cc = vp->vibrato_control_ratio;
	  incr = update_vibrato(song, vp, 0);
	  vibflag = 0;
	}
    }

  vp->vibrato_control_counter=cc;
  vp->sample_increment=incr;
  vp->sample_offset=ofs; /* Update offset */
  return song->resample_buffer;
}

static sample_t *rs_vib_bidir(MidiSong *song, Voice *vp, Sint32 count)
{
  sample_t v1, v2;
  Sint32 
    ofs=vp->sample_offset, 
    incr=vp->sample_increment,
    le=vp->sample->loop_end, 
    ls=vp->sample->loop_start;
  sample_t 
    *dest=song->resample_buffer, 
    *src=vp->sample->data;
  int 
    cc=vp->vibrato_control_counter;
  Sint32
    le2=le<<1,
    ls2=ls<<1,
    i, j;
  int
    vibflag = 0;

  /* Play normally until inside the loop region */
  while (count && incr > 0 && ofs < ls)
    {
      i = PRECALC_LOOP_COUNT(ofs, ls, incr);
      if (i > count) i = count;
      if (i > cc) 
	{
	  i = cc;
	  vibflag = 1;
	} 
      else cc -= i;
      count -= i;
      for (j = 0; j < i; j++)
	{
          v1 = src[ofs >> FRACTION_BITS];
          v2 = src[(ofs >> FRACTION_BITS)+1];
          *dest++ = v1 + (((v2 - v1) * (ofs & FRACTION_MASK)) >> FRACTION_BITS);
	  ofs += incr;
	}
      if (vibflag) 
	{
	  cc = vp->vibrato_control_ratio;
	  incr = update_vibrato(song, vp, 0);
	  vibflag = 0;
	}
    }
  
  /* Then do the bidirectional looping */

  while (count) 
    {
      /* Precalc how many times we should go through the loop */
      i = PRECALC_LOOP_COUNT(ofs, incr > 0 ? le : ls, incr);
      if(i > count) i = count;
      if(i > cc) 
	{
	  i = cc;
	  vibflag = 1;
	} 
      else cc -= i;
      count -= i;
      while (i--) 
	{
          v1 = src[ofs >> FRACTION_BITS];
          v2 = src[(ofs >> FRACTION_BITS)+1];
          *dest++ = v1 + (((v2 - v1) * (ofs & FRACTION_MASK)) >> FRACTION_BITS);
	  ofs += incr;
	}
      if (vibflag) 
	{
	  cc = vp->vibrato_control_ratio;
	  incr = update_vibrato(song, vp, (incr < 0));
	  vibflag = 0;
	}
      if (ofs >= le) 
	{
	  /* fold the overshoot back in */
	  ofs = le2 - ofs;
	  incr *= -1;
	} 
      else if (ofs <= ls) 
	{
	  ofs = ls2 - ofs;
	  incr *= -1;
	}
    }

  vp->vibrato_control_counter=cc;
  vp->sample_increment=incr;
  vp->sample_offset=ofs; /* Update offset */
  return song->resample_buffer;
}

sample_t *resample_voice(MidiSong *song, int v, Sint32 *countptr)
{
  Sint32 ofs;
  Uint8 modes;
  Voice *vp=&(song->voice[v]);
  
  if (!(vp->sample->sample_rate))
    {
      /* Pre-resampled data -- just update the offset and check if
         we're out of data. */
      ofs=vp->sample_offset >> FRACTION_BITS; /* Kind of silly to use
						 FRACTION_BITS here... */
      if (*countptr >= (vp->sample->data_length>>FRACTION_BITS) - ofs)
	{
	  /* Note finished. Free the voice. */
	  vp->status = VOICE_FREE;
	  
	  /* Let the caller know how much data we had left */
	  *countptr = (vp->sample->data_length>>FRACTION_BITS) - ofs;
	}
      else
	vp->sample_offset += *countptr << FRACTION_BITS;
      
      return vp->sample->data+ofs;
    }

  /* Need to resample. Use the proper function. */
  modes=vp->sample->modes;

  if (vp->vibrato_control_ratio)
    {
      if ((modes & MODES_LOOPING) &&
	  ((modes & MODES_ENVELOPE) ||
	   (vp->status==VOICE_ON || vp->status==VOICE_SUSTAINED)))
	{
	  if (modes & MODES_PINGPONG)
	    return rs_vib_bidir(song, vp, *countptr);
	  else
	    return rs_vib_loop(song, vp, *countptr);
	}
      else
	return rs_vib_plain(song, v, countptr);
    }
  else
    {
      if ((modes & MODES_LOOPING) &&
	  ((modes & MODES_ENVELOPE) ||
	   (vp->status==VOICE_ON || vp->status==VOICE_SUSTAINED)))
	{
	  if (modes & MODES_PINGPONG)
	    return rs_bidir(song, vp, *countptr);
	  else
	    return rs_loop(song, vp, *countptr);
	}
      else
	return rs_plain(song, v, countptr);
    }
}

void pre_resample(MidiSong *song, Sample *sp)
{
  double a, xdiff;
  Sint32 incr, ofs, newlen, count;
  Sint16 *newdata, *dest, *src = (Sint16 *) sp->data, *vptr;
  Sint32 v, v1, v2, v3, v4, v5, i;
#ifdef DEBUG_CHATTER
  static const char note_name[12][3] =
  {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
  };

  SNDDBG((" * pre-resampling for note %d (%s%d)\n",
	  sp->note_to_use,
	  note_name[sp->note_to_use % 12], (sp->note_to_use & 0x7F) / 12));
#endif

  a = ((double) (sp->root_freq) * song->rate) /
      ((double) (sp->sample_rate) * freq_table[(int) (sp->note_to_use)]);
  if(sp->data_length * a >= 0x7fffffffL) { /* Too large to compute */
    SNDDBG((" *** Can't pre-resampling for note %d\n", sp->note_to_use));
    return;
  }

  newlen = (Sint32)(sp->data_length * a);
  count = (newlen >> FRACTION_BITS) - 1;
  ofs = incr = (sp->data_length - (1 << FRACTION_BITS)) / count;

  if((double)newlen + incr >= 0x7fffffffL) { /* Too large to compute */
    SNDDBG((" *** Can't pre-resampling for note %d\n", sp->note_to_use));
    return;
  }

  dest = newdata = (Sint16 *) safe_malloc((newlen >> (FRACTION_BITS - 1)) + 2);
  if (!dest)
    return;

  if (--count)
    *dest++ = src[0];

  /* Since we're pre-processing and this doesn't have to be done in
     real-time, we go ahead and do the full sliding cubic interpolation. */
  count--;
  for(i = 0; i < count; i++)
    {
      vptr = src + (ofs >> FRACTION_BITS);
      v1 = ((vptr>=src+1)? *(vptr - 1):0);
      v2 = *vptr;
      v3 = *(vptr + 1);
      v4 = *(vptr + 2);
      v5 = v2 - v3;
      xdiff = FSCALENEG(ofs & FRACTION_MASK, FRACTION_BITS);
      v = (Sint32)(v2 + xdiff * (1.0/6.0) * (3 * (v3 - v5) - 2 * v1 - v4 +
		xdiff * (3 * (v1 - v2 - v5) + xdiff * (3 * v5 + v4 - v1))));
      *dest++ = (Sint16)((v > 32767) ? 32767 : ((v < -32768) ? -32768 : v));
      ofs += incr;
    }

  if (ofs & FRACTION_MASK)
    {
      v1 = src[ofs >> FRACTION_BITS];
      v2 = src[(ofs >> FRACTION_BITS) + 1];
      *dest++ = (Sint16)(v1 + (((v2 - v1) * (ofs & FRACTION_MASK)) >> FRACTION_BITS));
    }
  else
    *dest++ = src[ofs >> FRACTION_BITS];

  *dest = *(dest - 1) / 2;
 ++dest;
  *dest = *(dest - 1) / 2;

  sp->data_length = newlen;
  sp->loop_start = (Sint32)(sp->loop_start * a);
  sp->loop_end = (Sint32)(sp->loop_end * a);
  free(sp->data);
  sp->data = (sample_t *) newdata;
  sp->sample_rate = 0;
}
