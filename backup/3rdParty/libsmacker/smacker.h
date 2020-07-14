/**
	libsmacker - A C library for decoding .smk Smacker Video files
	Copyright (C) 2012-2017 Greg Kennedy

	libsmacker is a cross-platform C library which can be used for
	decoding Smacker Video files produced by RAD Game Tools.

	This software is released under the following license:
	Creative Commons Attribution-NonCommercial 2.0 (CC BY-NC 2.0)

	You are free:
	* to Share - to copy, distribute and transmit the work
	* to Remix - to adapt the work

	Under the following conditions:

	* Attribution - You must attribute the work in the manner specified by
		the author or licensor (but not in any way that suggests that
		they endorse you or your use of the work).
	* Noncommercial - You may not use this work for commercial purposes.

	This is a human-readable summary of the Legal Code (the full license).

	You should have received a copy of the full license
		along with libsmacker.  If not, see
		<http://creativecommons.org/licenses/by-nc/2.0>.
*/

#ifndef SMACKER_H
#define SMACKER_H

/* includes - needed for FILE* here */
#include <stdio.h>

/** forward-declaration for an struct */
typedef struct smk_t* smk;

/** a few defines as return codes from smk_next() */
#define SMK_DONE	0x00
#define SMK_MORE	0x01
#define SMK_LAST	0x02
#define SMK_ERROR	-1

/** file-processing mode, pass to smk_open_file */
#define SMK_MODE_DISK	0x00
#define SMK_MODE_MEMORY	0x01

/** Y-scale meanings */
#define	SMK_FLAG_Y_NONE	0x00
#define	SMK_FLAG_Y_INTERLACE	0x01
#define	SMK_FLAG_Y_DOUBLE	0x02

/** track mask and enable bits */
#define	SMK_AUDIO_TRACK_0	0x01
#define	SMK_AUDIO_TRACK_1	0x02
#define	SMK_AUDIO_TRACK_2	0x04
#define	SMK_AUDIO_TRACK_3	0x08
#define	SMK_AUDIO_TRACK_4	0x10
#define	SMK_AUDIO_TRACK_5	0x20
#define	SMK_AUDIO_TRACK_6	0x40
#define	SMK_VIDEO_TRACK	0x80

/* PUBLIC FUNCTIONS */
#ifdef __cplusplus
extern "C" {
#endif

/* OPEN OPERATIONS */
/** open an smk (from a file) */
smk smk_open_file(const char* filename, unsigned char mode);
/** open an smk (from a file pointer) */
smk smk_open_filepointer(FILE* file, unsigned char mode);
/** read an smk (from a memory buffer) */
smk smk_open_memory(const unsigned char* buffer, unsigned long size);

/* CLOSE OPERATIONS */
/** close out an smk file and clean up memory */
void smk_close(smk object);

/* GET FILE INFO OPERATIONS */
char smk_info_all(const smk object, unsigned long* frame, unsigned long* frame_count, double* usf);
char smk_info_video(const smk object, unsigned long* w, unsigned long* h, unsigned char* y_scale_mode);
char smk_info_audio(const smk object, unsigned char* track_mask, unsigned char channels[7], unsigned char bitdepth[7], unsigned long audio_rate[7]);

/* ENABLE/DISABLE Switches */
char smk_enable_all(smk object, unsigned char mask);
char smk_enable_video(smk object, unsigned char enable);
char smk_enable_audio(smk object, unsigned char track, unsigned char enable);

/** Retrieve palette */
const unsigned char* smk_get_palette(const smk object);
/** Retrieve video frame, as a buffer of size w*h */
const unsigned char* smk_get_video(const smk object);
/** Retrieve decoded audio chunk, track N */
const unsigned char* smk_get_audio(const smk object, unsigned char track);
/** Get size of currently pointed decoded audio chunk, track N */
unsigned long smk_get_audio_size(const smk object, unsigned char track);

/** rewind to first frame and unpack */
char smk_first(smk object);
/** was the pallet updated this frame */
unsigned char smk_palette_updated(smk s);
/** advance to next frame and unpack */
char smk_next(smk object);
/** seek to first keyframe before/at N in an smk */
char smk_seek_keyframe(smk object, unsigned long frame);

#ifdef __cplusplus
}
#endif

#endif
