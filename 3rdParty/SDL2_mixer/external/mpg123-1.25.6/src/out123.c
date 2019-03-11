/*
	out123: simple program to stream data to an audio output device

	copyright 1995-2016 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Thomas Orgis (extracted from mpg123.c)

	This is a stripped down mpg123 that only uses libout123 to write standard input
	to an audio device.

	TODO: Add basic parsing of WAV headers to be able to pipe in WAV files, especially
	from something like mpg123 -w -.
*/

#define ME "out123"
#include "config.h"
#include "compat.h"
#if WIN32
#include "win32_support.h"
#endif
#include "out123.h"

#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif

#include <errno.h>
#include <string.h>
#include <time.h>

#ifdef HAVE_SCHED_H
#include <sched.h>
#endif

#include "sysutil.h"
#include "getlopt.h"

#include "waves.h"

#include "debug.h"

/* be paranoid about setpriority support */
#ifndef PRIO_PROCESS
#undef HAVE_SETPRIORITY
#endif

static int intflag = FALSE;

static void usage(int err);
static void want_usage(char* arg);
static void long_usage(int err);
static void want_long_usage(char* arg);
static void print_title(FILE* o);
static void give_version(char* arg);

static int verbose = 0;
static int quiet = FALSE;

static FILE* input = NULL;
static char *encoding_name = NULL;
static int  encoding = MPG123_ENC_SIGNED_16;
static int  channels = 2;
static long rate     = 44100;
static char *driver = NULL;
static char *device = NULL;
size_t buffer_kb = 0;
static int realtime = FALSE;
#ifdef HAVE_WINDOWS_H
static int w32_priority = 0;
#endif
static int aggressive = FALSE;
static double preload = 0.2;
static long outflags = 0;
static long gain = -1;
static const char *name = NULL; /* Let the out123 library choose "out123". */
static double device_buffer; /* output device buffer */
long timelimit = -1;
off_t offset = 0;

char *wave_patterns = NULL;
char *wave_freqs    = NULL;
char *wave_phases   = NULL;
/* Default to around 2 MiB memory for the table. */
long wave_limit     = 300000;

size_t pcmblock = 1152; /* samples (pcm frames) we treat en bloc */
/* To be set after settling format. */
size_t pcmframe = 0;
unsigned char *audio = NULL;

/* Option to play some oscillatory test signals. */
struct wave_table *waver = NULL;

out123_handle *ao = NULL;
char *cmd_name = NULL;
/* ThOr: pointers are not TRUE or FALSE */
char *equalfile = NULL;
int fresh = TRUE;

size_t bufferblock = 4096;

int OutputDescriptor;

char *fullprogname = NULL; /* Copy of argv[0]. */
char *binpath; /* Path to myself. */

/* File-global storage of command line arguments.
   They may be needed for cleanup after charset conversion. */
static char **argv = NULL;
static int    argc = 0;

/* Drain output device/buffer, but still give the option to interrupt things. */
static void controlled_drain(void)
{
	int framesize;
	long rate;
	size_t drain_block;

	if(intflag || !out123_buffered(ao))
		return;
	if(out123_getformat(ao, &rate, NULL, NULL, &framesize))
		return;
	drain_block = 1024*framesize;
	if(!quiet)
		fprintf( stderr
		,	"\n"ME": draining buffer of %.1f s (you may interrupt)\n"
		,	(double)out123_buffered(ao)/framesize/rate );
	do {
		out123_ndrain(ao, drain_block);
	} while(!intflag && out123_buffered(ao));
}

static void safe_exit(int code)
{
	char *dummy, *dammy;

	if(!code)
		controlled_drain();
	if(intflag || code)
		out123_drop(ao);
	out123_del(ao);
#ifdef WANT_WIN32_UNICODE
	win32_cmdline_free(argc, argv); /* This handles the premature argv == NULL, too. */
#endif
	/* It's ugly... but let's just fix this still-reachable memory chunk of static char*. */
	split_dir_file("", &dummy, &dammy);
	if(fullprogname) free(fullprogname);
	if(audio) free(audio);
	wave_table_del(waver);
	exit(code);
}

static void check_fatal_output(int code)
{
	if(code)
	{
		if(!quiet)
			error2( "out123 error %i: %s"
			,	out123_errcode(ao), out123_strerror(ao) );
		safe_exit(code);
	}
}

static void set_output_module( char *arg )
{
	unsigned int i;
		
	/* Search for a colon and set the device if found */
	for(i=0; i< strlen( arg ); i++) {
		if (arg[i] == ':') {
			arg[i] = 0;
			device = &arg[i+1];
			debug1("Setting output device: %s", device);
			break;
		}	
	}
	/* Set the output module */
	driver = arg;
	debug1("Setting output module: %s", driver );
}

static void set_output_flag(int flag)
{
  if(outflags <= 0) outflags = flag;
  else outflags |= flag;
}

static void set_output_h(char *a)
{
	set_output_flag(OUT123_HEADPHONES);
}

static void set_output_s(char *a)
{
	set_output_flag(OUT123_INTERNAL_SPEAKER);
}

static void set_output_l(char *a)
{
	set_output_flag(OUT123_LINE_OUT);
}

static void set_output(char *arg)
{
	/* If single letter, it's the legacy output switch for AIX/HP/Sun.
	   If longer, it's module[:device] . If zero length, it's rubbish. */
	if(strlen(arg) <= 1) switch(arg[0])
	{
		case 'h': set_output_h(arg); break;
		case 's': set_output_s(arg); break;
		case 'l': set_output_l(arg); break;
		default:
			error1("\"%s\" is no valid output", arg);
			safe_exit(1);
	}
	else set_output_module(arg);
}

static void set_verbose (char *arg)
{
    verbose++;
}

static void set_quiet (char *arg)
{
	verbose=0;
	quiet=TRUE;
}

static void set_out_wav(char *arg)
{
	driver = "wav";
	device = arg;
}

void set_out_cdr(char *arg)
{
	driver = "cdr";
	device = arg;
}

void set_out_au(char *arg)
{
	driver = "au";
	device = arg;
}

void set_out_test(char *arg)
{
	driver = "test";
	device = NULL;
}

static void set_out_file(char *arg)
{
	driver = "raw";
	device = arg;
}

static void set_out_stdout(char *arg)
{
	driver = "raw";
	device = NULL;
}

static void set_out_stdout1(char *arg)
{
	driver = "raw";
	device = NULL;
}

#if !defined (HAVE_SCHED_SETSCHEDULER) && !defined (HAVE_WINDOWS_H)
static void realtime_not_compiled(char *arg)
{
	fprintf(stderr, ME": Option '-T / --realtime' not compiled into this binary.\n");
}
#endif

static void list_output_modules(char *arg)
{
	char **names = NULL;
	char **descr = NULL;
	int count = -1;
	out123_handle *lao;

	if((lao=out123_new()))
	{
		out123_param_string(lao, OUT123_BINDIR, binpath);
		out123_param_int(lao, OUT123_VERBOSE, verbose);
		if(quiet)
			out123_param_int(lao, OUT123_FLAGS, OUT123_QUIET);
		if((count=out123_drivers(lao, &names, &descr)) >= 0)
		{
			int i;
			for(i=0; i<count; ++i)
			{
				printf( "%-15s\t%s\n", names[i], descr[i] );
				free(names[i]);
				free(descr[i]);
			}
			free(names);
			free(descr);
		}
		out123_del(lao);
	}
	else if(!quiet)
		error("Failed to create an out123 handle.");
	exit(count >= 0 ? 0 : 1);
}

static void list_encodings(char *arg)
{
	int i;
	int enc_count = 0;
	int *enc_codes = NULL;

	enc_count = out123_enc_list(&enc_codes);
	/* All of the returned encodings have to have proper names!
	   It is a libout123 bug if not, and it should be quickly caught. */
	for(i=0;i<enc_count;++i)
		printf( "%s:\t%s\n"
		,	out123_enc_name(enc_codes[i]), out123_enc_longname(enc_codes[i]) );
	free(enc_codes);
	exit(0);
}

static int getencs(void)
{
	int encs = 0;
	out123_handle *lao;
	if(verbose)
		fprintf( stderr
		,	ME": getting supported encodings for %li Hz, %i channels\n"
		,	rate, channels );
	if((lao=out123_new()))
	{
		out123_param_int(lao, OUT123_VERBOSE, verbose);
		if(quiet)
			out123_param_int(lao, OUT123_FLAGS, OUT123_QUIET);
		if(!out123_open(lao, driver, device))
			encs = out123_encodings(lao, rate, channels);
		else if(!quiet)
			error1("cannot open driver: %s", out123_strerror(lao));
		out123_del(lao);
	}
	else if(!quiet)
		error("Failed to create an out123 handle.");
	return encs;
}

static void test_format(char *arg)
{
	int encs;
	encs = getencs();
	exit((encs & encoding) ? 0 : -1);
}

static void test_encodings(char *arg)
{
	int encs;
	encs = getencs();
	printf("%i\n", encs);
	exit(!encs);
}

static void query_format(char *arg)
{
	out123_handle *lao;

	if(verbose)
		fprintf(stderr, ME": querying default format\n");
	if((lao=out123_new()))
	{
		out123_param_int(lao, OUT123_VERBOSE, verbose);
		if(quiet)
			out123_param_int(lao, OUT123_FLAGS, OUT123_QUIET);
		if(!out123_open(lao, driver, device))
		{
			struct mpg123_fmt *fmts = NULL;
			int count;
			count = out123_formats(lao, NULL, 0, 0, 0, &fmts);
			if(count > 0 && fmts[0].encoding > 0)
			{
				const char *encname = out123_enc_name(fmts[0].encoding);
				printf( "--rate %li --channels %i --encoding %s\n"
				,	fmts[0].rate, fmts[0].channels
				,	encname ? encname : "???" );
			}
			else
			{
				if(verbose)
					fprintf(stderr, ME": no default format found\n");
			}
			free(fmts);
		}
		else if(!quiet)
			error1("cannot open driver: %s", out123_strerror(lao));
		out123_del(lao);
	}
	else if(!quiet)
		error("Failed to create an out123 handle.");
	exit(0);
}

/* Please note: GLO_NUM expects point to LONG! */
/* ThOr:
 *  Yeah, and despite that numerous addresses to int variables were 
passed.
 *  That's not good on my Alpha machine with int=32bit and long=64bit!
 *  Introduced GLO_INT and GLO_LONG as different bits to make that clear.
 *  GLO_NUM no longer exists.
 */
topt opts[] = {
	{'t', "test",        GLO_INT,  set_out_test, NULL, 0},
	{'s', "stdout",      GLO_INT,  set_out_stdout,  NULL, 0},
	{'S', "STDOUT",      GLO_INT,  set_out_stdout1, NULL, 0},
	{'O', "outfile",     GLO_ARG | GLO_CHAR, set_out_file, NULL, 0},
	{'v', "verbose",     0,        set_verbose, 0,           0},
	{'q', "quiet",       0,        set_quiet,   0,           0},
	{'m',  "mono",       GLO_INT,  0, &channels, 1},
	{0,   "stereo",      GLO_INT,  0, &channels, 2},
	{'c', "channels",    GLO_ARG | GLO_INT,  0, &channels, 0},
	{'r', "rate",        GLO_ARG | GLO_LONG, 0, &rate,  0},
	{0,   "headphones",  0,                  set_output_h, 0,0},
	{0,   "speaker",     0,                  set_output_s, 0,0},
	{0,   "lineout",     0,                  set_output_l, 0,0},
	{'o', "output",      GLO_ARG | GLO_CHAR, set_output, 0,  0},
	{0,   "list-modules",0,       list_output_modules, NULL,  0}, 
	{'a', "audiodevice", GLO_ARG | GLO_CHAR, 0, &device,  0},
#ifndef NOXFERMEM
	{'b', "buffer",      GLO_ARG | GLO_LONG, 0, &buffer_kb,  0},
	{0, "preload", GLO_ARG|GLO_DOUBLE, 0, &preload, 0},
#endif
#ifdef HAVE_SETPRIORITY
	{0,   "aggressive",	 GLO_INT,  0, &aggressive, 2},
#endif
#if defined (HAVE_SCHED_SETSCHEDULER) || defined (HAVE_WINDOWS_H)
	/* check why this should be a long variable instead of int! */
	{'T', "realtime",    GLO_INT,  0, &realtime, TRUE },
#else
	{'T', "realtime",    0,  realtime_not_compiled, 0,           0 },    
#endif
#ifdef HAVE_WINDOWS_H
	{0, "priority", GLO_ARG | GLO_INT, 0, &w32_priority, 0},
#endif
	{'w', "wav",         GLO_ARG | GLO_CHAR, set_out_wav, 0, 0 },
	{0, "cdr",           GLO_ARG | GLO_CHAR, set_out_cdr, 0, 0 },
	{0, "au",            GLO_ARG | GLO_CHAR, set_out_au, 0, 0 },
	{'?', "help",            0,  want_usage, 0,           0 },
	{0 , "longhelp" ,        0,  want_long_usage, 0,      0 },
	{0 , "version" ,         0,  give_version, 0,         0 },
	{'e', "encoding", GLO_ARG|GLO_CHAR, 0, &encoding_name, 0},
	{0, "list-encodings", 0, list_encodings, 0, 0 },
	{0, "test-format", 0, test_format, 0, 0 },
	{0, "test-encodings", 0, test_encodings, 0, 0},
	{0, "query-format", 0, query_format, 0, 0},
	{0, "name", GLO_ARG|GLO_CHAR, 0, &name, 0},
	{0, "devbuffer", GLO_ARG|GLO_DOUBLE, 0, &device_buffer, 0},
	{0, "timelimit", GLO_ARG|GLO_LONG, 0, &timelimit, 0},
	{0, "wave-pat", GLO_ARG|GLO_CHAR, 0, &wave_patterns, 0},
	{0, "wave-freq", GLO_ARG|GLO_CHAR, 0, &wave_freqs, 0},
	{0, "wave-phase", GLO_ARG|GLO_CHAR, 0, &wave_phases, 0},
	{0, "wave-limit", GLO_ARG|GLO_LONG, 0, &wave_limit, 0},
	{0, 0, 0, 0, 0, 0}
};

/* An strtok() that also returns empty tokens on multiple separators. */

static size_t mytok_count(const char *choppy)
{
	size_t count = 0;
	if(choppy)
	{
		count = 1;
		do {
			if(*choppy == ',')
				++count;
		} while(*(++choppy));
	}
	return count;
}

static char *mytok(char **choppy)
{
	char *tok;
	if(!*choppy)
		return NULL;
	tok  = *choppy;
	while(**choppy && **choppy != ',')
		++(*choppy);
	/* Another token follows if we found a separator. */
	if(**choppy == ',')
		*(*choppy)++ = 0;
	else
		*choppy = NULL; /* Nothing left. */
	return tok;
}

static void setup_wavegen(void)
{
	size_t count = 0;
	size_t i;
	double *freq = NULL;
	double *phase = NULL;
	const char **pat = NULL;

	if(wave_freqs)
	{
		char *tok;
		char *next;
		count = mytok_count(wave_freqs);
		freq = malloc(sizeof(double)*count);
		if(!freq){ error("OOM!"); safe_exit(1); }
		next = wave_freqs;
		for(i=0; i<count; ++i)
		{
			tok = mytok(&next);
			if(tok && *tok)
				freq[i] = atof(tok);
			else if(i)
				freq[i] = freq[i-1];
			else
				freq[i] = 0;
		}
	}
	else return;

	if(count && wave_patterns)
	{
		char *tok;
		char *next = wave_patterns;
		pat = malloc(sizeof(char*)*count);
		if(!pat){ error("OOM!"); safe_exit(1); }
		for(i=0; i<count; ++i)
		{
			tok = mytok(&next);
			if((tok && *tok) || i==0)
				pat[i] = tok;
			else
				pat[i] = pat[i-1];
		}
	}

	if(count && wave_phases)
	{
		char *tok;
		char *next = wave_phases;
		phase = malloc(sizeof(double)*count);
		if(!phase){ error("OOM!"); safe_exit(1); }
		for(i=0; i<count; ++i)
		{
			tok = mytok(&next);
			if(tok && *tok)
				phase[i] = atof(tok);
			else if(i)
				phase[i] = phase[i-1];
			else
				phase[i] = 0;
		}
	}

	waver = wave_table_new( rate, channels, encoding, count, freq, pat, phase
	,	(size_t)wave_limit );
	if(!waver)
	{
		error("Cannot set up wave generator.");
		safe_exit(132);
	}

	if(verbose)
	{
		fprintf(stderr, "wave table of %" SIZE_P " samples\n", waver->samples);
		/* TODO: There is a crash here! pat being optimised away ... */
		for(i=0; i<count; ++i)
			fprintf( stderr, "wave %" SIZE_P ": %s @ %g Hz (%g Hz) p %g\n"
			,	i
			,	(pat && pat[i]) ? pat[i] : wave_pattern_default
			,	freq[i], waver->freq[i]
			,	phase ? phase[i] : 0 );
	}

	if(phase)
		free(phase);
	if(pat)
		free(pat);
	if(freq)
		free(freq);
}

/* return 1 on success, 0 on failure */
int play_frame(void)
{
	size_t got_samples;
	size_t get_samples = pcmblock;
	if(timelimit >= 0)
	{
		if(offset >= timelimit)
			return 0;
		else if(timelimit < offset+get_samples)
			get_samples = (off_t)timelimit-offset;
	}
	if(waver)
		got_samples = wave_table_extract(waver, audio, get_samples);
	else
		got_samples = fread(audio, pcmframe, get_samples, input);
	/* Play what is there to play (starting with second decode_frame call!) */
	if(got_samples)
	{
		size_t got_bytes = pcmframe * got_samples;
		if(out123_play(ao, audio, got_bytes) < (int)got_bytes)
		{
			if(!quiet)
			{
				error2( "out123 error %i: %s"
				,	out123_errcode(ao), out123_strerror(ao) );
			}
			safe_exit(133);
		}
		offset += got_samples;
		return 1;
	}
	else return 0;
}

#if !defined(WIN32) && !defined(GENERIC)
static void catch_interrupt(void)
{
        intflag = TRUE;
}
#endif

int main(int sys_argc, char ** sys_argv)
{
	int result;
#if defined(WIN32)
	_setmode(STDIN_FILENO,  _O_BINARY);
#endif

#if defined (WANT_WIN32_UNICODE)
	if(win32_cmdline_utf8(&argc, &argv) != 0)
	{
		error("Cannot convert command line to UTF8!");
		safe_exit(76);
	}
#else
	argv = sys_argv;
	argc = sys_argc;
#endif

	if(!(fullprogname = compat_strdup(argv[0])))
	{
		error("OOM"); /* Out Of Memory. Don't waste bytes on that error. */
		safe_exit(1);
	}
	/* Extract binary and path, take stuff before/after last / or \ . */
	if(  (cmd_name = strrchr(fullprogname, '/')) 
	  || (cmd_name = strrchr(fullprogname, '\\')))
	{
		/* There is some explicit path. */
		cmd_name[0] = 0; /* End byte for path. */
		cmd_name++;
		binpath = fullprogname;
	}
	else
	{
		cmd_name = fullprogname; /* No path separators there. */
		binpath = NULL; /* No path at all. */
	}

	/* Get default flags. */
	{
		out123_handle *paro = out123_new();
		out123_getparam_int(paro, OUT123_FLAGS, &outflags);
		out123_del(paro);
	}

#ifdef OS2
        _wildcard(&argc,&argv);
#endif

	while ((result = getlopt(argc, argv, opts)))
	switch (result) {
		case GLO_UNKNOWN:
			fprintf (stderr, ME": invalid argument: %s\n", loptarg);
			usage(1);
		case GLO_NOARG:
			fprintf (stderr, ME": missing argument for parameter: %s\n", loptarg);
			usage(1);
	}

	if(quiet)
		verbose = 0;

	/* Ensure cleanup before we cause too much mess. */
#if !defined(WIN32) && !defined(GENERIC)
	catchsignal(SIGINT, catch_interrupt);
	catchsignal(SIGTERM, catch_interrupt);
#endif
	ao = out123_new();
	if(!ao){ error("Failed to allocate output."); exit(1); }

	if
	( 0
	||	out123_param_int(ao, OUT123_FLAGS, outflags)
	|| out123_param_float(ao, OUT123_PRELOAD, preload)
	|| out123_param_int(ao, OUT123_GAIN, gain)
	|| out123_param_int(ao, OUT123_VERBOSE, verbose)
	|| out123_param_string(ao, OUT123_NAME, name)
	|| out123_param_string(ao, OUT123_BINDIR, binpath)
	|| out123_param_float(ao, OUT123_DEVICEBUFFER, device_buffer)
	)
	{
		error("Error setting output parameters. Do you need a usage reminder?");
		usage(1);
	}
	
#ifdef HAVE_SETPRIORITY
	if(aggressive) { /* tst */
		int mypid = getpid();
		if(!quiet) fprintf(stderr, ME": Aggressively trying to increase priority.\n");
		if(setpriority(PRIO_PROCESS,mypid,-20))
			error("Failed to aggressively increase priority.\n");
	}
#endif

#if defined (HAVE_SCHED_SETSCHEDULER) && !defined (__CYGWIN__) && !defined (HAVE_WINDOWS_H)
/* Cygwin --realtime seems to fail when accessing network, using win32 set priority instead */
/* MinGW may have pthread installed, we prefer win32API */
	if(realtime)
	{  /* Get real-time priority */
		struct sched_param sp;
		if(!quiet) fprintf(stderr, ME": Getting real-time priority\n");
		memset(&sp, 0, sizeof(struct sched_param));
		sp.sched_priority = sched_get_priority_min(SCHED_FIFO);
		if (sched_setscheduler(0, SCHED_RR, &sp) == -1)
			error("Can't get realtime priority\n");
	}
#endif

/* make sure not Cygwin, it doesn't need it */
#if defined(WIN32) && defined(HAVE_WINDOWS_H)
	/* argument "3" is equivalent to realtime priority class */
	win32_set_priority(realtime ? 3 : w32_priority);
#endif

	if(encoding_name)
	{
		encoding = out123_enc_byname(encoding_name);
		if(!encoding)
		{
			error1("Unknown encoding '%s' given!\n", encoding_name);
			safe_exit(1);
		}
	}
	pcmframe = out123_encsize(encoding)*channels;
	bufferblock = pcmblock*pcmframe;
	audio = (unsigned char*) malloc(bufferblock);

	check_fatal_output(out123_set_buffer(ao, buffer_kb*1024));
	/* This needs bufferblock set! */
	check_fatal_output(out123_open(ao, driver, device));

	if(verbose)
	{
		long props = 0;
		const char *encname;
		char *realname = NULL;
		encname = out123_enc_name(encoding);
		fprintf(stderr, ME": format: %li Hz, %i channels, %s\n"
		,	rate, channels, encname ? encname : "???" );
		out123_getparam_string(ao, OUT123_NAME, &realname);
		if(realname)
			fprintf(stderr, ME": output real name: %s\n", realname);
		out123_getparam_int(ao, OUT123_PROPFLAGS, &props);
		if(props & OUT123_PROP_LIVE)
			fprintf(stderr, ME": This is a live sink.\n");
	}
	check_fatal_output(out123_start(ao, rate, channels, encoding));

	input = stdin;
	if(wave_freqs)
		setup_wavegen();

	while(play_frame() && !intflag)
	{
		/* be happy */
	}
	if(intflag) /* Make it quick! */
	{
		if(!quiet)
			fprintf(stderr, ME": Interrupted. Dropping the ball.\n");
		out123_drop(ao);
	}

	safe_exit(0); /* That closes output and restores terminal, too. */
	return 0;
}

static char* output_enclist(void)
{
	int i;
	char *list = NULL;
	char *pos;
	size_t len = 0;
	int enc_count = 0;
	int *enc_codes = NULL;

	enc_count = out123_enc_list(&enc_codes);
	for(i=0;i<enc_count;++i)
		len += strlen(out123_enc_name(enc_codes[i]));
	len += enc_count;

	if((pos = list = malloc(len))) for(i=0;i<enc_count;++i)
	{
		const char *name = out123_enc_name(enc_codes[i]);
		if(i>0)
			*(pos++) = ' ';
		strcpy(pos, name);
		pos+=strlen(name);
	}
	free(enc_codes);
	return list;
}

static void print_title(FILE *o)
{
	fprintf(o, "Simple audio output with raw PCM input\n");
	fprintf(o, "\tversion %s; derived from mpg123 by Michael Hipp and others\n", PACKAGE_VERSION);
	fprintf(o, "\tfree software (LGPL) without any warranty but with best wishes\n");
}

static void usage(int err)  /* print syntax & exit */
{
	FILE* o = stdout;
	if(err)
	{
		o = stderr; 
		fprintf(o, ME": You made some mistake in program usage... let me briefly remind you:\n\n");
	}
	print_title(o);
	fprintf(o,"\nusage: %s [option(s)] [file(s) | URL(s) | -]\n", cmd_name);
	fprintf(o,"supported options [defaults in brackets]:\n");
	fprintf(o,"   -v    increase verbosity level       -q    quiet (only print errors)\n");
	fprintf(o,"   -t    testmode (no output)           -s    write to stdout\n");
	fprintf(o,"   -w f  write output as WAV file\n");
	fprintf(o,"   -b n  output buffer: n Kbytes [0]                                  \n");
	fprintf(o,"   -r n  set samplerate [44100]\n");
	fprintf(o,"   -o m  select output module           -a d  set audio device\n");
	fprintf(o,"   -m    single-channel (mono) instead of stereo\n");
	#ifdef HAVE_SCHED_SETSCHEDULER
	fprintf(o,"   -T get realtime priority\n");
	#endif
	fprintf(o,"   -?    this help                      --version  print name + version\n");
	fprintf(o,"See the manpage out123(1) or call %s with --longhelp for more parameters and information.\n", cmd_name);
	safe_exit(err);
}

static void want_usage(char* arg)
{
	usage(0);
}

static void long_usage(int err)
{
	char *enclist;
	FILE* o = stdout;
	if(err)
	{
  	o = stderr; 
  	fprintf(o, "You made some mistake in program usage... let me remind you:\n\n");
	}
	enclist = output_enclist();
	print_title(o);
	fprintf(o,"\nusage: %s [option(s)] [file(s) | URL(s) | -]\n", cmd_name);

	fprintf(o,"        --name <n>         set instance name (p.ex. JACK client)\n");
	fprintf(o," -o <o> --output <o>       select audio output module\n");
	fprintf(o,"        --list-modules     list the available modules\n");
	fprintf(o," -a <d> --audiodevice <d>  select audio device (for files, empty or - is stdout)\n");
	fprintf(o," -s     --stdout           write raw audio to stdout (-o raw -a -)\n");
	fprintf(o," -S     --STDOUT           play AND output stream (not implemented yet)\n");
	fprintf(o," -O <f> --output <f>       raw output to given file (-o raw -a <f>)\n");
	fprintf(o," -w <f> --wav <f>          write samples as WAV file in <f> (-o wav -a <f>)\n");
	fprintf(o,"        --au <f>           write samples as Sun AU file in <f> (-o au -a <f>)\n");
	fprintf(o,"        --cdr <f>          write samples as raw CD audio file in <f> (-o cdr -a <f>)\n");
	fprintf(o," -r <r> --rate <r>         set the audio output rate in Hz (default 44100)\n");
	fprintf(o," -c <n> --channels <n>     set channel count to <n>\n");
	fprintf(o," -e <c> --encoding <c>     set output encoding (%s)\n"
	,	enclist != NULL ? enclist : "OOM!");
	fprintf(o," -m     --mono             set channel count to 1\n");
	fprintf(o,"        --stereo           set channel count to 2 (default)\n");
	fprintf(o,"        --list-encodings   list of encoding short and long names\n");
	fprintf(o,"        --test-format      return 0 if configued audio format is supported\n");
	fprintf(o,"        --test-encodings   print out possible encodings with given channels/rate\n");
	fprintf(o,"        --query-format     print out default format for given device, if any\n");
	fprintf(o," -o h   --headphones       (aix/hp/sun) output on headphones\n");
	fprintf(o," -o s   --speaker          (aix/hp/sun) output on speaker\n");
	fprintf(o," -o l   --lineout          (aix/hp/sun) output to lineout\n");
#ifndef NOXFERMEM
	fprintf(o," -b <n> --buffer <n>       set play buffer (\"output cache\")\n");
	fprintf(o,"        --preload <value>  fraction of buffer to fill before playback\n");
#endif
	fprintf(o,"        --devbuffer <s>    set device buffer in seconds; <= 0 means default\n");
	fprintf(o,"        --timelimit <s>    set time limit in PCM samples if >= 0\n");
	fprintf(o,"        --wave-freq <f>    set wave generator frequency or list of those\n");
	fprintf(o,"                           with comma separation for enabling a generated\n");
	fprintf(o,"                           test signal instead of standard input,\n");
	fprintf(o,"                           empty value repeating the previous\n");
	fprintf(o,"        --wave-pat <p>     set wave pattern(s) (out of those:\n");
	fprintf(o,"                           %s),\n", wave_pattern_list);
	fprintf(o,"                           empty value repeating the previous\n");
	fprintf(o,"        --wave-phase <p>   set wave phase shift(s), negative values\n");
	fprintf(o,"                           inverting the pattern in time and\n");
	fprintf(o,"                           empty value repeating the previous\n");
	fprintf(o,"        --wave-limit <l>   soft limit on wave table size\n");
	fprintf(o," -t     --test             no output, just read and discard data (-o test)\n");
	fprintf(o," -v[*]  --verbose          increase verboselevel\n");
	#ifdef HAVE_SETPRIORITY
	fprintf(o,"        --aggressive       tries to get higher priority (nice)\n");
	#endif
	#if defined (HAVE_SCHED_SETSCHEDULER) || defined (HAVE_WINDOWS_H)
	fprintf(o," -T     --realtime         tries to get realtime priority\n");
	#endif
	#ifdef HAVE_WINDOWS_H
	fprintf(o,"        --priority <n>     use specified process priority\n");
	fprintf(o,"                           accepts -2 to 3 as integer arguments\n");
	fprintf(o,"                           -2 as idle, 0 as normal and 3 as realtime.\n");
	#endif
	fprintf(o," -?     --help             give compact help\n");
	fprintf(o,"        --longhelp         give this long help listing\n");
	fprintf(o,"        --version          give name / version string\n");

	fprintf(o,"\nSee the manpage out123(1) for more information.\n");
	free(enclist);
	safe_exit(err);
}

static void want_long_usage(char* arg)
{
	long_usage(0);
}

static void give_version(char* arg)
{
	fprintf(stdout, "out123 "PACKAGE_VERSION"\n");
	safe_exit(0);
}
