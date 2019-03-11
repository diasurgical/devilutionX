/*
	stringlists: creation of paired string lists for one-time consumption

	copyright 2015 by the mpg123 project
	free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Thomas Orgis

	Thomas did not want to introduce a list type complete with management
	functions just for returning driver module lists.
*/

#include "compat.h"

/* Construction helper for paired string lists.
   Returns 0 on success. */
int stringlists_add( char ***alist, char ***blist
                   , const char *atext, const char *btext, int *count)
{
	char *atextcopy = NULL;
	char *btextcopy = NULL;
	char **morealist = NULL;
	char **moreblist = NULL;

	/* If one of these succeeded, the old memory is gone, so always overwrite
	   the old pointer, worst case is wasted but not leaked memory in an
	   out-of-memory situation. */
	if((morealist = safe_realloc(*alist, sizeof(char*)*(*count+1))))
		*alist = morealist;
	if((moreblist = safe_realloc(*blist, sizeof(char*)*(*count+1))))
		*blist = moreblist;
	if(!morealist || !moreblist)
		return -1;

	if(
		(atextcopy = compat_strdup(atext))
	&&	(btextcopy = compat_strdup(btext))
	)
	{
		(*alist)[*count] = atextcopy;
		(*blist)[*count] = btextcopy;
		++*count;
		return 0;
	}
	else
	{
		free(atextcopy);
		return -1;
	}
}

