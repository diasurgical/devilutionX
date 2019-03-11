/*
	audio: audio output interface

	copyright ?-2016 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Michael Hipp
*/

#include <errno.h>
#include "mpg123app.h"
#include "audio.h"
#include "out123.h"
#include "common.h"
#include "sysutil.h"

#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#include "debug.h"

mpg123_string* audio_enclist(void)
{
	int i;
	mpg123_string *list;
	size_t enc_count = 0;
	const int *enc_codes = NULL;

	/* Only the encodings supported by libmpg123 build
	   Those returned by out123_enc_list() are a superset. */
	mpg123_encodings(&enc_codes, &enc_count);
	if((list = malloc(sizeof(*list))))
		mpg123_init_string(list);
	/* Further calls to mpg123 string lib are hardened against NULL. */
	for(i=0;i<enc_count;++i)
	{
		if(i>0)
			mpg123_add_string(list, " ");
		mpg123_add_string(list, out123_enc_name(enc_codes[i]));
	}
	return list;
}

static void capline(mpg123_handle *mh, long rate)
{
	int enci;
	const int  *encs;
	size_t      num_encs;
	mpg123_encodings(&encs, &num_encs);
	fprintf(stderr," %5ld |", pitch_rate(rate));
	for(enci=0; enci<num_encs; ++enci)
	{
		switch(mpg123_format_support(mh, rate, encs[enci]))
		{
			case MPG123_MONO:               fprintf(stderr, "   M   |"); break;
			case MPG123_STEREO:             fprintf(stderr, "   S   |"); break;
			case MPG123_MONO|MPG123_STEREO: fprintf(stderr, "  M/S  |"); break;
			default:                        fprintf(stderr, "       |");
		}
	}
	fprintf(stderr, "\n");
}

void print_capabilities(out123_handle *ao, mpg123_handle *mh)
{
	int r,e;
	const long *rates;
	size_t      num_rates;
	const int  *encs;
	size_t      num_encs;
	char *name;
	char *dev;
	out123_driver_info(ao, &name, &dev);
	mpg123_rates(&rates, &num_rates);
	mpg123_encodings(&encs, &num_encs);
	fprintf(stderr,"\nAudio driver: %s\nAudio device: %s\nAudio capabilities:\n(matrix of [S]tereo or [M]ono support for sample format and rate in Hz)\n       |", name, dev);
	for(e=0;e<num_encs;e++)
	{
		const char *encname = out123_enc_name(encs[e]);
		fprintf(stderr," %5s |", encname ? encname : "???");
	}

	fprintf(stderr,"\n ------|");
	for(e=0;e<num_encs;e++) fprintf(stderr,"-------|");

	fprintf(stderr, "\n");
	for(r=0; r<num_rates; ++r) capline(mh, rates[r]);

	if(param.force_rate) capline(mh, param.force_rate);

	fprintf(stderr,"\n");
}

/* Quick-shot paired table setup with remembering search in it.
   this is for storing pairs of output sampling rate and decoding
   sampling rate. */
struct ratepair { long a; long b; };

long brate(struct ratepair *table, long arate, int count, int *last)
{
	int i = 0;
	int j;
	for(j=0; j<2; ++j)
	{
		i = i ? 0 : *last;
		for(; i<count; ++i) if(table[i].a == arate)
		{
			*last = i;
			return table[i].b;
		}
	}
	return 0;
}

/* This uses the currently opened audio device, queries its caps.
   In case of buffered playback, this works _once_ by querying the buffer for the caps before entering the main loop. */
void audio_capabilities(out123_handle *ao, mpg123_handle *mh)
{
	int force_fmt = 0;
	size_t ri;
	/* Pitching introduces a difference between decoder rate and playback rate. */
	long decode_rate;
	const long *rates;
	long *outrates;
	struct ratepair *unpitch;
	struct mpg123_fmt *outfmts = NULL;
	int fmtcount;
	size_t num_rates, rlimit;

	debug("audio_capabilities");
	mpg123_rates(&rates, &num_rates);

	mpg123_format_none(mh); /* Start with nothing. */

	if(param.force_encoding != NULL)
	{
		if(!param.quiet)
			fprintf(stderr, "Note: forcing output encoding %s\n", param.force_encoding);

		force_fmt = out123_enc_byname(param.force_encoding);
		if(!force_fmt)
		{
			error1("Failed to find an encoding to match requested \"%s\"!\n"
			,	param.force_encoding);
			return; /* No capabilities at all... */
		}
		else if(param.verbose > 2)
			fprintf(stderr, "Note: forcing encoding code 0x%x (%s)\n"
			,	force_fmt, out123_enc_name(force_fmt));
	}
	/* Lots of preparation of rate lists. */
	rlimit = param.force_rate > 0 ? num_rates+1 : num_rates;
	outrates = malloc(sizeof(*rates)*rlimit);
	unpitch  = malloc(sizeof(*unpitch)*rlimit);
	if(!outrates || !unpitch)
	{
		if(!param.quiet)
			error("DOOM");
		return;
	}
	for(ri = 0; ri<rlimit; ri++)
	{
		decode_rate = ri < num_rates ? rates[ri] : param.force_rate;
		outrates[ri] = pitch_rate(decode_rate);
		unpitch[ri].a = outrates[ri];
		unpitch[ri].b = decode_rate;
	}
	/* Actually query formats possible with given rates. */
	fmtcount = out123_formats(ao, outrates, rlimit, 1, 2, &outfmts);
	free(outrates);
	if(fmtcount > 0)
	{
		int fi;
		int unpitch_i = 0;
		if(param.verbose > 1 && outfmts[0].encoding > 0)
		{
			const char *encname = out123_enc_name(outfmts[0].encoding);
			fprintf(stderr, "Note: default format %li Hz, %i channels, %s\n"
			,	outfmts[0].rate, outfmts[0].channels
			,	encname ? encname : "???" );
		}
		for(fi=1; fi<fmtcount; ++fi)
		{
			int fmts = outfmts[fi].encoding;
			if(param.verbose > 2)
				fprintf( stderr
				,	"Note: output support for %li Hz, %i channels: 0x%x\n"
				,	outfmts[fi].rate, outfmts[fi].channels, outfmts[fi].encoding );
			if(force_fmt)
			{ /* Filter for forced encoding. */
				if((fmts & force_fmt) == force_fmt)
					fmts = force_fmt;
				else /* Nothing else! */
					fmts = 0;
			}
			mpg123_format( mh
			,	brate(unpitch, outfmts[fi].rate, rlimit, &unpitch_i)
			,	outfmts[fi].channels, fmts );
		}
	}
	free(outfmts);
	free(unpitch);

	if(param.verbose > 1) print_capabilities(ao, mh);
}

int set_pitch(mpg123_handle *fr, out123_handle *ao, double new_pitch)
{
	double old_pitch = param.pitch;
	long rate;
	int channels, format;
	int smode = 0;

	/* Be safe, check support. */
	if(mpg123_getformat(fr, &rate, &channels, &format) != MPG123_OK)
	{
		/* We might just not have a track handy. */
		error("There is no current audio format, cannot apply pitch. This might get fixed in future.");
		return 0;
	}

	param.pitch = new_pitch;
	if(param.pitch < -0.99) param.pitch = -0.99;

	if(channels == 1) smode = MPG123_MONO;
	if(channels == 2) smode = MPG123_STEREO;

	out123_stop(ao);
	/* Remember: This takes param.pitch into account. */
	audio_capabilities(ao, fr);
	if(!(mpg123_format_support(fr, rate, format) & smode))
	{
		/* Note: When using --pitch command line parameter, you can go higher
		   because a lower decoder sample rate is automagically chosen.
		   Here, we'd need to switch decoder rate during track... good? */
		error("Reached a hardware limit there with pitch!");
		param.pitch = old_pitch;
		audio_capabilities(ao, fr);
	}
	return out123_start(ao, pitch_rate(rate), channels, format);
}
