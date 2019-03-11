/*

    TiMidity -- Experimental MIDI to WAVE converter
    Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the Perl Artistic License, available in COPYING.
*/

#ifndef TIMIDITY_H
#define TIMIDITY_H
#ifdef __cplusplus
extern "C" {
#endif

typedef Sint16 sample_t;
typedef Sint32 final_volume_t;

#define VIBRATO_SAMPLE_INCREMENTS 32

/* Maximum polyphony. */
/* #define MAX_VOICES	48 */
#define MAX_VOICES	256
#define MAXCHAN	16
/* #define MAXCHAN	64 */
#define MAXBANK	128

typedef struct {
  Sint32
    loop_start, loop_end, data_length,
    sample_rate, low_freq, high_freq, root_freq;
  Sint32
    envelope_rate[6], envelope_offset[6];
  float
    volume;
  sample_t *data;
  Sint32 
    tremolo_sweep_increment, tremolo_phase_increment, 
    vibrato_sweep_increment, vibrato_control_ratio;
  Uint8
    tremolo_depth, vibrato_depth,
    modes;
  Sint8
    panning, note_to_use;
} Sample;

typedef struct {
  int
    bank, program, volume, sustain, panning, pitchbend, expression, 
    mono, /* one note only on this channel -- not implemented yet */
    pitchsens;
  /* chorus, reverb... Coming soon to a 300-MHz, eight-way superscalar
     processor near you */
  float
    pitchfactor; /* precomputed pitch bend factor to save some fdiv's */
} Channel;

typedef struct {
  Uint8
    status, channel, note, velocity;
  Sample *sample;
  Sint32
    orig_frequency, frequency,
    sample_offset, sample_increment,
    envelope_volume, envelope_target, envelope_increment,
    tremolo_sweep, tremolo_sweep_position,
    tremolo_phase, tremolo_phase_increment,
    vibrato_sweep, vibrato_sweep_position;
  
  final_volume_t left_mix, right_mix;

  float
    left_amp, right_amp, tremolo_volume;
  Sint32
    vibrato_sample_increment[VIBRATO_SAMPLE_INCREMENTS];
  int
    vibrato_phase, vibrato_control_ratio, vibrato_control_counter,
    envelope_stage, control_counter, panning, panned;

} Voice;

typedef struct {
  int samples;
  Sample *sample;
} Instrument;

/* Shared data */
typedef struct {
  char *name;
  int note, amp, pan, strip_loop, strip_envelope, strip_tail;
} ToneBankElement;

typedef struct {
  ToneBankElement *tone;
  Instrument *instrument[128];
} ToneBank;

typedef struct {
    Sint32 time;
    Uint8 channel, type, a, b;
} MidiEvent;

typedef struct {
    MidiEvent event;
    void *next;
} MidiEventList;

typedef struct {
    int playing;
    SDL_RWops *rw;
    Sint32 rate;
    Sint32 encoding;
    float master_volume;
    Sint32 amplification;
    ToneBank *tonebank[MAXBANK];
    ToneBank *drumset[MAXBANK];
    Instrument *default_instrument;
    int default_program;
    void (*write)(void *dp, Sint32 *lp, Sint32 c);
    int buffer_size;
    sample_t *resample_buffer;
    Sint32 *common_buffer;
    Sint32 *buffer_pointer;
    /* These would both fit into 32 bits, but they are often added in
       large multiples, so it's simpler to have two roomy ints */
    /* samples per MIDI delta-t */
    Sint32 sample_increment;
    Sint32 sample_correction;
    Channel channel[MAXCHAN];
    Voice voice[MAX_VOICES];
    int voices;
    Sint32 drumchannels;
    Sint32 buffered_count;
    Sint32 control_ratio;
    Sint32 lost_notes;
    Sint32 cut_notes;
    Sint32 samples;
    MidiEvent *events;
    MidiEvent *current_event;
    MidiEventList *evlist;
    Sint32 current_sample;
    Sint32 event_count;
    Sint32 at;
    Sint32 groomed_event_count;
} MidiSong;

/* Some of these are not defined in timidity.c but are here for convenience */

extern int Timidity_Init(void);
extern int Timidity_Init_NoConfig(void);
extern void Timidity_SetVolume(MidiSong *song, int volume);
extern int Timidity_PlaySome(MidiSong *song, void *stream, Sint32 len);
extern MidiSong *Timidity_LoadSong(SDL_RWops *rw, SDL_AudioSpec *audio);
extern void Timidity_Start(MidiSong *song);
extern void Timidity_Seek(MidiSong *song, Uint32 ms);
extern Uint32 Timidity_GetSongLength(MidiSong *song); /* returns millseconds */
extern void Timidity_FreeSong(MidiSong *song);
extern void Timidity_Exit(void);

#ifdef __cplusplus
}
#endif
#endif /* TIMIDITY_H */
