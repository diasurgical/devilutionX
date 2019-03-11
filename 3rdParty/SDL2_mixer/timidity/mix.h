/*

    TiMidity -- Experimental MIDI to WAVE converter
    Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the Perl Artistic License, available in COPYING.

    mix.h

*/

extern void mix_voice(MidiSong *song, Sint32 *buf, int v, Sint32 c);
extern int recompute_envelope(MidiSong *song, int v);
extern void apply_envelope_to_amp(MidiSong *song, int v);
