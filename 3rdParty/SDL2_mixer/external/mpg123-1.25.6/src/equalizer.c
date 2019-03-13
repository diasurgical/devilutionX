/*
	equalizer: code for loading equalizer settings

	copyright 1995-2008 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Michael Hipp (exported to this file by Thomas Orgis)
*/

#include "mpg123app.h"

/* Load the settings from the path in the global variable equalfile.
   If there is no file, restore equalizer defaults. 
   If NO_EQUALIZER is defined, it does nothing else that return zero */
int load_equalizer(mpg123_handle *mh)
{
#ifndef NO_EQUALIZER
	if(equalfile != NULL)
	{ /* tst; ThOr: not TRUE or FALSE: allocated or not... */
		FILE *fe;
		int i;
		fe = fopen(equalfile,"r");
		if(fe) {
			char line[256];
			for(i=0;i<32;i++) {
				float e0 = 1.0;
				float e1 = 1.0; /* %f -> float! */
				do /* ignore comments */
				{
					line[0]=0;
					fgets(line,255,fe);
				}
				while(line[0]=='#');
				/* Hm, why not use fscanf? Comments... */
				sscanf(line,"%f %f",&e0,&e1);
				/* If scanning failed, we have default 1.0 value. */
				mpg123_eq(mh, MPG123_LEFT,  i, e0);
				mpg123_eq(mh, MPG123_RIGHT, i, e1);
			}
			fclose(fe);
		}
		else
		{
			fprintf(stderr,"Can't open equalizer file '%s'\n",equalfile);
			return -1;
		}
	}
	else mpg123_reset_eq(mh);
#endif
	return 0;
}
