/*
	mpg123clr: MPEG Audio Decoder library Common Language Runtime version.

	copyright 2009-2011 by Malcolm Boczek - free software under the terms of the LGPL 2.1
	mpg123clr.dll is a derivative work of libmpg123 - all original mpg123 licensing terms apply.

	All rights to this work freely assigned to the mpg123 project.
*/
/*
	libmpg123: MPEG Audio Decoder library

	copyright 1995-2011 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org

*/
/*
	1.8.1.0	04-Aug-09	Initial release.
	1.9.0.0 24-Sep-09	Function names harmonized with libmpg123 (mb)
	1.9.0.0 01-Oct-09	Technical cleanup - subst nullptr for NULL (mb)
	1.13.0.0 13-Jan-11	release match - added strlen (mb)
*/

#include "StdAfx.h"
#include "string.h"

// Constructor for overlaid instance (instanced)
mpg123clr::mpg123str::mpg123str(void)
{
	instanced = true;
	sb = new ::mpg123_string;
	mpg123_init_string();

}

// Constructor for mpg123_string handle instance (referenced)
mpg123clr::mpg123str::mpg123str(mpg123_string* sb)
{
	instanced = false;
	this->sb = sb;
}

mpg123clr::mpg123str::mpg123str(const char* str)
{
	instanced = true;
	sb = new ::mpg123_string;
	mpg123_init_string();

	::mpg123_set_string(sb, str);
}

// Destructor cleans up all resources
mpg123clr::mpg123str::~mpg123str(void)
{
	// clean up code to release managed resources
	// ...

	// call Finalizer to clean up unmanaged resources
	this->!mpg123str();
}

// Finalizer cleans up unmanaged resources
mpg123clr::mpg123str::!mpg123str(void)
{
	if (instanced && (sb != nullptr)) mpg123_free_string();
}

int mpg123clr::mpg123str::mpg123_add_string(String ^ s)
{
	return mpg123_add_substring(s, 0, s->Length);
}

int mpg123clr::mpg123str::mpg123_add_substring(String ^ s, int from, int count)
{
	// convert CLR string to CLI string
	using namespace Runtime::InteropServices;
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s->Substring(from, count))).ToPointer();

	// add mpg123_string info
	int ret = ::mpg123_add_string(sb, chars);

	// free temporary memory
	Marshal::FreeHGlobal(IntPtr((void*)chars));

	return ret;
}

int mpg123clr::mpg123str::mpg123_copy_string(mpg123str^ to)
{
	return ::mpg123_copy_string(sb, to->sb);
}

void mpg123clr::mpg123str::mpg123_free_string()
{
	::mpg123_free_string(sb);
}

int mpg123clr::mpg123str::mpg123_grow_string(int newSize)
{
	return ::mpg123_grow_string(sb, newSize);
}

int mpg123clr::mpg123str::mpg123_resize_string(int newSize)
{
	return ::mpg123_resize_string(sb, newSize);
}

void mpg123clr::mpg123str::mpg123_init_string()
{
	::mpg123_init_string(sb);
}

int mpg123clr::mpg123str::mpg123_set_string(String ^ s)
{
	return mpg123_set_substring(s, 0, s->Length);
}

int mpg123clr::mpg123str::mpg123_set_substring(String ^ s, int from, int count)
{
	// convert CLR string to CLI string
	using namespace Runtime::InteropServices;
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s->Substring(from, count))).ToPointer();

	// set mpg123_string info
	int ret = ::mpg123_set_string(sb, chars);

	// free temporary memory
	Marshal::FreeHGlobal(IntPtr((void*)chars));

	return ret;
}

long long mpg123clr::mpg123str::mpg123_strlen(bool utf8)
{
	// TODO: determine use for utf8 vs ansi
	return ::mpg123_strlen(sb, utf8);
}

int mpg123clr::mpg123str::Fill::get()
{
	// WARN 4267 - clr limited to 32bit-length-size strings by PtrToStringAnsi
	return (int)sb->fill;
}

int mpg123clr::mpg123str::Size::get()
{
	// WARN 4267 - clr limited to 32bit-length-size strings by PtrToStringAnsi
	return (int)sb->size;
}

String^ mpg123clr::mpg123str::Text::get()
{
	if (sb->fill == 0) return gcnew String("");

	// WARN 4267 - clr limited to 32bit-length-size strings by PtrToStringAnsi
	return Marshal::PtrToStringAnsi((IntPtr)sb->p, (int)strnlen(sb->p, sb->fill));
}

mpg123clr::mpg123str::text_encoding mpg123clr::mpg123str::mpg123_enc_from_id3(unsigned char id3_enc_byte)
{
	return (mpg123clr::mpg123str::text_encoding) ::mpg123_enc_from_id3(id3_enc_byte);
}

int mpg123clr::mpg123str::mpg123_store_utf8(text_encoding enc, const unsigned char *source, size_t source_size)
{
	return ::mpg123_store_utf8(sb, (mpg123_text_encoding)enc, source, source_size);
}
