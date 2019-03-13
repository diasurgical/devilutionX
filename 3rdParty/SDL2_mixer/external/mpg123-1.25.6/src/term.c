/*
	term: terminal control

	copyright ?-2015 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Michael Hipp
*/

#include "mpg123app.h"

#ifdef HAVE_TERMIOS

#include <termios.h>
#include <ctype.h>

#include "term.h"
#include "common.h"
#include "playlist.h"
#include "metaprint.h"
#include "debug.h"

static int term_enable = 0;
static struct termios old_tio;
int seeking = FALSE;

extern out123_handle *ao;

/* Buffered key from a signal or whatnot.
   We ignore the null character... */
static char prekey = 0;

/* Hm, next step would be some system in this, plus configurability...
   Two keys for everything? It's just stop/pause for now... */
struct keydef { const char key; const char key2; const char* desc; };
struct keydef term_help[] =
{
	 { MPG123_STOP_KEY,  ' ', "interrupt/restart playback (i.e. '(un)pause')" }
	,{ MPG123_NEXT_KEY,    0, "next track" }
	,{ MPG123_PREV_KEY,    0, "previous track" }
	,{ MPG123_NEXT_DIR_KEY, 0, "next directory (next track until directory part changes)" }
	,{ MPG123_PREV_DIR_KEY, 0, "previous directory (previous track until directory part changes)" }
	,{ MPG123_BACK_KEY,    0, "back to beginning of track" }
	,{ MPG123_PAUSE_KEY,   0, "loop around current position (don't combine with output buffer)" }
	,{ MPG123_FORWARD_KEY, 0, "forward" }
	,{ MPG123_REWIND_KEY,  0, "rewind" }
	,{ MPG123_FAST_FORWARD_KEY, 0, "fast forward" }
	,{ MPG123_FAST_REWIND_KEY,  0, "fast rewind" }
	,{ MPG123_FINE_FORWARD_KEY, 0, "fine forward" }
	,{ MPG123_FINE_REWIND_KEY,  0, "fine rewind" }
	,{ MPG123_VOL_UP_KEY,   0, "volume up" }
	,{ MPG123_VOL_DOWN_KEY, 0, "volume down" }
	,{ MPG123_RVA_KEY,      0, "RVA switch" }
	,{ MPG123_VERBOSE_KEY,  0, "verbose switch" }
	,{ MPG123_PLAYLIST_KEY, 0, "list current playlist, indicating current track there" }
	,{ MPG123_TAG_KEY,      0, "display tag info (again)" }
	,{ MPG123_MPEG_KEY,     0, "print MPEG header info (again)" }
	,{ MPG123_HELP_KEY,     0, "this help" }
	,{ MPG123_QUIT_KEY,     0, "quit" }
	,{ MPG123_PITCH_UP_KEY, MPG123_PITCH_BUP_KEY, "pitch up (small step, big step)" }
	,{ MPG123_PITCH_DOWN_KEY, MPG123_PITCH_BDOWN_KEY, "pitch down (small step, big step)" }
	,{ MPG123_PITCH_ZERO_KEY, 0, "reset pitch to zero" }
	,{ MPG123_BOOKMARK_KEY, 0, "print out current position in playlist and track, for the benefit of some external tool to store bookmarks" }
};

void term_sigcont(int sig);
static void term_sigusr(int sig);

/* This must call only functions safe inside a signal handler. */
int term_setup(struct termios *pattern)
{
	struct termios tio = *pattern;

	/* One might want to use sigaction instead. */
	signal(SIGCONT, term_sigcont);
	signal(SIGUSR1, term_sigusr);
	signal(SIGUSR2, term_sigusr);

	tio.c_lflag &= ~(ICANON|ECHO); 
	tio.c_cc[VMIN] = 1;
	tio.c_cc[VTIME] = 0;
	return tcsetattr(0,TCSANOW,&tio);
}

void term_sigcont(int sig)
{
	term_enable = 0;

	if (term_setup(&old_tio) < 0)
	{
		fprintf(stderr,"Can't set terminal attributes\n");
		return;
	}

	term_enable = 1;
}

static void term_sigusr(int sig)
{
	switch(sig)
	{
		case SIGUSR1: prekey=*param.term_usr1; break;
		case SIGUSR2: prekey=*param.term_usr2; break;
	}
}

/* initialze terminal */
void term_init(void)
{
	const char hide_cursor[] = "\x1b[?25l";
	debug("term_init");

	if(term_width(STDERR_FILENO) >= 0)
		write(STDERR_FILENO, hide_cursor, sizeof(hide_cursor));

	debug1("param.term_ctrl: %i", param.term_ctrl);
	if(!param.term_ctrl)
		return;

	term_enable = 0;

	if(tcgetattr(0,&old_tio) < 0)
	{
		fprintf(stderr,"Can't get terminal attributes\n");
		return;
	}
	if(term_setup(&old_tio) < 0)
	{
		fprintf(stderr,"Can't set terminal attributes\n");
		return;
	}

	term_enable = 1;
}

void term_hint(void)
{
	if(term_enable)
		fprintf(stderr, "\nTerminal control enabled, press 'h' for listing of keys and functions.\n\n");
}

static void term_handle_input(mpg123_handle *, out123_handle *, int);

static int pause_cycle;

static int print_index(mpg123_handle *mh)
{
	int err;
	size_t c, fill;
	off_t *index;
	off_t  step;
	err = mpg123_index(mh, &index, &step, &fill);
	if(err == MPG123_ERR)
	{
		fprintf(stderr, "Error accessing frame index: %s\n", mpg123_strerror(mh));
		return err;
	}
	for(c=0; c < fill;++c) 
		fprintf(stderr, "[%lu] %lu: %li (+%li)\n",
		(unsigned long) c,
		(unsigned long) (c*step), 
		(long) index[c], 
		(long) (c ? index[c]-index[c-1] : 0));
	return MPG123_OK;
}

static off_t offset = 0;

/* Go back to the start for the cyclic pausing. */
void pause_recycle(mpg123_handle *fr)
{
	/* Take care not to go backwards in time in steps of 1 frame
		 That is what the +1 is for. */
	pause_cycle=(int)(LOOP_CYCLES/mpg123_tpf(fr));
	offset-=pause_cycle;
}

/* Done with pausing, no offset anymore. Just continuous playback from now. */
void pause_uncycle(void)
{
	offset += pause_cycle;
}

off_t term_control(mpg123_handle *fr, out123_handle *ao)
{
	offset = 0;
	debug2("control for frame: %li, enable: %i", (long)mpg123_tellframe(fr), term_enable);
	if(!term_enable) return 0;

	if(paused)
	{
		/* pause_cycle counts the remaining frames _after_ this one, thus <0, not ==0 . */
		if(--pause_cycle < 0)
			pause_recycle(fr);
	}

	do
	{
		off_t old_offset = offset;
		term_handle_input(fr, ao, stopped|seeking);
		if((offset < 0) && (-offset > framenum)) offset = - framenum;
		if(param.verbose && offset != old_offset)
			print_stat(fr,offset,ao,1);
	} while (!intflag && stopped);

	/* Make the seeking experience with buffer less annoying.
	   No sound during seek, but at least it is possible to go backwards. */
	if(offset)
	{
		if((offset = mpg123_seek_frame(fr, offset, SEEK_CUR)) >= 0)
		debug1("seeked to %li", (long)offset);
		else error1("seek failed: %s!", mpg123_strerror(fr));
		/* Buffer resync already happened on un-stop? */
		/* if(param.usebuffer) audio_drop(ao);*/
	}
	return 0;
}

/* Stop playback while seeking if buffer is involved. */
static void seekmode(mpg123_handle *mh, out123_handle *ao)
{
	if(param.usebuffer && !stopped)
	{
		int channels = 0;
		int encoding = 0;
		int pcmframe;
		off_t back_samples = 0;

		stopped = TRUE;
		out123_pause(ao);
		if(param.verbose)
			print_stat(mh, 0, ao, 0);
		mpg123_getformat(mh, NULL, &channels, &encoding);
		pcmframe = out123_encsize(encoding)*channels;
		if(pcmframe > 0)
			back_samples = out123_buffered(ao)/pcmframe;
		if(param.verbose > 2)
			fprintf(stderr, "\nseeking back %"OFF_P" samples from %"OFF_P"\n"
			,	(off_p)back_samples, (off_p)mpg123_tell(mh));
		mpg123_seek(mh, -back_samples, SEEK_CUR);
		out123_drop(ao);
		if(param.verbose > 2)
			fprintf(stderr, "\ndropped, now at %"OFF_P"\n"
			,	(off_p)mpg123_tell(mh));
		fprintf(stderr, "%s", MPG123_STOPPED_STRING);
		if(param.verbose)
			print_stat(mh, 0, ao, 1);
	}
}

/* Get the next pressed key, if any.
   Returns 1 when there is a key, 0 if not. */
static int get_key(int do_delay, char *val)
{
	fd_set r;
	struct timeval t;

	/* Shortcut: If some other means sent a key, use it. */
	if(prekey)
	{
		debug1("Got prekey: %c\n", prekey);
		*val = prekey;
		prekey = 0;
		return 1;
	}

	t.tv_sec=0;
	t.tv_usec=(do_delay) ? 10*1000 : 0;

	FD_ZERO(&r);
	FD_SET(0,&r);
	if(select(1,&r,NULL,NULL,&t) > 0 && FD_ISSET(0,&r))
	{
		if(read(0,val,1) <= 0)
		return 0; /* Well, we couldn't read the key, so there is none. */
		else
		return 1;
	}
	else return 0;
}

static void term_handle_key(mpg123_handle *fr, out123_handle *ao, char val)
{
	debug1("term_handle_key: %c", val);
	switch(tolower(val))
	{
	case MPG123_BACK_KEY:
		out123_pause(ao);
		out123_drop(ao);
		if(paused) pause_cycle=(int)(LOOP_CYCLES/mpg123_tpf(fr));

		if(mpg123_seek_frame(fr, 0, SEEK_SET) < 0)
		error1("Seek to begin failed: %s", mpg123_strerror(fr));

		framenum=0;
	break;
	case MPG123_NEXT_KEY:
		out123_pause(ao);
		out123_drop(ao);
		next_track();
	break;
	case MPG123_NEXT_DIR_KEY:
		out123_pause(ao);
		out123_drop(ao);
		next_dir();
	break;
	case MPG123_QUIT_KEY:
		debug("QUIT");
		if(stopped)
		{
			stopped = 0;
			out123_pause(ao); /* no chance for annoying underrun warnings */
			out123_drop(ao);
		}
		set_intflag();
		offset = 0;
	break;
	case MPG123_PAUSE_KEY:
		paused=1-paused;
		out123_pause(ao); /* underrun awareness */
		out123_drop(ao);
		if(paused)
		{
			/* Not really sure if that is what is wanted
				 This jumps in audio output, but has direct reaction to pausing loop. */
			out123_param_float(ao, OUT123_PRELOAD, 0.);
			pause_recycle(fr);
		}
		else
			out123_param_float(ao, OUT123_PRELOAD, param.preload);
		if(stopped)
			stopped=0;
		if(param.verbose)
			print_stat(fr, 0, ao, 1);
		else
			fprintf(stderr, "%s", (paused) ? MPG123_PAUSED_STRING : MPG123_EMPTY_STRING);
	break;
	case MPG123_STOP_KEY:
	case ' ':
		/* TODO: Verify/ensure that there is no "chirp from the past" when
		   seeking while stopped. */
		stopped=1-stopped;
		if(paused) {
			paused=0;
			offset -= pause_cycle;
		}
		if(stopped)
			out123_pause(ao);
		else
		{
			if(offset) /* If position changed, old is outdated. */
				out123_drop(ao);
			/* No out123_continue(), that's triggered by out123_play(). */
		}
		if(param.verbose)
			print_stat(fr, 0, ao, 1);
		else
			fprintf(stderr, "%s", (stopped) ? MPG123_STOPPED_STRING : MPG123_EMPTY_STRING);
	break;
	case MPG123_FINE_REWIND_KEY:
		seekmode(fr, ao);
		offset--;
	break;
	case MPG123_FINE_FORWARD_KEY:
		seekmode(fr, ao);
		offset++;
	break;
	case MPG123_REWIND_KEY:
		seekmode(fr, ao);
		  offset-=10;
	break;
	case MPG123_FORWARD_KEY:
		seekmode(fr, ao);
		offset+=10;
	break;
	case MPG123_FAST_REWIND_KEY:
		seekmode(fr, ao);
		offset-=50;
	break;
	case MPG123_FAST_FORWARD_KEY:
		seekmode(fr, ao);
		offset+=50;
	break;
	case MPG123_VOL_UP_KEY:
		mpg123_volume_change(fr, 0.02);
	break;
	case MPG123_VOL_DOWN_KEY:
		mpg123_volume_change(fr, -0.02);
	break;
	case MPG123_PITCH_UP_KEY:
	case MPG123_PITCH_BUP_KEY:
	case MPG123_PITCH_DOWN_KEY:
	case MPG123_PITCH_BDOWN_KEY:
	case MPG123_PITCH_ZERO_KEY:
	{
		double new_pitch = param.pitch;
		switch(val) /* Not tolower here! */
		{
			case MPG123_PITCH_UP_KEY:    new_pitch += MPG123_PITCH_VAL;  break;
			case MPG123_PITCH_BUP_KEY:   new_pitch += MPG123_PITCH_BVAL; break;
			case MPG123_PITCH_DOWN_KEY:  new_pitch -= MPG123_PITCH_VAL;  break;
			case MPG123_PITCH_BDOWN_KEY: new_pitch -= MPG123_PITCH_BVAL; break;
			case MPG123_PITCH_ZERO_KEY:  new_pitch = 0.0; break;
		}
		set_pitch(fr, ao, new_pitch);
		if(param.verbose > 1)
		{
			print_stat(fr,0,ao,0);
			fprintf(stderr, "\nNew pitch: %f\n", param.pitch);
			print_stat(fr,0,ao,1);
		}
	}
	break;
	case MPG123_VERBOSE_KEY:
		param.verbose++;
		if(param.verbose > VERBOSE_MAX)
		{
			param.verbose = 0;
			clear_stat();
		}
		mpg123_param(fr, MPG123_VERBOSE, param.verbose, 0);
	break;
	case MPG123_RVA_KEY:
		if(++param.rva > MPG123_RVA_MAX) param.rva = 0;
		mpg123_param(fr, MPG123_RVA, param.rva, 0);
		mpg123_volume_change(fr, 0.);
		if(param.verbose)
			print_stat(fr,0,ao,1);
	break;
	case MPG123_PREV_KEY:
		out123_pause(ao);
		out123_drop(ao);

		prev_track();
	break;
	case MPG123_PREV_DIR_KEY:
		out123_pause(ao);
		out123_drop(ao);
		prev_dir();
	break;
	case MPG123_PLAYLIST_KEY:
		if(param.verbose)
			print_stat(fr,0,ao,0);
		fprintf(stderr, "%s\nPlaylist (\">\" indicates current track):\n", param.verbose ? "\n" : "");
		print_playlist(stderr, 1);
		fprintf(stderr, "\n");
	break;
	case MPG123_TAG_KEY:
		if(param.verbose)
			print_stat(fr,0,ao,0);
		fprintf(stderr, "%s\n", param.verbose ? "\n" : "");
		print_id3_tag(fr, param.long_id3, stderr);
		fprintf(stderr, "\n");
	break;
	case MPG123_MPEG_KEY:
		if(param.verbose)
			print_stat(fr,0,ao,0);
		fprintf(stderr, "\n");
		if(param.verbose > 1)
			print_header(fr);
		else
			print_header_compact(fr);
		fprintf(stderr, "\n");
	break;
	case MPG123_HELP_KEY:
	{ /* This is more than the one-liner before, but it's less spaghetti. */
		int i;
		if(param.verbose)
			print_stat(fr,0,ao,0);
		fprintf(stderr,"\n\n -= terminal control keys =-\n");
		for(i=0; i<(sizeof(term_help)/sizeof(struct keydef)); ++i)
		{
			if(term_help[i].key2) fprintf(stderr, "[%c] or [%c]", term_help[i].key, term_help[i].key2);
			else fprintf(stderr, "[%c]", term_help[i].key);

			fprintf(stderr, "\t%s\n", term_help[i].desc);
		}
		fprintf(stderr, "\nAlso, the number row (starting at 1, ending at 0) gives you jump points into the current track at 10%% intervals.\n");
		fprintf(stderr, "\n");
	}
	break;
	case MPG123_FRAME_INDEX_KEY:
	case MPG123_VARIOUS_INFO_KEY:
		if(param.verbose) fprintf(stderr, "\n");
		switch(val) /* because of tolower() ... */
		{
			case MPG123_FRAME_INDEX_KEY:
			print_index(fr);
			{
				long accurate;
				if(mpg123_getstate(fr, MPG123_ACCURATE, &accurate, NULL) == MPG123_OK)
				fprintf(stderr, "Accurate position: %s\n", (accurate == 0 ? "no" : "yes"));
				else
				error1("Unable to get state: %s", mpg123_strerror(fr));
			}
			break;
			case MPG123_VARIOUS_INFO_KEY:
			{
				const char* curdec = mpg123_current_decoder(fr);
				if(curdec == NULL) fprintf(stderr, "Cannot get decoder info!\n");
				else fprintf(stderr, "Active decoder: %s\n", curdec);
			}
		}
	break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{
		off_t len;
		int num;
		num = val == '0' ? 10 : val - '0';
		--num; /* from 0 to 9 */

		/* Do not swith to seekmode() here, as we are jumping once to a
		   specific position. Dropping buffer contents is enough and there
		   is no race filling the buffer or waiting for more incremental
		   seek orders. */
		len = mpg123_length(fr);
		out123_pause(ao);
		out123_drop(ao);
		if(len > 0)
			mpg123_seek(fr, (off_t)( (num/10.)*len ), SEEK_SET);
	}
	break;
	case MPG123_BOOKMARK_KEY:
		continue_msg("BOOKMARK");
	break;
	default:
		;
	}
}

static void term_handle_input(mpg123_handle *fr, out123_handle *ao, int do_delay)
{
	char val;
	/* Do we really want that while loop? This means possibly handling multiple inputs that come very rapidly in one go. */
	while(get_key(do_delay, &val))
	{
		term_handle_key(fr, ao, val);
	}
}

void term_exit(void)
{
	const char cursor_restore[] = "\x1b[?25h";
	/* Bring cursor back. */
	if(term_width(STDERR_FILENO) >= 0)
		write(STDERR_FILENO, cursor_restore, sizeof(cursor_restore));

	if(!term_enable) return;

	tcsetattr(0,TCSAFLUSH,&old_tio);
}

#endif

