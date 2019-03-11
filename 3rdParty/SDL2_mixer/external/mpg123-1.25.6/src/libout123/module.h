/*
	module: module loading and listing interface

	copyright ?-2015 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Nicholas J. Humphfrey
*/

#ifndef _MPG123_MODULE_H_
#define _MPG123_MODULE_H_

#include "out123.h"

/* TODO: put that into out123_int.h instead? */
#define MPG123_MODULE_API_VERSION		(2)

/* The full structure is delared in audio.h */
struct audio_output_struct;

typedef struct mpg123_module_struct {
	const int api_version;						/* module API version number */

	const char* name;							/* short name of the module */
	const char* description;					/* description of what the module does */
	const char* revision;						/* source code revision */
	
	void* handle; /* dynamic loader handle */

	/* Initialisers - set to NULL if unsupported by module */
	int (*init_output)(out123_handle *ao);		/* audio output - returns 0 on success */

} mpg123_module_t;



/* ------ Declarations from "module.c" ------ */

mpg123_module_t* open_module( const char* type, const char* name, int verbose
,	const char* bindir );
void close_module(mpg123_module_t* module, int verbose);
int list_modules( const char *type, char ***names, char ***descr, int verbose
,	const char* bindir );

#endif
