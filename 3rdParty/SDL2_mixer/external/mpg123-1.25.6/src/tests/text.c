/*
	text: Test text transformations in libmpg123 (conversion to UTF-8).

	copyright 2009 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Thomas Orgis

	arguments: decoder testfile.mpeg
*/

#include <mpg123.h>
#include <compat.h>

#include "testtext.h"

int string_good(mpg123_string *sb)
{
	/* Let's accept additional null bytes... */
	if(sb->fill >= sizeof(utf8) && memcmp(utf8, sb->p, sizeof(utf8)) == 0
	   && (sb->fill <= sizeof(utf8) || sb->p[sizeof(utf8)] == 0) )
	return 1;
	else
	return 0;
}

int check_string(const char* name, enum mpg123_text_encoding enc, const unsigned char* source, size_t source_size)
{
	int ret = 0;
	mpg123_string sb;
	mpg123_init_string(&sb);
	printf("Conversion of %s: ", name);
	if(    mpg123_store_utf8(&sb, enc, source, source_size)
	    && string_good(&sb) )
	{
		printf("PASS\n");
		ret = 0;
	}
	else
	{
		printf("FAIL (%"SIZE_P" vs. %"SIZE_P")\n", (size_p)sb.fill, (size_p)sizeof(utf8));
		ret = 1;
	}

	mpg123_free_string(&sb);
	return ret;
}

/* We test: latin1, cp1252, utf8 and all of utf16be/le with and without BOM.
   Everything should succeed -- except utf16le without BOM. */
int main()
{
	int ret = 0;
	mpg123_string trans_utf16le;

	mpg123_init_string(&trans_utf16le);

	/* First, all conversions that should work. */
	ret += check_string("latin1", mpg123_text_latin1, latin1, sizeof(latin1));
	ret += check_string("cp1252", mpg123_text_cp1252, cp1252, sizeof(cp1252));
	ret += check_string("utf8",   mpg123_text_utf8,   utf8,   sizeof(utf8));
	ret += check_string("utf16bom_be", mpg123_text_utf16bom, utf16bom_be, sizeof(utf16bom_be));
	ret += check_string("utf16bom_le", mpg123_text_utf16,    utf16bom_le, sizeof(utf16bom_le));
	ret += check_string("utf16be",     mpg123_text_utf16be,  utf16be,     sizeof(utf16be));

	/* Now test the non-supported string. */
	printf("Let's see what happens with a non-BOM little endian UTF-16 string: ");
	mpg123_store_utf8(&trans_utf16le, mpg123_text_utf16, utf16le, sizeof(utf16le));
	if(string_good(&trans_utf16le))
	{
		++ret;
		printf("FAIL\n");
	}
	else printf("PASS\n");

	mpg123_free_string(&trans_utf16le);

	printf("\n%s\n", ret == 0 ? "PASS" : "FAIL");

	return ret;
}
