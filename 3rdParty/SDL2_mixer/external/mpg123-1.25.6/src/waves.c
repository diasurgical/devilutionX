/*
	waves: some oscillators, for fun

	copyright 2017 by the mpg123 project, license: LGPL 2.1

	This code does not care about efficiency constructing the period buffer,
	as that is done only once at the beginning. Double precision
	computations should be fine with software emulation.

	There might be some slight pulsing from aliasing inside the buffer, where
	there is a fractional number of samples per period and so some shidting
	of sample point locations.
*/

#include "waves.h"
#include "debug.h"

/* Not depending on C99 math for these simple things, */

static const double freq_error = 1e-4;
static const double twopi = 2.0*3.14159265358979323846;

/* absolute value */
static double myabs(double a)
{
	return a < 0 ? -a : a;
}

/* round floating point to size_t */
static size_t round2size(double a)
{
	return a < 0 ? 0 : (size_t)(a+0.5);
}

/* fractional part, relating to frequencies (so long matches) */
static double myfrac(double a)
{
	return a-(long)a;
}

/*
	Given a set of wave frequencies, compute an approximate common
	period for the combined signal. Invalid frequencies are set to
	the error bound for some sanity.
*/
static double common_samples_per_period( long rate, size_t count
,	double *freq, size_t size_limit )
{
	double spp = 0;
	size_t i;
	for(i=0; i<count; ++i)
	{
		double sppi;
		size_t periods = 1;
		/* Limiting sensible frequency range. */
		if(freq[i] < freq_error)
			freq[i] = freq_error;
		if(freq[i] > rate/2)
			freq[i] = rate/2;
		sppi = myabs((double)rate/freq[i]);
		debug2("freq=%g sppi=%g", freq[i], sppi);
		if(spp == 0)
			spp = sppi;
		while
		(
			periods*spp < size_limit &&
			myabs( myfrac(periods*spp / sppi) ) > freq_error
		)
			periods++;
		spp*=periods;
		debug3( "samples_per_period + %f Hz = %g (%" SIZE_P " periods)"
		,	freq[i], spp, periods );
	}
	return spp;
}

/* Compute a good size of a table covering the common period for all waves. */
static size_t tablesize( long rate, size_t count
,	double *freq, size_t size_limit )
{
	size_t ts;
	double samples_per_period;
	size_t periods;

	samples_per_period = common_samples_per_period( rate, count
	,	freq, size_limit );
	periods = 1;
	while
	(
		myabs( (double)(ts =
			round2size(periods*samples_per_period))
			- periods*samples_per_period
		) / periods > freq_error*samples_per_period
		&& periods*samples_per_period < size_limit
	)
		periods++;
	/* Ensure that we got an even number of samples to accomodate the minimal */
	/* sampling of a period. */
	ts += ts % 2;
	debug1("table size: %" SIZE_P, ts);
	return ts;
}

/* The wave functions. Argument is the phase normalised to the period. */
/* The argument is guaranteed to be 0 <= p < 1. */

const char *wave_pattern_default = "sine";
const char *wave_pattern_list = "sine, square, triangle, sawtooth, gauss, pulse, shot";

/* _________ */
/*           */
static double wave_none(double p)
{
	return 0;
}

/*   __       */
/*  /  \      */
/*      \__/  */
static double wave_sine(double p)
{
	return sin(twopi*p);
}

/*      ___   */
/*  ___|      */
static double wave_square(double p)
{
	return (myfrac(p) < 0.5 ? -1 : 1);
}

/*    1234    Avoid jump from zero at beginning. */
/*    /\      */
/*      \/    */
static double wave_triangle(double p)
{
	return 4*p < 1
	?	4*p        /* 1 */
	:	( 4*p < 3
		?	2.-4*p  /* 2 and 3 */
		:	-4+4*p  /* 4 */
		);
}

/*   /|    Avoid jump from zero ... */
/*    |/   */
static double wave_sawtooth(double p)
{
	return 2*p < 1 ? 2*p : -2+2*p;
}

/*    _    */
/* __/ \__ */
/*         */
static double wave_gauss(double p)
{
	double v = p-0.5;
	return exp(-30*v*v);
}

/*    _      */
/*  _/ -___  */
/*           */
/* p**2*exp(-a*p**2) */
/* Scaling: maximum at sqrt(1/a), value 1/a*exp(-1). */
static double wave_pulse(double p)
{
	return p*p*exp(-50*p*p)/0.00735758882342885;
}

/*  _     */
/* / -___ */
/*        */
/* p**2*exp(-a*p) */
/* Scaling: maximum at 4/a, value 4/a**2*exp(-2). */
static double wave_shot(double p)
{
	return p*p*exp(-100*p)/5.41341132946451e-05;
}

/* Fill table with given periodic wave function. */
static void add_table( double *table, size_t samples
, long rate, double *freq, const char *wavetype, double phase)
{
	size_t i, periods;
	double spp, phaseoff;
	double (*wave)(double);
	debug3("add_table %" SIZE_P " %ld %g", samples, rate, *freq);
	periods = round2size((double)samples*(*freq)/rate);
	if(!periods)
		periods = 1;
	spp     = (double)samples/periods;
	/* 2 samples is absolute minimum. */
	if(spp < 2)
		spp = 2;

	/* The actual frequency after the rounding of samples per period. */
	*freq = (double)rate/spp;

	/* Center samples in period, somewhat, to ensure getting extrema with very */
	/* small sample counts (instead of zero at beginning and center). */
	phaseoff = 1./(2*spp);

	if(!wavetype)
		wave = wave_none;
	else if(!strcasecmp(wavetype, "sine"))
		wave = wave_sine;
	else if(!strcasecmp(wavetype, "square"))
		wave = wave_square;
	else if(!strcasecmp(wavetype, "triangle"))
		wave = wave_triangle;
	else if(!strcasecmp(wavetype, "sawtooth"))
		wave = wave_sawtooth;
	else if(!strcasecmp(wavetype, "gauss"))
		wave = wave_gauss;
	else if(!strcasecmp(wavetype, "pulse"))
		wave = wave_pulse;
	else if(!strcasecmp(wavetype, "shot"))
		wave = wave_shot;
	else
		wave = wave_none;

	debug3( "adding wave: %s @ %g Hz + %g"
	,	wavetype ? wavetype : "none", *freq, phase );

	/*
		compromise: smooth onset for low frequencies, but also good sampling
		of high freqs extreme case: 2 samples per period should trigger
		max/min amplitude, not both zero so, center the samples within one
		period. That's achieved by adding 0.25 to the counter.
	*/
	if(phase >= 0)
		for(i=0; i<samples; ++i)
			table[i] *= wave(myfrac( ((double)i+phaseoff)/spp + phase));
	else
		for(i=0; i<samples; ++i)
			table[i] *= wave(1.-myfrac( ((double)i+phaseoff)/spp - phase));
}

/* Construct the prototype table in double precision. */
static double* mytable( long rate, size_t count, double *freq
,	const char** wavetype, double *phase
,	size_t size_limit, size_t *samples)
{
	size_t i;
	double *table = NULL;

	*samples = tablesize(rate, count, freq, size_limit);
	debug1("computed table size: %" SIZE_P, *samples);
	table = malloc(*samples*sizeof(double));
	if(!table)
	{
		error("OOM!");
		return NULL;
	}
	/* Initialise to zero. */
	for(i=0; i<*samples; ++i)
		table[i] = 1;
	/* Add individual waves, with default parameters. */
	for(i=0; i<count; ++i)
		add_table(
			table, *samples, rate, freq+i
		,	wavetype ? wavetype[i] : wave_pattern_default
		,	phase    ? phase[i]    : 0
		);
	/* Amplification could have caused clipping. */
	for(i=0; i<*samples; ++i)
	{
		if(table[i] >  1.0)
			table[i] =  1.0;
		if(table[i] < -1.0)
			table[i] = -1.0;
		debug2("table[%" SIZE_P "]=%f", i, table[i]);
	}
	return table;
}

/* Some trivial conversion functions. No need for a library. */

/* All symmetric, +/- 2^n-1. */
#define CONV(name, type, maxval) \
static type name(double d) \
{ \
	type imax = maxval; \
	d *= imax; \
	if(d>=0) \
	{ \
		d += 0.5; \
		return d > imax \
		?	imax \
		:	(type)d; \
	} \
	else \
	{ \
		d -= 0.5; \
		return d < -imax \
		?	-imax \
		:	(type)d; \
	} \
}

CONV(d2s32, int32_t, 2147483647L)
CONV(d2s16, int16_t, 32767)
CONV(d2s8,   char,   127)

#define HANDLE_OOM(p, h, p2) \
if(!(p)) \
{ \
	error("OOM!"); \
	if((p2)) free((p2)); \
	return wave_table_del((h)); \
}

/* Build internal table, allocate external table, convert to that one, */
/* adjusting sample storage format and channel count. */
struct wave_table* wave_table_new(
	long rate, int channels, int encoding
,	size_t count, double *freq
,	const char** wavetype, double *phase
,	size_t size_limit
){
	struct wave_table *handle;
	double *dtable;
	int c,i;

	handle = malloc(sizeof(struct wave_table));
	HANDLE_OOM(handle, handle, NULL)

	handle->buf = NULL;
	handle->fmt.rate = rate;
	handle->fmt.channels = channels;
	handle->fmt.encoding = encoding;
	handle->samples = 0;
	handle->offset  = 0;
	handle->count = count;
	handle->freq = NULL;

	handle->freq = malloc(sizeof(double)*count);
	HANDLE_OOM(handle->freq, handle, NULL)
	for(i=0; i<count; ++i)
		handle->freq[i] = freq[i];

	dtable = mytable( rate, count, handle->freq, wavetype, phase
	,	size_limit, &handle->samples );
	HANDLE_OOM(dtable, handle, NULL)
	handle->buf = malloc( handle->samples
	            * MPG123_SAMPLESIZE(encoding) * channels );
	HANDLE_OOM(handle->buf, handle, dtable)
	/* Now convert. */
	for(c=0; c<channels; ++c)
	{
		switch(encoding)
		{
			/* Samples are clipped. Rounding a double below 1.0 to float cannot */
			/* get larger than 1.0 since that is exact in single precision. */
			case MPG123_ENC_FLOAT_64:
				for(i=0; i<handle->samples; ++i)
					((double*)handle->buf)[i*channels+c] = dtable[i];
			break;
			case MPG123_ENC_FLOAT_32:
				for(i=0; i<handle->samples; ++i)
					((float*)handle->buf)[i*channels+c] = dtable[i];
			break;
			case MPG123_ENC_SIGNED_32:
				for(i=0; i<handle->samples; ++i)
					((int32_t*)handle->buf)[i*channels+c] = d2s32(dtable[i]);
			break;
			case MPG123_ENC_SIGNED_16:
				for(i=0; i<handle->samples; ++i)
					((int16_t*)handle->buf)[i*channels+c] = d2s16(dtable[i]);
			break;
			case MPG123_ENC_SIGNED_8:
				for(i=0; i<handle->samples; ++i)
					((char*)handle->buf)[i*channels+c] = d2s8(dtable[i]);
			break;
			default:
				error("unsupported encoding, choose f64, f32, s32, s16 or s8");
				free(dtable);
				return wave_table_del(handle);
		}
	}
	free(dtable);
	return handle;
}

void* wave_table_del(struct wave_table* handle)
{
	if(handle)
	{
		if(handle->freq)
			free(handle->freq);
		if(handle->buf)
			free(handle->buf);
		free(handle);
	}
	return NULL;
}

/* Copy blocks from offset to end until desired amount is extracted. */
size_t wave_table_extract( struct wave_table *handle
,	void *dest, size_t dest_samples )
{
	char *cdest = (char*)dest; /* Want to do arithmetic. */
	size_t framesize;
	size_t extracted = 0;

	if(!handle)
		return 0;

	framesize = MPG123_SAMPLESIZE(handle->fmt.encoding)
	          * handle->fmt.channels;
	while(dest_samples)
	{
		size_t block = dest_samples > handle->samples - handle->offset
		?	handle->samples - handle->offset
		:	dest_samples;
		debug1("block: %" SIZE_P, block);
		memcpy( cdest, (char*)handle->buf+handle->offset*framesize
		,	framesize*block );
		cdest  += framesize*block;
		handle->offset += block;
		handle->offset %= handle->samples;
		dest_samples -= block;
		extracted    += block;
	}
	debug1("extracted: %" SIZE_P, extracted);
	return extracted;
}
