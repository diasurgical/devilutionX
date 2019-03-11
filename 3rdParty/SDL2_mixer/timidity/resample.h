/*

    TiMidity -- Experimental MIDI to WAVE converter
    Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the Perl Artistic License, available in COPYING.

    resample.h
*/

extern sample_t *resample_voice(MidiSong *song, int v, Sint32 *countptr);
extern void pre_resample(MidiSong *song, Sample *sp);
