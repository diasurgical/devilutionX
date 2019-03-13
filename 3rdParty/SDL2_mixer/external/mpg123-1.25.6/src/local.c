/*
	local: some stuff for localisation

	Currently, this is just about determining if we got UTF-8 locale.

	copyright 2008 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Thomas Orgis, based on a patch by Thorsten Glaser.
*/

#include "config.h"
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#ifdef HAVE_LANGINFO_H
#include <langinfo.h>
#endif
#include "compat.h"
#include "mpg123app.h"
#include "debug.h"

int utf8env = 0;

/* Check some language variable for UTF-8-ness. */
static int is_utf8(const char *lang);

void check_locale(void)
{
	if(param.force_utf8) utf8env = 1;
	else
	{
		const char *cp;

		/* Check for env vars in proper oder. */
		if((cp = getenv("LC_ALL")) == NULL && (cp = getenv("LC_CTYPE")) == NULL)
		cp = getenv("LANG");

		if(is_utf8(cp)) utf8env = 1;
	}

#if defined(HAVE_SETLOCALE) && defined(LC_CTYPE)
	/* To query, we need to set from environment... */
	if(!utf8env && is_utf8(setlocale(LC_CTYPE, ""))) utf8env = 1;
#endif
#if defined(HAVE_NL_LANGINFO) && defined(CODESET)
	/* ...langinfo works after we set a locale, eh? So it makes sense after setlocale, if only. */
	if(!utf8env && is_utf8(nl_langinfo(CODESET))) utf8env = 1;
#endif

	debug1("UTF-8 locale: %i", utf8env);
}

static int is_utf8(const char *lang)
{
	if(lang == NULL) return 0;

	/* Now, if the variable mentions UTF-8 anywhere, in some variation, the locale is UTF-8. */
	if(   strstr(lang, "UTF-8") || strstr(lang, "utf-8")
	   || strstr(lang, "UTF8")  || strstr(lang, "utf8")  )
	return 1;
	else
	return 0;
}
