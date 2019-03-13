/*
	module.c: modular code loader

	copyright 1995-2015 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Nicholas J Humfrey
*/

/* Need snprintf(). */
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#include "config.h"
#include "intsym.h"
#include "stringlists.h"
#include "compat.h"
#include <errno.h>

#include "module.h"
#include "debug.h"

#ifndef USE_MODULES
#error This is a build without modules. Why am I here?
#endif

#define MODULE_SYMBOL_PREFIX "mpg123_"
#define MODULE_SYMBOL_SUFFIX "_module_info"

/* Windows code can convert these from UTF-8 (or ASCII, does not matter)
   to wide and then replace / by \. No need to define another list. */
static const char* modulesearch[] =
{
	 "../lib/mpg123"
	,"plugins"
	,"libout123/modules/.libs"
	,"libout123/modules"
	,"../libout123/modules/.libs"
	,"../libout123/modules"
};

static char *get_module_dir(int verbose, const char* bindir)
{
	char *moddir = NULL;
	char *defaultdir;
	/* First the environment override, then relative to bindir, then installation prefix. */
	defaultdir = compat_getenv("MPG123_MODDIR");
	if(defaultdir)
	{
		if(verbose > 1)
			fprintf(stderr, "Trying module directory from environment: %s\n", defaultdir);
		if(compat_isdir(defaultdir))
			moddir = defaultdir;
		else
			free(defaultdir);
	}
	else
	{
		if(bindir) /* Search relative to binary. */
		{
			size_t i;
			if(verbose > 1)
				fprintf(stderr, "Module dir search relative to: %s\n", bindir);
			for(i=0; i<sizeof(modulesearch)/sizeof(char*); ++i)
			{
				moddir = compat_catpath(bindir, modulesearch[i]);
				if(!moddir)
					continue;
				if(verbose > 1)
					fprintf(stderr, "Looking for module dir: %s\n", moddir);
				if(compat_isdir(moddir))
					break; /* found it! */
				else
				{
					free(moddir);
					moddir=NULL;
				}
			}
		}
		if(!moddir) /* Resort to installation prefix. */
		{
			if(compat_isdir(PKGLIBDIR))
			{
				if(verbose > 1)
					fprintf(stderr, "Using default module dir: %s\n", PKGLIBDIR);
				moddir = compat_strdup(PKGLIBDIR);
			}
		}
	}

	if(verbose > 1)
		fprintf(stderr, "Module dir: %s\n", moddir != NULL ? moddir : "<nil>");
	return moddir;
}

/* Open a module in given directory. */
mpg123_module_t* open_module_here( const char *dir, const char* type
,	const char* name, int verbose )
{
	void *handle = NULL;
	mpg123_module_t *module = NULL;
	char *module_file = NULL;
	size_t module_file_len = 0;
	char *module_symbol = NULL;
	size_t module_symbol_len = 0;
	char *module_path = NULL;

	/* Work out the path of the module to open */
	module_file_len = strlen(type) + 1 + strlen(name) + strlen(LT_MODULE_EXT) + 1;
	module_file = malloc(module_file_len);
	if(!module_file)
	{
		if(verbose > -1)
			error1( "Failed to allocate memory for module name: %s", strerror(errno) );
		return NULL;
	}
	snprintf(module_file, module_file_len, "%s_%s%s", type, name, LT_MODULE_EXT);
	module_path = compat_catpath(dir, module_file);
	free(module_file);
	if(!module_path)
	{
		if(verbose > -1)
			error("Failed to construct full path (out of memory?).");
		return NULL;
	}
	if(verbose > 1)
		fprintf(stderr, "Module path: %s\n", module_path );

	/* Open the module */
	handle = compat_dlopen(module_path);
	free(module_path);
	if (handle==NULL)
	{
		if(verbose > -1)
			error1("Failed to open module %s.", name);
		return NULL;
	}
	
	/* Work out the symbol name */
	module_symbol_len = strlen( MODULE_SYMBOL_PREFIX ) +
						strlen( type )  +
						strlen( MODULE_SYMBOL_SUFFIX ) + 1;
	module_symbol = malloc(module_symbol_len);
	if (module_symbol == NULL) {
		if(verbose > -1)
			error1( "Failed to allocate memory for module symbol: %s", strerror(errno) );
		return NULL;
	}
	snprintf( module_symbol, module_symbol_len, "%s%s%s", MODULE_SYMBOL_PREFIX, type, MODULE_SYMBOL_SUFFIX );
	debug1( "Module symbol: %s", module_symbol );
	
	/* Get the information structure from the module */
	module = (mpg123_module_t*)compat_dlsym(handle, module_symbol);
	free( module_symbol );
	if (module==NULL) {
		if(verbose > -1)
			error("Failed to get module symbol.");
		return NULL;
	}
	
	/* Check the API version */
	if (MPG123_MODULE_API_VERSION != module->api_version)
	{
		if(verbose > -1)
			error2( "API version of module does not match (got %i, expected %i).", module->api_version, MPG123_MODULE_API_VERSION);
		compat_dlclose(handle);
		return NULL;
	}

	/* Store handle in the data structure */
	module->handle = handle;
	return module;
}


/* Open a module, including directory search. */
mpg123_module_t* open_module( const char* type, const char* name, int verbose
,	const char* bindir )
{
	mpg123_module_t *module = NULL;
	char *moddir  = NULL;

	moddir = get_module_dir(verbose, bindir);
	if(!moddir)
	{
		if(verbose > -1)
			error("Failure getting module directory! (Perhaps set MPG123_MODDIR?)");
		return NULL;
	}

	module = open_module_here(moddir, type, name, verbose);

	free(moddir);
	return module;
}

void close_module( mpg123_module_t* module, int verbose )
{
	compat_dlclose(module->handle);
}

int list_modules( const char *type, char ***names, char ***descr, int verbose
,	const char* bindir )
{
	char *moddir  = NULL;
	int count = 0;
	struct compat_dir *dir;
	char *filename;

	debug1("verbose:%i", verbose);

	*names = NULL;
	*descr = NULL;

	moddir = get_module_dir(verbose, bindir);
	if(moddir == NULL)
	{
		if(verbose > -1)
			error("Failure getting module directory! (Perhaps set MPG123_MODDIR?)");
		return -1;
	}
	debug1("module dir: %s", moddir);

	/* Open the module directory */
	dir = compat_diropen(moddir);
	if (dir==NULL) {
		if(verbose > -1)
			error2("Failed to open the module directory (%s): %s\n"
			,	moddir, strerror(errno));
		free(moddir);
		return -1;
	}

	while((filename=compat_nextfile(dir)))
	{
		/* Pointers to the pieces. */
		char *module_name = NULL;
		char *module_type = NULL;
		char *uscore_pos = NULL;
		mpg123_module_t *module = NULL;
		char* ext;
		size_t name_len;

		/* Various checks as loop shortcuts, avoiding too much nesting. */
		debug1("checking entry: %s", filename);

		name_len = strlen(filename);
		if(name_len < strlen(LT_MODULE_EXT))
			goto list_modules_continue;
		ext = filename
		+	name_len
		-	strlen(LT_MODULE_EXT);
		if(strcmp(ext, LT_MODULE_EXT))
			goto list_modules_continue;
		debug("has suffix");

		/* Extract the module type and name */
		uscore_pos = strchr( filename, '_' );
		if(   uscore_pos==NULL
		  || (uscore_pos>=filename+name_len+1) )
		{
			debug("no underscore");
			goto list_modules_continue;
		}
		*uscore_pos = '\0';
		module_type = filename;
		module_name = uscore_pos+1;
		/* Only list modules of desired type. */
		if(strcmp(type, module_type))
		{
			debug("wrong type");
			goto list_modules_continue;
		}
		debug("has type");

		/* Extract the short name of the module */
		name_len -= uscore_pos - filename + 1;
		if(name_len <= strlen(LT_MODULE_EXT))
		{
			debug("name too short");
			goto list_modules_continue;
		}
		name_len -= strlen(LT_MODULE_EXT);
		module_name[name_len] = '\0';

		debug("opening module");
		/* Open the module
		   Yes, this re-builds the file name we chopped to pieces just now. */
		if((module=open_module_here(moddir, module_type, module_name, verbose)))
		{
			if( stringlists_add( names, descr
			,	module->name, module->description, &count) )
				if(verbose > -1)
					error("OOM");
			/* Close the module again */
			close_module(module, verbose);
		}
list_modules_continue:
		free(filename);
	}
	compat_dirclose(dir);
	return count;
}


