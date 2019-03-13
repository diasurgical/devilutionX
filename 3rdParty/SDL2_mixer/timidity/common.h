/*

    TiMidity -- Experimental MIDI to WAVE converter
    Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the Perl Artistic License, available in COPYING.

   common.h
*/

typedef struct {
  char *path;
  void *next;
} PathList;

extern SDL_RWops *open_file(const char *name);
extern void add_to_pathlist(const char *s);
extern void *safe_malloc(size_t count);
extern void free_pathlist(void);
