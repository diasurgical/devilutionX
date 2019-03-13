/*

    TiMidity -- Experimental MIDI to WAVE converter
    Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the Perl Artistic License, available in COPYING.

    mix.c */

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#include "timidity.h"
#include "options.h"
#include "instrum.h"
#include "playmidi.h"
#include "output.h"
#include "tables.h"
#include "resample.h"
#include "mix.h"

/* Returns 1 if envelope runs out */
int recompute_envelope(MidiSong *song, int v)
{
  int stage;

  stage = song->voice[v].envelope_stage;

  if (stage>5)
    {
      /* Envelope ran out. */
      song->voice[v].status = VOICE_FREE;
      return 1;
    }

  if (song->voice[v].sample->modes & MODES_ENVELOPE)
    {
      if (song->voice[v].status==VOICE_ON || song->voice[v].status==VOICE_SUSTAINED)
	{
	  if (stage>2)
	    {
	      /* Freeze envelope until note turns off. Trumpets want this. */
	      song->voice[v].envelope_increment=0;
	      return 0;
	    }
	}
    }
  song->voice[v].envelope_stage=stage+1;

  if (song->voice[v].envelope_volume==song->voice[v].sample->envelope_offset[stage] ||
      (stage > 2 && song->voice[v].envelope_volume <
       song->voice[v].sample->envelope_offset[stage]))
    return recompute_envelope(song, v);
  song->voice[v].envelope_target = song->voice[v].sample->envelope_offset[stage];
  song->voice[v].envelope_increment = song->voice[v].sample->envelope_rate[stage];
  if (song->voice[v].envelope_target < song->voice[v].envelope_volume)
    song->voice[v].envelope_increment = -song->voice[v].envelope_increment;
  return 0;
}

void apply_envelope_to_amp(MidiSong *song, int v)
{
  float lamp = song->voice[v].left_amp, ramp;
  Sint32 la,ra;
  if (song->voice[v].panned == PANNED_MYSTERY)
    {
      ramp = song->voice[v].right_amp;
      if (song->voice[v].tremolo_phase_increment)
	{
	  lamp *= song->voice[v].tremolo_volume;
	  ramp *= song->voice[v].tremolo_volume;
	}
      if (song->voice[v].sample->modes & MODES_ENVELOPE)
	{
	  lamp *= (float)vol_table[song->voice[v].envelope_volume>>23];
	  ramp *= (float)vol_table[song->voice[v].envelope_volume>>23];
	}

      la = (Sint32)FSCALE(lamp,AMP_BITS);
      
      if (la>MAX_AMP_VALUE)
	la=MAX_AMP_VALUE;

      ra = (Sint32)FSCALE(ramp,AMP_BITS);
      if (ra>MAX_AMP_VALUE)
	ra=MAX_AMP_VALUE;
      
      song->voice[v].left_mix = la;
      song->voice[v].right_mix = ra;
    }
  else
    {
      if (song->voice[v].tremolo_phase_increment)
	lamp *= song->voice[v].tremolo_volume;
      if (song->voice[v].sample->modes & MODES_ENVELOPE)
	lamp *= (float)vol_table[song->voice[v].envelope_volume>>23];

      la = (Sint32)FSCALE(lamp,AMP_BITS);

      if (la>MAX_AMP_VALUE)
	la=MAX_AMP_VALUE;

      song->voice[v].left_mix = la;
    }
}

static int update_envelope(MidiSong *song, int v)
{
  song->voice[v].envelope_volume += song->voice[v].envelope_increment;
  /* Why is there no ^^ operator?? */
  if (((song->voice[v].envelope_increment < 0) &&
       (song->voice[v].envelope_volume <= song->voice[v].envelope_target)) ||
      ((song->voice[v].envelope_increment > 0) &&
	   (song->voice[v].envelope_volume >= song->voice[v].envelope_target)))
    {
      song->voice[v].envelope_volume = song->voice[v].envelope_target;
      if (recompute_envelope(song, v))
	return 1;
    }
  return 0;
}

static void update_tremolo(MidiSong *song, int v)
{
  Sint32 depth = song->voice[v].sample->tremolo_depth << 7;

  if (song->voice[v].tremolo_sweep)
    {
      /* Update sweep position */

      song->voice[v].tremolo_sweep_position += song->voice[v].tremolo_sweep;
      if (song->voice[v].tremolo_sweep_position >= (1 << SWEEP_SHIFT))
	song->voice[v].tremolo_sweep=0; /* Swept to max amplitude */
      else
	{
	  /* Need to adjust depth */
	  depth *= song->voice[v].tremolo_sweep_position;
	  depth >>= SWEEP_SHIFT;
	}
    }

  song->voice[v].tremolo_phase += song->voice[v].tremolo_phase_increment;

  /* if (song->voice[v].tremolo_phase >= (SINE_CYCLE_LENGTH<<RATE_SHIFT))
     song->voice[v].tremolo_phase -= SINE_CYCLE_LENGTH<<RATE_SHIFT;  */

  song->voice[v].tremolo_volume = (float) 
    (1.0 - FSCALENEG((sine(song->voice[v].tremolo_phase >> RATE_SHIFT) + 1.0)
		    * depth * TREMOLO_AMPLITUDE_TUNING,
		    17));

  /* I'm not sure about the +1.0 there -- it makes tremoloed voices'
     volumes on average the lower the higher the tremolo amplitude. */
}

/* Returns 1 if the note died */
static int update_signal(MidiSong *song, int v)
{
  if (song->voice[v].envelope_increment && update_envelope(song, v))
    return 1;

  if (song->voice[v].tremolo_phase_increment)
    update_tremolo(song, v);

  apply_envelope_to_amp(song, v);
  return 0;
}

#define MIXATION(a)	*lp++ += (a)*s;

static void mix_mystery_signal(MidiSong *song, sample_t *sp, Sint32 *lp, int v,
			       int count)
{
  Voice *vp = song->voice + v;
  final_volume_t 
    left=vp->left_mix, 
    right=vp->right_mix;
  int cc;
  sample_t s;

  if (!(cc = vp->control_counter))
    {
      cc = song->control_ratio;
      if (update_signal(song, v))
	return;	/* Envelope ran out */
      left = vp->left_mix;
      right = vp->right_mix;
    }
  
  while (count)
    if (cc < count)
      {
	count -= cc;
	while (cc--)
	  {
	    s = *sp++;
	    MIXATION(left);
	    MIXATION(right);
	  }
	cc = song->control_ratio;
	if (update_signal(song, v))
	  return;	/* Envelope ran out */
	left = vp->left_mix;
	right = vp->right_mix;
      }
    else
      {
	vp->control_counter = cc - count;
	while (count--)
	  {
	    s = *sp++;
	    MIXATION(left);
	    MIXATION(right);
	  }
	return;
      }
}

static void mix_center_signal(MidiSong *song, sample_t *sp, Sint32 *lp, int v,
			      int count)
{
  Voice *vp = song->voice + v;
  final_volume_t 
    left=vp->left_mix;
  int cc;
  sample_t s;

  if (!(cc = vp->control_counter))
    {
      cc = song->control_ratio;
      if (update_signal(song, v))
	return;	/* Envelope ran out */
      left = vp->left_mix;
    }
  
  while (count)
    if (cc < count)
      {
	count -= cc;
	while (cc--)
	  {
	    s = *sp++;
	    MIXATION(left);
	    MIXATION(left);
	  }
	cc = song->control_ratio;
	if (update_signal(song, v))
	  return;	/* Envelope ran out */
	left = vp->left_mix;
      }
    else
      {
	vp->control_counter = cc - count;
	while (count--)
	  {
	    s = *sp++;
	    MIXATION(left);
	    MIXATION(left);
	  }
	return;
      }
}

static void mix_single_signal(MidiSong *song, sample_t *sp, Sint32 *lp, int v,
			      int count)
{
  Voice *vp = song->voice + v;
  final_volume_t 
    left=vp->left_mix;
  int cc;
  sample_t s;
  
  if (!(cc = vp->control_counter))
    {
      cc = song->control_ratio;
      if (update_signal(song, v))
	return;	/* Envelope ran out */
      left = vp->left_mix;
    }
  
  while (count)
    if (cc < count)
      {
	count -= cc;
	while (cc--)
	  {
	    s = *sp++;
	    MIXATION(left);
	    lp++;
	  }
	cc = song->control_ratio;
	if (update_signal(song, v))
	  return;	/* Envelope ran out */
	left = vp->left_mix;
      }
    else
      {
	vp->control_counter = cc - count;
	while (count--)
	  {
	    s = *sp++;
	    MIXATION(left);
	    lp++;
	  }
	return;
      }
}

static void mix_mono_signal(MidiSong *song, sample_t *sp, Sint32 *lp, int v,
			    int count)
{
  Voice *vp = song->voice + v;
  final_volume_t 
    left=vp->left_mix;
  int cc;
  sample_t s;
  
  if (!(cc = vp->control_counter))
    {
      cc = song->control_ratio;
      if (update_signal(song, v))
	return;	/* Envelope ran out */
      left = vp->left_mix;
    }
  
  while (count)
    if (cc < count)
      {
	count -= cc;
	while (cc--)
	  {
	    s = *sp++;
	    MIXATION(left);
	  }
	cc = song->control_ratio;
	if (update_signal(song, v))
	  return;	/* Envelope ran out */
	left = vp->left_mix;
      }
    else
      {
	vp->control_counter = cc - count;
	while (count--)
	  {
	    s = *sp++;
	    MIXATION(left);
	  }
	return;
      }
}

static void mix_mystery(MidiSong *song, sample_t *sp, Sint32 *lp, int v, int count)
{
  final_volume_t 
    left = song->voice[v].left_mix, 
    right = song->voice[v].right_mix;
  sample_t s;
  
  while (count--)
    {
      s = *sp++;
      MIXATION(left);
      MIXATION(right);
    }
}

static void mix_center(MidiSong *song, sample_t *sp, Sint32 *lp, int v, int count)
{
  final_volume_t 
    left = song->voice[v].left_mix;
  sample_t s;
  
  while (count--)
    {
      s = *sp++;
      MIXATION(left);
      MIXATION(left);
    }
}

static void mix_single(MidiSong *song, sample_t *sp, Sint32 *lp, int v, int count)
{
  final_volume_t 
    left = song->voice[v].left_mix;
  sample_t s;
  
  while (count--)
    {
      s = *sp++;
      MIXATION(left);
      lp++;
    }
}

static void mix_mono(MidiSong *song, sample_t *sp, Sint32 *lp, int v, int count)
{
  final_volume_t 
    left = song->voice[v].left_mix;
  sample_t s;
  
  while (count--)
    {
      s = *sp++;
      MIXATION(left);
    }
}

/* Ramp a note out in c samples */
static void ramp_out(MidiSong *song, sample_t *sp, Sint32 *lp, int v, Sint32 c)
{

  /* should be final_volume_t, but Uint8 gives trouble. */
  Sint32 left, right, li, ri;

  sample_t s=0; /* silly warning about uninitialized s */

  left=song->voice[v].left_mix;
  li=-(left/c);
  if (!li) li=-1;

  /* printf("Ramping out: left=%d, c=%d, li=%d\n", left, c, li); */

  if (!(song->encoding & PE_MONO))
    {
      if (song->voice[v].panned==PANNED_MYSTERY)
	{
	  right=song->voice[v].right_mix;
	  ri=-(right/c);
	  while (c--)
	    {
	      left += li;
	      if (left<0)
		left=0;
	      right += ri;
	      if (right<0)
		right=0;
	      s=*sp++;
	      MIXATION(left);
	      MIXATION(right);
	    }
	}
      else if (song->voice[v].panned==PANNED_CENTER)
	{
	  while (c--)
	    {
	      left += li;
	      if (left<0)
		return;
	      s=*sp++;	
	      MIXATION(left);
	      MIXATION(left);
	    }
	}
      else if (song->voice[v].panned==PANNED_LEFT)
	{
	  while (c--)
	    {
	      left += li;
	      if (left<0)
		return;
	      s=*sp++;
	      MIXATION(left);
	      lp++;
	    }
	}
      else if (song->voice[v].panned==PANNED_RIGHT)
	{
	  while (c--)
	    {
	      left += li;
	      if (left<0)
		return;
	      s=*sp++;
	      lp++;
	      MIXATION(left);
	    }
	}
    }
  else
    {
      /* Mono output.  */
      while (c--)
	{
	  left += li;
	  if (left<0)
	    return;
	  s=*sp++;
	  MIXATION(left);
	}
    }
}


/**************** interface function ******************/

void mix_voice(MidiSong *song, Sint32 *buf, int v, Sint32 c)
{
  Voice *vp = song->voice + v;
  sample_t *sp;
  if (vp->status==VOICE_DIE)
    {
      if (c>=MAX_DIE_TIME)
	c=MAX_DIE_TIME;
      sp=resample_voice(song, v, &c);
      if(c > 0)
	ramp_out(song, sp, buf, v, c);
      vp->status=VOICE_FREE;
    }
  else
    {
      sp=resample_voice(song, v, &c);
      if (song->encoding & PE_MONO)
	{
	  /* Mono output. */
	  if (vp->envelope_increment || vp->tremolo_phase_increment)
	    mix_mono_signal(song, sp, buf, v, c);
	  else
	    mix_mono(song, sp, buf, v, c);
	}
      else
	{
	  if (vp->panned == PANNED_MYSTERY)
	    {
	      if (vp->envelope_increment || vp->tremolo_phase_increment)
		mix_mystery_signal(song, sp, buf, v, c);
	      else
		mix_mystery(song, sp, buf, v, c);
	    }
	  else if (vp->panned == PANNED_CENTER)
	    {
	      if (vp->envelope_increment || vp->tremolo_phase_increment)
		mix_center_signal(song, sp, buf, v, c);
	      else
		mix_center(song, sp, buf, v, c);
	    }
	  else
	    { 
	      /* It's either full left or full right. In either case,
		 every other sample is 0. Just get the offset right: */
	      if (vp->panned == PANNED_RIGHT) buf++;
	      
	      if (vp->envelope_increment || vp->tremolo_phase_increment)
		mix_single_signal(song, sp, buf, v, c);
	      else 
		mix_single(song, sp, buf, v, c);
	    }
	}
    }
}
