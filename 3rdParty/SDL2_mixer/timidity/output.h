/* 

    TiMidity -- Experimental MIDI to WAVE converter
    Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the Perl Artistic License, available in COPYING.

    output.h

*/

/* Data format encoding bits */

#define PE_MONO 	0x01  /* versus stereo */
#define PE_SIGNED	0x02  /* versus unsigned */
#define PE_16BIT 	0x04  /* versus 8-bit */
#define PE_32BIT 	0x08  /* versus 8-bit or 16-bit */

/* Conversion functions -- These overwrite the Sint32 data in *lp with
   data in another format */

/* 8-bit signed and unsigned*/
extern void s32tos8(void *dp, Sint32 *lp, Sint32 c);
extern void s32tou8(void *dp, Sint32 *lp, Sint32 c);

/* 16-bit */
extern void s32tos16(void *dp, Sint32 *lp, Sint32 c);
extern void s32tou16(void *dp, Sint32 *lp, Sint32 c);

/* byte-exchanged 16-bit */
extern void s32tos16x(void *dp, Sint32 *lp, Sint32 c);
extern void s32tou16x(void *dp, Sint32 *lp, Sint32 c);

/* 32-bit */
extern void s32tof32(void *dp, Sint32 *lp, Sint32 c);
extern void s32tos32(void *dp, Sint32 *lp, Sint32 c);

/* byte-exchanged 32-bit */
extern void s32tos32x(void *dp, Sint32 *lp, Sint32 c);

/* little-endian and big-endian specific */
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define s32tos16l s32tos16
#define s32tos16b s32tos16x
#define s32tou16l s32tou16
#define s32tou16b s32tou16x
#define s32tos32l s32tos32
#define s32tos32b s32tos32x
#else
#define s32tos16l s32tos16x
#define s32tos16b s32tos16
#define s32tou16l s32tou16x
#define s32tou16b s32tou16
#define s32tos32l s32tos32x
#define s32tos32b s32tos32
#endif
