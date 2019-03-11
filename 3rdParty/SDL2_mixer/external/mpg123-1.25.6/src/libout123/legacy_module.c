/*
	legacy_module.c: dummy interface to modular code loader for legacy build system

	copyright 2008 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Nicholas J Humfrey
*/

#include "out123_int.h"
#include "debug.h"

/* A single module is staticly compiled in for each type */
extern mpg123_module_t mpg123_output_module_info;
/* extern mpg123_module_t mpg123_input_module_info; */


/* Open a module */
mpg123_module_t*
open_module(const char* type, const char* name, int verbose, const char *bindir)
{
	mpg123_module_t *mod = NULL;
	
	/* Select the module info structure, based on the desired type */
	if (strcmp(type, "output")==0) {
		mod = &mpg123_output_module_info;
/*
	} else if (strcmp(type, "input")==0) {
		mod = &mpg123_input_module_info;
*/
	} else {
		if(verbose >= 0)
			error1("Unable to open module type '%s'.", type);
		return NULL;
	}
	
	/* Check the module compiled in is the module requested */
	if (strcmp(name, mod->name)!=0) {
		if(verbose >= 0)
		{
			error1("Unable to open requested module '%s'.", name);
			error1("The only available statically compiled module is '%s'."
			,	mod->name);
		}
		return NULL;
	}
	
	/* Debugging info */
	debug1("Details of static module type '%s':", type);
	debug1("  api_version=%d", mod->api_version);
	debug1("  name=%s", mod->name);
	debug1("  description=%s", mod->description);
	debug1("  revision=%s", mod->revision);
	debug1("  handle=%p", (void*)mod->handle);

	return mod;
}


void close_module(mpg123_module_t* module, int verbose)
{
	debug("close_module()");
	
	/* Module was never really 'loaded', so nothing to do here. */
}


int list_modules(const char *type, char ***names, char ***descr, int verbose
,	const char *bindir)
{
	debug("list_modules()" );

	*names = NULL;
	*descr = NULL;

	if(
		(*names=malloc(sizeof(char*)))
	&&	!((*names)[0]=NULL) /* for safe cleanup */
	&&	((*names)[0]=compat_strdup(mpg123_output_module_info.name))
	&&	(*descr=malloc(sizeof(char*)))
	&&	!((*descr)[0]=NULL) /* for safe cleanup */
	&& ((*descr)[0]=compat_strdup(mpg123_output_module_info.description))
	)
		return 1;
	else
	{
		if(*names)
			free((*names)[0]);
		free(*names);
		if(*descr)
			free((*descr)[0]);
		free(*descr);
		return -1;
	}
}


