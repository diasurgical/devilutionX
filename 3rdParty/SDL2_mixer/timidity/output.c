/* 

    TiMidity -- Experimental MIDI to WAVE converter
    Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the Perl Artistic License, available in COPYING.

    output.c
    
    Audio output (to file / device) functions.
*/

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include "SDL.h"

#include "options.h"
#include "output.h"

/*****************************************************************/
/* Some functions to convert signed 32-bit data to other formats */

void s32tos8(void *dp, Sint32 *lp, Sint32 c)
{
  Sint8 *cp=(Sint8 *)(dp);
  Sint32 l;
  while (c--)
    {
      l=(*lp++)>>(32-8-GUARD_BITS);
      if (l>127) l=127;
      else if (l<-128) l=-128;
      *cp++ = (Sint8) (l);
    }
}

void s32tou8(void *dp, Sint32 *lp, Sint32 c)
{
  Uint8 *cp=(Uint8 *)(dp);
  Sint32 l;
  while (c--)
    {
      l=(*lp++)>>(32-8-GUARD_BITS);
      if (l>127) l=127;
      else if (l<-128) l=-128;
      *cp++ = 0x80 ^ ((Uint8) l);
    }
}

void s32tos16(void *dp, Sint32 *lp, Sint32 c)
{
  Sint16 *sp=(Sint16 *)(dp);
  Sint32 l;
  while (c--)
    {
      l=(*lp++)>>(32-16-GUARD_BITS);
      if (l > 32767) l=32767;
      else if (l<-32768) l=-32768;
      *sp++ = (Sint16)(l);
    }
}

void s32tou16(void *dp, Sint32 *lp, Sint32 c)
{
  Uint16 *sp=(Uint16 *)(dp);
  Sint32 l;
  while (c--)
    {
      l=(*lp++)>>(32-16-GUARD_BITS);
      if (l > 32767) l=32767;
      else if (l<-32768) l=-32768;
      *sp++ = 0x8000 ^ (Uint16)(l);
    }
}

void s32tos16x(void *dp, Sint32 *lp, Sint32 c)
{
  Sint16 *sp=(Sint16 *)(dp);
  Sint32 l;
  while (c--)
    {
      l=(*lp++)>>(32-16-GUARD_BITS);
      if (l > 32767) l=32767;
      else if (l<-32768) l=-32768;
      *sp++ = SDL_Swap16((Sint16)(l));
    }
}

void s32tou16x(void *dp, Sint32 *lp, Sint32 c)
{
  Uint16 *sp=(Uint16 *)(dp);
  Sint32 l;
  while (c--)
    {
      l=(*lp++)>>(32-16-GUARD_BITS);
      if (l > 32767) l=32767;
      else if (l<-32768) l=-32768;
      *sp++ = SDL_Swap16(0x8000 ^ (Uint16)(l));
    }
}

void s32tof32(void *dp, Sint32 *lp, Sint32 c)
{
  float *sp=(float *)(dp);
  while (c--)
    {
      *sp++ = (float)(*lp++) / 2147483647.0f;
    }
}

void s32tos32(void *dp, Sint32 *lp, Sint32 c)
{
  Sint32 *sp=(Sint32 *)(dp);
  while (c--)
    {
      *sp++ = (*lp++);
    }
}

void s32tos32x(void *dp, Sint32 *lp, Sint32 c)
{
  Sint32 *sp=(Sint32 *)(dp);
  while (c--)
    {
      *sp++ = SDL_Swap32(*lp++);
    }
}
