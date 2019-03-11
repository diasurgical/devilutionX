/*
	mpg123: main code of the program (not of the decoder...)

	copyright 1995-2013 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Michael Hipp
*/

#define ME "main"
#include "mpg123app.h"
#include "mpg123.h"
#include "out123.h"
#include "local.h"

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

/* be paranoid about setpriority support */
#ifndef PRIO_PROCESS
#undef HAVE_SETPRIORITY
#endif

#include "common.h"
#include "sysutil.h"
#include "getlopt.h"
#include "term.h"
#include "playlist.h"
#include "httpget.h"
#include "metaprint.h"
#include "httpget.h"
#include "streamdump.h"

#include "debug.h"

static void usage(int err);
static void want_usage(char* arg);
static void long_usage(int err);
static void want_long_usage(char* arg);
static void print_title(FILE* o);
static void give_version(char* arg);

struct parameter param = { 
  FALSE , /* aggressiv */
  FALSE , /* shuffle */
  FALSE , /* remote */
  FALSE , /* remote to stderr */
  FALSE , /* silent operation */
  FALSE , /* xterm title on/off */
  0 ,     /* second level buffer size */
  0 ,     /* verbose level */
  DEFAULT_OUTPUT_MODULE,	/* output module */
  NULL,   /* output device */
  0,      /* destination (headphones, ...) */
#ifdef HAVE_TERMIOS
  FALSE , /* term control */
  MPG123_TERM_USR1,
  MPG123_TERM_USR2,
#endif
  FALSE , /* checkrange */
  0 ,	  /* force_reopen, always (re)opens audio device for next song */
  /* test_cpu flag is valid for multi and 3dnow.. even if 3dnow is built alone; ensure it appears only once */
  FALSE , /* normal operation */
  FALSE,  /* try to run process in 'realtime mode' */
#ifdef HAVE_WINDOWS_H 
  0, /* win32 process priority */
#endif
	0, /* default is to play all titles in playlist */
	NULL, /* no playlist per default */
	0 /* condensed id3 per default */
	,0 /* list_cpu */
	,NULL /* cpu */ 
#ifdef FIFO
	,NULL
#endif
	,0 /* timeout */
	,1 /* loop */
	,0 /* delay */
	,0 /* index */
	/* Parameters for mpg123 handle, defaults are queried from library! */
	,0 /* down_sample */
	,0 /* rva */
	,0 /* halfspeed */
	,0 /* doublespeed */
	,0 /* start_frame */
	,-1 /* frame_number */
	,0 /* outscale */
	,0 /* flags */
	,0 /* force_rate */
	,1 /* ICY */
	,1024 /* resync_limit */
	,0 /* smooth */
	,0.0 /* pitch */
	,0 /* appflags */
	,NULL /* proxyurl */
	,0 /* keep_open */
	,0 /* force_utf8 */
	,INDEX_SIZE
	,NULL /* force_encoding */
	,0.2 /* preload */
	,-1 /* preframes */
	,-1 /* gain */
	,NULL /* stream dump file */
	,0 /* ICY interval */
	,"mpg123" /* name */
	,0. /* device buffer */
};

mpg123_handle *mh = NULL;
off_t framenum;
off_t frames_left;
out123_handle *ao = NULL;
static long output_propflags = 0;
char *prgName = NULL;
/* ThOr: pointers are not TRUE or FALSE */
char *equalfile = NULL;
struct httpdata htd;
int fresh = TRUE;
FILE* aux_out = NULL; /* Output for interesting information, normally on stdout to be parseable. */

int intflag = FALSE;
int deathflag = FALSE;
static int skip_tracks = 0;
int OutputDescriptor;

static int filept = -1;

static int network_sockets_used = 0; /* Win32 socket open/close Support */

char *fullprogname = NULL; /* Copy of argv[0]. */
char *binpath; /* Path to myself. */

/* File-global storage of command line arguments.
   They may be needed for cleanup after charset conversion. */
static char **argv = NULL;
static int    argc = 0;

/* Cleanup marker to know that we intiialized libmpg123 already. */
static int cleanup_mpg123 = FALSE;

static long new_header = FALSE;
static char *prebuffer = NULL;
static size_t prebuffer_size = 0;
static size_t prebuffer_fill = 0;
static size_t minbytes = 0;

void set_intflag()
{
	debug("set_intflag TRUE");
	intflag = TRUE;
	skip_tracks = 0;
}

#if !defined(WIN32) && !defined(GENERIC)
static void catch_interrupt(void)
{
	intflag = TRUE;
}
static void handle_fatal_msg(const char *msg, size_t n)
{
	if(msg && !param.quiet)
		write(STDERR_FILENO, msg, n);
	intflag = TRUE;
	deathflag = TRUE;
}
static void catch_fatal_term(void)
{
	const char msg[] = "\nmpg123: death by SIGTERM\n";
	handle_fatal_msg(msg, sizeof(msg));
}
static void catch_fatal_pipe(void)
{
	/* If the SIGPIPE is because of piped stderr, trying to write
	   in the signal handler hangs the program. */
	handle_fatal_msg(NULL, 0);
}
#endif

static void skip_track(void)
{
	intflag = TRUE;
	++skip_tracks;
}

void next_track(void)
{
	playlist_jump(+1);
	skip_track();
}

void prev_track(void)
{
	playlist_jump(-1);
	skip_track();
}

void next_dir(void)
{
	playlist_next_dir();
	skip_track();
}

void prev_dir(void)
{
	playlist_prev_dir();
	skip_track();
}

void safe_exit(int code);

static void play_prebuffer(void)
{
	/* Ensure that the prebuffer bit has been posted. */
	if(prebuffer_fill)
	{
		if(out123_play(ao, prebuffer, prebuffer_fill) < prebuffer_fill)
		{
			error("Deep trouble! Cannot flush to my output anymore!");
			safe_exit(133);
		}
		prebuffer_fill = 0;
	}
}

/* Drain output device/buffer, but still give the option to interrupt things. */
static void controlled_drain(void)
{
	int framesize;
	size_t drain_block;

	play_prebuffer();

	if(intflag || !out123_buffered(ao))
		return;
	if(out123_getformat(ao, NULL, NULL, NULL, &framesize))
		return;
	drain_block = 1152*framesize;
	if(param.verbose)
		fprintf(stderr, "\n");
	do
	{
		out123_ndrain(ao, drain_block);
		if(param.verbose)
			print_buf("Draining buffer: ", ao);
#ifdef HAVE_TERMIOS
		if(param.term_ctrl)
			term_control(mh, ao);
#endif
	}
	while(!intflag && out123_buffered(ao));
	if(param.verbose)
		fprintf(stderr, "\n");
}

void safe_exit(int code)
{
	char *dummy, *dammy;

	if(prebuffer)
		free(prebuffer);

	dump_close();
	if(!code)
		controlled_drain();
	if(intflag)
		out123_drop(ao);
	out123_del(ao);

	if(mh != NULL) mpg123_delete(mh);

	if(cleanup_mpg123) mpg123_exit();

	httpdata_free(&htd);

#ifdef WANT_WIN32_UNICODE
	win32_cmdline_free(argc, argv); /* This handles the premature argv == NULL, too. */
#endif
#if defined (WANT_WIN32_SOCKETS)
	win32_net_deinit();
#endif
	/* It's ugly... but let's just fix this still-reachable memory chunk of static char*. */
	split_dir_file("", &dummy, &dammy);
	if(fullprogname) free(fullprogname);

#ifdef HAVE_TERMIOS
	term_exit();
#endif
	exit(code);
}

static void check_fatal_output(int code)
{
	if(code)
	{
		if(!param.quiet)
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
			param.output_device = &arg[i+1];
			debug1("Setting output device: %s", param.output_device);
			break;
		}	
	}
	/* Set the output module */
	param.output_module = arg;
	debug1("Setting output module: %s", param.output_module );
}

static void set_output_flag(int flag)
{
  if(param.output_flags <= 0) param.output_flags = flag;
  else param.output_flags |= flag;
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
    param.verbose++;
}

static void set_quiet (char *arg)
{
	param.verbose=0;
	param.quiet=TRUE;
}

static void set_out_wav(char *arg)
{
	param.output_module = "wav";
	param.output_device = arg;
}

void set_out_cdr(char *arg)
{
	param.output_module = "cdr";
	param.output_device = arg;
}

void set_out_au(char *arg)
{
	param.output_module = "au";
	param.output_device = arg;
}

void set_out_test(char *arg)
{
	param.output_module = "test";
	param.output_device = NULL;
}

static void set_out_file(char *arg)
{
	param.output_module = "raw";
	param.output_device = arg;
}

static void set_out_stdout(char *arg)
{
	param.output_module = "raw";
	param.output_device = NULL;
}

static void set_out_stdout1(char *arg)
{
	param.output_module = "raw";
	param.output_device = NULL;
}

#if !defined (HAVE_SCHED_SETSCHEDULER) && !defined (HAVE_WINDOWS_H)
static void realtime_not_compiled(char *arg)
{
	fprintf(stderr,"Option '-T / --realtime' not compiled into this binary.\n");
}
#endif

static int frameflag; /* ugly, but that's the way without hacking getlopt */
static void set_frameflag(char *arg)
{
	/* Only one mono flag at a time! */
	if(frameflag & MPG123_FORCE_MONO) param.flags &= ~MPG123_FORCE_MONO;
	param.flags |= frameflag;
}
static void unset_frameflag(char *arg)
{
	param.flags &= ~frameflag;
}

static int appflag; /* still ugly, but works */
static void set_appflag(char *arg)
{
	param.appflags |= appflag;
}

static void list_output_modules(char *arg)
{
	char **names = NULL;
	char **descr = NULL;
	int count = -1;
	out123_handle *lao;

	if((lao=out123_new()))
	{
		printf("\n");
		printf("Available modules\n");
		printf("-----------------\n");
		out123_param_string(lao, OUT123_BINDIR, binpath);
		out123_param_int(lao, OUT123_VERBOSE, param.verbose);
		if(param.quiet)
			out123_param_int(lao, OUT123_FLAGS, OUT123_QUIET);
		if((count=out123_drivers(lao, &names, &descr)) >= 0)
		{
			int i;
			for(i=0; i<count; ++i)
			{
				printf( "%-15s%s  %s\n"
				,	names[i], "output", descr[i] );
				free(names[i]);
				free(descr[i]);
			}
			free(names);
			free(descr);
		}
		out123_del(lao);
	}
	else if(!param.quiet)
		error("Failed to create an out123 handle.");
	exit(count >= 0 ? 0 : 1);
}


/* static void unset_appflag(char *arg)
{
	param.appflags &= ~appflag;
} */

/* Please note: GLO_NUM expects point to LONG! */
/* ThOr:
 *  Yeah, and despite that numerous addresses to int variables were 
passed.
 *  That's not good on my Alpha machine with int=32bit and long=64bit!
 *  Introduced GLO_INT and GLO_LONG as different bits to make that clear.
 *  GLO_NUM no longer exists.
 */
#ifdef OPT_3DNOW
static int dnow = 0; /* helper for mapping the old 3dnow options */
#endif
topt opts[] = {
	{'k', "skip",        GLO_ARG | GLO_LONG, 0, &param.start_frame, 0},
	{'2', "2to1",        GLO_INT,  0, &param.down_sample, 1},
	{'4', "4to1",        GLO_INT,  0, &param.down_sample, 2},
	{'t', "test",        GLO_INT,  set_out_test, NULL, 0},
	{'s', "stdout",      GLO_INT,  set_out_stdout,  NULL, 0},
	{'S', "STDOUT",      GLO_INT,  set_out_stdout1, NULL, 0},
	{'O', "outfile",     GLO_ARG | GLO_CHAR, set_out_file, NULL, 0},
	{'c', "check",       GLO_INT,  0, &param.checkrange, TRUE},
	{'v', "verbose",     0,        set_verbose, 0,           0},
	{'q', "quiet",       0,        set_quiet,   0,           0},
	{'y', "no-resync",      GLO_INT,  set_frameflag, &frameflag, MPG123_NO_RESYNC},
	/* compatibility, no-resync is to be used nowadays */
	{0, "resync",      GLO_INT,  set_frameflag, &frameflag, MPG123_NO_RESYNC},
	{'0', "single0",     GLO_INT,  set_frameflag, &frameflag, MPG123_MONO_LEFT},
	{0,   "left",        GLO_INT,  set_frameflag, &frameflag, MPG123_MONO_LEFT},
	{'1', "single1",     GLO_INT,  set_frameflag, &frameflag, MPG123_MONO_RIGHT},
	{0,   "right",       GLO_INT,  set_frameflag, &frameflag, MPG123_MONO_RIGHT},
	{'m', "singlemix",   GLO_INT,  set_frameflag, &frameflag, MPG123_MONO_MIX},
	{0,   "mix",         GLO_INT,  set_frameflag, &frameflag, MPG123_MONO_MIX},
	{0,   "mono",        GLO_INT,  set_frameflag, &frameflag, MPG123_MONO_MIX},
	{0,   "stereo",      GLO_INT,  set_frameflag, &frameflag, MPG123_FORCE_STEREO},
	{0,   "reopen",      GLO_INT,  0, &param.force_reopen, 1},
	{'g', "gain",        GLO_ARG | GLO_LONG, 0, &param.gain,    0},
	{'r', "rate",        GLO_ARG | GLO_LONG, 0, &param.force_rate,  0},
	{0,   "8bit",        GLO_INT,  set_frameflag, &frameflag, MPG123_FORCE_8BIT},
	{0,   "float",       GLO_INT,  set_frameflag, &frameflag, MPG123_FORCE_FLOAT},
	{0,   "headphones",  0,                  set_output_h, 0,0},
	{0,   "speaker",     0,                  set_output_s, 0,0},
	{0,   "lineout",     0,                  set_output_l, 0,0},
	{'o', "output",      GLO_ARG | GLO_CHAR, set_output, 0,  0},
	{0,   "list-modules",0,       list_output_modules, NULL, 0},
	{'a', "audiodevice", GLO_ARG | GLO_CHAR, 0, &param.output_device,  0},
	{'f', "scale",       GLO_ARG | GLO_LONG, 0, &param.outscale,   0},
	{'n', "frames",      GLO_ARG | GLO_LONG, 0, &param.frame_number,  0},
#ifdef HAVE_TERMIOS
	{'C', "control",     GLO_INT,  0, &param.term_ctrl, TRUE},
	{0, "no-control",    GLO_INT,  0, &param.term_ctrl, FALSE},
	{0,   "ctrlusr1",    GLO_ARG | GLO_CHAR, 0, &param.term_usr1, 0},
	{0,   "ctrlusr2",    GLO_ARG | GLO_CHAR, 0, &param.term_usr2, 0},
#endif
#ifndef NOXFERMEM
	{'b', "buffer",      GLO_ARG | GLO_LONG, 0, &param.usebuffer,  0},
	{0,  "smooth",      GLO_INT,  0, &param.smooth, 1},
	{0, "preload", GLO_ARG|GLO_DOUBLE, 0, &param.preload, 0},
#endif
	{'R', "remote",      GLO_INT,  0, &param.remote, TRUE},
	{0,   "remote-err",  GLO_INT,  0, &param.remote_err, TRUE},
	{'d', "doublespeed", GLO_ARG | GLO_LONG, 0, &param.doublespeed, 0},
	{'h', "halfspeed",   GLO_ARG | GLO_LONG, 0, &param.halfspeed, 0},
#ifdef NETWORK
	{'p', "proxy",       GLO_ARG | GLO_CHAR, 0, &param.proxyurl,   0},
#endif
	{'@', "list",        GLO_ARG | GLO_CHAR, 0, &param.listname,   0},
	/* 'z' comes from the the german word 'zufall' (eng: random) */
	{'z', "shuffle",     GLO_INT,  0, &param.shuffle, 1},
	{'Z', "random",      GLO_INT,  0, &param.shuffle, 2},
	{'E', "equalizer",	 GLO_ARG | GLO_CHAR, 0, &equalfile,1},
	#ifdef HAVE_SETPRIORITY
	{0,   "aggressive",	 GLO_INT,  0, &param.aggressive, 2},
	#endif
	#ifdef OPT_3DNOW
#define SET_3DNOW 1
#define SET_I586  2
	{0,   "force-3dnow", GLO_CHAR,  0, &dnow, SET_3DNOW},
	{0,   "no-3dnow",    GLO_CHAR,  0, &dnow, SET_I586},
	{0,   "test-3dnow",  GLO_INT,  0, &param.test_cpu, TRUE},
	#endif
	{0, "cpu", GLO_ARG | GLO_CHAR, 0, &param.cpu,  0},
	{0, "test-cpu",  GLO_INT,  0, &param.test_cpu, TRUE},
	{0, "list-cpu", GLO_INT,  0, &param.list_cpu , 1},
#ifdef NETWORK
	{'u', "auth",        GLO_ARG | GLO_CHAR, 0, &httpauth,   0},
#endif
	#if defined (HAVE_SCHED_SETSCHEDULER) || defined (HAVE_WINDOWS_H)
	/* check why this should be a long variable instead of int! */
	{'T', "realtime",    GLO_LONG,  0, &param.realtime, TRUE },
	#else
	{'T', "realtime",    0,  realtime_not_compiled, 0,           0 },    
	#endif
	#ifdef HAVE_WINDOWS_H
	{0, "priority", GLO_ARG | GLO_INT, 0, &param.w32_priority, 0},
	#endif
	{0, "title",         GLO_INT,  0, &param.xterm_title, TRUE },
	{'w', "wav",         GLO_ARG | GLO_CHAR, set_out_wav, 0, 0 },
	{0, "cdr",           GLO_ARG | GLO_CHAR, set_out_cdr, 0, 0 },
	{0, "au",            GLO_ARG | GLO_CHAR, set_out_au, 0, 0 },
	{0,   "gapless",	 GLO_INT,  set_frameflag, &frameflag, MPG123_GAPLESS},
	{0,   "no-gapless", GLO_INT, unset_frameflag, &frameflag, MPG123_GAPLESS},
	{0, "no-infoframe", GLO_INT, set_frameflag, &frameflag, MPG123_IGNORE_INFOFRAME},
	{'?', "help",            0,  want_usage, 0,           0 },
	{0 , "longhelp" ,        0,  want_long_usage, 0,      0 },
	{0 , "version" ,         0,  give_version, 0,         0 },
	{'l', "listentry",       GLO_ARG | GLO_LONG, 0, &param.listentry, 0 },
	{0, "continue", GLO_INT, set_appflag, &appflag, MPG123APP_CONTINUE },
	{0, "rva-mix",         GLO_INT,  0, &param.rva, 1 },
	{0, "rva-radio",         GLO_INT,  0, &param.rva, 1 },
	{0, "rva-album",         GLO_INT,  0, &param.rva, 2 },
	{0, "rva-audiophile",         GLO_INT,  0, &param.rva, 2 },
	{0, "no-icy-meta",      GLO_INT,  0, &param.talk_icy, 0 },
	{0, "long-tag",         GLO_INT,  0, &param.long_id3, 1 },
#ifdef FIFO
	{0, "fifo", GLO_ARG | GLO_CHAR, 0, &param.fifo,  0},
#endif
	{0, "timeout", GLO_ARG | GLO_LONG, 0, &param.timeout, 0},
	{0, "loop", GLO_ARG | GLO_LONG, 0, &param.loop, 0},
	{'i', "index", GLO_INT, 0, &param.index, 1},
	{'D', "delay", GLO_ARG | GLO_INT, 0, &param.delay, 0},
	{0, "resync-limit", GLO_ARG | GLO_LONG, 0, &param.resync_limit, 0},
	{0, "pitch", GLO_ARG|GLO_DOUBLE, 0, &param.pitch, 0},
#ifdef NETWORK
	{0, "ignore-mime", GLO_INT, set_appflag, &appflag, MPG123APP_IGNORE_MIME },
#endif
	{0, "lyrics", GLO_INT, set_appflag, &appflag, MPG123APP_LYRICS},
	{0, "keep-open", GLO_INT, 0, &param.keep_open, 1},
	{0, "utf8", GLO_INT, 0, &param.force_utf8, 1},
	{0, "fuzzy", GLO_INT,  set_frameflag, &frameflag, MPG123_FUZZY},
	{0, "index-size", GLO_ARG|GLO_LONG, 0, &param.index_size, 0},
	{0, "no-seekbuffer", GLO_INT, unset_frameflag, &frameflag, MPG123_SEEKBUFFER},
	{'e', "encoding", GLO_ARG|GLO_CHAR, 0, &param.force_encoding, 0},
	{0, "preframes", GLO_ARG|GLO_LONG, 0, &param.preframes, 0},
	{0, "skip-id3v2", GLO_INT, set_frameflag, &frameflag, MPG123_SKIP_ID3V2},
	{0, "streamdump", GLO_ARG|GLO_CHAR, 0, &param.streamdump, 0},
	{0, "icy-interval", GLO_ARG|GLO_LONG, 0, &param.icy_interval, 0},
	{0, "ignore-streamlength", GLO_INT, set_frameflag, &frameflag, MPG123_IGNORE_STREAMLENGTH},
	{0, "name", GLO_ARG|GLO_CHAR, 0, &param.name, 0},
	{0, "devbuffer", GLO_ARG|GLO_DOUBLE, 0, &param.device_buffer, 0},
	{0, 0, 0, 0, 0, 0}
};

static int open_track_fd (void)
{
	/* Let reader handle invalid filept */
	if(mpg123_open_fd(mh, filept) != MPG123_OK)
	{
		error2("Cannot open fd %i: %s", filept, mpg123_strerror(mh));
		return 0;
	}
	debug("Track successfully opened.");
	fresh = TRUE;
	return 1;
	/*1 for success, 0 for failure */
}

/* 1 on success, 0 on failure */
int open_track(char *fname)
{
	filept=-1;
	httpdata_reset(&htd);
	if(MPG123_OK != mpg123_param(mh, MPG123_ICY_INTERVAL, 0, 0))
	error1("Cannot (re)set ICY interval: %s", mpg123_strerror(mh));
	if(!strcmp(fname, "-"))
	{
		filept = STDIN_FILENO;
#ifdef WIN32
		_setmode(STDIN_FILENO, _O_BINARY);
#endif
		return open_track_fd();
	}
	else if (!strncmp(fname, "http://", 7)) /* http stream */
	{
#if defined (WANT_WIN32_SOCKETS)
	if(param.streamdump != NULL)
	{
		fprintf(stderr, "\nWarning: win32 networking conflicts with stream dumping. Aborting the dump.\n");
		dump_close();
	}
	/*Use recv instead of stdio functions */
	win32_net_replace(mh);
	filept = win32_net_http_open(fname, &htd);
#else
	filept = http_open(fname, &htd);
#endif
	network_sockets_used = 1;
/* utf-8 encoded URLs might not work under Win32 */
		
		/* now check if we got sth. and if we got sth. good */
		if(    (filept >= 0) && (htd.content_type.p != NULL)
			  && !APPFLAG(MPG123APP_IGNORE_MIME) && !(debunk_mime(htd.content_type.p) & IS_FILE) )
		{
			error1("Unknown mpeg MIME type %s - is it perhaps a playlist (use -@)?", htd.content_type.p == NULL ? "<nil>" : htd.content_type.p);
			error("If you know the stream is mpeg1/2 audio, then please report this as "PACKAGE_NAME" bug");
			return 0;
		}
		if(filept < 0)
		{
			error1("Access to http resource %s failed.", fname);
			return 0;
		}
		if(MPG123_OK != mpg123_param(mh, MPG123_ICY_INTERVAL, htd.icy_interval, 0))
		error1("Cannot set ICY interval: %s", mpg123_strerror(mh));
		if(param.verbose > 1) fprintf(stderr, "Info: ICY interval %li\n", (long)htd.icy_interval);
	}

	if(param.icy_interval > 0)
	{
		if(MPG123_OK != mpg123_param(mh, MPG123_ICY_INTERVAL, param.icy_interval, 0))
		error1("Cannot set ICY interval: %s", mpg123_strerror(mh));
		if(param.verbose > 1) fprintf(stderr, "Info: Forced ICY interval %li\n", param.icy_interval);
	}

	debug("OK... going to finally open.");
	/* Now hook up the decoder on the opened stream or the file. */
	if(network_sockets_used) 
	{
		return open_track_fd();
	}
	else if(mpg123_open(mh, fname) != MPG123_OK)
	{
		error2("Cannot open %s: %s", fname, mpg123_strerror(mh));
		return 0;
	}
	debug("Track successfully opened.");

	fresh = TRUE;
	return 1;
}

/* for symmetry */
void close_track(void)
{
	mpg123_close(mh);
#if defined (WANT_WIN32_SOCKETS)
	if (network_sockets_used)
	win32_net_close(filept);
	filept = -1;
	return;
#endif
	network_sockets_used = 0;
	if(filept > -1) close(filept);
	filept = -1;
}

/* return 1 on success, 0 on failure */
int play_frame(void)
{
	unsigned char *audio;
	int mc;
	size_t bytes = 0;
	debug("play_frame");
	mc = mpg123_decode_frame(mh, &framenum, &audio, &bytes);
	mpg123_getstate(mh, MPG123_FRESH_DECODER, &new_header, NULL);

	/* Play what is there to play (starting with second decode_frame call!) */
	if(bytes)
	{
		if(param.frame_number > -1) --frames_left;
		if(fresh && framenum >= param.start_frame)
		{
			fresh = FALSE;
		}
		if(bytes < minbytes && !prebuffer_fill)
		{
			/* Postpone playback of little buffers until large buffers can
				follow them right away, preventing underruns. */
			if(prebuffer_size < minbytes)
			{
				if(prebuffer)
					free(prebuffer);
				if(!(prebuffer = malloc(minbytes)))
					safe_exit(11);
				prebuffer_size = minbytes;
			}
			memcpy(prebuffer, audio, bytes);
			prebuffer_fill = bytes;
			bytes = 0;
			debug1("prebuffered %"SIZE_P" bytes", prebuffer_fill);
		}
		if(param.checkrange)
		{
			long clip = mpg123_clip(mh);
			if(clip > 0) fprintf(stderr,"\n%ld samples clipped\n", clip);
		}
	}
	/* The bytes could have been postponed to later. */
	if(bytes)
	{
		unsigned char *playbuf = audio;
		if(prebuffer_fill)
		{
			size_t missing;
			/* This is tricky: The small piece needs to be filled up. Playing 10
			   pcm frames will always trigger underruns with ALSA, dammit!
			   This grabs some data from current frame, unless it itself would
			   end up smaller than the prebuffer. Ending up empty is fine. */
			if(  prebuffer_fill < prebuffer_size
			  && (  bytes <= (missing=prebuffer_size-prebuffer_fill)
			     || bytes >= missing+prebuffer_size ) )
			{
				if(bytes < missing)
					missing=bytes;
				memcpy(prebuffer+prebuffer_fill, playbuf, missing);
				playbuf += missing;
				bytes -= missing;
				prebuffer_fill += missing;
			}
			if(   out123_play(ao, prebuffer, prebuffer_fill) < prebuffer_fill
			   && !intflag )
			{
				error("Deep trouble! Cannot flush to my output anymore!");
				safe_exit(133);
			}
			prebuffer_fill = 0;
		}
		/* Interrupt here doesn't necessarily interrupt out123_play().
		   I wonder if that makes us miss errors. Actual issues should
		   just be postponed. */
		if(bytes && !intflag) /* Previous piece could already be interrupted. */
		if(out123_play(ao, playbuf, bytes) < bytes && !intflag)
		{
			error("Deep trouble! Cannot flush to my output anymore!");
			safe_exit(133);
		}
	}
	/* Special actions and errors. */
	if(mc != MPG123_OK)
	{
		if(mc == MPG123_ERR || mc == MPG123_DONE)
		{
			if(mc == MPG123_ERR) error1("...in decoding next frame: %s", mpg123_strerror(mh));
			return 0;
		}
		if(mc == MPG123_NO_SPACE)
		{
			error("I have not enough output space? I didn't plan for this.");
			return 0;
		}
		if(mc == MPG123_NEW_FORMAT)
		{
			long rate;
			int channels;
			int encoding;
			play_prebuffer(); /* Make sure we got rid of old data. */
			mpg123_getformat(mh, &rate, &channels, &encoding);
			/* A layer I frame duration at minimum for live outputs. */
			if(output_propflags & OUT123_PROP_LIVE)
				minbytes = out123_encsize(encoding)*channels*384;
			else
				minbytes = 0;
			if(param.verbose > 2)
			{
				const char* encname = out123_enc_name(encoding);
				fprintf( stderr
				,	"\nNote: New output format with %li Hz, %i channels, encoding %s.\n"
				,	rate, channels, encname ? encname : "???" );
			}
			new_header = TRUE;
			check_fatal_output(out123_start(ao, rate, channels, encoding));
			/* We may take some time feeding proper data, so pause by default. */
			out123_pause(ao);
		}
	}
	if(new_header && !param.quiet)
	{
		new_header = FALSE;
		fprintf(stderr, "\n");
		if(param.verbose > 1)
			print_header(mh);
		else
			print_header_compact(mh);
	}
	return 1;
}

/* Return TRUE if we should continue (second interrupt happens quickly), skipping tracks, or FALSE if we should die. */
#if !defined(WIN32) && !defined(GENERIC)
int skip_or_die(struct timeval *start_time)
{
	/* Death is fatal right away. */
	if(deathflag)
	{
		debug("The world wants me to die.");
		return FALSE;
	}
/* 
 * When HAVE_TERMIOS is defined, there is 'q' to terminate a list of songs, so
 * no pressing need to keep up this first second SIGINT hack that was too
 * often mistaken as a bug. [dk]
 * ThOr: Yep, I deactivated the Ctrl+C hack for active control modes.
 *       Though, some sort of hack remains, still using intflag for track skip.
 */
#ifdef HAVE_TERMIOS
	if(!param.term_ctrl)
#endif
	{
		struct timeval now;
		unsigned long secdiff;
		gettimeofday (&now, NULL);
		secdiff = (now.tv_sec - start_time->tv_sec) * 1000;
		if(now.tv_usec >= start_time->tv_usec)
		secdiff += (now.tv_usec - start_time->tv_usec) / 1000;
		else
		secdiff -= (start_time->tv_usec - now.tv_usec) / 1000;
		if (secdiff < 1000)
		{
			debug("got the second interrupt: out of here!");
			return FALSE;
		}
		else
		{
			debug("It's a track advancement message.");
			++skip_tracks;
		}
	}
#ifdef HAVE_TERMIOS
	else if(skip_tracks == 0)
	{
		debug("breaking up");
		return FALSE;
	}
#endif
	return TRUE; /* Track advancement... no instant kill on generic/windows... */
}
#else
/* On generic systems and win32, there is no decision here... just TRUE. */
#define skip_or_die(a) TRUE
#endif

int main(int sys_argc, char ** sys_argv)
{
	int result;
	char end_of_files = FALSE;
	long parr;
	char *fname;
	int libpar = 0;
	mpg123_pars *mp;
#if !defined(WIN32) && !defined(GENERIC)
	struct timeval start_time;
#endif
	aux_out = stdout; /* Need to initialize here because stdout is not a constant?! */
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
#if defined (WANT_WIN32_SOCKETS)
	win32_net_init();
#endif

#ifdef WIN32
	/* Despite in Unicode form, the path munging backend is still in ANSI/ASCII
	 * so using _wpgmptr with unicode paths after UTF8 conversion is broken on Windows
	 */
	
	fullprogname = compat_strdup(_pgmptr);
#else
	fullprogname = compat_strdup(argv[0]);
#endif

	if(!fullprogname)
	{
		error("OOM"); /* Out Of Memory. Don't waste bytes on that error. */
		safe_exit(1);
	}
	/* Extract binary and path, take stuff before/after last / or \ . */
	if(  (prgName = strrchr(fullprogname, '/')) 
	  || (prgName = strrchr(fullprogname, '\\')))
	{
		/* There is some explicit path. */
		prgName[0] = 0; /* End byte for path. */
		prgName++;
		binpath = fullprogname;
	}
	else
	{
		prgName = fullprogname; /* No path separators there. */
		binpath = NULL; /* No path at all. */
	}

	/* Need to initialize mpg123 lib here for default parameter values. */

	result = mpg123_init();
	if(result != MPG123_OK)
	{
		error1("Cannot initialize mpg123 library: %s", mpg123_plain_strerror(result));
		safe_exit(77);
	}
	cleanup_mpg123 = TRUE;

	mp = mpg123_new_pars(&result); /* This may get leaked on premature exit(), which is mainly a cosmetic issue... */
	if(mp == NULL)
	{
		error1("Crap! Cannot get mpg123 parameters: %s", mpg123_plain_strerror(result));
		safe_exit(78);
	}

	/* get default values */
	mpg123_getpar(mp, MPG123_DOWN_SAMPLE, &parr, NULL);
	param.down_sample = (int) parr;
	mpg123_getpar(mp, MPG123_RVA, &param.rva, NULL);
	mpg123_getpar(mp, MPG123_DOWNSPEED, &param.halfspeed, NULL);
	mpg123_getpar(mp, MPG123_UPSPEED, &param.doublespeed, NULL);
	mpg123_getpar(mp, MPG123_OUTSCALE, &param.outscale, NULL);
	mpg123_getpar(mp, MPG123_FLAGS, &parr, NULL);
	mpg123_getpar(mp, MPG123_INDEX_SIZE, &param.index_size, NULL);
	param.flags = (int) parr;
	param.flags |= MPG123_SEEKBUFFER; /* Default on, for HTTP streams. */
	mpg123_getpar(mp, MPG123_RESYNC_LIMIT, &param.resync_limit, NULL);
	mpg123_getpar(mp, MPG123_PREFRAMES, &param.preframes, NULL);
	/* Also need proper default flags from libout123. */
	{
		out123_handle *paro = out123_new();
		out123_getparam_int(paro, OUT123_FLAGS, &param.output_flags);
		out123_del(paro);
	}

#ifdef OS2
        _wildcard(&argc,&argv);
#endif
#ifdef HAVE_TERMIOS
	/* Detect terminal on input side, enable control by default. */
	param.term_ctrl = !(term_width(STDIN_FILENO) < 0);
#endif
	while ((result = getlopt(argc, argv, opts)))
	switch (result) {
		case GLO_UNKNOWN:
			fprintf (stderr, "%s: Unknown option \"%s\".\n", 
				prgName, loptarg);
			usage(1);
		case GLO_NOARG:
			fprintf (stderr, "%s: Missing argument for option \"%s\".\n",
				prgName, loptarg);
			usage(1);
	}
	/* Do this _after_ parameter parsing. */
	check_locale(); /* Check/set locale; store if it uses UTF-8. */

	if(param.list_cpu)
	{
		const char **all_dec = mpg123_decoders();
		printf("Builtin decoders:");
		while(*all_dec != NULL){ printf(" %s", *all_dec); ++all_dec; }
		printf("\n");
		mpg123_delete_pars(mp);
		return 0;
	}
	if(param.test_cpu)
	{
		const char **all_dec = mpg123_supported_decoders();
		printf("Supported decoders:");
		while(*all_dec != NULL){ printf(" %s", *all_dec); ++all_dec; }
		printf("\n");
		mpg123_delete_pars(mp);
		return 0;
	}
	if(param.gain != -1)
	{
	    warning("The parameter -g is deprecated and may be removed in the future.");
	}

	if (loptind >= argc && !param.listname && !param.remote) usage(1);
	/* Init audio as early as possible.
	   If there is the buffer process to be spawned, it shouldn't carry the mpg123_handle with it. */

	/* ========================================================================================================= */
	/* Enterning the leaking zone... we start messing with stuff here that should be taken care of when leaving. */
	/* Don't just exit() or return out...                                                                        */
	/* ========================================================================================================= */

	httpdata_init(&htd);

#if !defined(WIN32) && !defined(GENERIC)
	if(param.remote && !param.verbose)
		param.quiet = 1;
#endif

	/* Set the frame parameters from command line options */
	if(param.quiet)
	{
		param.flags |= MPG123_QUIET;
		param.output_flags |= OUT123_QUIET;
	}

#ifdef OPT_3DNOW
	if(dnow != 0) param.cpu = (dnow == SET_3DNOW) ? "3dnow" : "i586";
#endif
	if(param.cpu != NULL && (!strcmp(param.cpu, "auto") || !strcmp(param.cpu, ""))) param.cpu = NULL;
	if(!(  MPG123_OK == (result = mpg123_par(mp, MPG123_VERBOSE, param.verbose, 0))
	    && ++libpar
	    && MPG123_OK == (result = mpg123_par(mp, MPG123_FLAGS, param.flags, 0))
	    && ++libpar
	    && MPG123_OK == (result = mpg123_par(mp, MPG123_DOWN_SAMPLE, param.down_sample, 0))
	    && ++libpar
	    && MPG123_OK == (result = mpg123_par(mp, MPG123_RVA, param.rva, 0))
	    && ++libpar
	    && MPG123_OK == (result = mpg123_par(mp, MPG123_FORCE_RATE, param.force_rate, 0))
	    && ++libpar
	    && MPG123_OK == (result = mpg123_par(mp, MPG123_DOWNSPEED, param.halfspeed, 0))
	    && ++libpar
	    && MPG123_OK == (result = mpg123_par(mp, MPG123_UPSPEED, param.doublespeed, 0))
	    && ++libpar
	    && MPG123_OK == (result = mpg123_par(mp, MPG123_ICY_INTERVAL, 0, 0))
	    && ++libpar
	    && MPG123_OK == (result = mpg123_par(mp, MPG123_RESYNC_LIMIT, param.resync_limit, 0))
	    && ++libpar
	    && MPG123_OK == (result = mpg123_par(mp, MPG123_TIMEOUT, param.timeout, 0))
	    && ++libpar
	    && MPG123_OK == (result = mpg123_par(mp, MPG123_OUTSCALE, param.outscale, 0))
	    && ++libpar
	    && MPG123_OK == (result = mpg123_par(mp, MPG123_PREFRAMES, param.preframes, 0))
			))
	{
		error2("Cannot set library parameter %i: %s", libpar, mpg123_plain_strerror(result));
		safe_exit(45);
	}
	if (!(param.listentry < 0) && !param.quiet) print_title(stderr); /* do not pollute stdout! */

	{
		long default_index;
		mpg123_getpar(mp, MPG123_INDEX_SIZE, &default_index, NULL);
		if( param.index_size != default_index && (result = mpg123_par(mp, MPG123_INDEX_SIZE, param.index_size, 0.)) != MPG123_OK )
		error1("Setting of frame index size failed: %s", mpg123_plain_strerror(result));
	}

	if(param.force_rate && param.down_sample)
	{
		error("Down sampling and fixed rate options not allowed together!");
		safe_exit(1);
	}

	/* Now actually get an mpg123_handle. */
	mh = mpg123_parnew(mp, param.cpu, &result);
	if(mh == NULL)
	{
		error1("Crap! Cannot get a mpg123 handle: %s", mpg123_plain_strerror(result));
		safe_exit(77);
	}
	mpg123_delete_pars(mp); /* Don't need the parameters anymore ,they're in the handle now. */

	/* Prepare stream dumping, possibly replacing mpg123 reader. */
	if(dump_open(mh) != 0) safe_exit(78);

	load_equalizer(mh);

#ifdef HAVE_SETPRIORITY
	if(param.aggressive) { /* tst */
		int mypid = getpid();
		if(!param.quiet) fprintf(stderr,"Aggressively trying to increase priority.\n");
		if(setpriority(PRIO_PROCESS,mypid,-20))
			error("Failed to aggressively increase priority.\n");
	}
#endif

#if defined (HAVE_SCHED_SETSCHEDULER) && !defined (__CYGWIN__) && !defined (HAVE_WINDOWS_H)
/* Cygwin --realtime seems to fail when accessing network, using win32 set priority instead */
/* MinGW may have pthread installed, we prefer win32API */
	if (param.realtime) {  /* Get real-time priority */
	  struct sched_param sp;
	  fprintf(stderr,"Getting real-time priority\n");
	  memset(&sp, 0, sizeof(struct sched_param));
	  sp.sched_priority = sched_get_priority_min(SCHED_FIFO);
	  if (sched_setscheduler(0, SCHED_RR, &sp) == -1)
	    error("Can't get realtime priority\n");
	}
#endif

/* make sure not Cygwin, it doesn't need it */
#if defined(WIN32) && defined(HAVE_WINDOWS_H)
	/* argument "3" is equivalent to realtime priority class */
	win32_set_priority( param.realtime ? 3 : param.w32_priority);
#endif

	/* TODO: There's some memory leaking on fatal safe_exits().
	   This is a cosmetic issue, though. */


	/* Initializing output after the priority stuff, might influence
	   buffer process. */
	ao = out123_new();
	if(!ao)
	{
		if(!param.quiet)
			error("Failed to allocate output.");
		safe_exit(97);
	}
	if
	( 0
	||	out123_param_int(ao, OUT123_FLAGS, param.output_flags)
	|| out123_param_float(ao, OUT123_PRELOAD, param.preload)
	|| out123_param_int(ao, OUT123_GAIN, param.gain)
	|| out123_param_int(ao, OUT123_VERBOSE, param.verbose)
	|| out123_param_string(ao, OUT123_NAME, param.name)
	|| out123_param_string(ao, OUT123_BINDIR, binpath)
	|| out123_param_float(ao, OUT123_DEVICEBUFFER, param.device_buffer)
	)
	{
		if(!param.quiet)
			error("Error setting output parameters. Do you need a usage reminder?");
		safe_exit(98);
	}
	check_fatal_output(out123_set_buffer(ao, param.usebuffer*1024));
	check_fatal_output(out123_open( ao
	,	param.output_module, param.output_device ));
	out123_getparam_int(ao, OUT123_PROPFLAGS, &output_propflags);

	if(!param.remote) prepare_playlist(argc, argv);

#if !defined(WIN32) && !defined(GENERIC)
	/* Remote mode is special... but normal console and terminal-controlled operation needs to catch the SIGINT.
	   For one it serves for track skip when not in terminal control mode.
	   The more important use being a graceful exit, including telling the buffer process what's going on. */
	if(!param.remote)
		catchsignal(SIGINT, catch_interrupt);
	/* Need to catch things to exit cleanly, not messing up the terminal. */
	catchsignal(SIGTERM, catch_fatal_term);
	catchsignal(SIGPIPE, catch_fatal_pipe);
#endif
	/* Now either check caps myself or query buffer for that. */
	audio_capabilities(ao, mh);

	if(param.remote) {
		int ret;
		ret = control_generic(mh);
		safe_exit(ret);
	}
#ifdef HAVE_TERMIOS
			term_init();
#endif
	if(APPFLAG(MPG123APP_CONTINUE)) frames_left = param.frame_number;

	while ((fname = get_next_file()))
	{
		char *dirname, *filename;
		int newdir;
		/* skip_tracks includes the previous one. */
		if(skip_tracks) --skip_tracks;
		if(skip_tracks)
		{
			debug("Skipping this track.");
			continue;
		}
		if(param.delay > 0)
		{
			controlled_drain();
			/* One should enable terminal control during that sleeping phase! */
			if(param.verbose > 2) fprintf(stderr, "Note: pausing %i seconds before next track.\n", param.delay);
#ifdef WIN32
			Sleep(param.delay*1000);
#else
			sleep(param.delay);
#endif
		}
		if(!APPFLAG(MPG123APP_CONTINUE)) frames_left = param.frame_number;

		debug1("Going to play %s", strcmp(fname, "-") ? fname : "standard input");

		if(intflag || !open_track(fname))
		{
#ifdef HAVE_TERMIOS
			/* We need the opportunity to cancel in case of --loop -1 . */
			if(param.term_ctrl) term_control(mh, ao);
			else
#endif
			/* No wait for a second interrupt before we started playing. */
			if(intflag) break;

			/* We already interrupted this cycle, start fresh with the next one. */
			intflag = FALSE;
			continue;
		}

		if(!param.quiet) fprintf(stderr, "\n");
		if(param.index)
		{
			if(param.verbose) fprintf(stderr, "indexing...\r");
			mpg123_scan(mh);
		}
		/*
			Only trigger a seek if we do not want to start with the first frame.
			Rationale: Because of libmpg123 sample accuracy, this could cause an unnecessary backwards seek, that even may fail on non-seekable streams.
			For start frame of 0, we are already at the correct position!
		*/
		framenum = 0;
		if(param.start_frame > 0)
		framenum = mpg123_seek_frame(mh, param.start_frame, SEEK_SET);

		if(APPFLAG(MPG123APP_CONTINUE)) param.start_frame = 0;

		if(framenum < 0)
		{
			error1("Initial seek failed: %s", mpg123_strerror(mh));
			if(mpg123_errcode(mh) == MPG123_BAD_OUTFORMAT)
			{
				fprintf(stderr, "%s", "So, you have trouble getting an output format... this is the matrix of currently possible formats:\n");
				print_capabilities(ao, mh);
				fprintf(stderr, "%s", "Somehow the input data and your choices don't allow one of these.\n");
			}
			mpg123_close(mh);
			continue;
		}

		/* Prinout and xterm title need this, possibly independently. */
		newdir = split_dir_file(fname ? fname : "standard input", &dirname, &filename);

		if (!param.quiet)
		{
			size_t plpos;
			size_t plfill;
			long plloop;
			plpos = playlist_pos(&plfill, &plloop);
			if(newdir) fprintf(stderr, "Directory: %s\n", dirname);

#ifdef HAVE_TERMIOS
		/* Reminder about terminal usage. */
		if(param.term_ctrl) term_hint();
#endif

			if(param.verbose)
			{
				if(plloop < 0)
					fprintf(stderr, "Repeating endlessly.\n");
				else if(plloop > 1)
					fprintf( stderr, "Repeating %ld %s.\n"
					,	plloop-1, plloop > 2 ? "times" : "time" );
			}
			fprintf( stderr, "Playing MPEG stream %"SIZE_P" of %"SIZE_P": %s ...\n"
			,	(size_p)plpos, (size_p)plfill, filename );
			if(htd.icy_name.fill) fprintf(stderr, "ICY-NAME: %s\n", htd.icy_name.p);
			if(htd.icy_url.fill)  fprintf(stderr, "ICY-URL: %s\n",  htd.icy_url.p);
		}
#if !defined(GENERIC)
{
	const char *term_type;
	term_type = getenv("TERM");
	if(term_type && param.xterm_title)
	{
		if(!strncmp(term_type,"xterm",5) || !strncmp(term_type,"rxvt",4))
		fprintf(stderr, "\033]0;%s\007", filename);
		else if(!strncmp(term_type,"screen",6))
		fprintf(stderr, "\033k%s\033\\", filename);
		else if(!strncmp(term_type,"iris-ansi",9))
		fprintf(stderr, "\033P1.y %s\033\\\033P3.y%s\033\\", filename, filename);

		fflush(stderr); /* Paranoia: will the buffer buffer the escapes? */
	}
}
#endif

/* Rethink that SIGINT logic... */
#if !defined(WIN32) && !defined(GENERIC)
#ifdef HAVE_TERMIOS
		if(!param.term_ctrl)
#endif
			gettimeofday (&start_time, NULL);
#endif

		while(!intflag)
		{
			int meta;
			if(param.frame_number > -1)
			{
				debug1("frames left: %li", (long) frames_left);
				if(!frames_left)
				{
					if(APPFLAG(MPG123APP_CONTINUE)) end_of_files = TRUE;

					break;
				}
			}
			if(!play_frame()) break;
			if(!param.quiet)
			{
				meta = mpg123_meta_check(mh);
				if(meta & (MPG123_NEW_ID3|MPG123_NEW_ICY))
				{
					if(meta & MPG123_NEW_ID3) print_id3_tag(mh, param.long_id3, stderr);
					if(meta & MPG123_NEW_ICY) print_icy(mh, stderr);

#ifdef HAVE_TERMIOS
					if(!param.term_ctrl) /* Terminal user can query meta data again. */
#endif
					mpg123_meta_free(mh); /* Do not waste memory after delivering. */
				}
			}
			if(!fresh && param.verbose)
			{
				if(param.verbose > 1 || !(framenum & 0x7)) print_stat(mh,0,ao,1);
			}
#ifdef HAVE_TERMIOS
			if(!param.term_ctrl) continue;
			else term_control(mh, ao);
#endif
		}

	if(!param.smooth && !intflag)
		controlled_drain();
	if(param.verbose) print_stat(mh,0,ao,0);

	if(!param.quiet)
	{
		double secs;
		long frank;
		fprintf(stderr, "\n");
		if(mpg123_getstate(mh, MPG123_FRANKENSTEIN, &frank, NULL) == MPG123_OK && frank)
		fprintf(stderr, "This was a Frankenstein track.\n");

		mpg123_position(mh, 0, 0, NULL, NULL, &secs, NULL);
		fprintf(stderr,"[%d:%02d] Decoding of %s finished.\n", (int)(secs / 60), ((int)secs) % 60, filename);
	}
	else if(param.verbose) fprintf(stderr, "\n");

	close_track();

	if (intflag)
	{
		if(!skip_or_die(&start_time)) break;

        intflag = FALSE;

		if(!param.smooth)
			out123_drop(ao);
	}

		if(end_of_files) break;
	} /* end of loop over input files */

	if(APPFLAG(MPG123APP_CONTINUE))
	{
		continue_msg("CONTINUE");
	}

	/* Free up memory used by playlist */    
	if(!param.remote) free_playlist();

	safe_exit(0); /* That closes output and restores terminal, too. */
	return 0;
}

static void print_title(FILE *o)
{
	fprintf(o, "High Performance MPEG 1.0/2.0/2.5 Audio Player for Layers 1, 2 and 3\n");
	fprintf(o, "\tversion %s; written and copyright by Michael Hipp and others\n", PACKAGE_VERSION);
	fprintf(o, "\tfree software (LGPL) without any warranty but with best wishes\n");
}

static void usage(int err)  /* print syntax & exit */
{
	FILE* o = stdout;
	if(err)
	{
		o = stderr; 
		fprintf(o, "You made some mistake in program usage... let me briefly remind you:\n\n");
	}
	print_title(o);
	fprintf(o,"\nusage: %s [option(s)] [file(s) | URL(s) | -]\n", prgName);
	fprintf(o,"supported options [defaults in brackets]:\n");
	fprintf(o,"   -v    increase verbosity level       -q    quiet (don't print title)\n");
	fprintf(o,"   -t    testmode (no output)           -s    write to stdout\n");
	fprintf(o,"   -w f  write output as WAV file\n");
	fprintf(o,"   -k n  skip first n frames [0]        -n n  decode only n frames [all]\n");
	fprintf(o,"   -c    check range violations         -y    DISABLE resync on errors\n");
	fprintf(o,"   -b n  output buffer: n Kbytes [0]    -f n  change scalefactor [%li]\n", param.outscale);
	fprintf(o,"   -r n  set/force samplerate [auto]\n");
	fprintf(o,"   -o m  select output module           -a d  set audio device\n");
	fprintf(o,"   -2    downsample 1:2 (22 kHz)        -4    downsample 1:4 (11 kHz)\n");
	fprintf(o,"   -d n  play every n'th frame only     -h n  play every frame n times\n");
	fprintf(o,"   -0    decode channel 0 (left) only   -1    decode channel 1 (right) only\n");
#ifdef NETWORK
	fprintf(o,"   -m    mix both channels (mono)       -p p  use HTTP proxy p [$HTTP_PROXY]\n");
#else
	fprintf(o,"   -m    mix both channels (mono)\n");
#endif
	#ifdef HAVE_SCHED_SETSCHEDULER
	fprintf(o,"   -@ f  read filenames/URLs from f     -T get realtime priority\n");
	#else
	fprintf(o,"   -@ f  read filenames/URLs from f\n");
	#endif
	fprintf(o,"   -z    shuffle play (with wildcards)  -Z    random play\n");
	fprintf(o,"   -u a  HTTP authentication string     -E f  Equalizer, data from file\n");
#ifdef HAVE_TERMIOS
	fprintf(o,"   -C    enable control keys            --no-gapless  not skip junk/padding in mp3s\n");
#else
	fprintf(o,"                                        --no-gapless  not skip junk/padding in mp3s\n");
#endif
	fprintf(o,"   -?    this help                      --version  print name + version\n");
	fprintf(o,"See the manpage "PACKAGE_NAME"(1) or call %s with --longhelp for more parameters and information.\n", prgName);
	safe_exit(err);
}

static void want_usage(char* arg)
{
	usage(0);
}

static void long_usage(int err)
{
	mpg123_string *enclist;
	FILE* o = stdout;
	if(err)
	{
  	o = stderr; 
  	fprintf(o, "You made some mistake in program usage... let me remind you:\n\n");
	}
	enclist = audio_enclist();
	print_title(o);
	fprintf(o,"\nusage: %s [option(s)] [file(s) | URL(s) | -]\n", prgName);

	fprintf(o,"\ninput options\n\n");
	fprintf(o," -k <n> --skip <n>         skip n frames at beginning\n");
	fprintf(o,"        --skip-id3v2       skip ID3v2 tags without parsing\n");
	fprintf(o," -n     --frames <n>       play only <n> frames of every stream\n");
	fprintf(o,"        --fuzzy            Enable fuzzy seeks (guessing byte offsets or using approximate seek points from Xing TOC)\n");
	fprintf(o," -y     --no-resync        DISABLES resync on error (--resync is deprecated)\n");
#ifdef NETWORK
	fprintf(o," -p <f> --proxy <f>        set WWW proxy\n");
	fprintf(o," -u     --auth             set auth values for HTTP access\n");
	fprintf(o,"        --ignore-mime      ignore HTTP MIME types (content-type)\n");
#endif
	fprintf(o,"        --no-seekbuffer    disable seek buffer\n");
	fprintf(o," -@ <f> --list <f>         play songs in playlist <f> (plain list, m3u, pls (shoutcast))\n");
	fprintf(o," -l <n> --listentry <n>    play nth title in playlist; show whole playlist for n < 0\n");
	fprintf(o,"        --continue         playlist continuation mode (see man page)\n");
	fprintf(o,"        --loop <n>         loop track(s) <n> times, < 0 means infinite loop (not with --random!)\n");
	fprintf(o,"        --keep-open        (--remote mode only) keep loaded file open after reaching end\n");
	fprintf(o,"        --timeout <n>      Timeout in seconds before declaring a stream dead (if <= 0, wait forever)\n");
	fprintf(o," -z     --shuffle          shuffle song-list before playing\n");
	fprintf(o," -Z     --random           full random play\n");
	fprintf(o,"        --no-icy-meta      Do not accept ICY meta data\n");
	fprintf(o," -i     --index            index / scan through the track before playback\n");
	fprintf(o,"        --index-size <n>   change size of frame index\n");
	fprintf(o,"        --preframes  <n>   number of frames to decode in advance after seeking (to keep layer 3 bit reservoir happy)\n");
	fprintf(o,"        --resync-limit <n> Set number of bytes to search for valid MPEG data; <0 means search whole stream.\n");
	fprintf(o,"        --streamdump <f>   Dump a copy of input data (as read by libmpg123) to given file.\n");
	fprintf(o,"        --icy-interval <n> Enforce ICY interval in bytes (for playing a stream dump.\n");
	fprintf(o,"        --ignore-streamlength Ignore header info about length of MPEG streams.");
	fprintf(o,"\noutput/processing options\n\n");
	fprintf(o," -o <o> --output <o>       select audio output module\n");
	fprintf(o,"        --list-modules     list the available modules\n");
	fprintf(o," -a <d> --audiodevice <d>  select audio device (depending on chosen module)\n");
	fprintf(o," -s     --stdout           write raw audio to stdout (host native format)\n");
	fprintf(o," -S     --STDOUT           play AND output stream (not implemented yet)\n");
	fprintf(o," -w <f> --wav <f>          write samples as WAV file in <f> (- is stdout)\n");
	fprintf(o,"        --au <f>           write samples as Sun AU file in <f> (- is stdout)\n");
	fprintf(o,"        --cdr <f>          write samples as raw CD audio file in <f> (- is stdout)\n");
	fprintf(o,"        --reopen           force close/open on audiodevice\n");
	#ifdef OPT_MULTI
	fprintf(o,"        --cpu <string>     set cpu optimization\n");
	fprintf(o,"        --test-cpu         list optimizations possible with cpu and exit\n");
	fprintf(o,"        --list-cpu         list builtin optimizations and exit\n");
	#endif
	#ifdef OPT_3DNOW
	fprintf(o,"        --test-3dnow       display result of 3DNow! autodetect and exit (obsoleted by --cpu)\n");
	fprintf(o,"        --force-3dnow      force use of 3DNow! optimized routine (obsoleted by --test-cpu)\n");
	fprintf(o,"        --no-3dnow         force use of floating-pointer routine (obsoleted by --cpu)\n");
	#endif
	fprintf(o," -g     --gain             [DEPRECATED] set audio hardware output gain\n");
	fprintf(o," -f <n> --scale <n>        scale output samples (soft gain - based on 32768), default=%li)\n", param.outscale);
	fprintf(o,"        --rva-mix,\n");
	fprintf(o,"        --rva-radio        use RVA2/ReplayGain values for mix/radio mode\n");
	fprintf(o,"        --rva-album,\n");
	fprintf(o,"        --rva-audiophile   use RVA2/ReplayGain values for album/audiophile mode\n");
	fprintf(o," -0     --left --single0   play only left channel\n");
	fprintf(o," -1     --right --single1  play only right channel\n");
	fprintf(o," -m     --mono --mix       mix stereo to mono\n");
	fprintf(o,"        --stereo           duplicate mono channel\n");
	fprintf(o," -r     --rate             force a specific audio output rate\n");
	fprintf(o," -2     --2to1             2:1 downsampling\n");
	fprintf(o," -4     --4to1             4:1 downsampling\n");
  fprintf(o,"        --pitch <value>    set hardware pitch (speedup/down, 0 is neutral; 0.05 is 5%%)\n");
	fprintf(o,"        --8bit             force 8 bit output\n");
	fprintf(o,"        --float            force floating point output (internal precision)\n");
	fprintf(o," -e <c> --encoding <c>     force a specific encoding (%s)\n"
	,	enclist != NULL ? enclist->p : "OOM!");
	fprintf(o," -d n   --doublespeed n    play only every nth frame\n");
	fprintf(o," -h n   --halfspeed   n    play every frame n times\n");
	fprintf(o,"        --equalizer        exp.: scales freq. bands acrd. to 'equalizer.dat'\n");
	fprintf(o,"        --gapless          remove padding/junk on mp3s (best with Lame tag)\n");
	fprintf(o,"                           This is on by default when libmpg123 supports it.\n");
	fprintf(o,"        --no-gapless       disable gapless mode, not remove padding/junk\n");
	fprintf(o,"        --no-infoframe     disable parsing of Xing/Lame/VBR/Info frame\n");
	fprintf(o," -D n   --delay n          insert a delay of n seconds before each track\n");
	fprintf(o," -o h   --headphones       (aix/hp/sun) output on headphones\n");
	fprintf(o," -o s   --speaker          (aix/hp/sun) output on speaker\n");
	fprintf(o," -o l   --lineout          (aix/hp/sun) output to lineout\n");
#ifndef NOXFERMEM
	fprintf(o," -b <n> --buffer <n>       set play buffer (\"output cache\")\n");
	fprintf(o,"        --preload <value>  fraction of buffer to fill before playback\n");
	fprintf(o,"        --smooth           keep buffer over track boundaries\n");
#endif
	fprintf(o,"        --devbuffer <s>    set device buffer in seconds; <= 0 means default\n");

	fprintf(o,"\nmisc options\n\n");
	fprintf(o," -t     --test             only decode, no output (benchmark)\n");
	fprintf(o," -c     --check            count and display clipped samples\n");
	fprintf(o," -v[*]  --verbose          increase verboselevel\n");
	fprintf(o," -q     --quiet            quiet mode\n");
	#ifdef HAVE_TERMIOS
	fprintf(o," -C     --control          enable terminal control keys (else auto detect)\n");
	fprintf(o,"        --no-control       disable terminal control keys (disable auto detect)\n");
	fprintf(o,"        --ctrlusr1 <c>     control key (characer) to map to SIGUSR1\n");
	fprintf(o,"                           (default is for stop/start)\n");
	fprintf(o,"        --ctrlusr2 <c>     control key (characer) to map to SIGUSR2\n");
	fprintf(o,"                           (default is for next track)\n");
	#endif
	#ifndef GENERIC
	fprintf(o,"        --title            set terminal title to filename\n");
	#endif
	fprintf(o,"        --name <n>         set instance name (used in various places)\n");
	fprintf(o,"        --long-tag         spacy id3 display with every item on a separate line\n");
	fprintf(o,"        --lyrics           show lyrics (from ID3v2 USLT frame)\n");
	fprintf(o,"        --utf8             Regardless of environment, print metadata in UTF-8.\n");
	fprintf(o," -R     --remote           generic remote interface\n");
	fprintf(o,"        --remote-err       force use of stderr for generic remote interface\n");
#ifdef FIFO
	fprintf(o,"        --fifo <path>      open a FIFO at <path> for commands instead of stdin\n");
#endif
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

	fprintf(o,"\nSee the manpage "PACKAGE_NAME"(1) for more information.\n");
	mpg123_free_string(enclist);
	free(enclist);
	safe_exit(err);
}

static void want_long_usage(char* arg)
{
	long_usage(0);
}

static void give_version(char* arg)
{
	fprintf(stdout, PACKAGE_NAME" "PACKAGE_VERSION"\n");
	safe_exit(0);
}

void continue_msg(const char *name)
{
	fprintf( aux_out, "\n[%s] track %"SIZE_P" frame %"OFF_P"\n", name
	,	(size_p)playlist_pos(NULL, NULL), (off_p)framenum );
}
