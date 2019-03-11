/*

    TiMidity -- Experimental MIDI to WAVE converter
    Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the Perl Artistic License, available in COPYING.
*/

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "timidity.h"

#include "options.h"
#include "common.h"
#include "instrum.h"
#include "playmidi.h"
#include "readmidi.h"
#include "output.h"

#include "tables.h"

ToneBank *master_tonebank[MAXBANK], *master_drumset[MAXBANK];

static char def_instr_name[256] = "";

#define MAXWORDS 10

/* Quick-and-dirty fgets() replacement. */

static char *RWgets(SDL_RWops *rw, char *s, int size)
{
    int num_read = 0;
    char *p = s;

    --size;/* so that we nul terminate properly */

    for (; num_read < size; ++p)
    {
	if (SDL_RWread(rw, p, 1, 1) != 1)
	    break;

	num_read++;

	/* Unlike fgets(), don't store newline. Under Windows/DOS we'll
	 * probably get an extra blank line for every line that's being
	 * read, but that should be ok.
	 */
	if (*p == '\n' || *p == '\r')
	{
	    *p = '\0';
	    return s;
	}
    }

    *p = '\0';

    return (num_read != 0) ? s : NULL;
}

static int read_config_file(const char *name)
{
  SDL_RWops *rw;
  char tmp[1024], *w[MAXWORDS], *cp;
  ToneBank *bank=0;
  int i, j, k, line=0, words;
  static int rcf_count=0;

  if (rcf_count>50)
  {
    SNDDBG(("Probable source loop in configuration files\n"));
    return (-1);
  }

  if (!(rw=open_file(name)))
   return -1;

  while (RWgets(rw, tmp, sizeof(tmp)))
  {
    line++;
    words=0;
    w[0]=strtok(tmp, " \t\240");
    if (!w[0]) continue;

        /* Originally the TiMidity++ extensions were prefixed like this */
    if (strcmp(w[0], "#extension") == 0)
    {
        w[0]=strtok(0, " \t\240");
        if (!w[0]) continue;
    }

    if (*w[0] == '#')
        continue;

    while (w[words] && *w[words] != '#' && (words < (MAXWORDS-1)))
      w[++words]=strtok(0," \t\240");

        /*
         * TiMidity++ adds a number of extensions to the config file format.
         * Many of them are completely irrelevant to SDL_sound, but at least
         * we shouldn't choke on them.
         *
         * Unfortunately the documentation for these extensions is often quite
         * vague, gramatically strange or completely absent.
         */
    if (
           !strcmp(w[0], "comm")      /* "comm" program second        */
        || !strcmp(w[0], "HTTPproxy") /* "HTTPproxy" hostname:port    */
        || !strcmp(w[0], "FTPproxy")  /* "FTPproxy" hostname:port     */
        || !strcmp(w[0], "mailaddr")  /* "mailaddr" your-mail-address */
        || !strcmp(w[0], "opt")       /* "opt" timidity-options       */
       )
    {
            /*
             * + "comm" sets some kind of comment -- the documentation is too
             *   vague for me to understand at this time.
             * + "HTTPproxy", "FTPproxy" and "mailaddr" are for reading data
             *   over a network, rather than from the file system.
             * + "opt" specifies default options for TiMidity++.
             *
             * These are all quite useless for our version of TiMidity, so
             * they can safely remain no-ops.
             */
    } else if (!strcmp(w[0], "timeout")) /* "timeout" program second */
    {
            /*
             * Specifies a timeout value of the program. A number of seconds
             * before TiMidity kills the note. This may be useful to implement
             * later, but I don't see any urgent need for it.
             */
        SNDDBG(("FIXME: Implement \"timeout\" in TiMidity config.\n"));
    } else if (!strcmp(w[0], "copydrumset")  /* "copydrumset" drumset */
               || !strcmp(w[0], "copybank")) /* "copybank" bank       */
    {
            /*
             * Copies all the settings of the specified drumset or bank to
             * the current drumset or bank. May be useful later, but not a
             * high priority.
             */
        SNDDBG(("FIXME: Implement \"%s\" in TiMidity config.\n", w[0]));
    } else if (!strcmp(w[0], "undef")) /* "undef" progno */
    {
            /*
             * Undefines the tone "progno" of the current tone bank (or
             * drum set?). Not a high priority.
             */
        SNDDBG(("FIXME: Implement \"undef\" in TiMidity config.\n"));
    } else if (!strcmp(w[0], "altassign")) /* "altassign" prog1 prog2 ... */
    {
            /*
             * Sets the alternate assign for drum set. Whatever that's
             * supposed to mean.
             */
        SNDDBG(("FIXME: Implement \"altassign\" in TiMidity config.\n"));
    } else if (!strcmp(w[0], "soundfont")
               || !strcmp(w[0], "font"))
    {
            /*
             * I can't find any documentation for these, but I guess they're
             * an alternative way of loading/unloading instruments.
             * 
             * "soundfont" sf_file "remove"
             * "soundfont" sf_file ["order=" order] ["cutoff=" cutoff]
             *                     ["reso=" reso] ["amp=" amp]
             * "font" "exclude" bank preset keynote
             * "font" "order" order bank preset keynote
             */
        SNDDBG(("FIXME: Implmement \"%s\" in TiMidity config.\n", w[0]));
    } else if (!strcmp(w[0], "progbase"))
    {
            /*
             * The documentation for this makes absolutely no sense to me, but
             * apparently it sets some sort of base offset for tone numbers.
             * Why anyone would want to do this is beyond me.
             */
        SNDDBG(("FIXME: Implement \"progbase\" in TiMidity config.\n"));
    } else if (!strcmp(w[0], "map")) /* "map" name set1 elem1 set2 elem2 */
    {
            /*
             * This extension is the one we will need to implement, as it is
             * used by the "eawpats". Unfortunately I cannot find any
             * documentation whatsoever for it, but it looks like it's used
             * for remapping one instrument to another somehow.
             */
        SNDDBG(("FIXME: Implement \"map\" in TiMidity config.\n"));
    }

        /* Standard TiMidity config */
    
    else if (!strcmp(w[0], "dir"))
    {
      if (words < 2)
      {
	SNDDBG(("%s: line %d: No directory given\n", name, line));
	goto fail;
      }
      for (i=1; i<words; i++)
	add_to_pathlist(w[i]);
    }
    else if (!strcmp(w[0], "source"))
    {
      if (words < 2)
      {
	SNDDBG(("%s: line %d: No file name given\n", name, line));
	goto fail;
      }
      for (i=1; i<words; i++)
      {
	int status;
	rcf_count++;
	status = read_config_file(w[i]);
	rcf_count--;
	if (status != 0) {
	  SDL_RWclose(rw);
	  return status;
	}
      }
    }
    else if (!strcmp(w[0], "default"))
    {
      if (words != 2)
      {
	SNDDBG(("%s: line %d: Must specify exactly one patch name\n",
		name, line));
	goto fail;
      }
      strncpy(def_instr_name, w[1], 255);
      def_instr_name[255]='\0';
    }
    else if (!strcmp(w[0], "drumset"))
    {
      if (words < 2)
      {
	SNDDBG(("%s: line %d: No drum set number given\n", name, line));
	goto fail;
      }
      i=atoi(w[1]);
      if (i<0 || i>(MAXBANK-1))
      {
	SNDDBG(("%s: line %d: Drum set must be between 0 and %d\n",
		name, line, MAXBANK-1));
	goto fail;
      }
      if (!master_drumset[i])
      {
	master_drumset[i] = safe_malloc(sizeof(ToneBank));
	memset(master_drumset[i], 0, sizeof(ToneBank));
	master_drumset[i]->tone = safe_malloc(128 * sizeof(ToneBankElement));
	memset(master_drumset[i]->tone, 0, 128 * sizeof(ToneBankElement));
      }
      bank=master_drumset[i];
    }
    else if (!strcmp(w[0], "bank"))
    {
      if (words < 2)
      {
	SNDDBG(("%s: line %d: No bank number given\n", name, line));
	goto fail;
      }
      i=atoi(w[1]);
      if (i<0 || i>(MAXBANK-1))
      {
	SNDDBG(("%s: line %d: Tone bank must be between 0 and %d\n",
		name, line, MAXBANK-1));
	goto fail;
      }
      if (!master_tonebank[i])
      {
	master_tonebank[i] = safe_malloc(sizeof(ToneBank));
	memset(master_tonebank[i], 0, sizeof(ToneBank));
	master_tonebank[i]->tone = safe_malloc(128 * sizeof(ToneBankElement));
	memset(master_tonebank[i]->tone, 0, 128 * sizeof(ToneBankElement));
      }
      bank=master_tonebank[i];
    }
    else
    {
      if ((words < 2) || (*w[0] < '0' || *w[0] > '9'))
      {
	SNDDBG(("%s: line %d: syntax error\n", name, line));
	continue;
      }
      i=atoi(w[0]);
      if (i<0 || i>127)
      {
	SNDDBG(("%s: line %d: Program must be between 0 and 127\n",
		name, line));
	goto fail;
      }
      if (!bank)
      {
	SNDDBG(("%s: line %d: Must specify tone bank or drum set before assignment\n",
		name, line));
	goto fail;
      }
      if (bank->tone[i].name)
	free(bank->tone[i].name);
      strcpy((bank->tone[i].name=safe_malloc(strlen(w[1])+1)),w[1]);
      bank->tone[i].note=bank->tone[i].amp=bank->tone[i].pan=
      bank->tone[i].strip_loop=bank->tone[i].strip_envelope=
      bank->tone[i].strip_tail=-1;

      for (j=2; j<words; j++)
      {
	if (!(cp=strchr(w[j], '=')))
	{
	  SNDDBG(("%s: line %d: bad patch option %s\n", name, line, w[j]));
	  goto fail;
	}
	*cp++=0;
	if (!strcmp(w[j], "amp"))
	{
	  k=atoi(cp);
	  if ((k<0 || k>MAX_AMPLIFICATION) || (*cp < '0' || *cp > '9'))
	  {
	    SNDDBG(("%s: line %d: amplification must be between 0 and %d\n",
		    name, line, MAX_AMPLIFICATION));
	    goto fail;
	  }
	  bank->tone[i].amp=k;
	}
	else if (!strcmp(w[j], "note"))
	{
	  k=atoi(cp);
	  if ((k<0 || k>127) || (*cp < '0' || *cp > '9'))
	  {
	    SNDDBG(("%s: line %d: note must be between 0 and 127\n",
		    name, line));
	    goto fail;
	  }
	  bank->tone[i].note=k;
	}
	else if (!strcmp(w[j], "pan"))
	{
	  if (!strcmp(cp, "center"))
	    k=64;
	  else if (!strcmp(cp, "left"))
	    k=0;
	  else if (!strcmp(cp, "right"))
	    k=127;
	  else
	    k=((atoi(cp)+100) * 100) / 157;
	  if ((k<0 || k>127) || (k==0 && *cp!='-' && (*cp < '0' || *cp > '9')))
	  {
	    SNDDBG(("%s: line %d: panning must be left, right, center, or between -100 and 100\n",
		    name, line));
	    goto fail;
	  }
	  bank->tone[i].pan=k;
	}
	else if (!strcmp(w[j], "keep"))
	{
	  if (!strcmp(cp, "env"))
	    bank->tone[i].strip_envelope=0;
	  else if (!strcmp(cp, "loop"))
	    bank->tone[i].strip_loop=0;
	  else
	  {
	    SNDDBG(("%s: line %d: keep must be env or loop\n", name, line));
	    goto fail;
	  }
	}
	else if (!strcmp(w[j], "strip"))
	{
	  if (!strcmp(cp, "env"))
	    bank->tone[i].strip_envelope=1;
	  else if (!strcmp(cp, "loop"))
	    bank->tone[i].strip_loop=1;
	  else if (!strcmp(cp, "tail"))
	    bank->tone[i].strip_tail=1;
	  else
	  {
	    SNDDBG(("%s: line %d: strip must be env, loop, or tail\n",
		    name, line));
	    goto fail;
	  }
	}
	else
	{
	  SNDDBG(("%s: line %d: bad patch option %s\n", name, line, w[j]));
	  goto fail;
	}
      }
    }
  }
  SDL_RWclose(rw);
  return 0;
fail:
  SDL_RWclose(rw);
  return -2;
}

int Timidity_Init_NoConfig()
{
  /* Allocate memory for the standard tonebank and drumset */
  master_tonebank[0] = safe_malloc(sizeof(ToneBank));
  memset(master_tonebank[0], 0, sizeof(ToneBank));
  master_tonebank[0]->tone = safe_malloc(128 * sizeof(ToneBankElement));
  memset(master_tonebank[0]->tone, 0, 128 * sizeof(ToneBankElement));

  master_drumset[0] = safe_malloc(sizeof(ToneBank));
  memset(master_drumset[0], 0, sizeof(ToneBank));
  master_drumset[0]->tone = safe_malloc(128 * sizeof(ToneBankElement));
  memset(master_drumset[0]->tone, 0, 128 * sizeof(ToneBankElement));

  return 0;
}

int Timidity_Init()
{
  const char *env = SDL_getenv("TIMIDITY_CFG");

  /* !!! FIXME: This may be ugly, but slightly less so than requiring the
   *            default search path to have only one element. I think.
   *
   *            We only need to include the likely locations for the config
   *            file itself since that file should contain any other directory
   *            that needs to be added to the search path.
   */
#ifdef DEFAULT_PATH
    add_to_pathlist(DEFAULT_PATH);
#endif
#ifdef DEFAULT_PATH1
    add_to_pathlist(DEFAULT_PATH1);
#endif
#ifdef DEFAULT_PATH2
    add_to_pathlist(DEFAULT_PATH2);
#endif
#ifdef DEFAULT_PATH3
    add_to_pathlist(DEFAULT_PATH3);
#endif

  Timidity_Init_NoConfig();

  if (!env || read_config_file(env)<0) {
    if (read_config_file(CONFIG_FILE)<0) {
      if (read_config_file(CONFIG_FILE_ETC)<0) {
        if (read_config_file(CONFIG_FILE_ETC_TIMIDITY_FREEPATS)<0) {
          return(-1);
        }
      }
    }
  }
  return 0;
}

MidiSong *Timidity_LoadSong(SDL_RWops *rw, SDL_AudioSpec *audio)
{
  MidiSong *song;
  int i;

  if (rw == NULL)
      return NULL;

  /* Allocate memory for the song */
  song = (MidiSong *)safe_malloc(sizeof(*song));
  if (song == NULL)
      return NULL;
  memset(song, 0, sizeof(*song));

  for (i = 0; i < MAXBANK; i++)
  {
    if (master_tonebank[i])
    {
      song->tonebank[i] = safe_malloc(sizeof(ToneBank));
      memset(song->tonebank[i], 0, sizeof(ToneBank));
      song->tonebank[i]->tone = master_tonebank[i]->tone;
    }
    if (master_drumset[i])
    {
      song->drumset[i] = safe_malloc(sizeof(ToneBank));
      memset(song->drumset[i], 0, sizeof(ToneBank));
      song->drumset[i]->tone = master_drumset[i]->tone;
    }
  }

  song->amplification = DEFAULT_AMPLIFICATION;
  song->voices = DEFAULT_VOICES;
  song->drumchannels = DEFAULT_DRUMCHANNELS;

  song->rw = rw;

  song->rate = audio->freq;
  song->encoding = 0;
  if ((audio->format & 0xFF) == 16)
      song->encoding |= PE_16BIT;
  else if ((audio->format & 0xFF) == 32)
      song->encoding |= PE_32BIT;
  if (audio->format & 0x8000)
      song->encoding |= PE_SIGNED;
  if (audio->channels == 1)
      song->encoding |= PE_MONO;
  else if (audio->channels > 2) {
      SDL_SetError("Surround sound not supported");
      free(song);
      return NULL;
  }
  switch (audio->format) {
  case AUDIO_S8:
	  song->write = s32tos8;
	  break;
  case AUDIO_U8:
	  song->write = s32tou8;
	  break;
  case AUDIO_S16LSB:
	  song->write = s32tos16l;
	  break;
  case AUDIO_S16MSB:
	  song->write = s32tos16b;
	  break;
  case AUDIO_U16LSB:
	  song->write = s32tou16l;
	  break;
  case AUDIO_U16MSB:
	  song->write = s32tou16b;
	  break;
  case AUDIO_S32LSB:
	  song->write = s32tos32l;
	  break;
  case AUDIO_S32MSB:
	  song->write = s32tos32b;
	  break;
  case AUDIO_F32SYS:
	  song->write = s32tof32;
	  break;
  default:
	  SDL_SetError("Unsupported audio format");
	  free(song);
	  return NULL;
  }

  song->buffer_size = audio->samples;
  song->resample_buffer = safe_malloc(audio->samples * sizeof(sample_t));
  song->common_buffer = safe_malloc(audio->samples * 2 * sizeof(Sint32));
  
  song->control_ratio = audio->freq / CONTROLS_PER_SECOND;
  if (song->control_ratio < 1)
      song->control_ratio = 1;
  else if (song->control_ratio > MAX_CONTROL_RATIO)
      song->control_ratio = MAX_CONTROL_RATIO;

  song->lost_notes = 0;
  song->cut_notes = 0;

  song->events = read_midi_file(song, &(song->groomed_event_count),
      &song->samples);

  /* The RWops can safely be closed at this point, but let's make that the
   * responsibility of the caller.
   */
  
  /* Make sure everything is okay */
  if (!song->events) {
    free(song);
    return(NULL);
  }

  song->default_instrument = 0;
  song->default_program = DEFAULT_PROGRAM;

  if (*def_instr_name)
    set_default_instrument(song, def_instr_name);

  load_missing_instruments(song);

  return(song);
}

void Timidity_FreeSong(MidiSong *song)
{
  int i;

  free_instruments(song);

  for (i = 0; i < 128; i++)
  {
    if (song->tonebank[i])
      free(song->tonebank[i]);
    if (song->drumset[i])
      free(song->drumset[i]);
  }
  
  free(song->common_buffer);
  free(song->resample_buffer);
  free(song->events);
  free(song);
}

void Timidity_Exit(void)
{
  int i, j;

  for (i = 0; i < MAXBANK; i++)
  {
    if (master_tonebank[i])
    {
      ToneBankElement *e = master_tonebank[i]->tone;
      if (e != NULL)
      {
        for (j = 0; j < 128; j++)
        {
          if (e[j].name != NULL)
            free(e[j].name);
        }
        free(e);
      }
      free(master_tonebank[i]);
      master_tonebank[i] = NULL;
    }
    if (master_drumset[i])
    {
      ToneBankElement *e = master_drumset[i]->tone;
      if (e != NULL)
      {
        for (j = 0; j < 128; j++)
        {
          if (e[j].name != NULL)
            free(e[j].name);
        }
        free(e);
      }
      free(master_drumset[i]);
      master_drumset[i] = NULL;
    }
  }

  free_pathlist();
}
