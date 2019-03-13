/*
	getlopt: command line option/parameter parsing

	copyright ?-2008 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written Oliver Fromme
	old timestamp: Tue Apr  8 07:15:13 MET DST 1997
*/

#include "config.h"
#include "compat.h"
#include "getlopt.h"
#include "debug.h"

int loptind = 1;	/* index in argv[] */
int loptchr = 0;	/* index in argv[loptind] */
char *loptarg;		/* points to argument if present, else to option */

topt *findopt (int islong, char *opt, topt *opts)
{
	if (!opts)
		return (0);
	while (opts->lname) {
		if (islong) {
			if (!strcmp(opts->lname, opt))
				return (opts);
		}
		else
			if (opts->sname == *opt)
				return (opts);
		opts++;
	}
	return (0);
}

static int performoption (int argc, char *argv[], topt *opt)
{
	int result = GLO_CONTINUE;
	/* this really is not supposed to happen, so the exit may be justified to create asap ficing pressure */
	#define prog_error() \
	{ \
		fprintf(stderr, __FILE__ ":%i Option without type flag! This is a programming error! Developer: fix this ASAP to regain your honor.\n", __LINE__); \
		exit(1); \
	}

	debug2("performoption on %c / %s"
	,	opt->sname ? opt->sname : '_', opt->lname ? opt->lname : "");
	if (!(opt->flags & GLO_ARG)) { /* doesn't take argument */
		if (opt->var) {
			if (opt->flags & GLO_CHAR) /* var is *char */
			{
				debug1("char at %p", opt->var);
				*((char *) opt->var) = (char) opt->value;\
			}
			else if(opt->flags & GLO_LONG)
			{
				debug1("long at %p", opt->var);
				*( (long *) opt->var ) = opt->value;
			}
			else if(opt->flags & GLO_INT)
			{
				debug1("int at %p", opt->var);
				*( (int *) opt->var ) = (int) opt->value;
			}
			/* GLO_DOUBLE is not supported here */
			else prog_error();
								
			debug("casting assignment done");
		}
#if 0 /* Oliver: What was this for?! --ThOr */
		else
			result = opt->value ? opt->value : opt->sname;
#endif
	}
	else { /* requires argument */
		debug("argument required");
		if (loptind >= argc)
			return (GLO_NOARG);
		loptarg = argv[loptind++]+loptchr;
		loptchr = 0;
		if (opt->var) {
			if (opt->flags & GLO_CHAR) /* var is *char */
				*((char **) opt->var) = compat_strdup(loptarg); /* valgrind claims lost memory here */
			else if(opt->flags & GLO_LONG)
				*((long *) opt->var) = atol(loptarg);
			else if(opt->flags & GLO_INT)
				*((int *) opt->var) = atoi(loptarg);
			else if(opt->flags & GLO_DOUBLE)
				*((double *) opt->var) = atof(loptarg);
			else prog_error();
		}
#if 0 /* Oliver: What was this for?! --ThOr */
		else
			result = opt->value ? opt->value : opt->sname;
#endif
	}
	if (opt->func)
		opt->func(loptarg);
	debug4("result: %i (%p, %li, %i)", result, opt->var, opt->value, opt->sname);
	return (result);
}

int getsingleopt (int argc, char *argv[], topt *opts)
{
	char *thisopt;
	topt *opt;
	static char shortopt[2] = {0, 0};

	if (loptind >= argc)
		return (GLO_END);
	thisopt = argv[loptind];
	debug1("getsingleopt: %s", thisopt);
	if (!loptchr) { /* start new option string */
		if (thisopt[0] != '-' || !thisopt[1]) /* no more options */
			return (GLO_END);
		if (thisopt[1] == '-') { /* "--" */
			if (thisopt[2]) { /* long option */
				loptarg = thisopt+2;
				loptind++;
				if (!(opt = findopt(1, thisopt+2, opts)))
					return (GLO_UNKNOWN);
				else
					return (performoption(argc, argv, opt));
			}
			else { /* "--" == end of options */
				loptind++;
				return (GLO_END);
			}
		}
		else /* start short option(s) */
			loptchr = 1;
	}
	shortopt[0] = thisopt[loptchr];
	loptarg = shortopt;
	opt = findopt(0, thisopt+(loptchr++), opts);
	if (!thisopt[loptchr]) {
		loptind++;
		loptchr = 0;
	}
	if (!opt)
		return (GLO_UNKNOWN);
	else
		return (performoption(argc, argv, opt));
}

int getlopt (int argc, char *argv[], topt *opts)
{
	
	int result;
	
	while ((result = getsingleopt(argc, argv, opts)) == GLO_CONTINUE);
	return (result);
}

/* EOF */
